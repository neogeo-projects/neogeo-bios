#include <stdint.h>

#include "backup_ram.h"
#include "bios.h"
#include "calendar.h"
#include "coin.h"
#include "eyecatcher.h"
#include "game_menu.h"
#include "graphic.h"
#include "how_to_play.h"
#include "input.h"
#include "interrupt.h"
#include "mess_out.h"
#include "service.h"
#include "service_dips.h"
#include "service_hardware.h"
#include "utils.h"

#include "bios_calls.h"

void system_return(void) {
    //*BIOS_FRAME_SKIP = 0xFF;
    *WATCHDOG = 0;
    *IRQ_ACK = 0x07;
    *LSPCMODE = 0x4000;
    *BIOS_SYSTEM_MODE &= 0x7F;                  // Make sure BIOS vblank interrupt is used
    __asm__ volatile ("move #0x2000, %sr");     // Enable interrupt
    wait_for_vblank();
    *REG_SWPBIOS = 0;
    *BIOS_SWPMODE = 0xFF;
    __asm__ volatile ("move.l #0x10F300, %sp");

    reset_palettes();
    reset_fix_layer();
    reset_sprites();

    controller_setup();

    if (0 == *BIOS_SYSRET_STATUS) { // Init
        if (8 != *BRAM_SLOT_CURSOR) {
            save_game_data();
            change_slot_incremental();
            unlock_backup_ram();
            // If the selected slot is lower than the cursor, the system have overflowed the slots from max -> 0.
            if (*BRAM_SLOT_SELECTED <= *BRAM_SLOT_CURSOR) {
                *BRAM_SLOT_CURSOR = 8;
            } else {
                *BRAM_SLOT_CURSOR = *BRAM_SLOT_SELECTED;
            }
            lock_backup_ram();
            *BIOS_USER_REQUEST = USER_REQUEST_INIT;           
        }
        if (8 == *BRAM_SLOT_CURSOR) {
            // Check if service-button is pressed
            // TODO: In the original BIOS, there is also a service-button-test in system_io().
            uint8_t service_button = ((~(*REG_STATUS_A)) & 0x4) >> 2;
            uint8_t dipsw_settings = (~(*REG_DIPSW)) & 0x1;
            if (service_button || dipsw_settings) {
                *REG_BRDFIX = 0;
                show_bios_menu();
            }

            // If no game was found (could not read NGH-number), show hardware test menu
            if (0xFF == *BRAM_FIRST_PLAYABLE_SLOT) {
                *REG_BRDFIX = 0;
                show_bios_hardware_test();
            }

            // Normal initialization
            *REG_SLOT = *BRAM_FIRST_PLAYABLE_SLOT;
            *BRAM_SLOT_SELECTED = *BRAM_FIRST_PLAYABLE_SLOT;
            load_game_data();

            *BIOS_SYSRET_STATUS = 3;
            if (0 == *ROM_EYECATCH_FLAG) {          // Eyecatcher handled by BIOS
                play_bios_eyecatcher();
            } else if (1 == *ROM_EYECATCH_FLAG) {   // Eyecatcher handled by Game
                *BIOS_USER_REQUEST = USER_REQUEST_EYECATCH;
            }
        }
    } else if (3 == *BIOS_SYSRET_STATUS) {  // Game over
        // Fail-safe to not overwrite already saved data during startup
        if (USER_REQUEST_INIT != *BIOS_USER_REQUEST) {
            save_game_data();
        }
        load_game_data();
        if (*BRAM_CREDIT_P1 != 0 || *BRAM_CREDIT_P2 != 0) {
            *BIOS_USER_REQUEST = USER_REQUEST_TITLE;

            // Reset compulsion timer
            if (0 == *BRAM_DIP_GAME_START_FORCE) {
                *BIOS_COMPULSION_TIMER = *BRAM_DIP_COMPULSION_TIMER;
                *BIOS_COMPULSION_FRAME_TIMER = *BRAM_DIP_COMPULSION_FRAME_TIMER;
            }
        } else {
            *BIOS_USER_REQUEST = USER_REQUEST_DEMO;
        }
    } else if (4 == *BIOS_SYSRET_STATUS || 5 == *BIOS_SYSRET_STATUS) {  // Next/previous slot
        save_game_data();
        if (4 == *BIOS_SYSRET_STATUS) {
            change_slot_incremental();
        } else {
            change_slot_decremental();
        }

        load_game_data();
        *BIOS_USER_MODE = 1;

        // Ignore eyecatcher when slot is changed
        *BIOS_USER_REQUEST = USER_REQUEST_DEMO;

        // Ignore Demo if credit is present
        if (*BRAM_CREDIT_P1 != 0 || *BRAM_CREDIT_P2 != 0) {
            *BIOS_USER_REQUEST = USER_REQUEST_TITLE;
        }

        *BIOS_NEXT_GAME_ROTATE = 0;
    }

    start_game();
}

