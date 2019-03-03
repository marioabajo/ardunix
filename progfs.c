#include <stdlib.h>
#include <string.h>
#include "progfs.h"
#include "kernel.h"

static struct dirent * copy_dirent(DIR *d, uint8_t inode)
/* Fill a dirent structure copyng the data from a DIR structure.
 * No allocation is done as the memory is already reserved inside 
 * DIR structure
 * 
 * Input: DIR structute, inode number
 * Returns: direntry
 */
{
  struct dirent *e;
  PFS2 thisentry;

  // copy the inode data to memory for simplicity
  memcpy_P(&thisentry, &ProgFs2[inode], sizeof(PFS2));

  // fill the dirent structure
  e = &d->dd_ent;
  strncpy_P(e->d_name, thisentry.name, FILENAME_MAX);
  e->flags = thisentry.perm;
  e->d_ino = inode;
  e->size = thisentry.size;

  return e;
}

int8_t progfs_stat(const char *pathname, struct stat *buf)
/* Fill a stat structure with all the information about the file
 * if the file was not found, return false
 * 
 * Input: string (path), struct stat
 * Returns: 0 -> found
 *          EINVNAME -> invalid filename
 *          ENAMETOOLONG -> path too large
 *          ENOTFOUND -> not found
 */
{
  uint8_t i = 0, ptr = 0;
  char actualname[PATH_MAX] = "";
  PFS2 thisentry;

  if (pathname == 0)
    return EINVNAME; //invalid filename

  do
  {
    // copy to memory the progfs2 entry to work with
    memcpy_P(&thisentry, &ProgFs2[i], sizeof(PFS2));

    // read the name of the next entry, if it's not null copy to actualname, 
    // if it's null must be a directory
    if (thisentry.name)
    {
      strncpy_P(actualname + ptr, thisentry.name, PATH_MAX - ptr);

      //DEBUGING: printf("DEBUG progfs_stat: ptr:%d i:%d filename: %s actualname:%s\n",ptr,i,pathname,actualname);
      // compare it with the filename supplied, if coincide, we have a match and we have finished
      if (strncmp(pathname, actualname, PATH_MAX) == 0)
      {
        buf->st_ino = i;
        buf->st_mode = thisentry.perm;
        buf->st_size = thisentry.size;
        return 0;
      }

      // prepare for the next iteration
      // if the actual entry is a directory, add '/' at the end
      if (((thisentry.perm & FS_MASK_FILETYPE) == FS_DIR) && (thisentry.perm & FS_EXEC))
      {
        // ptr points to the last position of the directory in the pathname
        ptr += strlen_P(thisentry.name);
        // this if is to avoid doing this with the root directory
        if (ptr > 1)
        {
          if (ptr + 2 >= PATH_MAX)
            return ENAMETOOLONG; // path too large
          actualname[ptr++] = '/';
        }
        actualname[ptr] = 0;
      }
    }
    // if the entry it's null indicates the end of a directory
    else if (ptr > 0)
    {
      // go back and delete the last element in the path
      do
        ptr--;
      while (ptr > 0 && actualname[ptr-1] != '/');
      actualname[ptr] = 0;
    }
    i++;
  }
  while (ptr > 0);

  return ENOTFOUND; // Not found
}

int8_t progfs_fstat(FD *fd, struct stat *buf)
/* Fill a stat structure from an already open file
 * given the file descriptor
 * 
 * Input: FD, struct stat
 * Returns: 0 -> ok
 *          EINVNAME -> invalid FD
 *          EADDRNOTAVAIL -> invalid struct stat
 */
{
  uint8_t perm;

  if (fd == NULL)
    return EINVNAME;
  if (buf == NULL)
    return EADDRNOTAVAIL;

  // TODO: check limits of fd->inum
  perm = pgm_read_byte((uint8_t *) &ProgFs2[fd->inum] + offsetof(PFS2, perm));

  buf->st_ino = fd->inum;
  buf->st_mode = perm;
  buf->st_size = fd->size;

  return 0;
}

