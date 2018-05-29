#include "core.h"
#include "dascore.h"
#include "testhelpers.h"
#include <string.h>

int main() {
    struct chipbox_instruction_info info;

    info = disassemble_instruction(0x00E0);
    test(strcmp(info.mnemonic, "CLS") == 0, "0x00E0 should be CLS");
    test(info.args == 0, "0x00E0 (CLS) should have no arguments");

    print_end();
    return 0;
}
