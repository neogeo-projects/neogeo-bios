#include <string.h>

#include "bios.h"
#include "utils.h"

#include "backup_ram.h"

// Mimics the original backup-reset code
void reset_backup_ram() {
    unlock_backup_ram();

    // Reset backup ram to 0x0000
    volatile uint16_t *address = BACKUP_RAM;
    volatile uint16_t *end = address + 0x8000;  // 0x8000 words = 0x10000 bytes
    while (address < end) {
        *WATCHDOG = 0;
        *address = 0x0000;
        address++;
    }

    // Write backup-string
    for (uint8_t i = 0; i < BRAM_SIGNATURE_SIZE; i++) {
        BRAM_SIGNATURE_STR[i] = _bram_signature_str[i];
    }

    // Default cab settings
    *BRAM_DIP_COIN_P1_NEED = 0x01;
    *BRAM_DIP_COIN_P1_GIVE = 0x01;
    *BRAM_DIP_COIN_P2_NEED = 0x01;
    *BRAM_DIP_COIN_P2_GIVE = 0x02;
    *BRAM_DIP_0x3E = 0x01;
    *BRAM_DIP_0x3F = 0x01;
    *BRAM_DIP_0x40 = 0x01;
    *BRAM_DIP_0x41 = 0x03;
    *BRAM_DIP_GAME_SELECT_FREE = 0x00;
    *BRAM_DIP_GAME_START_FORCE = 0x00;
    *BRAM_DIP_COMPULSION_TIMER = to_bcd8(30);
    *BRAM_DIP_COMPULSION_FRAME_TIMER = 0x3B;
    *BRAM_DIP_DEMO_SOUND = 0x00;
  
    // Reset saved slot data
    for (uint8_t i = 0; i < 8; i++) {
        BRAM_NGH_BLOCK[i].ngh = 0x0000;
        BRAM_NGH_BLOCK[i].block = 0xFFFF;
    }

    lock_backup_ram();
}

