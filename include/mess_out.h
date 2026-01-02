#ifndef _MESS_OUT_H
#define _MESS_OUT_H

#include <stdint.h>
#include "bios.h"

#define BIOS_MESS_POINT   ((volatile uint32_t *) 0x10FDBE)
#define BIOS_MESS_BUSY    ((volatile  uint8_t *) 0x10FDC2)
#define BIOS_MESS_STACK   ((volatile uint32_t *) 0x10FEC6)
#define BIOS_MESS_BUFFER  ((volatile uint16_t *) 0x10FF00)
#define BIOS_MESS_BUFFER_PTR 0x10FF00

#define BIOS_MESS_TEMP    ((volatile uint32_t *) 0x10FFEE)

#define CALL_STACK_MAX 5

typedef struct {
    volatile uint16_t *pc;                       /* Current command ptr */
    volatile uint16_t *call_sp[CALL_STACK_MAX];  /* sub-list call stack */
    int8_t sp;                                   /* current stack index (-1 = empty) */
    uint8_t data_code;
    uint16_t data_format;
    uint16_t vram_addr;
    uint8_t vram_incr;
    uint32_t previous_address;
    
} MESState;

void cmd_00(MESState*);            /* End list                      */
void cmd_01(MESState*);            /* Set data format               */
void cmd_02(MESState*);            /* Set auto-increment            */
void cmd_03(MESState*);            /* Set VRAM address              */
void cmd_04(MESState*);            /* Set data source address       */
void cmd_05(MESState*);            /* Add to VRAM address           */
void cmd_06(MESState*);            /* Resume data output            */
void cmd_07(MESState*);            /* In-line data (byte/word blob) */
void cmd_08(MESState*);            /* ASCII text (8×16 font)        */
void cmd_09(MESState*);            /* Japanese text                 */
void cmd_0A(MESState*);            /* Call sub list                 */
void cmd_0B(MESState*);            /* Return from sub list          */
void cmd_0C(MESState*);            /* Repeat one word n times       */
void cmd_0D(MESState*);            /* Incrementing words            */

void _mess_out();

#endif // _MESS_OUT_H