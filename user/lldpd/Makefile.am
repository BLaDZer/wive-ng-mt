include doxygen.am

ACLOCAL_AMFLAGS = -I m4

CFLAGS	+= -Os -pipe -fpack-struct -Wall -Wno-unused -fno-caller-saves -fno-reorder-blocks -fno-aggressive-loop-optimizations
CFLAGS	+= -ffunction-sections -fdata-sections
LDFLAGS	+= -Wl,--gc-sections

SUBDIRS      = src/compat src src/daemon src/lib src/client tests osx
EXTRA_DIST   = $(DX_CONFIG) include get-version autogen.sh
DIST_SUBDIRS = $(SUBDIRS)
DISTCLEANFILES = ChangeLog filsystem

dist_doc_DATA = README.md NEWS CONTRIBUTE.md
doc_DATA = ChangeLog

__force-changelog-generation:
ChangeLog: __force-changelog-generation
	$(AM_V_GEN)if test -d $(top_srcdir)/.git; then \
		prev=$$(git describe --tags --always --match '[0-9]*' 2> /dev/null) ; \
		for tag in $$(git tag | $(EGREP) '^[0-9]+(\.[0-9]+){1,}$$' | sort -rn); do \
			if [ x"$$prev" = x ]; then prev=$$tag ; fi ; \
			if [ x"$$prev" = x"$$tag" ]; then continue; fi ; \
			echo "$$prev [$$(git log $$prev -1 --pretty=format:'%ai')]:" ; \
			echo "" ; \
			git log --pretty=' - [%h] %s (%an)' $$tag..$$prev ; \
			echo "" ; \
			prev=$$tag ; \
		done > $@ ; \
	else \
		touch $@ ; \
	fi

dist-hook:
	echo $(VERSION) > $(distdir)/.dist-version

MOSTLYCLEANFILES = $(DX_CLEANFILES)

# systemd and launchd files are not installed in the prefix, don't
# request them for distcheck
DISTCHECK_CONFIGURE_FLAGS = \
        --with-sysusersdir=no \
        --with-systemdsystemunitdir=no \
	--with-launchddaemonsdir=no

romfs: install
	$(ROMFSINST) -S init.d/W84lldpd /etc/rc.d/W84lldpd
	$(ROMFSINST) -s /etc/rc.d/W84lldpd /etc/init.d/lldpd
	$(ROMFSINST) -S lldpd.conf /etc/lldpd.conf
	cp -apr filesystem/lib/*.so* $(ROMFSDIR)/lib
	cp -apr filesystem/sbin/* $(ROMFSDIR)/sbin
	rm -rf filesystem
