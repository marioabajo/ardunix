#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

typedef unsigned char u8;
typedef bool boolean;

#ifndef __AVR__
typedef unsigned short int u16;
#else
//typedef int u16;
#endif

struct dict_list
{
	char *key;
	char *value;
	struct dict_list *next;
};

#endif

