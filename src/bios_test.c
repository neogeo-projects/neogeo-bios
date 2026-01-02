#include "backup_ram.h"
#include "bios.h"
#include "bios_calls.h"
#include "calendar.h"
#include "graphic.h"
#include "mess_out.h"
#include "utils.h"

#include "bios_test.h"

void test_led() {
    *REG_LEDLATCHES = 0xFF;
    nop_loop(12);
    *REG_LEDDATA = 0xFF;
    *REG_LEDLATCHES = 0xF7;
    nop_loop(12);
    *REG_LEDLATCHES = 0xFF;
    nop_loop(12);
    *REG_LEDLATCHES = 0xEF;
    nop_loop(12);
    *REG_LEDLATCHES = 0xFF;
    nop_loop(12);
    *REG_LEDLATCHES = 0xDF;
    nop_loop(12);
    *REG_LEDLATCHES = 0xFF;    
};

void test_palette_ram() {
    // Palette RAM (0x400000 - 0x401FFF)
    for (int8_t bank = 1; bank >= 0; bank--) {
        if (bank == 0) {
            *REG_PALBANK0 = 0;
        } else if (bank == 1) {
            *REG_PALBANK1 = 0;
        }
        volatile uint16_t *address = PALETTE_RAM;
        volatile uint16_t *end = address + 0x2000;
        while (address < end) {
            *WATCHDOG = 0;
            *address = 0x5555;
            if (*address != 0x5555) {
                print_error_msg(BIOS_ERROR_PALETTE_BANK_0 + bank, (uint32_t)address, 0x5555, *address);
            }
            *address = 0xAAAA;
            if (*address != 0xAAAA) {
                print_error_msg(BIOS_ERROR_PALETTE_BANK_0 + bank, (uint32_t)address, 0xAAAA, *address);
            }        
            *address = 0x0000;
            address++;
        }
    }
    *REG_PALBANK0 = 0;
    *PALETTE_REFERENCE = 0x8000;
    *PALETTE_BACKGROUND = 0x0000;
    *REG_PALBANK0 = 1;
    *PALETTE_REFERENCE = 0x8000;
    *PALETTE_BACKGROUND = 0x0000;
};

void test_video_ram() {
    *REG_VRAMMOD = 1;

    // Write 0x5555, read back the value and compare. Set to 0x0000 if test pass
    // Test VRAM 0x0000 -> 0x7FFF
    *REG_VRAMADDR = 0;
    for (uint16_t i = 0; i < 0x8000; i++) {
        *WATCHDOG = 0;
        *REG_VRAMRW = (uint16_t)0x5555;
    }
    *REG_VRAMADDR = 0;
    for (uint16_t i = 0; i < 0x8000; i++) {
        *WATCHDOG = 0;
        uint16_t value = *REG_VRAMRW;
        if (0x5555 != value) {
            print_error_msg(BIOS_ERROR_VIDEO_RAM, i, 0x5555, value);
        }
        *REG_VRAMRW = (uint16_t)0x0000;
    }
    *REG_VRAMADDR = 0;
    for (uint16_t i = 0; i < 0x8000; i++) {
        *WATCHDOG = 0;
        *REG_VRAMRW = (uint16_t)0xAAAA;
    }
    *REG_VRAMADDR = 0;
    for (uint16_t i = 0; i < 0x8000; i++) {
        *WATCHDOG = 0;
        uint16_t value = *REG_VRAMRW;
        if (0xAAAA != value) {
            print_error_msg(BIOS_ERROR_VIDEO_RAM, i, 0xAAAA, value);
        }
        *REG_VRAMRW = (uint16_t)0x0000;
    }

    // Test VRAM 0x8000 -> 0x85FF
    *REG_VRAMADDR = 0x8000;
    for (uint16_t i = 0; i < 0x600; i++) {
        *WATCHDOG = 0;
        *REG_VRAMRW = (uint16_t)0x5555;
    }
    *REG_VRAMADDR = 0x8000;
    for (uint16_t i = 0; i < 0x600; i++) {
        *WATCHDOG = 0;
        uint16_t value = *REG_VRAMRW;
        if (0x5555 != value) {
            print_error_msg(BIOS_ERROR_VIDEO_RAM, i, 0x5555, value);
        }
        *REG_VRAMRW = (uint16_t)0x0000;
    }
    *REG_VRAMADDR = 0x8000;
    for (uint16_t i = 0; i < 0x600; i++) {
        *WATCHDOG = 0;
        *REG_VRAMRW = (uint16_t)0xAAAA;
    }
    *REG_VRAMADDR = 0x8000;
    for (uint16_t i = 0; i < 0x600; i++) {
        *WATCHDOG = 0;
        uint16_t value = *REG_VRAMRW;
        if (0xAAAA != value) {
            print_error_msg(BIOS_ERROR_VIDEO_RAM, i, 0xAAAA, value);
        }
        *REG_VRAMRW = (uint16_t)0x0000;
    }    
};

