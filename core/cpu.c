#include "cpu.h"
#include "core.h"
#include "log.h"

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
    if (state->PC >= CHIPBOX_MEMORY_SIZE) {
        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
        state->log_msg = CHIPBOX_LOG_ILLEGAL;
        return 0;
    }
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

/* chipbox_cpu_jump: a safe function that sets PC to address, given that it is safe for this implementation,
   returning 1 for success and 0 for failure */
int chipbox_cpu_jump(struct chipbox_chip8_state *state, dbyte address) {
    if (address >= CHIPBOX_MEMORY_SIZE) {
        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
        state->log_msg = CHIPBOX_LOG_ILLEGAL;
        return 0;
    }
    if (address < CHIPBOX_PROGRAM_START) {
        state->log_level = CHIPBOX_LOG_LEVEL_WARN;
        state->log_msg = CHIPBOX_LOG_UNSAFE;
    }
    state->PC = address;
    return 1;
}

/* chipbox_cpu_eval_opcode: evaluates an opcode by manipulating state
   this is where the bulk of the logic for the chip-8 interpreter/emulator lies
   returns 1 on success, 0 on error */
int chipbox_cpu_eval_opcode(struct chipbox_chip8_state *state, dbyte opcode) {
    int i, j;
    byte x, y;

    chipbox_cpu_opcode_xy(opcode, &x, &y);

    state->log_level = CHIPBOX_LOG_LEVEL_NONE;

    switch ((opcode & 0xF000) >> 12) { /* get highest/left-most nybble */
        case 0:
            switch (opcode & 0x00FF) {
                case 0xE0: /* 00E0 (CLS): clear screen */
                    for (i = 0; i < CHIPBOX_SCREEN_HEIGHT; i++) {
                        for (j = 0; j < CHIPBOX_SCREEN_WIDTH_BYTES; j++) {
                            state->screen[CHIPBOX_SCREEN_WIDTH_BYTES * i + j] = 0;
                        }
                    }
                    return 1;
                case 0xEE: /* 00EE (RET): jump to address popped off stack */
                    if (state->SP == 0) {
                        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
                        state->log_msg = CHIPBOX_LOG_ILLEGAL;
                        return 0;
                    }
                    state->SP--;
                    return chipbox_cpu_jump(state, state->stack[state->SP]);
                default: /* 0NNN (SYS): execute machine language subroutine at NNN (ignored in modern interpreters) */
                    state->log_level = CHIPBOX_LOG_LEVEL_WARN;
                    state->log_msg = CHIPBOX_LOG_UNIMPL;
                    return 1;
            }
        case 1: /* 1NNN (JP NNN): jump (set PC) to NNN */
            return chipbox_cpu_jump(state, opcode & 0x0FFF);
        case 2: /* 2NNN (CALL NNN): jump (set PC) to NNN after pushing PC to stack */
            if (state->SP >= CHIPBOX_STACK_SIZE - 1) {
                state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
                state->log_msg = CHIPBOX_LOG_ILLEGAL;
                return 0;
            } else if (state->SP >= CHIPBOX_STACK_SAFE_SIZE - 1) {
                state->log_level = CHIPBOX_LOG_LEVEL_WARN;
                state->log_msg = CHIPBOX_LOG_UNSAFE;
            }
            state->stack[state->SP] = state->PC;
            state->SP++;
            return chipbox_cpu_jump(state, opcode & 0x0FFF);
        case 3: /* 3XNN (SE VX, NN): skip the next instruction if VX == NN */
            if (state->V[x] == (opcode & 0x00FF)) {
                state->PC += 2;
            }
            return 1;
        case 4: /* 4XNN (SNE VX, NN): skip the next instruction if VX != NN */
            if (state->V[x] != (opcode & 0x00FF)) {
                state->PC += 2;
            }
            return 1;
        case 5: /* 5XY0: (SE VX, VY): skip the next instruction if VX == VY */
            if ((opcode & 0x000F) == 0) {
                if (state->V[x] == state->V[y]) {
                    state->PC += 2;
                }
                return 1;
            }
            break;
        case 6: /* 6XNN (LD VX, NN): set value of VX to NN */
            state->V[x] = (opcode & 0x00FF);
            return 1;
        case 7: /* 7XNN (ADD VX, NN): add NN to VX */
            state->V[x] += (opcode & 0x00FF);
            return 1;
        case 8:
            switch (opcode & 0x000F) {
                case 0: /* 8XY0 (LD VX, VY): set VX to VY */
                    state->V[x] = state->V[y];
                    return 1;
            }
    }

    /* if we are here, then no implemented instruction was run */
    state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
    state->log_msg = CHIPBOX_LOG_UNIMPL;
    return 0;
}
