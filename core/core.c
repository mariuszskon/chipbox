#include "core.h"

struct chipbox_chip8_state chipbox_init_state() {
    struct chipbox_chip8_state state;
    int i, j;

    /* initialise screen */
    for (i = 0; i < CHIPBOX_SCREEN_HEIGHT; i++) {
        for (j = 0; j < CHIPBOX_SCREEN_WIDTH_BYTES; j++) {
            state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * i + j] = 0;
        }
    }

    /* initialise memory */
    for (i = 0; i < CHIPBOX_MEMORY_SIZE; i++) {
        state.memory[i] = 0;
    }

    /* initialise registers */
    for (i = 0; i < CHIPBOX_GREGISTERS; i++) {
        state.V[i] = 0;
    }
    state.I = 0;
    state.SP = 0;
    state.DT = state.ST = 0;
    state.PC = CHIPBOX_PROGRAM_START;

    for (i = 0; i < CHIPBOX_INPUT_KEYS; i++) {
        state.input[i] = 0;
    }

    return state;
}
