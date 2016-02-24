#include "coreutils.h"
#include <stdio.h>
#include <string.h>
#ifdef __AVR__
#include <Arduino.h>
#endif

// list files and directories
uint8_t ls(uint8_t argc, char *argv[])
{
  DIR *dir;
  struct dirent *entry;
  char c;

  //TODO: add more options
  if (argc > 0)
    dir = opendir(argv[1]);
  else
    dir = opendir("/");

  if (dir == NULL)
    return -1;

  entry = readdir(dir);
  while (entry != NULL)
  {
    switch(entry->flags & 0xc0)
    {
      case 0x0:  // File
        c = '-';
        break;
      case 0x40: // Directory
        c = 'd';
        break;
      case 0x80: // Link
        c = 'l';
        break;
      case 0xc0: // Device
        c = 'c'; // TODO: Another case for each type of dev
    }
    putchar(c);
    c = (entry->flags & 0x4) ? 'r' : '-';
    putchar(c);
    c = (entry->flags & 0x2) ? 'w' : '-';
    putchar(c);
    c = (entry->flags & 0x1) ? 'x' : '-';
    putchar(c);
    printf(" %4d %s\n", entry->size, entry->d_name);
    entry = readdir(dir);
  }
  closedir(dir);

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
  unsigned long t1 = millis();

  if (argc>0)
  {
    //TODO: pass env to execve
    execve(argc-1, &(argv[1]), NULL);
  }
  printf_P(PSTR("millis: %ld\n"), millis() - t1);
  return 0;
}

