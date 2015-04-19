#include <stdint.h>
#include <string.h>

#include "lookup.h"

int 
lookup(char* key, struct l1_entry* l1, struct trie* l2, struct tib_file* tf_out, struct list** files, struct trie* miss)
{
	unsigned int i;
	int64_t key_hash;
	int32_t key_length;
	struct l1_entry* entry;
	struct tib_file* miss_file;
	
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
	{
		unsigned int idx = 0;
		unsigned int lowest_hits = l1[0].file->l1_hits + l1[0].file->l2_hits;
		unsigned int i;

		for (i = 0; i < 10; i++)
		{
			if (l1[i].file->l1_hits + l1[i].file->l2_hits < lowest_hits && l1[i].file->always_l1 != 1)
			{
				lowest_hits = l1[i].file->l1_hits + l1[i].file->l2_hits;
				idx = i;
			}
		}

		l1[idx].key = strdup(key);
		l1[idx].length = strlen(key);
		l1[idx].hash = l1_hash(l1[idx].key, l1[idx].length);
		l1[idx].file = tf_out;

		return TIBOR_FOUND;
	}

	printf("Logging a miss\n");
	/* MISS */
	if (trie_get(miss, key, &miss_file) == TRIE_FOUND)
		miss_file->misses++;
	else
	{
		struct tib_file* new_miss;
		new_miss = malloc(sizeof(struct tib_file));
		new_miss->misses = 1;
		new_miss->l1_hits = 0;
		new_miss->l2_hits = 0;
		memcpy(new_miss->path, key, strlen(key));
		
		trie_set(miss, key, new_miss);		

		struct list* new_miss2;
		new_miss2 = list_create(new_miss);
		*files = list_add(*files, new_miss2);
	}

	return TIBOR_NOT_FOUND;
}

