/*
 * 
 */

#include "ardunix.h"

//#define DEBUG 1

#ifdef __AVR__

static FILE uart = {0} ;

int uart_putchar (char c)
{
    if (c == '\n')
      Serial.write('\r');
    Serial.write(c);
    return 0 ;
}

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
    printf_P(PSTR("Booting...\n"));
}

void loop()
#else
int main(void)
#endif
{
  // Show welcome message
  exec("/bin/cat", "/etc/issue");

#ifdef DEBUG
  printf("execve: %d\n", exec("bin"));
  printf("execve: %d\n", exec("/bin"));
  printf("execve: %d\n", exec("/bin/ls"));
  printf("execve: %d\n", exec("/bin/lso"));
  printf("execve: %d\n", exec("/etc/motd"));
  printf("execve: %d\n", exec("/etc/script"));  
#endif

  exec("/bin/sh");
  //main_sh(NULL, NULL);
  
  printf_P(PSTR("Init process exited, waiting 10 seconds to restart\n"));
  delay(10000);
}
  
