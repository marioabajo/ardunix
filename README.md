# ardunix

A shell prototype for arduino for extremely low memory devices (like arduinos with 2kb of ram). It can't be posix compliant in any way, but i try to make it familiar in some sense.
Objectives for v.1:

- Low memory foot print at start (below 512 bytes of ram, and 9kb of flash)
- Shell with variables, very basic control flow (for, while, if) and calls
- Simple filesystem in flash (PROGMEM) and another one for eeprom (RW)
- Simple interface with the shell (using read, write calls)
- Support for automatically run commands at boot

## Commands

- sh
- ls
- free
- times
- true
- false
- cat
- echo
- debug
- set

## Api

### kernel

- execve
    Execute a file, with its environment
- exec
    The same as execve but without environment

### fs

- opendir
    Open a directory for reading the contents
- readdir
    Read next directory entry (call opendir first)
- closedir
    Close directory open by opendir call (needed to free memory used)
- rewinddir
    Point to the first element of the directory in a dir opened by opendir
- stat
    Check access to a file, return pointer to it
- open
    open a file, no close needed as all the information is in a variable used
    by the function calling open
- read
    load in a buffer a number of defined bytes from a file

