#!/bin/bash
clang *.c -o lebre $(mysql_config --cflags --libs) -lreadline