void test_backup_ram() {
    // Backup RAM (0xD00000 - 0xD0FFFF)
    unlock_backup_ram();

    uint16_t value = 0;
    volatile uint16_t *address = BACKUP_RAM;
    volatile uint16_t *end = address + 0x10000;
    while (address < end) {
        *WATCHDOG = 0;
        value = *address;

        *address = 0x5555;
        if (*address != 0x5555) {
            print_error_msg(BIOS_ERROR_BACKUP_RAM, (uint32_t)address, 0x5555, *address);
        }
        *address = 0xAAAA;
        if (*address != 0xAAAA) {
            print_error_msg(BIOS_ERROR_BACKUP_RAM, (uint32_t)address, 0xAAAA, *address);
        }        
        *address = value;
        address++;
    }

    lock_backup_ram();

    if (0 == *REG_DIPSW) {
        reset_backup_ram();
        return;
    }

    for (uint8_t i = 0; i < BRAM_SIGNATURE_SIZE; i++) {
        if (BRAM_SIGNATURE_STR[i] != _bram_signature_str[i]) {
            reset_backup_ram();
        }
    }
};

void test_memory_card() {
    card();     // use the already defined bios call for testing the memory card
}

void test_bios_checksum() {
    uint16_t checksum = 0;

    // Iterate half the size of SYSTEM_ROM since it is uint16_t (2 byte)
    for (uint32_t i = 0; i < (0x20000 / 2); i++) {
        *WATCHDOG = 0;
        if ((0x80 / 2) == i) {
            continue;
        }
        checksum += SYSTEM_ROM[i];
    }

    if (*BIOS_CHECKSUM != checksum) {
        print_error_msg(BIOS_ERROR_SYSTEM_ROM, 0, 0, 0);
    }
}

void test_sound() {
    *REG_SOUND = 0x01;      // Prepare switch
    __asm__ volatile ("nop");
    for (uint16_t i = 0; i < 50000; i++) {
        *WATCHDOG = 0;
        if (*REG_SOUND == 0x01) {
            __asm__ volatile ("nop");
            return;
        }
    }
    print_error_msg(BIOS_ERROR_Z80, 0, 0, 0);
}

void test_rtc() {
    write_rtc_command(0x8);
    uint8_t previous_status = 0x40;
    wait_for_rtc_pulse_edge(&previous_status);
    *BIOS_INT1_FRAME_COUNTER = 0;
    *BIOS_INT1_SKIP = 1;
    __asm__ volatile ("move #0x2000, %sr");
    wait_for_rtc_pulse_edge(&previous_status);
    __asm__ volatile ("move #0x2700, %sr");
    *BIOS_INT1_SKIP = 0;
    write_rtc_command(0x7);
    if (*BIOS_INT1_FRAME_COUNTER < 57 || *BIOS_INT1_FRAME_COUNTER > 63) {
        print_error_msg(BIOS_ERROR_CALENDAR, 0, 0, 0);
    }    
}

