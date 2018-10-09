#!/bin/bash

echo "==================CONFIGURE-BUSYBOX==========================="
APROOTDIR=`pwd`

# prefer use bash if multishell
if [ -e /bin/bash ]; then
    SHELL="/bin/bash"
else
    SHELL="/bin/sh"
fi

export SHELL

make oldconfig
