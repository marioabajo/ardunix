# ardunix

A shell prototype for arduino for extremely low memory devices (like arduinos with 2kb of ram). It can't be posix compliant in any way, but i try to make it familiar in some sense.
Objectives for v.1:

- Low memory foot print at start (below 512 bytes of ram, and 5kb of flash)
- Shell with variables, very basic control flow (for, while, if) and calls
- Simple filesystem in flash (PROGMEM) and another one for eeprom (RW)
- Simple interface with the shell (using read, write calls)
- Support for automatically run commands at boot

## Commands

- sh
- ls
- free
- times

## Api

### kernel

- execve
    Execute a file, with its environment

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

