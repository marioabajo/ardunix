#ifndef SH_H
#define SH_H

#include "coreutils.h"
// include this to initialice the rootfs in main
#include "fs.h"
#include "env.h"

#ifdef __cplusplus
extern "C" {
#endif
  /*uint8_t parsecmd(uint8_t argc, char *argv[], struct dict_list *env);
  bool getcmd(uint8_t buff[]);
  uint8_t splitcmd(uint8_t cmd[], char *args[]);*/
  uint8_t sh(uint8_t argc, char *argv[], struct dict_list **env);
#ifdef __cplusplus
}
#endif

#endif
