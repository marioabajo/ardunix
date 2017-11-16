#ifndef PLATFORM_H
#define PLATFORM_H

/* Common
 *
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "config.h"

/*
 *  Arduino
 */

#ifdef __AVR__
#include <avr/pgmspace.h>

  #ifdef __cplusplus
  extern "C"{
  #endif
  
  #ifdef __cplusplus
  }
  #endif

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
  uint8_t pgm_read_byte(const uint8_t *d);
  #ifdef __cplusplus
  }
  #endif
  
  #define millis() (long) time(NULL)
  #define pgm_read_ptr(x) *(x)
  #define PROGMEM
  #define PSTR(s) (s)
  #define strlen_P strlen
  #define strcpy_P strcpy
  #define strncpy_P strncpy
  #define strcmp_P strcmp
  #define strncmp_P strncmp
  #define fprintf_P fprintf
  #define printf_P printf
  #define snprintf_P snprintf
#endif

#endif
