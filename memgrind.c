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

int third_test() {
    char *pointers[120];
    int number_of_allocs = 0;
    for (int i = 0; i < 120; i++) {
        if (rand() % 2 == 0) {
            pointers[i] = malloc(1);
            if (pointers[i] == NULL) {
                return EXIT_FAILURE;
            }
            number_of_allocs++;
        } else {
            if (number_of_allocs > 0) {
                int free_number = rand() % number_of_allocs;
                for (int j = 0; j < i; j++) {
                    if (pointers[j] != NULL) {
                        if (free_number == 0) {
                            free(pointers[j]);
                            pointers[j] = NULL;
                            number_of_allocs--;
                            break;
                        }
                        free_number--;
                    }
                }
            }
        }
    }
    for (int i = 0; i < 120; i++) {
        if (pointers[i] != NULL) {
            free(pointers[i]);
        }
    }
    return EXIT_SUCCESS;
}


struct Node {
    struct Node *next;
    int data;
};

int fourth_test() {
    struct Node *curr = malloc(sizeof(struct Node));
    if (curr == NULL) {
        return EXIT_FAILURE;
    }
    curr->next = NULL;
    curr->data = 0;
    struct Node *first_node = curr;
    for (int i = 1; i < 120; i++) {
        struct Node *next_node = malloc(sizeof(struct Node));
        if (next_node == NULL) {
            return EXIT_FAILURE;
        }
        next_node->data = i;
        next_node->next = curr;
        curr = next_node;
    }
    curr = first_node;
    while (curr != NULL) {
        first_node = curr->next;
        free(curr);
        curr = first_node;
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
    run_test(third_test, 50, 3);
    //run_test(fourth_test, 2, 4);
    return EXIT_SUCCESS;
}