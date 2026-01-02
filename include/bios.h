#ifndef _BIOS_H
#define _BIOS_H

#include <stdint.h>

#define GAME_ENTRY       ((volatile uint16_t *) 0x100000)
#define WATCHDOG         ((volatile uint8_t *)  0x300001)
#define REG_DIPSW        ((volatile uint8_t *)  0x300001) // Hardware dips are active Low
#define LSPCMODE         ((volatile uint16_t *) 0x3C0006)
#define IRQ_ACK          ((volatile uint8_t *)  0x3C000C)

// MVS Memory Map
#define WORK_RAM         ((volatile uint16_t *) 0x100000) // 0x100000 - 0x10F2FF
#define PALETTE_RAM      ((volatile uint16_t *) 0x400000) // 0x400000 - 0x401FFF
#define MEMCARD          ((volatile uint16_t *) 0x800000) // 0x800000 - 0xBFFFFF
#define SYSTEM_ROM       ((volatile uint16_t *) 0xC00000) // 0xC00000 - 0xC1FFFF
#define BACKUP_RAM       ((volatile uint16_t *) 0xD00000) // 0xD00000 - 0xD0FFFF

// I/O registers
#define REG_P1CNT        ((volatile uint8_t *) 0x300000)
#define REG_SYSTYPE      ((volatile uint8_t *) 0x300081)
#define REG_SOUND        ((volatile uint8_t *) 0x320000)
#define REG_STATUS_A     ((volatile uint8_t *) 0x320001)
#define REG_P2CNT        ((volatile uint8_t *) 0x340000)
#define REG_STATUS_B     ((volatile uint8_t *) 0x380000)
#define REG_POUTPUT      ((volatile uint8_t *) 0x380001)
#define REG_CRDBANK      ((volatile uint8_t *) 0x380011)
#define REG_SLOT         ((volatile uint8_t *) 0x380021)  // Slot selection for multi-slot boards
#define REG_LEDLATCHES   ((volatile uint8_t *) 0x380031)
#define REG_LEDDATA      ((volatile uint8_t *) 0x380041)
#define REG_RTCCTRL      ((volatile uint8_t *) 0x380051)
#define REG_RESETCL1     ((volatile uint8_t *) 0x380065)
#define REG_RESETCL2     ((volatile uint8_t *) 0x380067)

// System registers
#define REG_NOSHADOW     ((volatile uint8_t *) 0x3A0001)  // Normal video output
#define REG_SHADOW       ((volatile uint8_t *) 0x3A0011)  // Darken video output
#define REG_SWPBIOS      ((volatile uint8_t *) 0x3A0003)  // BIOS vector table
#define REG_SWPROM       ((volatile uint8_t *) 0x3A0013)  // Use the cart's vector table
#define REG_CRDUNLOCK1   ((volatile uint8_t *) 0x3A0005)  // Enable writes to memory card (unused in CD systems)
#define REG_CRDLOCK1     ((volatile uint8_t *) 0x3A0015)  // Disable writes to memory card (unused in CD systems)
#define REG_CRDLOCK2     ((volatile uint8_t *) 0x3A0007)  // Disable writes to memory card (unused in CD systems)
#define REG_CRDUNLOCK2   ((volatile uint8_t *) 0x3A0017)  // Enable writes to memory card (unused in CD systems)
#define REG_CRDREGSEL    ((volatile uint8_t *) 0x3A0009)  // Enable "Register select" for memory card
#define REG_CRDNORMAL    ((volatile uint8_t *) 0x3A0019)  // Disable "Register select" for memory card
#define REG_BRDFIX       ((volatile uint8_t *) 0x3A000B)  // Use the embedded SFIX ROM and SM1 ROM
#define REG_CRTFIX       ((volatile uint8_t *) 0x3A001B)  // Use the cart's S ROM and M1 ROM
#define REG_SRAMLOCK     ((volatile uint8_t *) 0x3A000D)  // Write-protects backup RAM (MVS)
#define REG_SRAMUNLOCK   ((volatile uint8_t *) 0x3A001D)  // Unprotects backup RAM (MVS)

#define BACKUP_RAM_LOCK   ((volatile uint8_t *) 0x3A000D)
#define BACKUP_RAM_UNLOCK ((volatile uint8_t *) 0x3A001D)

