#include <stddef.h>
#include <stdint.h>
#include "dtrie.h"

/* left should be lesser *and* mask to zero (and vice-versa) */
static int breaks_invariant(uintptr_t val, uintptr_t mask, int dir) {
	return ((dir > 0 && (val&mask)==0) || (dir < 0 && (val&mask)));
}

static void swap_child(dtrie_node_t *parent, dtrie_node_t *oldchld, dtrie_node_t *newchld) {
	dtrie_node_t **backptr = (parent->child[0] == oldchld) ? &parent->child[0] : &parent->child[1];
	*backptr = newchld;
	newchld->child[0] = oldchld->child[0];
	newchld->child[1] = oldchld->child[1];
	oldchld->child[0] = NULL;
	oldchld->child[1] = NULL;
}

static void swap_root(dtrie_t *trie, dtrie_node_t *node) {
	dtrie_node_t *root = trie->root;
	trie->root = node;
	node->child[0] = root->child[0];
	node->child[1] = root->child[1];
	root->child[0] = NULL;
	root->child[1] = NULL;
}

void dtrie_insert(dtrie_t *trie, dtrie_node_t *node) {
	dtrie_node_t *cur = trie->root;
	if (!cur) {
		trie->root = node;
		return;
	}
	dtrie_node_t *parent = NULL;

	/*
	 * Walk down the trie and exchange the node to be inserted
	 * with one already in the trie where not doing so would break 
	 * the trie invariants (binary tree + bitwise trie).
	 * Start comparing bits at the msb of the largest int.
	 */
	uintptr_t mask = (uintptr_t)1<<((sizeof(uintptr_t)*8)-1);
	while (cur) {
		uintptr_t c = node->val - cur->val;
		if (!c) { return; } /* TODO: handle duplicate */

		/* preserve bitwise level invariant; swap if necessary */
		if (breaks_invariant(node->val, mask, c)) {
			parent ? swap_child(parent, cur, node) : swap_root(trie, node);
			dtrie_node_t *tmp = cur;
			cur = node;
			node = tmp;
		}

		parent = cur;
		cur = cur->child[!!(node->val&mask)];
		mask >>= 1;
	}
	parent->child[!!(node->val&mask<<1)] = node;
}

#define SUCCESSOR 1
#define PREDECESSOR 0
/*
 * adjacent(node, SUCCESSOR) or adjacent(node, PREDECESSOR)
 * finds the successor/predecessor to a node, plus the pred/succ
 * parent.
 */
static inline dtrie_node_t *adjacent(dtrie_node_t *node, int dir, dtrie_node_t **pparent) {
	dtrie_node_t *parent = node;
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
static inline dtrie_node_t *find_replacement(dtrie_node_t *node, dtrie_node_t **parent) {
	dtrie_node_t *rep = adjacent(node, SUCCESSOR, parent);
	return rep ? rep : adjacent(node, PREDECESSOR, parent);
}

static inline void delete_node(dtrie_node_t *node, dtrie_node_t *parent) {
	dtrie_node_t *rparent;
	dtrie_node_t *rep = find_replacement(node, &rparent);
	if (!rep)
		return;

	swap_child(parent, node, rep);
	delete_node(rep, rparent);
}

dtrie_node_t *dtrie_remove_smallest(dtrie_t *trie) {
	dtrie_node_t *cur = trie->root;
	if (!cur)
		return NULL;

	dtrie_node_t *parent = NULL;
	while (cur->child[0]) {
		parent = cur;
		cur = cur->child[0];
	}
	if (!parent) {
		trie->root = NULL;
	} else {
		delete_node(parent, cur);
	}
	return cur;
}

static void ordered_apply(dtrie_node_t *node, treefunc func, void *uctx) {
	if (node->child[0])
		ordered_apply(node->child[0], func, uctx);

	func(uctx, node);
	if (node->child[1])
		ordered_apply(node->child[1], func, uctx);

}

void dtrie_traverse(dtrie_t *trie, treefunc func, void *uctx) {
	if (trie->root)
		ordered_apply(trie->root, func, uctx);
	
}
