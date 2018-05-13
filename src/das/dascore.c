#include "core.h"
#include "dascore.h"
#include <string.h>

struct chipbox_instruction_info disassemble_instruction(dbyte instruction) {
    struct chipbox_instruction_info info;

    switch ((instruction & 0xF000) >> 12) { /* get highest/left-most nybble */

    }

    /* if we have not returned yet, the instruction was unknown */
    strcpy(info.mnemonic, "???");
    info.num_args = 0;
    return info;
}
