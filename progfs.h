#ifndef PROGFS_H
#define PROGFS_H

#include "platform.h"
#include "fs.h"

typedef struct progfsentry
{
  const char *filename;
  const uint8_t flags;
  union
  {
    const struct progfsentry *child;
    const void *data;
  };
  const uint16_t size;
  const struct progfsentry *next;
} PFS;

#define PROGFS_FILE(fname, flags, func, next) \
  extern const PFS PROGFS_##next; \
  const char text_##fname[] PROGMEM = #fname;\
  const PFS PROGFS_##fname PROGMEM = {&text_##fname[0], flags, {.data = func}, 0, &PROGFS_##next}

#define PROGFS_FILE_LAST(fname, flags, func) \
  const char text_##fname[] PROGMEM = #fname; \
  const PFS PROGFS_##fname PROGMEM = {&text_##fname[0], flags, {.data = func}, 0, NULL}

#define PROGFS_DIR_SUB(fname, flags, child, next) \
  extern const PFS PROGFS_##next; \
  extern const PFS PROGFS_##child; \
  const char text_##fname[] PROGMEM = #fname;\
  const PFS PROGFS_##fname PROGMEM = {&text_##fname[0], flags, {&PROGFS_##child}, 0, &PROGFS_##next}

#define PROGFS_DIR(fname, flags, next) \
  extern const PFS PROGFS_##next; \
  const char text_##fname[] PROGMEM = #fname;\
  const PFS PROGFS_##fname PROGMEM = {&text_##fname[0], flags, {NULL}, 0, &PROGFS_##next}

#define PROGFS_DIR_SUB_LAST(fname, flags, child) \
  extern const PFS PROGFS_##child; \
  const char text_##fname[] PROGMEM = #fname; \
  const PFS PROGFS_##fname PROGMEM = {&text_##fname[0], flags, {&PROGFS_##child}, 0, NULL}
  
#define PROGFS_DIR_LAST(fname, flags) \
  const char PROGMEM text_##fname[] PROGMEM = #fname; \
  const PFS PROGFS_##fname PROGMEM = {&text_##fname[0], flags, {NULL}, 0, NULL}

#define PROGFS_ROOT(child) \
  extern const PFS PROGMEM PROGFS_##child; \
  const char text_root[] PROGMEM = "/"; \
  const PFS PROGFS_ROOT PROGMEM = {&text_root[0], 0x45, {&PROGFS_##child}, 0, NULL}; \
  PFS *ProgFs = (PFS *)&PROGFS_ROOT


extern PFS *ProgFs;

#ifdef __cplusplus
extern "C" {
#endif
  int progfs_stat(const char *pathname, struct stat *buf);
  DIR *progfs_opendir(const char *path);
#ifdef __cplusplus
}
#endif

#endif
