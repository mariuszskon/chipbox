#include "dascore.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct chipbox_instruction_info info = disassemble_instruction(0x3502);
    printf("%s\n", info.mnemonic);
    return 0;
}
