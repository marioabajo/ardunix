#ifndef SH_H
#define SH_H

#include <stdio.h>
#include "env.h"
#include "fs.h"

void parsecmd(u8 argc, char *argv[]);
boolean getcmd(u8 buff[]);
u8 splitcmd(u8 cmd[], char *args[]);
u8 sh(u8 argc, char *argv[]);

#endif
