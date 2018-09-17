#ifndef SH_H
#define SH_H

#include "platform.h"

extern uint8_t CONSOLE_ECHO;

#ifdef __cplusplus
extern "C" {
#endif
  int8_t main_sh(char *argv[], char *env[]);
#ifdef __cplusplus
}
#endif

#endif
