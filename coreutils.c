#include "coreutils.h"
#include <stdio.h>
#include <string.h>

// list files and directories
uint8_t ls(uint8_t argc, char *argv[])
{
  /*DIR *dir;

  //TODO: add more options
  if (argc > 0)
    dir = opendir(argv[1]);
  else
    dir = opendir("/");

  if (dir == NULL)
    return -1;

  while (dir != NULL)
  {
    //TODO: print more attributes
    printf("%s\n", dir->filename);
    dir = readdir(dir);
  }
  argc--;*/

  return 0;
}

// Print free ram
uint8_t freeMem (uint8_t argc, char *argv[])
{
#ifdef __AVR__
  extern unsigned int __heap_start;
  extern void *__brkval;
  struct __freelist{
    size_t sz;
    struct __freelist *nx;
  } *current;
  extern struct __freelist *__flp;
  int free_memory;
  int total = 0;

  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += total;
  }
  printf_P(PSTR("Free: %d\n"),free_memory);
  
  /*extern int __heap_start, *__brkval;
  int v;
  printf("Free: %d\n", (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));*/
#else
  printf("Free not implemented in this arch.\n");
#endif
}

uint8_t times (uint8_t argc, char *argv[])
{
  long t1=millis();

  if (argc>0)
  {
    //TODO: pass env to execve
    execve(argc-1,&(argv[1]),NULL);
  }
  printf_P(PSTR("millis: %l\n"),millis()-t1);
  return 0;
}

