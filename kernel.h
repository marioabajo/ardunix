#ifndef KERNEL_H
#define KERNEL_H

#include "platform.h"
#include "fs.h"
#include "progfs.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t execve(uint8_t argc, char *argv[], char *envp[]);
#ifdef __cplusplus
}
#endif

#endif