// Writes additional values into backup-ram. Not part of the reset, but values that are added later on.
// This function locks backup ram
void setup_backup_ram() {
    unlock_backup_ram();

    *BRAM_PLAY_SECONDS_HEX = 0;
    *BRAM_COIN_STATUS_PREVIOUS = 0;
    *BRAM_PLAY_FRAME_TIMER = *BRAM_DIP_COMPULSION_FRAME_TIMER;

    // Zero out credits
    // TODO: Maybe save credit if restarted?
    volatile uint8_t *address = (volatile uint8_t *)BRAM_COIN_COUNTER_INTERNAL;
    for (uint8_t i = 0; i < 22; i++) {
        *WATCHDOG = 0;
        *address++ = 0x00;
    }

    // TODO: Read calendar from RTC
    memcpy((void *)BIOS_CLOCK_PTR, (void *)BRAM_CLOCK_PTR, 4);
    memcpy((void *)BIOS_DATE_PTR, (void *)BRAM_DATE_PTR, 4);

    // Update the MVS slot count
    *BRAM_SLOT_COUNT = 2;
    if (((*REG_SYSTYPE) & 0x40) != 0) {
        if(((*REG_STATUS_A) & 0x20) == 0) {
            *BRAM_SLOT_COUNT = 4;
        } else {
            *BRAM_SLOT_COUNT = 6;
        }
    }

    // Reset temporary NGH-memory
    for (uint8_t slot = 0; slot < MAX_SLOTS; slot++) {
        *WATCHDOG = 0;
        BIOS_NGH_BLOCK[slot] = (SlotEntry){
            .ngh = 0x0000,
            .block = 0xFFFF,
        };
    }

    // Find first working slot
    *BRAM_FIRST_PLAYABLE_SLOT = 0xFF;
    *REG_SWPROM = 0;

    for (uint8_t slot = 0; slot < *BRAM_SLOT_COUNT; slot++) {
        *WATCHDOG = 0;
        *REG_SLOT = slot;
        if (*ROM_NGH_NUMBER != 0xFFFF) {
            if (0xFF == *BRAM_FIRST_PLAYABLE_SLOT) {
                *BRAM_FIRST_PLAYABLE_SLOT = slot;
            }

            // Original BIOS check for these strings. Will ignore it for now
            // "NEO-GEO"
            // "NEO-CLN"
            // 4E 00 4F 00 47 00 4F (N \0 O \0 G \0 O)

            if (!validate_security_code()) {
                continue;
            }

            BIOS_NGH_BLOCK[slot] = (SlotEntry){
                .ngh = *ROM_NGH_NUMBER,
                .block = 0xFFFF,
            };
        }
    }

    // Check for duplicate games
    for (uint8_t i = 0; i + 1 < *BRAM_SLOT_COUNT; i++) {
        *WATCHDOG = 0;
        uint16_t id = BIOS_NGH_BLOCK[i].ngh;
        if (id == 0) {
            continue;
        }
        for (uint8_t j = i + 1; j < MAX_SLOTS; j++) {
            if (BIOS_NGH_BLOCK[j].ngh == id) {
                BIOS_NGH_BLOCK[j].ngh = 0;
            }
        }
    }

    // Save 'orphan' games into a temporary slot at D00164
    volatile OrphanBookEntry* orphans  = ORPHAN_BOOKS;

    // Reset data before populating it
    for (uint8_t i = 0; i < MAX_SLOTS; i++) {
        *WATCHDOG = 0;
        orphans[i].ngh = 0x0000;
        orphans[i].block = 0xFFFF;
        orphans[i].created = 0;
    }

    uint8_t block_id = 0;
 
    // Loop all BRAM slots and check if those games exist in active BIOS slots
    for (uint8_t i = 0; i < MAX_SLOTS; i++) {
        *WATCHDOG = 0;
        uint16_t ngh = BRAM_NGH_BLOCK[i].ngh;
        if (ngh == 0x0000) {
            continue;
        }

        uint8_t found = 0;
        for (uint8_t s = 0; s < *BRAM_SLOT_COUNT; s++) {
            if (BIOS_NGH_BLOCK[s].ngh == ngh) {
                found = 1;
                break;
            }
        }

        if (found) {
            continue;
        }

        if (block_id >= MAX_SLOTS) {
            break;
        }

        orphans[block_id].ngh = BRAM_NGH_BLOCK[i].ngh;
        orphans[block_id].block = BRAM_NGH_BLOCK[i].block;
        orphans[block_id].created = *BRAM_SLOT_BOOKKEEP_DATE(i);
        block_id++;
    }

    // Find already assigned blocks for current games and save it to BIOS slots
    for (uint8_t s = 0; s < *BRAM_SLOT_COUNT; s++) {
        *WATCHDOG = 0;
        uint16_t ngh = BIOS_NGH_BLOCK[s].ngh;
        if (ngh == 0x0000) {
            continue;
        }

        uint8_t i;
        for (i = 0; i < MAX_SLOTS; i++) {
            if (BRAM_NGH_BLOCK[i].ngh == ngh) {
                break;
            }
        }

        if (i == MAX_SLOTS) {
            continue;
        }

        BIOS_NGH_BLOCK[s].block = BRAM_NGH_BLOCK[i].block;
        *BIOS_SLOT_BOOKKEEP_DATE(s) = *BRAM_SLOT_BOOKKEEP_DATE(i);
    }

    // Insert BIOS slots data to BRAM
    for (uint8_t i = 0; i < *BRAM_SLOT_COUNT; i++) {
        *WATCHDOG = 0;
        BRAM_NGH_BLOCK[i].ngh = BIOS_NGH_BLOCK[i].ngh;
        BRAM_NGH_BLOCK[i].block = BIOS_NGH_BLOCK[i].block;

        *BRAM_SLOT_BOOKKEEP_DATE(i) = *BIOS_SLOT_BOOKKEEP_DATE(i);
    }

    // Append "orphan" games, first come, first served
    for (uint8_t i = *BRAM_SLOT_COUNT; i < MAX_SLOTS; i++) {
        *WATCHDOG = 0;
        BRAM_NGH_BLOCK[i].ngh = orphans->ngh;
        BRAM_NGH_BLOCK[i].block = orphans->block;
        *BRAM_SLOT_BOOKKEEP_DATE(i) = orphans->created;
        orphans++;
    }

    *REG_SWPBIOS = 0;
    
    lock_backup_ram();
}

void lock_backup_ram() {
    // TODO: Investigate what this is actually for.
    if (*BIOS_BRAM_USED) {
        *BIOS_BRAM_USED = 0;
    }
    *BACKUP_RAM_LOCK = 0;
}

void unlock_backup_ram() {
    *BIOS_BRAM_USED = 1;
    *BACKUP_RAM_UNLOCK = 0;
}

// Loads game data from workram. Tries to reserve a block if none is assigned yet.
// This function locks backup ram
void load_game_data() {
    unlock_backup_ram();
    *REG_SWPROM = 0;

    uint16_t rom_ngh = *ROM_NGH_NUMBER;
    uint16_t block = find_game_data_block(rom_ngh);

    // Check if game slot is unassigned
    if (0xFFFF == block) {
        goto _load_game_data_end;
    }

    // Load dip settings
    memcpy((void*)BIOS_GAME_DIP_PTR, (void*)BRAM_GAME_DIP_SETTINGS(block), 16);

    // Load game block
    volatile uint16_t size = *ROM_GAME_SAVE_SIZE;
    if (size > BRAM_GAME_BLOCK_SIZE) {
        size = BRAM_GAME_BLOCK_SIZE;
    }
    memcpy((void*)*ROM_BACKUP_RAM_PTR, (void*)BRAM_GAME_BLOCK(block), size);

_load_game_data_end:
    *REG_SWPBIOS = 0;
    lock_backup_ram();
}

