ACLOCAL_AMFLAGS = -I m4
CFLAGS += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections

if BUILD_CLI
  CLI_DIR = cli
endif
if BUILD_DAEMON
if !WIN32
  DAEMON_DIR = daemon
endif
endif
if BUILD_GTK
  GTK_DIR = gtk po
endif
if BUILD_MAC
  MAC_DIR = macosx
endif

SUBDIRS = \
  extras \
  third-party \
  libtransmission \
  utils \
  $(DAEMON_DIR) \
  $(CLI_DIR) \
  $(GTK_DIR) \
  $(MAC_DIR) \
  web

EXTRA_DIST = \
  qt \
  NEWS \
  AUTHORS \
  COPYING \
  README \
  autogen.sh \
  update-version-h.sh \
  Transmission.xcodeproj/project.pbxproj

dist-hook:
	rm -rf `find $(distdir)/qt -name .svn`

romfs: install
	$(ROMFSINST) -S ../init.d/W95transmission /etc/rc.d/W95transmission
	$(ROMFSINST) -s /etc/rc.d/W95transmission /etc/init.d/transmission
	$(ROMFSINST) -S filesystem/bin/transmission-daemon /bin/transmission-daemon
	cp -rfva filesystem/share/transmission* $(ROMFSDIR)/share/transmission/

DISTCLEANFILES = \
  intltool-extract \
  intltool-merge \
  intltool-update \
  filesystem
