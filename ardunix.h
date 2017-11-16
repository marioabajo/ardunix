#ifndef ARDUNIX_H
#define ARDUNIX_H

#include "progfs.h"
#include "sh.h"
#include <stdio.h>

#define PROGFS_ENTRY(PATHNAME, FUNC, FLAGS, SIZE) {PATHNAME, (void *)FUNC, FLAGS, SIZE},
#define ARDUNIX_VER "0.5 beta"
#define ARDUNIX_DATE "2017/10/30"

// sh.h
#if !defined CMD_SH || CMD_SH == true
const char PROGMEM _bin_sh[] = "sh";
#define PROGFS_ENT__BIN_SH    PROGFS_ENTRY(_bin_sh, main_sh, FS_EXEC | FS_READ, 0)
#endif

#define PROGFS_ENT_SH  PROGFS_ENT__BIN_SH

// coreutils.h
#if !defined CMD_LS || CMD_LS == true
const char PROGMEM _bin_ls[] = "ls";
#define PROGFS_ENT__BIN_LS    {_bin_ls,         (void *)main_ls,    FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_CD || CMD_CD == true
const char PROGMEM _bin_cd[] = "cd";
#define PROGFS_ENT__BIN_CD    {_bin_cd,         (void *)main_cd,    FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_FREE || CMD_FREE == true
const char PROGMEM _bin_free[] = "free";
#define PROGFS_ENT__BIN_FREE  {_bin_free,       (void *)main_free,  FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_TIMES || CMD_TIMES == true
const char PROGMEM _bin_times[] = "times";
#define PROGFS_ENT__BIN_TIMES {_bin_times,      (void *)main_times, FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_SET || CMD_SET == true
const char PROGMEM _bin_set[] = "set";
#define PROGFS_ENT__BIN_SET   {_bin_set,        (void *)main_set,   FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_TRUE || CMD_TRUE == true
const char PROGMEM _bin_true[] = "true";
#define PROGFS_ENT__BIN_TRUE  {_bin_true,       (void *)main_true,  FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_FALSE || CMD_FALSE == true
const char PROGMEM _bin_false[] = "false";
#define PROGFS_ENT__BIN_FALSE {_bin_false,      (void *)main_false, FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_CAT || CMD_CAT == true
const char PROGMEM _bin_cat[] = "cat";
#define PROGFS_ENT__BIN_CAT   {_bin_cat,        (void *)main_cat,   FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_ECHO || CMD_ECHO == true
const char PROGMEM _bin_echo[] = "echo";
#define PROGFS_ENT__BIN_ECHO  {_bin_echo,       (void *)main_echo,  FS_EXEC | FS_READ,          0}
#endif

#if !defined CMD_DEBUG || CMD_DEBUG == true
const char PROGMEM _bin_debug[] = "debug";
#define PROGFS_ENT__BIN_DEBUG {_bin_debug,      (void *)main_debug, FS_EXEC | FS_READ,          0}
#endif

#define PROGFS_ENT_COREUTILS   PROGFS_ENT__BIN_LS, \
  PROGFS_ENT__BIN_CD, \
  PROGFS_ENT__BIN_FREE, \
  PROGFS_ENT__BIN_TIMES, \
  PROGFS_ENT__BIN_SET, \
  PROGFS_ENT__BIN_TRUE, \
  PROGFS_ENT__BIN_FALSE, \
  PROGFS_ENT__BIN_CAT, \
  PROGFS_ENT__BIN_ECHO, \
  PROGFS_ENT__BIN_DEBUG,


// file system definition
const char PROGMEM _[] = "/\0";
const char PROGMEM _bin[] = "bin";
const char PROGMEM _etc[] = "etc";
const char PROGMEM _etc_issue[] = "issue";
const char PROGMEM issue[] = "Ardunix " ARDUNIX_VER " (" ARDUNIX_DATE ")\n";

#define DEF_PROGFS \
const PFS2 ProgFs2[] = \
{{_,               NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
 {_bin,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
  PROGFS_ENT_SH \
  PROGFS_ENT_COREUTILS \
  PROGFS_ENT_BIN_EXTRAS \
 {0,               0,                  0,                          0}, \
 {_etc,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0}, \
 {_etc_issue,      (void *)issue,      FS_READ,                    sizeof(issue)}, \
 PROGFS_ENT_ETC_EXTRAS \
 {0,               0,                  0,                          0}, \
{0, 0, 0, 0}};

// end of filesystem definition

#endif

