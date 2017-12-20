#ifndef PLATFORM_X86_H
#define PLATFORM_X86_H

#include <time.h>
#include <limits.h>

#ifdef __cplusplus
extern "C"{
#endif
  void delay(long ms);
  unsigned int pgm_read_word(const uint16_t *d);
  uint8_t pgm_read_byte(const uint8_t *d);
#ifdef __cplusplus
}
#endif

// TODO check out this memory variables
extern unsigned int etext, edata, end;
#define __heap_start end
#define __brkval edata
#define RAMSTART (size_t) &etext
#define RAMEND (size_t) &end

#define PROGFS_PLATFORM_PREPARE 
#define PROGFS_ENT_DEV 

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
#define puts_P puts
#define memcpy_P memcpy

#endif
