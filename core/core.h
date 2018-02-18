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

/* number of general purpose registers
 16 means we have V0 to VF */
#define CHIPBOX_GREGISTERS 16

/* Chip-8 has 16 keys for input (hexadecimal) */
#define CHIPBOX_INPUT_KEYS 16

/* programs start at byte 512 (0x200) because of space for
   interpreter on the original platform */
#define CHIPBOX_PROGRAM_START 512

struct chipbox_chip8_state {
    byte screen[CHIPBOX_SCREEN_WIDTH_BYTES * CHIPBOX_SCREEN_HEIGHT];
    byte memory[CHIPBOX_MEMORY_SIZE];
    byte V[CHIPBOX_GREGISTERS];
    dbyte I; /* memory address register */
    byte DT; /* delay timer */
    byte ST; /* sound timer */
    dbyte PC; /* program counter */
    byte SP; /* stack pointer */
    byte input[CHIPBOX_INPUT_KEYS];
};

struct chipbox_chip8_state chipbox_init_state();

#endif
