#ifndef SH_H
#define SH_H

#include "env.h"
#include "fs.h"

#ifdef __cplusplus
extern "C"{
#endif
  void parsecmd(u8 argc, char *argv[]);
  boolean getcmd(u8 buff[]);
  u8 splitcmd(u8 cmd[], char *args[]);
  u8 sh(u8 argc, char *argv[]);
  u8 ls(u8 argc, char *argv[]);
#ifdef __cplusplus
}
#endif


const struct fsentry fs[fsentries]={
  // root
  {NULL,"/",NULL,0x51},
 
  // first level directories
  {&fs[0],"bin",NULL,0x51},

  // files
  {&fs[1], "ls", (const void (*)(u8, char**)) ls, 0x50}
};


#endif
