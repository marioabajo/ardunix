/*
 * 
 */

#include "sh.h"
#include <stdio.h>

static FILE uart = {0} ;

// Init root entry
PROGFS_ROOT(bin);
  PROGFS_DIR_SUB(bin, 0x45, sh, dev);
    PROGFS_FILE(sh,   5, (void *)sh, ls);
    PROGFS_FILE(ls,   5, (void *)ls, free);
    PROGFS_FILE(free, 5, (void *)freeMem, times);
    PROGFS_FILE_LAST(times, 5, (void *)times);
  PROGFS_DIR(dev, 0x45, etc);
  PROGFS_DIR_LAST(etc, 0x45);
//  PROGFS_DIR_SUB_LAST(etc, 0x45, motd);
//    PROGFS_FILE_LAST(motd, 4, motd);
    
#ifdef __AVR__

int uart_putchar (char c, FILE *stream)
{
    if (c == '\n')
      Serial.write('\r');
    Serial.write(c);
    return 0 ;
}

int uart_getchar (FILE *stream)
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
  printf_P(PSTR("Ardunix 0.2 (17/02/2016)\n"));

  #ifdef DEBUG
  env_test();
  #endif

  //const char *cmd[] = {"/bin/sh", NULL};

  sh(0,NULL);
  //execve(0, (char **)cmd, NULL);
  printf_P(PSTR("Init process exited, waiting 10 seconds to restart\n"));
  delay(10000);
}

