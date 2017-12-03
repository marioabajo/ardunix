#include "kernel.h"
#include "progfs.h"
#include "fs.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


uint8_t execl_P(const PROGMEM char *argv, ...)
{
  char *arglist[NCARGS], *p, *str;
  uint8_t len, ret = 0;
  int8_t i = 0;
  va_list va;

  if(argv == NULL)
    return 0;

  p = (char *) argv;

  va_start(va, argv);

  do
  {
    len = strlen_P(p);
    if ((str = malloc(len + 1)) == NULL)
    {
      ret = 6;
      goto finish;
    }
    strncpy_P(str, p, len);
    str[len] = 0;
    arglist[i++] = str;    
  }while (i < NCARGS && (p = va_arg(va, char *)) != NULL);

  va_end(va);

  arglist[i] = NULL;

  ret = execve((const char **)arglist, NULL);

finish:
  // free the allocated blocks
  for (i--; i>=0; i--)
    free(arglist[i]);

  return ret;

}

uint8_t execl(const char *argv, ...)
{
  char *arglist[NCARGS], *p;
  uint8_t i = 0;
  va_list va;

  if(argv == NULL)
    return 0;

  arglist[i++] = (char *) argv;
  
  va_start(va, argv);

  while (i < NCARGS && (p = va_arg(va, char *)) != NULL)
    arglist[i++] = p;
    
  va_end(va);

  arglist[i] = NULL;
  
  return execve((const char **) arglist, NULL);
}

uint8_t exec(const char *argv[])
{
  return execve(argv, NULL);
}

// NOTE: force noinline in order to save stack memory
uint8_t __attribute__((noinline)) analize_file(const char *argv[], struct stat *file, struct statvfs *fs, uint8_t *script)
{
  char aux[PATH_MAX];
  FD fd;
  uint8_t i;

  if (argv == NULL || argv[0] == NULL)
    return 1; // filename invalid

  // If the filename looks like a full path, try to open it directly
  if (argv[0][0] == '/')
  {
    if (open(argv[0], 0, &fd) != 0)
      return 2; // could not open file
  }
  else
  // If not, try to prepend the PATH variable to the filename and try to open then 
  {
    snprintf_P(aux, PATH_MAX, PSTR(STR(PATH) "/%s"), argv[0]);
    if (open(aux, 0, &fd) != 0)
      return 3; // file not found
  }

  // and has the correct permissions 
  fstat(&fd, file);
  if (!(file->st_mode & FS_EXEC))
    return 4; // bad permissions

  // get the filesystem type of the filesystem of this file
  if (fstatvfs(&fd, fs) != 0)
    return 5; // kernel error, filesystem not found!?

  // Check if its a script
  read(&fd, aux, PATH_MAX);
  if (aux[0] == '#' && aux[1] == '!')
  {
    // modify argv, so put first the interpreter, and second this script filename
    argv[1] = argv[0];
    argv[2] = NULL;
    argv[0] = &aux[2];
    // end the interpreter string with a 0
    for (i=2; i < PATH_MAX; i++)
    {
      if (aux[i] == ' ' || aux[i] == '\r' || aux[i] == '\n')
      {
          aux[i] = 0;
          break;
      }
    }
    // it's a script, call the interpreter
    *script = 1;
    return 0; // ok
  }

  return 0;
  
}

uint8_t execve(const char *argv[], char *envp[])
{
  struct stat file;
  struct statvfs fs;
  uint8_t ret, script = 0;

  ret = analize_file(argv, &file, &fs, &script);
  if (ret)
    return ret;
  if (script)
    // call the interpreter
    return execve(argv, envp);

  // Try to execute dircetly ONLY if it's in PROGFS filesystem (internal flash)
  if (fs.vfs_fstype == FS_TYPE_PROGFS)
  {
      return ((uint8_t (*)(const char *argv[], char *envp[])) pgm_read_ptr(&(ProgFs2[file.st_ino].ptr)))(argv, envp);
      //return ((uint8_t (*)(const char *argv[], char *envp[])) (ProgFs2[file.st_ino].ptr))(argv, envp);
      //DEBUG: printf("debug: %s offset:%x\n", filename,pgm_read_ptr(&(file.st_ino)));
      //return 0; // ok
  }
  
  return 255; // not a valid executable
}

