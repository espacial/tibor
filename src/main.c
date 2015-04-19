#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "conf.h"
#include "file.h"
#include "l1.h"
#include "lookup.h"
#include "request.h"
#include "response.h"
#include "trie.h"
#include "list.h"
#include "dump.h"

key_t key_req;
int mq_req;

key_t key_res;
int mq_res;

int key_fd;
struct l1_entry l1[10];
struct trie* l2;
struct trie* miss;
struct list* files;
pthread_t dump_thread;

static void
write_pid_file(void)
{
	FILE* pid_file;

	pid_file = fopen("/var/run/tibor.pid", "w");
	fprintf(pid_file, "%d\n", getpid());
	fflush(pid_file);
	fclose(pid_file);

	return;
}

static int
instance_exists(void)
{
	FILE* pid_file;

	pid_file = fopen("/var/run/tibor.pid", "r");
	if (pid_file != NULL)
	{
		fclose(pid_file);
		return 1;
	}
	else
		return 0;
}

static void
create_message_queues(void)
{
	/* if ((key_fd = open("/var/run/tibor.mq", O_CREAT | O_RDWR)) == -1) */
	/* { */
	/* 	perror("open"); */	
	/* 	exit(1); */
	/* } */

	/* if (fchmod(key_fd, S_IRWXU | S_IRWXG | S_IRWXO) == -1) */
	/* { */
	/* 	perror("fchmod"); */	
	/* 	exit(1); */
	/* } */

	/* if ((key = ftok("/var/run/tibor.mq", 'a')) == -1) */
	/* { */
	/* 	perror("ftok"); */	
	/* 	exit(1); */
	/* } */

	key_req = 111;
	if ((mq_req = msgget(key_req, IPC_CREAT | 0777)) == -1)
	{
		perror("msgget");	
		exit(1);
	}

	key_res = 222;
	if ((mq_res = msgget(key_res, IPC_CREAT | 0777)) == -1)
	{
		perror("msgget");	
		exit(1);
	}
}

static void*
dump_thread_fn(void* arg)
{
	(void)arg;

	printf("A\n");

	for (;;)
	{
		printf("B\n");
		dump("/var/log/tibor.dump", files);
		sleep(10);
	}

	printf("C\n");
}

int
main(int argc, char* argv[])
{
	struct tib_request req;
	struct tib_response res;
	struct tib_file tf;
	struct conf *config;
	struct mushroom_conf* mc;
	pthread_t dump_thread;

	daemon(1, 1);

	files = NULL;
	l2 = trie_create();
	miss = trie_create();

	if (instance_exists())
	{
		fprintf(stderr, "Another instance of tibor is running.\n");
		return EXIT_FAILURE;
	}
	
	if (argc > 1)
	{
		printf("Going to load conf.\n");
		if ((config = load_conf(argv[1])) != NULL)
		{
			printf("After load.\n");
			struct list* runner;
			unsigned int l1_idx = 0;
			key_t key = 400;
			for (runner = config->mushrooms; runner != NULL; )
			{
				mc = list_data(runner);

				struct tib_file* tf;
				tf = malloc(sizeof(struct tib_file));
				tf->l1_hits = 0;
				tf->l2_hits = 0;
				tf->misses = 0;
				tf->always_l1 = mc->on_l1;
				snprintf(tf->path, 1024, "%s/%s", config->root, mc->file_path);

				int shmid;
				char* shmptr;
				struct stat sb;

				stat(tf->path, &sb);

				/* printf("%s\n", tf->path); */

				if ((shmid = shmget(key++, sb.st_size, IPC_CREAT | 0777)) == -1)
				{
					perror("shmget");
					exit(1);
				}

				if ((shmptr = shmat(shmid, NULL, 0)) == (char*)-1)
				{
					perror("shmat");	
					exit(1);
				}

				tf->fd = open(tf->path, O_RDWR);
				unsigned int k;
				for (k = 0; k < sb.st_size; k++)
				{
					char buffer[1];
					read(tf->fd, buffer, 1);
					shmptr[k] = buffer[0];
				}


				if (tf->always_l1 == 1 && l1_idx < 10)
				{
					l1[l1_idx].file = tf;
					l1[l1_idx].key = strdup(tf->path);
					l1[l1_idx].length = strlen(tf->path);
					l1[l1_idx].hash = l1_hash(l1[l1_idx].key, l1[l1_idx].length);

					l1_idx++;
				}

				struct list* to_add;
				to_add = list_create(tf);
				files = list_add(files, to_add);

				trie_set(l2, tf->path, tf);
				/* trie_dump(l2); */


				runner = list_next(runner);
			}
		}
		else
		{
			fprintf(stderr, "Error loading configuration.\n");
			exit(1);
		}
	}

	printf("Dumping the L2\n");
	trie_dump(l2);

	pthread_create(&dump_thread, NULL, dump_thread_fn, NULL);	
	write_pid_file();
	create_message_queues();
	
	for (;;)
	{
		printf("Waiting for a message...\n");
		msgrcv(mq_req, &req, TIB_REQUEST_SIZE, 0, 0);
		printf("Someone requested %s\n", req.key);

		if (req.mtype == 1)
		{
			res.mtype = 1;

			if (lookup(req.key, l1, l2, &tf, &files, miss) == TIBOR_FOUND)
			{
				res.shmid = tf.shmid;
				res.offset = tf.offset;
			}	
			else
			{
				res.shmid = -1;
				res.offset = -1;
			}

			printf("%d, %d\n", res.shmid, res.offset);

			msgsnd(mq_res, &res, TIB_RESPONSE_SIZE, 0);
		}
	}

	if (unlink("/var/run/tibor.pid") == -1)
	{
		perror("unlink");
		exit(1);
	}

	return EXIT_SUCCESS;
}

