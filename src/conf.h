#ifndef TIBOR_CONF_H
#define TIBOR_CONF_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <jansson.h>

#include "list.h"

#define TIBOR_ERROR 0
#define TIBOR_OK 1

#define TIBOR_DROP 2
#define TIBOR_KEEP 3 
#define TIBOR_UPDATE 4
#define TIBOR_FOLLOW 5

#define TIBOR_ON_L1 6
#define TIBOR_NOT_ON_L1 7

/* struct pool_cfg */ 
/* { */
/* 	char pattern[64]; */
/* 	uint32_t size; */
/* 	uint8_t on_delete; */
/* 	uint8_t on_modify; */
/* 	uint8_t on_attrib; */
/* 	uint8_t on_rename; */
/* }; */

struct mushroom_conf
{
	char file_path[512];
	uint8_t on_delete;
	uint8_t on_modify;
	uint8_t on_attrib;
	uint8_t on_rename;
	uint8_t on_l1;
};

struct conf
{
	char root[512];
	uint32_t max_file_size;
	uint16_t l1_size;
	/* struct list* pools; */
	struct list* mushrooms;  
};

int load_conf(char* file_path, struct conf* config);
/* static int action_behavior(const char* behavior); */
/* static int load_configuration (char* file_name, json_t** configuration); */

#endif
