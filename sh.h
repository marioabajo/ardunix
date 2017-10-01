#ifndef SH_H
#define SH_H

#include "kernel.h"
#include "defines.h"
#include "coreutils.h"
// include this to initialice the rootfs in main
#include "fs.h"
#include "env.h"

// Some definitions
#define FALSE 0
#define TRUE 1

#define TK_EXCL  1	// !
#define TK_BROP  2	// {
#define TK_BRCL  3	// }
#define TK_CASE  4	// case
#define TK_DO    5	// do
#define TK_DONE  6	// done
#define TK_ELIF  7	// elif
#define TK_ELSE  8	// else
#define TK_ESAC  9	// esac
#define TK_FI    10	// fi
#define TK_FOR   11	// for
#define TK_IF    12	// if
#define TK_IN    13	// in
#define TK_THEN  14	// then
#define TK_UNTIL 15	// until
#define TK_WHILE 16	// while
#define TK_NL    17 // \n
#define TK_CR    18 // \r
#define TK_SC    19 // ;
#define TK_PO    20 // (
#define TK_PC    21 // )

// Data types
typedef unsigned int tpos;
typedef unsigned char token;

#ifdef __cplusplus
extern "C" {
#endif
  uint8_t main_sh(uint8_t argc, char *argv[], char *env[]);
#ifdef __cplusplus
}
#endif

#endif
