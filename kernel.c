#include "kernel.h"
#include "progfs.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


/*uint8_t execle(const char *argv, ...)
{
  char *arglist[NCARGS], *p;
  char **env = NULL;
  uint8_t i = 0;
  va_list va;

  if(argv == NULL)
    return 0;

  arglist[i++] = (char *) argv;
  
  va_start(va, argv);

  if (i < NCARGS)
  {
    p = va_arg(va, char *);
    if (*p != 0)
      arglist[i++] = p;
  }
  else if (i == NCARGS)
    env = va_arg(va, char **);
    
  va_end(va);

  // if envp is NULL then, the last recorded value in arglist in the env
  if (env == NULL)
    env = (char **) arglist[--i];

  arglist[i] = NULL;
  
  return execve((const char **) arglist, env);
}*/

uint8_t execl(const char *argv, ...)
{
  char *arglist[NCARGS], *p;
  uint8_t i = 0;
  va_list va;

  if(argv == NULL)
    return 0;

  arglist[i++] = (char *) argv;
  
  va_start(va, argv);

  if (i < NCARGS)
  {
    p = va_arg(va, char *);
    if (*p != 0)
      arglist[i++] = p;
  }
    
  va_end(va);

  arglist[i] = NULL;
  
  return execve((const char **) arglist, NULL);
}

uint8_t exec(const char *argv[])
{
  return execve(argv, NULL);
}

uint8_t execve(const char *argv[], char *envp[])
{
  struct stat file;
  struct statvfs fs;
  FD fd;
  char header[3] = {0, 0, 0};
  char *fullfilename;

  if (argv == NULL || argv[0] == NULL)
    return 1; // filename invalid

  fullfilename = (char *) argv[0];

  // If the filename looks like a full path, try to open it directly
  if (fullfilename[0] == '/')
  {
    if (open(fullfilename, 0, &fd) != 0)
      return 2; // could not open file
  }
  else
  // If not, try to prepend the PATH variable to the filename and try to open then 
  {
    // TODO: any method to avoid malloc? and a static memory allocation waste?
    //       should we try strncat instead?
    if ((fullfilename = malloc(PATH_MAX)) == NULL)
      return 3; // not enought memory
    snprintf_P(fullfilename, PATH_MAX, PSTR("%s/%s"), PATH, argv[0]);
    if (open(fullfilename, 0, &fd) != 0)
    {
      free(fullfilename);
      return 4; // file not found
    }
    free(fullfilename);
  }

  // and has the correct permissions 
  fstat(&fd, &file);
  if (!(file.st_mode & FS_EXEC))
    return 5; // bad permissions

  // get the filesystem type of the filesystem of this file
  if (fstatvfs(&fd, &fs) != 0)
    return 6; // kernel error, filesystem not found!?

  // Check if its a script
  read(&fd, header, 2);
  if (header[0] == '#' && header[1] == '!')
  {
    // TODO: call the interpreter
    printf_P(PSTR("ERROR: not implemented\n"));
    return 0; // ok
  }

  // Try to execute dircetly ONLY if it's in PROGFS filesystem (internal flash)
  if (fs.vfs_fstype == FS_TYPE_PROGFS)
  {
      //return ((uint8_t (*)(uint8_t argc, char *argv[], char *envp[])) pgm_read_ptr(&(ProgFs2[file.st_ino].ptr)))(argc, argv2, envp);
      return ((uint8_t (*)(const char *argv[], char *envp[])) (ProgFs2[file.st_ino].ptr))(argv, envp);
      //DEBUG: printf("debug: %s offset:%x\n", filename,pgm_read_ptr(&(file.st_ino)));
      //return 0; // ok
  }
  
  return 255; // not a valid executable
}

