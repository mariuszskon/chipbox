#include "vm.h"
#include "chipbox_sdl.h"
#include <string.h>

int chipbox_vm_step(struct chipbox_chip8_state* state, unsigned long *last_timer_change_time) {
    dbyte opcode;
    dbyte log_PC = state->PC;
    unsigned long elapsed;
    int eval_result;
    opcode = chipbox_cpu_get_opcode(state);
    if (state->log_level != CHIPBOX_LOG_LEVEL_NONE) {
        chipbox_print_log(state, log_PC, opcode);
    }
    if (state->log_level == CHIPBOX_LOG_LEVEL_ERROR) {
        return 0;
    } else {
        eval_result = chipbox_cpu_eval_opcode(state, opcode);
        chipbox_print_log(state, log_PC, opcode);
        elapsed = SDL_GetTicks() - *last_timer_change_time;
        if (elapsed >= CHIPBOX_TIMER_DEC_INTERVAL) {
            if (state->DT > 0) {
                state->DT -= elapsed / CHIPBOX_TIMER_DEC_INTERVAL;
            }
            if (state->ST > 0) {
                state->ST -= elapsed / CHIPBOX_TIMER_DEC_INTERVAL;
            }
            *last_timer_change_time += elapsed;
        }
        return eval_result;
    }
}

void chipbox_vm_update_input(struct chipbox_chip8_state *state) {
    /* assumes events have been polled already */
    /*
      original keyboard:
      1 2 3 C
      4 5 6 D
      7 8 9 E
      A 0 B F
      mapped to:
      1 2 3 4
      Q W E R
      A S D F
      Z X C V
    */
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    /* scancodes for physical position */
    state->input[0x1] = keys[SDL_SCANCODE_1];
    state->input[0x2] = keys[SDL_SCANCODE_2];
    state->input[0x3] = keys[SDL_SCANCODE_3];
    state->input[0xC] = keys[SDL_SCANCODE_4];

    state->input[0x4] = keys[SDL_SCANCODE_Q];
    state->input[0x5] = keys[SDL_SCANCODE_W];
    state->input[0x6] = keys[SDL_SCANCODE_E];
    state->input[0xD] = keys[SDL_SCANCODE_R];

    state->input[0x7] = keys[SDL_SCANCODE_A];
    state->input[0x8] = keys[SDL_SCANCODE_S];
    state->input[0x9] = keys[SDL_SCANCODE_D];
    state->input[0xE] = keys[SDL_SCANCODE_F];

    state->input[0xA] = keys[SDL_SCANCODE_Z];
    state->input[0x0] = keys[SDL_SCANCODE_X];
    state->input[0xB] = keys[SDL_SCANCODE_C];
    state->input[0xF] = keys[SDL_SCANCODE_V];
}

void chipbox_print_log(struct chipbox_chip8_state* state, dbyte PC, dbyte opcode) {
    char level[CHIPBOX_MAX_LOG_LEVEL_LENGTH], message[CHIPBOX_MAX_LOG_MSG_LENGTH];
    if (state->log_level == CHIPBOX_LOG_LEVEL_NONE) {
        return;
    }

    switch (state->log_level) {
        case CHIPBOX_LOG_LEVEL_ERROR:
            strcpy(level, "ERROR");
            break;
        case CHIPBOX_LOG_LEVEL_WARN:
            strcpy(level, "WARN");
            break;
        case CHIPBOX_LOG_LEVEL_INFO:
            strcpy(level, "INFO");
            break;
    }

    switch (state->log_msg) {
        case CHIPBOX_LOG_UNIMPL:
            strcpy(message, "Unimplemented");
            break;
        case CHIPBOX_LOG_IMPL_DEFINED:
            strcpy(message, "May be implementation defined");
            break;
        case CHIPBOX_LOG_UNSAFE:
            strcpy(message, "May not work correctly on classic implementation (unsafe)");
            break;
        case CHIPBOX_LOG_ILLEGAL:
            strcpy(message, "Illegal operation");
            break;
        case CHIPBOX_LOG_RANGE:
            strcpy(message, "Input was out of range");
            break;
        case CHIPBOX_LOG_WAIT:
            strcpy(message, "Waiting on input");
            break;
    }

    fprintf(stderr, "%s @ 0x%04X (%04X): %s\n", level, PC, opcode, message);
}
