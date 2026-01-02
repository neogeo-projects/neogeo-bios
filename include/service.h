#ifndef _SERVICE_H
#define _SERVICE_H

#include <stdint.h>

#define SERVICE_CURSOR              ((volatile int8_t *)  0x10FCDA)
#define SERVICE_CURSOR_MAX          ((volatile uint8_t *) 0x10FCDB)
#define SERVICE_CURSOR_SIDEWAYS     ((volatile int8_t *)  0x10FCDC)
#define SERVICE_CURSOR_PAGE         ((volatile uint8_t *) 0x10FCDD)
#define SERVICE_CURSOR_PAGE_PREV    ((volatile uint8_t *) 0x10FCDE)

#define SERVICE_CALENDAR_CURSOR ((volatile uint8_t *) 0x10FCDA)
#define SERVICE_LED_MARQUEE     ((volatile uint8_t *) 0x10FCDA)
#define SERVICE_LED_COIN        ((volatile uint8_t *) 0x10FCDE)
#define SERVICE_SOUND_CURSOR    ((volatile int8_t *)  0x10FCDE)
#define SERVICE_SOUND_SELECT    ((volatile int8_t *)  0x10FCE2)
#define SOFT_DIPS_GAME_SELECT   ((volatile int8_t *)  0x10FCE2)
#define SERVICE_TEST            ((volatile uint8_t *) 0x10FCE3)

#define NEW_BIOS_DATE(n)        ((volatile uint8_t *) (0x10FCE2 + (n)))
#define NEW_BIOS_YEAR           ((volatile uint8_t *) 0x10FCE2)
#define NEW_BIOS_MONTH          ((volatile uint8_t *) 0x10FCE3)
#define NEW_BIOS_DAY            ((volatile uint8_t *) 0x10FCE4)
#define NEW_BIOS_WEEKDAY        ((volatile uint8_t *) 0x10FCE5)
#define NEW_BIOS_HOUR           ((volatile uint8_t *) 0x10FCE6)
#define NEW_BIOS_MINUTE         ((volatile uint8_t *) 0x10FCE7)
#define NEW_BIOS_SECOND         ((volatile uint8_t *) 0x10FCE8)

#define MENU_BUTTON_FORWARD         0x10
#define MENU_BUTTON_BACKWARD        0x40

// BIOS menu
#define MENU_BIOS_MAIN              8
#define MENU_BIOS_HARDWARE          9
#define MENU_BIOS_HARD_DIPS         10
#define MENU_BIOS_SOFT_DIPS         11
#define MENU_BIOS_BOOK_KEEPING      12
#define MENU_BIOS_CODE_NUMBER       13
#define MENU_BIOS_CALENDAR          14
#define MENU_BIOS_EXIT              15
#define MENU_BIOS_SOFT_DIPS_CABINET 16
#define MENU_BIOS_SOFT_DIPS_GAME    17

// Hardware menu
#define MAX_NUM_MENUS       7
#define MENU_CROSSHATCH     0
#define MENU_COLOR          1
#define MENU_IO             2
#define MENU_SOUND_TEST     3
#define MENU_MEMORY_CARD    4
#define MENU_CLEAR_BACKUP   5
#define MENU_SETUP_CALENDAR 6

void show_bios_menu();
void show_bios_menu_service();
void update_bios_menu_service();
void show_bios_hardware_test();

// Local service functions
volatile uint16_t * _add_large_char(volatile uint16_t *address, uint8_t selected, uint16_t position, uint16_t value);
volatile uint16_t * _add_string(volatile uint16_t *address, uint8_t selected, uint16_t position, const char *string);
void _move_cursor(int8_t menu_items, int8_t side_items, int8_t pages);

static const uint16_t bios_menu [] = {
    0x0003, 0x7145, 
    0x0108, 0x4841, 0x5244, 0x5741, 0x5245, 0x2054, 0x4553, 0x54ff, 
    0x0005, 0x0002, 
    0x0108, 0x5345, 0x5454, 0x494e, 0x4720, 0x5550, 0x2054, 0x4845, 0x2048, 0x4152, 0x4420, 0x4449, 0x50ff, 
    0x0005, 0x0002, 
    0x0108, 0x5345, 0x5454, 0x494e, 0x4720, 0x5550, 0x2054, 0x4845, 0x2053, 0x4f46, 0x5420, 0x4449, 0x50ff, 
    0x0005, 0x0002, 
    0x0108, 0x424f, 0x4f4b, 0x204b, 0x4545, 0x5049, 0x4e47, 0xffff, 
    0x0005, 0x0002, 
    0x0108, 0x5345, 0x5454, 0x494e, 0x4720, 0x5550, 0x2054, 0x4845, 0x2043, 0x4f44, 0x4520, 0x4e55, 0x4d42, 0x4552, 0xffff, 
    0x0005, 0x0002, 
    0x0108, 0x5345, 0x5454, 0x494e, 0x4720, 0x5550, 0x2054, 0x4845, 0x2043, 0x414c, 0x454e, 0x4441, 0x52ff, 
    0x0005, 0x0002, 
    0x0108, 0x4558, 0x4954, 0xffff, 
    0x0003, 0x725a,
    0x0108, 0x2f20, 0x202f, 0x2020, 0xffff, 
    0x0000
};

#endif // _SERVICE_H