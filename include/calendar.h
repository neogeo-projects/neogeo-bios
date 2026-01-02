#ifndef _CALENDAR_H
#define _CALENDAR_H

#include <stdint.h>

#define RTC_CTRL_DATA   0x01  // Bit 0 = Data in
#define RTC_CTRL_CLK    0x02  // Bit 1 = Clock
#define RTC_CTRL_STB    0x04  // Bit 2 = Strobe

#define RTC_CMD_HOLD        0x00    // Hold shift register, let the counters run again
#define RTC_CMD_SHIFT       0x01    // Shift the 48‑bit data register
#define RTC_CMD_TIME_SET    0x02    // Copy 48‑bit shift register into the counters and hold it
#define RTC_CMD_TIME_READ   0x03    // Copy counters to 48‑bit shift register, then hold. Use 0x1 afterwards to shift the data out.

void _read_calendar();
void _setup_calendar();
uint8_t read_rtc_byte();
void write_rtc_byte(uint8_t byte);
void write_rtc_command(uint8_t command);

#endif // _CALENDAR_H