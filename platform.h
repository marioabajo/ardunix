#ifndef PLATFORM_H
#define PLATFORM_H

/* Common
 *
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>


/*
 *  Arduino
 */

#ifdef __AVR__
#include <avr/pgmspace.h>

/*
 * PC
 */
#else
  #define PSTR(s) (s)
  #define fprintf_P fprintf
  #define printf_P printf
#endif

#endif
