#!/bin/bash

if [ ! -f ./configure ]; then
    automake --add-missing --force-missing --copy
    ./autogen.sh
fi
./configure