#define REG_PALBANK1     ((volatile uint8_t *) 0x3A000F)  // Use palette bank 1
#define REG_PALBANK0     ((volatile uint8_t *) 0x3A001F)  // Use palette bank 0

// Video registers
#define REG_VRAMADDR     ((volatile uint16_t *) 0x3C0000)
#define REG_VRAMRW       ((volatile uint16_t *) 0x3C0002)
#define REG_VRAMMOD      ((volatile uint16_t *) 0x3C0004)
#define REG_LSPCMODE     ((volatile uint16_t *) 0x3C0006)
#define REG_TIMERHIGH    ((volatile uint16_t *) 0x3C0008)
#define REG_TIMERLOW     ((volatile uint16_t *) 0x3C000A)
#define REG_IRQACK       ((volatile uint16_t *) 0x3C000C)
#define REG_TIMERSTOP    ((volatile uint16_t *) 0x3C000E)

#define BIOS_CURRENT_SLOT_LED       ((volatile uint8_t *) 0x10FCEC)

// Temporary storage for NGH during boot
#define BIOS_NGH_BLOCK              ((volatile SlotEntry*) 0x10FD00)
#define BIOS_SLOT_BOOKKEEP_DATE(n)  ((volatile uint32_t*) (0x10FD20 + ((n) * 4)))      // n=0‑7, YYMMDDdd 
#define MAX_SLOTS                   8

#define BIOS_SYSTEM_MODE            ((volatile uint8_t *) 0x10FD80)    // 0:BIOS wants vblank (system mode), 0x80:Ok to use vblank (game mode)
/*
Status code for the system_return function.
0 = Initialize the game
3 = Game over
4 = Next slot, incremental
5 = Next slot, decremental
*/
#define BIOS_SYSRET_STATUS      ((volatile uint8_t *) 0x10FD81)
#define BIOS_MVS_FLAG           ((volatile uint8_t *) 0x10FD82)    // 0:AES, 1:MVS
#define BIOS_COUNTRY_CODE       ((volatile uint8_t *) 0x10FD83)    // 0x00 = Japan, 0x01 = USA, 0x02 = Europe
#define BIOS_GAME_DIP           ((volatile uint8_t *) 0x10FD84)
#define BIOS_GAME_DIP_PTR       0x10FD84
#define BIOS_PLAYER_MOD1        ((volatile uint8_t *) 0x10FDB6)
#define BIOS_PLAYER_MOD2        ((volatile uint8_t *) 0x10FDB7)
#define BIOS_PLAYER_MOD3        ((volatile uint8_t *) 0x10FDB8)
#define BIOS_PLAYER_MOD4        ((volatile uint8_t *) 0x10FDB9)
/*
BIOS asks game for this state when calling USER():
0:Init, 1:Boot animation, 2:Demo, 3:Title
*/
#define BIOS_USER_REQUEST       ((volatile uint8_t *) 0x10FDAE)
// BIOS_USER_REQUEST values
#define USER_REQUEST_INIT       0x00
#define USER_REQUEST_EYECATCH   0x01
#define USER_REQUEST_DEMO       0x02
#define USER_REQUEST_TITLE      0x03
/*
Game tells the BIOS where it is:
0:Init/Boot animation, 1:Title/Demo, 2:Game
*/
#define BIOS_USER_MODE          ((volatile uint8_t *) 0x10FDAF)

// Arcade panel / Joypad registers
#define A_BUTTON                0x10
#define B_BUTTON                0x20
#define C_BUTTON                0x40
#define D_BUTTON                0x80
#define P1_START                0x01    //select P4, Start P4, Select P3, Start P3, Select P2, Start P2, Select P1, Start P1
#define P1_SELECT               0x02
#define BIOS_P1STATUS           ((volatile uint8_t *) 0x10FD94)
#define BIOS_P1PREVIOUS         ((volatile uint8_t *) 0x10FD95)
#define BIOS_P1CURRENT          ((volatile uint8_t *) 0x10FD96)
#define BIOS_P1CHANGE           ((volatile uint8_t *) 0x10FD97)    // DCBA Right Left Down Up (positive logic)
#define BIOS_P1REPEAT           ((volatile uint8_t *) 0x10FD98)
#define BIOS_P1TIMER            ((volatile int8_t *)  0x10FD99)

