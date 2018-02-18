#ifndef CHIPBOX_CPU_H
#define CHIPBOX_CPU_H

#include <string.h>
#include "core.h"

int chipbox_cpu_load_program(struct chipbox_chip8_state *state, byte *program_data, size_t size);
dbyte chipbox_cpu_get_opcode(struct chipbox_chip8_state *state);
void chipbox_cpu_opcode_to_nybbles(dbyte opcode, byte nybbles[]);
void chipbox_cpu_eval_opcode(struct chipbox_chip8_state *state, dbyte opcode);

#endif
