#include "core.h"
#include "dascore.h"
#include "testhelpers.h"
#include <string.h>

int main() {
    struct chipbox_instruction_info info;

    info = disassemble_instruction(0x00E0);
    test(strcmp(info.mnemonic, "CLS") == 0, "0x00E0 should be CLS");
    test(info.num_args == 0, "0x00E0 (CLS) should have no arguments");

    info = disassemble_instruction(0x00EE);
    test(strcmp(info.mnemonic, "RET") == 0, "0x00EE should be RET");
    test(info.num_args == 0, "0x00EE (RET) should have no arguments");

    print_end();
    return 0;
}
