#include "bios.h"
#include "bios_calls.h"

#include "mess_out.h"

void _mess_out() {
    if (*BIOS_MESS_BUSY) {
        return;
    }
    *BIOS_MESS_BUSY = 1;
 
    if ((volatile uint32_t) BIOS_MESS_BUFFER_PTR >= *BIOS_MESS_POINT) {
        *BIOS_MESS_POINT = (volatile uint32_t) BIOS_MESS_BUFFER_PTR;
        *BIOS_MESS_BUSY = 0;
        return;
    }

    MESState s = {0};
    s.sp = -1;

    volatile uint32_t *command_list = (volatile uint32_t *)BIOS_MESS_BUFFER;

    uint8_t is_pointer = 0;

    while(command_list < (uint32_t *)(*BIOS_MESS_POINT)) {
        // If command_list is 0, assume it is an inline command list. Otherwise an address.
        if (0x00000000 != *command_list) {
            s.pc = (uint16_t *) *command_list;  // Assign pc to the address of the next command
            command_list++;                     // Increase the lenght of an address (long)
            is_pointer = 1;
        } else {
            command_list++;                     // Jump ahead the null-pointer (0x00000000 / long)
            s.pc = (uint16_t *) command_list;   // Assign pc to the address next after null-pointer, assumes it is inlin
            is_pointer = 0;
        }
        // Loop through all commands, until command 0x00 is reached.
        while(1) {
            uint8_t command = (uint8_t)(*s.pc & 0x000F);
            *WATCHDOG = 0;
            if (0x00 == command) {
                cmd_00(&s);
                break;
            } else if (0x01 == command) {
                cmd_01(&s);
                continue;
            } else if (0x02 == command) {
                cmd_02(&s);
                continue;
            } else if (0x03 == command) {
                cmd_03(&s);
                continue;
            } else if (0x04 == command) {
                cmd_04(&s);
                continue;
            } else if (0x05 == command) {
                cmd_05(&s);
                continue;
            } else if (0x06 == command) {
                cmd_06(&s);
                continue;                
            } else if (0x07 == command) {
                cmd_07(&s);
                continue;
            } else if (0x08 == command) {
                cmd_08(&s);
                continue;
            } else if (0x09 == command) {
                cmd_09(&s);
                continue;
            } else if (0x0A == command) {
                cmd_0A(&s);
                continue;
            } else if (0x0B == command) {
                cmd_0B(&s);
                continue;
            } else if (0x0C == command) {
                cmd_0C(&s);
                continue;
            } else if (0x0D == command) {
                cmd_0D(&s);
                continue;
            } else {
                // Unsupported command
                goto mess_out_end;
            }
        }
        
        // Set the new command_list according to the previous list-type
        if (!is_pointer) {
            command_list = (uint32_t *)s.pc;    // Assign command_list to be where the previous command list ended
        }
    }
mess_out_end:
    *BIOS_MESS_POINT = (uint32_t) BIOS_MESS_BUFFER_PTR;
    *BIOS_MESS_BUSY = 0;
}

/* Command 0: End of command list */
void cmd_00(MESState* s) {
    s->pc++;
}

/* Command 1: Set data format */
void cmd_01(MESState* s) {
    s->data_code = (int8_t)((s->pc[0] & 0xFF00) >> 8);
    s->data_format = s->pc[1];
    s->pc += 2;
}

/* Command 2: Set auto-increment */
void cmd_02(MESState* s) {
    int8_t value = (int8_t)((s->pc[0] & 0xFF00) >> 8);
    *REG_VRAMMOD = (int16_t)value;
    s->vram_incr = value;
    s->pc++;
}

/* Command 3: Set VRAM address */
void cmd_03(MESState* s) {
    s->vram_addr = s->pc[1];
    *REG_VRAMADDR = s->vram_addr;    
    s->pc += 2;
}

/* Command 4: Set data address */
void cmd_04(MESState* s) {
    s->pc++;
    uint32_t message_address = ((uint32_t)s->pc[0] << 16) | s->pc[1];
    s->pc += 2;
    s->previous_address = message_address;
    *BIOS_MESS_TEMP = message_address;
    cmd_06(s);
    s->pc--;    // fix for the cmd_06 increment
}

/* Command 5: Add to current VRAM address */
void cmd_05(MESState* s) {
    s->vram_addr += s->pc[1];
    *REG_VRAMADDR = s->vram_addr;
    s->pc += 2;
}

