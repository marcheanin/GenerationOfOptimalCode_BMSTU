# /usr/bin/bash

g++ lab3.cpp -g -O3 -Wformat=0 `llvm-config --system-libs --cppflags --ldflags --libs core`