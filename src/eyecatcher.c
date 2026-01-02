#include "backup_ram.h"
#include "bios.h"
#include "bios_calls.h"
#include "graphic.h"
#include "mess_out.h"
#include "utils.h"

#include "eyecatcher.h"

void play_bios_eyecatcher() {
    *REG_NOSHADOW = 0;
    *REG_SOUND = 0x03;

    *EYECATCHER_POSITION = 0;
    wait_for_vblank();

    while(1) {
        // If AES, button or start will end eyecatcher
        // If MVS, only coin-in will end eyecatcher
        if (0 == *BIOS_MVS_FLAG) {
            uint16_t p1_buttons = *BIOS_P1CHANGE & 0xF0;
            uint16_t p1_start = *BIOS_STATCHANGE & 0x01;
            if (p1_buttons || p1_start) {
                system_return();
            }
        } else {
            if (0x00 != *BRAM_CREDIT) {
                system_return();
            }
        }

        *BIOS_MESS_BUSY = 1;
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uint32_t) neo_geo_logo;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
        *BIOS_MESS_BUSY = 0;
        wait_for_vblank();
        
        // Update the palette if requested
        if (1 == *EYECATCHER_CHANGE_PALETTE) {
            volatile uint16_t *address = PALETTE_15;
            uint16_t *palette = (uint16_t *)neo_geo_byte_stream_palette_1;
            palette += *EYECATCHER_PALETTE_OFFSET;
            for (uint8_t i = 0; i < 5; i++) {
                *address = *palette;
                address++;
                palette++;
            }
            *PALETTE_BACKGROUND = *palette;
            palette++;
            *EYECATCHER_PALETTE_OFFSET += 6;
            *EYECATCHER_CHANGE_PALETTE = 0;
        }
        
        if (0 == *EYECATCHER_POSITION) {            // Setup
            *BIOS_MESS_BUSY = 1;
            reset_palettes();
            fix_clear();
            sprite_clear();

            uint8_t upper_tile_byte = *ROM_EYECATCH_SPRITE_BANK;
            *REG_VRAMMOD = 1;
            uint16_t index = 0;
            uint16_t vram_address = 0x40;
            // Writes 0x40 -> 0x4C0
            for (uint8_t i = 0; i < 18; i++) {
                *REG_VRAMADDR = vram_address;
                for (uint8_t x = 0; x < 4; x++) {
                    *REG_VRAMRW = (upper_tile_byte << 8) | neo_geo_byte_stream_1[index];    // Tile number
                    __asm__ volatile ("nop");
                    *REG_VRAMRW = 0x0F01;                                                   // Tile palette
                    index++;
                }

                uint16_t tile = (upper_tile_byte << 8) | 0xFF;
                for (uint8_t x = 0; x < 28; x++) {
                    *REG_VRAMRW = tile;         // Tile number
                    __asm__ volatile ("nop");
                    *REG_VRAMRW = 0x0000;       // Tile palette
                }

                vram_address += 0x40;
            }

            index = 0;
            vram_address = 0x4c0;
            // writes 0x4C0 -> 0x940
            for (uint8_t i = 0; i < 18; i++) {
                *REG_VRAMADDR = vram_address;
                for (uint8_t x = 0; x < 4; x++) {
                    *REG_VRAMRW = (upper_tile_byte << 8) | neo_geo_byte_stream_2[index];
                    __asm__ volatile ("nop");
                    *REG_VRAMRW = 0x0F00;
                    index++;
                }
                uint16_t tile = (upper_tile_byte << 8) | 0xFF;
                for (uint8_t x = 0; x < 28; x++) {
                    *REG_VRAMRW = tile;
                    __asm__ volatile ("nop");
                    *REG_VRAMRW = 0x0000;
                }
                vram_address += 0x40;
            }

            *REG_VRAMADDR = 0;
            *REG_VRAMMOD = 0x02;
            // writes 0x0 -> 0x40
            for (uint8_t i = 0; i < 32; i++) {
                *REG_VRAMRW = (upper_tile_byte << 8) | 0xFF;
            }

            // 10FC00 -> 10FC47 - Horizontal position
            // 10FC48 -> 10FC8F - Vertical position
            // 10FC90 -> 10FCD7 - Shrinking
            *(volatile uint16_t *)0x10FC00 = 0x4B80;

            // Writes 0x10FC48 -> 0x10FC8F
            volatile uint16_t *address = (volatile uint16_t *)0x10FC48;
            *address = 0xE404;
            address++;
            for (uint8_t i = 0; i < 35; i++) {
                *address = 0x44;
                address++;
            }
            *(volatile uint16_t *)0x10FC6C = 0x0000;
            
            // Writes 0x10FC90 -> 0x10FCD7
            address = (volatile uint16_t *)0x10FC90;
            for (uint8_t i = 0; i < 18; i++) {
                *address = 0xff;
                address++;
            }
            for (uint8_t i = 0; i < 18; i++) {
                *address = 0xfff;
                address++;
            }

            *EYECATCHER_CHANGE_PALETTE = 1;
            *EYECATCHER_ITERATOR = 0;

            *BIOS_MESS_BUSY = 0;
            *EYECATCHER_POSITION = 1;
        } else if (1 == *EYECATCHER_POSITION) {     // Logo folds out horizontally
            *EYECATCHER_ITERATOR += 1;
            if (5 == *EYECATCHER_ITERATOR) {
                if (0 == *BRAM_DIP_DEMO_SOUND) {
                    *REG_SOUND = 0x2;
                }
            }
            
            if (0 == *EYECATCHER_ITERATOR % 2) {
                *(volatile uint16_t *)0x10FC00 -= 0x480;

                volatile uint16_t *address = (volatile uint16_t *)0x10FC90;
                uint16_t value = (*address) + 0x100;
                for (uint8_t i = 0; i < 18; i++) {
                    *address = value;
                    address++;
                }

                if (0xFFF == value) {
                    *EYECATCHER_ITERATOR = 0xA;
                    *EYECATCHER_POSITION = 2;
                }
            }
            
        } else if (2 == *EYECATCHER_POSITION) {     // Delay
            *EYECATCHER_ITERATOR -= 1;
            if (0 == *EYECATCHER_ITERATOR) {
                *EYECATCHER_PALETTE_OFFSET = 0;
                *EYECATCHER_POSITION = 3;
            }
        } else if (3 == *EYECATCHER_POSITION) {     // Logo folds in vertically, fade to black
            *EYECATCHER_ITERATOR += 1;
            if (0 == *EYECATCHER_ITERATOR % 2) {
                *EYECATCHER_CHANGE_PALETTE = 1;
                *(volatile uint16_t *)0x10FC48 -= 0x100;    // Move downwards
                *(volatile uint8_t *)0x10FC91 -= 0x10;      // Shrink
                
                if (0xFF == *(volatile uint8_t *)0x10FC91) {
                    *(volatile uint16_t *)0x10FCB4 = 0x0F00;
                    *(volatile uint16_t *)0x10FC6C = *(volatile uint16_t *)0x10FC48;
                    *(volatile uint16_t *)0x10FC48 = 0x6404;
                    *(volatile uint16_t *)0x10FC24 = 0x800;                  
                    *(volatile uint16_t *)0x10FCDE = 0x00;
                    *(volatile uint16_t *)0x10FCE0 = 0x71B0;
                    *(volatile uint32_t *)0x10FCE2 = (uint32_t)max_330;
                    *EYECATCHER_ITERATOR = 0x00;
                    *EYECATCHER_POSITION = 4;
                }
            }
        } else if (4 == *EYECATCHER_POSITION) {     // Logo folds out vertically, fade to black
            *EYECATCHER_ITERATOR += 1;
            if (0 == *EYECATCHER_ITERATOR % 2) {
                *EYECATCHER_CHANGE_PALETTE = 1;
                *(volatile uint16_t *)0x10FC6C += 0x100;    // Move upwards
                if (*(volatile uint8_t *)0x10FCB5 == 0) {
                    *(volatile uint8_t *)0x10FCB5 = 0xFF;
                }
                *(volatile uint8_t *)0x10FCB5 += 0x10;      // Un-shrink
                if (*(volatile uint8_t *)0x10FCB5 == 0xFF) {
                    *EYECATCHER_ITERATOR = 0;
                    *EYECATCHER_POSITION = 5;
                }
            }
        } else if (5 == *EYECATCHER_POSITION) {     // MAX 330 MEGA
            *EYECATCHER_ITERATOR += 1;
            if (0 == *EYECATCHER_ITERATOR % 4) {
                *BIOS_MESS_BUSY = 1;
                *(uint16_t *) *BIOS_MESS_POINT = 0x0000;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0000;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0301;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0002;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0102;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0003;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = *(volatile uint16_t *)0x10FCE0;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0004;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *(uint32_t *) *BIOS_MESS_POINT = *(volatile uint32_t *)0x10FCE2;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
                *(uint16_t *) *BIOS_MESS_POINT = 0x0000;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
                *BIOS_MESS_BUSY = 0;
                *(volatile uint16_t *)0x10FCE0 += 0x20;
                *(volatile uint32_t *)0x10FCE2 += 0x04;
                *(volatile uint16_t *)0x10FCDE += 0x01;

                if (*(volatile uint16_t *)0x10FCDE == 0xF) {
                    *(volatile uint16_t *)0x10FCE0 = 0x7192;
                }
            }

            if (*(volatile uint16_t *)0x10FCDE >= 0x20) {
                *EYECATCHER_ITERATOR = 0x14;
                *EYECATCHER_POSITION = 6;
            }
        } else if (6 == *EYECATCHER_POSITION) {     // Delay
            *EYECATCHER_ITERATOR -= 1;
            if (0 == *EYECATCHER_ITERATOR) {
                *EYECATCHER_ITERATOR = 0;
                *EYECATCHER_POSITION = 7;
            }
        } else if (7 == *EYECATCHER_POSITION) {     // Blue SNK logo
            // Add tiles for the blue logo
            if (0 == *EYECATCHER_ITERATOR) {
                *BIOS_MESS_BUSY = 1;
                *(volatile uint32_t *) *BIOS_MESS_POINT = (uint32_t) neo_geo_logo_blue;
                *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
                *BIOS_MESS_BUSY = 0;
            }

            *EYECATCHER_ITERATOR += 1;

            if (0 == *EYECATCHER_ITERATOR % 2) {
                *(volatile uint16_t *)0x10FCDA += 1;
                *EYECATCHER_CHANGE_PALETTE = 1;
                if (0x10 == *(volatile uint16_t *)0x10FCDA) {
                    *EYECATCHER_POSITION = 8;
                }
            }
        } else if (8 == *EYECATCHER_POSITION) {     // Copyright symbol
            *(volatile uint32_t *)0x400002 = 0x0ddd0000;    // Palette 0, Color 0x1
            *REG_VRAMADDR = 0x7469;
            *REG_VRAMRW = 0x7b;
            *EYECATCHER_ITERATOR = 0xB4;
            *EYECATCHER_POSITION = 9;
        } else if (9 == *EYECATCHER_POSITION) {     // Delay
            *EYECATCHER_ITERATOR -= 1;
            if (0 == *EYECATCHER_ITERATOR) {
                break;
            }
        }
    }

    sprite_clear();
    fix_clear();

    uint16_t delay_timer = 0;
    if (1 == *BIOS_MVS_FLAG) {
        *BIOS_MESS_BUSY = 1;
        if (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_JAPAN) {
            *(volatile uint32_t *)0x4001e2 = 0x0eee0000;
            *(volatile uint32_t *) *BIOS_MESS_POINT = (uint32_t)japan_warning;
            *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
            delay_timer = 0x12C;
        } else if (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_USA) {
            *REG_SWPBIOS = 0;
            *REG_BRDFIX = 0;
            for (uint8_t i = 0; i < sizeof(fbi_palette) / sizeof(fbi_palette[0]); i++) {
                PALETTE_FBI[i] = fbi_palette[i];
            }
            *(volatile uint32_t *) *BIOS_MESS_POINT = (uint32_t)fbi_logo;
            *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
            delay_timer = 0x12C;
        } else if (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_EUROPE) {
            delay_timer = 0;
        }
        *BIOS_MESS_BUSY = 0;
        wait_for_vblank();
    }
    
    for (uint16_t i = 0; i < delay_timer; i++) {
        wait_for_vblank();
    }

    wait_for_z80();
    wait_for_vblank();
    *REG_SOUND = 0x03;
    system_return();
};
