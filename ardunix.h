#ifndef ARDUNIX_H
#define ARDUNIX_H

#include "progfs.h"
#include "sh.h"
#include "coreutils.h"
#include "kernel.h"
#include <stdio.h>

#define ARDUNIX_VER "0.5.1"
#define ARDUNIX_DATE "2019/03/03"

#define DEBUG 0

// sh.h
#if !defined CMD_SH
#define CMD_SH true
#endif
#if CMD_SH == true
const char PROGMEM _bin_sh[] = "sh";
#define PROGFS_ENT__BIN_SH    PROGFS_ENTRY(_bin_sh, main_sh, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_SH
#endif

#define PROGFS_ENT_SH  PROGFS_ENT__BIN_SH

// coreutils.h
#if !defined CMD_LS
#define CMD_LS true
#endif
#if CMD_LS == true
const char PROGMEM _bin_ls[] = "ls";
#define PROGFS_ENT__BIN_LS    PROGFS_ENTRY(_bin_ls, main_ls, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_LS
#endif

#if !defined CMD_PS
#define CMD_PS true
#endif
#if CMD_PS == true
const char PROGMEM _bin_ps[] = "ps";
#define PROGFS_ENT__BIN_PS    PROGFS_ENTRY(_bin_ps, main_ps, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_PS
#endif

#if !defined CMD_PWD
#define CMD_PWD true
#endif
#if CMD_PWD == true
const char PROGMEM _bin_pwd[] = "pwd";
#define PROGFS_ENT__BIN_PWD    PROGFS_ENTRY(_bin_pwd, main_pwd, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_PWD
#endif

#if !defined CMD_FREE
#define CMD_FREE true
#endif
#if CMD_FREE == true
const char PROGMEM _bin_free[] = "free";
#define PROGFS_ENT__BIN_FREE  PROGFS_ENTRY(_bin_free, main_free, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_FREE
#endif

#if !defined CMD_TIMES
#define CMD_TIMES true
#endif
#if CMD_TIMES == true
const char PROGMEM _bin_times[] = "times";
#define PROGFS_ENT__BIN_TIMES PROGFS_ENTRY(_bin_times, main_times, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_TIMES
#endif

#if !defined CMD_SET
#define CMD_SET true
#endif
#if CMD_SET == true
const char PROGMEM _bin_set[] = "set";
#define PROGFS_ENT__BIN_SET   PROGFS_ENTRY(_bin_set, main_set, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_SET
#endif

#if !defined CMD_TRUE
#define CMD_TRUE true
#endif
#if CMD_TRUE == true
const char PROGMEM _bin_true[] = "true";
#define PROGFS_ENT__BIN_TRUE  PROGFS_ENTRY(_bin_true, main_true, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_TRUE
#endif

#if !defined CMD_FALSE
#define CMD_FALSE true
#endif
#if CMD_FALSE == true
const char PROGMEM _bin_false[] = "false";
#define PROGFS_ENT__BIN_FALSE PROGFS_ENTRY(_bin_false, main_false, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_FALSE
#endif

#if !defined CMD_CAT
#define CMD_CAT true
#endif
#if CMD_CAT == true
const char PROGMEM _bin_cat[] = "cat";
#define PROGFS_ENT__BIN_CAT   PROGFS_ENTRY(_bin_cat, main_cat, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_CAT
#endif

#if !defined CMD_ECHO
#define CMD_ECHO true
#endif
#if CMD_ECHO == true
const char PROGMEM _bin_echo[] = "echo";
#define PROGFS_ENT__BIN_ECHO  PROGFS_ENTRY(_bin_echo, main_echo, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_ECHO
#endif

#if !defined CMD_DEBUG
#define CMD_DEBUG true
#endif
#if CMD_DEBUG == true
const char PROGMEM _bin_debug[] = "debug";
#define PROGFS_ENT__BIN_DEBUG PROGFS_ENTRY(_bin_debug, main_debug, FS_EXEC | FS_READ, 0)
#else
#define PROGFS_ENT__BIN_DEBUG
#endif

#define PROGFS_ENT_COREUTILS   PROGFS_ENT__BIN_LS \
  PROGFS_ENT__BIN_PS \
  PROGFS_ENT__BIN_PWD \
  PROGFS_ENT__BIN_FREE \
  PROGFS_ENT__BIN_TIMES \
  PROGFS_ENT__BIN_SET \
  PROGFS_ENT__BIN_TRUE \
  PROGFS_ENT__BIN_FALSE \
  PROGFS_ENT__BIN_CAT \
  PROGFS_ENT__BIN_ECHO \
  PROGFS_ENT__BIN_DEBUG


// file system definition
const char PROGMEM _[] = "/";
const char PROGMEM _bin[] = "bin";
const char PROGMEM _dev[] = "dev";
const char PROGMEM _etc[] = "etc";
const char PROGMEM _etc_issue[] = "issue";
const char PROGMEM issue[] = "Ardunix " ARDUNIX_VER " (" ARDUNIX_DATE ")\n";

PROGFS_PLATFORM_PREPARE

#define INIT_PROGFS \
const PFS2 ProgFs2[] = \
{{_,               NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
 {_bin,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
  PROGFS_ENT_SH \
  PROGFS_ENT_COREUTILS \
  PROGFS_ENT_BIN_EXTRAS \
 {0,               0,                  0,                          0}, \
 {_dev,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
  PROGFS_ENT_DEV \
 {0,               0,                  0,                          0}, \
 {_etc,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
 {_etc_issue,      (void *)issue,      FS_READ,                    sizeof(issue)}, \
 PROGFS_ENT_ETC_EXTRAS \
 {0,               0,                  0,                          0}, \
{0, 0, 0, 0}};

// end of filesystem definition

#define INIT_ARDUNIX \
  INIT_TTY\
  init_proc();\
  execl_P(PSTR("cat"), PSTR("/etc/issue"), 0);


#endif
