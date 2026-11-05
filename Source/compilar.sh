#!/bin/bash
clang -o lebre *.c -I/opt/homebrew/opt/mariadb-connector-c/include/mariadb -L/opt/homebrew/opt/mariadb-connector-c/lib/mariadb -lmariadb -lreadline