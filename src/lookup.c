#include <stdint.h>
#include <string.h>

#include "lookup.h"

int 
lookup(char* key, struct l1_entry* l1, struct trie* l2, struct tib_file* tf_out)
{
	unsigned int i;
	int64_t key_hash;
	int32_t key_length;
	struct l1_entry* entry;
	
	key_length = strlen(key);
	key_hash = l1_hash(key, key_length);

	printf("Looking into L1\n");
	/* L1 */
	for (i = 0; i < 10; i++)
	{
		entry = &l1[i];

		if ((entry->hash == key_hash)
		 && (entry->length == key_length)
		 && (strcmp(entry->key, key) == 0))
		{
			*tf_out = *entry->file;	
			return TIBOR_FOUND;
		}
	}

	printf("Looking into L2\n");
	/* L2 */
	if (trie_get(l2, key, &tf_out) == TRIE_FOUND)
		return TIBOR_FOUND;

	return TIBOR_NOT_FOUND;
}

