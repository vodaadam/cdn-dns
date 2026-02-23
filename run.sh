#!/usr/bin/env bash

g++ -std=c++17 -Wall -Wextra -pedantic -O2 -I. main.cpp -o cdn-dns
./cdn-dns