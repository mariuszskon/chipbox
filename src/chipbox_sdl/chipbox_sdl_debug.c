#include "chipbox_sdl_debug.h"
#include "dascore.h"
#include <stdio.h>

void chipbox_print_debug_instruction(dbyte pc, dbyte opcode) {
    print_chipbox_instruction_info(pc, opcode, disassemble_instruction(opcode));
}

void chipbox_print_debug_state_dump(struct chipbox_chip8_state *state) {
    printf("Dump: TODO\n");
}
