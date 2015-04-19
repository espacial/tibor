#include <stdio.h>

#include "dump.h"
#include "file.h"

void
dump(char* path, struct list* files)
{
	FILE* f;
	struct list* runner;
	struct tib_file* tf;

	f = fopen(path, "w");
	if (f == NULL)
		return;

	fprintf(f, "File,L1,L2,Misses,Always\n");

	for (runner = files; runner != NULL; )
	{
		tf = list_data(runner);
		fprintf(f, "%s,%llu,%llu,%llu,%d\n", tf->path, tf->l1_hits, tf->l2_hits, 
		                                     tf->misses, tf->always_l1);
		runner = list_next(runner);
	}

	fclose(f);
}
