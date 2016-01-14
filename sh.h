#ifndef SH_H
#define SH_H

#include <stdio.h>
#include "env.h"
#include "fs.h"

extern struct fsentry fs;

#ifdef __cplusplus
extern "C"{
#endif
  void parsecmd(u8 argc, char *argv[]);
  boolean getcmd(u8 buff[]);
  u8 splitcmd(u8 cmd[], char *args[]);
  u8 sh(u8 argc, char *argv[]);
  u8 ls(u8 argc, char *argv[]);
  u8 freeMem (u8 argc, char *argv[]);
#ifdef __cplusplus
}
#endif

/*
#define _FILE (const void (*)(u8, char**))
#define ROOT 0
#define BIN 1
#define DEV 2

// TODO: dynamize this structure at compile time
const struct fsentry fs[]={
  // root
  {ROOT,"/",NULL,0x45},
  
  // first level directories
  {ROOT,"bin",NULL,0x45},
  {ROOT,"dev",NULL,0x45},

  // files
  {BIN, "ls", _FILE ls, 0x5},
  {BIN, "free", _FILE freeMem, 0x5}
};

//#define fsentries ARRAY_SIZE(fs)
*/

#endif
