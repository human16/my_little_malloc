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
    //simulate the usage of a linked list
    struct Node *head = NULL;
    for (int i = 0; i < 120; i++) {
        struct Node *new_node = malloc(sizeof(struct Node));
        if (new_node == NULL) {
            return EXIT_FAILURE;
        }
        new_node->data = i;
        new_node->next = head;
        head = new_node;
    }

    while (head != NULL) {
        struct Node *temp = head;
        head = head->next;
        free(temp);
    }
    return EXIT_SUCCESS;
}

struct binary_node {
    struct binary_node *left;
    struct binary_node *right;
    int data;
};

void free_tree(struct binary_node *node) {
    if (node == NULL) {
        return;
    }
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

int fifth_test() {
    //simulate a binary tree
    struct binary_node *root = malloc(sizeof(struct binary_node));
    if (root == NULL) {
        return EXIT_FAILURE;
    }
    root->data = 0;
    root->left = NULL;
    root->right = NULL;

    struct binary_node *nodes[120];
    nodes[0] = root;
    int node_count = 1;

    for (int i = 1; i < 120; i++) {
        struct binary_node *new_node = malloc(sizeof(struct binary_node));
        if (new_node == NULL) {
            free_tree(root);
            return EXIT_FAILURE;
        }
        new_node->data = i;
        new_node->left = NULL;
        new_node->right = NULL;
        nodes[node_count] = new_node;

        // Insert into tree - alternate between left and right children
        int parent_index = (node_count - 1) / 2;
        if (node_count % 2 == 1) {
            nodes[parent_index]->left = new_node;
        } else {
            nodes[parent_index]->right = new_node;
        }
        node_count++;
    }

    free_tree(root);
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
    run_test(fourth_test, 50, 4);
    return EXIT_SUCCESS;
}