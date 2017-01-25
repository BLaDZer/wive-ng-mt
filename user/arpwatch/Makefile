#  Copyright (c) 1990, 1992, 1993, 1994, 1995, 1996, 1997, 1998, 2000
#	The Regents of the University of California.  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that: (1) source code distributions
#  retain the above copyright notice and this paragraph in its entirety, (2)
#  distributions including binary code include the above copyright notice and
#  this paragraph in its entirety in the documentation or other materials
#  provided with the distribution, and (3) all advertising materials mentioning
#  features or use of this software display the following acknowledgement:
#  ``This product includes software developed by the University of California,
#  Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
#  the University nor the names of its contributors may be used to endorse
#  or promote products derived from this software without specific prior
#  written permission.
#  THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
#  WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
#
# @(#) $Id: Makefile.in,v 1.68 2000/06/15 00:39:54 leres Exp $ (LBL)

#
# Various configurable paths (remember to edit Makefile.in, not Makefile)
#

# Top level hierarchy
prefix = ./filesystem
exec_prefix = ${prefix}
# Pathname of directory to install the binary
BINDEST = ${exec_prefix}/sbin
# Pathname of directory to install the man page
MANDEST = ${prefix}/man
# Pathname of directory to install database file
ARPDIR = $(prefix)/arpwatch

# VPATH
srcdir = .


#
# You shouldn't need to edit anything below here.
#
CC ?= gcc
PROG = arpwatch
CCOPT = -Os
INCLS = -I.  -Ilinux-include
DEFS = -DPACKAGE_NAME=\"\" -DPACKAGE_TARNAME=\"\" -DPACKAGE_VERSION=\"\" -DPACKAGE_STRING=\"\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DHAVE_FCNTL_H=1 -DHAVE_MEMORY_H=1 -DTIME_WITH_SYS_TIME=1 -DHAVE_BCOPY=1 -DHAVE_STRERROR=1 -DRETSIGTYPE=void -DRETSIGVAL= -DHAVE_SIGSET=1 -Dsignal=sigset -DDECLWAITSTATUS=int  -DARPDIR=\"/tmp\" -DPATH_SENDMAIL=\"\"

CFLAGS += $(CCOPT) $(DEFS) $(INCLS)
CFLAGS += -ffunction-sections -fdata-sections

LDFLAGS += -Wl,--gc-sections

# Standard LIBS
LIBS =  -lpcap
# Standard LIBS without libpcap.a
SLIBS =

INSTALL = /usr/bin/install -c
SENDMAIL =

# Explicitly define compilation rule since SunOS 4's make doesn't like gcc.
# Also, gcc does not remove the .o before forking 'as', which can be a
# problem if you don't own the file but can write to the directory.
.c.o:
	@rm -f $@
	$(CC) $(CFLAGS) -c $(srcdir)/$*.c

CSRC =	db.c dns.c ec.c file.c intoa.c machdep.c util.c report.c setsignal.c
WSRC =	arpwatch.c
SSRC =	arpsnmp.c
GENSRC = version.c

SRC =	$(WSRC) $(SSRC) $(CSRC) $(GENSRC)

# We would like to say "OBJ = $(SRC:.c=.o)" but Ultrix's make cannot
# hack the extra indirection
OBJ =	$(WSRC:.c=.o) $(SSRC:.c=.o) $(CSRC:.c=.o) $(GENSRC:.c=.o)
WOBJ =	$(WSRC:.c=.o) $(CSRC:.c=.o) $(GENSRC:.c=.o)
SOBJ =	$(SSRC:.c=.o) $(CSRC:.c=.o) $(GENSRC:.c=.o)
HDR =	arpwatch.h db.h dns.h ec.h file.h machdep.h report.h setsignal.h

TAGHDR = \
	/usr/include/net/if.h \
	/usr/include/netinet/if_ether.h \
	/usr/include/netinet/in.h \
	/usr/include/sys/socket.h

TAGFILES = $(SRC) $(HDR) $(TAGHDR)

ALL = arpwatch

CLEANFILES = $(ALL) $(OBJ) $(GENSRC) zap.o zap

all: $(ALL)

arpwatch: $(WOBJ)
	@rm -f $@
	$(CC) $(CFLAGS) -o $@ $(WOBJ) $(LDFLAGS) $(LIBS)

arpsnmp: $(SOBJ)
	@rm -f $@
	$(CC) $(CFLAGS) -o $@ $(SOBJ) $(LDFLAGS) $(SLIBS)

version.o: version.c
version.c: $(srcdir)/VERSION
	@rm -f $@
	sed -e 's/.*/char version[] = "&";/' $(srcdir)/VERSION > $@

zap: zap.o intoa.o
	$(CC) $(CFLAGS) -o $@ zap.o intoa.o -lutil

install: force
	$(INSTALL) -m 755 arpwatch $(DESTDIR)$(BINDEST)

install-man:
	$(INSTALL) -m 644 $(srcdir)/arpwatch.8 $(DESTDIR)$(MANDEST)/man8
	$(INSTALL) -m 644 $(srcdir)/arpsnmp.8 $(DESTDIR)$(MANDEST)/man8

romfs :
	$(ROMFSINST) -S arpwatch /bin/arpwatch
	$(ROMFSINST) -S ../init.d/W92arpwatch /etc/rc.d/W92arpwatch
	$(ROMFSINST) -s /etc/rc.d/W92arpwatch /etc/init.d/arpwatch

clean:
	rm -f $(CLEANFILES)

distclean:
	rm -f $(CLEANFILES) config.cache config.log config.status
