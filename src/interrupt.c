#include "bios.h"
#include "bios_calls.h"
#include "game_menu.h"

#include "interrupt.h"

void __attribute__((interrupt)) vblank_handler(void) {
    *IRQ_ACK = 0x04;
    *WATCHDOG = 0;
    *BIOS_FRAME_COUNTER += 1;

    if (*BIOS_INT1_SKIP == 0) {
        mess_out();
        system_io();

        if (*BIOS_GAME_MENU == GAME_MENU_VISIBLE) {
            menu_update();
        }

        *BIOS_VBLANK_CLEAR = 0;
    }
    *BIOS_INT1_FRAME_COUNTER += 1;
}

void __attribute__((interrupt)) hblank_handler(void) {
    *WATCHDOG = 0;
    *IRQ_ACK = 0x02;
    *LSPCMODE = 0x4000;
}

void __attribute__((interrupt)) cold_boot_handler(void) {
    *WATCHDOG = 0;
    *IRQ_ACK = 0x01;
}

void __attribute__((interrupt)) default_handler(void) {
    *WATCHDOG = 0;
    *IRQ_ACK = 0x07;
}

void __attribute__((interrupt)) bus_error_handler(void) {
    *IRQ_ACK = 0x10;
}

void __attribute__((interrupt)) address_error_handler(void) {
    *IRQ_ACK = 0x11;
}

void __attribute__((interrupt)) illegal_instruction_handler(void) {
    *IRQ_ACK = 0x12;
}

void __attribute__((interrupt)) division_by_zero_handler(void) {
    *IRQ_ACK = 0x13;
}

void __attribute__((interrupt)) chk_handler(void) {
    *IRQ_ACK = 0x14;
}

void __attribute__((interrupt)) trapv_handler(void) {
    *IRQ_ACK = 0x15;
}