#include "fs.h"

// Open a directory for reading. 
u8 opendir(const char* path)
{
  u8 dnode=0;
  u8 i=1,j=1,k;

  // TODO: only absolute paths allowed for now
  // Empty paths not allowed
  if (path[0]!='/' || path[0]==0)
    return -1;

  // root
  if (path[0]=='/' && path[1]==0)
    return 0;

  while (path[j]!=0)
  {
    // split path by "/"
    do { j++; } while (path[j]!='/' && path[j]!=0);
    /*printf("DIR: %s\n", path+i);
    printf("D: %d %d\n",i,j);*/
    k=1;
    while (k<fsentries)
    {
      if (fs[k].parent_inode==dnode && 
          fs[k].flags & 0x40 && 
          strncmp(fs[k].filename,path+i,j-i))
      {
        dnode=k;
        break;
      }
      k++;
    }
    if (k==fsentries)
      return -1;
    i=j+1;
  }
  // look for directory inode
  // TODO: split directories in path
  /*while (dnode==0 && i<fsentries)
  {
    if (!strcmp(fs[i].filename, PATH))
      dnode=i;
    i++;
  }*/
}

