#ifndef CHIPBOX_SDL_DEBUG_H
#define CHIPBOX_SDL_DEBUG_H

#include "core.h"

#define CHIPBOX_DEBUG_LEVEL_NONE 0
#define CHIPBOX_DEBUG_LEVEL_DISASSEMBLE 1
#define CHIPBOX_DEBUG_LEVEL_COUNT      (1 << 1)
#define CHIPBOX_DEBUG_LEVEL_SCREEN     (1 << 2)
#define CHIPBOX_DEBUG_LEVEL_REGISTERS  (1 << 3)
#define CHIPBOX_DEBUG_LEVEL_STACK      (1 << 4)
#define CHIPBOX_DEBUG_LEVEL_INPUT      (1 << 5)
#define CHIPBOX_DEBUG_LEVEL_MEMORY     (1 << 6)
#define CHIPBOX_DEBUG_LEVEL_EVERYTHING 0xFF

#define CHIPBOX_DEBUG_LEVEL_DEFAULT CHIPBOX_DEBUG_LEVEL_NONE

#define CHIPBOX_KEYPAD_COLS 4
#define CHIPBOX_KEYPAD_ROWS 4

#define CHIPBOX_MEMDUMP_COLS 16

void chipbox_print_debug_state_dump(struct chipbox_chip8_state *state, byte level);

#endif
