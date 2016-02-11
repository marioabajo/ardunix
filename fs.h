#ifndef FS_H
#define FS_H

#include "platform.h"

#define FILENAME_MAX 16

/* Structure of the filesystem in flash. Each entry is composed of one of this structure.

   / (root) the rest of directories are child of this
   | 
   |-> bin -> ls -> free ....
   |-> mnt
   |   |-> eeprom
   |...

   Is a list of this estructures in wich each entry could be a file (with a pointer to its data) or a
   directory with a pointer to its childs (a new list). To list everry element of a directory, just 
   keep reading the "next" entry until it is NULL.

   Flags:
                bits -> 87654321
                        |||||---
                        ||--- \___ Permissions: RWX
                        -- \______ Reserved
                         \________ File type (see below)

                Types            bits 87:
                   - File             00
                   - Directory        01
                   - Link             10
                   - Device           11

*/
typedef struct fsentry
{
  const char *filename;
  const uint8_t flags;
  union
  {
    const struct fsentry *child;
    const void *data;
  };
  const uint16_t size;
  const struct fsentry *next;
} DIR;

struct dirent
{
  char d_name[FILENAME_MAX];
  uint8_t flags;
  void *inode;
  uint16_t size;
};


struct stat 
{
  unsigned int st_ino;         /* inode number */
  uint8_t  st_mode;        /* protection */
  uint8_t  st_size;        /* total size, in bytes */
};

#define FS_FILE(fname, flags, func, next) \
  extern const DIR FS_##next; \
  const char text_##fname[] PROGMEM = #fname;\
  const DIR FS_##fname PROGMEM = {&text_##fname[0], flags, {.data = func}, 0, &FS_##next}

#define FS_FILE_LAST(fname, flags, func) \
  const char text_##fname[] PROGMEM = #fname; \
  const DIR FS_##fname PROGMEM = {&text_##fname[0], flags, {.data = func}, 0, NULL}

#define FS_DIR_SUB(fname, flags, child, next) \
  extern const DIR FS_##next; \
  extern const DIR FS_##child; \
  const char text_##fname[] PROGMEM = #fname;\
  const DIR FS_##fname PROGMEM = {&text_##fname[0], flags, {&FS_##child}, 0, &FS_##next}

#define FS_DIR(fname, flags, next) \
  extern const DIR FS_##next; \
  const char text_##fname[] PROGMEM = #fname;\
  const DIR FS_##fname PROGMEM = {&text_##fname[0], flags, {NULL}, 0, &FS_##next}

#define FS_DIR_SUB_LAST(fname, flags, child) \
  extern const DIR FS_##child; \
  const char text_##fname[] PROGMEM = #fname; \
  const DIR FS_##fname PROGMEM = {&text_##fname[0], flags, {&FS_##child}, 0, NULL}
  
#define FS_DIR_LAST(fname, flags) \
  const char PROGMEM text_##fname[] PROGMEM = #fname; \
  const DIR FS_##fname PROGMEM = {&text_##fname[0], flags, {NULL}, 0, NULL}

#define FS_ROOT(child) \
  extern const DIR PROGMEM FS_##child; \
  const char text_root[] PROGMEM = "/"; \
  const DIR FS_ROOT PROGMEM = {&text_root[0], 0x45, {&FS_##child}, 0, NULL}; \
  DIR *ProgFs = (DIR *)&FS_ROOT

extern DIR *ProgFs;

#ifdef __cplusplus
extern "C"{
#endif
  int stat(const char *pathname, struct stat *buf);
  // Open a directory for reading. 
  DIR *opendir(const char* path);
  struct dirent *readdir(DIR *dirp);
  DIR *fsentry_sublevel_find(const char *filename);
  struct fsentry *fsentry_onelevel_find(struct fsentry *parent, const char *name, uint8_t namesize);
  bool fsentry_add(const char *name, struct fsentry *parent, uint8_t flags, void *function_name);
#ifdef __cplusplus
}
#endif

#endif

