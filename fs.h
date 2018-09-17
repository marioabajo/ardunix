#ifndef FS_H
#define FS_H

#include "platform.h"

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

#define FS_TYPE_PROGFS 0   // Progfs filesystem id 0
#define FS_MASK_FILETYPE 192
#define FS_MASK_PERMISSION 7
#define FS_READ   1
#define FS_WRITE  2
#define FS_EXEC   4
#define FS_FILE   0
#define FS_DIR    64
#define FS_LINK   128
#define FS_DEV    192

// Open call flags
#define O_RDONLY 0x00
#define O_WRONLY 0x01
#define O_RDWR   0x02
#define O_APPEND 0x08
#define O_CREATE 0x10
#define O_TRUNC  0x20
#define O_EXCL   0x40

struct dirent
{
  char     d_name[FILENAME_MAX];
  uint8_t  flags;  /* Flags of the file */
  long     d_ino;  /* Address of the data */
  uint16_t size;   /* size of the file */
};

typedef struct fsentry
{
  uint8_t dd_size; /* amount of data returned by getdirentries */
  uint8_t dd_buf;  /* Pointer to data buffer */
  long    dd_loc;  /* offset inside data buffer */
  struct dirent dd_ent; /* pointer to dirent structure */
} DIR;

struct stat 
{
  long     st_ino;         /* inode number */
  uint8_t  st_mode;        /* protection */
  uint16_t st_size;        /* total size, in bytes */
};

struct statvfs
{
  uint8_t  vfs_fstype;      /* file system type id */
  long     vfs_bksize;      /* block size in bytes */
  long     vfs_size;        /* total size in blocks */
  long     vfs_free;        /* free size in blocks */
};

typedef struct file_descriptor
{
  // TODO: pointer to fs operations
  uint8_t dev;              /* device number */
  long    inum;             /* inode number */
  long    address;          /* address inside file */
  long    size;             /* file size */
  uint8_t flags;            /* file flags */
} FD;

#ifdef __cplusplus
extern "C"{
#endif
  char *normalize_path(const char *path);
  char *normalize_path_l(const char *path, uint8_t len);
  char *normalize_paths(const char *path1, const char* path2);
  char *normalize_paths_l(const char *path1, uint8_t len1, const char *path2, uint8_t len2);
  char *getcwd(char *buf, size_t size);
  int8_t statvfs(const char* path, struct statvfs *buf);
  int8_t fstatvfs(FD *fd, struct statvfs *buf);
  int8_t stat(const char *pathname, struct stat *buf);
  int8_t fstat(FD *fd, struct stat *buf);
  int8_t opendir(const char *path, DIR *d);
  int8_t closedir(DIR *dirp);
  struct dirent *readdir(DIR *dirp);
  void rewinddir(DIR *dirp);
  int8_t open(const char *path, uint8_t flags, FD *fd);
  uint8_t read(FD *fd, void *buf, uint8_t size);
  uint8_t write(FD *fd, void *buf, uint8_t size);
  int8_t chdir(const char *path);
#ifdef __cplusplus
}
#endif

// tests
/*
printf("normalize_path: %s\n", normalize_path("/"));	
printf("normalize_path: %s\n", normalize_path("/home/abc"));
printf("normalize_path: %s\n", normalize_path("home/abc"));
printf("normalize_path: %s\n", normalize_path("/its/a/path/too/large/.///to/be/stored/../../../../in//the/PATH/MAX/variable/except/if/correctly/..//proccessed/"));
printf("normalize_path: %s\n", normalize_path("/home/abc/../../etc/example/../"));
*/

#endif


