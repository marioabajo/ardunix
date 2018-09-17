#ifndef PLATFORM_H
#define PLATFORM_H

/* Common
 *
 */
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
/* stdint defines FILENAME_MAX with 4096 by default in most architectures, 
 * we need to redefine it to our value no avoid stack corruption
 */
#include <stdio.h>
#include <limits.h>
#undef FILENAME_MAX
#undef ARGMAX
#undef PATH_MAX
#undef NCARGS
#include <setjmp.h>

/*
 *  System parameters
 */

// ARGMAX never bigger than 256, uint8_t used as index
#define ARGMAX 80       // max input line size
#define FILENAME_MAX 16 // filename max size
#define PATH_MAX 64     // path max size
#define NCARGS 16       // max number of parameters
#define ENV_MAX 16      // max number of environment variables
#define PID_MAX 3       // max number of processes (proces 0 reserved for the scheduler)
#define IFS ' '         // inter field separator
// TODO: just one path at this moment
#define PATH "/bin"     // fixed path
#define HOME "/"        // fixed home

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

/*
 *  Error codes
 */

#define EINVNAME  -1   // filename invalid
#define ECANTOPEN -2   // could not open file
#define ENOTFOUND -3   // file not found
#define EPERM     -4   // invalid permissions
#define EINVFS    -5   // Invalid filesystem
#define ENOMEM    -6   // not enought memory
#define ENOTEXEC  -7   // not executable
#define E2BIG     -8   // arg list too long
#define ENOPID    -9   // no more pid available
#define ENAMETOOLONG -10 // filename too long
#define EADDRNOTAVAIL -11 // Address not available
#define ENOTDIR   -12  // Not a directory
#define EISDIR    -13  // Is a directory

/*
 *  Arduino platform
 */

#ifdef __AVR__
  #include <avr/pgmspace.h>
  #include "platform_avr.h"

/*
 * PC platform
 */
#elif defined(__x86_64__) || defined(__i386__)
  #include "platform_x86.h"
#endif

#endif
