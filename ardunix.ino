/*
 * 
 */

#include "kernel.h"
#include "progfs.h"
#include "sh.h"
#include "coreutils.h"
#include <stdio.h>

//#define DEBUG 1

const char PROGMEM compile_date[] = __DATE__ " " __TIME__;

static FILE uart = {0} ;

// file system definition
const char PROGMEM motd[] = "ardunix test v0.4\n";
const char PROGMEM script[] = "#!/bin/sh\nls\n";

const PFS2 ProgFs2[] = \
{{"/\0", NULL, FS_DIR | FS_EXEC | FS_READ, 0},\
  {"bin", NULL, FS_DIR | FS_EXEC | FS_READ, 0},\
    {"ls", (void *)main_ls, FS_EXEC | FS_READ, 0},\
    {"sh", (void *)main_sh, FS_EXEC | FS_READ, 0},\
    {"free", (void *)main_free, FS_EXEC | FS_READ, 0},\
    {"times", (void *)main_times, FS_EXEC | FS_READ, 0},\
    {"set", (void *)main_set, FS_EXEC | FS_READ, 0},\
    {"true", (void *)main_true, FS_EXEC | FS_READ, 0},\
    {"false", (void *)main_false, FS_EXEC | FS_READ, 0},\
  {0, 0, 0, 0},\
  {"etc", NULL, FS_DIR | FS_EXEC | FS_READ, 0},\
    {"motd", (void *)motd, FS_READ, sizeof(motd)},\
    {"test", NULL, FS_DIR | FS_EXEC | FS_READ, 0},\
      {"empty", NULL, FS_EXEC | FS_READ, 0},\
    {0, 0, 0, 0},\
    {"script", (void *)script, FS_EXEC | FS_READ, sizeof(script)},\
  {0, 0, 0, 0},\
{0, 0, 0, 0}};


#ifdef __AVR__
//int uart_putchar (char c, FILE *stream)
int uart_putchar (char c)
{
    if (c == '\n')
      Serial.write('\r');
    Serial.write(c);
    return 0 ;
}

//int uart_getchar (FILE *stream)
int uart_getchar ()
{
  char a;

  while(Serial.available()==0);
  a=Serial.read();
  if (a == '\r')
    Serial.write("\n");
  Serial.write(a);
  return a;
}

void setup()
{
    fdev_setup_stream (&uart, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdin = stdout = stderr = &uart;

    Serial.begin(115200);
    //while (!Serial);
    printf_P(PSTR("Booting..."));
}

void loop()
#else
int main(void)
#endif
{
  //struct dict_list *env=NULL;

  printf_P(PSTR("Ardunix 0.3 beta (15/04/2017)\n"));

  /*#ifdef DEBUG
  env_test();
  #endif*/

#ifdef DEBUG
  printf("execve: %d\n", exec("bin", NULL));
  printf("execve: %d\n", exec("/bin", NULL));
  printf("execve: %d\n", exec("/bin/ls", NULL));
  printf("execve: %d\n", exec("/bin/lso", NULL));
  printf("execve: %d\n", exec("/etc/motd", NULL));
  printf("execve: %d\n", exec("/etc/script", NULL));  
#endif
  
  printf("execve: %d\n", exec("/bin/sh", NULL));
  //main_sh(0, NULL, NULL);
  
  printf_P(PSTR("Init process exited, waiting 10 seconds to restart\n"));
  delay(10000);
}
  