#define BIOS_P2STATUS           ((volatile uint8_t *) 0x10FD9A)
#define BIOS_P2PREVIOUS         ((volatile uint8_t *) 0x10FD9B)
#define BIOS_P2CURRENT          ((volatile uint8_t *) 0x10FD9C)
#define BIOS_P2CHANGE           ((volatile uint8_t *) 0x10FD9D)
#define BIOS_P2REPEAT           ((volatile uint8_t *) 0x10FD9E)
#define BIOS_P2TIMER            ((volatile uint8_t *) 0x10FD9F)

#define BIOS_P3STATUS           ((volatile uint8_t *) 0x10FDA0)
#define BIOS_P3PREVIOUS         ((volatile uint8_t *) 0x10FDA1)
#define BIOS_P3CURRENT          ((volatile uint8_t *) 0x10FDA2)
#define BIOS_P3CHANGE           ((volatile uint8_t *) 0x10FDA3)
#define BIOS_P3REPEAT           ((volatile uint8_t *) 0x10FDA4)
#define BIOS_P3TIMER            ((volatile uint8_t *) 0x10FDA5)

#define BIOS_P4STATUS           ((volatile uint8_t *) 0x10FDA6)
#define BIOS_P4PREVIOUS         ((volatile uint8_t *) 0x10FDA7)
#define BIOS_P4CURRENT          ((volatile uint8_t *) 0x10FDA8)
#define BIOS_P4CHANGE           ((volatile uint8_t *) 0x10FDA9)
#define BIOS_P4REPEAT           ((volatile uint8_t *) 0x10FDAA)
#define BIOS_P4TIMER            ((volatile uint8_t *) 0x10FDAB)

#define BIOS_STATCURRENT        ((volatile uint8_t *) 0x10FDAC)
#define BIOS_STATCHANGE         ((volatile uint8_t *) 0x10FDAD)
#define BIOS_CREDIT_DEC         ((volatile uint8_t *) 0x10FDB0)
#define BIOS_P1CREDIT_DEC       ((volatile uint8_t *) 0x10FDB0)
#define BIOS_P2CREDIT_DEC       ((volatile uint8_t *) 0x10FDB1)
#define BIOS_P3CREDIT_DEC       ((volatile uint8_t *) 0x10FDB2)
#define BIOS_P4CREDIT_DEC       ((volatile uint8_t *) 0x10FDB3)
#define BIOS_START_FLAG         ((volatile uint8_t *) 0x10FDB4)    // Signal and control to PLAYER_START-rutine

#define BIOS_PLAYER_MOD_START   ((volatile uint32_t *) 0x10FDBA)
#define BIOS_PLAYER_MOD1_START  ((volatile uint8_t *)  0x10FDBA)
#define BIOS_PLAYER_MOD2_START  ((volatile uint8_t *)  0x10FDBB)
#define BIOS_PLAYER_MOD3_START  ((volatile uint8_t *)  0x10FDBC)
#define BIOS_PLAYER_MOD4_START  ((volatile uint8_t *)  0x10FDBD)

// Memory card registers
#define BIOS_CARD_COMMAND       ((volatile uint8_t *)  0x10FDC4)
#define BIOS_CARD_MODE          ((volatile uint8_t *)  0x10FDC5)
#define BIOS_CARD_ANSWER        ((volatile uint8_t *)  0x10FDC6)
#define BIOS_CARD_START         ((volatile uint32_t *) 0x10FDC8)
#define BIOS_CARD_SIZE          ((volatile uint16_t *) 0x10FDCC)
#define BIOS_CARD_NGH           ((volatile uint16_t *) 0x10FDCE)
#define BIOS_CARD_SUB           ((volatile uint16_t *) 0x10FDD0)

// Memory card commands
#define CARD_COMMAND_FORMAT         0
#define CARD_COMMAND_DATA_SEARCH    1
#define CARD_COMMAND_DATA_LOAD      2
#define CARD_COMMAND_DATA_SAVE      3
#define CARD_COMMAND_DATA_DELETE    4
#define CARD_COMMAND_DATA_TITLE     5
#define CARD_COMMAND_USERNAME_SAVE  6
#define CARD_COMMAND_USERNAME_LOAD  7

// Memory card answers
#define CARD_ANSWER_NOT_INSERTED    0x80
#define CARD_ANSWER_NOT_FORMATED    0x81
#define CARD_ANSWER_DATA_MISSING    0x82
#define CARD_ANSWER_FAT_ERROR       0x83
#define CARD_ANSWER_DATA_FULL       0x84
#define CARD_ANSWER_WRITE_DISABLED  0x85

