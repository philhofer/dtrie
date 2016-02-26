#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "utrie.h"

#ifndef MAX
#define MAX (64*1024)
#endif

unode nodes[MAX];
utrie trie;

int main(void) {
	for (int i=0; i<MAX; i++)
		nodes[i].key = (uintptr_t)rand();

	clock_t start = clock();
	for (int i=0; i<MAX; i++) {
		unode *node = utrie_insert(&trie, &nodes[i]);
		if (node)
			assert(node->key == nodes[i].key);

	}
	clock_t elapsed = clock() - start;
	printf("%d insertions in %lu clocks\n", MAX, elapsed);

	/* int maxlvl = 0; */
	/* int total = 0; */
	/* for (int i=0; i<MAX; i++) { */
	/* 	int cur = 0; */
	/* 	unode *node = utrie_find(&trie, nodes[i].key); */
	/* 	while (node->parent) { */
	/* 		cur++; */
	/* 		node = node->parent; */
	/* 	} */
	/* 	total += cur; */
	/* 	if (cur > maxlvl) */
	/* 		maxlvl = cur; */

	/* } */
	/* printf("max depth is %d; avg is %d for log2(elems)=%lu\n", maxlvl, total/MAX, (sizeof(uintptr_t)*8)-__builtin_clzl(MAX)); */

	/* start = clock(); */
	/* for (int i=0; i<MAX; i++) { */
	/* 	utrie_remove(&trie, &nodes[i]); */
	/* } */
	/* elapsed = clock() - start; */
	/* printf("%d removals in %lu clocks\n", MAX, elapsed); */
}