// Saves game data from workram
void save_game_data() {
    unlock_backup_ram();
    // TODO: Sort our a better way to not fight with bram-lock and vblank routine
    __asm__ volatile ("move #0x2700, %sr");     // Disable interrupt
    *REG_SWPROM = 0;
    *BIOS_SWPMODE = 0x00;

    uint16_t rom_ngh = *ROM_NGH_NUMBER;
    uint16_t block = find_game_data_block(rom_ngh);
    uint8_t new_block = 0;

    // Check if game slot is unassigned
    if (0xFFFF == block) {
        block = find_next_available_data_block();
        
        for (uint8_t bram_slot = 0; bram_slot < MAX_SLOTS; bram_slot++) {
            if (rom_ngh != BRAM_NGH_BLOCK[bram_slot].ngh) {
                continue;
            }
            BRAM_NGH_BLOCK[bram_slot].block = block;
            reset_game_data(block);
            *BRAM_SLOT_BOOKKEEP_DATE(bram_slot) = *(volatile uint32_t *)BRAM_DATE_PTR;
            new_block = 1;
            break;
        }
    }

    // Make sure not to overwrite existing block data
    if (0 == *BIOS_SYSRET_STATUS && 0 == new_block) {
        goto _save_game_data_end;
    }

    // Copy metadata
    uint8_t region = *SROM_COUNTRY_CODE;
    volatile uint32_t dips_addr = ROM_SOFT_DIP_TABLE[region];
    volatile uint8_t *dips = (volatile uint8_t *)dips_addr;

    // Copy the game name to backup ram block
    for (uint8_t i = 0; i < 16; i++) {
        BRAM_GAME_NAME(block)[i] = *dips;
        dips++;
    }

    // Copy timed/counter options
    for (uint8_t i = 0; i < 6; i++) {
        BRAM_GAME_DIP_SETTINGS(block)[i] = *dips;
        dips++;
    }
    // Copy default values for all 10 potential soft dips.
    for (uint8_t i = 6; i < 16; i++) {
        BRAM_GAME_DIP_SETTINGS(block)[i] = *dips >> 4;
        dips++;
    }

    // Copy game data into backup
    volatile uint8_t *bram_game_data = BRAM_GAME_BLOCK(block);
    volatile uint8_t *rom_game_data = (volatile uint8_t *) *ROM_BACKUP_RAM_PTR;

    volatile uint16_t size = *ROM_GAME_SAVE_SIZE;
    if (size > 0x1000) {
        size = (uint16_t)BRAM_GAME_BLOCK_SIZE;
    }

    memcpy((void*)BRAM_GAME_BLOCK(block), (void*)*ROM_BACKUP_RAM_PTR, size);

_save_game_data_end:
    *REG_SWPBIOS = 0;
    *BIOS_SWPMODE = 0xFF;
    __asm__ volatile ("move #0x2000, %sr");     // Enable interrupt
    lock_backup_ram();
}

void reset_game_data(uint16_t block) {
    // Reset book date
    memset((void *)BRAM_SLOT_BOOKKEEP_DATE(block), 0, 4);
    // Reset soft dips
    memset((void *)BRAM_GAME_DIP_SETTINGS(block), 0, 16);
    // Reset game name
    memset((void *)BRAM_GAME_NAME(block), 0, 16);
    // Reset game data
    memset((void *)BRAM_GAME_BLOCK(block), 0, BRAM_GAME_BLOCK_SIZE);
}

uint16_t find_game_data_block(uint16_t rom_ngh) {
    uint16_t block = 0xFFFF;
    for (uint16_t i = 0; i < BRAM_MAX_SLOTS; i++) {
        if (BRAM_NGH_BLOCK[i].ngh == rom_ngh) {
            block = BRAM_NGH_BLOCK[i].block;
            break;
        }
    }
    return block;
}

uint16_t find_game_data_block_by_name() {
    uint16_t block = 0xFF;
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t region = *SROM_COUNTRY_CODE;
        uint32_t dips_addr = ROM_SOFT_DIP_TABLE[region];
        volatile uint8_t *cartridge_game_name = (volatile uint8_t *)(uint32_t)dips_addr;

        volatile uint8_t *block_game_name = BRAM_GAME_NAME(i);
        uint8_t match = 1;
        for (uint8_t c = 0; c < 0x10; c++) {
            if (cartridge_game_name[c] != block_game_name[c]) {
                match = 0;
            }
        }
        if (match == 1) {
            block = i;
            break;
        }
    }
    return block;
}

// Finds the next available data block based on current used BRAM_NGH_BLOCK-data.
uint16_t find_next_available_data_block() {
    for (uint16_t candidate = 0; candidate < MAX_SLOTS; candidate++) {
        *WATCHDOG = 0;
        uint8_t used = 0;
        for (uint8_t i = 0; i < MAX_SLOTS; i++) {
            uint16_t block = BRAM_NGH_BLOCK[i].block;
            if (block == 0xFFFF) {
                continue;
            }
            if ((block & 0x00FF) == (candidate & 0x00FF)) {
                used = 1;
                break;
            }
        }
        if (!used) {
            return candidate;
        }
    }
    return 0xFFFF;
}