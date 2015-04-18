#ifndef TIBOR_L1_H
#define TIBOR_L1_H

#include <stdint.h>

struct l1_entry
{
	int32_t hash;
	char* key;
	struct tib_file* file;
	int32_t length;
};

int32_t l1_hash(char* key, int32_t length);

#endif

