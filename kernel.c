#include "kernel.h"
#include <stdio.h>
#include <string.h>

uint8_t execve(uint8_t argc, char *argv[], char *envp[])
{
  // TODO: just one path at this moment
  const char *PATH="/bin";
  struct stat file;
  uint8_t num=0;

  if (argv[0] == NULL)
    return -1;

  // absoulte path
  if (stat(argv[0],&file) != 0 || !file.st_mode & 0x1)
  {
    printf_P(PSTR("%s: Command not found\n"),argv[0]);
    return -1;
  }

  // execute the command
  if (argc > 0)
    num = argc-1;

  //TODO FIX, terrible workaround
  PFS *dir;
  dir = (PFS *)file.st_ino;

  //DEBUG
  //extern ls;
  //printf("DEBUG execve: %d %x %x %x %x %x\n", num, file.st_mode, file.st_ino, dir->data, pgm_read_ptr(&(dir->data)), &(ls));
  return ((uint8_t (*)(uint8_t argc, char *argv[])) pgm_read_ptr(&(dir->data)))(num, argv);
}

