#ifndef FS_H
#define FS_H

#include "types.h"
#include "defines.h"

struct fsentry
{
	//const u8 inode;
  const u8 parent_inode;
	const char *filename;
	const void (*data)(u8 argc, char *argv[]);  // Points to function in memory
	u8 flags;  /* bits -> 87654321
                             RWX
                        \_________ Special file / normal file-directory
                         \________ File/Directory
	              bits -> 12345678  Type= 00000 File
		                    RWX Type        01000 Directory
		                                    10000 Especial file
		                                    11000  |- Gpio
		   */
};

#ifdef __cplusplus
extern "C"{
#endif
  // Open a directory for reading. 
  u8 opendir(const char* path);
#ifdef __cplusplus
}
#endif
/*
getattr(const char* path, struct stat* stbuf)
    Return file attributes. The "stat" structure is described in detail in the stat(2) manual page. For the given pathname, this should fill in the elements of the "stat" structure. If a field is meaningless or semi-meaningless (e.g., st_ino) then it should be set to 0 or given a "reasonable" value. This call is pretty much required for a usable filesystem. 
fgetattr(const char* path, struct stat* stbuf)
    As getattr, but called when fgetattr(2) is invoked by the user program. 
access(const char* path, mask)
    This is the same as the access(2) system call. It returns -ENOENT if the path doesn't exist, -EACCESS if the requested permission isn't available, or 0 for success. Note that it can be called on files, directories, or any other object that appears in the filesystem. This call is not required but is highly recommended. 
readlink(const char* path, char* buf, size_t size)
    If path is a symbolic link, fill buf with its target, up to size. See readlink(2) for how to handle a too-small buffer and for error codes. Not required if you don't support symbolic links. NOTE: Symbolic-link support requires only readlink and symlink. FUSE itself will take care of tracking symbolic links in paths, so your path-evaluation code doesn't need to worry about it. 
opendir(const char* path, struct fuse_file_info* fi)
    Open a directory for reading. 
readdir(const char* path, void* buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info* fi)
    Return one or more directory entries (struct dirent) to the caller. This is one of the most complex FUSE functions. It is related to, but not identical to, the readdir(2) and getdents(2) system calls, and the readdir(3) library function. Because of its complexity, it is described separately below. Required for essentially any filesystem, since it's what makes ls and a whole bunch of other things work. 
open(const char* path, struct fuse_file_info* fi)
    Open a file. If you aren't using file handles, this function should just check for existence and permissions and return either success or an error code. If you use file handles, you should also allocate any necessary structures and set fi->fh. In addition, fi has some other fields that an advanced filesystem might find useful; see the structure definition in fuse_common.h for very brief commentary. 
read(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
    Read size bytes from the given file into the buffer buf, beginning offset bytes into the file. See read(2) for full details. Returns the number of bytes transferred, or 0 if offset was at or beyond the end of the file. Required for any sensible filesystem. 
write(const char* path, char *buf, size_t size, off_t offset, struct fuse_file_info* fi)
    As for read above, except that it can't return 0. 
releasedir(const char* path, struct fuse_file_info *fi)
    This is like release, except for directories. 
bmap(const char* path, size_t blocksize, uint64_t* blockno)
    This function is similar to bmap(9). If the filesystem is backed by a block device, it converts blockno from a file-relative block number to a device-relative block. It isn't entirely clear how the blocksize parameter is intended to be used. 
ioctl(const char* path, int cmd, void* arg, struct fuse_file_info* fi, unsigned int flags, void* data
    Support the ioctl(2) system call. As such, almost everything is up to the filesystem. On a 64-bit machine, FUSE_IOCTL_COMPAT will be set for 32-bit ioctls. The size and direction of data is determined by _IOC_*() decoding of cmd. For _IOC_NONE, data will be NULL; for _IOC_WRITE data is being written by the user; for _IOC_READ it is being read, and if both are set the data is bidirectional. In all non-NULL cases, the area is _IOC_SIZE(cmd) bytes in size. 
poll(const char* path, struct fuse_file_info* fi, struct fuse_pollhandle* ph, unsigned* reventsp);
    Poll for I/O readiness. If ph is non-NULL, when the filesystem is ready for I/O it should call fuse_notify_poll (possibly asynchronously) with the specified ph; this will clear all pending polls. The callee is responsible for destroying ph with fuse_pollhandle_destroy() when ph is no longer needed. 
 */

/*#define _FILE (const void (*)(u8, char**))
#define ROOT 0
#define BIN 1
#define DEV 2

extern _FILE ls;
extern _FILE freeMem;

// TODO: dynamize this structure at compile time
const struct fsentry fs[]={
  // root
  {ROOT,"/",0,0x45},
  
  // first level directories
  {ROOT,"bin",0,0x45},
  {ROOT,"dev",0,0x45},

  // files
  {BIN, "ls", _FILE ls, 0x5},
  {BIN, "free", _FILE freeMem, 0x5}
};

#define fsentries ARRAY_SIZE(fs)
*/

extern const struct fsentry fs[];
#define fsentries ARRAY_SIZE(fs)
 
#endif

