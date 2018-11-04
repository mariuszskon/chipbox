#ifndef CHIPBOX_SDL_DEBUG_H
#define CHIPBOX_SDL_DEBUG_H

#include "core.h"

#define CHIPBOX_DEBUG_LEVEL_NONE 0
#define CHIPBOX_DEBUG_LEVEL_INSTRUCTION 1
#define CHIPBOX_DEBUG_LEVEL_EVERYTHING 2

#define CHIPBOX_DEBUG_LEVEL_DEFAULT CHIPBOX_DEBUG_LEVEL_NONE

#define CHIPBOX_KEYPAD_COLS 4
#define CHIPBOX_KEYPAD_ROWS 4

#define CHIPBOX_MEMDUMP_COLS 16

void chipbox_print_debug_instruction(dbyte pc, dbyte opcode);
void chipbox_print_debug_state_dump(struct chipbox_chip8_state *state);

#endif