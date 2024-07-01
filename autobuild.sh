#!/usr/bin/bash

set -e

if [ ! -d `pwd`/build ]; then
    mkdir `pwd`/build
fi

if [ -d `pwd`/lib ]; then
    rm -rf `pwd`/lib
fi

rm -rf `pwd`/build/*

cd `pwd`/build && 
    cmake .. &&
    make