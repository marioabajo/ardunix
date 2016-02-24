#ifndef PLATFORM_H
#define PLATFORM_H

/* Common
 *
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "defines.h"

/*
 *  Arduino
 */

#ifdef __AVR__
#include <avr/pgmspace.h>

/*
 * PC
 */
#else
  #include <time.h>

  #ifdef __cplusplus
  extern "C"{
  #endif
  void delay(long ms);
  unsigned int pgm_read_word(const uint16_t *d);
  #ifdef __cplusplus
  }
  #endif
  #define millis() (long) time(NULL)
  #define pgm_read_byte(x) *(x)
  #define pgm_read_ptr(x) *(x)
  #define PROGMEM
  #define PSTR(s) (s)
  #define strcpy_P strcpy
  #define strncpy_P strncpy
  #define strncmp_P strncmp
  #define fprintf_P fprintf
  #define printf_P printf
#endif

#endif
