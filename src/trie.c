#include "trie.h"

static int
level_hash (char key)
{
	if (key == '-')
		return 0;
	
	if (key == '_')
		return 1;

	if (key == ' ')
		return 2;

	if (key >= 'a' && key <= 'z')
		return (key - 'a') + 3;

	if (key >= 'A' && key <= 'Z')
		return (key - 'A') + 3 + 26;

	return -1;
}

static int
key_valid (char* key)
{
	unsigned int i;

	for (i = 0; key[i] != '\0'; i++)
		if (level_hash(key[i]) < 0)
			return 0;
			
	return 1;
}

struct trie*
trie_create ()
{
	struct trie* node;
	unsigned int i;

	node = malloc(sizeof(struct trie));

	for (i = 0; i < TRIE_KEY_SET_SIZE; i++)
		node->children[i] = NULL;

	node->key = '\0';
	node->tf = NULL;

	return node;
}

int
trie_set (struct trie* root, char* key, struct tib_file* tf)
{
	unsigned int i;
	unsigned int h;
	struct trie* node;

	if (!key_valid(key))
		return TRIE_KEY_INVALID;

	node = root;

	for (i = 0; key[i] != '\0'; i++)
	{
		h = level_hash(key[i]);

		if (node->children[h] == NULL)
		{
			node->children[h] = trie_create();
			node->children[h]->key = key[i];
			node->children[h]->tf = NULL;
		}

		node = node->children[h];
	}

	node->tf = tf;

	return TRIE_FOUND;
}

int
trie_remove (struct trie* root, char* key)
{
	unsigned int i;
	struct trie* node;

	if (!key_valid(key))
		return TRIE_KEY_INVALID;

	node = root;

	for (i = 0; key[i] != '\0'; i++)
		if ((node = node->children[level_hash(key[i])]) == NULL)
			return TRIE_NOT_FOUND;

	if (node->tf != NULL)
	{
		node->tf = NULL;
	}

	return TRIE_FOUND;
}

static void
dump (struct trie* node, unsigned int level)
{
	unsigned int i;
	
	for (i = 0; i < level; i++)
		printf("  ");

	printf("%c", node->key);
	if (node->tf != NULL)
		printf("*");
	printf("\n");

	for (i = 0; i < TRIE_KEY_SET_SIZE; i++)
		if (node->children[i] != NULL)
			dump(node->children[i], level+1);
}

int
trie_dump (struct trie* root)
{
	dump(root, 0);	
	return TRIE_FOUND;
}

int
trie_get (struct trie* root, char* key, struct tib_file** tf_out)
{
	unsigned int i;
	struct trie* node;

	if (!key_valid(key))
		return TRIE_KEY_INVALID;

	node = root;

	for (i = 0; key[i] != '\0'; i++)
		if ((node = node->children[level_hash(key[i])]) == NULL)
			return TRIE_NOT_FOUND;

	if (node->tf != NULL)
	{
		*tf_out = node->tf;
		return TRIE_FOUND;
	}
	else
		return TRIE_NOT_FOUND;
}

int
trie_contains (struct trie* root, char* key)
{
	unsigned int i;
	struct trie* node;

	if (!key_valid(key))
		return TRIE_KEY_INVALID;

	node = root;

	for (i = 0; key[i] != '\0'; i++)
		if ((node = node->children[level_hash(key[i])]) == NULL)
			return TRIE_NOT_FOUND;

	return TRIE_FOUND;
}

