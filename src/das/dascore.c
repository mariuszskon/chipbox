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
        case 9:
            if ((instruction & 0x000F) == 0) {
                return get_XY_args(&info, instruction, "SNE");
            }
            break; /* 0x9XYZ, Z != 0 is undefined */
        case 0xA:
            strcpy(info.args[info.num_args], "I");
            info.num_args++;
            return get_NNN_arg(&info, instruction, "LD");
        case 0xB:
            strcpy(info.args[info.num_args], "V0");
            info.num_args++;
            return get_NNN_arg(&info, instruction, "JP");
        case 0xC:
            return get_XNN_args(&info, instruction, "RND");
        case 0xD:
            get_XY_args(&info, instruction, "DRW");
            sprintf(info.args[info.num_args], "%X", instruction & 0x000F);
            info.num_args++;
            return info;
        case 0xE:
            switch (instruction & 0x00FF) {
                case 0x9E:
                    return get_X_arg(&info, instruction, "SKP");
                case 0xA1:
                    return get_X_arg(&info, instruction, "SKNP");
            }
            break; /* unknown instruction of form 0xEXNN - NN is unknown */
        case 0xF:
            switch (instruction & 0x00FF) {
                case 0x07:
                    get_X_arg(&info, instruction, "LD");
                    strcpy(info.args[info.num_args], "DT");
                    info.num_args++;
                    return info;
                case 0x0A:
                    get_X_arg(&info, instruction, "LD");
                    strcpy(info.args[info.num_args], "K");
                    info.num_args++;
                    return info;
                case 0x15:
                    strcpy(info.args[info.num_args], "DT");
                    info.num_args++;
                    return get_X_arg(&info, instruction, "LD");
                case 0x18:
                    strcpy(info.args[info.num_args], "ST");
                    info.num_args++;
                    return get_X_arg(&info, instruction, "LD");
                case 0x1E:
                    strcpy(info.args[info.num_args], "I");
                    info.num_args++;
                    return get_X_arg(&info, instruction, "ADD");
                case 0x29:
                    strcpy(info.args[info.num_args], "F");
                    info.num_args++;
                    return get_X_arg(&info, instruction, "LD");
                case 0x33:
                    strcpy(info.args[info.num_args], "B");
                    info.num_args++;
                    return get_X_arg(&info, instruction, "LD");
                case 0x55:
                    strcpy(info.args[info.num_args], "[I]");
                    info.num_args++;
                    return get_X_arg(&info, instruction, "LD");
                case 0x65:
                    get_X_arg(&info, instruction, "LD");
                    strcpy(info.args[info.num_args], "[I]");
                    info.num_args++;
                    return info;
            }
            break; /* 0xFXNN, NN unknown */
    }

    /* if we have not returned yet, the instruction was unknown */
    strcpy(info.mnemonic, "???");
    return info;
}

struct chipbox_instruction_info get_NNN_arg(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_no_args(info, mnemonic);
    sprintf(info->args[info->num_args], "%X", instruction & 0x0FFF);
    info->num_args++;
    return *info;
}

struct chipbox_instruction_info get_X_arg(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_no_args(info, mnemonic);
    sprintf(info->args[info->num_args], "V%X", (instruction & 0x0F00) >> 8);
    info->num_args++;
    return *info;
}

struct chipbox_instruction_info get_NN_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    sprintf(info->args[info->num_args], "%02X", instruction & 0x00FF);
    info->num_args++;
    return *info;
}

struct chipbox_instruction_info get_XNN_args(struct chipbox_instruction_info *info, dbyte instruction, char *mnemonic) {
    get_X_arg(info, instruction, mnemonic);
    get_NN_arg(info, instruction);
    return *info;
}

struct chipbox_instruction_info get_Y_arg(struct chipbox_instruction_info *info, dbyte instruction) {
    sprintf(info->args[info->num_args], "V%X", (instruction & 0x00F0) >> 4);
    info->num_args++;
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
