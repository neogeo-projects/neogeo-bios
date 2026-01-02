#include "bios.h"
#include "utils.h"

#include "led.h"

void set_led_data(uint8_t led_latch, uint8_t value) {
    *REG_LEDLATCHES = 0xFF;
    nop_loop(36);
    *REG_LEDDATA = ~value;          // Data is inverted
    *REG_LEDLATCHES = led_latch;
    nop_loop(36);
    *REG_LEDLATCHES = 0xFF;
}