#include "kernel.h"
#include "progfs.h"
#include "fs.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


int8_t execl_P(const PROGMEM char *argv, ...)
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
      ret = ENOMEM;
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

int8_t execl(const char *argv, ...)
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

int8_t exec(const char *argv[])
{
  return execve(argv, NULL);
}

// NOTE: force noinline in order to save stack memory
int8_t __attribute__((noinline)) analize_file(const char *argv[], long *inode, uint8_t *vfs_type)
{
  struct statvfs fs;
  struct stat file;
  char aux[PATH_MAX];
  FD fd;
  uint8_t i;

  if (argv == NULL || argv[0] == NULL)
    return EINVNAME; // filename invalid

  // If the filename looks like a full path, try to open it directly
  if (argv[0][0] == '/')
  {
    if (open(argv[0], 0, &fd) != 0)
      return ECANTOPEN; // could not open file
  }
  else
  // If not, try to prepend the PATH variable to the filename and try to open then 
  {
    snprintf_P(aux, PATH_MAX, PSTR(STR(PATH) "/%s"), argv[0]);
    if (open(aux, 0, &fd) != 0)
      return ENOTFOUND; // file not found
  }

  // and has the correct permissions 
  fstat(&fd, &file);
  if (!(file.st_mode & FS_EXEC))
    return EPERM; // bad permissions

  *inode = file.st_ino;

  // get the filesystem type of the filesystem of this file
  if (fstatvfs(&fd, &fs) != 0)
    return EINVFS; // kernel error, filesystem not found!?

  *vfs_type = fs.vfs_fstype;

  // Check if its a script
  read(&fd, aux, (uint8_t) PATH_MAX);
  if (aux[0] == '#' && aux[1] == '!')
  {
    // end the interpreter string with a 0
    for (i=2; i < PATH_MAX; i++)
    {
      if (aux[i] == ' ' || aux[i] == '\r' || aux[i] == '\n')
      {
          aux[i] = 0;
          break;
      }
    }
    // modify argv, so put first the interpreter, and second this script filename
    argv[1] = argv[0];
    argv[2] = NULL;
    argv[0] = malloc(i - 1);
    if (argv[0] == NULL)
      return ENOMEM; // not enought memory

    // copy the string to the new memory location
    memcpy((char *)argv[0], aux + 2, i - 1);

    return 1; // it's a script, call the interpreter
  }

  return 0;
  
}

int8_t execve(const char *argv[], char *envp[])
/* Executes a program (function) given in the first parameter of argv
 * 
 * Input: array with arguments NULL terminated, 
 *        array with environemnt variables NULL terminated
 * Returns: >=0 -> The return code of the program runned
 *          -1  -> filename invalid
 *          -2  -> could not open file
 *          -3  -> file not found
 *          -4  -> bad permissions
 *          -5  -> invalid filesystem
 *          -6  -> not enought memory
 */
{
  int8_t ret = 1;
  uint8_t vfs_type;
  long inode;

  // Check file permissions, and if is a script
  ret = analize_file(argv, &inode, &vfs_type);
  if (ret < 0)
    return ret;

  if (ret)
  {
    // call the interpreter
    ret = execve(argv, envp);
    free((char *)argv[0]);
    return ret;
  }

  // Try to execute dircetly ONLY if it's in PROGFS filesystem (internal flash)
  if (vfs_type == FS_TYPE_PROGFS)
  {
      ret = ((int8_t (*)(const char *argv[], char *envp[])) pgm_read_ptr(&(ProgFs2[inode].ptr)))(argv, envp);
      //return ((int8_t (*)(const char *argv[], char *envp[])) (ProgFs2[file.st_ino].ptr))(argv, envp);
  }
  
  return ret; // not a valid executable
}

