#include <stddef.h>

typedef struct dtrie_node dtnode_t;

struct dtrie_node {
	dtnode_t   *child[2];
	uintptr_t       val;
};

typedef struct {
	dtnode_t *root;
} dtrie_t;

/*
 * dtrie_insert inserts a node into the trie
 * using node->val.
 */
int dtrie_insert(dtrie_t *trie, dtnode_t *node);

/* 
 * dtrie_remove_smallest removes the smallest
 * element of the trie.
 */
dtnode_t *dtrie_remove_smallest(dtrie_t *trie);

#define MAX_LEVELS (sizeof(uintptr_t)*8)

/*
 * iter_t is an iterator for a sorted trie.
 */
typedef struct {
	dtnode_t *nodes[MAX_LEVELS];
	dtnode_t *prev;
	int       level;
} dter_t;

/*
 * dtrie_iter() initializes the trie iterator.
 */
void dtrie_iter(dtrie_t *trie, dter_t *iter);

/*
 * dtrie_next() returns the next node in the
 * iterator, or NULL if the iterator is exhausted.
 * The nodes are returned in sorted order.
 *
 * NOTE: modifying the trie from which the iterator
 * was created will invalidate the iterator.
 */
dtnode_t *dtrie_next(dter_t *iter);
