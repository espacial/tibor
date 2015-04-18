#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>

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

key_t key;
int mq_id;
int key_fd;
struct l1_entry l1[10];
struct trie* l2;

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

int
main(int argc, char* argv[])
{
	struct tib_request req;
	struct tib_response res;
	struct tib_file tf;
	struct conf *config;

	if (instance_exists())
	{
		fprintf(stderr, "Another instance of tibor is running.\n");
		return EXIT_FAILURE;
	}
	
	if (argc > 0)
	{
		if ((config = load_conf(argv[1])) != NULL)
		{
			/* PUT STUFF INTO L2 */
			struct list* runner;
			for (runner = config->mushrooms; runner != NULL; )
			{
				struct mushroom_conf* mc = list_data(runner);
				printf("MC: %s\n", mc->file_path);
				runner = list_next(runner);
			}
		}
		else
		{
			fprintf(stderr, "Error loading configuration.\n");
			exit(1);
		}
	}

	daemon(0, 1);
	write_pid_file();
	create_message_queue();
	
	for (;;)
	{
		printf("Waiting for a message...\n");
		msgrcv(mq_id, &req, TIB_REQUEST_SIZE, 0, 0);

		if (req.mtype == 1)
		{
			res.mtype = 1;

			if (lookup(req.key, l1, l2, &tf) == TIBOR_FOUND)
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

