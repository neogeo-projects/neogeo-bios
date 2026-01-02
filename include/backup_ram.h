#ifndef _BACKUP_RAM_H
#define _BACKUP_RAM_H

#include <stdint.h>

#define BRAM_UNUSED_START                   ((volatile uint8_t *)0xD00000)  // 16 byte – unused, fill with 0x00 
#define BRAM_UNUSED_SIZE                    0x10
#define BRAM_SIGNATURE_STR                  ((volatile char    *)0xD00010)  // "BACKUP RAM OK !\x80" 
#define BRAM_SIGNATURE_SIZE                 0x10
#define BRAM_COIN_STATUS_PREVIOUS           ((volatile uint8_t *)0xD00020)  // REG_STATUS_A, previous frame 
#define BRAM_COIN_STATUS_CURRENT            ((volatile uint8_t *)0xD00021)  // REG_STATUS_A, current frame  
#define BRAM_COIN_STATUS_CHANGE             ((volatile uint16_t*)0xD00022)  // temp for rising‑edge detect  
#define BRAM_COIN_COUNTER_INTERNAL          ((volatile uint16_t*)0xD00024)  // internal coin counter
#define BRAM_CREDIT                         ((volatile uint8_t *)0xD00034)  // Start of credit
#define BRAM_CREDIT_P1                      ((volatile uint8_t *)0xD00034)  // P1
#define BRAM_CREDIT_P2                      ((volatile uint8_t *)0xD00035)  // P2

// Cabinet soft dip settings
#define BRAM_DIP_COIN_P1_NEED               ((volatile uint8_t *)0xD0003A)  // coins per credit P1
#define BRAM_DIP_COIN_P1_GIVE               ((volatile uint8_t *)0xD0003B)  // credits given P1
#define BRAM_DIP_COIN_P2_NEED               ((volatile uint8_t *)0xD0003C)  // coins per credit P2
#define BRAM_DIP_COIN_P2_GIVE               ((volatile uint8_t *)0xD0003D)  // credits given P2
#define BRAM_DIP_0x3E                       ((volatile uint8_t *)0xD0003E)  // Mystery 0x3E, never used in original BIOS
#define BRAM_DIP_0x3F                       ((volatile uint8_t *)0xD0003F)  // Mystery 0x3F, never used in original BIOS
#define BRAM_DIP_0x40                       ((volatile uint8_t *)0xD00040)  // Mystery 0x40, never used in original BIOS
#define BRAM_DIP_0x41                       ((volatile uint8_t *)0xD00041)  // Mystery 0x41, never used in original BIOS
#define BRAM_DIP_GAME_SELECT_FREE           ((volatile uint8_t *)0xD00042)  // Allowed to change game when 0 = credit is present, 1 = free selection
#define BRAM_DIP_GAME_START_FORCE           ((volatile uint8_t *)0xD00043)  // 0 = Enforced compulsion, 1 = Without compulsion
#define BRAM_DIP_COMPULSION_TIMER           ((volatile uint8_t *)0xD00044)  // Seconds (BCD) between coin & start
#define BRAM_DIP_COMPULSION_FRAME_TIMER     ((volatile uint8_t *)0xD00045)  // Compulsion frame counter. Used by game
#define BRAM_DIP_DEMO_SOUND                 ((volatile uint8_t *)0xD00046)  // 0 = Game setting, 1 = No demo sound

#define BRAM_SLOT_COUNT                     ((volatile uint8_t *)0xD00047)  // detected slots (1‑6)         
#define BRAM_PLAY_SECONDS_HEX               ((volatile uint32_t*)0xD00048)  // total secs played            
#define BRAM_PLAY_FRAME_TIMER               ((volatile uint8_t *)0xD0004C)  // frame countdown (0x3C..0x00) 

#define BRAM_FIRST_PLAYABLE_SLOT            ((volatile uint8_t *)0xD00058)

