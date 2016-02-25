#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "dtrie.h"

#define MANY (1024*256)

dtnode_t nodes[MANY];
dtrie_t trie;

#define RUNS 8
int runs[RUNS] = {16, 64, 256, 1024, MANY/8, MANY/4, MANY/2, MANY};

int main(void) {
	for (int i=0; i<MANY; ++i) {
		nodes[i].val = (uintptr_t)rand();
	}

	for (int r=0; r<RUNS; ++r) {
		clock_t start = clock();
		for (int i=0; i<runs[r]; ++i) {
			dtrie_insert(&trie, &nodes[i]);
		}
		clock_t elapsed = clock() - start;
		printf("%d elements in %lu clocks\n", runs[r], elapsed);
		memset(&nodes, 0, sizeof(nodes));
	}
}
