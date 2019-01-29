#!/bin/bash

if [ ! -f ./configure ] || [ ! -f missing -a ! -f build-aux/missing ]; then
    automake --add-missing --force-missing --copy
    ./autogen.sh
fi
./configure
