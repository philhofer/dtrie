#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "utrie.h"

utrie trie;

static void print_node(unode *node, int depth) {
	int dots = depth;
	while (dots--)
		printf("-");
	
	if (node) {
		printf("[%lu]\n", node->key);
		for (int i=0; i<SPLAY; ++i) {
			print_node(node->child[i], depth+1);
		}
	} else {
		puts("[]");
	}
}

static void show(utrie *trie) {
	for (int i=0; i<(NBITS); ++i) {
		print_node(trie->buckets[i], 0);
	}
}

int main(void) {
	unode nodes[16];
	for (int i=0; i<16; i++) {
		nodes[i] = (unode){{0}};
		nodes[i].key = (uintptr_t)i+1;
		assert(utrie_insert(&trie, &nodes[i]) == &nodes[i]);
		assert(utrie_find(&trie, (uintptr_t)i+1));
	}
	show(&trie);
}
