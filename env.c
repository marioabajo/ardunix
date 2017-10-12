#include "env.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// add/update new environment variable
bool env_add_str(char *env[], char *str, uint8_t len, uint8_t pos)
{
  uint8_t i;
  
  if (env == NULL)
    return 1; // no environment defined

  // Look for the last item in the list
  i = 0;
  while (env[i] != NULL && i < ENV_MAX)
  {
    // found an existing env with the same name, overwrite
    if (strncmp(str, env[i], pos) == 0)
    {
      free(env[i]);
      env[i] = NULL;
      break;
    }
    i++;
  }

  // if it's an empty env variable 'x=' then delete the env
  if (pos == len - 1)
    return 0;

  if (i >= ENV_MAX)
    return 2; // too much env variables defined

  if ((env[i] = malloc(len + 1)) == NULL)
    return 3; // cannot allocate memory

  // copy 
  memcpy(env[i], str, len);
  // end string with \0 just in case
  env[i][len] = 0;
  
  return 0;
}

/*
// add/update new environment variable
bool env_add(struct dict_list **env, const char *key, const char *value)
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
bool env_del(struct dict_list **env, const char *key)
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
char * env_nget(struct dict_list *env, const char *key, uint8_t n)
{
	while (env != NULL)
	{
		// Found a matching key
		if (strncmp(env->key,key,n) == 0)
			return env->value;
		env = (struct dict_list *) env->next;
	}
	return NULL;
}

char * env_get(struct dict_list *env, const char *key)
{
	return env_nget(env, key, 0);
}

#ifdef DEBUG
void env_test()
{
  struct dict_list *env=NULL;

  // debug: environment functions
  printf("env: C?  %s\n",env_get(env,"C")); // get nonexistant var, empty env
  printf("env: del B  (%d)\n", env_del(&env,"B")); // del nonexistant var, empty env
  printf("env: A = 1  (%d)\n", env_add(&env,"A","1")); // add var
  printf("env: A?  %s\n",env_get(env,"A")); // get var
  printf("env: HOME = aSd  (%d)\n", env_add(&env,"HOME","aSd")); // add vars
  printf("env: B = 23  (%d)\n", env_add(&env,"B","23"));
  printf("env: A = 4  (%d)\n", env_add(&env,"A","4"));
  printf("env: A?  %s\n",env_get(env,"A")); // get var
  printf("env: del A  (%d)\n", env_del(&env,"A")); // del var
  printf("env: A?  %s\n",env_get(env,"A")); // get deleted var
  printf("env: B?  %s\n",env_get(env,"B")); // get var next to deleted
  printf("env: HOME?  %s\n",env_get(env,"HOME"));
}
#endif*/
