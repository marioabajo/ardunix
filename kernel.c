#include "kernel.h"
#include "env.h"
#include "progfs.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

struct proc procs[PID_MAX];
int8_t current_proc = 0; // the kernel uses the PID = 0 to start other processes

static int8_t proc_allocate(void)
{
	int8_t i = 1;

	// Find an available pid
	while (procs[i].state != 0)
	{
		i++;
		if (i>= PID_MAX)
			return ENOPID;
	}
	
	procs[i].state = 1;
	return i;
}

static void proc_clean(int8_t pid)
{
	procs[pid].state = 0;
	free(procs[pid].name);
	procs[pid].ppid = 0;
	procs[pid].name = NULL;
	procs[pid].cwd = NULL;
}

static int8_t proc_start(const char *name)
{
	uint8_t len;
	int8_t pid;

	// Allocate a proccess id
	if ((pid = proc_allocate()) == ENOPID)
		return ENOPID;

	// fill process data
	len = strlen(name);
	if ((procs[pid].name = malloc(len)) == NULL)
		return ENOMEM;
	memcpy(procs[pid].name, name, len);
	procs[pid].name = NULL;
	procs[pid].ppid = current_proc;
	procs[pid].cwd = procs[current_proc].cwd;
	procs[pid].inherited.cwd = 1;

	// change running status of the current process
	procs[current_proc].state = PROC_STOP;
	current_proc = pid;
	procs[pid].state = PROC_RUN;

	return 0;
}

static void proc_stop(void)
{
	int8_t pid = current_proc;
	
	// free process
	current_proc = procs[pid].ppid;
	proc_clean(pid);
	procs[current_proc].state = PROC_RUN;
}

int8_t init_proc(void)
{
	uint8_t i;
	
	// clean proc array execpt pid 0
	for (i=1; i < PID_MAX; i++)
		proc_clean(i);

	if ((procs[0].cwd = malloc(PATH_MAX)) == NULL)
		return ENOMEM;
	
	// fill pid 0 data
	procs[0].state = PROC_RUN;
	procs[0].name = NULL;
	procs[0].cwd[0] = '/';
	procs[0].cwd[1] = 0;
	procs[0].inherited.cwd = 0;
	
	return 0;
}

int8_t execl_P(const PROGMEM char *argv, ...)
{
  char *arglist[NCARGS], *p, *str;
  uint8_t len, ret = 0;
  int8_t i = 0;
  va_list va;

  if(argv == NULL)
    return 0;

  p = (char *) argv;

  va_start(va, argv);

  do
  {
    len = strlen_P(p);
    if ((str = malloc(len + 1)) == NULL)
    {
      ret = ENOMEM;
      goto finish;
    }
    strncpy_P(str, p, len);
    str[len] = 0;
    arglist[i++] = str;    
  }while (i < NCARGS && (p = va_arg(va, char *)) != NULL);

  va_end(va);

  arglist[i] = NULL;

  ret = execve((const char **)arglist, NULL);

finish:
  // free the allocated blocks
  for (i--; i>=0; i--)
    free(arglist[i]);

  return ret;

}

int8_t execl(const char *argv, ...)
{
  char *arglist[NCARGS], *p;
  uint8_t i = 0;
  va_list va;

  if(argv == NULL)
    return 0;

  arglist[i++] = (char *) argv;
  
  va_start(va, argv);

  while (i < NCARGS && (p = va_arg(va, char *)) != NULL)
    arglist[i++] = p;
    
  va_end(va);

  arglist[i] = NULL;
  
  return execve((const char **) arglist, NULL);
}

int8_t exec(const char *argv[])
{
  return execve(argv, NULL);
}

static uint8_t insert_interpreter(FD fd, char *argv[])
{
	uint8_t i=0;
	char aux[PATH_MAX];

	read(&fd, &aux, (uint8_t)PATH_MAX);
	// trim the string if we found a \n, \r or space
	while (i<PATH_MAX && aux[i] != ' ' && aux[i] != '\r' && aux[i] != '\n')
		i++;

	// end the interpreter string with a 0
	aux[i++] = 0;
	
	// modify argv, so put first the interpreter, and second this script 
	// filename
	argv[1] = argv[0];
	argv[2] = NULL;
	argv[0] = malloc(i);
	if (argv[0] == NULL)
		return ENOMEM; // not enough memory

	// copy the string to the new memory location
	memcpy((char *)argv[0], aux, i);
	return 0;
}

