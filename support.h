#ifndef SUPPORT_H
#define SUPPORT_H

#include "platform.h"

#ifdef __cplusplus
extern "C" {
#endif
    uint8_t filename_plus_args_null_terminated_to_argv_conv(const char *filename, const char *argv[], char *argv2[NCARGS]);
#ifdef __cplusplus
}
#endif

#endif
