/*
 * 
 */

#include "sh.h"
#include <stdio.h>

static FILE uart = {0} ;

// Init root entry
FS_ROOT(bin);
  FS_DIR_SUB(bin, 0x45, sh, dev);
    FS_FILE(sh,   5, (void *)sh, ls);
    FS_FILE(ls,   5, (void *)ls, free);
    FS_FILE(free, 5, (void *)freeMem, times);
    FS_FILE_LAST(times, 5, (void *)times);
  FS_DIR(dev, 0x45, etc);
  FS_DIR_LAST(etc, 0x45);
//  FS_DIR_SUB_LAST(etc, 0x45, motd);
//    FS_FILE_LAST(motd, 4, motd);
    
#ifdef __AVR__

static int uart_putchar (char c, FILE *stream)
{
    if (c == '\n')
      Serial.write('\r');
    Serial.write(c);
    return 0 ;
}

static int uart_getchar (FILE *stream)
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

    Serial.begin(9600);
    while (!Serial);
}

void loop()
#else
int main(void)
#endif
{
  printf_P(PSTR("Ardunix 0.1 (29/12/2015)\n"));

  #ifdef DEBUG
  env_test();
  #endif

  struct stat aux;
  const char *cmd[] = {"/bin/sh", NULL};

  //sh(0,NULL);
  execve(0, (char **)cmd, NULL);
  printf_P(PSTR("Init process exited, waiting 10 seconds to restart"));
  delay(10000);
}

