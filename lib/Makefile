# Makefile for uClibc
#
# Copyright (C) 2000-2005 Erik Andersen <andersen@uclibc.org>
#
# Licensed under the LGPL v2.1, see the file COPYING.LIB in this tarball.
#

ifndef ROOTDIR
ROOTDIR=..
endif

UCLINUX_BUILD_LIB=1
-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)

top_srcdir=./
top_builddir=$(if $(O),$(O),.)/
export top_builddir

# We do not need built-in implicit rules
MAKEFLAGS += -r
CONFIG_SHELL ?= /bin/sh
export CONFIG_SHELL

include $(top_srcdir)Makefile.in
include $(top_srcdir)Makerules
include $(top_srcdir)Makefile.help
