#include <stdio.h>
#include <stdlib.h>
#define MEMLENGTH 4096

static union {
char bytes[MEMLENGTH];
double not_used;
} heap;


void * mymalloc(size_t size, char *file, int line) {

}


void myfree(void *ptr, char *file, int line) {

}