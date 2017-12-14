#ifndef KERNEL_H
#define KERNEL_H

#include "platform.h"

#ifdef __cplusplus
extern "C"{
#endif
  int8_t execl(const char *argv, ...);
  int8_t execl_P(const PROGMEM char *argv, ...);
  int8_t exec(const char *argv[]);
  int8_t execve(const char *argv[], char *env[]);
#ifdef __cplusplus
}
#endif

#endif
