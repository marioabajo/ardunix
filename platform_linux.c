#include "platform.h"

//##if defined(__x86_64__) || defined(__i386__) || defined(__ARM__)
#if defined(__linux__)

uint8_t CONSOLE_ECHO=0;

void delay(long ms)
{
  struct timespec d = {ms/1000, (ms%1000)*1000000};
  nanosleep(&d,NULL);
}

unsigned int pgm_read_word(const uint16_t *d)
{
  return *d;
}

uint8_t pgm_read_byte(const uint8_t *d)
{
  return *d;
}

#endif
