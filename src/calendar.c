
#include "bios.h"
#include "utils.h"

#include "calendar.h"

void _read_calendar() {
    write_rtc_command(0x3);
    nop_loop(24);
    write_rtc_command(0x1);

    *BIOS_SECOND = read_rtc_byte();
    *BIOS_MINUTE = read_rtc_byte();
    *BIOS_HOUR = read_rtc_byte();
    *BIOS_DAY = read_rtc_byte();
    
    uint8_t month_weekday = read_rtc_byte();
    *BIOS_WEEKDAY = month_weekday & 0x0F;
    uint8_t month = month_weekday >> 4;
    if (month >= 10) {
        month += 6;
    }
    *BIOS_MONTH = month;

    *BIOS_YEAR = read_rtc_byte();

    // TODO: Do we need 4 dummy bit read?
    // (void)read_rtc_bits(3); 
}

void _setup_calendar() {
    write_rtc_command(RTC_CMD_SHIFT);

    write_rtc_byte(*BIOS_SECOND);
    write_rtc_byte(*BIOS_MINUTE);
    write_rtc_byte(*BIOS_HOUR);
    write_rtc_byte(*BIOS_DAY);

    uint8_t month_weekday = *BIOS_WEEKDAY & 0x07;
    uint8_t month = *BIOS_MONTH;
    if (month >= 0x10) {
        month -= 0x06;
    }
    month_weekday |= month << 4;

    write_rtc_byte(month_weekday);

    write_rtc_byte(*BIOS_YEAR);

    write_rtc_command(RTC_CMD_TIME_SET);
    write_rtc_command(RTC_CMD_HOLD);
}

uint8_t read_rtc_byte() {
    uint8_t byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t rtc_bit = (*REG_STATUS_A) & 0x80;
        *REG_RTCCTRL = RTC_CTRL_CLK;
        *REG_RTCCTRL = 0x0;
        byte >>= 1;
        byte |= rtc_bit;
    }
    return byte;
}

void write_rtc_byte(uint8_t byte) {
    for (uint8_t i = 0; i < 8; i++) {
        uint8_t bit = byte & 0x01;
        *REG_RTCCTRL = bit;
        *REG_RTCCTRL = bit | RTC_CTRL_CLK;
        *REG_RTCCTRL = bit;
        byte >>= 1;
    }
}

void write_rtc_command(uint8_t command) {
    for (uint8_t i = 0; i < 4; i++) {        
        *WATCHDOG = 0;
        uint8_t write = command;
        write &= 0x1;
        *REG_RTCCTRL = write;
        write |= RTC_CTRL_CLK;
        *REG_RTCCTRL = write;
        write ^= RTC_CTRL_CLK;
        *REG_RTCCTRL = write;
        command >>= 1;
    }
    *REG_RTCCTRL = RTC_CTRL_STB;
    *REG_RTCCTRL = 0x0;
}