void system_io(void) {
    update_coin();
    update_controller_input();
    // TODO: Include a in-game pause with controller
    // Maybe use a global pause_requested variable to use outside of system_io()?
    if (((*REG_DIPSW) & 0x80) == 0) {
        pause_system();
    }

    int8_t skip_frame = (int8_t)*BIOS_FRAME_SKIP;
    if (skip_frame > 0) {
        *BIOS_FRAME_SKIP -= 1;
    } else if (skip_frame == 0) {
        // Game mode = bit 0x80 set
        if ((*BIOS_SYSTEM_MODE) & 0x80) {
            check_start_button();

            // Check if PLAYER_START wants to accept a start
            // PLAYER_START will update BIOS_PLAYER_MOD[1-4]
            if (0 < *BIOS_START_FLAG) {
                SUBR_CART_PLAYER_START();
            }

            // Check if PLAYER_START have removed any flags.
            if (0 < *BIOS_START_FLAG) {
                *BIOS_COMPULSION_TIMER = 0;
                *BIOS_FRAME_SKIP = 1;           // Delay-fix for something?
            }

            // Check if 'select' is pressed to change the game on a multislot
            check_change_game();
        }
    }
    
    if (*BIOS_MVS_FLAG == 1 && *BIOS_USER_MODE == 2) {
        unlock_backup_ram();
        
        uint32_t player_status = *BIOS_PLAYER_MOD_START;
        if (player_status & 0x000F == 1 || player_status & 0x00F0 == 1) {
            *BRAM_PLAY_FRAME_TIMER -= 1;
            if (*BRAM_PLAY_FRAME_TIMER == 0) {
                *BRAM_PLAY_FRAME_TIMER = *BRAM_DIP_COMPULSION_FRAME_TIMER;  // TODO: Make sure this value is actually safe to use.
                *BRAM_PLAY_SECONDS_HEX += 1;
            }
        }

        lock_backup_ram();
    }

    check_menu_hotkey();

    return;
}

void credit_check(void) {
    // If the system is AES, no credit is needed
    if (*BIOS_MVS_FLAG == 0) {
        return;
    }

    // Check for freeplay hardware dip-switch
    if ((~(*REG_DIPSW)) & 0x40) {
        return;
    }
    
    // Player 1 credit
    *BIOS_P1CREDIT_DEC = _credit_check_player(1);
    // Player 2 credit
    *BIOS_P2CREDIT_DEC = _credit_check_player(2);

    return;
}

// P1 = 1, P2 = 2 etc
uint8_t _credit_check_player(uint8_t player) {
    if (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_USA) {    
        uint8_t credit = from_bcd8(BRAM_CREDIT[player - 1]);
        if (credit > 0) {
            return 1;
        }
    } else if ((*BIOS_COUNTRY_CODE == BIOS_COUNTRY_EUROPE) || (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_JAPAN)) {
        uint8_t credit = from_bcd8(*BRAM_CREDIT_P1);
        
        if (1 == player) {
            if (credit > 0) {
                return 1;
            }
        } else {
            uint8_t need_credit = 0;
            // If the game has already started, assume the P1 already had its credit reduction.
            if (2 == *BIOS_USER_MODE) {
                need_credit = 1;
            } else {
                need_credit = 2;
            }
            if (credit >= need_credit) {
                return need_credit;
            }
        }
    }
    return 0;
}

void credit_down(void) {
    if (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_USA) {  
        decrease_credit(1, *BIOS_P1CREDIT_DEC);
        decrease_credit(2, *BIOS_P2CREDIT_DEC);
    } else if ((*BIOS_COUNTRY_CODE == BIOS_COUNTRY_EUROPE) || (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_JAPAN)) {
        decrease_credit(1, *BIOS_P1CREDIT_DEC);
        decrease_credit(1, *BIOS_P2CREDIT_DEC);
    }

    // TODO: Update bookkeeping values
    return;
}

// 0 = P1, 1 = P2 etc
void _credit_down(uint8_t player_offset) {
    // Ignore if freeplay dip-switch is active
    if (*REG_DIPSW & 0x40) {
        return;
    }
    uint8_t credit = _credit_check_player(player_offset);
    if (credit != 0) {
        credit -= *(BIOS_CREDIT_DEC + player_offset);
    }
    *(BRAM_CREDIT + player_offset) = to_bcd8(credit);
}

void read_calendar(void) {
    _read_calendar();
}

void setup_calendar(void) {
    _setup_calendar();
}

void card(void) {
    *WATCHDOG = 0;

    uint8_t memory_card_inserted = (*REG_STATUS_B) & 0x30;
    // 00:Memory card correctly inserted
    if (0 == memory_card_inserted) {     
        *REG_CRDNORMAL = 0;
        *REG_CRDBANK = 0;
        *BIOS_CARD_COMMAND = 0x05;

        // Return error for now until test-code is done
        *BIOS_CARD_ANSWER = CARD_ANSWER_NOT_INSERTED;
        // TODO: Test the memory card!
    } else {
        *BIOS_CARD_ANSWER = CARD_ANSWER_NOT_INSERTED;
    }    
}

void card_error(void) {
    return;
}

void how_to_play(void) {
    if (1 == *BIOS_MVS_FLAG) {
        _how_to_play();
    }
    return;
}

void checksum_loop(void) {
    return;
}

void fix_clear(void) {
    reset_fix_layer();
}

void sprite_clear(void) {
    reset_sprites();
}

void mess_out(void) {
    _mess_out();
}

void controller_setup(void) {
    *BIOS_P1STATUS = 0;
    *BIOS_P2STATUS = 0;
    *BIOS_P3STATUS = 0;
    *BIOS_P4STATUS = 0;

    // 0:Normal controller, 1:Mahjong keyboard
    if (*REG_DIPSW & 0x4) {
        *BIOS_P1STATUS = 3;
    }
    *REG_POUTPUT = 0;
    return;
}
