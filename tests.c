#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int test1() {
    char *obj[2];
    obj[0] = malloc(100);
    obj[1] = malloc(100);
    if (obj[0] == NULL || obj[1] == NULL) {
        return EXIT_FAILURE;
    }
    free(obj[0]);
    free(obj[1]);
    return EXIT_SUCCESS;
}

int test2() {
    return EXIT_FAILURE; // create test
}

int main(int argc, char **argv) {
  int (*tests[])() = {
    test1, test2, 
  };
  double num_tests = sizeof(tests) / sizeof(tests[0]);
  int num_passed = 0;
  for (int i = 0; i < num_tests; i++) {
    if (tests[i]() == EXIT_SUCCESS) {
        num_passed++;
        printf("| Test %d passed \n", i+1);
    } else {
        printf("| Test %d failed \n", i+1);
    }
  }
  printf("%d / %d tests passed\n", num_passed, (int)num_tests);
  return 0;
}