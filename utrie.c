#include <stddef.h>
#include <assert.h>
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
	assert(dir < SPLAY && dir >= 0);
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
		assert(dir < SPLAY && dir >= 0);
		b = b->child[dir];
		kmask<<=SPLAY_BITS;
	}
	return NULL;
}
