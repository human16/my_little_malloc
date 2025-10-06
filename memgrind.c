#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mymalloc.h"

int first_test() {
    for (int i = 0; i < 120; i++) {
        char *p1 = malloc(1);
        if (p1 == NULL) {
            return EXIT_FAILURE;
        }
        free(p1);
    }
    return EXIT_SUCCESS;
}

int second_test() {
    char *pointers[120];
    for (int i = 0; i<120; i++) {
        pointers[i] = malloc(1);
        if (pointers[i] == NULL) {
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i<120; i++) {
        free(pointers[i]);
    }
    return EXIT_SUCCESS;
}


int run_test(int (*test)(), int iterations, int test_num) {
    struct timeval init_time, end_time;
    gettimeofday(&init_time, NULL);
    for (int i=0; i<iterations; i++) {
        if (test()) {
            printf("Test %d failed in the %d iteration\n", test_num, i+1);
            return EXIT_FAILURE;
        }
    }
    gettimeofday(&end_time, NULL);
    double elapsed = (end_time.tv_sec - init_time.tv_sec) + (end_time.tv_usec - init_time.tv_usec)/1000000.0; //in seconds
    printf("Test %d passed with an average time of %f seconds\n", test_num, (elapsed)/iterations);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    run_test(first_test, 50, 1);
    run_test(second_test, 50, 2);
    return EXIT_SUCCESS;
}