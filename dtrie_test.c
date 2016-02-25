#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "dtrie.h"

#define INTS 1024

dtrie_t trie;

static void dump(void) {
	dter_t iter;
	dtrie_iter(&trie, &iter);
	dtnode_t *node = dtrie_next(&iter);
	while (node) {
		printf("%lu\n", node->val);
		node = dtrie_next(&iter);
	}
}

static void orderchk(void) {
	dter_t iter;
	dtrie_iter(&trie, &iter);
	dtnode_t *node = dtrie_next(&iter);
	uintptr_t prev = 0;
	while (node) {
		if (node->val < prev) {
			printf("%lu after %lu!\n", node->val, prev);
			dump();
			_exit(1);
		}
		node = dtrie_next(&iter);
	}
}

int main(void) {
	dtnode_t nodes[INTS];
	for (int i=0; i<INTS; ++i) {
		nodes[i].child[0] = NULL;
		nodes[i].child[1] = NULL;
		nodes[i].val = (uintptr_t)rand();
	}
	
	for (int i=0; i<INTS; ++i) {
		assert(dtrie_insert(&trie, &nodes[i]) == 0);
		orderchk();
	}

	uintptr_t prevdel = 0;
	for (int i=0; i<INTS; ++i) {
		dtnode_t *v = dtrie_remove_smallest(&trie);
		if (!v) {
			printf("remove(%d): no value returned", i);
			_exit(1);
		}
		if (v->val < prevdel) {
			printf("remove_smallest returned %lu followed by %lu\n", prevdel, v->val);
			_exit(1);
		}
		prevdel = v->val;
		orderchk();
	}
	return 0;
}
