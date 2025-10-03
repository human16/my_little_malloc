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
  unsigned char is_allocated;
  unsigned char padding; // unused
} metadata;

void create_metadata(metadata *md, unsigned short prev, unsigned short next, unsigned short length, unsigned char is_allocated) {
    md->prev = prev;
    md->next = next;
    md->length = length;
    md->is_allocated = is_allocated;
    md->padding = 0;
}

metadata *get_metadata(char *pointer) {
    return (metadata *)pointer;
}

static int heap_initalized = 0;

void * initialize_heap() {
  // toggle heap initialization flag 
  heap_initalized = 1;

  // register leak detector
  // atexit(check_for_leaks);

  // get pointer to start of the heap and write initial metadata for the single free chunk
  metadata *head = get_metadata(heap.bytes);
  create_metadata(head, 0, 0, MEMLENGTH - sizeof(metadata), 0);

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
  if (!heap_initalized) {
    initialize_heap();
  }

  size_t size_rounded = size;
  if (size % 8 != 0) {
    size_rounded = size + (8 - (size % 8));
  }

  metadata *curr = (metadata *)&heap.bytes[0];

  while (1) {
    if (curr->is_allocated && curr->length >= size_rounded) {

      // check if we should split the block
      size_t remaining = curr->length - size_rounded;
      if (remaining >= sizeof(metadata) + 8) {
        metadata *new_block = (metadata *)((char *)(curr + 1) + size_rounded);
        new_block->prev = (char *)curr - heap.bytes;
        new_block->next = curr->next;
        new_block->length = remaining - sizeof(metadata);
        new_block->is_allocated = 1;

        // update curr
        curr->length = size_rounded;
        curr->next = (char *)new_block - heap.bytes;

        // update next block prev pointer if it exists
        if (new_block->next != 0) {
          metadata *next = (metadata *)&heap.bytes[new_block->next];
          next->prev = (char *)new_block - heap.bytes;
        }
      }

      curr->is_allocated = 0;

      return (void *)(curr + 1);
    }

    // next block
    if (curr->next == 0) {
      fprintf(stderr, "Error in %s line %d: malloc of %zu bytes failed\n", file, line, size);
      return NULL;
    }

    curr = (metadata *)&heap.bytes[curr->next];
  }

  return NULL;
}


void myfree(void *ptr, char *file, int line) {
  if (!ptr) return;
  if (!heap_initalized) {
    initialize_heap();
  }
  metadata *md = get_metadata(ptr-sizeof(metadata));
  if (md->is_allocated) {
    printf("Freeing a free chunk at %p in file %s line %d", ptr, file, line);
    return; // ADD ATEXIT TO REPORT ERROR
  }
  md -> is_allocated = 1;
  

  // coalescing free chunks

  // checking if prev chunk is free
  if (md->prev != 0) {
    metadata *prev_md = get_metadata(heap.bytes + md->prev*8);
    if (prev_md->is_allocated) {

      // case 1 as discribed in the README
      prev_md->length += md->length + sizeof(metadata); 
      prev_md->next = md->next; // linking current chunk to next chunk
      if (md->next != 0) {

        // making sure the current chunk is not the last chunk
        metadata *next_md = get_metadata(heap.bytes + md->next*8);
        next_md->prev = md->prev; // linking next chunk to previous chunk
        if (next_md->is_allocated) {

          // case 3 as discribed in the README
          prev_md->length += next_md->length + sizeof(metadata);
          prev_md->next = next_md->next; // linking previous chunk to next next chunk
          if (next_md->next != 0) {

            metadata *next_next_md = get_metadata(heap.bytes + next_md->next*8);
            next_next_md->prev = md->prev; // linking next next chunk to previous chunk
          }
        }
      }
    } else if (md->next != 0) { // checking if next chunk is free
      metadata *next_md = get_metadata(heap.bytes + md->next*8);
      if (next_md->is_allocated) {

        // case 2 as discribed in the README
        md->length += next_md->length + sizeof(metadata); // extending length of chunk
        md->next = next_md->next; // linking current chunk to next next chunk
        if (next_md->next != 0) {

          metadata *next_next_md = get_metadata(heap.bytes + next_md->next*8);
          next_next_md->prev = (md - heap.bytes)/8; // linking next next chunk to current chunk
        }
      }
    }
  }


}

/*int main(void) {
  initialize_heap();
}*/
