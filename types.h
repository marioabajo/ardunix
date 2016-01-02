#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

typedef unsigned char u8;
typedef bool boolean;
typedef unsigned short int u16;

struct dict_list
{
	char *key;
	char *value;
	struct dict_list *next;
};

#endif

