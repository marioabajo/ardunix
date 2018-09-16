#ifndef KERNEL_H
#define KERNEL_H

#include "platform.h"

struct proc
{
	uint8_t state;           /* states:
					0 -> empty process, pid non used
					1 -> stopped
					2 -> running
					3 -> exitting
	                         */
	char *name;
	char cwd[PATH_MAX];
};

extern struct proc procs[PID_MAX];
extern int8_t current_proc;

#ifdef __cplusplus
extern "C"{
#endif
  int8_t init_proc(void);
  int8_t execl(const char *argv, ...);
  int8_t execl_P(const PROGMEM char *argv, ...);
  int8_t exec(const char *argv[]);
  int8_t execve(const char *argv[], char *env[]);
#ifdef __cplusplus
}
#endif

#endif
