#include "fs.h"
#include "progfs.h"
#include "kernel.h"
#include <string.h>
#include <stdlib.h>

char *normalize_path(const char *path)
/* Given a path string, returns a normalized path. The given string must be
 * freed after use.
 * 
 * Input: path string
 * Returns: NULL -> error
 *          other -> a normalized path
 */
{
	return normalize_path_l(path, strlen(path));
}

char *normalize_path_l(const char *path, uint8_t len)
/* Given a path string and it's length, returns a normalized path. 
 * The given string must be freed after use.
 * 
 * Input: path string,
 *        lenght of the string
 * Returns: NULL -> error
 *          other -> a normalized path
 */
{
	char newpath[PATH_MAX];
	char *path_export;
	uint8_t i = 0, j = 0;

	// in case of absolute path, copy the first slash
	if (path[0] == '/')
		newpath[j++] = path[i++];
	

	/* Start reading the string "path" with an index "i" and copy block
	   by block "xxx/" to another string "newpath" with an index "j".
	   Go back one element of the path if find a "../"
	*/
	
	while (i < len)
	{
		// threat path elements like './' or '../' or '..'
		if (path[i] == '.')
		{
			if (path[i+1] == '.' && 
			   (path[i+2] == '/' || path[i+2] == 0))
			{
				// go back one element
				if (j > 0)
					j--;
				while (j > 0 && newpath[--j] != '/');
				if (j > 0 || newpath[j] == '/')
					j++;
				i+=3;
				continue;
			}
			else if (path[i+1] == '/')
			{
				// ignore element
				i+=2;
				continue;
			}
		}
		// threat path elements like '//'
		else if (path[i] == '/')
		{
			i++;
			continue;
		}
		// copy element
		do
			newpath[j++] = path[i];
		while (path[i++] != '/' && j < PATH_MAX && i < len);
		//if (j < PATH_MAX && i < len && path[i] == '/')
		//	newpath[j++] = path[i++];
	}

	// verify the limits
	if (j > PATH_MAX)
		return NULL;

	// tranform this (eg.: /a/b/ or /a/b/.) in this (eg.: /a/b)
	if (j > 1 && (newpath[j-1] == '/' || newpath[j-1] == '.'))
		j--;

	// close string
	newpath[j++] = 0;

	// reserve the exact memory needed
	if ((path_export = (char *)malloc(j)) == NULL)
		return NULL;

	// copy the string
	memcpy(path_export, newpath, j);

	return path_export;
}

char *normalize_paths(const char *path1, const char* path2)
/* Given two paths strings, returns a normalized path with the concatenation of
 * both. This function is envisioned with the PATH var normalization in mind.
 * The given string must be freed after use.
 * 
 * Input: path string,
 *        path string
 * Returns: NULL -> error
 *          other -> a normalized path
 */
{
	return normalize_paths_l(path1, strlen(path1), path2, strlen(path2));
}

char *normalize_paths_l(const char *path1, uint8_t len1, const char *path2, uint8_t len2)
/* Given two paths strings and it's length, returns a normalized path with the
 * concatenation of both. This function is envisioned with the PATH var 
 * normalization in mind.
 * The given string must be freed after use.
 * 
 * Input: path string,
 *        lenght of the string,
 *        path string,
 *        lenght of the string
 * Returns: NULL -> error
 *          other -> a normalized path
 */
{
	char *newpath, *path_export;
	
	// reserve the exact memory needed
	if ((newpath = (char *)malloc(len1 + len2)) == NULL)
		return NULL;

	memcpy(newpath, path1, len1);
	newpath[len1] = '/';
	memcpy(newpath + len1 + 1, path2, len2);
	path_export = normalize_path_l(newpath, len1 + len2 + 1);
	free(newpath);
	if (path_export == NULL)
		return NULL;
	
	return path_export;
}

static char * sanitize_path(const char *path)
{
	char *newpath;

	if (path[0] == '/')
		newpath = normalize_path(path);
	else
		newpath = normalize_paths(procs[current_proc].cwd, path);

	return newpath;
}

char *getcwd(char *buf, size_t size)
{
	char *ptr;
	uint8_t total;

	ptr = NULL;
	total = strlen(procs[current_proc].cwd) + 1;

	if (buf == NULL)
	{
		if ((ptr = (char *)malloc(total)) == NULL)
			return NULL;
		memcpy(ptr, procs[current_proc].cwd, total);
	}
	else if (size >= total)
	{
		memcpy(buf, procs[current_proc].cwd, total);
		ptr = buf;
	}

	return ptr;
}

int8_t statvfs(const char* path, struct statvfs *buf)
{
	int8_t ret;
	char *pathok;

	if ((pathok = sanitize_path(path)) == NULL)
		return EINVNAME;

	// TODO: actually we only support the internal FS
	// TODO: HARDCODED!!!!
	buf->vfs_fstype = FS_TYPE_PROGFS;
	buf->vfs_bksize = 1;
	buf->vfs_size = 0;
	buf->vfs_free = 0;
	ret = 0;
	free(pathok);

	return ret;
}

int8_t fstatvfs(FD *fd, struct statvfs *buf)
{
	// TODO: actually we only support the internal FS
	// TODO: HARDCODED!!!!
	buf->vfs_fstype = FS_TYPE_PROGFS;
	buf->vfs_bksize = 1;
	buf->vfs_size = 0;
	buf->vfs_free = 0;

	return 0;
}

int8_t stat(const char *path, struct stat *buf)
{
	int8_t ret;
	char *pathok;

	if ((pathok = sanitize_path(path)) == NULL)
		return EINVNAME;

	// TODO: virtual filesystem structure in process
	ret = progfs_stat(pathok, buf);

	free(pathok);

	return ret;
}

// Open a directory for reading. 
int8_t opendir(const char *path, DIR *d)
{
	int8_t ret;
	char *pathok;

	if ((pathok = sanitize_path(path)) == NULL)
		return EINVNAME;

	// TODO: virtual filesystem structure in process
	ret = progfs_opendir(pathok, d);

	free(pathok);

	return ret;
}

// Close a directory previously opened
int8_t closedir(DIR *dirp)
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

int8_t open(const char *path, uint8_t flags, FD *fd)
{
	int8_t ret;
	char *pathok;

	if ((pathok = sanitize_path(path)) == NULL)
		return EINVNAME;

	// TODO: virtual filesystem structure in process
	ret = progfs_open(pathok, flags, fd);

	free(pathok);

	return ret;
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

int8_t fstat(FD *fd, struct stat *buf)
{
	// TODO: virtual filesystem structure in process
	return progfs_fstat(fd, buf);
}

int8_t chdir(const char *path)
{
	int8_t ret;
	char *pathok;

	if ((pathok = sanitize_path(path)) == NULL)
		return EINVNAME;

	// TODO: virtual filesystem structure in process
	ret = progfs_chdir(pathok);

	free(pathok);

	return ret;

}
