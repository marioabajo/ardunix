#include "kernel.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

uint8_t execve(uint8_t argc, char *argv[], struct dict_list **envp)
{
  struct stat file;
  uint8_t aux;
  char *filename;

  if (argv[0] == NULL)
    return -1;

  // absoulte path
  if (argv[0][0] == '/')
    filename = argv[0];

  // relative path or direct command, use the PATH
  else
  {
    if ((filename = (char *)malloc(PATH_MAX)) == NULL)
    {
      printf_P(PSTR("Not enought memory to execute command."));
      return -1;
    }
    strcpy(filename, PATH);
    aux = strlen(PATH);
    filename[aux] = '/';
    filename[aux+1] = 0;
    strncat(filename, argv[0], PATH_MAX - aux);

    // reuse num variable as a flag to free the memory reserved by filename
    aux = 255;
  }


  if (stat(filename,&file) != 0 || (file.st_mode & 0xc1) != 1)
  {
    printf_P(PSTR("%s: Command not found\n"), filename);
    return -1;
  }

  // filename variable is not used anymore so free it
  if (aux == 255)
    free(filename);
  aux = 0;

  // execute the command
  if (argc > 0)
    aux = argc-1;

  //TODO FIX, terrible workaround
  //FIX: open file, look at the header (if it's a shell script, launch the
  //     interpreter) and execute it.
  PFS *dir;
  dir = (PFS *)file.st_ino;
  

  //DEBUG
  //extern ls;
  //printf("DEBUG execve: %d %x %x %x %x %x\n", aux, file.st_mode, file.st_ino, dir->data, pgm_read_ptr(&(dir->data)), &(ls));
  return ((uint8_t (*)(uint8_t argc, char *argv[], struct dict_list **envp)) pgm_read_ptr(&(dir->data)))(aux, argv, envp);
}

