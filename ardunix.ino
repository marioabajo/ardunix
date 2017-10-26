/*
 * 
 */
 
//#define CMD_LS false
//#define CMD_SH false
//#define CMD_FREE false
//#define CMD_TIMES false
//#define CMD_SET false
//#define CMD_TRUE false
//#define CMD_FALSE false
//#define CMD_CAT false
//#define CMD_ECHO false
//#define CMD_DEBUG false

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
  execl("/bin/cat", "/etc/issue");

#ifdef DEBUG
  printf("execve: %d\n", execl("bin"));
  printf("execve: %d\n", execl("/bin"));
  printf("execve: %d\n", execl("/bin/ls"));
  printf("execve: %d\n", execl("/bin/lso"));
  printf("execve: %d\n", execl("/etc/motd"));
  printf("execve: %d\n", execl("/etc/script"));  
#endif

  execl("/bin/sh");
  //main_sh(NULL, NULL);
  
  printf_P(PSTR("Init process exited, waiting 10 seconds to restart\n"));
  delay(10000);
}
  
