#include "core.h"
#include "dascore.h"
#include "testhelpers.h"
#include <stdio.h>
#include <string.h>

#define MAX_MESSAGE_LENGTH 128

#define DASI(DBYTE) (info = disassemble_instruction(DBYTE))
#define MEQ(MNEMONIC) (strcmp(info.mnemonic, MNEMONIC) == 0)
#define NOARGS (info.num_args == 0)
#define ONEARG(ARG1) (info.num_args == 1 && strcmp(info.args[0], ARG1) == 0)
#define TWOARGS(ARG1, ARG2) (info.num_args == 2 && strcmp(info.args[0], ARG1) == 0 && strcmp(info.args[1], ARG2) == 0)

void test_instruction(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], int expected_args);
void test_noargs(dbyte actual_test, char *generic_dbyte, char *mnemonic);
void test_onearg(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char *arg1);
void test_twoargs(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char *arg1, char *arg2);
int args_equal(char given_args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], char test_args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], int num_args);

int main() {
    struct chipbox_instruction_info info;

    test_noargs(0x00E0, "0x00E0", "CLS");

    DASI(0x00EE);
    test(MEQ("RET"), "0x00EE should be RET");
    test(NOARGS, "0x00EE (RET) should have no arguments");

    DASI(0x01EE);
    test(MEQ("SYS"), "0x0NNN should be SYS if not CLS or RET");
    test(ONEARG("1EE"), "0x0NNN (SYS NNN) should have one argument: NNN");

    test_onearg(0x1234, "0x1NNN", "JP", "NNN", "234");

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

    DASI(0x8AB0);
    test(MEQ("LD"), "0x8XY0 should be LD");
    test(TWOARGS("VA", "VB"), "0x8XY0 (LD VX, VY) should have two arguments: VX and VY");

    print_end();
    return 0;
}

void test_instruction(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char args[][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], int expected_args) {
    struct chipbox_instruction_info info = disassemble_instruction(actual_test);
    char message[MAX_MESSAGE_LENGTH];

    sprintf(message, "%s should be %s", generic_dbyte, mnemonic);
    test(strcmp(info.mnemonic, mnemonic) == 0, message);

    sprintf(message, "%s (%s %s) should have %d argument(s)", generic_dbyte, mnemonic, generic_args, expected_args);
    test(info.num_args == expected_args && args_equal(info.args, args, expected_args), message);
}

void test_noargs(dbyte actual_test, char *generic_dbyte, char *mnemonic) {
    test_instruction(actual_test, generic_dbyte, mnemonic, "", NULL, 0);
}

void test_onearg(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char *arg1) {
    char args[1][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1];
    strcpy(args[0], arg1);
    test_instruction(actual_test, generic_dbyte, mnemonic, generic_args, args, 1);
}

void test_twoargs(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char *arg1, char *arg2) {
    char args[2][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1];
    strcpy(args[0], arg1);
    strcpy(args[1], arg2);
    test_instruction(actual_test, generic_dbyte, mnemonic, generic_args, args, 2);
}

int args_equal(char given_args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], char test_args[][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], int num_args) {
    int i;
    for (i = 0; i < num_args; i++) {
        if (strcmp(given_args[i], test_args[i]) != 0) {
            return 0;
        }
    }

    return 1;
}
