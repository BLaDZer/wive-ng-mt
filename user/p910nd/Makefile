STRIP = $(CROSS)strip

CFLAGS += -Os -Wall -ffunction-sections -fdata-sections -fno-caller-saves -fno-reorder-blocks -fno-aggressive-loop-optimizations
LDFLAGS	+= -Wl,--gc-sections

ifneq ("$(LIBWRAP)", "")
LIBWRAP += -DUSE_LIBWRAP
endif

PROG = p910nd
CONFIG = p910nd.conf
INITSCRIPT = p910nd.init
MANPAGE = p910nd.8
INSTALL = install
BINDIR = /usr/sbin
CONFIGDIR = /etc/sysconfig
SCRIPTDIR = /etc/init.d
MANDIR = /usr/share/man/man8
DESTDIR = ./filesystem

$(PROG):	p910nd.c
	$(CC) -o $@ $^ $(CFLAGS) $(LIBWRAP)

strip: $(PROG)
	$(STRIP) -s $(PROG)

install: $(PROG) $(CONFIG) $(INITSCRIPT) $(MANPAGE)
	mkdir -p $(DESTDIR)$(BINDIR) $(DESTDIR)$(CONFIGDIR) \
		$(DESTDIR)$(SCRIPTDIR) $(DESTDIR)$(MANDIR)
	$(INSTALL) $(PROG) $(DESTDIR)$(BINDIR)
	$(INSTALL) -m 644 $(CONFIG) $(DESTDIR)$(CONFIGDIR)/$(PROG)
	$(INSTALL) $(INITSCRIPT) $(DESTDIR)$(SCRIPTDIR)/$(PROG)
	$(INSTALL) -m 644 $(MANPAGE) $(DESTDIR)$(MANDIR)

.PHONY: clean
clean:
	rm -f *.o $(PROG)

romfs:
	$(ROMFSINST) -S /bin/p910nd
