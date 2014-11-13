#
# Top-level "Makefile.am" for libupnp
#
# Copyright (C) 2005 Rémi Turboult <r3mi@users.sourceforge.net>
#

ACLOCAL_AMFLAGS = -I m4

DISTCHECK_CONFIGURE_FLAGS = --enable-debug --enable-samples

SUBDIRS = \
	ixml \
	threadutil \
	upnp \
	docs/dist


EXTRA_DIST = \
	Doxyfile \
	libupnp.pc.in \
	LICENSE \
	THANKS \
	libupnp.spec \
	build/libupnp.bpf \
	build/libupnp.bpr \
	build/libupnp.dsp \
	build/libupnp.dsw \
	build/inc/autoconfig.h \
	build/inc/upnpconfig.h \
	build/msvc/inttypes.h \
	build/msvc/stdint.h \
	build/vc8/ixml.vcproj \
	build/vc8/libupnp.sln \
	build/vc8/libupnp.vcproj \
	build/vc8/threadutil.vcproj \
	build/vc8/tvcombo.vcproj \
	build/vc8/tvctrlpt.vcproj \
	build/vc8/tvdevice.vcproj \
	build/vc9/ixml.vcproj \
	build/vc9/libupnp.sln \
	build/vc9/libupnp.vcproj \
	build/vc9/threadutil.vcproj \
	build/vc9/tvcombo.vcproj \
	build/vc9/tvctrlpt.vcproj \
	build/vc9/tvdevice.vcproj 


# This variable must have 'exec' in its name, in order to be installed 
# by 'install-exec' target (instead of default 'install-data')
pkgconfigexecdir = $(libdir)/pkgconfig
pkgconfigexec_DATA = libupnp.pc


$(pkgconfigexec_DATA): config.status

shared: install
	cp -vfa ./filesystem/lib/*.*    $(LIBCDIRSHARED)/lib
	cp -vfra ./filesystem/include/* $(LIBCDIRSHARED)/include
	cp -vfra ./ixml/src/inc/*.h	$(LIBCDIRSHARED)/include/upnp
	cp -vfra ./upnp/src/inc/*.h	$(LIBCDIRSHARED)/include/upnp
	cp -vf autoconfig.h		$(LIBCDIRSHARED)/include/upnp/autoconfig.h
	rm -rf ./filesystem

if WITH_DOCUMENTATION
	doc_DATA = LICENSE README NEWS TODO THANKS
endif


CLEANFILES = IUpnpErrFile.txt IUpnpInfoFile.txt

