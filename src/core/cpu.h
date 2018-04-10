#ifndef CHIPBOX_CPU_H
#define CHIPBOX_CPU_H

#include <stddef.h>
#include "core.h"

int chipbox_cpu_load_program(struct chipbox_chip8_state *state, byte *program_data, size_t size);
dbyte chipbox_cpu_get_opcode(struct chipbox_chip8_state *state);
void chipbox_cpu_opcode_xy(dbyte opcode, byte *x, byte *y);
int chipbox_cpu_jump(struct chipbox_chip8_state *state, dbyte address);
void chipbox_cpu_srand(struct chipbox_chip8_state *state, dbyte seed);
byte chipbox_cpu_rand();
int chipbox_cpu_validate_address(struct chipbox_chip8_state *state, dbyte address);
byte chipbox_cpu_mem_read(struct chipbox_chip8_state *state, dbyte address);
int chipbox_cpu_mem_write(struct chipbox_chip8_state *state, dbyte address, byte value);
int chipbox_cpu_draw(struct chipbox_chip8_state *state, dbyte opcode);
byte chipbox_cpu_raw_byte_draw(struct chipbox_chip8_state *state, byte draw_byte, byte row, byte byte_col);
int chipbox_cpu_eval_opcode(struct chipbox_chip8_state *state, dbyte opcode);

#endif
