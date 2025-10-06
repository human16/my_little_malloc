CC = gcc
CFLAGS = -g -Wall -Wvla -std=c99 -fsanitize=address,undefined

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o memgrind

memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

test: tests.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o test

debug-memgrind: memgrind.o debug_mymalloc.o
	$(CC) $(CFLAGS) $^ -o debug-memgrind

debug-test: tests.o debug_mymalloc.o
	$(CC) $(CFLAGS) $^ -o debug-test

debug: memtest.o debug_mymalloc.o
	$(CC) $(CFLAGS) $^ -o memtest-debug

debug_mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -DDEBUG=1 -c mymalloc.c -o debug_mymalloc.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

memtest.o: mymalloc.h
mymalloc.o: mymalloc.h
tests.o: mymalloc.h
memgrind.o: mymalloc.h

clean:
	rm -f *.o test memtest memtest-debug debug-test memgrind debug-memgrind

