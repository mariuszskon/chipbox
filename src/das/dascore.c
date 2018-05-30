#include "core.h"
#include "dascore.h"
#include <string.h>
#include <stdio.h>

struct chipbox_instruction_info disassemble_instruction(dbyte instruction) {
    struct chipbox_instruction_info info;

    switch (instruction >> 12) { /* get highest/left-most nybble */
        case 0:
            switch (instruction) {
                case 0x00E0:
                    strcpy(info.mnemonic, "CLS");
                    info.num_args = 0;
                    return info;
                case 0x00EE:
                    strcpy(info.mnemonic, "RET");
                    info.num_args = 0;
                    return info;
                default:
                    strcpy(info.mnemonic, "SYS");
                    get_NNN_arg(&info, instruction);
                    return info;
            }
    }

    /* if we have not returned yet, the instruction was unknown */
    strcpy(info.mnemonic, "???");
    info.num_args = 0;
    return info;
}

void get_NNN_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    char temp_arg[CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1];

    info->num_args = 1;
    sprintf(temp_arg, "%X", instruction & 0x0FFF);
    strcpy(info->args[0], temp_arg);
}
