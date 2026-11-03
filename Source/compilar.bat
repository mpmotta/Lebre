@echo off
mingw64\bin\gcc.exe *.c -o lebre.exe -I.\mysql-connector-c-6.1.11-winx64\include -L.\mysql-connector-c-6.1.11-winx64\lib -lws2_32 -lmysql