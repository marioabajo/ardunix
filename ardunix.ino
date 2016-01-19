/*
 * 
 */

#include "sh.h"
#include <stdio.h>

static FILE uart = {0} ;

// Init root entry
FS_FILE(free, freeMem, 0x5, NULL);
FS_FILE(ls, ls, 0x5, &F_free);  
FS_DIR(dev, NULL, 0x85, NULL);
FS_DIR(bin, &F_ls, 0x85, &D_dev);
struct fsentry fs = {(char *)"/",&D_bin,0x85,NULL};

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
    printf_P(PSTR("Ardunix 0.1 (29/12/2015)\n"));
}

void loop()
#else
int main(void)
#endif
{
	sh(0,NULL);
}

