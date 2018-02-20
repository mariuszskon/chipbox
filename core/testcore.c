#include <stdio.h>
#include <string.h>
#include "core.h"
#include "cpu.h"
#include "fonts.h"

void test(int condition, char* name);
int all_equal(byte array[], int size, int value);

int tests = 0;
int failed = 0;

int main() {
    struct chipbox_chip8_state state;
    byte example_data[CHIPBOX_MEMORY_SIZE] = {0x0D, 0xEA, 0xD0, 0x0B, 0xE0, 0xE0, 0xF0};
    byte x, y;
    /* SECTION 1: typedefs and other low-level administrative stuff */
    test(sizeof(byte) == 1, "byte type should be one byte in length");
    test(sizeof(dbyte) == 2, "dbyte type should be two bytes in length");

    /* SECTION 2: validating initial state */
    state = chipbox_init_state();
    test(sizeof(state.screen) == CHIPBOX_SCREEN_WIDTH_PIXELS * CHIPBOX_SCREEN_HEIGHT / 8, "screen should have appropriate size");
    test(all_equal(state.screen, CHIPBOX_SCREEN_WIDTH_BYTES * CHIPBOX_SCREEN_HEIGHT, 0), "screen should be initialised to all zeroes (off)");
    test(sizeof(state.memory) == CHIPBOX_MEMORY_SIZE, "memory should have appropriate size");
    test(all_equal(state.V, CHIPBOX_GREGISTERS, 0), "general purpose registers should be initialised to 0");
    test(state.I == 0, "I register should be set to 0");
    test(state.SP == 0, "stack pointer should be set to 0");
    test(state.DT == 0 && state.ST == 0, "delay and sound timers should be set to 0");
    test(state.PC == CHIPBOX_PROGRAM_START, "PC (program counter) should start at specified value");
    test(all_equal(state.input, CHIPBOX_INPUT_KEYS, 0), "all input keys should be set to 0 (unpressed)");
    test(memcmp(&(state.memory[CHIPBOX_PROGRAM_START - CHIPBOX_FONT_TOTAL_BYTES]), chipbox_font, CHIPBOX_FONT_TOTAL_BYTES) == 0, "font should be loaded into area of memory below program memory");
    test(state.compat_mode == CHIPBOX_COMPATIBILITY_MODE_DEFAULT, "compatibility mode should be set to default");

    /* SECTION 3: validating internal cpu functions */
    state = chipbox_init_state();
    test(chipbox_cpu_load_program(&state, example_data, 7), "chipbox_cpu_load_program should return a true value on valid input");
    test(memcmp(&(state.memory[CHIPBOX_PROGRAM_START]), example_data, 7) == 0, "program should be loaded into appropriate memory location");
    test(state.PC == CHIPBOX_PROGRAM_START, "PC should be set to starting address of loaded program");

    state = chipbox_init_state();
    test(chipbox_cpu_load_program(&state, example_data, CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START), "loading a program of maximum size should be permitted");
    state = chipbox_init_state();
    test(chipbox_cpu_load_program(&state, example_data, CHIPBOX_MEMORY_SIZE - CHIPBOX_PROGRAM_START + 1) == 0, "loading a program of too great size should be rejected");

    state = chipbox_init_state();
    chipbox_cpu_load_program(&state, example_data, 7);
    test(chipbox_cpu_get_opcode(&state) == 0x0DEA, "chipbox_cpu_get_opcode should correctly return opcode (next two bytes, big endian)");
    test(state.PC == CHIPBOX_PROGRAM_START + 2, "chipbox_cpu_get_opcode should increment PC by 2");

    chipbox_cpu_opcode_xy(0xABCD, &x, &y);
    test(x == 0xB && y == 0xC, "chipbox_cpu_opcode_xy should correctly extract the nybbles representing X and Y in the opcode");

    /* SECTION 4: opcode testing - the good part */
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x0123), "0x0NNN (SYS NNN) should succeed");

    state = chipbox_init_state();
    state.screen[5] = 1;
    state.screen[42] = 1;
    test(chipbox_cpu_eval_opcode(&state, 0x00E0), "0x00E0 (CLS) should succeed");
    test(all_equal(state.screen, CHIPBOX_SCREEN_WIDTH_BYTES * CHIPBOX_SCREEN_HEIGHT, 0), "screen should be all zeroes after 0x00E0");

    /* END */
    printf("Tests: %d, failed: %d\n", tests, failed);
    return 0;
}

void test(int condition, char* name) {
    if (!condition) {
        printf("%4d FAILED: '%s'\n", tests, name);
        failed++;
    } else {
        printf("%4d OK\n", tests);
    }
    tests++;
}

int all_equal(byte array[], int size, int value) {
    int i;
    for (i = 0; i < size; i++) {
        if (array[i] != value) {
            return 0;
        }
    }
    return 1;
}
