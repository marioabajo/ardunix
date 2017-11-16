#include "env.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

uint8_t env_search(char *env[], char *key, uint8_t lenk)
{
  uint8_t i;

  // Look for the item in the list
  i = 0;
  while (i < ENV_MAX)
  {
    // first, look for the equal and if the key size math our given length, compare it
    if (env[i] != NULL)
      if (env[i][lenk] == '=')
         if (strncmp(key, env[i], lenk) == 0)
          return i;

    i++;
  }
  return 255; // Not found
}

// add/update new environment variable
uint8_t env_add(char *env[], char *key, char *value)
{
  return env_add_l(env, key, strlen(key), value, strlen(value));
}

// add/update new environment variable (with string limits)
uint8_t env_add_l(char *env[], char *key, uint8_t lenk, char *value, uint8_t lenv)
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
    // if value is empty, then delete the env
    if (lenv == 0)
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
    return 3; // cannot allocate memory
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

// get a value from a environment variable
char * env_get(char *env[], char *key)
{
  return env_get_l(env, key, strlen(key));
}

// get a value from a environment variable (with string limits)
char * env_get_l(char *env[], char *key, uint8_t lenk)
{
  uint8_t i;
  char *p = NULL;
 
  if (env == NULL)
    return NULL; // no environment defined

  if ((i = env_search(env, key, lenk)) != 255)
    p = strchr(env[i], '=') + 1;

  return p;
}

/*
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
