CC = gcc
CFLAGS = -g -Wall -Wvla -std=c99 -fsanitize=address,undefined

memtest: memtest.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $<

memtest.o: mymalloc.h
mymalloc.o: mymalloc.h