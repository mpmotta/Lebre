#!/bin/bash

gcc -o ../Lebre-Linux *.c $(mysql_config --cflags --libs) -lreadline -lm

if [ $? -eq 0 ]; then
    echo "Compilacao concluida: ../Lebre-Linux"
else
    echo "Erro na compilacao."
    exit 1
fi