#ifndef CHIPBOX_CORE_H
#define CHIPBOX_CORE_H

typedef unsigned char byte;
typedef unsigned short dbyte; /* double byte - 16 bits */

/* set the dimensions of chip8 screens, in pixels */
#define CHIPBOX_SCREEN_WIDTH_PIXELS 64
#define CHIPBOX_SCREEN_HEIGHT 32
/* and width in bytes (height in bytes not useful for most cases -
   one should always use height in pixels which are rows)
   access row, column of screen with screen[CHIPBOX_SCREEN_WIDTH_BYTES * row + column]
   where a cell is 8 pixels wide and 1 in height
*/
#define CHIPBOX_SCREEN_WIDTH_BYTES (CHIPBOX_SCREEN_WIDTH_PIXELS / 8)

/* Chip-8 has 4KB/4096 bytes of memory */
#define CHIPBOX_MEMORY_SIZE 4 * 1024

/* stack size for this implementation */
#define CHIPBOX_STACK_SIZE 16
/* stack size for COSMIC VIP (classic implementation) */
#define CHIPBOX_STACK_SAFE_SIZE 12

/* number of general purpose registers
 16 means we have V0 to VF */
#define CHIPBOX_GREGISTERS 16

/* Chip-8 has 16 keys for input (hexadecimal) */
#define CHIPBOX_INPUT_KEYS 16

/* programs start at byte 512 (0x200) because of space for
   interpreter on the original platform */
#define CHIPBOX_PROGRAM_START 512

/* the rate, in hz, that the delay and sound timer decrement */
#define CHIPBOX_TIMER_RATE 60

/* it seems that there are two slightly differing standards to CHIP-8
   Cowgod's docs: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
   Mattmik's docs: http://mattmik.com/files/chip8/mastering/chip8.html
   Mattmik's is more accurate (closest to the classic implementation)
   but Cowgod's is very popular amongst interpreter/emulator writers */
#define CHIPBOX_COMPATIBILITY_MODE_COWGOD 0
#define CHIPBOX_COMPATIBILITY_MODE_MATTMIK 1

#define CHIPBOX_COMPATIBILITY_MODE_DEFAULT CHIPBOX_COMPATIBILITY_MODE_MATTMIK

struct chipbox_chip8_state {
    byte screen[CHIPBOX_SCREEN_WIDTH_BYTES * CHIPBOX_SCREEN_HEIGHT];
    byte memory[CHIPBOX_MEMORY_SIZE];
    byte V[CHIPBOX_GREGISTERS];
    dbyte I; /* memory address register */
    byte DT; /* delay timer */
    byte ST; /* sound timer */
    dbyte PC; /* program counter */
    byte SP; /* stack pointer */
    dbyte stack[CHIPBOX_STACK_SIZE];
    byte input[CHIPBOX_INPUT_KEYS];
    byte compat_mode;
    byte log_level; /* level of current message (info, warning, error) */
    byte log_msg; /* message code (see log.h) */
    dbyte seed; /* seed used for random number generation */
    dbyte speed; /* how many instructions executed per second (hz) */
    unsigned long instruction_count; /* how many instructions executed since start */
};

struct chipbox_chip8_state chipbox_init_state(dbyte speed);

#endif
