#include <stdio.h>
#include <string.h>
#include "core.h"
#include "cpu.h"
#include "fonts.h"
#include "log.h"

void test(int condition, char* name);
int all_equal(byte array[], int size, int value);

int tests = 0;
int failed = 0;

int main() {
    struct chipbox_chip8_state state;
    byte example_data[CHIPBOX_MEMORY_SIZE] = {0x0D, 0xEA, 0xD0, 0x0B, 0xE0, 0xE0, 0xF0};
    byte x, y;

    /* SECTION 1: typedefs and other low-level administrative stuff */
    printf("== SECTION 1 ==\n");
    test(sizeof(byte) == 1, "byte type should be one byte in length");
    test(sizeof(dbyte) == 2, "dbyte type should be two bytes in length");

    /* SECTION 2: validating initial state */
    printf("== SECTION 2 ==\n");
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
    test(state.log_level == CHIPBOX_LOG_LEVEL_NONE, "log level for initial message should be set to none");

    /* SECTION 3: validating internal cpu functions */
    printf("== SECTION 3 ==\n");
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

    state = chipbox_init_state();
    state.PC = CHIPBOX_MEMORY_SIZE;
    test(chipbox_cpu_get_opcode(&state) == 0, "chipbox_cpu_get_opcode should return 0 if PC is too high");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "chipbox_cpu_get_opcode should raise an illegal error if PC is too high");

    chipbox_cpu_opcode_xy(0xABCD, &x, &y);
    test(x == 0xB && y == 0xC, "chipbox_cpu_opcode_xy should correctly extract the nybbles representing X and Y in the opcode");

    state = chipbox_init_state();
    test(chipbox_cpu_jump(&state, 0x456), "chipbox_cpu_jump should succeed with a valid address");
    test(state.PC == 0x456, "chipbox_cpu_jump should set PC to a valid address");
    state = chipbox_init_state();
    test(chipbox_cpu_jump(&state, 0x1FE), "chipbox_cpu_jump should succeed with a valid but unsafe for original implementation address");
    test(state.PC == 0x1FE, "chipbox_cpu_jump should set PC to a original-unsafe address");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNSAFE, "chipbox_cpu_jump should warn about original-unsafe addresses");
    state = chipbox_init_state();
    test(chipbox_cpu_jump(&state, 0x201), "chipbox_cpu_jump should succeed on an odd address");
    test(state.PC == 0x201, "chipbox_cpu_jump should set PC to an odd address if required");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_IMPL_DEFINED, "chipbox_cpu_jump should raise an implementation defined warning if target address is odd");
    state = chipbox_init_state();
    test(chipbox_cpu_jump(&state, 0x1000) == 0, "chipbox_cpu_jump should fail on an invalid address");
    test(state.PC != 0x1000, "chipbox_cpu_jump should not set PC to an invalid address");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "chipbox_cpu_jump should raise an illegal error on an invalid (too high) address");

    /* SECTION 4: opcode testing - the good part */
    printf("== SECTION 4 ==\n");
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x0123), "0x0NNN (SYS NNN) should succeed");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNIMPL, "0x0NNN should raise an unimplemented warning");

    state = chipbox_init_state();
    state.screen[5] = 1;
    state.screen[42] = 1;
    test(chipbox_cpu_eval_opcode(&state, 0x00E0), "0x00E0 (CLS) should succeed");
    test(all_equal(state.screen, CHIPBOX_SCREEN_WIDTH_BYTES * CHIPBOX_SCREEN_HEIGHT, 0), "screen should be all zeroes after 0x00E0");

    state = chipbox_init_state();
    state.PC = 0x432;
    state.stack[0] = 0x314;
    state.SP = 1;
    test(chipbox_cpu_eval_opcode(&state, 0x00EE), "0x00EE (RET) should succeed");
    test(state.PC == 0x314, "0x00EE should set PC to stack[SP-1]");
    test(state.SP == 0, "0x00EE should decrement SP");
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x00EE) == 0, "0x00EE (RET) should fail if SP == 0");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "0x00EE (RET) should send an illegal error if SP == 0");
    state = chipbox_init_state();
    state.stack[0] = 0x1FF;
    state.SP = 1;
    test(chipbox_cpu_eval_opcode(&state, 0x00EE), "0x00EE (RET) should succeed even if target address is unsafe");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNSAFE, "0x00EE (RET) should raise an unsafe warning if target address is below program start address");

    state = chipbox_init_state();
    state.PC = 0x300;
    test(chipbox_cpu_eval_opcode(&state, 0x1AB2), "0x1NNN (JP NNN) should succeed");
    test(state.PC == 0xAB2, "0x1NNN (JP NNN) should set PC to NNN");
    state = chipbox_init_state();
    state.PC = 0x312;
    test(chipbox_cpu_eval_opcode(&state, 0x11FF), "0x1NNN (JP NNN) should succeed even if target address is below program start address");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNSAFE, "0x1NNN (JP NNN) should raise unsafe warning if target address is below program start address");

    state = chipbox_init_state();
    state.PC = 0xEF8;
    test(chipbox_cpu_eval_opcode(&state, 0x2F02), "0x2NNN (CALL NNN) should succeed");
    test(state.stack[0] == 0xEF6, "0x2NNN (CALL NNN) should set stack[SP] to the calling address, which is PC-2 due to PC already being incremented to point to next instruction");
    test(state.SP == 1, "0x2NNN (CALL NNN) should increment the stack pointer");
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x21FF), "0x2NNN (CALL NNN) should succeed even if target address is below program starting address");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNSAFE, "0x2NNN (CALL NNN) should raise unsafe warning if target address is below program start address");
    state = chipbox_init_state();
    state.SP = CHIPBOX_STACK_SIZE - 1;
    test(chipbox_cpu_eval_opcode(&state, 0x2CB4) == 0, "0x2NNN (CALL NNN) should fail if SP is at its maximum");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "0x2NNN (CALL NNN) should raise an illegal error if SP is its maximum (stack size - 1)");
    state = chipbox_init_state();
    state.SP = CHIPBOX_STACK_SIZE - 1;
    test(chipbox_cpu_eval_opcode(&state, 0x2888), "0x2NNN (CALL NNN) should succeed even if SP exceeds safe values for original implementation");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNSAFE, "0x2NNN (CALL NNN) should raise an unsafe warning if SP goes beyond safe values for original implementation");

    state = chipbox_init_state();
    state.PC = 0x456;
    state.V[7] = 0xBE;
    test(chipbox_cpu_eval_opcode(&state, 0x37BE), "0x3XNN (SE VX, NN) should succeed");
    test(state.PC == 0x458, "0x3XNN (SE VX, NN) should increment PC by 2 if VX == NN");
    state = chipbox_init_state();
    state.PC = 0x234;
    state.V[0xB] = 0xCE;
    chipbox_cpu_eval_opcode(&state, 0x3BDE);
    test(state.PC == 0x234, "0x3XNN (SE VX, NN) should not modify PC if VX != NN");

    state = chipbox_init_state();
    state.PC = 0x876;
    state.V[3] = 0x12;
    test(chipbox_cpu_eval_opcode(&state, 0x4313), "0x4XNN (SNE VX, NN) should succeed");
    test(state.PC == 0x878, "0x4XNN (SNE VX, NN) should increment PC by 2 if VX != NN");
    state = chipbox_init_state();
    state.PC = 0x202;
    state.V[9] = 0x42;
    chipbox_cpu_eval_opcode(&state, 0x4942);
    test(state.PC == 0x202, "0x4XNN (SNE VX, NN) should not modify PC if VX == NN");

    state = chipbox_init_state();
    state.PC = 0x890;
    state.V[4] = 0x99;
    state.V[0xB] = 0x99;
    test(chipbox_cpu_eval_opcode(&state, 0x54B0), "0x5XY0 (SE VX, VY) should succeed");
    test(state.PC == 0x892, "0x5XY0 (SE VX, VY) should increment PC by 2 if VX == VY");
    state = chipbox_init_state();
    state.PC = 0x230;
    state.V[0xE] = 0x3;
    state.V[1] = 0x4;
    chipbox_cpu_eval_opcode(&state, 0x5E10);
    test(state.PC == 0x230, "0x5XY0 (SE VX, VY) should not modify PC if VX != VY");

    /* END */
    printf("== END ==\n");
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
