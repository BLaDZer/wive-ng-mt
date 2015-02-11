MAKEFLAGS += --no-print-directory

PREFIX = ./filesystem
SBINDIR = $(PREFIX)/sbin
MANDIR = $(PREFIX)/share/man
PKG_CONFIG = #pkg-config

MKDIR ?= mkdir -p
INSTALL ?= install
CC ?= "gcc"

CFLAGS += -Os -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration

OBJS = iw.o genl.o event.o info.o phy.o \
	interface.o ibss.o station.o survey.o util.o \
	mesh.o mpath.o scan.o reg.o version.o \
	reason.o status.o connect.o link.o offch.o ps.o cqm.o \
	bitrate.o wowlan.o roc.o p2p.o
OBJS += sections.o

OBJS-$(HWSIM) += hwsim.o

OBJS += $(OBJS-y) $(OBJS-Y)

ALL = iw

LIBS += -lnl

ifeq ($(V),1)
Q=
NQ=true
else
Q=@
NQ=echo
endif

all: $(ALL)

VERSION_OBJS := $(filter-out version.o, $(OBJS))

version.c: version.sh $(patsubst %.o,%.c,$(VERSION_OBJS)) nl80211.h iw.h Makefile \
		$(wildcard .git/index .git/refs/tags)
	@$(NQ) ' GEN ' $@
	$(Q)./version.sh $@

%.o: %.c iw.h nl80211.h
	@$(NQ) ' CC  ' $@
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

iw:	$(OBJS)
	@$(NQ) ' CC  ' iw
	$(Q)$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o iw

check:
	$(Q)$(MAKE) all CC="REAL_CC=$(CC) CHECK=\"sparse -Wall\" cgcc"

%.gz: %
	@$(NQ) ' GZIP' $<
	$(Q)gzip < $< > $@

install: iw iw.8.gz
	@$(NQ) ' INST iw'
	$(Q)$(MKDIR) $(DESTDIR)$(SBINDIR)
	$(Q)$(INSTALL) -m 755 iw $(DESTDIR)$(SBINDIR)
	@$(NQ) ' INST iw.8'
	$(Q)$(MKDIR) $(DESTDIR)$(MANDIR)/man8/
	$(Q)$(INSTALL) -m 644 iw.8.gz $(DESTDIR)$(MANDIR)/man8/

clean:
	$(Q)rm -f iw *.o *~ *.gz version.c *-stamp

romfs:
	$(ROMFSINST) -S /bin/iw
