#ifndef UTILS_H
#define UTILS_H

#include "kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t main_ls(char *argv[]);
  uint8_t main_cd(char *argv[], char *env[]);
  uint8_t main_free (char *argv[]);
  uint8_t main_times (char *argv[]);
  uint8_t main_set(char *argv[], char *env[]);
  uint8_t main_true();
  uint8_t main_false();
  uint8_t main_cat(char *argv[]);
  uint8_t main_echo(char *argv[]);
  uint8_t main_debug(char *argv[]);
#ifdef __cplusplus
}
#endif

#endif
