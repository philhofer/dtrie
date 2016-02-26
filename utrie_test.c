#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "utrie.h"

utrie trie;

static void dirprint(unode *node) {
	for (int i=0; i<SPLAY; ++i) {
		unode *s = node->child[i];
		if (s) {
			printf("\t\"%#lx\" ->\"%#lx\"\n", node->key, s->key);
			dirprint(s);
		}
	}
}

static void digraph(utrie *trie, const char *name) {
	printf("digraph %s {\n", name);
	for (int i=0; i<(NBITS); ++i) {
		unode *r = trie->buckets[i];
		if (r) {
			printf("\troot -> bucket_%d;\n", i);
			printf("\tbucket_%d -> \"%#lx\";\n", i, r->key);
			dirprint(r);
		}
	}
	puts("}");
}

#define COUNT 32

int main(void) {
	unode nodes[COUNT];
	for (int i=0; i<COUNT; i++) {
		nodes[i] = (unode){{0}};
		nodes[i].key = (uintptr_t)rand();
		assert(utrie_insert(&trie, &nodes[i]) == &nodes[i]);
		assert(utrie_find(&trie, nodes[i].key) == &nodes[i]);
		validate(&trie);
	}
	digraph(&trie, "before");

	for (int i=0; i<COUNT; i++) {
		unode *node = NULL;
		node = utrie_find(&trie, nodes[i].key);
		if (!node) {
			printf("find(%#lx) = NULL!\n", nodes[i].key);
			digraph(&trie, "after");
			_exit(1);
		}
		printf("removing %#lx...\n", node->key);
		utrie_remove(&trie, node);
		if (!validate(&trie)) {
			digraph(&trie, "broken");
			_exit(1);
		}
	}
}
