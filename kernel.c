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
  uint8_t argc;

  if (filename == NULL)
    return 1; // filename invalid

  //DEBUG: 
  //printf("%s \t stat: %d, mode:%d\n", filename, stat(filename, &file), file.st_mode);
  // check if the file exists and has the correct permissions
  // TODO: maybe its better to use open and implement fstat to get the stat struct
  //       because open is using stat inside so we are doing the same thing two times
  if (stat(filename, &file) || (file.st_mode & FS_MASK_FILETYPE) || !(file.st_mode & FS_EXEC))
    return 2; // file not found or inaccesible

  // get the filesystem type of the filesystem of this file
  if (statvfs(filename, &fs) != 0)
    return 3; // kernel error, filesystem not found!?

  // copy args
  if ((argc = filename_plus_args_null_terminated_to_argv_conv(filename, argv, argv2)) == 255)
    return 4; // Too many args

  // Check if its a script
  if ((open(filename, 0, &fd)) != 0)
    return 2;
  if (read(&fd, header, 2) == 2 && strncmp(header, "#!",2) == 0)
  {
    // TODO: call the interpreter
    printf_P(PSTR("ERROR: scripting not yet implemented\n"));
    return 0; // ok
  }

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

