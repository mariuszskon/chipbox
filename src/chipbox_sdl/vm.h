#ifndef CHIPBOX_VM_H
#define CHIPBOX_VM_H

#include "core.h"

#define CHIPBOX_MAX_LOG_LEVEL_LENGTH 10
#define CHIPBOX_MAX_LOG_MSG_LENGTH 256

int chipbox_vm_step(struct chipbox_chip8_state* state, byte min_log_level);
void chipbox_vm_update_input(struct chipbox_chip8_state *state);
void chipbox_print_log(struct chipbox_chip8_state* state, dbyte PC, dbyte opcode, byte min_log_level);

#endif
