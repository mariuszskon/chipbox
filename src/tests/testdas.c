#include "core.h"
#include "dascore.h"
#include "testhelpers.h"
#include <string.h>

#define MEQ(MNEMONIC) (strcmp(info.mnemonic, MNEMONIC) == 0)

int main() {
    struct chipbox_instruction_info info;

    info = disassemble_instruction(0x00E0);
    test(MEQ("CLS"), "0x00E0 should be CLS");
    test(info.num_args == 0, "0x00E0 (CLS) should have no arguments");

    info = disassemble_instruction(0x00EE);
    test(MEQ("RET"), "0x00EE should be RET");
    test(info.num_args == 0, "0x00EE (RET) should have no arguments");

    info = disassemble_instruction(0x01EE);
    test(MEQ("SYS"), "0x0NNN should be SYS if not CLS or RET");
    test(info.num_args == 1 && strcmp(info.args[0], "1EE") == 0, "0x0NNN (SYS NNN) should have one argument: NNN");

    info = disassemble_instruction(0x1234);
    test(MEQ("JP"), "0x1NNN should be JP");
    test(info.num_args == 1 && strcmp(info.args[0], "234") == 0, "0x1NNN (JP NNN) should have one argument: NNN");

    info = disassemble_instruction(0x2456);
    test(MEQ("CALL"), "0x2NNN should be CALL");
    test(info.num_args == 1 && strcmp(info.args[0], "456") == 0, "0x2NNN (CALL NNN) should have one argument: NNN");

    info = disassemble_instruction(0x35AB);
    test(MEQ("SE"), "0x3XNN should be SE");
    test(info.num_args == 2 && strcmp(info.args[0], "V5") == 0 && strcmp(info.args[1], "AB") == 0, "0x3XNN (SE VX, NN) should have two arguments: VX and NN");

    print_end();
    return 0;
}
