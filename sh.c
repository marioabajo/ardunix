#include "sh.h"
#include "env.h"
#include "defines.h"
#include "kernel.h"
#include <stdio.h>
#include <string.h>

// parse command
uint8_t parsecmd(uint8_t argc, char *argv[], struct dict_list **env)
{
  uint8_t carg = 0;  // Current argument being studied
  uint8_t *pos = 0;

  // Variable assingment
  while (argc >= 0 && ((pos = strchr(argv[carg], '=')) != NULL))
  {
    *pos = 0;
    env_add(env, argv[carg], ++pos);
    argc--;
    argv++;
  }

  // Run out of parameters? nothing more to do, bye bye
  if (argc == 0)
    return 0;

  // Check for reserved words
  // exit
  if (strncmp_P(argv[carg],PSTR("exit"),5) == 0)
  {
    // Do we have more parameters?
    switch (argc-carg)
    {
      // TODO: if the exit code is ommited, use the result of the last command
      case 1:
        return 1;
        break;
      // TODO: strtol
      case 2:
        return 1;
        break;
      default:
        fprintf_P(stderr,PSTR("exit: too many arguments\n"));
        return 0;
    }
  }

  // logout
  else if (strncmp_P(argv[carg],PSTR("logout"),7) == 0)
    return 1;
  else
    // execute the command
    execve(argc-carg, argv, env);
}

// get command from input
bool getcmd(char buff[])
{
  char buffp=0;

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
        // TODO: go back one position in screen
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
uint8_t splitcmd(char cmd[], char *args[])
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

uint8_t varexpansion(char line[], struct dict_list **env)
{
  struct dict_list e;
  uint8_t pos = 0, len, varlen, j;
  int8_t extralen;
  char *var, *i;

  len = strlen(line);

  while (pos < len)
  {
    switch (line[pos])
    {
      // Invalidate function of next character
      case '\\':
        pos++;
        break;
      // Parameter expansion
      case '$':
        //TODO: check for length
        pos++;
        // It's a variable name?
        if ((line[pos] >= 'a' && line[pos] <= 'z') ||
            (line[pos] >= 'A' && line[pos] <= 'Z'))
        {
          // calculate the end of the parameter name
          i = line + pos;
          while ((*i >= 'a' && *i <= 'z') ||
                 (*i >= 'A' && *i <= 'Z') ||
                 (*i >= '0' && *i <= '9') ||
                 (*i == '_'))
            i++;
          // now i points to the end of the parameter, this minus pos equals
          // to the lenght of the parameter name
          extralen = i - (line + pos);
          varlen = 0;
          // get the environment variable value
          if ((var = env_nget(*env, line + pos, extralen)) != NULL)
          {
            // get the lenght of the parameter value
            varlen = strlen(var);
            // calculate the difference betwen the length of the variable 
            // name and the length of the data stored in it
            extralen -= varlen - 1;
          }
          extralen = -extralen;
          // Now, maybe the value of the parameter is bigger that the parameter
          // name, so, to put it in the same line we have to shift the line
          // the number of characters in extralen
          // First, check the limits
          if (len + extralen > ARGMAX)
          {
            fprintf_P(stderr,PSTR("ERROR: Line too long, %d limit reached\n"), ARGMAX);
            return 0;
          }
          // adjust the length of the line (now is bigger)
          len += extralen;
          // shift the string the value of extralen (negative for right shift)
          j = len;
          while (j >= pos)
          {
            //printf("DEBUG: ini env = %x\n", *env);
            line[j] = line[j - extralen];
            //printf("DEBUG: fin env = %x\n", *env);
            j--;
          }
          // put value of the environment variable
          strncpy(line + pos -1, var, varlen);
          pos += varlen - 2;
        }

        break;
    }

    pos++;
  }
  return 1;
}

uint8_t sh(uint8_t argc, char *argv[], struct dict_list **env)
{
  // input line buffer
  uint8_t line[ARGMAX];
  // array with pointers to arguments inside line buffer
  char *args[NCARGS];
  uint8_t argsnum=0;
  uint8_t exit_flag=1;

  do
  {
    // show prompt
    printf_P(PSTR("# "));

    // Get command
    if (!getcmd(line))
      continue;

    // Parameter expansion and environment variables
    if (!varexpansion(line, env))
      continue;

    // Split command line into parameters
    if ((argsnum = splitcmd(line,args)) == 0)
      continue;

    // parse line
    switch (parsecmd(argsnum, args, env))
    {
      case 1:  // exit shell
        exit_flag=0;
        break;
    }

    // debug: split command
    /*printf("total args: %d\n", argsnum);
    for (int i=0;i<argsnum;i++)
      printf("Arg %d: %s\n", i, args[i]);*/
    
  // TODO: Implement a decent exit mechanism
  } while(exit_flag);

  return 0;
}


