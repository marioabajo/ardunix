/*
 * 
 */

#include "sh.h"

static FILE uart = {0} ;

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
	//DIR D_bin = {(char *)"bin", NULL, 0x85, NULL};
	//fs.child = &D_bin;

	//DIR D_dev = {(char *)"dev", NULL, 0x85, NULL};
	//D_bin.next = &D_dev;

	//DIR F_ls = {(char *)"ls", NULL, 0x5, NULL};
	//D_bin.child = &F_ls;
	//F_ls.data = &ls;

	//DIR F_free = {(char *)"free", NULL, 0x5, NULL};
	//F_ls.next = &F_free;
	//F_free.data = freeMem;

  //#define FS_FILE(n, command, flags, next) DIR F_(n)={(char *)n, {.data = command}, flags, next};

  //FS_FILE("free", freeMem, 0x5, NULL);
  DIR F_free = {(char *)"free", {.data = freeMem}, 0x5, NULL};
  DIR F_ls = {(char *)"ls", {.data = ls}, 0x5, &F_free};
  DIR D_dev = {(char *)"dev", NULL, 0x85, NULL};
  DIR D_bin = {(char *)"bin", {&F_ls}, 0x85, &D_dev};
  
	//fsentry_add("bin",&fs,0x85,NULL);
	//fsentry_add("dev",&fs,0x89,NULL);
	sh(0,NULL);
}

