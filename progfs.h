#ifndef PROGFS_H
#define PROGFS_H

#include "platform.h"
#include "fs.h"

typedef struct progfsentry {
    const char *name;
    void *ptr;
    uint8_t perm;
    uint16_t size;
} PFS2;

extern const PFS2 ProgFs2[];

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t progfs_stat(const char *pathname, struct stat *buf);
  uint8_t progfs_fstat(FD *fd, struct stat *buf);
  uint8_t progfs_opendir(const char *path, DIR *d);
  uint8_t progfs_closedir(DIR *dirp);
  struct dirent *progfs_readdir(DIR *dirp);
  void progfs_rewinddir(DIR *dirp);
  uint8_t progfs_open(const char *path, uint8_t flags, FD *fd);
  uint8_t progfs_read(FD *fd, void *buf, uint8_t size);
  uint8_t progfs_write(FD *fd, void *buf, uint8_t size);
#ifdef __cplusplus
}
#endif

#endif
