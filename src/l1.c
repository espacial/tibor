#include "l1.h"

int32_t
l1_hash(char* key, int32_t length)
{
	uint32_t hash;
	int i;

	hash = 5381;

	for (i = 0; i < length; i++)
		hash = ((hash << 5) + hash) + key[i];

	return hash;
}

