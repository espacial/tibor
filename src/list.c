#include <stdlib.h>

#include "list.h"

struct list*
list_create(void* data)
{
	struct list* l;
	l = malloc(sizeof(struct list));

	l->data = data;
	l->next = NULL;
	l->prev = NULL;

	return l;	
}

struct list*
list_add(struct list* head, struct list* to_add)
{
	if (head == NULL)
		return to_add;

	to_add->next = head;
	head->prev = to_add;

	return to_add;
}

void*
list_data(struct list* element)
{
	return element->data;
}

struct list*
list_next(struct list* element)
{
	return element->next;
}

