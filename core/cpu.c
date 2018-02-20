#include "cpu.h"

int chipbox_cpu_load_program(struct chipbox_chip8_state *state, byte *program_data, size_t size) {
    if (size > CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START) {
        /* we cannot fit the program */
        return 0;
    }
    memcpy(&(state->memory[CHIPBOX_PROGRAM_START]), program_data, size);
    state->PC = CHIPBOX_PROGRAM_START;
    return 1;
}

dbyte chipbox_cpu_get_opcode(struct chipbox_chip8_state *state) {
    dbyte opcode = 0;
    opcode = state->memory[state->PC] << 8 | state->memory[state->PC + 1];
    state->PC += 2;
    return opcode;
}

/* chipbox_cpu_opcode_xy: write middle two nybbles into x and y
   this mimics the common notation of using XY in opcodes
   that take different values
   e.g. 0xAXYB, X, Y can be any hexadecimal */
void chipbox_cpu_opcode_xy(dbyte opcode, byte *x, byte *y) {
    *y = (opcode >> 4) & 0x0F;
    *x = (opcode >> 8) & 0x0F;
}

/* chipbox_cpu_eval_opcode: evaluates an opcode by manipulating state
   this is where the bulk of the logic for the chip-8 interpreter/emulator lies */
void chipbox_cpu_eval_opcode(struct chipbox_chip8_state *state, dbyte opcode) {
    /* TODO: evaluate the opcodes! */
}
