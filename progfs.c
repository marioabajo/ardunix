#include "progfs.h"

int progfs_stat(const char *pathname, struct stat *buf)
{
  PFS *dir = ProgFs;
  uint8_t i=0,j=0;

  // Only absolute paths allowed for now
  // Empty paths not allowed
  if (pathname == NULL || pathname[0] != '/' || pathname[0] == 0)
    return -1;

  // root
  if (pathname[0] == '/' && pathname[1] == 0)
  {
    buf->st_ino = (unsigned int)dir;
    buf->st_mode = pgm_read_byte(&(dir->flags));
    buf->st_size = pgm_read_byte(&(dir->size));
    return 0;
  }

  dir = (PFS *)pgm_read_ptr(&(dir->child));

  while (pathname[j] != 0)
  {
    // Advance in the path string to the next component, delimited by i and j
    i = j + 1;
    do
    {
      j++;
    } while (pathname[j]!='/' && pathname[j]!=0);

    //DEBUG// printf("NEXT:%d, %d: %s %d\n", i, j, pathname+i, j-i);

    while (dir != NULL && (strncmp_P(pathname+i, (char *)pgm_read_ptr(&(dir->filename)), j-i) != 0))
      dir = (PFS *)pgm_read_ptr(&(dir->next));

    // DEBUG
    //char filename[MAX_FILENAME];
    //strncpy_P(filename, (DIR *)pgm_read_ptr(&(dir->filename)), 16);
    //printf("DEBUG: %s %d %s %d\n", pathname+i, j-i, filename, strncmp_P(pathname+i, (DIR *)pgm_read_ptr(&(dir->filename)), j-i));

    // not found
    if (dir == NULL)
      return -1;

    // Found a match, check if the is more components left to search
    if (pathname[j]==0)
    {
      buf->st_ino = (unsigned int)dir;
      buf->st_mode = (uint8_t)pgm_read_byte(&(dir->flags));
      buf->st_size = (uint16_t)pgm_read_byte(&(dir->size));
      return 0;
    }
    // There are more elements left in the path, so check if we are in a directory.
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

  // Stat the directory and check that it is a directory
  if (stat(path, &file) != 0  || file.st_mode & 0x40 != 0)
    return NULL;

  return (DIR *)file.st_ino;
}

