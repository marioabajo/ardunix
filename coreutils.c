#include "coreutils.h"
#include "kernel.h"
#include "fs.h"
#include <stdio.h>
#include <string.h>

void ls_print_entry(uint8_t f, uint16_t size, const char *entry_name)
{
	char c = '?';
	char r = '-', w = '-', x = '-';

	switch(f & FS_MASK_FILETYPE)
	{
		case FS_FILE:  // File
			c = '-';
			break;
		case FS_DIR: // Directory
			c = 'd';
			break;
		case FS_LINK: // Link
			c = 'l';
			break;
		case FS_DEV: // Device
			c = 'c'; // TODO: Another case for each type of dev?
	}

	if (f & FS_READ)
		r = 'r';
	if (f & FS_WRITE)
		w = 'w';
	if (f & FS_EXEC)
		x = 'x';
	printf_P(PSTR("%c%c%c%c %4d %s\n"), c, r, w, x, size, entry_name);
}

// list files and directories
int8_t main_ls(char *argv[])
{
	DIR dir;
	char *fn, *cwd;
	struct stat obj;
	struct dirent *entry;
	int8_t ret = 0;
	uint8_t i = 1, _continue = 1;

	cwd = getcwd(NULL, 0);
	fn = cwd;
	do
	{
		if (argv[i] == NULL)
		{
			if (i == 1)
				_continue = 0;
			else
				break;
		}
		else
			fn = argv[i];

		if ((ret = stat(fn, &obj)))
			break;;
	
		if (obj.st_mode & FS_DIR)
		{
			if ((ret = opendir(fn, &dir)))
				break;

			ls_print_entry(dir.dd_ent.flags, dir.dd_ent.size, ".");
			while ((entry = readdir(&dir)) != NULL)
				ls_print_entry(entry->flags, entry->size, entry->d_name);
		}
		else
			ls_print_entry(obj.st_mode, obj.st_size, fn);
		
		i++;
	} while (_continue);

	free(cwd);
	return ret;
}

int8_t main_ps(void)
{
	int8_t i;

	puts_P(PSTR("PID  STATUS  NAME"));
	for (i=0; i < PID_MAX; i++)
	{
		// TODO: userland should not access process structure directly
		if (procs[i].state)
			printf_P(PSTR("%3d  %d     %s\n"), i, procs[i].state, procs[i].name);
	}

	return 0;
}

int8_t main_pwd(void)
{
	char buf[PATH_MAX];
	
	if ((getcwd(buf, PATH_MAX)) == NULL)
		return 1;

	puts(buf);
	return 0;
}

// Print free ram
int8_t main_free (void)
{
  size_t HEAPSTART = (size_t) &__heap_start;
  size_t HEAPEND = (__brkval == 0 ? HEAPSTART : (size_t) __brkval);
  int total = 0;

  printf_P(PSTR("total=%d data=%d heap=%d stack=%d free=%d\n"), RAMEND - RAMSTART, HEAPSTART - RAMSTART, 
           HEAPEND - HEAPSTART, RAMEND - (size_t) &total, (size_t) &total - HEAPEND + total);

  return 0;
}

int8_t main_times (char *argv[])
{
  unsigned long t1;
  int8_t i = 0;
  
  t1=millis();
  if (argv[1] != NULL)
  {
    //TODO: pass env to execve
    //FIXME: look like it's not passing correctly the parameters to the command
    i = execve((const char **) &argv[1], NULL);
  }
  printf_P(PSTR("millis: %ld\n"), millis() - t1);
  return i;
}

// list environment variables
int8_t main_set(char *argv[], char *env[])
{
  uint8_t i = 0;

  while (i < ENV_MAX)
  {
    if (env[i] != NULL)
      printf_P(PSTR("%s\n"), env[i]);
    i++;
  }

  return 0;
}

int8_t main_true()
{
  return 0;
}

int8_t main_false()
{
  return 1;
}

int8_t main_cat(char *argv[])
{
  FD fd;
  char c;
  
  if (argv[1] == NULL)
    return 0;
    
  if ((open(argv[1], 0, &fd)) != 0)
    return 1;

  while (read(&fd, &c, 1) != 0)
    putchar(c);

  return 0;
}

int8_t main_echo(char *argv[])
{
  uint8_t i = 1;

  while (argv[i] != NULL)
  {
    printf_P(PSTR("%s"), argv[i]);
    if (argv[i + 1] != NULL)
      printf_P(PSTR(" "));
    i++;
  }
  printf_P(PSTR("\n"));
  
  return 0;
}

void debug_dump(uint16_t src, const void *ptr, size_t msize)
{
  size_t i, j;
  uint8_t *p, d;

  printf_P(PSTR("Size: 0x%x\n"), msize);
  for (i=0; i < msize; i+=16)
  {
    p = (uint8_t *) ptr + i;
    printf_P(PSTR("0x%x: "), src + i);
    for (j=0; j < 32; j++)
    {
      d = *(p + (j & 0xf));
      if (j < 16)
        printf_P(PSTR("%x%x "), d >> 4, d & 0x0F);
      else
        printf_P(PSTR("%c"), (d < 32) ? '.' : d);
    }
    printf_P(PSTR("\n"));
  }
}

int8_t main_debug(char *argv[])
{

  size_t marker;
  int msize = 0;
  size_t HEAPSTART = (size_t) &__heap_start;
  size_t HEAPEND = (__brkval == 0 ? HEAPSTART : (size_t) __brkval);
  size_t DATAEND = HEAPSTART;
  size_t DATASIZE = DATAEND - RAMSTART;
  uint8_t i = 1;

  if (argv[i] == NULL)
    goto help;

  while (argv[i] != NULL)
  {
    if (argv[i][0] != '-' || argv[i][2] != 0)
      goto error;

    switch(argv[i][1])
    {
      case 'h':
        printf_P(PSTR("Debug help:\n -d  dump data memory\n -s  print stack\n -H  dump heap\n"));
        break;
      case 'd':
        debug_dump((size_t) RAMSTART, (void*) RAMSTART, DATASIZE);
        break;
      case 's':
        msize = RAMEND - (size_t) &marker + 1;
        debug_dump((size_t) &marker, (void*) &marker, msize);
        /* kind of backtrace, but doesn't work as expected 
        for (i=0; i<2; i++)
          printf_P(PSTR("0x%x\n"), __builtin_return_address(i));*/
        break;
      case 'H':
        msize = HEAPEND - HEAPSTART;
        debug_dump((size_t) HEAPSTART, (void*) HEAPSTART, msize);
        break;
      default:
        goto error;
    }
    i++;
  }
  return 0;
error:
  printf_P(PSTR("Unknown param: \"%s\"\n"), argv[i]);
help:
  printf_P(PSTR("Use -h to print help\n"));
  return 1;
}


