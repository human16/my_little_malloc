#include <stdio.h>
#include <stdlib.h>
#include "mymalloc.h"
#define MEMLENGTH 4096

#ifndef DEBUG
#define DEBUG 0
#endif

static union {
char bytes[MEMLENGTH];
double not_used;
} heap;

static int heap_initialized = 0;

typedef struct {
  unsigned short prev;
  unsigned short next;
  unsigned short length;
  unsigned char is_allocated;
  unsigned char padding; // unused
} metadata;

static void create_metadata(metadata *md, unsigned short prev, unsigned short next, unsigned short length, unsigned char is_allocated) {
    md->prev = prev;
    md->next = next;
    md->length = length;
    md->is_allocated = is_allocated;
    md->padding = 0;
}

static metadata *get_metadata(char *pointer) {
    return (metadata *)pointer;
}

static void visualize_heap() {
  // This function creates a visual representation of the heap with 0s representing allocated chunks and -s representing free chunks
  // Each character represents 8 bytes of memory
  metadata *curr = get_metadata(heap.bytes);
  int block_num = 0;
  while (1) {
    printf("|");
    if (curr->is_allocated) {
      for (int i = 0; i < curr->length/8; i++) {
        printf("0"); 
      } 
    } else {
      for (int i = 0; i < curr->length/8; i++) {
        printf("-");
      }
    }
    block_num++;
    if (curr->next == 0) {
      break;
    }
    curr = get_metadata(heap.bytes + curr->next);
  }
  printf("\n");
}

static void check_for_leaks() {
  // traverse heap and report a leak
  // a leak occurs when there is a chunk marked "allocated" on the heap
  metadata *curr_metadata = get_metadata(heap.bytes);
  while (1) {
    if (curr_metadata->is_allocated) {
      fprintf(stderr, "Memory leak detected: chunk at %p of size %u bytes not freed\n", (void *)(curr_metadata + 1), curr_metadata->length);
    }
    if (curr_metadata->next == 0) {
      return ;
    }
    curr_metadata = get_metadata(heap.bytes + curr_metadata->next);
  }
}

static void initialize_heap() {
  atexit(check_for_leaks);
  if (DEBUG) {
    printf("| Initialize_heap: Initializing heap\n");
  }
  // toggle heap initialization flag 
  heap_initialized = 1;

  // register leak detector
  // atexit(check_for_leaks);

  // get pointer to start of the heap and write initial metadata for the single free chunk
  metadata *head = get_metadata(heap.bytes);
  create_metadata(head, 1, 0, MEMLENGTH - sizeof(metadata), 0);

  if (DEBUG) {
    printf("| Initialize_heap: Heap initialized...\n");
    printf("| Initialize_heap: Metadata size: %zu bytes\n", sizeof(metadata));
    printf("| Initialize_heap: Initial free chunk size: %u bytes\n", head->length);
    printf("| Initialize_heap: Heap starts at: %p\n", (void*)head);
    printf("| Initialize_heap: Payload starts at: %p\n", (void*)head+8);
  } 
}

void * mymalloc(size_t size, char *file, int line) {
  if (!heap_initialized) {
    initialize_heap();
  }

  if (size == 0) {
    return NULL;
  }

  size_t size_rounded = size;
  if (size % 8 != 0) {
    size_rounded = size + (8 - (size % 8));
  }

  metadata *curr = (metadata *)heap.bytes;

  while (1) {

    if (!curr->is_allocated && curr->length >= size_rounded) {
      // check if we should split the block
      size_t remaining = curr->length - size_rounded;
      if (remaining >= sizeof(metadata) + 8) {
        metadata *new_block = (metadata *)((char *)(curr + 1) + size_rounded);
        new_block->prev = (char *)curr - heap.bytes;
        new_block->next = curr->next;
        new_block->length = remaining - sizeof(metadata);
        new_block->is_allocated = 0;

        // update curr
        curr->length = size_rounded;
        curr->next = (char *)new_block - heap.bytes;

        // update next block prev pointer if it exists
        if (new_block->next != 0) {
          metadata *next = (metadata *)&heap.bytes[new_block->next];
          next->prev = (char *)new_block - heap.bytes;
        }
      }

      curr->is_allocated = 1;

      if (DEBUG) {
        printf("| Malloc: Allocated %zu bytes at %p\n", size_rounded, (void *)(curr + 1));
        visualize_heap();
        printf("\n");
      }
      return (void *)(curr + 1);
    }

    // next block
    if (curr->next == 0) {
      fprintf(stderr, "Error in %s line %d: malloc of %zu bytes failed\n", file, line, size);
      if (DEBUG) {
        printf("| Malloc: Last block reached at: %p\n", (void*)curr + 8);
      }
      return NULL;
    }
    
    
    curr = (metadata *)&heap.bytes[curr->next];
  }
  if (DEBUG) {
    printf("| Malloc: failed\n");
  }
  return NULL;
}

