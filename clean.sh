#!/usr/bin/bash

set -e

if [ -d `pwd`/build ]; then
    rm -rf `pwd`/build
fi

if [ -d `pwd`/lib ]; then
    rm -rf `pwd`/lib
fi