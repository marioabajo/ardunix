#include "coreutils.h"
#include "kernel.h"
#include "fs.h"
#include <stdio.h>
#include <string.h>
#ifdef __AVR__
#include <Arduino.h>
#endif

void ls_print_entry(uint8_t f, uint16_t size, const char *entry_name)
{
  char c = '?';
  char r = '-', w = '-', x = '-';

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

  if (f & FS_READ)
    r = 'r';
  if (f & FS_WRITE)
    w = 'w';
  if (f & FS_EXEC)
    x = 'x';
  printf_P(PSTR("%c%c%c%c %4d %s\n"), c, r, w, x, size, entry_name);
}

// list files and directories
int8_t main_ls(char *argv[])
{
  DIR dir;
  char *fn = HOME;
  struct stat obj;
  struct dirent *entry;
  int8_t ret = 0;
  uint8_t i = 1;
  bool _exit = 0;

  // TODO: initialize fn with CWD env variable
  while (! _exit)
  {
    if (argv[i] != NULL)
      fn = argv[i];
    else
    {
      _exit = 1;
      if (i > 1)
        break;
    }

    if (stat(fn, &obj))
      ret = 1;
    else
    {
      if (obj.st_mode & FS_DIR)
      {
        if (opendir(fn, &dir))
          ret = 1;
        else
        {
          ls_print_entry(dir.dd_ent.flags, dir.dd_ent.size ,".");
          while ((entry = readdir(&dir)) != NULL)
            ls_print_entry(entry->flags, entry->size, entry->d_name);
        }
      }
      else
        ls_print_entry(obj.st_mode, obj.st_size, fn);
    }
          
    i++;
  }

  return ret;
}

/*char * realpath(char *path, char *cwd)
{
  char *tmp;
  uint8_t i = 0, j = 0;

  if ((tmp = malloc(PATH_MAX)) == NULL)
    return NULL; // not enought memory

  snprintf_P(PATH_MAX, PSTR("%s/%s"), cwd, path);
  
  // TODO TODO
  
  return NULL;
}*/

int8_t main_cd(char *argv[], char *env[])
{
/*  char *cwd, *named = NULL;
  struct stat file;
  DIR dir;
  uint8_t ret = 0;

  if (env == NULL)
    return 1; // No environment defined

  // go to home
  if (argv[1] == NULL)
  {
    env_add(env, "CWD", HOME);
    return 0;
  }
  // absolute path
  else if (argv[1][0] == '/')
    named = realpath(argv[1], NULL);
  // relative path
  else
    if ((named = realpath(argv[1], env_get(env, "CWD"))) == NULL)
      return 5; // invalid path

  // check permissions and type
  if (stat(named, &file))
    ret = 2; // directory not found
  if (!(file.st_mode & FS_EXEC))
    ret = 3; // permission denied
  if (!(file.st_mode & FS_DIR))
    ret = 4; // not a directory
  
  if (!ret)
    env_add(env, "CWD", named);
  if (named != argv[1])
    free(named);
  return ret;*/
  return 0;
}

// Print free ram
int8_t main_free (char *argv[])
{
#ifdef __AVR__
  extern unsigned int __heap_start;
  extern void *__brkval;
  unsigned int HEAPEND = (__brkval == 0 ? (unsigned int) &__heap_start : (unsigned int) __brkval);
  int total = 0;

  printf_P(PSTR("total=%d data=%d heap=%d stack=%d free=%d\n"), RAMEND - RAMSTART, (unsigned int) &__heap_start - RAMSTART, 
           HEAPEND - (unsigned int) &__heap_start, RAMEND - (unsigned int) &total, (unsigned int) &total - HEAPEND + total);
#else
  printf("Free not implemented in this arch.\n");
#endif
  return 0;
}

int8_t main_times (char *argv[])
{
  unsigned long t1;
  int8_t i = 0;
  
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
int8_t main_set(char *argv[], char *env[])
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

int8_t main_true()
{
  return 0;
}

int8_t main_false()
{
  return 1;
}

int8_t main_cat(char *argv[])
{
  FD fd;
  char c;
  
  if (argv[1] == NULL)
    return 0;
    
  if ((open(argv[1], 0, &fd)) != 0)
    return 1;

  while (read(&fd, &c, 1) != 0)
    putchar(c);

  return 0;
}

int8_t main_echo(char *argv[])
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

int8_t main_debug(char *argv[])
{

#ifdef __AVR__
  uint16_t marker;
  int msize = 0;
  extern unsigned int __heap_start;
  extern void *__brkval;
  unsigned int DATAEND =(int) &__heap_start;
  unsigned int DATASIZE = DATAEND - RAMSTART;
  unsigned int HEAPSTART = (int) &__heap_start;
  unsigned int HEAPEND = (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
  uint8_t i = 1;

  if (argv[i] == NULL)
    goto help;

  while (argv[i] != NULL)
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
        /* kind of backtrace, but doesn't work as expected 
        for (i=0; i<2; i++)
          printf_P(PSTR("0x%x\n"), __builtin_return_address(i));*/
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
help:
  printf_P(PSTR("Use -h to print help\n"));
  return 1;
#else
  printf("debug not implemented in this arch.\n");
#endif
  return 0;
}

