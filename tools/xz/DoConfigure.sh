#!/bin/bash
[ ! -f ./configure ] && ./autogen.sh
./configure
