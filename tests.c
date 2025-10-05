#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"

int test1() {
  //BASIC
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
  //MALLOC SO BIG
  char *obj = malloc(4096);
  if (obj == NULL) {
    return EXIT_SUCCESS; // pass test
  }
  return EXIT_FAILURE; // Curiouser and curiouser
}

int test3() { //CHECKED, PASSES
  //DOUBLE FREE
  char *obj = malloc(100);
  if (obj == NULL) {
    return EXIT_FAILURE;
  }
  free(obj);
  free(obj);
  return EXIT_FAILURE;
}

int test4() { //CHECKED, PASSES AND FLAGGED LEAKED MEMORY
  //INVALID POINTER
  char *obj = malloc(100);
  free(obj+1);
  return EXIT_FAILURE;
}

int test5() {
  //CHECKING 8-BYTE ALIGNMENT AND OVERFILLING
  char *obj[256];
  for (int i = 0; i < 256; i++) {
    obj[i] = malloc(1);
    if (obj[i] == NULL) {
      return EXIT_FAILURE;
    }
  }
  char *extra = malloc(1);
  if (extra != NULL) {
    return EXIT_FAILURE;
  }
  for (int i = 0; i<256; i++) {
    free(obj[i]);
  }
  free(extra);
  return EXIT_SUCCESS;
}

int test6() {
  //CHECK COALESCING CASE 3
  char *obj1 = malloc(100); //000---------------------------
  char *obj2 = malloc(100); //000000------------------------
  free(obj1);               //---000------------------------
  free(obj2);               //------------------------------
  char *obj3 = malloc(4000);
  if (obj3 == NULL) {
    return EXIT_FAILURE;
  }
  free(obj3);
  return EXIT_SUCCESS;
}

int test7() {
  //CHECK COALESCING CASE 2
  char *obj1 = malloc(100); //000---------------------------
  char *obj2 = malloc(100); //000000------------------------
  free(obj2);               //000---------------------------
  free(obj1);               //------------------------------
  char *obj3 = malloc(4000);
  if (obj3 == NULL) {
    return EXIT_FAILURE;
  }
  free(obj3);
  return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
  int (*tests[])() = {
    test1, test2, //test5, test6, test7, 
  };
  double num_tests = sizeof(tests) / sizeof(tests[0]);
  int num_passed = 0;
  for (int i = 0; i < num_tests; i++) {
    if (tests[i]() == EXIT_SUCCESS) {
        num_passed++;
        printf("| Test #%d passed \n", i+1);
    } else {
        printf("| Test #%d failed \n", i+1);
    }
  }
  printf("%d / %d tests passed\n", num_passed, (int)num_tests);
  return 0;
}