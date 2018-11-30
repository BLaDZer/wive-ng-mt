#
# Makefile for wsdd2 WSD/LLMNR server
#
#	Copyright (c) 2016 NETGEAR
#	Copyright (c) 2016 Hiro Sugawara
#

#allways agressive optimize for size
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

OBJFILES	= wsdd2.o wsd.o llmnr.o
HEADERS		= wsdd.h wsd.h

INSTALLPREFIX	= ./fileystem
SBININSTALLDIR	= $(INSTALLPREFIX)/sbin
MANINSTALLDIR	= $(INSTALLPREFIX)/share/man

all: wsdd2

wsdd2:  $(OBJFILES)

install:
	#ok

romfs:
	$(ROMFSINST) -S wsdd2 /bin/wsdd2

clean:
	rm -f wsdd2 $(OBJFILES)
