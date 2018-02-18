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

/* chipbox_cpu_opcode_to_nybbles: convert an opcode into nybbles (4 bits)
   nybbles array must be at have at least 4 cells
   nybbles are stored most significant bit first
   i.e. 0xABCD becomes {0xA, 0xB, 0xC, 0xD} so nybbles[0] == 0xA */
void chipbox_cpu_opcode_to_nybbles(dbyte opcode, byte nybbles[]) {
    int i;
    for (i = 0; i < 4; i++) {
        nybbles[3-i] = opcode & 0xF;
        opcode >>= 4;
    }
}
