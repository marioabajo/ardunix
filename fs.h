#ifndef FS_H
#define FS_H

#include "types.h"

struct fsentry
{
	const struct fsentry *parent;
	const char *filename;
	const void (*inode)(u8 argc, char *argv[]);  // Points to function in memory
	u8 flags;  /* bits -> 12345678  Type= 00000 File
		                    RWX Type        01000 Directory
		                                    10000 Especial file
		                                    11000  |- Gpio
		   */
};

#endif

