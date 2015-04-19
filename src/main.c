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

key_t key;
int mq_id;
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
create_message_queue(void)
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

	key = 111;
	if ((mq_id = msgget(key, IPC_CREAT | 0777)) == -1)
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
		if ((config = load_conf(argv[1])) != NULL)
		{
			struct list* runner;
			unsigned int l1_idx = 0;
			for (runner = config->mushrooms; runner != NULL; )
			{
				mc = list_data(runner);

				struct tib_file* tf;
				tf = malloc(sizeof(struct tib_file));
				tf->l1_hits = 0;
				tf->l2_hits = 0;
				tf->misses = 0;
				tf->always_l1 = mc->on_l1;

				int shmid;
				char* shmptr;
				key_t key;
				struct stat sb;

				stat(tf->path, &sb);

				if ((key = ftok(tf->path, 'a') == -1))
				{
					perror("ftok");
					exit(1);
				}

				if ((shmid = shmget(key, sb.st_size, IPC_CREAT | 0777)) == -1)
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

				snprintf(tf->path, 1024, "%s/%s", config->root, mc->file_path);

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

				runner = list_next(runner);
			}
		}
		else
		{
			fprintf(stderr, "Error loading configuration.\n");
			exit(1);
		}
	}

	trie_dump(l2);

	pthread_create(&dump_thread, NULL, dump_thread_fn, NULL);	
	write_pid_file();
	create_message_queue();
	
	for (;;)
	{
		printf("Waiting for a message...\n");
		msgrcv(mq_id, &req, TIB_REQUEST_SIZE, 0, 0);

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

			msgsnd(mq_id, &res, TIB_RESPONSE_SIZE, 0);
		}
	}

	if (unlink("/var/run/tibor.pid") == -1)
	{
		perror("unlink");
		exit(1);
	}

	return EXIT_SUCCESS;
}