static char pointer_validity(void *ptr) {
  metadata *curr_metadata = get_metadata(heap.bytes);
  while (1) {
    if ((void *)curr_metadata > ptr) {
      return 0;
    }
    if ((void *)(curr_metadata + 1) == ptr) {
      return 1;
    }
    if (curr_metadata->next == 0) {
      return 0;
    }
    curr_metadata = get_metadata(heap.bytes + curr_metadata->next);
  }
}

void myfree(void *ptr, char *file, int line) {

  if (!ptr) return;
  if (!heap_initialized) {
    initialize_heap();
  }

  // Making sure the pointer is within the range of the heap
  if (ptr < (void *)heap.bytes || ptr >= (void *)(heap.bytes + MEMLENGTH)) {
    fprintf(stderr, "free: Inappropriate pointer (%s:%d)\n", file, line);
    exit(2);
  }

  // Making sure the pointer points to the start of a payload
  if (!pointer_validity(ptr)) {
    fprintf(stderr, "free: Invalid pointer (%s:%d)\n", file, line);
    exit(2);
  }

  metadata *md = get_metadata(ptr-8);

  // Checking for double free
  if (!md->is_allocated) {
    fprintf(stderr, "free: Double free (%s:%d)\n", file, line);
    exit(2);
  }
  md->is_allocated = 0;
  

  // coalescing free chunks

  // checking if prev chunk is free
  if (md->prev != 1) {
    metadata *prev_md = get_metadata(heap.bytes + md->prev);
    if (!prev_md->is_allocated) {

      // case 1 as discribed in the README
        if (DEBUG) {
          printf("| Free: Coalescing with prev chunk\n");
        }

      prev_md->length += md->length + sizeof(metadata); 
      prev_md->next = md->next; // linking current chunk to next chunk
      if (md->next != 0) {

        // making sure the current chunk is not the last chunk
        metadata *next_md = get_metadata(heap.bytes + md->next);
        next_md->prev = md->prev; // linking next chunk to previous chunk
        if (!next_md->is_allocated) {

          // case 3 as discribed in the README
          if (DEBUG) {
            printf("| Free: Coalescing with both chunk\n");
          }
          prev_md->length += next_md->length + sizeof(metadata);
          prev_md->next = next_md->next; // linking previous chunk to next next chunk

          if (next_md->next != 0) {
            metadata *next_next_md = get_metadata(heap.bytes + next_md->next);
            next_next_md->prev = md->prev; // linking next next chunk to previous chunk
          }
        }
      }
      if (DEBUG) {
        //helps visualize the heap after running free, but extremely obnoxious
        printf("| Free: Pointer %p freed\n", ptr);
        printf("| Free: Metadata: prev: %u, next: %u, length: %u, is_allocated: %u\n", md->prev, md->next, md->length, md->is_allocated);
        visualize_heap();
        printf("\n");
      }
      return ;
    }
   }
   if (md->next != 0) { // checking if next chunk is free
    metadata *next_md = get_metadata(heap.bytes + md->next);
    if (!next_md->is_allocated) {
      // case 2 as discribed in the README
      if (DEBUG) {
        printf("| Free: Coalescing with next chunk\n");
      }
      md->length += next_md->length + sizeof(metadata); // extending length of chunk
      md->next = next_md->next; // linking current chunk to next next chunk
      if (DEBUG) {
        printf("| Free: Current metadata: prev: %u, next: %u, length: %u, is_allocated: %u\n", md->prev, md->next, md->length, md->is_allocated);
      }
      if (next_md->next != 0) {
        metadata *next_next_md = get_metadata(heap.bytes + next_md->next);
        next_next_md->prev = md->prev; // linking next next chunk to current chunk
      }
    }
  } 
  if (DEBUG) {
    //helps visualize the heap after running free, but extremely obnoxious
    printf("| Free: Pointer %p freed\n", ptr);
    printf("| Free: Metadata: prev: %u, next: %u, length: %u, is_allocated: %u\n", md->prev, md->next, md->length, md->is_allocated);
    visualize_heap();
    printf("\n");
  }
}
