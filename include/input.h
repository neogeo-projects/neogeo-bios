#ifndef _INPUT_H
#define _INPUT_H

#include <stdint.h>

/*
7	6	5	4	3	    2	    1	    0
D	C	B	A	Right	Left	Down	Up
*/
#define INPUT_P1 ((volatile uint8_t *)0x300000)
#define INPUT_P2 ((volatile uint8_t *)0x340000)

void update_controller_input();
void check_start_button();
void check_change_game();
void check_compulsion_timer();

#endif // _INPUT_H