#
# Makefile for wsdd2 WSD/LLMNR server
#
#	Copyright (c) 2016 NETGEAR
#	Copyright (c) 2016 Hiro Sugawara
#

#allways agressive optimize for size
CFLAGS	+= -lnvram -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

all:
	$(CC) $(CFLAGS) $(LDFLAGS) wsdd2.c wsd.c llmnr.c -o wsdd2

install:
	#ok

romfs:
	$(ROMFSINST) -S wsdd2 /bin/wsdd2

clean:
	rm -f wsdd2 *.o
