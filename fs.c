#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int stat(const char *pathname, struct stat *buf)
{
  DIR *dir = ProgFs;
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

  dir = (DIR *)pgm_read_ptr(&(dir->child));

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
      dir = (DIR *)pgm_read_ptr(&(dir->next));

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
    dir = (DIR *)pgm_read_ptr(&(dir->child));
  }

  return -1;
  
}

// Open a directory for reading. 
DIR *opendir(const char *path)
{
  struct stat file;

  // Stat the directory and check that it is a directory
  if (stat(path, &file) != 0  || file.st_mode & 0x40 != 0)
    return NULL;

  return (DIR *)file.st_ino;
}

struct dirent *readdir(DIR *dirp)
{
  /*static struct dirent result;
  char filename[FILENAME_MAX];

  if (dirp == NULL)
    return NULL;

  strcpy_P(result.d_name, (char *)pgm_read_ptr(dirp->filename));
  result.flags = (uint8_t)pgm_read_byte(&(dirp->flags));
  result.size = (uint16_t)pgm_read_word((uint16_t *)&(dirp->size));
  if (result.flags & 0x40)
    result.inode = NULL;
  //else
  //  result.data = pgm_read_word_near(dirp->data);
  dirp = (DIR *)pgm_read_ptr(&(dirp->next));

  return &result;*/
}

/*DIR *fsentry_sublevel_find(const char *filename)
{
  DIR *dir = (struct fsentry *)fs.child;
  uint8_t i=1,j=1;

  // Only absolute paths allowed for now
  // Empty paths not allowed
  if (filename == NULL || filename[0] != '/' || filename[0] == 0)
    return NULL;

  // root
  if (filename[0] == '/' && filename[1] == 0)
    return dir;

  // look for each component of the path
  while (filename[j]!=0)
  {
    // split path by "/"
    do 
    { 
      j++; 
    } while (filename[j]!='/' && filename[j]!=0);
    if ((dir = fsentry_onelevel_find(dir,filename+i,j-i)) == NULL)
      // path not found
      return NULL;
    if (filename[j]!=0 && dir->flags & 0x40)
      dir = (struct fsentry *)dir->child;
    i=j+1;
  }

  return dir;
}

DIR *fsentry_onelevel_find(DIR *parent, const char *name, uint8_t namesize)
{
	if (namesize == 0)
		namesize = strlen(name);

	if ((name == NULL) || (strncmp("/",name,namesize))==0)
		return parent;

	while (parent != NULL)
	{
		if (strncmp(parent->filename,name,namesize) == 0)
			return parent;

		parent=parent->next;
	}
	return NULL;
}*/

/*bool fsentry_add(const char *name, DIR *parent, uint8_t flags, void *function_name)
{
	struct fsentry *temp;

	// parent directory must by a directory
	if (! parent->flags & 0x80)
		return false;

	// reserve memory for the new entry
	if ((temp = malloc(sizeof(struct fsentry))) == NULL ||
	    (temp->filename = malloc(strlen(name))) == NULL)
	{
		free(temp);
		return false;
	}

	// Look if th directory is empty
	if (parent->child==NULL)
	{
		parent->child = temp;
		parent=parent->child;
	}
	else
	{
		// go to the last child element of the directory
		parent = parent->child;
		while (parent->next != NULL)
			parent = parent->next;
		parent->next = temp;
		parent = parent->next;
	}

	// fill the data
	strcpy(parent->filename, name);
	parent->flags = flags;
	parent->next = NULL;
	if (parent->flags & 0x80)
		parent->child = NULL;
	else
		parent->data=function_name;

	return true;
}*/

