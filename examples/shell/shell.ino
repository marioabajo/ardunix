#include "ardunix.h"

#define PROGFS_ENT_ETC_EXTRAS
#define PROGFS_ENT_BIN_EXTRAS
INIT_PROGFS

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  int8_t ret;
  
  INIT_ARDUNIX

  ret = execl("sh", 0);

  // Warning message in case the process end's and introduce a delay before respawn
  printf_P(PSTR("Init process exited(%d), waiting 10 seconds to restart\n"), ret);
  delay(10000);
}

