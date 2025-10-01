#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#define MEMLENGTH 4096

static union {
char bytes[MEMLENGTH];
double not_used;
} heap;

typedef struct {
  unsigned short prev;
  unsigned short next;
  unsigned short length;
  unsigned char is_free;
  unsigned char padding; // unused
} metadata;

static int heap_initalized = 0;

void * initialize_heap() {
  // toggle heap initialization flag 
  heap_initalized = 1;

  // register leak detector
  // atexit(check_for_leaks);

  // get pointer to start of the heap and write initial metadata for the single free chunk
  metadata *head = (metadata *)&heap.bytes;
  head->prev = 0;
  head->next = 0;
  head->length = MEMLENGTH - sizeof(metadata);
  head->is_free = 0;

  printf("Heap initialized...\n");
  printf("Metadata size: %zu bytes\n", sizeof(metadata));
  printf("Initial free chunk size: %u bytes\n", head->length);
  printf("Heap starts at: %p\n", (void*)head);
  printf("Payload starts at: %p\n", (void*)(head+1));
}

void check_for_leaks() {
  // traverse heap and report a leak
  // a leak occurs when there is a chunk marked "allocated" on the heap

}

void * mymalloc(size_t size, char *file, int line) {

}


void myfree(void *ptr, char *file, int line) {

}

int main(void) {
  initialize_heap();
}
