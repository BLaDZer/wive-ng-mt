## Process this file with automake to produce Makefile.in
CFLAGS    += -ffunction-sections -fdata-sections -Wno-maybe-uninitialized
LDFLAGS   += -Wl,--gc-sections

ACLOCAL_AMFLAGS = -I m4
SUBDIRS = bstring
EXTRA_DIST = CREDITS LICENSE acsite.m4
if WITH_JSONLIB
SUBDIRS += json
endif
SUBDIRS += src doc www conf debian distro
if WITH_MINIPORTAL
SUBDIRS += miniportal
endif
