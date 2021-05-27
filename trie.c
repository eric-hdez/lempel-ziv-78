#include "trie.h"

#include "code.h"

#include <stdio.h>
#include <stdlib.h>

// Constructor for a TrieNode ADT
//
TrieNode *trie_node_create(uint16_t code) {
    TrieNode *n = malloc(sizeof(TrieNode));
    if (!n) {
        return NULL;
    }

    n->code = code;

    for (int sym = 0; sym < ALPHABET; sym++) {
        n->children[sym] = NULL;
    }

    return n;
}

// Destructor for a TrieNode
//
void trie_node_delete(TrieNode *n) {
    if (n) {
        free(n);
    }
}

// Constructor for the root trie node of a trie
//
TrieNode *trie_create(void) {
    TrieNode *root = trie_node_create(EMPTY_CODE);
    if (!root) {
        return NULL;
    }

    return root;
}

// resets a trie structure starting at the root
// 
void trie_reset(TrieNode *root) {
    for (int sym = 0; sym < ALPHABET; sym++) {
        trie_delete(root->children[sym]);
        root->children[sym] = NULL;
    }
}

// Destructor for a sub-trie
//
void trie_delete(TrieNode *n) {
    if (n) {
        for (int sym = 0; sym < ALPHABET; sym++) {
            if (n->children[sym] != NULL) {
                trie_delete(n->children[sym]);
            }
        }

        trie_node_delete(n);
    }
}

// if sym is a valid ASCII character, returns a pointer to the
// Node that represents that sym
//
TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    return n->children[sym];
}
