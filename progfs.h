#ifndef PROGFS_H
#define PROGFS_H

#include "platform.h"
#include "fs.h"

// The datastructure of the filesystem
typedef struct progfsentry {
    const char *name;
    void *ptr;
    uint8_t perm;
    uint16_t size;
} PFS2;


// Definition helpers for the progfs filesystem
#define PROGFS_ENTRY(PATHNAME, FUNC, FLAGS, SIZE) {PATHNAME, (void *)FUNC, FLAGS, SIZE},

// The global variable for the filesystem
extern const PFS2 PROGMEM ProgFs2[];

#ifdef __cplusplus
extern "C" {
#endif
  int8_t progfs_stat(const char *pathname, struct stat *buf);
  int8_t progfs_fstat(FD *fd, struct stat *buf);
  int8_t progfs_opendir(const char *path, DIR *d);
  int8_t progfs_closedir(DIR *dirp);
  struct dirent *progfs_readdir(DIR *dirp);
  void progfs_rewinddir(DIR *dirp);
  int8_t progfs_open(const char *path, uint8_t flags, FD *fd);
  uint8_t progfs_read(FD *fd, void *buf, uint8_t size);
  uint8_t progfs_write(FD *fd, void *buf, uint8_t size);
  int8_t progfs_chdir(const char *path);
#ifdef __cplusplus
}
#endif

#endif