// Calendar
#define BIOS_DATE(n)    ((volatile uint8_t *) (0x10FDD2 + (n)))
#define BIOS_DATE_PTR   0x10FDD2
#define BIOS_YEAR       ((volatile uint8_t *) 0x10FDD2)
#define BIOS_MONTH      ((volatile uint8_t *) 0x10FDD3)
#define BIOS_DAY        ((volatile uint8_t *) 0x10FDD4)
#define BIOS_WEEKDAY    ((volatile uint8_t *) 0x10FDD5)
#define BIOS_CLOCK_PTR  0x10FDD6
#define BIOS_HOUR       ((volatile uint8_t *) 0x10FDD6)
#define BIOS_MINUTE     ((volatile uint8_t *) 0x10FDD7)
#define BIOS_SECOND     ((volatile uint8_t *) 0x10FDD8)

#define BIOS_COMPULSION_TIMER       ((volatile uint8_t *) 0x10FDDA)
#define BIOS_COMPULSION_FRAME_TIMER ((volatile uint8_t *) 0x10FDDB)

#define BIOS_DEV_MODE            ((volatile uint64_t *) 0x10FE80)
#define BIOS_FRAME_COUNTER       ((volatile uint32_t *) 0x10FE88)
#define BIOS_FRAME_COUNTER_LOW   ((volatile uint8_t *)  0x10FE8B)
#define BIOS_VBLANK_CLEAR        ((volatile uint8_t *)  0x10FE8C)
#define BIOS_SWPMODE             ((volatile uint8_t *)  0x10FE8D)  // REG_SWPBIOS (0xFF) vs REG_SWPROM (0x00)

#define BIOS_BRAM_USED           ((volatile uint8_t *)  0x10FEBF)  // 0:Backup RAM not currently used, 1:Currently used
#define BIOS_Z80_BUSY            ((volatile uint8_t *)  0x10FEDB)  // 1 = busy
#define BIOS_STATCURRENT_RAW     ((volatile uint8_t *)  0x10FEDC)  // Select P4, Start P4, Select P3, Start P3, Select P2, Start P2, Select P1, Start P1 (positive logic)
#define BIOS_STATCHANGE_RAW      ((volatile uint8_t *)  0x10FEDD)  // Select P4, Start P4, Select P3, Start P3, Select P2, Start P2, Select P1, Start P1 (positive logic)
#define BIOS_NEXT_GAME_ROTATE    ((volatile uint8_t *)  0x10FEE0)
#define BIOS_FRAME_SKIP          ((volatile uint8_t *)  0x10FEE1)
// 0x10FEE2 skip sound?
#define BIOS_INT1_SKIP           ((volatile uint8_t *)  0x10FEE3)
#define BIOS_INT1_FRAME_COUNTER  ((volatile uint8_t *)  0x10FEE4)

// SYSTEM registers
#define SROM_MVS_FLAG            ((volatile uint8_t *)  0xC00400)   // 0=AES, 0x80=MVS
#define SROM_COUNTRY_CODE        ((volatile uint8_t *)  0xC00401)   // 0x00 = Japan, 0x01 = USA, 0x02 = Europe

// ROM registers
#define ROM_HEADER_PTR           ((volatile char     *) 0x000100)
#define ROM_NGH_NUMBER           ((volatile uint16_t *) 0x000108)   // The game's identifying number, used for memory card saves and MVS bookkeeping.
#define ROM_PROGRAM_SIZE         ((volatile uint32_t *) 0x00010A)   // The size of the program (in bytes).
#define ROM_BACKUP_RAM_PTR       ((volatile uint32_t *) 0x00010E)   // Points to a location in user RAM, used on MVS for saving backup data. (The first two bytes are used for debug dipswitches.
#define ROM_GAME_SAVE_SIZE       ((volatile uint16_t *) 0x000112)   // Size of the game's save size (in bytes).
#define ROM_EYECATCH_FLAG        ((volatile uint8_t  *) 0x000114)   // Determines how/if the BIOS plays the eyecatcher sequence. (0=handled by BIOS; 1=handled by game; 2=don't show)
#define ROM_EYECATCH_SPRITE_BANK ((volatile uint8_t  *) 0x000115)   // Defines the upper 8 bits of the tile number for the eyecatcher, if handled by the BIOS.
#define ROM_SOFT_DIP_TABLE       ((volatile uint32_t *) 0x000116)
#define ROM_JPN_SOFT_DIP         ((volatile uint32_t *) 0x000116)
#define ROM_USA_SOFT_DIP         ((volatile uint32_t *) 0x00011A)
#define ROM_EU_SOFT_DIP          ((volatile uint32_t *) 0x00011E)
#define ROM_SECURITY_CODE_PTR    (*(volatile uint32_t*) 0x000182)

