#include "chipbox_sdl_debug.h"
#include "dascore.h"
#include <stdio.h>

void chipbox_print_debug_state_dump(struct chipbox_chip8_state *state, byte level) {
    int i, j;
    dbyte opcode;
    int keymap[CHIPBOX_INPUT_KEYS] = {
        0x1, 0x2, 0x3, 0xC,
        0x4, 0x5, 0x6, 0xD,
        0x7, 0x8, 0x9, 0xE,
        0xA, 0x0, 0xB, 0xF
    };

    if (level == CHIPBOX_DEBUG_LEVEL_NONE) {
        return;
    }

    printf("STATE DUMP START\n");

    if (level & CHIPBOX_DEBUG_LEVEL_DISASSEMBLE) {
        opcode = state->memory[state->PC] << 8 | state->memory[state->PC + 1];
        print_chipbox_instruction_info(state->PC, opcode, disassemble_instruction(opcode));
    }

    if (level & CHIPBOX_DEBUG_LEVEL_COUNT) {
        printf("INSTRUCTION COUNT: %10lu\n", state->instruction_count);
    }

    if (level & CHIPBOX_DEBUG_LEVEL_SCREEN) {
        printf("SCREEN START\n");
        for (i = 0; i < CHIPBOX_SCREEN_HEIGHT; i++) {
            for (j = 0; j < CHIPBOX_SCREEN_WIDTH_PIXELS; j++) {
                if (state->screen[CHIPBOX_SCREEN_WIDTH_BYTES * i + (j / 8)] & (0x80 >> (j % 8))) { /* check if pixel is on */
                    printf("#");
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }
        printf("SCREEN END\n");
    }

    if (level & CHIPBOX_DEBUG_LEVEL_REGISTERS) {
        printf("REGISTERS START\n");
        printf("V ");
        for (i = 0; i < CHIPBOX_GREGISTERS; i++) {
            printf("%-2X ", i);
        }
        printf("\n  ");
        for (i = 0; i < CHIPBOX_GREGISTERS; i++) {
            printf("%02X ", state->V[i]);
        }
        printf("\n%-4s %-4s %-2s %-2s %-2s\n", "PC", "I", "DT", "ST", "SP");
        printf("%04X %04X %02X %02X %02X\n", state->PC, state->I, (byte)state->DT, (byte)state->ST, state->SP);
        printf("REGISTERS END\n");
    }

    if (level & CHIPBOX_DEBUG_LEVEL_STACK) {
        printf("STACK START\n");
        printf("%-5s ", "addr");
        for (i = 0; i < CHIPBOX_STACK_SIZE; i++) {
            printf("%04X ", i);
        }
        printf("\n%-5s ", "value");
        for (i = 0; i < CHIPBOX_STACK_SIZE; i++) {
            printf("%04X ", state->stack[i]);
        }
        printf("\n%*s\n", 5 + 5 * (state->SP + 1), "^^^^");
        printf("STACK END\n");
    }

    if (level & CHIPBOX_DEBUG_LEVEL_INPUT) {
        printf("INPUT START\n");
        for (i = 0; i < CHIPBOX_KEYPAD_ROWS; i++) {
            for (j = 0; j < CHIPBOX_KEYPAD_COLS; j++) {
                if (state->input[keymap[CHIPBOX_KEYPAD_ROWS * i + j]]) {
                    printf("%X ", keymap[CHIPBOX_KEYPAD_ROWS * i + j]);
                } else {
                    printf("  ");
                }
            }
            printf("\n");
        }
        printf("INPUT END\n");
    }

    if (level & CHIPBOX_DEBUG_LEVEL_MEMORY) {
        printf("MEMORY START\n");
        for (i = 0; i < CHIPBOX_MEMORY_SIZE; i += CHIPBOX_MEMDUMP_COLS) {
            printf("%04X  ", i);
            for (j = 0; j < CHIPBOX_MEMDUMP_COLS; j++) {
                printf("%02X ", state->memory[i + j]);
            }
            printf("\n");
        }
        printf("MEMORY END\n");
    }

    printf("STATE DUMP END\n");
}
