#include "platform.h"

#ifdef __AVR__

#else

void delay(long ms)
{
  struct timespec d = {ms/1000, (ms%1000)*1000000};
  nanosleep(&d,NULL);
}

unsigned int pgm_read_word(const uint16_t *d)
{
  return *d;
}

#endif