void print_error_msg(uint8_t type, uint32_t memory_location, uint16_t expected, uint16_t actual) {
    *IRQ_ACK = 0x07;
    // TODO: Different error on MVS and AES?
    // Only color on AES?
    // if (MVS_FLAG) {}
    *WATCHDOG = 0;
    *REG_VRAMMOD = 1;
    *REG_VRAMADDR = 0x7000;     // Fix map
    for (uint16_t i = 0; i < 0x500; i++) {
        *REG_VRAMRW = (uint16_t)0xF020;
    }
    *(volatile uint32_t *)0x4001e2 = 0x0eee0000;
    *BIOS_MESS_BUSY = 1;

    *BIOS_MESS_POINT = BIOS_MESS_BUFFER_PTR;

    if (BIOS_ERROR_WORK_RAM == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) work_ram_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);
    } else if (BIOS_ERROR_BACKUP_RAM == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) backup_ram_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);
    } else if (BIOS_ERROR_PALETTE_BANK_0 == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) palette_bank_0_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);
    } else if (BIOS_ERROR_PALETTE_BANK_1 == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) palette_bank_1_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);
    } else if (BIOS_ERROR_VIDEO_RAM == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) video_ram_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);
    } else if (BIOS_ERROR_CALENDAR == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) calendar_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);        
    } else if (BIOS_ERROR_SYSTEM_ROM == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) system_rom_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);         
    } else if (BIOS_ERROR_MEMORY_CARD == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) memory_card_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);         
    } else if (BIOS_ERROR_Z80 == type) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = (uintptr_t) z80_error;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uintptr_t);         
    }

    if (type < 5) {
        *(volatile uint32_t *) *BIOS_MESS_POINT = 0x00000000;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
        *(volatile uint32_t *) *BIOS_MESS_POINT = 0x0001f0ff;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
        *(volatile uint16_t *) *BIOS_MESS_POINT = 0x2002;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
        *(volatile uint32_t *) *BIOS_MESS_POINT = 0x0003708d;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
        *(volatile uint16_t *) *BIOS_MESS_POINT = 0x0007;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);

        // Add additional dynamic text
        for (int8_t byte_idx = 3; byte_idx >= 0; byte_idx--) {
            uint8_t byte = (uint8_t) (memory_location >> (byte_idx * 8));
            uint8_t hi, lo;
            _convert_hex_to_fix_ascii(byte, &hi, &lo);

            *(volatile uint16_t *) *BIOS_MESS_POINT = ((uint16_t) hi << 8) | lo;
            *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
        }

        // Add space
        *(volatile uint16_t *) *BIOS_MESS_POINT = 0x2020;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);

        // Add expected value
        for (int8_t byte_idx = 1; byte_idx >= 0; byte_idx--) {
            uint8_t byte = (uint8_t) (expected >> (byte_idx * 8));
            uint8_t hi, lo;
            _convert_hex_to_fix_ascii(byte, &hi, &lo);

            *(volatile uint16_t *) *BIOS_MESS_POINT = ((uint16_t) hi << 8) | lo;
            *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
        }

        // Add space
        *(volatile uint16_t *) *BIOS_MESS_POINT = 0x2020;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);

        // Add actual value
        for (int8_t byte_idx = 1; byte_idx >= 0; byte_idx--) {
            uint8_t byte = (uint8_t) (actual >> (byte_idx * 8));
            uint8_t hi, lo;
            _convert_hex_to_fix_ascii(byte, &hi, &lo);

            *(volatile uint16_t *) *BIOS_MESS_POINT = ((uint16_t) hi << 8) | lo;
            *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint16_t);
        }

        *(volatile uint32_t *) *BIOS_MESS_POINT = 0x20ff0000;
        *BIOS_MESS_POINT = *BIOS_MESS_POINT + sizeof(uint32_t);
    }

    *BIOS_MESS_BUSY = 0;
    
    _mess_out();

    *IRQ_ACK = 0x07;
    while(1) {
        *WATCHDOG = 0xFF;
    }
}

void _convert_hex_to_fix_ascii(uint8_t input, uint8_t *output_hi, uint8_t *output_lo) {
    *output_hi = (input & 0xF0) >> 4;
    if (*output_hi < 0x0A) {
        *output_hi += 0x30;
    } else {
        *output_hi += 0x37;
    }
    
    *output_lo = input & 0x0F;
    if (*output_lo < 0x0A) {
        *output_lo += 0x30;
    } else {
        *output_lo += 0x37;
    }
}