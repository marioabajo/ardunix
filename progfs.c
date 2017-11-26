//#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "progfs.h"
/* Warning: be careful when including stdio.h because it has the first definition
 of FILENAME_MAX and can(will) do stack corruption 
 */

struct dirent * copy_dirent(DIR *d, uint8_t inode)
{
  struct dirent *e;
  PFS2 thisentry;

  memcpy_P(&thisentry, &ProgFs2[inode], sizeof(PFS2));

  e = &d->dd_ent;
  strncpy_P(e->d_name, thisentry.name, FILENAME_MAX);
  e->flags = thisentry.perm;
  e->d_ino = inode;
  e->size = thisentry.size;

  return e;
}

uint8_t progfs_stat(const char *pathname, struct stat *buf)
{
    uint8_t i = 0, ptr = 0;
    char actualname[PATH_MAX] = "";
    PFS2 thisentry;
    
    if (pathname == 0)
        return 1; //invalid filename
    
    do
    {
        memcpy_P(&thisentry, &ProgFs2[i], sizeof(PFS2));
        // read the name of the next entry, if it's not null copy to actualname
        if (thisentry.name)
        {
            strncpy_P(actualname + ptr, thisentry.name, PATH_MAX - ptr);
            
            //DEBUGING: printf("DEBUG progfs_stat: ptr:%d i:%d actualname:%s\n",ptr,i,actualname);
            // compare it with the filename supplied, if coincide, we have a match
            if (strncmp(pathname, actualname, PATH_MAX) == 0)
            {
                buf->st_ino = i;
                buf->st_mode = thisentry.perm;
                buf->st_size = thisentry.size;
                return 0;
            }

            // prepare for the next iteration
            // if the actual entry is a directory, add '/' at the end
            // TODO: check permissions
            if ((thisentry.perm & FS_MASK_FILETYPE) == FS_DIR)
            {
                // ptr points to the last position of the directory in the pathname
                ptr += strlen_P(thisentry.name);
                if (ptr>1)
                {
                    if (ptr + 2 >= PATH_MAX)
                        return 1; // path too large
                    actualname[ptr++] = '/';
                    actualname[ptr] = 0;
                }
            }
        }
        // if the entry it's null indicates the end of a directory
        else
        {
            // go back and delete the last element in the path
            ptr--;
            while (ptr > 0 && actualname[ptr-1] != '/')
                ptr--;
            actualname[ptr] = 0;
        }
    
        i++;
    }
    while (ptr > 0);

    return 255; // Not found
}

uint8_t progfs_fstat(FD *fd, struct stat *buf)
{
  PFS2 thisentry;

  if (fd == NULL)
    return 1;
  if (buf == NULL)
    return 2;

  // TODO: we could do better, just copy the perm byte
  memcpy_P(&thisentry, &ProgFs2[fd->inum], sizeof(PFS2));

  buf->st_ino = fd->inum;
  // TODO: check limits of fd-inum
  buf->st_mode = thisentry.perm;
  buf->st_size = fd->size;

  return 0;
}

uint8_t progfs_opendir(const char *path, DIR *d)
{
  struct stat file;
  PFS2 thisentry;

  // Stat the directory and check that it is a directory
  if (progfs_stat(path, &file) != 0 || (file.st_mode & FS_MASK_FILETYPE) != FS_DIR)
    return 1; // directory doesn't exist

  if ((file.st_mode & FS_MASK_PERMISSION) != (FS_EXEC | FS_READ))
    return 2; // permission denied

  // TODO: we could do better, just copy the size word
  memcpy_P(&thisentry, &ProgFs2[file.st_ino], sizeof(PFS2));

  // Position to the first child
  d->dd_size = thisentry.size;
  d->dd_buf = file.st_ino;
  d->dd_loc = 0;
  
  copy_dirent(d, file.st_ino);

  return 0;
}

struct dirent *progfs_readdir(DIR *dirp)
{
  uint8_t level = 0, i;
  struct dirent *e;
  PFS2 thisentry;

  // if it's the root directory, we want to list whats inside it, so we need this litle trick
  if (dirp->dd_loc == 0)
      dirp->dd_loc++;
 
  i = dirp->dd_buf + dirp->dd_loc;

  // TODO: we could do better, just copy the perm byte
  memcpy_P(&thisentry, &ProgFs2[i], sizeof(PFS2));

  // If the actual entry is a directory terminator, we reached the end of the listing
   if (thisentry.name == 0)
      return NULL;

  // Read entry
  e = copy_dirent(dirp, i);

  // Point to next entry, but check first if its a directory and bypass the contents
  do
  {
      if ((thisentry.perm & FS_MASK_FILETYPE) == FS_DIR)
          level++;
      else if (thisentry.name == 0)
          level--;
      dirp->dd_loc++;
      i++;
      // TODO: we could do better, just copy the name pointer
      if (level > 0)
        memcpy_P(&thisentry, &ProgFs2[i], sizeof(PFS2));
  } while (level > 0);

  return e;
}

uint8_t progfs_closedir(DIR *dirp)
{
  return 0;
}

void progfs_rewinddir(DIR *dirp)
{
  dirp->dd_loc = 0;
}

uint8_t progfs_open(const char *path, uint8_t flags, FD *fd)
{
  struct stat file;

  // Stat the file
  if (progfs_stat(path, &file) != 0)
    return 1; // file doesn't exist

  if ((file.st_mode & FS_MASK_FILETYPE) == FS_DIR)
    return 2; // it's a directory

  // TODO: check flags

  // TODO: fd->dev = ;  
  fd->inum = file.st_ino;
  fd->address = 0;
  fd->size = file.st_size;
  fd->flags = flags;

  return 0; // ok
}

uint8_t progfs_read(FD *fd, void *buf, uint8_t size)
{
  char *ptr;
  long cont=0;
  PFS2 thisentry;

  // TODO: we could do better, just copy the ptr pointer
  memcpy_P(&thisentry, &ProgFs2[fd->inum], sizeof(PFS2));
  
  for (; fd->address < fd->size && cont < size; fd->address++)
  {
    ptr = buf + cont;
    *ptr = pgm_read_byte(thisentry.ptr + fd->address);
    cont++;
  }

  return cont;
}

uint8_t progfs_write(FD *fd, void *buf, uint8_t size)
{
  // TODO
  return 0;
}


