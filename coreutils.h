#ifndef UTILS_H
#define UTILS_H

#include "platform.h"

#ifdef __cplusplus
extern "C"{
#endif
  int8_t main_ls(char *argv[]);
  int8_t main_ps(void);
  int8_t main_pwd(void);
  int8_t main_cd(char *argv[], char *env[]);
  int8_t main_free (void);
  int8_t main_times (char *argv[]);
  int8_t main_set(char *argv[], char *env[]);
  int8_t main_true();
  int8_t main_false();
  int8_t main_cat(char *argv[]);
  int8_t main_echo(char *argv[]);
  int8_t main_debug(char *argv[]);
#ifdef __cplusplus
}
#endif

#endif
