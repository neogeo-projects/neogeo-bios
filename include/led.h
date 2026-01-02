#ifndef _LED_H
#define _LED_H

#include <stdint.h>

#define LED_LATCH_LED1      0xEF
#define LED_LATCH_LED2      0xDF
#define LED_LATCH_MARQUEE   0x7F

void set_led_data(uint8_t led_latch, uint8_t value);

#endif // _LED_H