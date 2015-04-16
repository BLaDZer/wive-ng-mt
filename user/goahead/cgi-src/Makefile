#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.21.2.2 2009-04-17 03:40:21 chhung Exp $
#

# system cgi scripts must be static
LDFLAGS += -static

# system cgi scripts always optimize for size
CFLAGS  += -Os -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables
CFLAGS  += -DUPLOAD_FIRMWARE_SUPPORT

UPLOAD_CGI 		= upload.cgi
UPLOAD_RWFS		= upload_rwfs.cgi
UPLOAD_SETTINGS		= upload_settings.cgi

ALL_EXE = $(UPLOAD_CGI) $(UPLOAD_SETTINGS) $(UPLOAD_RWFS)

all: $(ALL_EXE)

$(UPLOAD_CGI): upload.cgi.o
	$(CC) $(CFLAGS) upload.cgi.o -o $@ $(LDFLAGS)
	$(STRIP) -R .comment -R .note -g --strip-unneeded $@

$(UPLOAD_RWFS): upload_rwfs.cgi.o
	$(CC) $(CFLAGS) upload_rwfs.cgi.o -o $@ $(LDFLAGS)
	$(STRIP) -R .comment -R .note -g --strip-unneeded $@

$(UPLOAD_SETTINGS): upload_settings.cgi.o
	$(CC) $(CFLAGS) upload_settings.cgi.o -o $@ $(LDFLAGS)
	$(STRIP) -R .comment -R .note -g --strip-unneeded $@


romfs:
	$(ROMFSINST) $(UPLOAD_CGI) $(ROOT_DIRECTORY)/cgi-bin/$(UPLOAD_CGI)
	$(ROMFSINST) $(UPLOAD_RWFS) $(ROOT_DIRECTORY)/cgi-bin/$(UPLOAD_RWFS)
	$(ROMFSINST) $(UPLOAD_SETTINGS)  $(ROOT_DIRECTORY)/cgi-bin/$(UPLOAD_SETTINGS)
	$(ROMFSINST) -S unpackrwfs.sh /bin/unpackrwfs.sh
	$(ROMFSINST) reboot.sh $(ROOT_DIRECTORY)/cgi-bin/reboot.sh
	$(ROMFSINST) ExportSettings.sh $(ROOT_DIRECTORY)/cgi-bin/ExportSettings.sh
	$(ROMFSINST) history.sh  $(ROOT_DIRECTORY)/cgi-bin/history.sh
	$(ROMFSINST) $(ROOTDIR)/History  $(ROOT_DIRECTORY)/cgi-bin/History

#They are not cgi but cgi-related.
clean:
	rm -f $(ALL_EXE) $(ARCH) *.o
