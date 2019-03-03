#!/usr/bin/expect
set timeout 1
log_user 0

spawn ../main
expect ".*/#"

source lib.tcl

# non closed string
Check "\"if false;" "\r\nResult: -3"

