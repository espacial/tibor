#ifndef TIBOR_RESPONSE_H
#define TIBOR_RESPONSE_H

struct tib_response
{
	long mtype;
	int shmid;
	uint64_t offset;
};

#define TIB_RESPONSE_SIZE (sizeof(struct tib_response) - sizeof(long))

#endif


