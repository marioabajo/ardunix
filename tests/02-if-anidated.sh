#!/usr/bin/expect
set timeout 1
log_user 0

spawn ../main
expect ".*/#"

source lib.tcl

# Basic anidated if
Check "if true;then echo a;if true;then echo b;fi;fi" "\r\na\r\nb\r\nResult: 0"

# Basic anidated if (else run)
Check "if false;then echo a;if true;then echo b;else echo c;fi;else echo d;fi" "\r\nd\r\nResult: 0"

# Basic anidated if (anidated else run)
Check "if true;then echo a;if false;then echo b;else echo c;fi;fi" "\r\na\r\nc\r\nResult: 0"