static const uint8_t security_bytes [] = {
    0x76, 0x00, 0x4A, 0x6D, 0x0A, 0x14, 0x66, 0x00,
    0x00, 0x3C, 0x20, 0x6D, 0x0A, 0x04, 0x3E, 0x2D,
    0x0A, 0x08, 0x13, 0xC0, 0x00, 0x30, 0x00, 0x01,
    0x32, 0x10, 0x0C, 0x01, 0x00, 0xFF, 0x67, 0x1A,
    0x30, 0x28, 0x00, 0x02, 0xB0, 0x2D, 0x0A, 0xCE,
    0x66, 0x10, 0x30, 0x28, 0x00, 0x04, 0xB0, 0x2D,
    0x0A, 0xCF, 0x66, 0x06, 0xB2, 0x2D, 0x0A, 0xD0,
    0x67, 0x08, 0x50, 0x88, 0x51, 0xCF, 0xFF, 0xD4,
    0x36, 0x07, 0x4E, 0x75, 0x20, 0x6D, 0x0A, 0x04,
    0x3E, 0x2D, 0x0A, 0x08, 0x32, 0x10, 0xE0, 0x49,
    0x0C, 0x01, 0x00, 0xFF, 0x67, 0x1A, 0x30, 0x10,
    0xB0, 0x2D, 0x0A, 0xCE, 0x66, 0x12, 0x30, 0x28,
    0x00, 0x02, 0xE0, 0x48, 0xB0, 0x2D, 0x0A, 0xCF,
    0x66, 0x06, 0xB2, 0x2D, 0x0A, 0xD0, 0x67, 0x08,
    0x58, 0x88, 0x51, 0xCF, 0xFF, 0xD8, 0x36, 0x07,
    0x4E, 0x75
};

// Jump rutines
typedef void (*subr_fn_t)(void);
#define SUBR_CART_USER           ((subr_fn_t)0x000122)    // This sub-function is expected to call system_return() instead of returning
#define SUBR_CART_PLAYER_START   ((subr_fn_t)0x000128)    // This sub-function is expected to return(RTS) as normal, with the BIOS_START_FLAG updated with 'accepted' player.
#define SUBR_CART_DEMO_END       ((subr_fn_t)0x00012E)    // This sub-function is expected to return(RTS) as normal
#define SUBR_CART_COIN_SOUND     ((subr_fn_t)0x000134)    // This sub-function is expected to return(RTS) as normal

// Regions
#define BIOS_COUNTRY_JAPAN       0
#define BIOS_COUNTRY_USA         1
#define BIOS_COUNTRY_EUROPE      2

// In-game menu values
#define BIOS_GAME_MENU_START_POSITION   0x710a
#define BIOS_GAME_MENU_ROW_SIZE         12
#define BIOS_GAME_MENU_COLUMN_SIZE      24
#define BIOS_GAME_MENU_SPRITE_BACKUP    ((volatile uint16_t *) 0x10FA00)
#define BIOS_GAME_MENU                  ((volatile uint8_t *)  0x10FE90) // 0 = Closed/Hidden, 1 = Open/Visible
#define GAME_MENU_HIDDEN                0
#define GAME_MENU_VISIBLE               1
#define BIOS_GAME_MENU_TOGGLE           ((volatile uint8_t *)  0x10FE92)
#define BIOS_GAME_LSPCMODE              ((volatile uint16_t *) 0x10FE94)
#define BIOS_GAME_PALETTE_1             ((volatile uint32_t *) 0x10FE98)
//#define BIOS_GAME_PALETTE_2             ((volatile uint16_t *) 0x10FE94)

static const char _rom_header_str[] = "NEO-GEO";

void start_game();
void set_default_values();
void enable_slot(uint8_t slot);
void change_slot_incremental();
void change_slot_decremental();
uint8_t validate_security_code();

#endif // _BIOS_H