#define BRAM_CLOCK_PTR                      0xD0005A
#define BRAM_CLOCK_FRAMES                   ((volatile uint8_t *)0xD0005A)  // FF 
#define BRAM_CLOCK_SECONDS                  ((volatile uint8_t *)0xD0005B)  // SS 
#define BRAM_CLOCK_MINUTES                  ((volatile uint8_t *)0xD0005C)  // MM 
#define BRAM_CLOCK_HOURS_X4                 ((volatile uint8_t *)0xD0005D)  // HH ×4 
// 0xD0005E - 0xD00103 : Unknown
#define BRAM_DATE_PTR                       0xD00104                        // YYMMDDdd
#define BRAM_DATE_YEAR                      ((volatile uint8_t *)0xD00104)  // YY 
#define BRAM_DATE_MONTH                     ((volatile uint8_t *)0xD00105)  // MM 
#define BRAM_DATE_DAY                       ((volatile uint8_t *)0xD00106)  // DD 
#define BRAM_DATE_DAYOFWEEK                 ((volatile uint8_t *)0xD00107)  // dd (0=Sun) 
#define BRAM_SLOT_SELECTED                  ((volatile uint8_t *)0xD00108)  // 0‑7 
#define BRAM_SLOT_CURSOR                    ((volatile uint8_t *)0xD00109)  // 0-7
#define BRAM_MAGIC_0x23                     ((volatile uint16_t*)0xD00122)  // ??
#define BRAM_MAX_SLOTS                      8

// 0xD00124–0xD00143 : NGH + game‑block IDs per slot
typedef struct {
    uint16_t ngh;     // NGH number (game ID)
    uint16_t block;   // Block for saved game data
} SlotEntry;
#define BRAM_NGH_BLOCK                      ((volatile SlotEntry*) 0xD00124)

// 0xD00144–0xD00163 : book creation time per slot
#define BRAM_SLOT_BOOKKEEP_DATE(n)          ((volatile uint32_t*) (0xD00144 + ((n) * 4)))      // n=0‑7, YYMMDDdd 

// 0xD00164-0xD001A3 : backup of books of orphan games
typedef struct {
    uint16_t ngh;
    uint16_t block;
    uint32_t created;   // YYMMDDdd
} OrphanBookEntry;
#define ORPHAN_BOOKS                        ((volatile OrphanBookEntry*) 0xD00164)

// 0xD00220-0xD0029F : per‑block game soft dip (16 byte each)
/*
    Metal Slug default dipsw

    12 38 04 12 12 02 01 04 03 00
    continue        12      off/on*
    difficulty      38      1,2,3,4*,5,6,7,8
    play time       04      60*,70,80,90
    demo sound      12      off,on*
    play manual     12      off,on*
    blood           02      off*,on
    next page       01      -
    credit/level    04      *on/on,on/off,off/on,off/off
    language        03      *english,portuguese,spanish
                    00
*/
#define BRAM_GAME_DIP_SETTINGS(n)           ((volatile uint8_t *)(0xD00220 + ((n) * 0x10)))   // n=0-7
// 0xD002A0–0xD0031F : per‑block game names (16 byte each)
#define BRAM_GAME_NAME(n)                   ((volatile uint8_t *)(0xD002A0 + ((n) * 0x10)))   // n=0‑7 
// 0xD00320–0xD0831F : 8× game data blocks (4 kB each)
#define BRAM_GAME_BLOCK(n)                  ((volatile uint8_t *)(0xD00320 + ((n) * 0x1000))) // n=0‑7 
#define BRAM_GAME_BLOCK_SIZE                0x1000

// 0xD09BA0–0xD0A19F : bookkeeping records
#define BRAM_BOOKKEEP_COIN_MONTH            ((volatile uint8_t *)0xD09BA0)  // Daily cab coin data. Each entry is $10 byte long. $240 in total. 18 bytes, 31 days?
#define BRAM_BOOKKEEP_PLAY_MONTH            ((volatile uint8_t *)0xD09DE0)  // Daily cab play data. Each entry is $10 byte long. $240 in total. 18 bytes, 31 days?
#define BRAM_BOOKKEEP_COIN_YEAR             ((volatile uint8_t *)0xD0A020)  // Monthly cab coin data ($10 bytes entries * 12). 0xC0 in total.
#define BRAM_BOOKKEEP_PLAY_YEAR             ((volatile uint8_t *)0xD0A0E0)  // Monthly cab play data ($10 bytes entries * 12). 0xC0 in total.  

static const char *_bram_signature_str = "BACKUP RAM OK !\x80";

void reset_backup_ram();
void setup_backup_ram();
void lock_backup_ram();
void unlock_backup_ram();

void load_game_data();
void save_game_data();
void reset_game_data(uint16_t block);

uint16_t find_game_data_block(uint16_t rom_ngh);
uint16_t find_game_data_block_by_name();
uint16_t find_next_available_data_block();

#endif // _BACKUP_RAM_H