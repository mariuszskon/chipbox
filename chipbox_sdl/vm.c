#include "vm.h"
#include "chipbox_sdl.h"
#include <string.h>

int chipbox_vm_step(struct chipbox_chip8_state* state) {
    dbyte opcode;
    if (!((opcode = chipbox_cpu_get_opcode(state)) && chipbox_cpu_eval_opcode(state, opcode))) {
        chipbox_print_log(state);
        return 0;
    } else {
        chipbox_print_log(state);
        /* TODO: manage DT and ST */
        return 1;
    }
}

void chipbox_print_log(struct chipbox_chip8_state* state) {
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

    fprintf(stderr, "%s @ 0x%04X: %s\n", level, state->PC-2, message);
}
