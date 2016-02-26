#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include "utrie.h"

/* capture upper n bits */
#define UPPER_MASK ((uintptr_t)(SPLAY-1)<<(NBITS-SPLAY_BITS))

static inline uintptr_t mask_dir(uintptr_t mask) {
	return (mask&UPPER_MASK)>>(NBITS-SPLAY_BITS);
}

/* tail-recursive; trivially transformed by compiler */
static inline unode *push(unode *parent, unode *child, uintptr_t kmask) {
	/* right children are adjacent; left children are down */
	if (parent->key == child->key)
		return parent;
	
	/* direction is the upper n bits (00, 01, 10, 11 for 2 bits) */
	uintptr_t dir = mask_dir(kmask);
	unode *sub = parent->child[dir];
	if (!sub) {
		parent->child[dir] = child;
		child->parent = parent;
		child->pidx = (int)dir;
		return child;
	}
	return push(sub, child, kmask<<SPLAY_BITS);
}

unode *utrie_insert(utrie *trie, unode *node) {
	assert(node->parent == NULL);
	int zeros = __builtin_clzl(node->key);
	int first = NBITS - zeros;
	unode *b = trie->buckets[first];
	if (!b) {
		trie->buckets[first] = node;
		return node;
	}
	/* shift away leading zeros and first 1 */
	uintptr_t kmask = node->key<<(zeros + 1);

	return push(b, node, kmask);
}

unode *utrie_find(utrie *trie, uintptr_t key) {
	int zeros = __builtin_clzl(key);
	int first = NBITS - zeros;
	unode *b = trie->buckets[first];
	uintptr_t kmask = key<<(zeros+1);
	while (b) {
		if (b->key == key)
			return b;
		
		uintptr_t dir = mask_dir(kmask);
		b = b->child[dir];
		kmask<<=SPLAY_BITS;
	}
	return NULL;
}

static unode *find_leaf(unode *node) {
	unode *leaf = node;

again:
	for (int i=0; i<SPLAY; i++) {
		if (leaf->child[i]) {
			leaf = leaf->child[i];
			goto again;
		}
	}
	
	if (leaf == node)
		return NULL;


	/* unlink from parent */
	assert(leaf->parent);
	leaf->parent->child[leaf->pidx] = NULL;
	return leaf;
}

static void copy_node(unode *dst, unode *src) {
		dst->pidx = src->pidx;
		dst->parent = src->parent;
		for (int i=0; i<SPLAY; i++) {
			dst->child[i] = src->child[i];
			if (dst->child[i]) {
				assert(dst->child[i]->parent == src);
				dst->child[i]->parent = dst;
			}
		}
		if (dst->parent) {
			assert(dst->parent->child[dst->pidx] == src);
			dst->parent->child[dst->pidx] = dst;
		}
}

static void clr(unode *node) {
	for (int i=0; i<SPLAY; i++)
		node->child[i] = NULL;

	node->parent = NULL;
}

static int check_children(unode *node) {
	for (int i=0; i<SPLAY; i++) {
		if (node->child[i]) {
			if (node->child[i]->parent != node) {
				printf("node %#lx child %#lx has parent %p\n", node->key, node->child[i]->key, (void *)node->child[i]->parent);
				return 0;
			}
			if (node->child[i]->pidx != i) {
				printf("node %#lx child %d has index %d\n", node->key, i, node->child[i]->pidx);
				return 0;
			}
			if (!check_children(node->child[i]))
				return 0;

		}
	}
	return 1;
}

int validate(utrie *trie) {
	for (int i=0; i<NBITS; i++) {
		if (trie->buckets[i]) {
			assert(NBITS-__builtin_clzl(trie->buckets[i]->key) == i);
			if (!check_children(trie->buckets[i]))
				return 0;

		}
	}
	return 1;
}

void utrie_remove(utrie *trie, unode *node) {
	unode *leaf = find_leaf(node);
	if (leaf)
		copy_node(leaf, node);
	else if (node->parent)
		node->parent->child[node->pidx] = NULL;
		
	if (!node->parent) {
		int bucket = NBITS-__builtin_clzl(node->key);
		assert(trie->buckets[bucket] == node);
		trie->buckets[bucket] = leaf;
	}
	clr(node);
}
