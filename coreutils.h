#ifndef UTILS_H
#define UTILS_H

#include "kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t main_ls(uint8_t argc, char *argv[]);
  uint8_t main_free (uint8_t argc, char *argv[]);
  uint8_t main_times (uint8_t argc, char *argv[]);
  uint8_t main_set(uint8_t argc, char *argv[], char *env[]);
  uint8_t main_true(uint8_t argc, char *argv[], char *env[]);
  uint8_t main_false(uint8_t argc, char *argv[], char *env[]);
  uint8_t main_cat(uint8_t argc, char *argv[], char *env[]);
#ifdef __cplusplus
}
#endif

#endif
