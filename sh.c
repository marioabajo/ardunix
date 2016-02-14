#include "sh.h"
#include "env.h"
#include "defines.h"
#include "kernel.h"
#include <stdio.h>
#include <string.h>

// parse command
uint8_t parsecmd(uint8_t argc, char *argv[])
{
  // TODO: check if the line is a reserved word, variable assigment, etc...
  // TODO: pass environment

  // Check for reserved words
  if (strncmp_P(argv[0],PSTR("exit"),5) == 0)
    // exit
    return 1;
  else
    // execute the command
    execve(argc, argv, NULL);
}

// get command from input
bool getcmd(uint8_t buff[])
{
  uint8_t buffp=0;

  do
  {
    // read input
    buff[buffp]=getchar();
    //read(0, buff+buffp, 1);

    // treat special charaters
    switch(buff[buffp])
    {
      // backspace
      case 0x08:
        // TODO: go back une position in screen
        buff[buffp]=0;
        break;
      // horizontal tab
      case 0x09:
        // TODO
        break;
      // Rest of characters
      default:
        // increment index
        buffp++;
    }

    // Check the buffer used
    if (buffp == ARGMAX)
    {
      fprintf_P(stderr,PSTR("ERROR: Line too long, %d limit reached\n"), ARGMAX);
      return false;
    }
  }
  while (buff[buffp-1] != '\n' && buff[buffp-1] != '\r');

  // end the command with a \0
  buff[buffp] = 0;

  return true;
}

// Break command line into arguments separated by spaces
uint8_t splitcmd(uint8_t cmd[], char *args[])
{
  uint8_t cont=0, pos=0, ipos=0, par=0;

  while(cmd[pos] != 0)
  {
    switch (cmd[pos])
    {
      case '\r':
      case '\n':
        cmd[pos] = ' ';
        break;
      case '\"':
        if (par == 0)
          par = 1;
        else if (par == 1)
          par = 0;
        break;
      case '\'':
        if (par == 0)
          par = 2;
        else if (par == 2)
          par = 0;
        break;
    }
    if (cmd[pos] == IFS && par == 0)
    {
      // we got a new argument
      if (pos != ipos)
      {
        args[cont++] = (char *) &cmd[ipos];
        // check NCARGS
        if (cont == NCARGS)
        {
          fprintf_P(stderr,PSTR("ERROR: max number of args reached %d\n"),NCARGS);
          return 0;
        }
        ipos=pos+1;
        // end string with a \0
        cmd[pos]=0;
      }
      // trim trailling spaces
      else
        ipos++;
    }
    pos++;
  }
  if (pos != ipos)
    args[cont++] = (char *) &cmd[ipos];

  return cont;
}

uint8_t sh(uint8_t argc, char *argv[])
{
  // input line buffer
  uint8_t line[ARGMAX];
  // array with pointers to arguments inside line buffer
  char *args[NCARGS];
  uint8_t argsnum=0;
  uint8_t exit_flag=1;
  struct dict_list *env=NULL;

  // debug: environment functions
  /*printf("env: C?  %s\n",env_get(env,"C"));
  printf("env: del B  (%d)\n", env_del(&env,"B"));
  printf("env: A = 1  (%d)\n", env_add(&env,"A","1"));
  printf("env: A?  %s\n",env_get(env,"A"));
  printf("env: HOME = aSd  (%d)\n", env_add(&env,"HOME","aSd"));
  printf("env: B = 23  (%d)\n", env_add(&env,"B","23"));
  printf("env: A = 4  (%d)\n", env_add(&env,"A","4"));
  printf("env: A?  %s\n",env_get(env,"A"));
  printf("env: del A  (%d)\n", env_del(&env,"A"));
  printf("env: A?  %s\n",env_get(env,"A"));
  printf("env: B?  %s\n",env_get(env,"B"));
  printf("env: HOME?  %s\n",env_get(env,"HOME"));*/

  do
  {
    // show prompt
    printf_P(PSTR("# "));

    // Get command
    if (!getcmd(line))
      continue;

    // TODO: environment variable substituion step

    // Split command line into parameters
    if ((argsnum = splitcmd(line,args)) == 0)
      continue;

    // parse line
    switch (parsecmd(argsnum, args))
    {
      case 1:  // exit shell
        exit_flag=0;
        break;
    }

    // debug: split command
    /*printf("total args: %d\n", argsnum);
    for (int i=0;i<argsnum;i++)
      printf("Arg %d: %s\n", i, args[i]);*/
    
  // TODO: Implement an exit mechanism
  } while(exit_flag);

  return 0;
}


