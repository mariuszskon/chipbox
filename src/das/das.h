#ifndef CHIPBOX_DAS_H
#define CHIPBOX_DAS_H

#include "core.h"
#include "dascore.h"

int disassemble_file(char *filename);
void print_chipbox_instruction_info(dbyte pc, dbyte instruction, struct chipbox_instruction_info info);

#endif
