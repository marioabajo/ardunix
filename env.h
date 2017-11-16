#ifndef ENV_H
#define ENV_H

#include "platform.h"

/*
struct dict_list
{
	char *key;
	char *value;
	struct dict_list *next;
};
*/
#ifdef __cplusplus
extern "C"{
#endif
  uint8_t env_search(char *env[], char *key, uint8_t lenk);
  uint8_t env_add(char *env[], char *key, char *value);
  uint8_t env_add_l(char *env[], char *key, uint8_t lenk, char *value, uint8_t lenv);
  char * env_get(char *env[], char *key);
  char * env_get_l(char *env[], char *key, uint8_t lenk);
/*
#ifdef DEBUG
  void env_test();
#endif
*/
#ifdef __cplusplus
}
#endif

#endif

