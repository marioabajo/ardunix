#ifndef UTILS_H
#define UTILS_H

#include "kernel.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t ls(uint8_t argc, char *argv[], struct dict_list **env);
  uint8_t freeMem (uint8_t argc, char *argv[], struct dict_list **env);
  uint8_t times (uint8_t argc, char *argv[], struct dict_list **env);
  uint8_t set(uint8_t argc, char *argv[], struct dict_list **env);
#ifdef __cplusplus
}
#endif

#endif
