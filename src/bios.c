#include "backup_ram.h"
#include "bios_calls.h"
#include "bios_test.h"
#include "eyecatcher.h"
#include "graphic.h"
#include "how_to_play.h"
#include "mess_out.h"
#include "service.h"
#include "service_dips.h"
#include "service_hardware.h"
#include "utils.h"

#include "bios.h"

__attribute__((used, section(".text._start")))
void _start() {
    __asm__ volatile ("move #0x2700, %sr");  // Disable interrupts
    *WATCHDOG = 0;
    *IRQ_ACK = 0x07;
    *LSPCMODE = 0x4000;

    *REG_BRDFIX = 0;
    *REG_SWPBIOS = 0;
    *REG_RESETCL1 = 0;
    *REG_RESETCL2 = 0;

    test_led();
    test_work_ram();
#if defined(SYSTEM_MVS)    
    test_backup_ram();
#endif    
    test_palette_ram();
    test_video_ram();

    *BIOS_SWPMODE = 0xFF;

    // Test work RAM in an 'infinate' loop if all dips are active
    // Note: Should not depend on any defined variables laying in work ram.
    if (0 == *REG_DIPSW) {
        reset_fix_layer();
        reset_palettes();

        *(uint32_t *)0x400002 = 0x0EEE0000;     // Palette 0
        *BIOS_MESS_BUSY = 1;
        *(volatile uint32_t *) BIOS_MESS_BUFFER = (uint32_t)work_ram_test;
        *BIOS_MESS_POINT = BIOS_MESS_BUFFER_PTR + sizeof(uint32_t);
        *BIOS_MESS_BUSY = 0;
        mess_out();

        while (0 == *REG_DIPSW) {
            test_work_ram();
#if defined(SYSTEM_MVS)
            test_backup_ram();                  // Original BIOS also test/reset Backup-RAM
#endif
        }
        reset_backup_ram();
    }

    test_rtc();
    test_bios_checksum();
    test_sound();
    test_memory_card();

    // 0x00 = AES, 0x80 = MVS
    if (*SROM_MVS_FLAG == 0x80) {
        *BIOS_MVS_FLAG = 0x01;
    } else {
        *BIOS_MVS_FLAG = 0x00;
    }
    
    *BIOS_COUNTRY_CODE = *SROM_COUNTRY_CODE;

    setup_backup_ram();
    controller_setup();

    *IRQ_ACK = 0x07;                            // Ack all interrupts
    __asm__ volatile ("move #0x2000, %sr");     // Set interrupt mask to level 2

    *BIOS_SYSRET_STATUS = 0;
 
    unlock_backup_ram();
    *BRAM_SLOT_CURSOR = 8;
    if (0xFF != *BRAM_FIRST_PLAYABLE_SLOT) {
        *BRAM_SLOT_CURSOR = *BRAM_FIRST_PLAYABLE_SLOT;
        lock_backup_ram();        
        enable_slot(*BRAM_FIRST_PLAYABLE_SLOT);
        *BIOS_SWPMODE = 0;
        *BIOS_USER_REQUEST = USER_REQUEST_INIT;
        SUBR_CART_USER();
    }
    lock_backup_ram();
    system_return();
};

void start_game() {
    *BIOS_PLAYER_MOD1 = 0x00;
    *BIOS_PLAYER_MOD2 = 0x00;
    *BIOS_PLAYER_MOD3 = 0x00;
    *BIOS_PLAYER_MOD4 = 0x00;

    *IRQ_ACK = 0x07;                            // Acknowledge all interrupt
    *BIOS_SYSTEM_MODE &= 0x7F;                  // Make sure BIOS vblank interrupt is used
    __asm__ volatile ("move #0x2000, %sr");     // Enable interrupt
    *REG_SOUND = 0x03;                          // Reset z80
    wait_for_vblank();
    __asm__ volatile ("move #0x2700, %sr");     // Disable interrupt
    *BIOS_SYSTEM_MODE |= 0x80;                  // Return vblank interrupt to game

    lock_backup_ram();

    *LSPCMODE = 0x4000;
    *REG_SWPROM = 0;
    *REG_CRTFIX = 0;
    *BIOS_SWPMODE = 0;

    __asm__ volatile ("move.l #0x10F300, %sp");
    __asm__ volatile ("jmp 0x000122.w");            // 0x122 = User routine
}

void set_default_values() {
    *BIOS_INT1_SKIP = 0;        // Only used to skip input when checking for RTC-pulse with interrupts enabled?
}

void enable_slot(uint8_t slot) {
    wait_for_z80();
    unlock_backup_ram();
    *BRAM_SLOT_SELECTED = slot;
    lock_backup_ram();
    *REG_SLOT = slot;
    *REG_CRTFIX = 0;
    *REG_SWPROM = 0;
}

void change_slot_incremental() {
    uint8_t count = *BRAM_SLOT_COUNT;
    if (count == 0 || count > BRAM_MAX_SLOTS) {
        return;
    }
    uint8_t slot = *BRAM_SLOT_SELECTED + 1;
    if (slot >= count) {
        slot = 0;
    }
    for (uint8_t i = 0; i < count; i++) {
        if (BIOS_NGH_BLOCK[slot].ngh != 0x0000) {
            enable_slot(slot);
            return;
        }
        slot++;
        if (slot >= count) {
            slot = 0;
        }
    }
}

void change_slot_decremental() {
    uint8_t count = *BRAM_SLOT_COUNT;
    if (count == 0 || count > BRAM_MAX_SLOTS) {
        return;
    }
    uint8_t slot = *BRAM_SLOT_SELECTED;
    if (slot == 0) {
        slot = count - 1;
    } else {
        slot--;
    }
    for (uint8_t i = 0; i < count; i++) {
        if (BIOS_NGH_BLOCK[slot].ngh != 0x0000) {
            enable_slot(slot);
            return;
        }
        if (slot == 0) {
            slot = count - 1;
        } else {
            slot--;
        }
    }
}

uint8_t validate_security_code() {
    volatile const uint8_t *rom_bytes = (volatile const uint8_t *)ROM_SECURITY_CODE_PTR;

    for (uint8_t i = 0; i < sizeof(security_bytes); i++) {
        if (rom_bytes[i] != security_bytes[i]) {
            return 0;
        }
    }

    return 1;
}