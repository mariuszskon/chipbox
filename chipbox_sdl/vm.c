#include "vm.h"
#include "chipbox_sdl.h"

int chipbox_vm_step(struct chipbox_chip8_state* state) {
    dbyte opcode;
    if (!((opcode = chipbox_cpu_get_opcode(state)) && chipbox_cpu_eval_opcode(state, opcode))) {
        chipbox_print_log(state);
        return 0;
    } else {
        chipbox_print_log(state);
        /* TODO: manage DT and ST */
        return 1;
    }
}

void chipbox_print_log(struct chipbox_chip8_state* state) {
    
}
