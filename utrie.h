#include <stddef.h>
#include <stdint.h>

#define NBITS (sizeof(uintptr_t)*8)
#define SPLAY_BITS 2
#define SPLAY (1<<SPLAY_BITS)

typedef struct unode unode;

struct unode {
	unode    *child[SPLAY];
	unode    *parent;
	uintptr_t key;
	int       pidx; /* index in parent */
};

typedef struct {
	unode *buckets[NBITS];
} utrie;

unode *utrie_insert(utrie *trie, unode *node);
unode *utrie_find(utrie *trie, uintptr_t key);
