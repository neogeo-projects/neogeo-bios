#include "backup_ram.h"
#include "bios.h"
#include "mess_out.h"
#include "utils.h"

#include "game_menu.h"

void menu_enter() {
    //*REG_SHADOW = 0;
    *BIOS_GAME_PALETTE_1 = *(uint32_t *)0x400020;
    uint16_t pos = BIOS_GAME_MENU_START_POSITION;
    for (uint16_t x = 0; x < BIOS_GAME_MENU_COLUMN_SIZE; x++) {
        for (uint16_t y = 0; y < BIOS_GAME_MENU_ROW_SIZE; y++) {
            *REG_VRAMADDR = pos + y;
            BIOS_GAME_MENU_SPRITE_BACKUP[(x * BIOS_GAME_MENU_ROW_SIZE) + y] = *REG_VRAMRW;
        }
        pos += 0x20;
    }
}

void menu_exit() {
    //*REG_NOSHADOW = 0;
    *(uint32_t *)0x400020 = *BIOS_GAME_PALETTE_1;
    uint16_t pos = BIOS_GAME_MENU_START_POSITION;
    for (uint16_t x = 0; x < BIOS_GAME_MENU_COLUMN_SIZE; x++) {
        for (uint16_t y = 0; y < BIOS_GAME_MENU_ROW_SIZE; y++) {
            *REG_VRAMADDR = pos + y;
            *REG_VRAMRW = BIOS_GAME_MENU_SPRITE_BACKUP[(x * BIOS_GAME_MENU_ROW_SIZE) + y];
        }
        pos += 0x20;
    }    
}

void menu_update() {
    *(uint32_t *)0x400020 = 0x0000FFFF; // Palette 1, color 0-1
    *(uint32_t *)0x400024 = 0x01110000; // Palette 1, color 2-3

    *BIOS_MESS_BUSY = 1;

    *(volatile uint32_t *) BIOS_MESS_BUFFER = (uint32_t)game_menu_box;
    *BIOS_MESS_POINT = BIOS_MESS_BUFFER_PTR + sizeof(uint32_t);

    volatile uint16_t *address = (volatile uint16_t *)*BIOS_MESS_POINT;
    *address++ = 0x0000;
    *address++ = 0x0000;
    *address++ = 0x0001;
    *address++ = 0x10ff;  // Palette 1
    *address++ = 0x2002;  // Increase each write with a column
    *address++ = 0x0003;
    *address++ = BIOS_GAME_MENU_START_POSITION + 32 + 1;    // Add one column and one row
    *address++ = 0x0007;

    uint8_t block = find_game_data_block(*ROM_NGH_NUMBER);

    for (uint8_t i = 0; i < 16; i++) {
        *(volatile uint8_t *)address = BRAM_GAME_NAME(block)[i];
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
    }
    *address++ = 0xFFFF;

    *address++ = 0x0003;
    *address++ = BIOS_GAME_MENU_START_POSITION + 32 + 2;    // Add one column and two rows
    *address++ = 0x0007;
    // Insert game name and dip settings
    uint8_t region = *SROM_COUNTRY_CODE;    
    uint32_t dips_addr = ROM_SOFT_DIP_TABLE[region];
    volatile uint8_t *dips = (volatile uint8_t *)(uint32_t)dips_addr;
    dips += 32;
    for (uint8_t i = 0; i < 12; i++) {
        *(volatile uint8_t *)address = *dips;
        address = (volatile uint16_t *)((volatile uint8_t *)address + 1);
        dips++; 
    }
    *address++ = 0xFFFF;

    *address++ = 0x0000;
    *BIOS_MESS_POINT = (volatile uint32_t)address;

    *BIOS_MESS_BUSY = 0;    
}

void menu_toggle() {
    if (*BIOS_GAME_MENU == GAME_MENU_HIDDEN) {
        menu_enter();
        *BIOS_GAME_MENU = GAME_MENU_VISIBLE;

        *REG_SWPBIOS = 0;
        *BIOS_GAME_LSPCMODE = *REG_LSPCMODE;
        *REG_LSPCMODE = (uint16_t)(*BIOS_GAME_LSPCMODE & ~LSPC_TIMER_EN);
    } else {
        menu_exit();
        *BIOS_GAME_MENU = GAME_MENU_HIDDEN;

        *REG_LSPCMODE = *BIOS_GAME_LSPCMODE;
        *REG_SWPROM = 0;
    }
}

void check_menu_hotkey() {
    // Ignore hotkey if we are not in a game
    if (0xFF == *BIOS_SWPMODE) {
        return;
    }

    // Hotkey for menu, p1 start + p1 select
    if (((*BIOS_STATCHANGE_RAW) & P1_START) != 0 && ((*BIOS_STATCHANGE_RAW) & P1_SELECT) != 0) {
        menu_toggle();
    }
}