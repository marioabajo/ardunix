#include <stdlib.h>
#include "env.h"

//#include <stdio.h>

// add/update new environment variable
boolean env_add(struct dict_list **env, const char *key, const char *value)
{
	struct dict_list *last=NULL, *a=*env;
	// go to the last key checking if there is already a matching key
	while (a != NULL)
	{
		// Found a matching key, update value
		if (strcmp(a->key,key) == 0)
		{
			free(a->value);
			// If we cannot reserve memory, the value will be NULL
			if ((a->value = malloc(strlen(value))) == NULL)
				return false;
			strcpy(a->value, value);
			return true;
		}
		last = a;
		a = (struct dict_list *) a->next;
	}
	if (((a = malloc(sizeof(struct dict_list))) == NULL) ||
	    ((a->key = malloc(strlen(key))) == NULL) ||
	    ((a->value = malloc(strlen(value))) == NULL))
	{
		free(a->key);
		free(a);
		return false;
	}
	a->next = NULL;
	strcpy(a->key, key);
	strcpy(a->value, value);
	if (*env == NULL)
		*env = a;
	else
		last->next = a;
	return true;
}

// delete environment variable
boolean env_del(struct dict_list **env, const char *key)
{
	struct dict_list *last=NULL, *a=*env;
	while (a != NULL)
	{
		if (strcmp(a->key, key) == 0)
		{
			if (last == NULL)
				*env = (struct dict_list *) (*env)->next;
			else
				last->next = a->next;
			free(a->key);
			free(a->value);
			free(a);
			return true;
		}
		last = a;
		a = (struct dict_list *) a->next;
	}
	return false;
}

// get a value from a environment variable
char * env_get(struct dict_list *env, const char *key)
{
	while (env != NULL)
	{
		// Found a matching key
		if (strcmp(env->key,key) == 0)
			return env->value;
		env = (struct dict_list *) env->next;
	}
	return NULL;
}


