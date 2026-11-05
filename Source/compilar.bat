@echo off
mingw64\bin\gcc.exe -o lebre.exe *.c -I./include -I./include/mariadb -I./include/mysql -L./lib -lmariadb -lws2_32