#ifndef TRIE_H
#define TRIE_H

#include <sys/types.h>
#include <stdint.h>

#include <stdlib.h>
#include <stdio.h>

#include "file.h"

#define TRIE_FOUND 0
#define TRIE_NOT_FOUND 1
#define TRIE_KEY_INVALID 2
#define TRIE_KEY_SET_SIZE 65

struct trie
{
	char key;
	struct tib_file* tf;
	struct trie* children[TRIE_KEY_SET_SIZE];
};

struct trie* trie_create();
int trie_set(struct trie* root, char* key, struct tib_file* tf);
int trie_remove(struct trie* root, char* key);
int trie_dump(struct trie* root);
int trie_get(struct trie* root, char* key, struct tib_file** tf_out);
int trie_contains(struct trie* root, char* key);

#endif

