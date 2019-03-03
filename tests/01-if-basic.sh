#!/usr/bin/expect
set timeout 1
log_user 0

spawn ../main
expect ".*/#"

source lib.tcl

# Basic if
Check "if true; then echo \"hola\"; fi" "\r\n\"hola\"\r\nResult: 0"

# Basic if (false condition)
Check "if false; then echo \"hola\"; fi" "\r\nResult: 0"

# Basic if with else
Check "if true; then echo \"hola\"; else echo no; fi" "\r\n\"hola\"\r\nResult: 0"

# Basic if with else (false condition)
Check "if false; then echo \"hola\"; else echo no; fi" "\r\nno\r\nResult: 0"

# Malformed: basic if (1)
Check "if true; then echo a fi" "\r\nResult: 1"

# Malformed: basic if (2)
Check "if true then echo a; fi" "\r\nResult: 1"

# Malformed: basic if (3)
Check "if true then; echo a; fi" "\r\nResult: 1"

# Malformed: basic if (4)
Check "if true; ; fi" "\r\nResult: 1"

# Malformed: basic if (5)
Check "if true; then echo a;" "\r\nResult: 1"

# Malformed: basic if (6)
Check "if true; then echo a; else echo b fi" "\r\nResult: 1"

# MAlformed: basic if (7)
Check "if true; then echo a else echo b fi" "\r\nResult: 1"

# Malformed: basic if (8)
Check "if true; then echo a; else echo b; else echo c; fi" "\r\nResult: 1"

