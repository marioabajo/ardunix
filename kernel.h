#ifndef KERNEL_H
#define KERNEL_H

#include "platform.h"
#include "fs.h"
#include "env.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t exec(const char *filename, const char *argv[]);
  uint8_t execve(const char *filename, const char *argv[], char *envp[]);
#ifdef __cplusplus
}
#endif

#endif
