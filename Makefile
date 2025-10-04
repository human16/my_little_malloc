CC = gcc
CFLAGS = -g -Wall -Wvla -std=c99 -fsanitize=address,undefined

memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

debug: memtest.o debug_mymalloc.o
	$(CC) $(CFLAGS) $^ -o memtest-debug

debug_mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -DDEBUG=1 -c mymalloc.c -o debug_mymalloc.o

%.o: %.c
	$(CC) $(CFLAGS) -c $<

memtest.o: mymalloc.h
mymalloc.o: mymalloc.h

clean:
	rm -f *.o memtest memtest-debug debug_mymalloc.o

