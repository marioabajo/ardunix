/*
 * 
 */

#include "sh.h"

static FILE uart = {0} ;


#define fsentries 3
const struct fsentry fs[fsentries]={
  // root
	{NULL,"/",NULL,0x51},
 
  // first level directories
	{&fs[0],"bin",NULL,0x51},

  // files
  {&fs[1], "ls", (const void (*)(u8, char**)) ls, 0x50}
};

// list files and directories
u8 ls(u8 argc, char *argv[])
{
	u16 i;
	for (i=0;i<fsentries;i++)
	{
    printf("%s\n",fs[i].filename);
	}
	return 0;
}

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
  char a=Serial.read();
  if (a == '\r')
    Serial.write("\n");
  Serial.write(a);
  return a;
}

void setup()
{
    fdev_setup_stream (&uart, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdin = stdout = &uart;
    
    Serial.begin(9600);
    while (!Serial);
    printf("Ardunix 0.1 (29/12/2015)\n");
}

void loop()
{
	sh(0,NULL);
}

#else

int main(void)
{
	sh(0,NULL);
}
#endif

