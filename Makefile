
CC = gcc
CFLAGS = -Wall -Werror -g -O3

dtrie.c: dtrie.h

test: dtrie.c dst_test.c
	$(CC) $(CFLAGS) dtrie.c dst_test.c -o test

bench: dtrie.c dst_bench.c
	$(CC) $(CFLAGS) dtrie.c dst_bench.c -o bench