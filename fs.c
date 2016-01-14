#include "fs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Init root entry
struct fsentry fs = {"/",NULL,0x89,NULL};

// Open a directory for reading. 
DIR *opendir(const char* path)
{
  DIR *dir=fs.child;
  u8 i=1,j=1;

  // TODO: only absolute paths allowed for now
  // Empty paths not allowed
  if (path[0]!='/' || path[0]==0)
    return NULL;

  // root
  if (path[0]=='/' && path[1]==0)
    return dir;

  // look for each component of the path
  while (path[j]!=0)
  {
    // split path by "/"
    do 
    { 
      j++; 
    } while (path[j]!='/' && path[j]!=0);
    if ((dir = fsentry_onelevel_find(dir,path+i,j-i)) == NULL)
      // path not found
      return NULL;
    i=j+1;
  }

  return dir->child;
}

DIR *readdir(DIR *dirp)
{
	if (dirp == NULL)
		return NULL;

	dirp = dirp->next;
	return dirp;
}

DIR *fsentry_onelevel_find(DIR *parent, const char *name, u8 namesize)
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
}

boolean fsentry_add(const char *name, DIR *parent, u8 flags, void *function_name)
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
}

/*boolean fsentry_add_file(const char *name, struct fsentry *parent, u8 flags, void *function_name)
{
	// find the parent directory
	//if ((new = fsentry_search(&fs,parentname))==NULL)
	//	return false;

}*/
