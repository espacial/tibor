#ifndef LIST_H
#define LIST_H

struct list
{
	void* data;
	struct list* next;
	struct list* prev;
};

struct list*
list_create(void* data);

struct list*
list_add(struct list* head, struct list* to_add);

void*
list_data(struct list* element);

struct list*
list_next(struct list* element);

#endif

