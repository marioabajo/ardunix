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
/*  bool env_add(struct dict_list **env, const char *key, const char *value);
  bool env_del(struct dict_list **env, const char *key);
  char * env_get(struct dict_list *env, const char *key);
  char * env_nget(struct dict_list *env, const char *key, uint8_t n);
#ifdef DEBUG
  void env_test();
#endif
*/
  bool env_add_str(char **env, char *str, uint8_t len, uint8_t pos);
#ifdef __cplusplus
}
#endif

#endif

