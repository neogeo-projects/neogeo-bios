#include "backup_ram.h"
#include "bios.h"
#include "bios_calls.h"
#include "coin.h"
#include "utils.h"

#include "input.h"

void update_controller_input() {
    /*
        $10FD94	BIOS_P1STATUS	byte	BIOS	0:No connection, 1:Normal joypad, 2:Expanded joypad, 3:Mahjong controller, 4:Keyboard
        $10FD95	BIOS_P1PREVIOUS	byte	BIOS	Inputs on previous frame [DCBA Right Left Down Up] (positive logic).
        $10FD96	BIOS_P1CURRENT	byte	BIOS	Inputs on current frame [DCBA Right Left Down Up] (positive logic).
        $10FD97	BIOS_P1CHANGE	byte	BIOS	Buttons just pressed, active edge [DCBA Right Left Down Up] (positive logic).    
    */
    // Player 1 input
    uint8_t p1_buttons = ~(*REG_P1CNT);
    if (p1_buttons == *BIOS_P1CURRENT) {
        (*BIOS_P1TIMER)--;
        if (0 > (*BIOS_P1TIMER)) {
            *BIOS_P1TIMER = 8;
            *BIOS_P1REPEAT = p1_buttons;
        }
    } else {
        *BIOS_P1TIMER = 0x10;
        *BIOS_P1REPEAT = p1_buttons;
    }
    *BIOS_P1PREVIOUS = *BIOS_P1CURRENT;
    *BIOS_P1CURRENT = p1_buttons;
    *BIOS_P1CHANGE = (*BIOS_P1PREVIOUS ^ *BIOS_P1CURRENT) & *BIOS_P1CURRENT;

    // Player 2 input
    uint8_t p2_buttons = ~(*REG_P2CNT);
    if (p2_buttons == *BIOS_P2CURRENT) {
        (*BIOS_P2TIMER)--;
        if (0 > (*BIOS_P2TIMER)) {
            *BIOS_P2TIMER = 8;
            *BIOS_P2REPEAT = p2_buttons;
        }
    } else {
        *BIOS_P2TIMER = 0x10;
        *BIOS_P2REPEAT = p2_buttons;
    }
    *BIOS_P2PREVIOUS = *BIOS_P2CURRENT;
    *BIOS_P2CURRENT = p2_buttons;
    *BIOS_P2CHANGE = (*BIOS_P2PREVIOUS ^ *BIOS_P2CURRENT) & *BIOS_P2CURRENT;

    // Start - Select
    uint8_t start_select = (~(*REG_STATUS_B)) & 0xF;
    uint8_t previous = *BIOS_STATCURRENT_RAW;
    *BIOS_STATCHANGE_RAW = (previous ^ start_select) & start_select;
    *BIOS_STATCURRENT_RAW = start_select;

    *BIOS_STATCURRENT = (*BIOS_STATCURRENT_RAW) & 0x55;
    *BIOS_STATCHANGE = (*BIOS_STATCHANGE_RAW) & 0x55;

    if (*BIOS_P1STATUS == 3 || *BIOS_P2STATUS == 3) { /* Mahjong stuff */ }

    return;
}

void check_start_button() {
    // TODO: Support 4 players
    if (0 != ((*BIOS_STATCHANGE_RAW) & 0x55)) {
        uint8_t p1_start = *BIOS_STATCHANGE_RAW & 0x01;
        // Ignore start button if the player is already playing
        if (1 == *BIOS_PLAYER_MOD1) {
            p1_start = 0;
        }
        uint8_t p2_start = *BIOS_STATCHANGE_RAW & 0x04;
        // Ignore start button if the player is already playing
        if (1 == *BIOS_PLAYER_MOD2) {
            p2_start = 0;
        }        

        // TODO: Check for enough credits
        if (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_USA) {
            if (0 < p1_start) {
                decrease_credit(1, *BIOS_P1CREDIT_DEC);
                *BIOS_START_FLAG |= 0x01;
            }
            if (0 < p2_start) {
                decrease_credit(2, *BIOS_P2CREDIT_DEC);
                *BIOS_START_FLAG |= 0x02;                
            }
        } else if ((*BIOS_COUNTRY_CODE == BIOS_COUNTRY_EUROPE) || (*BIOS_COUNTRY_CODE == BIOS_COUNTRY_JAPAN)) {
            if (0 < p1_start && 0 != *BIOS_P1CREDIT_DEC) {
                decrease_credit(1, *BIOS_P1CREDIT_DEC);
                *BIOS_START_FLAG |= 0x01;
            }
            if (0 < p2_start && 0 != *BIOS_P2CREDIT_DEC) {
                // Start both players with p2 start if we are not in game-mode yet
                // use BIOS_PLAYER_MOD2 instead?
                decrease_credit(1, *BIOS_P2CREDIT_DEC);
                if(0x02 == *BIOS_USER_MODE) {
                    *BIOS_START_FLAG |= 0x02;
                } else {
                    *BIOS_START_FLAG |= 0x01;
                    *BIOS_START_FLAG |= 0x02;
                }
            }
        }
    } else {
        check_compulsion_timer();
    }    
}

void check_change_game() {
    // Fail-safe to not over rotate
    if (0 != *BIOS_NEXT_GAME_ROTATE) {
        return;
    }

    // If we are not in Title/Demo, skip the rest.
    if (1 != *BIOS_USER_MODE) {
        return;
    }

    // Game start compulsion (0 = enabled, 1 = disabled)
    if (0 != *BRAM_DIP_GAME_START_FORCE) {
        return;
    }

    // Check if freeplay / credit is enough
    if (0 == *BRAM_CREDIT_P1 && 0 == *BRAM_CREDIT_P2 && 0 == *BRAM_DIP_GAME_SELECT_FREE) {
        return;
    }

    // Check if the 'select' button is pressed
    if (((*BIOS_STATCHANGE_RAW) & 0x0a) != 0) {
        if (((*BIOS_STATCURRENT_RAW) & 0x02) != 0) {
            // P1 select = next slot
            *BIOS_SYSRET_STATUS = 4;
        } else {
            // P2 select = previous slot
            *BIOS_SYSRET_STATUS = 5;
        }
        *BIOS_NEXT_GAME_ROTATE = 1;
        SUBR_CART_DEMO_END();
        system_return();
    }
}

void check_compulsion_timer() {
    // Game start compulsion (0 = enabled, 1 = disabled)
    if (0 != *BRAM_DIP_GAME_START_FORCE) {
        return;
    }

    // Return if the "game" is already running
    if (2 == *BIOS_USER_MODE) {
        return;
    }

    // Ignore compulsion timer if none of the players got credit
    if (0 == *BRAM_CREDIT_P1 && 0 == *BRAM_CREDIT_P2) {
        return;
    }

    (*BIOS_COMPULSION_FRAME_TIMER)--;
    if(0 < *BIOS_COMPULSION_FRAME_TIMER) {
        return;
    }
    
    uint8_t compulsion_timer = from_bcd8(*BIOS_COMPULSION_TIMER);
    
    if (0 == compulsion_timer) {
        // TODO: Seems lika weird assumption from orignal code?
        // How to keep track of which player added a coin?
        // TODO: Check for enough credits
        if (0 < *BRAM_CREDIT_P1) {
            *BIOS_START_FLAG |= 0x01;
        } else {
            *BIOS_START_FLAG |= 0x02;
        }
        return;
    }

    compulsion_timer--;
    *BIOS_COMPULSION_TIMER = to_bcd8(compulsion_timer);
    *BIOS_COMPULSION_FRAME_TIMER = 59;
}
