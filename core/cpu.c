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

/* chipbox_cpu_validate_address: returns 1 on valid address, 0 on invalid (and raises error in log) */
int chipbox_cpu_validate_address(struct chipbox_chip8_state *state, dbyte address) {
    if (address >= CHIPBOX_MEMORY_SIZE) {
        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
        state->log_msg = CHIPBOX_LOG_ILLEGAL;
        return 0;
    } else {
        return 1;
    }
}

/* chipbox_cpu_jump: a safe function that sets PC to address, given that it is safe for this implementation,
   returning 1 for success and 0 for failure */
int chipbox_cpu_jump(struct chipbox_chip8_state *state, dbyte address) {
    if (!chipbox_cpu_validate_address(state, address)) {
        return 0;
    }
    if (address < CHIPBOX_PROGRAM_START) {
        state->log_level = CHIPBOX_LOG_LEVEL_WARN;
        state->log_msg = CHIPBOX_LOG_UNSAFE;
    }
    state->PC = address;
    return 1;
}

/* chipbox_cpu_srand: seeds the random number generator */
void chipbox_cpu_srand(struct chipbox_chip8_state *state, dbyte seed) {
    state->seed = seed;
    srand(seed);
}

/* chipbox_cpu_rand: returns a random byte */
byte chipbox_cpu_rand() {
    return rand();
}

/* chipbox_cpu_mem_read: safely read a byte from Chip 8 RAM - caller responsible for checking if we sent an error */
byte chipbox_cpu_mem_read(struct chipbox_chip8_state *state, dbyte address) {
    if(!chipbox_cpu_validate_address(state, address)) {
        return 0;
    }
    return state->memory[address];
}

/* chipbox_cpu_mem_write: set memory[address] to value safely, returning 1 on success, 0 on error */
int chipbox_cpu_mem_write(struct chipbox_chip8_state *state, dbyte address, byte value) {
    if (!chipbox_cpu_validate_address(state, address)) {
        return 0;
    }
    state->memory[address] = value;
    return 1;
}

