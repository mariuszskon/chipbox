#ifndef CHIPBOX_CPU_H
#define CHIPBOX_CPU_H

#include <stdlib.h>
#include <string.h>
#include "core.h"
#include "log.h"

int chipbox_cpu_load_program(struct chipbox_chip8_state *state, byte *program_data, size_t size);
dbyte chipbox_cpu_get_opcode(struct chipbox_chip8_state *state);
void chipbox_cpu_opcode_xy(dbyte opcode, byte *x, byte *y);
int chipbox_cpu_jump(struct chipbox_chip8_state *state, dbyte address);
void chipbox_cpu_srand(struct chipbox_chip8_state *state, dbyte seed);
byte chipbox_cpu_rand();

int chipbox_cpu_eval_opcode(struct chipbox_chip8_state *state, dbyte opcode);

#endif
