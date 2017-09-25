#include "support.h"

uint8_t filename_plus_args_null_terminated_to_argv_conv(const char *filename, const char *argv[], char *argv2[NCARGS])
{
  uint8_t i = 0;
  
  argv2[0] = (char *)filename;
  if (argv!=NULL)
    for (i=0; argv[i]!=NULL; i++)
    {
      if (i >= NCARGS - 1)
        return 255; // too many args

      argv2[i+1] = (char *)argv[i];
    }
    
  return i;
}
