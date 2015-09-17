include $(top_srcdir)/Make_global.am

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = $(man_MANS)

SUBDIRS = src include utils
DIST_SUBDIRS = src include utils

man_MANS = #nfnetlink.3

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnfnetlink.pc

dist-hook:
	rm -rf `find $(distdir)/debian -name .svn`

shared: install
	cp -vfa ./filesystem/lib/*.so*    $(LIBCDIRSHARED)/lib
	-cp -vfa ./filesystem/lib/*.la*    $(LIBCDIRSHARED)/lib
	cp -vfra ./filesystem/include/* $(LIBCDIRSHARED)/include
	rm -rf ./filesystem