/* TODO: rewrite */
int chipbox_cpu_draw(struct chipbox_chip8_state *state, dbyte opcode) {
    byte sprite_byte, draw_byte;
    byte bytes_to_draw;
    byte row = state->V[(opcode >> 4) & 0xF] % CHIPBOX_SCREEN_HEIGHT;
    byte col_start = state->V[(opcode >> 8) & 0xF] % CHIPBOX_SCREEN_WIDTH_PIXELS;
    byte col;
    dbyte read_address = state->I;
    for (bytes_to_draw = opcode & 0x000F; bytes_to_draw > 0; bytes_to_draw--) {
        sprite_byte = chipbox_cpu_mem_read(state, read_address);
        if (state->log_level != CHIPBOX_LOG_LEVEL_NONE) {
            return 0;
        }
        col = col_start;
        while (sprite_byte) { /* draw a sprite row */
            draw_byte = sprite_byte >> (col % 8);
            sprite_byte <<= (8 - (col % 8)); /* remove sprite bits which we will draw and setup next sprite_byte */
            if (row < CHIPBOX_SCREEN_HEIGHT && col < CHIPBOX_SCREEN_WIDTH_PIXELS) {
                /* in bytes, not pixels */
                if (state->screen[CHIPBOX_SCREEN_WIDTH_BYTES * row + (col / 8)] & draw_byte) { /* check for collision */
                    state->V[0xF] = 1;
                } else {
                    state->V[0xF] = 0;
                }
                state->screen[CHIPBOX_SCREEN_WIDTH_BYTES * row + (col / 8)] ^= draw_byte;
            } /* else clip the sprite */
            col = col - (col % 8); /* round col to nearest multiple of 8 (byte) */
            col += 8; /* go to next byte column */
        }
        row++;
        read_address++;
    }
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
                case 1: /* 8XY1 (OR VX, VY): set VX to VX OR VY */
                    state->V[x] |= state->V[y];
                    return 1;
                case 2: /* 8XY2 (AND VX, VY): set VX to VX AND VY */
                    state->V[x] &= state->V[y];
                    return 1;
                case 3: /* 8XY3 (XOR VX, VY): set VX to VX XOR VY */
                    state->V[x] ^= state->V[y];
                    return 1;
                case 4: /* 8XY4 (ADD VX, VY): set VX to VX + VY, VF = carry */
                    i = state->V[x] + state->V[y];
                    state->V[0xF] = i > 255 ? 1 : 0;
                    state->V[x] = i;
                    return 1;
                case 5: /* 8XY5 (SUB VX, VY): set VX to VX - VY, VF = not borrow */
                    state->V[0xF] = state->V[x] < state->V[y] ? 0 : 1;
                    state->V[x] -= state->V[y];
                    return 1;
                case 6: /* 8XY6 (SHR VX, VY): set VX to VY >> 1 (in MATTMIK compatibility mode)
                           set VX to VX >> 1 (ignoring VY) (in COWGOD compatibility mode)
                           in both modes, VF = least significant bit of shifted register */
                    if (x != y) {
                        state->log_level = CHIPBOX_LOG_LEVEL_WARN;
                        state->log_msg = CHIPBOX_LOG_IMPL_DEFINED; /* because of differences between MATTMIK and COWGOD for X != Y */
                    }
                    if (state->compat_mode == CHIPBOX_COMPATIBILITY_MODE_COWGOD) {
                        y = x;
                    }
                    state->V[0xF] = state->V[y] & 1;
                    state->V[x] = state->V[y] >> 1;
                    return 1;
                case 7: /* 8XY7 (SUBN VX, VY): set VX to VY - VX, VF = not borrow */
                    state->V[0xF] = state->V[y] < state->V[x] ? 0 : 1;
                    state->V[x] = state->V[y] - state->V[x];
                    return 1;
                case 0xE: /* 8XYE (SHL VX, VY): see 8XY6 (SHR VX, VY) - this is left instead of right */
                    if (x != y) {
                        state->log_level = CHIPBOX_LOG_LEVEL_WARN;
                        state->log_msg = CHIPBOX_LOG_IMPL_DEFINED;
                    }
                    if (state->compat_mode == CHIPBOX_COMPATIBILITY_MODE_COWGOD) {
                        y = x;
                    }
                    state->V[0xF] = (state->V[y] >> 7)& 1;
                    state->V[x] = state->V[y] << 1;
                    return 1;
            }
            break; /* no 8XYN instruction (N invalid) */
        case 9: /* 9XY0 (SNE VX, VY): skip next instruction if VX != VY */
            if ((opcode & 0x000F) == 0) {
                if (state->V[x] != state->V[y]) {
                    state->PC += 2;
                }
                return 1;
            }
            break;
        case 0xA: /* ANNN (LD I, NNN): set I to NNN */
            state->I = opcode & 0x0FFF;
            return 1;
        case 0xB: /* BNNN (JP V0, NNN): set PC (jump) to NNN + V0 */
            return chipbox_cpu_jump(state, (opcode & 0x0FFF) + state->V[0]);
        case 0xC: /* CXNN (RND VX, NN): set VX to a random number AND NN */
            state->V[x] = chipbox_cpu_rand() & (opcode & 0x00FF);
            return 1;
        case 0xD: /* DXYN (DRW VX, VY, N): draw N bytes of sprite data starting from I,
                     at the position VX, VY */
            return chipbox_cpu_draw(state, opcode);
        case 0xE:
            switch (opcode & 0x00FF) {
                case 0x9E: /* 0xEX9E (SKP VX): skips the next instruction if key corresponding to value of VX is pressed */
                    if (state->V[x] <= 0xF) {
                        if (state->input[state->V[x]] == 1) {
                            state->PC += 2;
                        }
                        return 1;
                    } else {
                        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
                        state->log_msg = CHIPBOX_LOG_RANGE;
                        return 0;
                    }
                case 0xA1: /* 0xEXA1 (SKNP VX): skips the next instruction if key corresponding to value of VX is NOT pressed */
                    if (state->V[x] <= 0xF) {
                        if (state->input[state->V[x]] == 0) {
                            state->PC += 2;
                        }
                        return 1;
                    } else {
                        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
                        state->log_msg = CHIPBOX_LOG_RANGE;
                        return 0;
                    }
            }
            break;
        case 0xF:
            switch (opcode & 0x00FF) {
                case 0x07: /* FX07 (LD VX, DT): set VX to value of DT */
                    state->V[x] = state->DT;
                    return 1;
                case 0x0A: /* FX0A (LD VX, K): wait for a keypress, and set VX to the value of the key */
                    j = -1;
                    for (i = 0; i < CHIPBOX_INPUT_KEYS; i++) {
                        if (state->input[i] == 1) {
                            j = i;
                            break;
                        }
                    }
                    if (j == -1) {
                        /* no key pressed, so wait for it */
                        state->log_level = CHIPBOX_LOG_LEVEL_INFO;
                        state->log_msg = CHIPBOX_LOG_WAIT;
                        state->PC -= 2;
                    } else {
                        state->V[x] = j;
                    }
                    return 1;
                case 0x15: /* FX15 (LD DT, VX): set value of DT to value of VX */
                    state->DT = state->V[x];
                    return 1;
                case 0x18: /* FX18 (LD ST, VX): set value of ST to VX */
                    state->ST = state->V[x];
                    return 1;
                case 0x1E: /* FX1E (ADD I, VX): add VX to I */
                    state->I += state->V[x];
                    return 1;
                case 0x29: /* FX29 (LD F, VX): set I to address for font sprite corresponding to value of VX */
                    if (state->V[x] <= 0xF) {
                        state->I = CHIPBOX_PROGRAM_START - CHIPBOX_FONT_TOTAL_BYTES + (state->V[x] * CHIPBOX_FONT_SIZE);
                        return 1;
                    } else {
                        state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
                        state->log_msg = CHIPBOX_LOG_RANGE;
                        return 0;
                    }
            }
            break;
    }

    /* if we are here, then no implemented instruction was run */
    state->log_level = CHIPBOX_LOG_LEVEL_ERROR;
    state->log_msg = CHIPBOX_LOG_UNIMPL;
    return 0;
}
