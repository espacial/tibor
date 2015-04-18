#include "json.h"

/* int */
/* main(int argc, char* argv[]) */
/* { */
/* 	tibor_cfg_t tibor_config; */
	
/* 	if (argc != 2) */
/* 		return TIBOR_ERROR; */

/* 	if (configure_tibor(argv[1], &tibor_config) == TIBOR_OK) */
/* 	{ */
/* 		printf("tiny test\n"); */
/* 		printf("%s\n", tibor_config.path); */
/* 		printf("%s\n", list_data(tibor_config.pools)); */
/* 	} */
/* } */

struct conf* 
load_conf(char* file_path)
{
	json_t* configuration;
	void* iter;
	const char* key;
	json_t* value,
	json_t* value_2
	json_t* value_3; 
	int path_length;
	json_t* mushrooms; 
	struct mushroom_conf* mush_item;
	struct conf* config;

	config = malloc(sizeof(struct conf));
	configuration = load_json_file(file_path);

	if (configuration == NULL)
		return NULL;

	value = json_object_get(configuration, "path");
	strncpy(config->path, json_string_value(value), strlen(json_string_value(value)));
	
	value = json_object_get(configuration, "l1_size");
	config->l1_size = json_integer_value(value);
	
	config->mushrooms = NULL;
	value = json_object_get(configuration, "mushrooms");
	iter = json_object_iter(value);
	while (iter)
	{
		mush_item = malloc(sizeof(struct mushroom_conf));
		value_2 = json_object_iter_value(iter);
		key = json_object_iter_key(iter);
		iter = json_object_iter_next(value, iter);

		strncpy(mush_item->file_path, key, strlen(key));
		
		value_3 = json_object_get(value_2, "on_l1");
		if (value_3 != NULL)
		{
			if (json_typeof(value_3) == JSON_TRUE)
				mush_item->on_l1 = true;
			else if (json_typeof(value_3) == JSON_FALSE)
				mush_item->on_l1 = false;	
		}			
		else
			return NULL;
		
		value_3 = json_object_get(value_2, "on_delete");
		if (value_3 != NULL)
			mush_item->on_delete = action_behavior(json_string_value(value_3));
		else
			return NULL;
		
		value_3 = json_object_get(value_2, "on_modify");
		if (value_3 != NULL)
			mush_item->on_modify = action_behavior(json_string_value(value_3));
		else
			return NULL;
		
		value_3 = json_object_get(value_2, "on_attrib");
		if (value_3 != NULL)
			mush_item->on_attrib = action_behavior(json_string_value(value_3));
		else
			return NULL;
		
		value_3 = json_object_get(value_2, "on_rename");
		if (value_3 != NULL)
			mush_item->on_delete = action_behavior(json_string_value(value_3));
		else
			return NULL;
	
		config->mushrooms = list_add(config->mushrooms, list_create(mush_item));
	}

	return config;
}

static int
action_behavior(const char* behavior)
{
	if (strcmp(behavior, "drop") == 0)
		return TIBOR_DROP;

	if (strcmp(behavior, "update") == 0)
		return TIBOR_UPDATE;

	if (strcmp(behavior, "keep") == 0)
		return TIBOR_KEEP;

	if (strcmp(behavior, "follow") == 0)
		return TIBOR_FOLLOW;

	return TIBOR_ERROR;
}

static json_t* 
load_json_file (char* file_path)
{
	FILE* fp;
	json_error_t error;
	json_t* config;
	
	fp = fopen(file_path, "r");
	if (fp == NULL)
		return TIBOR_ERROR;

	config = malloc(sizeof(json_t));
	*config = json_loadf(fp, 0, &error);
	fclose(fp);

	return config;
}

