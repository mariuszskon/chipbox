#ifndef CHIPBOX_DASCORE_H
#define CHIPBOX_DASCORE_H

#include "core.h"

#define CHIPBOX_OPCODE_MNEMONIC_MAX_LENGTH 4
#define CHIPBOX_INSTRUCTION_MAX_ARGS 3
#define CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH 3

struct chipbox_instruction_info {
    char mnemonic[CHIPBOX_OPCODE_MNEMONIC_MAX_LENGTH+1];
    char args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1];
    int num_args;
};

struct chipbox_instruction_info disassemble_instruction(dbyte instruction);
void get_NNN_arg(struct chipbox_instruction_info *info, dbyte instruction);
void get_X_arg(struct chipbox_instruction_info *info, dbyte instruction);
void get_NN_arg(struct chipbox_instruction_info *info, dbyte instruction);
void get_Y_arg(struct chipbox_instruction_info *info, dbyte instruction);
void get_XY_args(struct chipbox_instruction_info *info, dbyte instruction);

#endif
