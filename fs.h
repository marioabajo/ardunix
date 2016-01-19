#ifndef FS_H
#define FS_H

#include "platform.h"

struct fsentry
{
  char *filename;
  union
  {
    struct fsentry *child;
    uint8_t (*data)(uint8_t, char**); // Points to function in memory
  };
  uint8_t flags;  /* bits -> 87654321
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
  struct fsentry *next;
};

#define DIR struct fsentry

#ifdef __cplusplus
extern "C"{
#endif
  // Open a directory for reading. 
  DIR *opendir(const char* path);
  DIR *readdir(DIR *dirp);
  struct fsentry *fsentry_onelevel_find(struct fsentry *parent, const char *name, uint8_t namesize);
  bool fsentry_add(const char *name, struct fsentry *parent, uint8_t flags, void *function_name);
#ifdef __cplusplus
}
#endif

#define FS_FILE(fname, func, flags, next) DIR F_ ## fname={(char *)#fname, {.data = func}, flags, next}
#define FS_DIR(dname, child, flags, next) DIR D_ ## dname={(char *)#dname, {child}, flags, next}

extern struct fsentry fs;

#endif

