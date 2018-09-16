#include "env.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static uint8_t env_search(char *env[], char *key, uint8_t lenk)
/* Search a key in the env and return it index in the array
 * 
 * Input: array with environment variables NULL terminated,
 *        string with the key (NULL terminated),
 *        the position of the '=' sign
 * Returns: 0 - 254 -> index
 *          255     -> Not found
 */
{
	uint8_t i=0;

	// Look for the item in the list
	while (i < ENV_MAX && env[i] != NULL)
	{
		// first, search the equal and if the key size match our given 
		// length, compare it
		if (env[i][lenk] == '=' && ! strncmp(key, env[i], lenk))
			return i;
		i++;
	}
	return 255; // Not found
}

char ** env_free(char *env[])
/* Free the memory reserved by the environment
 * 
 * Input: array with environemnt variables NULL terminated,
 * Returns: NULL if correctly freed
 */
{
	uint8_t i = 0;

	if (env == NULL)
		return NULL; // no environment defined

	while (i < ENV_MAX && env[i] != NULL)
		free(env[i++]);
	
	free(env);
	
	return env;
}

uint8_t env_add(char *env[], char *key, char *value)
/* Add an environment variable
 * 
 * Input: array with environemnt variables NULL terminated,
 *        string with the key (NULL terminated),
 *        string with the value (NULL terminated)
 * 
 * Returns: see env_add_l
 */
{
	return env_add_l(env, key, strlen(key), value, strlen(value));
}

uint8_t env_add_l(char *env[], char *key, uint8_t lenk, char *value, uint8_t lenv)
/* Add of update an environment variable
 * 
 * Input: array with environemnt variables NULL terminated,
 *        string with the key,
 *        the lenght of the string key,
 *        string with the value,
 *        the lenght of the string value
 *        (if lenv value is 0, the variable will be deleted)
 * Returns: 0 -> ok
 *          1 -> environment invalid
 *          2 -> too much env variables
 *          ENOMEM -> Not enough memory
 */
{
	uint8_t i;
	char *p;
 
	if (env == NULL)
		return 1; // no environment defined

	if ((i = env_search(env, key, lenk)) != 255)
	{
		// found, erase old value
		free(env[i]);
		env[i] = NULL;
		// if the value is empty, then delete the env
		if (! lenv)
			return 0;
	}
	else
	{
		// find an empty hole in the list
		for (i=0; i < ENV_MAX && env[i] != NULL; i++);
		if (i == ENV_MAX)
			return 2; // too much env variables
	}
  
	// Add value
	if ((env[i] = malloc(lenk + lenv + 2)) == NULL)
		return ENOMEM; // cannot allocate memory
	// following code is equivalent to this one:
	//sprintf_P(env[i], PSTR("%.*s=%.*s"), lenk, key, lenv, value);
	p = env[i];
	memcpy(p, key, lenk);
	p += lenk;
	*p = '=';
	p++;
	memcpy(p, value, lenv);
	p += lenv;
	*p = 0;

	return 0;
}

char * env_get(char *env[], char *key)
/* Return the pointer to the value string given the key string
 * 
 * Input: array with environemnt variables NULL terminated,
 *        string with the key (NULL terminated),
 * Returns: see env_get_l
 */
{
	return env_get_l(env, key, strlen(key));
}

char * env_get_l(char *env[], char *key, uint8_t lenk)
/* Return the pointer to the value string given the key string
 * 
 * Input: array with environemnt variables NULL terminated,
 *        string with the key,
 *        lenght of the key string
 * Returns: NULL -> environment invalid or key not found
 *          rest of values -> pointer to string
 *          
 */
{
	uint8_t i;
	char *p = NULL;

	if (env == NULL)
		return NULL; // no environment defined

	if ((i = env_search(env, key, lenk)) != 255)
		p = strchr(env[i], '=') + 1;

	return p;
}

