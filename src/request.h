#ifndef TIBOR_REQUEST_H
#define TIBOR_REQUEST_H

struct tib_request
{
	long mtype;
	char key[512];
};

#define TIB_REQUEST_SIZE (sizeof(struct tib_request) - sizeof(long))

#endif

