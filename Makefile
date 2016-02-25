
CC = gcc
CFLAGS = -Wall -Werror -g -O3

%.c: %.h

%_test: %.c %_test.c
	$(CC) $(CFLAGS) $^ -o $@

%_bench: %.c %_bench.c
	$(CC) $(CFLAGS) $^ -o $@
