#ifndef UTILS_H
#define UTILS_H

#include "platform.h"

#ifdef __cplusplus
extern "C"{
#endif
  uint8_t ls(uint8_t argc, char *argv[]);
  uint8_t freeMem (uint8_t argc, char *argv[]);
#ifdef __cplusplus
}
#endif

#endif
