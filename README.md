netid: by240, elp95

# How to run everything

## Memtest

To run memtest regularly, use `make memtest`, which will generate a file which will run with `./memtest`

To run it with debugging, use  `make debug`, which will generate a file which will run with `./debug`

## tests

To run tests regularly, use `make test`, which will generate a file which will run with `./test`

To run it with debugging, use `make debug-test`, which will generate a file which will run with `./debug-test`

## memgrind

To run memgrind regularly, use `make memgrind`, which will generate a file whihc will run with `./memgrind`

To run it with debugging, use `make debug-memgrind`, which will generate a file which will run with `./debug-memgrind`

# Plan:


# Testing:

Testing will be conducted through the file tests.c
Each test will be a different function and when running "make test" the resulting binary will run all tests.


## test 1:
    Create two objects and then free them. Easy

## test 2:
    Try to allocate more space than is available

## test 3:
    Try to double free

## test4:
    Pass an invalid pointer to free

## test5:
    Try to overfill the heap and make sure when allocating 1 byte, 8 will be reserved in the heap.

## test6:
    Check coalescing case 3

## test7:
    Check coalescing case 2

## test8:
    Check coalescing case 1




## DEBUGING:

Included in the make files are 3 important options:

make memtest:   The regular option that will compile everything without any debugging flag

make clean:     Will clean the files created by make. Gotta love organization 😍

make debug:     Will run the program with `DEBUG=1`, this allows us to shove extra monitoring into mymalloc.c and monitor more components of the program while it runs.
                In mymalloc.c, we can add if statements that will only run `if (DEBUG)`, that's how we insert tests.
                Test comments should follow the format `"| *Function*: ________"`

make debug-test: Will run `tests.c` with the debug-enabled `mymalloc.c`

make debug-memgrind: Will run `memgrind.c` with the debug-enabled `mymalloc.c`

## METADATA:

According to section 1.2, alignment will be done on an 8-byte basis

Let's define a chunk to be an 8-byte long piece of the heap, so that our 4096 bytes of heap can be divided into 512 chunks.
Now we'll define a block to be the metadata with the data it carries. Each block will be made of n # of chunks.

Since we know every block will be placed in a multiple of 8, we can access each of the 512 possible chunk positions with a 2-byte "pointer"
This "pointer" can point to any block in the different chunk positions, so we can take one pointer for the next block and one for the previous.
that will occupy 4 of our 8 available metadata bytes

We can also use one bit to decide if the chunk is being used or not. But let's use 1 byte, because why not
so far, we totalled at 5 of our 8 available metadata bytes.

Well, we have so much space left, I guess we can use another 2 bytes (even though 1.5 is needed) to get the length of the chunk as well.
that leaves us with 7 of our available 8 metadata bytes.

We'll leave the last byte to be whatever, it will not be used.

### METADATA STRUCTURE:
metadata = 8 chars (bytes)

`metadata[0,1]`   := prev pointer: a number between 0-511 (can go higher, but will not) that represents the chunk location of the previous chunk
                    ----first block's previous pointer will be 1.
                    ----to access the chunk in the heap, we will have to multiply the pointer's value by 8.

`metadata[2,3]`   := next pointer: a number between 0-511 (can go higher, but will not) that represents the chunk location of the next chunk
                    ----last block's next pointer will be 0.
                    ----to access the chunk in the heap, we will have to multiply the pointer's value by 8.

`metadata[4]`     := free or not: 0 will represent free, 1 is allocated.

`metadata[5,6]`   := the length/size of the chunk

`metadata[7]`     := undefined, do not use


## MY_MALLOC:

Malloc will allocate a number of bytes divisible by 8 that is greater than or equal to the number of bytes passed in by the client.

First we'll check whether our heap has been initialized and if the heap was not initialized we'll initialize it using INITIALIZE_HEAP

If we cannot find a chunk that can fulfil the client's request then return NULL and print a message to standard error in the format below

`malloc: Unable to allocate 1234 bytes (source.c:1000)`

If mymalloc is able to reverse unallocated memory we need to return a pointer to an object that does not overlap with another allocated object


## INITIALIZE_HEAP:


Here is need to write the first header using out 8 byte metadata structure:

`metadata[0,1]`: prev pointer should be set to 1 

`metadata[2,3]`: next pointer should be set to 0 since there are no next blocks

`metadata[4]`: set this to 0 to indicate chunk is free

`metadata[5,6]`: length/size of our chunk should be set to 4088 since we allocate 8 bytes for the header

Register a leak detection function that runs when the program terminates by calling atexit in the `initialize heap` function 



## FREE:


Free is a relatively easy function. All we have to do is go to the correct place in the heap, and change the metadata to denote the block is free.

However, We should deal with the possibility that adjacent blocks are free. there's 3 possibilities for adjacent blocks to be free:
(For these cases, we will refer to "this block" as the block that is currently free)

    1:
    this block comes after a block that is already free.
    ----To deal with this case, we can check the previous block, if it's free, change metadatas as follows:
    --------change the previous block's length to it's length + this block's length + 8 (this block's metadata).
    --------change the previous block's next pointer to the current block's next pointer.
    --------change the next block's previous pointer to the current block's previous pointer.
    ----This will leave the current block in memory, but it will be completely ignored and used as free space.

    2:
    this block comes before a block that is already free.
    ----To deal with this case, we can check the next block, if it's free, change metadatas as follows:
    --------change this block's length to it's length + the next block's length + 8 (the next block's metadata)
    --------change this block's next pointer to the next block's next pointer.
    --------change the block after the next block's previous pointer to the next block's previous pointer.
    ----This will do the same thing as the previous case just for the next block.

    3:
    this block is between free blocks.
    ----To deal with this case, we'll basically connect the previous and next blocks to one big block with the following:
    --------change the previous block's length to it's length + this block's length + the next block's length + 16 (the current and next block's metadata)
    --------change the previous block's next pointer to the next block's next pointer
    --------change the next block's previous pointer to the previous block's previous pointer.
    ----This basically combines the last two cases.

if an object that is already free is freed again or out of bounds free, crash out. 

more details will be added as time goes on.
