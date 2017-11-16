/*
 * 
 */

//#define CMD_SH_SIMPLE true

#include "ardunix.h"

/* ARDUNIX CONFIG PARAMETERS
 * ========================= */

//#define CMD_LS false
//#define CMD_CD false
//#define CMD_SH false
//#define CMD_FREE false
//#define CMD_TIMES false
//#define CMD_SET false
//#define CMD_TRUE false
//#define CMD_FALSE false
//#define CMD_CAT false
//#define CMD_ECHO false
//#define CMD_DEBUG false
//#define DEBUG 1

// Add files (and content) to internal filesystem
const char PROGMEM _etc_test[] = "test";
const char PROGMEM _etc_test_empty[] = "empty";
const char PROGMEM _etc_script[] = "script";
const char PROGMEM script[] = "#!/bin/sh\n\nif /bin/false\nthen\n  /bin/ls /bin\nelse\n  /bin/ls /etc\nfi\n";

#define PROGFS_ENT_ETC_EXTRAS \
 PROGFS_ENTRY(_etc_script, script, FS_EXEC | FS_READ, sizeof(script)) \
 PROGFS_ENTRY(_etc_test, NULL, FS_DIR | FS_EXEC | FS_READ, 0) \
 PROGFS_ENTRY(_etc_test_empty, NULL, FS_EXEC | FS_READ, 0) \
 PROGFS_ENTRY(0, 0, 0, 0)

#define PROGFS_ENT_BIN_EXTRAS
/* END ARDUNIX CONFIG */

// Init ardunix internal filesystem
DEF_PROGFS

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
  printf_P(PSTR("execve: %d\n"), execl("bin"));
  printf_P(PSTR("execve: %d\n"), execl("/bin"));
  printf_P(PSTR("execve: %d\n"), execl("/bin/ls"));
  printf_P(PSTR("execve: %d\n"), execl("/bin/lso"));
  printf_P(PSTR("execve: %d\n"), execl("/etc/motd"));
  printf_P(PSTR("execve: %d\n"), execl("/etc/script"));  
#endif

  execl("/bin/sh");
  //main_sh(NULL, NULL);
  
  printf_P(PSTR("Init process exited, waiting 10 seconds to restart\n"));
  delay(10000);
}
  
