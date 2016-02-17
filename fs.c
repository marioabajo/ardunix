#include "fs.h"
#include "progfs.h"

int stat(const char *pathname, struct stat *buf)
{
  // TODO: virtual filesystem structure in process
  return progfs_stat(pathname, buf);
}

// Open a directory for reading. 
DIR *opendir(const char *path)
{
  // TODO: virtual filesystem structure in process
  return progfs_opendir(path);
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

