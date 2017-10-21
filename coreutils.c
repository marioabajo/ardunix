#include "coreutils.h"
#include <stdio.h>
#include <string.h>
#ifdef __AVR__
#include <Arduino.h>
#endif

void ls_print_entry(struct dirent *entry, const char *entry_name)
{
  char c = 0;
  uint8_t f = entry->flags;

  switch(f & FS_MASK_FILETYPE)
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

  printf_P(PSTR("%c%c%c%c %4d %s\n"), c, (f & FS_READ) ? 'r' : '-'
                                       , (f & FS_WRITE) ? 'w' : '-'
                                       , (f & FS_EXEC) ? 'x' : '-'
                                       , entry->size, entry_name);
}

// list files and directories
uint8_t main_ls(char *argv[])
{
  DIR dir;
  struct stat obj;
  struct dirent *entry;
  uint8_t aux = 0;

  //TODO: add more options
  //printf_P(PSTR("args: %d\n"), argc);
  if (argv[1] == NULL)
    aux = opendir("/", &dir);
  else
  {
    if (stat(argv[1], &obj))
      return 1;
      // TODO: list files alone not only entire dirs
    /*if ((obj.st_mode & FS_MASK_FILETYPE) == FS_FILE)
      ls_print_entry(,NULL);
    else*/
      aux = opendir(argv[1], &dir);
  }

  ls_print_entry(&(dir.dd_ent),".");

  while ((entry = readdir(&dir)) != NULL)
      ls_print_entry(entry, entry->d_name);

  //closedir(&dir);

  return aux;
}

// Print free ram
uint8_t main_free (char *argv[])
{
#ifdef __AVR__
  extern unsigned int __heap_start;
  extern void *__brkval;
  unsigned int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  int total = 0;

  printf_P(PSTR("total=%d data=%d heap=%d stack=%d free=%d\n"), RAMEND - RAMSTART, (int) &__heap_start - RAMSTART, HEAPEND - (int) &__heap_start, RAMEND - (int) &total, (int) &total - HEAPEND + total);  
#else
  printf("Free not implemented in this arch.\n");
#endif
  return 0;
}

uint8_t main_times (char *argv[])
{
  unsigned long t1;
  uint8_t i = 0;
  
  t1=millis();
  if (argv[1] != NULL)
  {
    //TODO: pass env to execve
    //FIXME: look like it's not passing correctly the parameters to the command
    i = execve((const char **) &argv[1], NULL);
  }
  printf_P(PSTR("millis: %ld\n"), millis() - t1);
  return i;
}

// list environment variables
uint8_t main_set(char *argv[], char *env[])
{
  uint8_t i = 0;

  while (i < ENV_MAX)
  {
    if (env[i] != NULL)
      printf_P(PSTR("%s\n"), env[i]);
    i++;
  }

  return 0;
}

uint8_t main_true()
{
  return 0;
}

uint8_t main_false()
{
  return 1;
}

uint8_t main_cat(char *argv[])
{
  FD fd;
  char c;
  
  if (argv[1] == NULL)
    return 0;
    
  if ((open(argv[1], 0, &fd)) != 0)
    return 1;

  while (read(&fd, &c, 1) != 0)
    printf_P(PSTR("%c"),c);

  return 0;
}

uint8_t main_echo(char *argv[])
{
  uint8_t i = 1;

  while (argv[i] != NULL)
  {
    printf_P(PSTR("%s"), argv[i]);
    if (argv[i + 1] != NULL)
      printf_P(PSTR(" "));
    i++;
  }
  printf_P(PSTR("\n"));
  
  return 0;
}

void debug_dump(uint16_t src, const void *ptr, size_t msize)
{
  size_t i, j;
  uint8_t *p, d;

  printf_P(PSTR("Size: 0x%x\n"), msize);
  for (i=0; i < msize; i+=16)
  {
    p = (uint8_t *) ptr + i;
    printf_P(PSTR("0x%x: "), src + i);
    for (j=0; j < 32; j++)
    {
      d = *(p + (j & 0xf));
      if (j < 16)
        printf_P(PSTR("%x%x "), d >> 4, d & 0x0F);
      else
        printf_P(PSTR("%c"), (d < 32) ? '.' : d);
    }
    printf_P(PSTR("\n"));
  }
}

uint8_t main_debug(char *argv[])
{

#ifdef __AVR__
  extern unsigned int __heap_start;
  extern void *__brkval;
  uint16_t marker;
  int msize = 0;
  unsigned int DATAEND =(int) &__heap_start;
  unsigned int DATASIZE = DATAEND - RAMSTART;
  unsigned int HEAPSTART = (int) &__heap_start;
  unsigned int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  uint8_t i = 1;
 
  if (argv[1] == NULL)
    printf_P(PSTR("Use -h to print help\n"));
  while (argv[i] != NULL > 0)
  {
    if (argv[i][0] != '-' || argv[i][2] != 0)
      goto error;

    switch(argv[i][1])
    {
      case 'h':
        printf_P(PSTR("Debug help:\n -d  dump data memory\n -s  print stack\n -H  dump heap\n"));
        break;
      case 'd':
        debug_dump((uint16_t) RAMSTART, (void*) RAMSTART, DATASIZE);
        break;
      case 's':
        msize = RAMEND - (uint16_t) &marker + 1;
        debug_dump((uint16_t) &marker, (void*) &marker, msize);
        break;
      case 'H':
        msize = HEAPEND - HEAPSTART;
        debug_dump((uint16_t) HEAPSTART, (void*) HEAPSTART, msize);
        break;
      default:
        goto error;
    }
    i++;
  }
  return 0;
error:
  printf_P(PSTR("Unknown param: \"%s\"\n"), argv[i]);
  return 1;
#else
  printf("debug not implemented in this arch.\n");
#endif
  return 0;
}

