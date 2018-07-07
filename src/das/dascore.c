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
                    return get_no_args(&info, "CLS");
                case 0x00EE:
                    return get_no_args(&info, "RET");
                default:
                    return get_NNN_arg(&info, instruction, "SYS");
            }
        case 1:
            return get_NNN_arg(&info, instruction, "JP");
        case 2:
            return get_NNN_arg(&info, instruction, "CALL");
        case 3:
            return get_XNN_args(&info, instruction, "SE");
        case 4:
            return get_XNN_args(&info, instruction, "SNE");
        case 5:
            if ((instruction & 0x000F) == 0) {
                return get_XY_args(&info, instruction, "SE");
            }
            break; /* 0x5XYZ where Z != 0 is undefined */
        case 6:
            return get_XNN_args(&info, instruction, "LD");
        case 7:
            return get_XNN_args(&info, instruction, "ADD");
        case 8:
            switch (instruction & 0x000F) {
                case 0:
                    return get_XY_args(&info, instruction, "LD");
                case 1:
                    return get_XY_args(&info, instruction, "OR");
                case 2:
                    return get_XY_args(&info, instruction, "AND");
                case 3:
                    return get_XY_args(&info, instruction, "XOR");
                case 4:
                    return get_XY_args(&info, instruction, "ADD");
                case 5:
                    return get_XY_args(&info, instruction, "SUB");
                case 6:
                    return get_XY_args(&info, instruction, "SHR");
                case 7:
                    return get_XY_args(&info, instruction, "SUBN");
                case 0xE:
                    return get_XY_args(&info, instruction, "SHL");
            }
            break; /* fall through to unknown instruction handling - 0x8XYN where N is unexcepted value */
    }

    /* if we have not returned yet, the instruction was unknown */
    strcpy(info.mnemonic, "???");
    return info;
}

struct chipbox_instruction_info get_NNN_arg(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_no_args(info, mnemonic);
    info->num_args = 1;
    sprintf(info->args[info->num_args-1], "%X", instruction & 0x0FFF);
    return *info;
}

struct chipbox_instruction_info get_X_arg(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_no_args(info, mnemonic);
    info->num_args++;
    sprintf(info->args[info->num_args-1], "V%X", (instruction & 0x0F00) >> 8);
    return *info;
}

struct chipbox_instruction_info get_NN_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    info->num_args++;
    sprintf(info->args[info->num_args-1], "%02X", instruction & 0x00FF);
    return *info;
}

struct chipbox_instruction_info get_XNN_args(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_X_arg(info, instruction, mnemonic);
    get_NN_arg(info, instruction);
    return *info;
}

struct chipbox_instruction_info get_Y_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    info->num_args++;
    sprintf(info->args[info->num_args-1], "V%X", (instruction & 0x00F0) >> 4);
    return *info;
}

struct chipbox_instruction_info get_XY_args(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_X_arg(info, instruction, mnemonic);
    get_Y_arg(info, instruction);
    return *info;
}

struct chipbox_instruction_info get_no_args(struct chipbox_instruction_info *info, char *mnemonic) {
    strcpy(info->mnemonic, mnemonic);
    return *info;
}