/* Command 6: Resume data output */
void cmd_06(MESState* s) {
    s->pc++;
    const uint32_t message_address = s->previous_address;
    uint16_t *pc = (uint16_t *) message_address;

    if (s->data_code == 0) {      
        const uint16_t upper = s->data_format & 0xFF00;
        const uint8_t end_code = (uint8_t) s->data_format & 0x00FF;

        while (*(uint8_t *)pc != end_code) {
            *REG_VRAMRW = upper | *(uint8_t *)pc;
            pc = (uint16_t *)((uint8_t *)pc + 1);
        }
    } else if (s->data_code == 1) {
        const uint16_t upper = s->data_format & 0xFF00;
        const uint8_t length = (uint8_t) s->data_format & 0x00FF;
        for (uint8_t i = 0; i < length; i++) {
            *REG_VRAMRW = upper | *(uint8_t *)pc;
            pc = (uint16_t *)((uint8_t *)pc + 1);
        }
    } else if (s->data_code == 2) {
        const uint16_t end_code = s->data_format;
        while (*pc != end_code) {        
            *REG_VRAMRW = *pc;           
            pc++;
        }
    } else if (s->data_code == 3) {
        for (uint16_t i = 0; i < s->data_format; i++) {
            *REG_VRAMRW = *pc;           
            pc++; 
        }
    }

    s->previous_address = (uint32_t) pc;
}

/* Command 7: Directly define output data */
void cmd_07(MESState* s) {
    *WATCHDOG = 0;

    s->pc++;
    uint8_t *pc = (uint8_t *) s->pc;

    uint16_t upper = s->data_format & 0xFF00;
    uint8_t end_code = (uint8_t) s->data_format & 0x00FF;

    while (*pc != end_code) {
        *REG_VRAMRW = upper | *pc;
        pc++;
    }
    pc++;
    if ((uintptr_t)pc & 1) {
        pc++;
    }
    s->pc = (uint16_t *) pc;
}

/* Command 8: Write text with 8x16 font */
void cmd_08(MESState* s) {
    *WATCHDOG = 0;

    *REG_VRAMMOD = 0x0020;
    int16_t hi = s->pc[0] & 0xFF00;
    s->pc++;

    // Row 1
    uint8_t *pc = (uint8_t *) s->pc;
    while(*pc != 0xFF) {
        *REG_VRAMRW = hi | *pc;
        pc++;
    }

    hi += 0x100;
    s->vram_addr++;
    *REG_VRAMADDR = s->vram_addr;

    // Row 2
    pc = (uint8_t *) s->pc;
    while(*pc != 0xFF) {
        *REG_VRAMRW = hi | *pc;
        pc++;
    }

    pc++;    
    if ((uintptr_t)pc & 1) {
        pc++;
    }
    s->pc = (uint16_t *) pc;
}

/* Command 9: Write Japanese text */
// TODO: Fix support for Japanese
void cmd_09(MESState* s) {
    while(1) {
        *WATCHDOG = 0x09;
    }
}

/* Command A: Call sub list */
void cmd_0A(MESState* s) {
    uint32_t sub_list = *(uint32_t *)(s->pc + 1);

    s->pc += 3;

    if (CALL_STACK_MAX - 1 <= s->sp) {
        return;
    }
    s->sp += 1;

    s->call_sp[s->sp] = s->pc;
    s->pc = (uint16_t *) sub_list;
}

/* Command B: Return from sub list */
void cmd_0B(MESState* s) {
    s->pc = s->call_sp[s->sp];
    s->sp -= 1;
}

/* Command C: Repeated data output */
void cmd_0C(MESState* s) {
    uint16_t count = s->pc[0] >> 8;
    uint16_t value = s->pc[1];
    for (uint8_t i = 0; i < count; i++) {
        *REG_VRAMRW = value;
    }
    s->pc += 2;
}

/* Command D: Repeat and increment output */
void cmd_0D(MESState* s) {
    uint16_t count = s->pc[0] >> 8;
    uint16_t value = s->pc[1];

    uint16_t hi = value & 0xFF00;
    uint8_t  lo = value & 0x00FF;

    for (uint8_t i = 0; i < count; i++) {
        *REG_VRAMRW = hi | lo;
        lo++;
    }
    s->pc += 2;
}