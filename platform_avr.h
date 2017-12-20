#ifndef PLATFORM_AVR_H
#define PLATFORM_AVR_H

#include <Arduino.h>

extern unsigned int __heap_start;
extern void *__brkval;

#define PROGFS_PLATFORM_PREPARE \
  const char PROGMEM _dev_gpio0[] = "0"; \
  const char PROGMEM _dev_gpio1[] = "1"; \
  const char PROGMEM _dev_gpio2[] = "2"; \
  const char PROGMEM _dev_gpio3[] = "3"; \
  const char PROGMEM _dev_gpio4[] = "4"; \
  const char PROGMEM _dev_gpio5[] = "5"; \
  const char PROGMEM _dev_gpio6[] = "6"; \
  const char PROGMEM _dev_gpio7[] = "7"; \
  const char PROGMEM _dev_gpio8[] = "8"; \
  const char PROGMEM _dev_gpio9[] = "9"; \
  const char PROGMEM _dev_gpio10[] = "10"; \
  const char PROGMEM _dev_gpio11[] = "11"; \
  const char PROGMEM _dev_gpio12[] = "12"; \
  const char PROGMEM _dev_gpio13[] = "13"; \
  const char PROGMEM _dev_gpioA0[] = "A0"; \
  const char PROGMEM _dev_gpioA1[] = "A1"; \
  const char PROGMEM _dev_gpioA2[] = "A2"; \
  const char PROGMEM _dev_gpioA3[] = "A3"; \
  const char PROGMEM _dev_gpioA4[] = "A4"; \
  const char PROGMEM _dev_gpioA5[] = "A5"; \
  const char PROGMEM _dev_gpioA6[] = "A6"; \
  const char PROGMEM _dev_gpioA7[] = "A7";
  
#define PROGFS_ENT_DEV \
    PROGFS_ENTRY(_dev_gpio0, 0, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio1, 1, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio2, 2, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio3, 3, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio4, 4, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio5, 5, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio6, 6, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio7, 7, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio8, 8, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio9, 9, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio10, 10, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio11, 11, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio12, 12, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpio13, 13, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA0, A0, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA1, 21, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA2, 22, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA3, 23, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA4, 24, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA5, 25, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA6, 26, FS_DEV | FS_READ | FS_WRITE, 0) \
    PROGFS_ENTRY(_dev_gpioA7, 27, FS_DEV | FS_READ | FS_WRITE, 0)

#endif

