#ifndef _BIOS_CALLS_H
#define _BIOS_CALLS_H

#include <stdint.h>

extern void _start(void);
//_start                                // 0xC00402-0xC00432
//extern void system_int1(void);        // 0xC00438
//extern void system_int2(void);        // 0xC0043E
extern void system_return(void);        // 0xC00444
extern void system_io(void);            // 0xC0044A
extern void credit_check(void);         // 0xC00450
extern void credit_down(void);          // 0xC00456
extern void read_calendar(void);        // 0xC0045C
extern void setup_calendar(void);       // 0xC00462
extern void card(void);                 // 0xC00468
extern void card_error(void);           // 0xC0046E
extern void how_to_play(void);          // 0xC00474
extern void checksum_loop(void);        // 0xC0047A
                                        // 0xC00480-0xC004BC
extern void fix_clear(void);            // 0xC004C2
extern void sprite_clear(void);         // 0xC004C8
extern void mess_out(void);             // 0xC004CE
extern void controller_setup(void);     // 0xC004D4

uint8_t _credit_check_player(uint8_t player);
void _credit_down(uint8_t player);

#endif // _BIOS_CALLS_H