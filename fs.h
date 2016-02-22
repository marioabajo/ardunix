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
typedef struct fsentry
{
  uint8_t dd_size; /* amount of data returned by getdirentries */
  char   *dd_buf;  /* Pointer to data buffer */
  long    dd_loc;  /* offset inside data buffer */
  struct dirent *dd_ent; /* pointer to dirent structure */
} DIR;

struct dirent
{
  char     d_name[FILENAME_MAX];
  uint8_t  flags;  /* Flags of the file */
  long     d_ino;  /* Address of the data */
  uint16_t size;   /* size of the file */
};

struct stat 
{
  long     st_ino;         /* inode number */
  uint8_t  st_mode;        /* protection */
  uint8_t  st_size;        /* total size, in bytes */
};


#ifdef __cplusplus
extern "C"{
#endif
  int stat(const char *pathname, struct stat *buf);
  // Open a directory for reading. 
  DIR *opendir(const char* path);
  uint8_t closedir(DIR *dirp);
  struct dirent *readdir(DIR *dirp);
  //DIR *fsentry_sublevel_find(const char *filename);
  //struct fsentry *fsentry_onelevel_find(struct fsentry *parent, const char *name, uint8_t namesize);
  //bool fsentry_add(const char *name, struct fsentry *parent, uint8_t flags, void *function_name);
#ifdef __cplusplus
}
#endif

#endif

