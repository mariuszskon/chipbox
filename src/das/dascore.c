#include "core.h"
#include "dascore.h"
#include <string.h>
#include <stdio.h>

struct chipbox_instruction_info disassemble_instruction(dbyte instruction) {
    struct chipbox_instruction_info info;
    info.num_args = 0;

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
        case 1:
            strcpy(info.mnemonic, "JP");
            get_NNN_arg(&info, instruction);
            return info;
        case 2:
            strcpy(info.mnemonic, "CALL");
            get_NNN_arg(&info, instruction);
            return info;
        case 3:
            strcpy(info.mnemonic, "SE");
            get_X_arg(&info, instruction);
            get_NN_arg(&info, instruction);
            return info;
    }

    /* if we have not returned yet, the instruction was unknown */
    strcpy(info.mnemonic, "???");
    return info;
}

void get_NNN_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    info->num_args = 1;
    sprintf(info->args[info->num_args-1], "%X", instruction & 0x0FFF);
}

void get_X_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    info->num_args++;
    sprintf(info->args[info->num_args-1], "V%X", (instruction & 0x0F00) >> 8);
}

void get_NN_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    info->num_args++;
    sprintf(info->args[info->num_args-1], "%02X", instruction & 0x00FF);
}
