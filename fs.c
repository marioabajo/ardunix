#include "fs.h"
#include "progfs.h"

uint8_t statvfs(const char* path, struct statvfs *buf)
{
    // TODO: actually we only support the internal FS
    
    buf->vfs_fstype = FS_TYPE_PROGFS;
    buf->vfs_bksize = 1;
    buf->vfs_size = 0;
    buf->vfs_free = 0;

    return 0;
}

uint8_t fstatvfs(FD *fd, struct statvfs *buf)
{
    // TODO: actually we only support the internal FS
    
    buf->vfs_fstype = FS_TYPE_PROGFS;
    buf->vfs_bksize = 1;
    buf->vfs_size = 0;
    buf->vfs_free = 0;

    return 0;
}

uint8_t stat(const char *pathname, struct stat *buf)
{
  // TODO: virtual filesystem structure in process
  return progfs_stat(pathname, buf);
}

// Open a directory for reading. 
uint8_t opendir(const char *path, DIR *d)
{
  // TODO: virtual filesystem structure in process
  return progfs_opendir(path, d);
}

// Close a directory previously opened
uint8_t closedir(DIR *dirp)
{
  // TODO: virtual filesystem structure in process
  return progfs_closedir(dirp);
}

struct dirent *readdir(DIR *dirp)
{
  // TODO: virtual filesystem structure in process
  return progfs_readdir(dirp);
}

void rewinddir(DIR *dirp)
{
  // TODO: virtual filesystem structure in process
  return progfs_rewinddir(dirp);
}

uint8_t open(const char *path, uint8_t flags, FD *fd)
{
  // TODO: virtual filesystem structure in process
  return progfs_open(path, flags, fd);
}

uint8_t read(FD *fd, void *buf, uint8_t size)
{
  // TODO: virtual filesystem structure in process
  return progfs_read(fd, buf, size);
}

uint8_t write(FD *fd, void *buf, uint8_t size)
{
  // TODO: virtual filesystem structure in process
  return progfs_write(fd, buf, size);
}

uint8_t fstat(FD *fd, struct stat *buf)
{
  // TODO: virtual filesystem structure in process
  return progfs_fstat(fd, buf);
}