// NOTE: force noinline in order to save memory in the stack
static int8_t __attribute__((noinline)) check_file(char *argv[], long *inode)
/* Checks if the given file is valid for execution and return the inode and
 * if it's a script. It also modifies argv in case of a script appending the
 * interpreter
 * 
 * Input: array with arguments NULL terminated,
 * Output: inode number
 * Returns: 1         -> ok, it's a script
 *          0         -> ok, builtin command
 *          EINVNAME  -> invalid filename
 *          ENOTFOUND -> file not found
 *          EPERM     -> bad attributes
 *          EINVFS    -> invalid filesystem
 *          ENOTEXEC  -> not a valid executable
 *          E2BIG     -> argument list too long
 *          ENOMEM    -> not enough memory
 */
{
	char *pathok, aux[2];
	uint8_t ret;
	FD fd;
	struct stat file;
	struct statvfs fs;

	// Check that the argument is not empty / null
	if (argv == NULL || argv[0] == NULL)
		return EINVNAME; // filename invalid

	// Verify the executable path, if it's a relative path, add PATH to it
	if (argv[0][0] == '/')
	{
		if ((pathok = normalize_path(argv[0])) == NULL)
			return EINVNAME;
	}
	else
	{
		if ((pathok = normalize_paths(PATH, argv[0])) == NULL)
			return EINVNAME;
	}

	if (open(pathok, 0, &fd) != 0)
		return ENOTFOUND; // file not found

	// free the variable name
	free(pathok);
  
	// Check that has the correct permissions 
	fstat(&fd, &file);
	if (!(file.st_mode & FS_EXEC))
		return EPERM; // bad permissions
	*inode = file.st_ino;

	// Check if its a script
	read(&fd, aux, (uint8_t) 2);
	if (aux[0] == '#' && aux[1] == '!')
	{
		// modify argv to include the interpreter and parameters
		ret = insert_interpreter(fd, argv);
		// passtrougt the error
		if (ret)
			return ret;
		return 1; // it's a script, call the interpreter
	}

	// get the filesystem type of the filesystem of this file
	if (fstatvfs(&fd, &fs) != 0)
		return EINVFS; // kernel error, filesystem not found!?

	// up to this point, the file is executable, accesible and not a script
	// so it must be a builtin command to be called or an error
	if (fs.vfs_fstype == FS_TYPE_PROGFS)
		return 0;

	return ENOTEXEC; // Not a valid executable
	
}

int8_t execve(const char *argv[], char *envp[])
/* Executes a program (function) given in the first parameter of argv
 * 
 * Input: array with arguments NULL terminated, 
 *        array with environemnt variables NULL terminated
 * Returns: >=0 -> The return code of the program runned
 *          -1  -> filename invalid
 *          -2  -> could not open file
 *          -3  -> file not found
 *          -4  -> bad permissions
 *          -5  -> invalid filesystem
 *          -6  -> not enough memory
 */
{
	int8_t  ret, cleanenv = 0;
	long    inode;

	// Check file path and permissions
	ret = check_file((char **)argv, &inode);

	// If it's a script, call the interpreter
	if (ret == 1)
	{
		// call the interpreter
		ret = execve(argv, envp);
		// free the string reserved for the interpreter
		free((char *)argv[0]);
	}
	// If it's a builtin function, call it
	else if (ret == 0)
	{
		// If environemnt doen't exist, create one and destroy at exit
		if (envp == NULL)
		{
			// alloc a new env
			if ((envp = malloc(ENV_MAX * sizeof(char *))) == NULL)
				return ENOMEM; // cannot allocate memory
			cleanenv = 1;
			// clean env
			memset(envp, 0, ENV_MAX * sizeof(char *));
		}

		// prepare process structure and start it
		if ((ret = proc_start(argv[0])) != 0)
			return ret;

		// Run the builtin command
		ret = ((int8_t (*)(const char *argv[], char *envp[])) \
		      pgm_read_ptr(&(ProgFs2[inode].ptr)))(argv, envp);

		// free the environemnt if it has been created during execution
		if (cleanenv)
			envp = env_free(envp);
		
		proc_stop();
	}

	return ret;
}

