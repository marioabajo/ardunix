#include "progfs.h"
#include "sh.h"
#include <stdio.h>

// file system definition
const char PROGMEM issue[] = "Ardunix 0.5 beta (2017/10/13)\n";
const char PROGMEM script[] = "#!/bin/sh\n\nif /bin/false\nthen\n  /bin/ls /bin\nelse\n  /bin/ls /etc\nfi\n";

const char PROGMEM _[] = "/\0";
const char PROGMEM _bin[] = "bin";
const char PROGMEM _bin_ls[] = "ls";
const char PROGMEM _bin_sh[] = "sh";
const char PROGMEM _bin_free[] = "free";
const char PROGMEM _bin_times[] = "times";
const char PROGMEM _bin_set[] = "set";
const char PROGMEM _bin_true[] = "true";
const char PROGMEM _bin_false[] = "false";
const char PROGMEM _bin_cat[] = "cat";
const char PROGMEM _bin_echo[] = "echo";
const char PROGMEM _bin_debug[] = "debug";
const char PROGMEM _etc[] = "etc";
const char PROGMEM _etc_issue[] = "issue";
const char PROGMEM _etc_test[] = "test";
const char PROGMEM _etc_test_empty[] = "empty";
const char PROGMEM _etc_script[] = "script";

const PFS2 ProgFs2[] = \
{{_,               NULL,               FS_DIR | FS_EXEC | FS_READ, 0},\
 {_bin,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0},\
 {_bin_ls,         (void *)main_ls,    FS_EXEC | FS_READ,          0},\
 {_bin_sh,         (void *)main_sh,    FS_EXEC | FS_READ,          0},\
 {_bin_free,       (void *)main_free,  FS_EXEC | FS_READ,          0},\
 {_bin_times,      (void *)main_times, FS_EXEC | FS_READ,          0},\
 {_bin_set,        (void *)main_set,   FS_EXEC | FS_READ,          0},\
 {_bin_true,       (void *)main_true,  FS_EXEC | FS_READ,          0},\
 {_bin_false,      (void *)main_false, FS_EXEC | FS_READ,          0},\
 {_bin_cat,        (void *)main_cat,   FS_EXEC | FS_READ,          0},\
 {_bin_echo,       (void *)main_echo,  FS_EXEC | FS_READ,          0},\
 {_bin_debug,      (void *)main_debug, FS_EXEC | FS_READ,          0},\
 {0,               0,                  0,                          0},\
 {_etc,            NULL,               FS_DIR | FS_EXEC | FS_READ, 0},\
 {_etc_issue,      (void *)issue,      FS_READ,                    sizeof(issue)},\
 {_etc_test,       NULL,               FS_DIR | FS_EXEC | FS_READ, 0},\
 {_etc_test_empty, NULL,               FS_EXEC | FS_READ,          0},\
 {0,               0,                  0,                          0},\
 {_etc_script,     (void *)script,     FS_EXEC | FS_READ,          sizeof(script)},\
 {0,               0,                  0,                          0},\
{0, 0, 0, 0}};

// end of filesystem definition
