#include <stddef.h>

typedef struct dtrie_node dtrie_node_t;

struct dtrie_node {
	dtrie_node_t   *child[2];
	uintptr_t       val;
};

typedef struct {
	dtrie_node_t *root;
} dtrie_t;

/*
 * dtrie_insert inserts a node into the trie
 * using node->val.
 */
void dtrie_insert(dtrie_t *trie, dtrie_node_t *node);

/* 
 * dtrie_remove_smallest removes the smallest
 * element of the trie.
 */
dtrie_node_t *dtrie_remove_smallest(dtrie_t *trie);

typedef void (*treefunc)(void *ctx, dtrie_node_t *node);

/*
 * dtrie_traverse calls treefunc(uctx, node) recursively
 * on all of the trie nodes in order.
 */
void dtrie_traverse(dtrie_t *trie, treefunc func, void *uctx);
