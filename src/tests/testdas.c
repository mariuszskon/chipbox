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
void test_threeargs(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char *arg1, char *arg2, char *arg3);
int args_equal(char given_args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], char test_args[CHIPBOX_INSTRUCTION_MAX_ARGS][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1], int num_args);

int main() {
    test_noargs(0x00E0, "0x00E0", "CLS");

    test_noargs(0x00EE, "0x00EE", "RET");

    test_onearg(0x01EE, "0x0NNN", "SYS", "NNN", "1EE");

    test_onearg(0x1234, "0x1NNN", "JP", "NNN", "234");

    test_onearg(0x2456, "0x2NNN", "CALL", "NNN", "456");

    test_twoargs(0x35AB, "0x3XNN", "SE", "VX, NN", "V5", "AB");

    test_twoargs(0x41DE, "0x4XNN", "SNE", "VX, NN", "V1", "DE");

    test_twoargs(0x5AB0, "0x5XY0", "SE", "VX, VY", "VA", "VB");

    test_twoargs(0x61CD, "0x6XNN", "LD", "VX, NN", "V1", "CD");

    test_twoargs(0x7242, "0x7XNN", "ADD", "VX, NN", "V1", "42");

    test_twoargs(0x8AB0, "0x8XY0", "LD", "VX, VY", "VA", "VB");

    test_twoargs(0x8421, "0x8XY1", "OR", "VX, VY", "V4", "V2");

    test_twoargs(0x8532, "0x8XY2", "AND", "VX, VY", "V5", "V3");

    test_twoargs(0x8643, "0x8XY3", "XOR", "VX, VY", "V6", "V4");

    test_twoargs(0x8094, "0x8XY4", "ADD", "VX, VY", "V0", "V9");

    test_twoargs(0x8BE5, "0x8XY5", "SUB", "VX, VY", "VB", "VE");

    test_twoargs(0x8EF6, "0x8XY6", "SHR", "VX, VY", "VE", "VF");

    test_twoargs(0x8AD7, "0x8XY7", "SUBN", "VX, VY", "VA", "VD");

    test_twoargs(0x8DFE, "0x8XYE", "SHL", "VX, VY", "VD", "VF");

    test_twoargs(0x9AB0, "0x9XY0", "SNE", "VX, VY", "VA", "VB");

    test_twoargs(0xA432, "0xANNN", "LD", "I, NNN", "I", "432");

    test_twoargs(0xB654, "0xBNNN", "JP", "V0, NNN", "V0", "654");

    test_twoargs(0xCE0A, "0xCXNN", "RND", "VX, NN", "VE", "0A");

    test_threeargs(0xDAB8, "0xDXYN", "DRW", "VX, VY, N", "VA", "VB", "8");

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

void test_threeargs(dbyte actual_test, char *generic_dbyte, char *mnemonic, char *generic_args, char *arg1, char *arg2, char *arg3) {
    char args[3][CHIPBOX_INSTRUCTION_MAX_ARG_LENGTH+1];
    strcpy(args[0], arg1);
    strcpy(args[1], arg2);
    strcpy(args[2], arg3);
    test_instruction(actual_test, generic_dbyte, mnemonic, generic_args, args, 3);
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
