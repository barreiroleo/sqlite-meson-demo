#!/bin/bash

echo "Compiling"
clang++ -std=c++23 -l sqlite3 main.cpp -o main

printf "Running\n\n"
./main
