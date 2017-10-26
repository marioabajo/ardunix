#ifndef KERNEL_H
#define KERNEL_H

#include "platform.h"
#include "fs.h"
#include "env.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t execl(const char *argv, ...);
  //uint8_t execle(const char *argv, ...);
  uint8_t exec(const char *argv[]);
  uint8_t execve(const char *argv[], char *env[]);
#ifdef __cplusplus
}
#endif

#endif
