#ifndef CHIPBOX_VM_H
#define CHIPBOX_VM_H

#include "../core/core.h"
#include "../core/cpu.h"

int chipbox_vm_step(struct chipbox_chip8_state* state);
void chipbox_print_log(struct chipbox_chip8_state* state);

#endif
