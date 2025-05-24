#!/bin/bash

clang++ -std=c++23 -l sqlite3 main.cpp -o main
./main
