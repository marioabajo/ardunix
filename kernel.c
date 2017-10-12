#include "kernel.h"
#include "progfs.h"
#include "support.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


uint8_t exec(const char *filename, const char *argv[])
{
    return execve(filename, argv, NULL);
}

uint8_t execve(const char *filename, const char *argv[], char *envp[])
{
  struct stat file;
  struct statvfs fs;
  FD fd;
  char *argv2[NCARGS];
  char header[3];
  uint8_t argc = 0;
  char *fullfilename;

  if (filename == NULL)
    return 1; // filename invalid

  // check if the file exists
  if (filename[0] == '/' && open(filename, 0, &fd) == 0)
    goto check;
  // TODO: any method to avoid malloc? and a static memory allocation waste?
  if ((fullfilename = malloc(PATH_MAX)) == NULL)
    return 6; // not enought memory
  snprintf_P(fullfilename, PATH_MAX, PSTR("%s/%s"), PATH, filename);
  if (open(fullfilename, 0, &fd) == 0)
  {
    free(fullfilename);
    goto check;
  }
  free(fullfilename);
  return 2; // file not found

check:
  // and has the correct permissions 
  fstat(&fd, &file);
  if (!(file.st_mode & FS_EXEC))
    return 3; // bad permissions

  // get the filesystem type of the filesystem of this file
  if (fstatvfs(&fd, &fs) != 0)
    return 4; // kernel error, filesystem not found!?

  // Check if its a script
  if (read(&fd, header, 2) == 2 && strncmp_P(header, PSTR("#!"),2) == 0)
  {
    // TODO: call the interpreter
    printf_P(PSTR("ERROR: not implemented\n"));
    return 0; // ok
  }

  // copy args
  if ((argc = filename_plus_args_null_terminated_to_argv_conv(filename, argv, argv2)) == 255)
    return 5; // Too many args

  // Try to execute dircetly ONLY if it's in PROGFS filesystem (internal flash)
  if (fs.vfs_fstype == FS_TYPE_PROGFS)
  {
      //return ((uint8_t (*)(uint8_t argc, char *argv[], char *envp[])) pgm_read_ptr(&(ProgFs2[file.st_ino].ptr)))(argc, argv2, envp);
      return ((uint8_t (*)(uint8_t argc, char *argv[], char *envp[])) (ProgFs2[file.st_ino].ptr))(argc, argv2, envp);
      //DEBUG: printf("debug: %s offset:%x\n", filename,pgm_read_ptr(&(file.st_ino)));
      //return 0; // ok
  }
  
  return 255; // not a valid executable
}

