#!/bin/bash

set -e
rm -rf valgrind-3.22.0
rm -rf valgrind-3.22.0.tar.bz2

wget https://sourceware.org/pub/valgrind/valgrind-3.22.0.tar.bz2
tar -xjf valgrind-3.22.0.tar.bz2
cd valgrind-3.22.0

export CC=/usr/bin/gcc
export CXX=/usr/bin/g++
export CFLAGS=""
export CXXFLAGS=""
export LDFLAGS=""
export CPPFLAGS=""
./configure
make -j`nproc`
make install
