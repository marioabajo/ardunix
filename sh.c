#ifdef __AVR__
#include <avr/pgmspace.h>
#endif
#include "sh.h"
#include <string.h>

// list files and directories
u8 ls(u8 argc, char *argv[])
{
  DIR *dir;

  //TODO: add more options
  if (argc > 0)
    dir = opendir(argv[1]);
  else
    dir = opendir("/");

  if (dir == NULL)
    return -1;

  while (dir != NULL)
  {
    //TODO: print more attributes
    printf("%s\n", dir->filename);
    dir = readdir(dir);
  }
  argc--;

  return 0;
}

// Print free ram
u8 freeMem (u8 argc, char *argv[])
{
#ifdef __AVR__
  extern unsigned int __heap_start;
  extern void *__brkval;
  struct __freelist{
    size_t sz;
    struct __freelist *nx;
  } *current;
  extern struct __freelist *__flp;
  int free_memory;
  int total = 0;

  for (current = __flp; current; current = current->nx) {
    total += 2; /* Add two bytes for the memory block's header  */
    total += (int) current->sz;
  }
  
  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += total;
  }
  printf("Free: %d\n",free_memory);
  
  /*extern int __heap_start, *__brkval;
  int v;
  printf("Free: %d\n", (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));*/
#else
  printf("Free not implemented in this arch.\n");
#endif
}

// parse command
void parsecmd(u8 argc, char *argv[])
{
  // TODO: just one path at this moment
  const char *PATH="/bin";
  DIR *dir;

  dir = opendir(PATH);

  while (dir != NULL && strcmp(dir->filename, argv[0]))
    dir = readdir(dir);

  if (dir == NULL)
  {
#ifdef __AVR__
    printf_P(PSTR("%s: Command not found\n"),argv[0]);
#else
    printf("%s: Command not found\n",argv[0]);
#endif
    return;
  }

  // execute the command
  dir->data(argc-1, argv);
  return;
}

// get command from input
boolean getcmd(u8 buff[])
{
  u8 buffp=0;

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
#ifdef __AVR__
      fprintf_P(stderr,PSTR("ERROR: Line too long, %d limit reached\n"), ARGMAX);
#else
      fprintf(stderr,"ERROR: Line too long, %d limit reached\n", ARGMAX);
#endif
      return false;
    }
  }
  while (buff[buffp-1] != '\n' && buff[buffp-1] != '\r');

  // end the command with a \0
  buff[buffp] = 0;

  return true;
}

// Break command line into arguments separated by spaces
u8 splitcmd(u8 cmd[], char *args[])
{
  u8 cont=0, pos=0, ipos=0, par=0;

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
          fprintf(stderr,"ERROR: max number of args reached %d\n",NCARGS);
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

u8 sh(u8 argc, char *argv[])
{
  // input line buffer
  u8 line[ARGMAX];
  // array with pointers to arguments inside line buffer
  char *args[NCARGS];
  u8 argsnum=0;
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
    printf("# ");

    // Get command
    if (!getcmd(line))
      continue;

    // TODO: environment variable substituion step

    // Split command line into parameters
    if ((argsnum = splitcmd(line,args)) == 0)
      continue;

    // parse line
    parsecmd(argsnum, args);

    // debug: split command
    /*printf("total args: %d\n", argsnum);
    for (int i=0;i<argsnum;i++)
      printf("Arg %d: %s\n", i, args[i]);*/
    
  } while(1);
}


