#include "core.h"
#include "das.h"
#include "dascore.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s [file]\n", argv[0]);
        return 1;
    }

    return !disassemble_file(argv[1]);
}

int disassemble_file(char *filename) {
    int c;
    dbyte instruction;
    dbyte pc = CHIPBOX_PROGRAM_START; /* ensure address is offset correctly to allow for easier reading of e.g. jumps */
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Could not open file '%s'", filename);
        return 0;
    }

    while ((c = fgetc(fp)) != EOF) {
        instruction = (((byte) c) << 8);
        pc++;
        if ((c = fgetc(fp)) == EOF) {
            /* since every chip-8 instruction is 2 bytes long, an EOF should not be reached is we have read 1 byte */
            fprintf(stderr, "Unexpected end-of-file at 0x%04X (actual byte %X/%u)", pc, pc - CHIPBOX_PROGRAM_START, pc - CHIPBOX_PROGRAM_START);
            return 0;
        }
        instruction |= (byte) c;
        pc++;
        print_chipbox_instruction_info(pc - 2, instruction, disassemble_instruction(instruction));
    }

    fclose(fp);

    return 1;
}

void print_chipbox_instruction_info(dbyte pc, dbyte instruction, struct chipbox_instruction_info info) {
    int i;
    printf("%04X: %04X    %-*s ", pc, instruction, CHIPBOX_OPCODE_MNEMONIC_MAX_LENGTH, info.mnemonic);
    for (i = 0; i < info.num_args; i++) {
        printf(" %s", info.args[i]);
        if (i != info.num_args - 1) { /* not last argument */
            printf(",");
        }
    }
    printf("\n");
}
