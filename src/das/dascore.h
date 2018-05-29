#ifndef CHIPBOX_DASCORE_H
#define CHIPBOX_DASCORE_H

#include "core.h"

#define CHIPBOX_OPCODE_MNEMONIC_MAX_LENGTH 4
#define CHIPBOX_INSTRUCTION_MAX_ARGS 2
#define CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH 3

struct chipbox_instruction_info {
    char mnemonic[CHIPBOX_OPCODE_MNEMONIC_MAX_LENGTH+1];
    char args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1];
    int num_args;
};

struct chipbox_instruction_info disassemble_instruction(dbyte instruction);

#endif
