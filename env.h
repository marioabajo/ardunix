#ifndef ENV_H
#define ENV_H

#include "platform.h"

#ifdef __cplusplus
extern "C"{
#endif
  char ** env_free(char *env[]);
  uint8_t env_add(char *env[], char *key, char *value);
  uint8_t env_add_l(char *env[], char *key, uint8_t lenk, char *value, uint8_t lenv);
  char * env_get(char *env[], char *key);
  char * env_get_l(char *env[], char *key, uint8_t lenk);
#ifdef __cplusplus
}
#endif

#endif

