#include <stdio.h>
#include "core.h"
#include "testhelpers.h"

int tests = 0;
int failed = 0;

void test(int condition, char* name) {
    if (!condition) {
        printf("\n%4d FAILED: '%s'\n", tests, name);
        failed++;
    } else {
        printf(".");
    }
    tests++;
}

int all_equal(byte array[], int size, int value) {
    int i;
    for (i = 0; i < size; i++) {
        if (array[i] != value) {
            return 0;
        }
    }
    return 1;
}

void print_section(int section_num) {
    printf("\n===== SECTION %d =====\n", section_num);
}

void print_end() {
    printf("\n======== END ========\n");
    printf("Tests: %d, failed: %d\n", tests, failed);
}
