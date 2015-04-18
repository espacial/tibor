#ifndef TIB_FILE_H
#define TIB_FILE_H

struct tib_file
{
	int fd;
	int shmid;
	uint64_t offset;
};

#endif

