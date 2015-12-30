#ifndef ENV_H
#define ENV_H

#include "defines.h"
#include "types.h"
#include <string.h>


boolean env_add(struct dict_list **env, const char *key, const char *value);
boolean env_del(struct dict_list **env, const char *key);
char * env_get(struct dict_list *env, const char *key);

#endif

