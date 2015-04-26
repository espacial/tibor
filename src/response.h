#ifndef TIBOR_RESPONSE_H
#define TIBOR_RESPONSE_H

#include <sys/ipc.h>

struct tib_response
{
	long mtype;
	key_t key;
	uint64_t length;
	int success;
};

#define TIB_RESPONSE_SIZE (sizeof(struct tib_response) - sizeof(long))

#endif