int8_t progfs_opendir(const char *path, DIR *d)
/* Fill a DIR structure with data over a directory string 
 * passed as an argument, with the idea of going through
 * any or all the elements of the directory.
 * 
 * Input: path string, DIR struct (already reserved)
 * Returns: 0 -> ok
 *          EINVNAME -> invalid filename
 *          ENAMETOOLONG -> path too large
 *          ENOTFOUND -> not found
 *          ENOTDIR -> Not a directory
 */
{
  uint8_t ret;
  struct stat file;
  uint16_t size;

  // Stat the directory and check that it is a directory
  if ((ret = progfs_stat(path, &file)) != 0)
    return ret; // see progfs_stat returns

  if (!((file.st_mode & FS_MASK_FILETYPE) == FS_DIR))
    return ENOTDIR; // not a directory

  // Read the size of the directory
  size = pgm_read_word((uint16_t *) &ProgFs2[file.st_ino] + offsetof(PFS2, size));

  // Position to the first child
  d->dd_size = size;
  d->dd_buf = file.st_ino;
  d->dd_loc = 1;
  
  copy_dirent(d, file.st_ino);

  return 0;
}

struct dirent *progfs_readdir(DIR *dirp)
/* Read the next entry in the dircetory
 * 
 * Input: DIR struct
 * Returns: dirent struct
 *          NULL -> No more entries left or no 
 *                  permission to list the directory
 */
{
  uint8_t level = 0, i;
  struct dirent *e;
  PFS2 thisentry;

  if (!(dirp->dd_ent.flags & FS_READ))
    return NULL; // we don't have reading permissions in this directory
 
  i = dirp->dd_buf + dirp->dd_loc;

  // Copy the entry to ram so it's easy to work with
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
    if (level)
      memcpy_P(&thisentry, &ProgFs2[i], sizeof(PFS2));
  } while (level);

  return e;
}

int8_t progfs_closedir(DIR *dirp)
{
  return 0;
}

void progfs_rewinddir(DIR *dirp)
{
  dirp->dd_loc = 1;
}

int8_t progfs_open(const char *path, uint8_t flags, FD *fd)
/* Open a file given it's path so a File Descriptor structure is
 * filled with data
 * 
 * Input: path string, open flags (actually read is supported),
 *        FD structures (already reserved)
 * Returns: 0 -> ok
 *          EINVNAME -> invalid filename
 *          ENAMETOOLONG -> path too large
 *          ENOTFOUND -> not found
 *          EISDIR -> it's a directory
 */
{
  uint8_t ret;
  struct stat file;

  // Stat the file
  if ((ret = progfs_stat(path, &file)) != 0)
    return ret; // see progfs_stat returns

  if ((file.st_mode & FS_MASK_FILETYPE) == FS_DIR)
    return EISDIR; // it's a directory

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
  //unsigned int data;
  long cont=0;
  PFS2 thisentry;

  // TODO: we could do better, just copy the ptr pointer
  //data = pgm_read_ptr(&ProgFs2[fd->inum] + offsetof(PFS2, ptr)); 
  memcpy_P(&thisentry, &ProgFs2[fd->inum], sizeof(PFS2));

  for (; fd->address < fd->size && cont < size; fd->address++)
  {
    ptr = buf + cont;
    *ptr = pgm_read_byte(thisentry.ptr + fd->address);
    //*ptr = pgm_read_byte(data + fd->address);
    cont++;
  }

  return cont;
}

uint8_t progfs_write(FD *fd, void *buf, uint8_t size)
{
	return 1;  // we actually cannot support writing in arduino harvard architecture
}

int8_t progfs_chdir(const char *path)
{
	int8_t ret;
	struct stat dir;

	// Try to access it and recover information
	if ((ret = progfs_stat(path, &dir)) != 0)
		return ret; // see progfs_stat returns

	// If we have acces and is a directory, then is valid
	if ((dir.st_mode & FS_MASK_FILETYPE) != FS_DIR)
		return ENOTDIR; // it's not a directory

	return ret;
}
