#include "progfs.h"
#include <stdlib.h>
#include <string.h>

int progfs_stat(const char *pathname, struct stat *buf)
{
  PFS *dir = ProgFs;
  uint8_t i=0,j=0;

  // Only absolute paths allowed for now
  // Empty paths not allowed
  // TODO: normalize pathnames
  if (pathname == NULL || pathname[0] != '/' || pathname[0] == 0)
    return -1;

  // If path is "/" return root element
  if (pathname[0] == '/' && pathname[1] == 0)
  {
    buf->st_ino = (long)dir;
    buf->st_mode = (uint8_t)pgm_read_byte(&(dir->flags));
    buf->st_size = (uint16_t)pgm_read_word(&(dir->size));
    return 0;
  }

  // Lets look inside the root's children
  // /  (root)
  // |- ...
  // ...
  dir = (PFS *)pgm_read_ptr(&(dir->child));

  // Now, examine the pathname and start digging into the directory structure
  // while reading the pathname elementes: /element1/element2/....
  // vars i and j will be pointing to the beging and the end of each element
  // inside the string, just to save some bytes of ram
  while (pathname[j] != 0)
  {
    // Advance in the path string to the next component, delimited by i and j
    i = j + 1;
    do
    {
      j++;
    } while (pathname[j]!='/' && pathname[j]!=0);

    //DEBUG// printf("NEXT:%d, %d: %s %d\n", i, j, pathname+i, j-i);

    // loop within the elements of the filesystem in the same level and compare
    // the name with the name of the current element being watched in pathname
    while (dir != NULL && (strncmp_P(pathname+i, (char *)pgm_read_ptr(&(dir->filename)), j-i) != 0))
      dir = (PFS *)pgm_read_ptr(&(dir->next));

    // DEBUG
    //char filename[FILENAME_MAX];
    //strncpy_P(filename, (DIR *)pgm_read_ptr(&(dir->filename)), 16);
    //printf("DEBUG: %s %d %s %d\n", pathname+i, j-i, filename, strncmp_P(pathname+i, (DIR *)pgm_read_ptr(&(dir->filename)), j-i));

    // element not found
    if (dir == NULL)
      return -1;

    // Found a match, check if there is more components left in the pathname
    // to look for
    if (pathname[j]==0)
    {
      buf->st_ino = (long)dir;
      buf->st_mode = (uint8_t)pgm_read_byte(&(dir->flags));
      buf->st_size = (uint16_t)pgm_read_word(&(dir->size));
      return 0;
    }

    // There are more elements left in the path, so necesary this (actual)
    // element must be a directory (and have access), we must check it.
    if ((uint8_t)pgm_read_byte(&(dir->flags)) & 0x41 == 0)
      return -1;

    // Get into the child object
    dir = (PFS *)pgm_read_ptr(&(dir->child));
  }

  return -1;
  
}

DIR *progfs_opendir(const char *path)
{
  struct stat file;
  DIR *d;
  PFS *pfsd;

  // Stat the directory and check that it is a directory
  if (stat(path, &file) != 0  || file.st_mode & 0x40 == 0)
    return NULL;

  if ((d = (DIR *)malloc(sizeof(DIR))) == NULL)
	return NULL;

  // Position to the first child
  //pfsd = (PFS *)pgm_read_ptr(&(file.st_ino));
  pfsd = (PFS *)file.st_ino;
  d->dd_size = pfsd->size;
  d->dd_buf = (PFS *)pgm_read_ptr(&(pfsd->child));
  d->dd_loc = 0;
  d->dd_ent = (struct dirent *)malloc(sizeof(struct dirent));
  if (d->dd_ent == NULL)
    return NULL;

  return d;
}

uint8_t progfs_closedir(DIR *dirp)
{
  free(dirp->dd_ent);
  free(dirp);
  return 0;
}

struct dirent *progfs_readdir(DIR *dirp)
{
  static struct dirent *result;
  PFS *entry;
  long offset;

  if (dirp == NULL)
    return NULL;

  // Are we already in the last entry? if so, exit
  if (dirp->dd_loc == -(long)(dirp->dd_buf))
    return NULL;

  result = dirp->dd_ent;

  // Load the pointer to the disk/flash structure
  entry = (PFS *)(dirp->dd_buf + dirp->dd_loc);

  // Copy the filename, flags and size
  strncpy_P(result->d_name, (const char *)pgm_read_ptr(&(entry->filename)), FILENAME_MAX);
  result->flags = (uint8_t)pgm_read_byte(&(entry->flags));
  result->size = (uint16_t)pgm_read_word((uint16_t *)&(entry->size));

  // Load the pointer to the data if it's a file
  if (result->flags & 0x40)
    result->d_ino = 0;
  else
    result->d_ino = (long)pgm_read_ptr(&(entry->data));

  // Put in the dd_loc the increment of the pointer to the next entry
  offset = (long)pgm_read_ptr(&(entry->next));

  // Last entry? set special code, offset = -buffer
  if (offset == 0)
    dirp->dd_loc = -(long)(dirp->dd_buf);
  else
    dirp->dd_loc = offset - (long)dirp->dd_buf;

  return result;
}

void progfs_rewinddir(DIR *dirp)
{
  dirp->dd_loc = 0;
}

