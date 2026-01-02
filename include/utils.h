#ifndef _UTILS_H
#define _UTILS_H

#include <stdint.h>
#include <stddef.h>

uint8_t to_bcd8(uint8_t v);
uint8_t from_bcd8(uint8_t b);
void nop_loop(uint8_t count);
void wait_for_rtc_pulse_edge(uint8_t *previous_status);
void wait_for_vblank();
void wait_for_z80();
void reset_palettes();
void reset_fix_layer();
void reset_sprites();
void pause_system();
void reset_system();
void *memset(void *ptr, int value, size_t num);
void *memcpy(void *dest, const void *src, size_t num);
uint8_t pages_for_items(volatile uint8_t total_items, uint8_t per_page);

#endif // _UTILS_H