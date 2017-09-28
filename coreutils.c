#include "coreutils.h"
#include <stdio.h>
#include <string.h>
#ifdef __AVR__
#include <Arduino.h>
#endif

void ls_print_entry(struct dirent *entry, const char *entry_name)
{
  char c = 0;
  char *name = entry->d_name;

  switch(entry->flags & FS_MASK_FILETYPE)
  {
    case FS_FILE:  // File
      c = '-';
      break;
    case FS_DIR: // Directory
      c = 'd';
      break;
    case FS_LINK: // Link
      c = 'l';
      break;
    case FS_DEV: // Device
      c = 'c'; // TODO: Another case for each type of dev
  }
  putchar(c);
  c = (entry->flags & FS_READ) ? 'r' : '-';
  putchar(c);
  c = (entry->flags & FS_WRITE) ? 'w' : '-';
  putchar(c);
  c = (entry->flags & FS_EXEC) ? 'x' : '-';
  putchar(c);
  if (entry_name != NULL)
    name = (char *) entry_name;
  printf_P(PSTR(" %4d %s\n"), entry->size, name);
}

// list files and directories
uint8_t main_ls(uint8_t argc, char *argv[])
{
  DIR dir;
  struct dirent *entry;
  uint8_t aux;

  //TODO: add more options
  //printf_P(PSTR("args: %d\n"), argc);
  if (argc > 0)
    aux = opendir(argv[1], &dir);
  else
    aux = opendir("/", &dir);

  if (aux != 0)
    return 1;
  
  ls_print_entry(&(dir.dd_ent),".");

  while ((entry = readdir(&dir)) != NULL)
      ls_print_entry(entry, NULL);

  //closedir(&dir);

  return 0;
}

// Print free ram
uint8_t main_free (uint8_t argc, char *argv[])
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
  return 0;
}

uint8_t main_times (uint8_t argc, char *argv[])
{
  unsigned long t1;
  uint8_t i = 0;
  
  t1=millis();
  if (argc > 0)
  {
    //TODO: pass env to execve
    //FIXME: look like it's not passing correctly the parameters to the command
    i = execve(argv[1], &argv[2], NULL);
  }
  printf_P(PSTR("millis: %ld\n"), millis() - t1);
  return i;
}

// list environment variables
uint8_t main_set(uint8_t argc, char *argv[], char *env[])
{
  /*struct dict_list *c=*env;

  while (c != NULL)
  {
    printf("%s=%s\n", c->key, c->value);
    c = (struct dict_list *) c->next;
  }*/

  return 0;
}

uint8_t main_true(uint8_t argc, char *argv[], char *env[])
{
  return 0;
}

uint8_t main_false(uint8_t argc, char *argv[], char *env[])
{
  return 1;
}

uint8_t main_cat(uint8_t argc, char *argv[], char *env[])
{
  FD fd;
  char c;
  
  if (argc == 0)
    return 0;
    
  if ((open(argv[1], 0, &fd)) != 0)
    return 1;

  while (read(&fd, &c, 1) != 0)
    printf("%c",c);

  return 0;
}


