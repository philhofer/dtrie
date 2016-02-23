#include <stddef.h>
#include <stdint.h>
#include "dtrie.h"

/* left should be lesser *and* mask to zero (and vice-versa) */
static int breaks_invariant(uintptr_t val, uintptr_t mask, int dir) {
	return ((dir > 0 && (val&mask)==0) || (dir < 0 && (val&mask)));
}

static void swap_child(dtnode_t *parent, dtnode_t *oldchld, dtnode_t *newchld) {
	dtnode_t **backptr = (parent->child[0] == oldchld) ? &parent->child[0] : &parent->child[1];
	*backptr = newchld;
	newchld->child[0] = oldchld->child[0];
	newchld->child[1] = oldchld->child[1];
	oldchld->child[0] = NULL;
	oldchld->child[1] = NULL;
}

static void swap_root(dtrie_t *trie, dtnode_t *node) {
	dtnode_t *root = trie->root;
	trie->root = node;
	node->child[0] = root->child[0];
	node->child[1] = root->child[1];
	root->child[0] = NULL;
	root->child[1] = NULL;
}

int dtrie_insert(dtrie_t *trie, dtnode_t *node) {
	dtnode_t *cur = trie->root;
	if (!cur) {
		trie->root = node;
		return 0;
	}
	dtnode_t *parent = NULL;

	/*
	 * Walk down the trie and exchange the node to be inserted
	 * with one already in the trie where not doing so would break 
	 * the trie invariants (binary tree + bitwise trie).
	 * Start comparing bits at the msb of the largest int.
	 */
	uintptr_t mask = (uintptr_t)1<<(MAX_LEVELS-1);
	while (cur) {
		uintptr_t c = node->val - cur->val;
		if (!c)
			return -1;

		/* preserve bitwise level invariant; swap if necessary */
		if (breaks_invariant(node->val, mask, c)) {
			parent ? swap_child(parent, cur, node) : swap_root(trie, node);
			dtnode_t *tmp = cur;
			cur = node;
			node = tmp;
		}

		parent = cur;
		cur = cur->child[!!(node->val&mask)];
		mask >>= 1;
	}
	parent->child[!!(node->val&mask<<1)] = node;
	return 0;
}

#define SUCCESSOR 0
#define PREDECESSOR 1
/*
 * adjacent(node, SUCCESSOR) or adjacent(node, PREDECESSOR)
 * finds the successor/predecessor to a node, plus the pred/succ
 * parent.
 */
static inline dtnode_t *adjacent(dtnode_t *node, int dir, dtnode_t **pparent) {
	dtnode_t *parent = node;
	node = node->child[!dir];
	if (!node)
		return NULL;
	
	while (node->child[dir]) {
		parent = node;
		node = node->child[dir];
	}
	*pparent = parent;
	return node;
}

/* select the node to replace the one being deleted */
static inline dtnode_t *find_replacement(dtnode_t *node, dtnode_t **parent) {
	dtnode_t *rep = adjacent(node, SUCCESSOR, parent);
	return rep ? rep : adjacent(node, PREDECESSOR, parent);
}

static inline void delete_node(dtnode_t *node, dtnode_t *parent) {
	dtnode_t *rparent;
	dtnode_t *rep = find_replacement(node, &rparent);
	if (!rep)
		return;

	swap_child(parent, node, rep);
	delete_node(rep, rparent);
}

dtnode_t *dtrie_remove_smallest(dtrie_t *trie) {
	dtnode_t *cur = trie->root;
	if (!cur)
		return NULL;

	dtnode_t *parent = NULL;
	while (cur->child[0]) {
		parent = cur;
		cur = cur->child[0];
	}
	if (!parent) {
		trie->root = NULL;
	} else {
		delete_node(cur, parent);
	}
	return cur;
}

void dtrie_iter(dtrie_t *trie, dter_t *iter) {
	iter->level = 0;
	iter->prev = trie->root;
}

dtnode_t *dtrie_next(dter_t *iter) {
	if (iter->prev || iter->level) {
		dtnode_t *cur = iter->prev;
		while (cur) {
			iter->nodes[iter->level++] = cur;
			cur = cur->child[0];
		}
		cur = iter->nodes[--iter->level];
		iter->prev = cur->child[1];
		return cur;
	}
	return NULL;
}
