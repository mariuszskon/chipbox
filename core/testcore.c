#include <stdio.h>
#include <time.h>
#include <string.h>
#include "core.h"
#include "cpu.h"
#include "fonts.h"
#include "log.h"

void test(int condition, char* name);
int all_equal(byte array[], int size, int value);
void print_section(int section_num);

int tests = 0;
int failed = 0;

int main() {
    struct chipbox_chip8_state state;
    byte example_data[CHIPBOX_MEMORY_SIZE] = {0x0D, 0xEA, 0xD0, 0x0B, 0xE0, 0xE0, 0xF0};
    byte x, y;

    /* SECTION 1: typedefs and other low-level administrative stuff */
    print_section(1);
    test(sizeof(byte) == 1, "byte type should be one byte in length");
    test(sizeof(dbyte) == 2, "dbyte type should be two bytes in length");

    /* SECTION 2: validating initial state */
    print_section(2);
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
    print_section(3);
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
    state = chipbox_init_state();
    test(chipbox_cpu_jump(&state, 0x1000) == 0, "chipbox_cpu_jump should fail on an invalid address");
    test(state.PC != 0x1000, "chipbox_cpu_jump should not set PC to an invalid address");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "chipbox_cpu_jump should raise an illegal error on an invalid (too high) address");

    /* SECTION 4: opcode testing - the good part */
    print_section(4);
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x0123), "0x0NNN (SYS NNN) should succeed");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNIMPL, "0x0NNN (SYS NNN) should raise an unimplemented warning");

    state = chipbox_init_state();
    state.screen[5] = 1;
    state.screen[42] = 1;
    test(chipbox_cpu_eval_opcode(&state, 0x00E0), "0x00E0 (CLS) should succeed");
    test(all_equal(state.screen, CHIPBOX_SCREEN_WIDTH_BYTES * CHIPBOX_SCREEN_HEIGHT, 0), "0x00E0 (CLS) should set screen to all zeroes (off)");

    state = chipbox_init_state();
    state.PC = 0x432;
    state.stack[0] = 0x314;
    state.SP = 1;
    test(chipbox_cpu_eval_opcode(&state, 0x00EE), "0x00EE (RET) should succeed");
    test(state.PC == 0x314, "0x00EE (RET) should set PC to stack[SP-1]");
    test(state.SP == 0, "0x00EE (RET) should decrement SP");
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x00EE) == 0, "0x00EE (RET) should fail if SP == 0");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "0x00EE (RET) should send an illegal error if SP == 0");
    state = chipbox_init_state();
    state.stack[0] = 0x1FE;
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
    test(state.stack[0] == 0xEF8, "0x2NNN (CALL NNN) should set stack[SP] to the calling address + 2 (which is PC)");
    test(state.SP == 1, "0x2NNN (CALL NNN) should increment the stack pointer");
    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x21FF), "0x2NNN (CALL NNN) should succeed even if target address is below program starting address");
    test(state.log_level == CHIPBOX_LOG_LEVEL_WARN && state.log_msg == CHIPBOX_LOG_UNSAFE, "0x2NNN (CALL NNN) should raise unsafe warning if target address is below program start address");
    state = chipbox_init_state();
    state.SP = CHIPBOX_STACK_SIZE - 1;
    test(chipbox_cpu_eval_opcode(&state, 0x2CB4) == 0, "0x2NNN (CALL NNN) should fail if SP is at its maximum");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_ILLEGAL, "0x2NNN (CALL NNN) should raise an illegal error if SP is its maximum (stack size - 1)");
    state = chipbox_init_state();
    state.SP = CHIPBOX_STACK_SAFE_SIZE - 1;
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

    state = chipbox_init_state();
    test(chipbox_cpu_eval_opcode(&state, 0x6D42), "0x6XNN (LD VX, NN) should succeed");
    test(state.V[0xD] == 0x42, "0x6XNN (LD VX, NN) should set VX to NN");

    state = chipbox_init_state();
    state.V[0xB] = 0x21;
    test(chipbox_cpu_eval_opcode(&state, 0x7BEF), "0x7XNN (ADD VX, NN) should succeed");
    test(state.V[0xB] == (byte)(0x21 + 0xEF), "0x7XNN (ADD VX, NN) should add NN to VX");
    test(state.V[0xF] == 0, "0x7XNN (ADD VX, NN) should not modify the VF register, even on overflow");

    state = chipbox_init_state();
    state.V[0x1] = 17;
    state.V[0x8] = 200;
    test(chipbox_cpu_eval_opcode(&state, 0x8810), "0x8XY0 (LD VX, VY) should succeed");
    test(state.V[0x8] == state.V[0x1] && state.V[0x8] == 17, "0x8XY0 (LD VX, VY) should set VX to VY");

    state = chipbox_init_state();
    state.V[0x3] = 0xAB;
    state.V[0x8] = 0x1D;
    test(chipbox_cpu_eval_opcode(&state, 0x8381), "0x8XY1 (OR VX, VY) should succeed");
    test(state.V[0x3] == (0xAB | 0x1D), "0x8XY1 (OR VX, VY) should set VX to VX OR VY");

    state = chipbox_init_state();
    state.V[0xE] = 0x15;
    state.V[0x0] = 0xB6;
    test(chipbox_cpu_eval_opcode(&state, 0x8E02), "0x8XY2 (AND VX, VY) should succeed");
    test(state.V[0xE] == (0x15 & 0xB6), "0x8XY2 (AND VX, VY) should set VX to VX AND VY");

    state = chipbox_init_state();
    state.V[0x4] = 0xBE;
    state.V[0x5] = 0x78;
    test(chipbox_cpu_eval_opcode(&state, 0x8453), "0x8XY3 (XOR VX, VY) should succeed");
    test(state.V[0x4] == (0xBE ^ 0x78), "0x8XY3 (XOR VX, VY) should set VX to VX XOR VY");

    state = chipbox_init_state();
    state.V[0x3] = 210;
    state.V[0xA] = 42;
    state.V[0xF] = 5;
    test(chipbox_cpu_eval_opcode(&state, 0x83A4), "0x8XY4 (ADD VX, VY) should succeed");
    test(state.V[0x3] == 252, "0x8XY4 (ADD VX, VY) should add VY to VX");
    test(state.V[0xF] == 0, "0x8XY4 (ADD VX, VY) should set VF to 0 if not carry occurs");
    state = chipbox_init_state();
    state.V[0x7] = 254;
    state.V[0x0] = 2;
    state.V[0xF] = 6;
    chipbox_cpu_eval_opcode(&state, 0x8704);
    test(state.V[0x7] == 0, "0x8XY4 (ADD VX, VY) should wrap on overflow");
    test(state.V[0xF] == 1, "0x8XY4 (ADD VX, VY) should set VF to 1 on carry/overflow");

    state = chipbox_init_state();
    state.V[0x2] = 50;
    state.V[0x5] = 26;
    state.V[0xF] = 8;
    test(chipbox_cpu_eval_opcode(&state, 0x8255), "0x8XY5 (SUB VX, VY) should succeed");
    test(state.V[0x2] == 24, "0x8XY5 (SUB VX, VY) should subtract VY from VX (VX = VX - VY)");
    test(state.V[0xF] == 1, "0x8XY5 (SUB VX, VY) should set VF to 1 if no borrow/underflow occurs");
    state = chipbox_init_state();
    state.V[0x3] = 1;
    state.V[0xE] = 2;
    state.V[0xF] = 99;
    chipbox_cpu_eval_opcode(&state, 0x83E5);
    test(state.V[0x3] == 255, "0x8XY5 (SUB VX, VY) should wrap on underflow");
    test(state.V[0xF] == 0, "0x8XY5 (SUB VX, VY) should set VF to 0 if a borrow/underflow occurs");

    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    state.V[4] = 0xF0;
    state.V[6] = 42;
    state.V[0xF] = 50;
    test(chipbox_cpu_eval_opcode(&state, 0x8646), "0x8XY6 (SHR VX, VY) should succeed (MATTMIK mode)");
    test(state.V[4] == 0xF0, "0x8XY6 (SHR VX, VY) should leave VY unchanged in MATTMIK compatibility mode");
    test(state.V[6] == 0xF0 >> 1, "0x8XY6 (SHR VX, VY) should set VX to VY >> 1 in MATTMIK compatibility mode");
    test(state.V[0xF] == 0, "0x8XY6 (SHR VX, VY) should set VF to 0 if least significant bit of VY is 0 (MATTMIK)");
    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    state.V[4] = 0x0F;
    state.V[6] = 42;
    state.V[0xF] = 50;
    chipbox_cpu_eval_opcode(&state, 0x8646);
    test(state.V[0xF] == 1, "0x8XY6 (SHR VX, VY) should set VF to 1 if least significant bit of VY is 1 (MATTMIK)");
    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    state.V[4] = 0xF0;
    state.V[6] = 0xE0;
    state.V[0xF] = 50;
    test(chipbox_cpu_eval_opcode(&state, 0x8646), "0x8XY6 (SHR VX, VY) should succeed (COWGOD mode)");
    test(state.V[4] == 0xF0, "0x8XY6 (SHR VX, VY) should leave VY unchanged in COWGOD compatibility mode");
    test(state.V[6] == 0xE0 >> 1, "0x8XY6 (SHR VX, VY) should set VX to VX >> 1 in COWGOD compatibility mode");
    test(state.V[0xF] == 0, "0x8XY6 (SHR VX, VY) should set VF to 0 if least significant bit of VX is 0 (COWGOD)");
    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    state.V[4] = 0xF0;
    state.V[6] = 0x0D;
    state.V[0xF] = 50;
    chipbox_cpu_eval_opcode(&state, 0x8646);
    test(state.V[0xF] == 1, "0x8XY6 (SHR VX, VY) should set VF to 1 if least significant bit of VX is 1 (COWGOD)");

    state = chipbox_init_state();
    state.V[0x0] = 42;
    state.V[0xB] = 11;
    state.V[0xF] = 99;
    test(chipbox_cpu_eval_opcode(&state, 0x8B07), "0x8XY7 (SUBN VX, VY) should succeed");
    test(state.V[0xB] == 31, "0x8XY7 (SUBN VX, VY) should set VX to VY - VX");
    test(state.V[0xF] == 1, "0x8XY7 (SUBN VX, VY) should set VF to 1 if no borrow/underflow occurs");
    state = chipbox_init_state();
    state.V[0x0] = 42;
    state.V[0xB] = 11;
    state.V[0xF] = 99;
    chipbox_cpu_eval_opcode(&state, 0x80B7);
    test(state.V[0x0] == (byte)(11 - 42), "0x8XY7 (SUBN VX, VY) should wrap on underflow");
    test(state.V[0xF] == 0, "0x8XY7 (SUBN VX, VY) should set VF to 0 if borrow/underflow occurs");

    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    state.V[4] = 0xF0;
    state.V[6] = 42;
    state.V[0xF] = 50;
    test(chipbox_cpu_eval_opcode(&state, 0x864E), "0x8XYE (SHL VX, VY) should succeed (MATTMIK mode)");
    test(state.V[4] == 0xF0, "0x8XYE (SHL VX, VY) should leave VY unchanged in MATTMIK compatibility mode");
    test(state.V[6] == (byte)(0xF0 << 1), "0x8XYE (SHL VX, VY) should set VX to VY << 1 in MATTMIK compatibility mode");
    test(state.V[0xF] == 1, "0x8XYE (SHL VX, VY) should set VF to 1 if most significant bit of VY is 1 (MATTMIK)");
    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    state.V[4] = 0x0F;
    state.V[6] = 42;
    state.V[0xF] = 50;
    chipbox_cpu_eval_opcode(&state, 0x864E);
    test(state.V[0xF] == 0, "0x8XYE (SHL VX, VY) should set VF to 0 if most significant bit of VY is 0 (MATTMIK)");
    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    state.V[4] = 0xF0;
    state.V[6] = 0xE0;
    state.V[0xF] = 50;
    test(chipbox_cpu_eval_opcode(&state, 0x864E), "0x8XYE (SHL VX, VY) should succeed (COWGOD mode)");
    test(state.V[4] == 0xF0, "0x8XYE (SHL VX, VY) should leave VY unchanged in COWGOD compatibility mode");
    test(state.V[6] == (byte)(0xE0 << 1), "0x8XYE (SHL VX, VY) should set VX to VX << 1 in COWGOD compatibility mode");
    test(state.V[0xF] == 1, "0x8XYE (SHL VX, VY) should set VF to 1 if most significant bit of VX is 1 (COWGOD)");
    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    state.V[4] = 0xF0;
    state.V[6] = 0x0E;
    state.V[0xF] = 50;
    chipbox_cpu_eval_opcode(&state, 0x864E);
    test(state.V[0xF] == 0, "0x8XYE (SHL VX, VY) should set VF to 0 if most significant bit of VX is 0 (COWGOD)");

    state = chipbox_init_state();
    state.V[0xA] = 81;
    state.V[0xC] = 81;
    state.PC = 0x404;
    test(chipbox_cpu_eval_opcode(&state, 0x9AC0), "0x9XY0 (SNE VX, VY) should succeed");
    test(state.PC == 0x404, "0x9XY0 (SNE VX, VY) should not modify PC if VX == VY");
    state = chipbox_init_state();
    state.V[0x6] = 99;
    state.V[0x1] = 98;
    state.PC = 0x404;
    chipbox_cpu_eval_opcode(&state, 0x9610);
    test(state.PC == 0x406, "0x9XY0 (SNE VX, VY) should  increment PC by 2 if VX != VY");

    state = chipbox_init_state();
    state.I = 0x802;
    test(chipbox_cpu_eval_opcode(&state, 0xAAB2), "0xANNN (LD I, NNN) should succeed");
    test(state.I == 0xAB2, "0xANNN (LD I, NNN) should set I to NNN");

    state = chipbox_init_state();
    state.PC = 0x300;
    state.V[0] = 0x60;
    test(chipbox_cpu_eval_opcode(&state, 0xB210), "0xBNNN (JP V0, NNN) should succeed");
    test(state.PC == 0x270, "0xBNNN (JP V0, NNN) should set PC to NNN + V0");

    state = chipbox_init_state();
    chipbox_cpu_srand(&state, time(NULL));
    test(chipbox_cpu_eval_opcode(&state, 0xC0AB), "0xCXNN (RND VX, NN) should succeed");
    chipbox_cpu_eval_opcode(&state, 0xC1AB);
    chipbox_cpu_eval_opcode(&state, 0xC2AB);
    chipbox_cpu_eval_opcode(&state, 0xC3AB);
    test(!all_equal(state.V, 4, 0), "0xCXNN (RND VX, NN) should set VX to a random number");
    test((state.V[0] & ~0xAB) == 0 && (state.V[1] & ~0xAB) == 0 && (state.V[2] & ~0xAB) == 0 && (state.V[3] & ~0xAB) == 0, "0xCXNN (RND VX, NN) should mask random value in VN with NN (in other words, VX = rand() & 0xNN)");

    state = chipbox_init_state();
    state.memory[0x300] = 0xFE;
    state.I = 0x300;
    state.V[7] = 16;
    state.V[0xB] = 7;
    state.V[0xF] = 99;
    test(chipbox_cpu_eval_opcode(&state, 0xD7B1), "0xDXYN (DRW VX, VY, N) should succeed");
    test(state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 7 + 2] == 0xFE, "0xDXYN (DRW VX, VY, N) should draw a perfectly aligned 1 byte sprite correctly");
    test(state.V[0xF] == 0, "0xDXYN (DRW VX, VY, N) should set VF to 0 if no collision occurred");
    state.memory[0x301] = 0x02;
    state.I = 0x301;
    state.V[0xF] = 99;
    chipbox_cpu_eval_opcode(&state, 0xD7B1);
    test(state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 7 + 2] == 0xFC, "0xDXYN (DRW VX, VY, N) should XOR sprites");
    test(state.V[0xF] == 1, "0xDXYN (DRW VX, VY, N) should set VF to 1 if a collision occurred");
    state = chipbox_init_state();
    state.memory[0x300] = 0xFE;
    state.I = 0x300;
    state.V[7] = 19;
    state.V[0xB] = 5;
    chipbox_cpu_eval_opcode(&state, 0xD7B1);
    test(state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 5 + 2] == (0xFE >> 3) && state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 5 + 3] == (byte)(0xFE << 5), "0xDXYN (DRW VX, VY, N) should draw a non-aligned 1 byte sprite correctly");
    state = chipbox_init_state();
    state.I = 0x300;
    state.V[7] = 16;
    state.V[0xB] = 7;
    state.memory[0x300] = 0xFE;
    state.memory[0x301] = 0x12;
    chipbox_cpu_eval_opcode(&state, 0xD7B2);
    test(state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 7 + 2] == 0xFE && state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 8 + 2] == 0x12, "0xDXYN (DRW VX, VY, N) should draw a perfectly aligned multi-byte sprite correctly");
    state = chipbox_init_state();
    state.I = 0x300;
    state.V[7] = 19;
    state.V[0xB] = 5;
    state.memory[0x300] = 0xFE;
    state.memory[0x301] = 0x12;
    chipbox_cpu_eval_opcode(&state, 0xD7B2);
    test(state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 5 + 2] == (0xFE >> 3) && state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 5 + 3] == (byte)(0xFE << 5) && state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 6 + 2] == (0x12 >> 3) && state.screen[CHIPBOX_SCREEN_WIDTH_BYTES * 6 + 3] == (byte)(0x12 << 5), "0xDXYN (DRW VX, VY, N) should draw a non-aligned multi-byte sprite correctly");

    state = chipbox_init_state();
    state.PC = 0x404;
    state.V[1] = 0xB;
    state.input[0xB] = 0;
    test(chipbox_cpu_eval_opcode(&state, 0xE19E), "0xE19E (SKP VX) should succeed");
    test(state.PC == 0x404, "0xE19E (SKP VX) should not change PC if key corresponding to value of VX is not pressed");
    state = chipbox_init_state();
    state.PC = 0x404;
    state.V[1] = 0xB;
    state.input[0xB] = 1;
    chipbox_cpu_eval_opcode(&state, 0xE19E);
    test(state.PC == 0x406, "0xE19E (SKP VX) should increment PC by 2 if key corresponding to value of VX is pressed");
    state = chipbox_init_state();
    state.V[1] = 0x10;
    test(chipbox_cpu_eval_opcode(&state, 0xE19E) == 0, "0xE19E (SKP VX) should fail if value of VX is out of range");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_RANGE, "0xE19E (SKP VX) should raise a range error if value of VX is out of range");

    state = chipbox_init_state();
    state.PC = 0x404;
    state.V[1] = 0xB;
    state.input[0xB] = 0;
    test(chipbox_cpu_eval_opcode(&state, 0xE1A1), "0xE1A1 (SKNP VX) should succeed");
    test(state.PC == 0x406, "0xE1A1 (SKNP VX) should increment PC by 2 if key corresponding to value of VX is not pressed");
    state = chipbox_init_state();
    state.PC = 0x404;
    state.V[1] = 0xB;
    state.input[0xB] = 1;
    chipbox_cpu_eval_opcode(&state, 0xE1A1);
    test(state.PC == 0x404, "0xE1A1 (SKNP VX) should not change PC if key corresponding to value of VX is pressed");
    state = chipbox_init_state();
    state.V[1] = 0x10;
    test(chipbox_cpu_eval_opcode(&state, 0xE1A1) == 0, "0xE1A1 (SKNP VX) should fail if value of VX is out of range");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_RANGE, "0xE1A1 (SKNP VX) should raise a range error if value of VX is out of range");

    state = chipbox_init_state();
    state.DT = 50;
    state.V[4] = 99;
    test(chipbox_cpu_eval_opcode(&state, 0xF407), "0xFX07 (LD VX, DT) should succeed");
    test(state.V[4] == 50, "0xFX07 (LD VX, DT) should set VX to DT");

    state = chipbox_init_state();
    state.PC = 0x404;
    state.V[1] = 99;
    test(chipbox_cpu_eval_opcode(&state, 0xF10A), "0xFX0A (LD VX, K) should succeed");
    test(state.log_level == CHIPBOX_LOG_LEVEL_INFO && state.log_msg == CHIPBOX_LOG_WAIT, "0xFX0A (LD VX, K) should log that it is waiting if no keys are pressed");
    test(state.PC == 0x402, "0xFX0A (LD VX, K) should decrement PC by 2 to ensure it is run again when a key is pressed");
    state.input[0xA] = 1;
    chipbox_cpu_eval_opcode(&state, 0xF10A);
    test(state.V[1] == 0xA, "0xFX0A (LD VX, K) should store the key that is pressed in VX");

    state = chipbox_init_state();
    state.V[9] = 70;
    test(chipbox_cpu_eval_opcode(&state, 0xF915), "0xFX15 (LD DT, VX) should succeed");
    test(state.DT == 70, "0xFX15 (LD DT, VX) should set DT to value of VX");

    state = chipbox_init_state();
    state.V[0xD] = 23;
    test(chipbox_cpu_eval_opcode(&state, 0xFD18), "0xFX18 (LD ST, VX) should succeed");
    test(state.ST == 23, "0xFX18 (LD ST, VX) should set ST to value of VX");

    state = chipbox_init_state();
    state.V[3] = 5;
    state.I = 0x400;
    test(chipbox_cpu_eval_opcode(&state, 0xF31E), "0xFX1E (ADD I, VX) should succeed");
    test(state.I == 0x405, "0xFX1E (ADD I, VX) should add the value of VX to I");

    state = chipbox_init_state();
    state.V[6] = 0xB;
    state.I = 0x500;
    test(chipbox_cpu_eval_opcode(&state, 0xF629), "0xFX29 (LD F, VX) should succeed on a valid value in VX");
    test(state.I == CHIPBOX_PROGRAM_START - CHIPBOX_FONT_TOTAL_BYTES + (CHIPBOX_FONT_SIZE * 0xB), "0xFX29 (LD F, VX) should set I to the memory address corresponding to the font character based on the value of VX");
    state = chipbox_init_state();
    state.V[6] = 0x10;
    test(!chipbox_cpu_eval_opcode(&state, 0xF629), "0xFX29 (LD F, VX) should fail on invalid value of VX");
    test(state.log_level == CHIPBOX_LOG_LEVEL_ERROR && state.log_msg == CHIPBOX_LOG_RANGE, "0xFX29 (LD F, VX) should raise a range error on invalid value of VX");

    state = chipbox_init_state();
    state.V[0xA] = 253;
    state.I = 0x301;
    test(chipbox_cpu_eval_opcode(&state, 0xFA33), "0xFX33 (LD B, VX) should succeed");
    test(state.memory[0x301] == 2 && state.memory[0x302] == 5 && state.memory[0x303] == 3, "0xFX33 (LD B, VX) should store the binary coded decimal value of VX at memory[I], memory[I+1] and memory[I+2], highest to lowest significance");

    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    state.V[0] = 2;
    state.V[1] = 99;
    state.V[2] = 42;
    state.V[3] = 255;
    state.I = 0x304;
    test(chipbox_cpu_eval_opcode(&state, 0xF355), "0xFX55 (LD [I], VX) should succeed");
    test(state.memory[0x304] == 2 && state.memory[0x305] == 99 && state.memory[0x306] == 42 && state.memory[0x307] == 255, "0xFX55 (LD [I], VX) should store registers V0 to VX in memory starting from address stored in I");
    test(state.I == 0x308, "0xFX55 (LD [I], VX) should set I to I + X + 1 in MATTMIK compatiblity mode");
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    test(state.I == 0x304, "0xFX55 (LD [I], VX) should leave I untouched in COWGOD compatibltiy mode");

    state = chipbox_init_state();
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_MATTMIK;
    state.memory[0x404] = 77;
    state.memory[0x405] = 123;
    state.memory[0x406] = 222;
    state.I = 0x404;
    test(chipbox_cpu_eval_opcode(&state, 0xF265), "0xFX65 (LD VX, [I]) should succeed");
    test(state.V[0] == 77 && state.V[1] == 123 && state.V[2] == 222, "0xFX65 (LD VX, [I]) should fill registers V0 to VX with memory starting from address in I");
    test(state.I == 0x407, "0xFX65 (LD VX, [I]) should set I to I + X + 1 in MATTMIK compatiblity mode");
    state.compat_mode = CHIPBOX_COMPATIBILITY_MODE_COWGOD;
    test(state.I == 0x404, "0xFX65 (LD VX, [I]) should leave I untouched in COWGOD compatiblity mode");

    /* END */
    printf("\n== END ==\n");
    printf("Tests: %d, failed: %d\n", tests, failed);
    return 0;
}

void test(int condition, char* name) {
    if (!condition) {
        printf("\n%4d FAILED: '%s'\n", tests, name);
        failed++;
    } else {
        printf(".");
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

void print_section(int section_num) {
    printf("\n===== SECTION %d =====\n", section_num);
}
