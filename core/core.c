#include "core.h"
#include "fonts.h"
#include "log.h"

struct chipbox_chip8_state chipbox_init_state() {
    struct chipbox_chip8_state state;
    int i, j;

    /* initialise screen */
    for (i = 0; i < CHIPBOX_SCREEN_HEIGHT; i++) {
        for (j = 0; j < CHIPBOX_SCREEN_WIDTH_BYTES; j++) {
            state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * i + j] = 0;
        }
    }

    /* load fonts into memory */
    for (i = 0; i < CHIPBOX_FONT_TOTAL_BYTES; i++) {
        state.memory[CHIPBOX_PROGRAM_START - CHIPBOX_FONT_TOTAL_BYTES + i] = chipbox_font[i];
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

    /* set the default compatibiltiy mode */
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_DEFAULT;

    /* set 'none' log level for current message */
    state.log_level = CHIPBOX_LOG_LEVEL_NONE;

    /* seed should be changed every runtime using chipbox_cpu_srand */
    state.seed = 0;

    return state;
}
