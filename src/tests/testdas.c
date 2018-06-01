#include "core.h"
#include "dascore.h"
#include "testhelpers.h"
#include <string.h>

#define DASI(DBYTE) (info = disassemble_instruction(DBYTE))
#define MEQ(MNEMONIC) (strcmp(info.mnemonic, MNEMONIC) == 0)
#define NOARGS (info.num_args == 0)
#define ONEARG(ARG1) (info.num_args == 1 && strcmp(info.args[0], ARG1) == 0)
#define TWOARGS(ARG1, ARG2) (info.num_args == 2 && strcmp(info.args[0], ARG1) == 0 && strcmp(info.args[1], ARG2) == 0)

int main() {
    struct chipbox_instruction_info info;

    DASI(0x00E0);
    test(MEQ("CLS"), "0x00E0 should be CLS");
    test(NOARGS, "0x00E0 (CLS) should have no arguments");

    DASI(0x00EE);
    test(MEQ("RET"), "0x00EE should be RET");
    test(NOARGS, "0x00EE (RET) should have no arguments");

    DASI(0x01EE);
    test(MEQ("SYS"), "0x0NNN should be SYS if not CLS or RET");
    test(ONEARG("1EE"), "0x0NNN (SYS NNN) should have one argument: NNN");

    DASI(0x1234);
    test(MEQ("JP"), "0x1NNN should be JP");
    test(ONEARG("234"), "0x1NNN (JP NNN) should have one argument: NNN");

    DASI(0x2456);
    test(MEQ("CALL"), "0x2NNN should be CALL");
    test(ONEARG("456"), "0x2NNN (CALL NNN) should have one argument: NNN");

    DASI(0x35AB);
    test(MEQ("SE"), "0x3XNN should be SE");
    test(TWOARGS("V5", "AB"), "0x3XNN (SE VX, NN) should have two arguments: VX and NN");

    DASI(0x41DE);
    test(MEQ("SNE"), "0x4XNN should be SNE");
    test(TWOARGS("V1", "DE"), "0x4XNN (SNE VX, NN) should have two arguments: VX and NN");

    DASI(0x5AB0);
    test(MEQ("SE"), "0x5XY0 should be SE");
    test(TWOARGS("VA", "VB"), "0x5XY0 (SE VX, VY) should have two arguments: VX and VY");

    DASI(0x61CD);
    test(MEQ("LD"), "0x6XNN should be LD");
    test(TWOARGS("V1", "CD"), "0x6XNN (LD VX, NN) should have two arguments: VX and NN");

    DASI(0x7242);
    test(MEQ("ADD"), "0x7XNN should be ADD");
    test(TWOARGS("V1", "42"), "0x7XNN (ADD VX, NN) should have two argument: VX and NN");

    print_end();
    return 0;
}
