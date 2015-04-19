#ifndef TIBOR_LOOKUP_H
#define TIBOR_LOOKUP_H

#include "file.h"
#include "l1.h"
#include "trie.h"
#include "list.h"

#define TIBOR_FOUND 1
#define TIBOR_NOT_FOUND 2

int 
lookup(char* key, struct l1_entry* l1, struct trie* l2, struct tib_file* tf_out, struct list** files, struct trie* miss);

#endif

