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
            get_XNN_args(&info, instruction);
            return info;
        case 4:
            strcpy(info.mnemonic, "SNE");
            get_XNN_args(&info, instruction);
            return info;
        case 5:
            if ((instruction & 0x000F) == 0) {
                strcpy(info.mnemonic, "SE");
                get_XY_args(&info, instruction);
                return info;
            }
            break; /* 0x5XYZ where Z != 0 is undefined */
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

void get_XNN_args(struct chipbox_instruction_info *info, dbyte instruction) {
    get_X_arg(info, instruction);
    get_NN_arg(info, instruction);
}

void get_Y_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    info->num_args++;
    sprintf(info->args[info->num_args-1], "V%X", (instruction & 0x00F0) >> 4);
}

void get_XY_args(struct chipbox_instruction_info *info, dbyte instruction) {
    get_X_arg(info, instruction);
    get_Y_arg(info, instruction);
}
