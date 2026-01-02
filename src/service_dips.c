#include <stdint.h>

#include "backup_ram.h"
#include "bios.h"
#include "mess_out.h"
#include "service.h"
#include "utils.h"

#include "service_dips.h"

void show_bios_menu_hard_dips() {
    *(uint32_t *)0x400002 = 0x0EEE0000; // Palette 0
    *BIOS_MESS_BUSY = 1;

    *(volatile uint32_t *) BIOS_MESS_BUFFER = (uint32_t)bios_menu_hard_dips;
    *BIOS_MESS_POINT = BIOS_MESS_BUFFER_PTR + sizeof(uint32_t);
    
    *BIOS_MESS_BUSY = 0;
}

void update_bios_menu_hard_dips() {
    *BIOS_MESS_BUSY = 1;

    volatile uint16_t *address = (volatile uint16_t *)*BIOS_MESS_POINT;
    *address++ = 0x0000;
    *address++ = 0x0000;

    *address++ = 0x2002;

    // Setting mode
    *address++ = 0x0003;
    *address++ = 0x724c;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x01) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x738c;
    *address++ = 0x0108;
    uint8_t i = *REG_DIPSW & 0x01 ? 1 : 0;
    for (uint8_t c = 0; bios_menu_on_off[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_on_off[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }

    // Controller
    *address++ = 0x0003;
    *address++ = 0x726e;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x02) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x738e;
    *address++ = 0x0108;
    i = *REG_DIPSW & 0x02 ? 1 : 0;
    for (uint8_t c = 0; bios_menu_on_off[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_on_off[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }   

    // Chute
    *address++ = 0x0003;
    *address++ = 0x7290;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x04) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x7390;
    *address++ = 0x0108;
    i = *REG_DIPSW & 0x04 ? 1 : 0;
    for (uint8_t c = 0; bios_menu_on_off[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_on_off[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }        

    // Communication - Dip 1
    *address++ = 0x0003;
    *address++ = 0x72b2;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x08) ? 0x30FF : 0x31FF;
    
    // Communication - Dip 2
    *address++ = 0x0003;
    *address++ = 0x72d2;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x10) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x7412;
    *address++ = 0x0108;
    i = (~(*REG_DIPSW >> 3)) & 0x03;
    for (uint8_t c = 0; bios_menu_communication_link[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_communication_link[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }

    // Communication - Dip 3
    *address++ = 0x0003;
    *address++ = 0x72f2;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x20) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x7392;
    *address++ = 0x0108;
    i = (*REG_DIPSW & 0x20) ? 1 : 0;
    for (uint8_t c = 0; bios_menu_on_off[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_on_off[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }

    // Free play
    *address++ = 0x0003;
    *address++ = 0x7314;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x40) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x7394;
    *address++ = 0x0108;
    i = *REG_DIPSW & 0x40 ? 1 : 0;
    for (uint8_t c = 0; bios_menu_on_off[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_on_off[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }

    // Stop mode / Freeze
    *address++ = 0x0003;
    *address++ = 0x7336;
    *address++ = 0x0108;
    *address++ = (*REG_DIPSW & 0x80) ? 0x30FF : 0x31FF;
    *address++ = 0x0003;
    *address++ = 0x7396;
    *address++ = 0x0108;
    i = *REG_DIPSW & 0x80 ? 1 : 0;
    for (uint8_t c = 0; bios_menu_on_off[i][c] != '\0'; c++) {
        *(volatile uint8_t *)address = (bios_menu_on_off[i][c]);
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    if (((uint32_t)address & 1) != 0 ) {
        *(volatile uint8_t *)address = 0xFF;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    } else {
        *address++ = 0xFFFF;
    }

    // End
    *address++ = 0x0000;
    *BIOS_MESS_POINT = (volatile uint32_t)address;

    *BIOS_MESS_BUSY = 0;
}

void show_bios_menu_soft_dips() {
    *SERVICE_CURSOR_MAX = 1;            // 1 because there are games + cabinet settings
    *(uint32_t *)0x400002 = 0x0EEE0000; // Palette 0
    *(uint32_t *)0x400022 = 0x0E000000; // Palette 1
    *BIOS_MESS_BUSY = 1;

    *(volatile uint32_t *) BIOS_MESS_BUFFER = (uint32_t)bios_menu_soft_dips;
    *BIOS_MESS_POINT = BIOS_MESS_BUFFER_PTR + sizeof(uint32_t);

    volatile uint16_t *address = (volatile uint16_t *)*BIOS_MESS_POINT;
    *address++ = 0x0000;
    *address++ = 0x0000;

    for (uint8_t i = 0; i < *BRAM_SLOT_COUNT; i++) {
        *address++ = 0x0005;
        *address++ = 0x0002;
        *address++ = 0x0108;
        *address++ = 0x534c;
        *address++ = 0x4f54;
        *address++ = 0x2031 + i;
        *address++ = 0x3A20;

        uint16_t ngh = BRAM_NGH_BLOCK[i].ngh;
        uint16_t block_id = BRAM_NGH_BLOCK[i].block;
        if (ngh != 0x0000) {
            volatile uint8_t *game_name = BRAM_GAME_NAME(block_id);
            for (uint8_t c = 0; c < 0x10; c++) {
                *(volatile uint8_t *)address = game_name[c];
                address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
            }
            *address++ = 0xFFFF;
        } else {
            *address++ = 0x4E2F;
            *address++ = 0x41FF;
        }
        *SERVICE_CURSOR_MAX += 1;
    }

    *address++ = 0x0000;
    *BIOS_MESS_POINT = (volatile uint32_t)address;

    *BIOS_MESS_BUSY = 0;
}

void update_bios_menu_soft_dips() {
    *BIOS_MESS_BUSY = 1;
    uint8_t menu_items = *SERVICE_CURSOR_MAX;
    _move_cursor(menu_items, 0, 0);

    volatile uint16_t *address = (volatile uint16_t *)*BIOS_MESS_POINT;
    *address++ = 0x0000;
    *address++ = 0x0000;

    *address++ = 0x0003;
    *address++ = 0x70E5;

    for (int8_t i = 0; i < menu_items; i++) {
        if (i != *SERVICE_CURSOR) {
            *address++ = 0x0108;
            *address++ = 0x20FF;  // Space (overwrite arrow)
        } else {
            *address++ = 0x1108;
            *address++ = 0x11FF;  // Arrow
        }
        if (menu_items - 1 != i) {
            *address++ = 0x0005;
            *address++ = 0x0002;
        }
    }

    *address++ = 0x0000;
    *BIOS_MESS_POINT = (volatile uint32_t)address;

    *BIOS_MESS_BUSY = 0;
}

void show_bios_menu_soft_dips_cabinet() {
    *(uint32_t *)0x400002 = 0x0EEE0000; // Palette 0
    *(uint32_t *)0x400022 = 0x0E000000; // Palette 1
    *BIOS_MESS_BUSY = 1;

    *(volatile uint32_t *) BIOS_MESS_BUFFER = (uint32_t)bios_menu_soft_dips_cabinet;
    *BIOS_MESS_POINT = BIOS_MESS_BUFFER_PTR + sizeof(uint32_t);

    *BIOS_MESS_BUSY = 0;
}

void update_bios_menu_soft_dips_cabinet() {
    *BIOS_MESS_BUSY = 1;
    uint8_t menu_items = 5;
    // TODO: Make the side_items-value dynamic. d = [1 2 1 2 2]; d[*SERVICE_CURSOR]
    _move_cursor(menu_items, 1, 0);

    volatile uint16_t *address = (volatile uint16_t *)*BIOS_MESS_POINT;
    *address++ = 0x0000;
    *address++ = 0x0000;

    *address++ = 0x0003;
    *address++ = 0x7068;

    for (int8_t i = 0; i < menu_items; i++) {
        if (i != *SERVICE_CURSOR) {
            *address++ = 0x0108;
            *address++ = 0x20FF;  // Space (overwrite arrow)
        } else {
            *address++ = 0x1108;
            *address++ = 0x11FF;  // Arrow
        }
        if (menu_items - 1 != i) {
            *address++ = 0x0005;
            *address++ = 0x0002;
        }
    }

    // If 'A' button is pressed
    if ((*BIOS_P1CHANGE & A_BUTTON) != 0) {
        unlock_backup_ram();
        if (0 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 0) {
            if (*BRAM_DIP_COIN_P1_NEED >= 9) {
                *BRAM_DIP_COIN_P1_NEED = 1;
            } else {
                *BRAM_DIP_COIN_P1_NEED += 1;
            }
        } else if (0 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 1) {
            if (*BRAM_DIP_COIN_P1_GIVE >= 9) {
                *BRAM_DIP_COIN_P1_GIVE = 1;
            } else {
                *BRAM_DIP_COIN_P1_GIVE += 1;
            }
        } else if (1 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 0) {
            if (*BRAM_DIP_COIN_P2_NEED >= 9) {
                *BRAM_DIP_COIN_P2_NEED = 1;
            } else {
                *BRAM_DIP_COIN_P2_NEED += 1;
            }
        } else if (1 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 1) {
            if (*BRAM_DIP_COIN_P2_GIVE >= 9) {
                *BRAM_DIP_COIN_P2_GIVE = 1;
            } else {
                *BRAM_DIP_COIN_P2_GIVE += 1;
            }
        } else if (2 == *SERVICE_CURSOR) {
            *BRAM_DIP_GAME_SELECT_FREE ^= 1;
        } else if (3 == *SERVICE_CURSOR) {
            uint8_t seconds = from_bcd8(*BRAM_DIP_COMPULSION_TIMER);
            if (seconds >= 60) {
                seconds = 0;
                *BRAM_DIP_GAME_START_FORCE ^= 1;
            } else {
                if (seconds == 0) {
                    *BRAM_DIP_GAME_START_FORCE ^= 1;
                }
                seconds++;
            }
            *BRAM_DIP_COMPULSION_TIMER = to_bcd8(seconds);
        } else if (4 == *SERVICE_CURSOR) {
            *BRAM_DIP_DEMO_SOUND ^= 1;
        }
        lock_backup_ram();
    // If 'B' button is pressed
    } else if ((*BIOS_P1CHANGE & B_BUTTON) != 0) {
        unlock_backup_ram();
        if (0 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 0) {
            if (*BRAM_DIP_COIN_P1_NEED <= 1) {
                *BRAM_DIP_COIN_P1_NEED = 9;
            } else {
                *BRAM_DIP_COIN_P1_NEED -= 1;
            }
        } else if (0 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 1) {
            if (*BRAM_DIP_COIN_P1_GIVE <= 1) {
                *BRAM_DIP_COIN_P1_GIVE = 9;
            } else {
                *BRAM_DIP_COIN_P1_GIVE -= 1;
            }
        } else if (1 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 0) {
            if (*BRAM_DIP_COIN_P2_NEED <= 1) {
                *BRAM_DIP_COIN_P2_NEED = 9;
            } else {
                *BRAM_DIP_COIN_P2_NEED -= 1;
            }
        } else if (1 == *SERVICE_CURSOR && *SERVICE_CURSOR_SIDEWAYS == 1) {
            if (*BRAM_DIP_COIN_P2_GIVE <= 1) {
                *BRAM_DIP_COIN_P2_GIVE = 9;
            } else {
                *BRAM_DIP_COIN_P2_GIVE -= 1;
            }
        } else if (2 == *SERVICE_CURSOR) {
            *BRAM_DIP_GAME_SELECT_FREE ^= 1;
        } else if (3 == *SERVICE_CURSOR) {
            uint8_t seconds = from_bcd8(*BRAM_DIP_COMPULSION_TIMER);
            if (seconds == 0) {
                seconds = 60;
                *BRAM_DIP_GAME_START_FORCE ^= 1;
            } else {
                if (seconds == 1) {
                    *BRAM_DIP_GAME_START_FORCE ^= 1;
                }
                seconds--;
            }
            *BRAM_DIP_COMPULSION_TIMER = to_bcd8(seconds);
        } else if (4 == *SERVICE_CURSOR) {
            *BRAM_DIP_DEMO_SOUND ^= 1;
        }
        lock_backup_ram();
    }

    // Coin1
    *address++ = 0x0003;
    *address++ = 0x7248;
    *address++ = ((*SERVICE_CURSOR == 0 && *SERVICE_CURSOR_SIDEWAYS == 0) ? 0x1108 : 0x0108);
    *address++ = ((uint16_t)(*BRAM_DIP_COIN_P1_NEED + 0x30) << 8) | 0x00FF;
    *address++ = 0x0003;
    *address++ = 0x7348;
    *address++ = ((*SERVICE_CURSOR == 0 && *SERVICE_CURSOR_SIDEWAYS == 1) ? 0x1108 : 0x0108);
    *address++ = ((uint16_t)(*BRAM_DIP_COIN_P1_GIVE + 0x30) << 8) | 0x00FF;
    // Pural 's'
    *address++ = 0x0003;
    *address++ = 0x72e8;
    *address++ = 0x0108;
    if (*BRAM_DIP_COIN_P1_NEED > 1) {
        *address++ = 0x53FF;
    } else {
        *address++ = 0x20FF;
    }
    *address++ = 0x0003;
    *address++ = 0x7428;
    *address++ = 0x0108;
    if (*BRAM_DIP_COIN_P1_GIVE > 1) {
        *address++ = 0x53FF;
    } else {
        *address++ = 0x20FF;
    }

    // Coin2
    *address++ = 0x0003;
    *address++ = 0x724b;
    *address++ = ((*SERVICE_CURSOR == 1 && *SERVICE_CURSOR_SIDEWAYS == 0) ? 0x1108 : 0x0108);
    *address++ = ((uint16_t)(*BRAM_DIP_COIN_P2_NEED + 0x30) << 8) | 0x00FF;
    *address++ = 0x0003;
    *address++ = 0x734b;
    *address++ = ((*SERVICE_CURSOR == 1 && *SERVICE_CURSOR_SIDEWAYS == 1) ? 0x1108 : 0x0108);
    *address++ = ((uint16_t)(*BRAM_DIP_COIN_P2_GIVE + 0x30) << 8) | 0x00FF;
    // Pural 's'
    *address++ = 0x0003;
    *address++ = 0x72eb;
    *address++ = 0x0108;
    if (*BRAM_DIP_COIN_P2_NEED > 1) {
        *address++ = 0x53FF;
    } else {
        *address++ = 0x20FF;
    }
    *address++ = 0x0003;
    *address++ = 0x742b;
    *address++ = 0x0108;
    if (*BRAM_DIP_COIN_P2_GIVE > 1) {
        *address++ = 0x53FF;
    } else {
        *address++ = 0x20FF;
    }

    // Game select
    address = _add_string(address, (*SERVICE_CURSOR == 2 ? 1 : 0), 0x724e, bios_menu_soft_dips_cabinet_game_select[*BRAM_DIP_GAME_SELECT_FREE]);

    // Game start (compulsion)
    address = _add_string(address, (*BRAM_DIP_GAME_START_FORCE == 1 ? 1 : 0), 0x7391, bios_menu_soft_dips_cabinet_game_start[*BRAM_DIP_GAME_START_FORCE]);
    if (0 == *BRAM_DIP_GAME_START_FORCE) {
        address = _add_large_char(address, (*SERVICE_CURSOR == 3 ? 1 : 0), 0x7391, *BRAM_DIP_COMPULSION_TIMER);
    }

    // Demo sound
    address = _add_string(address, (*SERVICE_CURSOR == 4 ? 1 : 0), 0x7254, bios_menu_soft_dips_cabinet_demo_sound[*BRAM_DIP_DEMO_SOUND]);

    *address++ = 0x0000;
    *BIOS_MESS_POINT = (volatile uint32_t)address;

    *BIOS_MESS_BUSY = 0;
}

void show_bios_menu_soft_dips_game() {
    *SERVICE_CURSOR_MAX = 0;
    *SERVICE_CURSOR_PAGE = 0;
    *(uint32_t *)0x400002 = 0x0EEE0000; // Palette 0
    *(uint32_t *)0x400022 = 0x0E000000; // Palette 1

    // Activate game slot so we can read software dips and description/values
    *REG_SWPROM = 0;
    *REG_SLOT = *SOFT_DIPS_GAME_SELECT;

    uint8_t region = *SROM_COUNTRY_CODE;
    volatile uint32_t dips_addr = ROM_SOFT_DIP_TABLE[region];
    // Original dip values from cartridge
    volatile uint8_t *dips = (volatile uint8_t *)dips_addr;
    dips += SOFT_DIPS_NAME_LEN;

    // Count all possible settings
    for (uint8_t i = 0; i < SOFT_DIPS_SPECIAL_COUNT; i++) {
        if (*(uint16_t *)dips != 0xFFFF) {
            *SERVICE_CURSOR_MAX += 1;
        }
        dips += 2;
    }
    for (uint8_t i = 0; i < SOFT_DIPS_SPECIAL_COUNT; i++) {
        if (*dips != 0xFF) {
            *SERVICE_CURSOR_MAX += 1;
        }
        dips++;
    }
    for (uint8_t i = 0; i < SOFT_DIPS_SIMPLE_COUNT; i++) {
        if (*dips != 0x00) {
            *SERVICE_CURSOR_MAX += 1;
        }
        dips++;
    }
    
    *REG_SWPBIOS = 0;
}

void update_bios_menu_soft_dips_game() {
    *BIOS_MESS_BUSY = 1;

    uint8_t start = *SERVICE_CURSOR_PAGE * SOFT_DIPS_PAGE_SIZE;
    uint8_t remaining = (start < *SERVICE_CURSOR_MAX) ? (*SERVICE_CURSOR_MAX - start) : 0;
    uint8_t menu_items = (remaining > SOFT_DIPS_PAGE_SIZE) ? SOFT_DIPS_PAGE_SIZE : remaining;

    uint8_t pages = pages_for_items(*SERVICE_CURSOR_MAX, SOFT_DIPS_PAGE_SIZE);
    *SERVICE_CURSOR_PAGE_PREV = *SERVICE_CURSOR_PAGE;
    _move_cursor(menu_items, 0, pages);

    if (*SERVICE_CURSOR_PAGE < *SERVICE_CURSOR_PAGE_PREV) {
        *SERVICE_CURSOR = SOFT_DIPS_PAGE_SIZE - 1;
    }
    if (*SERVICE_CURSOR_PAGE != *SERVICE_CURSOR_PAGE_PREV) {
        reset_fix_layer();
        return;
    }

    volatile uint16_t *address = (volatile uint16_t *)*BIOS_MESS_POINT;
    *address++ = 0x0000;
    *address++ = 0x0000;

    *address++ = 0x0003;
    *address++ = 0x70E6;

    for (int8_t i = 0; i < menu_items; i++) {
        if (i != *SERVICE_CURSOR) {
            *address++ = 0x0108;
            *address++ = 0x20FF;  // Space (overwrite arrow)
        } else {
            *address++ = 0x1108;
            *address++ = 0x11FF;  // Arrow
        }
        if (menu_items - 1 != i) {
            *address++ = 0x0005;
            *address++ = 0x0002;
        }
    }

    // Saved dip values from backup ram
    volatile uint8_t *chosen_dip_value = BRAM_GAME_DIP_SETTINGS(*SOFT_DIPS_GAME_SELECT);

    // If 'A' button is pressed
    if ((*BIOS_P1CHANGE & A_BUTTON) != 0) {
        _change_bram_dip(*SERVICE_CURSOR + (*SERVICE_CURSOR_PAGE * SOFT_DIPS_PAGE_SIZE), 1);
    } else if ((*BIOS_P1CHANGE & B_BUTTON) != 0) {
        _change_bram_dip(*SERVICE_CURSOR + (*SERVICE_CURSOR_PAGE * SOFT_DIPS_PAGE_SIZE), -1);
    }

    uint8_t menu_item_counter = 0;

    // Activate game slot so we can read software dips and description/values
    *REG_SWPROM = 0;
    *REG_SLOT = *SOFT_DIPS_GAME_SELECT;

    uint8_t region = *SROM_COUNTRY_CODE;
    volatile uint32_t dips_addr = ROM_SOFT_DIP_TABLE[region];
    // Original dip values from cartridge
    volatile uint8_t *dips = (volatile uint8_t *)dips_addr;

    *address++ = 0x0003;
    *address++ = 0x7143;
    *address++ = 0x0108;

    // Game name
    memcpy((void *)address, (void *)dips_addr, SOFT_DIPS_NAME_LEN);
    address = (volatile uint16_t *)((volatile uint8_t *)address + SOFT_DIPS_NAME_LEN);
    *address++ = 0xFFFF;

    // Special time settings (description)
    uint16_t description_offset = SOFT_DIPS_HEADER_LEN;
    for (uint8_t i = 0; i < SOFT_DIPS_SPECIAL_COUNT; i++) {
        uint16_t dip_value = *(uint16_t *)chosen_dip_value;
        chosen_dip_value += 2;
        if (dip_value == 0xFFFF) {
            continue;
        }

        if (menu_item_counter < start || menu_item_counter > start + SOFT_DIPS_PAGE_SIZE) {
            goto _special_time_settings_end;
        }
       
        *address++ = 0x0005;
        *address++ = 0x0002;
        *address++ = 0x0108;

        // Print the description
        memcpy((void *)address, (void *)(dips_addr + description_offset), SOFT_DIPS_DESC_LEN);
        address = (volatile uint16_t *)((volatile uint8_t *)address + SOFT_DIPS_DESC_LEN);

        *address++ = 0xFFFF;

_special_time_settings_end:
        description_offset += SOFT_DIPS_DESC_LEN;
        menu_item_counter += 1;
    }

    // Special count settings
    for (uint8_t i = 0; i < SOFT_DIPS_SPECIAL_COUNT; i++) {
        uint8_t dip_value = *chosen_dip_value++;
        if (dip_value == 0xFF) {
            continue;
        }

        if (menu_item_counter < start || menu_item_counter > start + SOFT_DIPS_PAGE_SIZE) {
            goto _special_count_settings_end;
        }

        *address++ = 0x0005;
        *address++ = 0x0002;
        *address++ = 0x0108;

        // Print the description
        memcpy((void *)address, (void *)(dips_addr + description_offset), SOFT_DIPS_DESC_LEN);
        address = (volatile uint16_t *)((volatile uint8_t *)address + SOFT_DIPS_DESC_LEN);

        // Add a few extra space
        *address++ = 0x2020;

        // Print the choosen value
        uint8_t value = to_bcd8(dip_value);
        uint8_t upper = (value >> 4) & 0x0F;
        uint8_t lower = value & 0x0F;
        *address++ = ((upper + 0x30) << 8) | (lower + 0x30);

        *address++ = 0xFFFF;

_special_count_settings_end:
        description_offset += SOFT_DIPS_DESC_LEN;
        menu_item_counter += 1;
    }

    // Offset the cartridge dips to start at the simple setting values
    dips += SOFT_DIPS_NAME_LEN + 6;

    // Simple settings
    for (uint8_t i = 0; i < SOFT_DIPS_SIMPLE_COUNT; i++) {
        uint8_t dip_value = *chosen_dip_value++;
        uint8_t default_dip_value = *dips >> 4;
        uint8_t choices = (*dips++) & 0x0F;
        if (choices == 0x00) {
            continue;
        }

        // Reset dip-value if it for some reason goes above the number of choices.
        if (dip_value >= choices) {
            dip_value = default_dip_value;
        }
        
        if (menu_item_counter < start || menu_item_counter > start + SOFT_DIPS_PAGE_SIZE) {
            goto _simple_settings_end;
        }

        *address++ = 0x0005;
        *address++ = 0x0002;
        *address++ = 0x0108;

        // Print the description
        memcpy((void *)address, (void *)(dips_addr + description_offset), SOFT_DIPS_DESC_LEN);
        address = (volatile uint16_t *)((volatile uint8_t *)address + SOFT_DIPS_DESC_LEN);

        // Add a few extra space
        *address++ = 0x2020;

        // Add an offset to the choosen dip setting
        uint16_t dips_offset = SOFT_DIPS_DESC_LEN;
        dips_offset += dip_value * SOFT_DIPS_DESC_LEN;

        // Print the choosen value
        memcpy((void *)address, (void *)(dips_addr + description_offset + dips_offset), SOFT_DIPS_DESC_LEN);
        address = (volatile uint16_t *)((volatile uint8_t *)address + SOFT_DIPS_DESC_LEN);
        *address++ = 0xFFFF;

_simple_settings_end:
        description_offset += SOFT_DIPS_DESC_LEN + (SOFT_DIPS_DESC_LEN * choices);
        menu_item_counter += 1;
    }

    *REG_SWPBIOS = 0;

    *address++ = 0x0000;
    *BIOS_MESS_POINT = (volatile uint32_t)address;

    *BIOS_MESS_BUSY = 0;
}

void _change_bram_dip(uint8_t item, int8_t direction) {
    *REG_SWPROM = 0;
    *REG_SLOT = *SOFT_DIPS_GAME_SELECT;

    volatile uint8_t *bram_dip_values = BRAM_GAME_DIP_SETTINGS(*SOFT_DIPS_GAME_SELECT);

    uint8_t region = *SROM_COUNTRY_CODE;
    volatile uint32_t dips_addr = ROM_SOFT_DIP_TABLE[region];
    // Original dip values from cartridge
    volatile uint8_t *dips = (volatile uint8_t *)dips_addr;
    dips += SOFT_DIPS_NAME_LEN;

    uint8_t offset = 0;
    uint8_t dip_count = 0;

    int8_t value = 0;

    // Count all possible settings
    for (uint8_t i = 0; i < SOFT_DIPS_SPECIAL_COUNT; i++) {
        if (*(uint16_t *)(dips + offset) != 0xFFFF) {
            dip_count++;
        }
        if ((dip_count - 1) != item) {
            offset += 2;
            continue;
        }
        goto _change_bram_dip;
    }
    for (uint8_t i = 0; i < SOFT_DIPS_SPECIAL_COUNT; i++) {
        if (*(dips + offset) != 0xFF) {
            dip_count++;
        }
        if ((dip_count - 1) != item) {
            offset++;
            continue;
        }
        value = *(bram_dip_values + offset);
        value += direction;
        if (value >= 100) {
            value = 1;
        } else if (value <= 0) {
            value = 99;             // saved value 'is' BCD (= max 99)
        }
        goto _change_bram_dip;
    }
    for (uint8_t i = 0; i < SOFT_DIPS_SIMPLE_COUNT; i++) {
        if (*(dips + offset) != 0x00) {
            dip_count++;
        }
        if ((dip_count - 1) != item) {
            offset++;
            continue;
        }
        uint8_t choices = (*(dips + offset)) & 0x0F;
        value = *(bram_dip_values + offset);
        value += direction;
        if (value >= choices ) {
            value = 0;
        } else if (value < 0) {
            value = choices - 1;    // saved value is zero-based
        }
        goto _change_bram_dip;
    }

_change_bram_dip:
    unlock_backup_ram();
    *(bram_dip_values + offset) = value;
    lock_backup_ram();
    *REG_SWPBIOS = 0;
}