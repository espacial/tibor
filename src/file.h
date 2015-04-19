#ifndef TIB_FILE_H
#define TIB_FILE_H

#include <stdint.h>

struct tib_file
{
	char path[1024];
	int fd;

	int shmid;
	uint64_t offset;

	uint8_t always_l1;

	uint64_t l1_hits;
	uint64_t l2_hits;
	uint64_t misses;
};

#endif

