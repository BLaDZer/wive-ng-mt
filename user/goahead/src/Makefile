#
# Makefile for the GoAhead web server reference source base
#  for the uClinux OS
#
# Copyright (c) GoAhead Software, Inc. 1995-2000
#
# $Id: Makefile,v 1.57.2.1 2009-04-08 08:52:59 chhung Exp $
#

-include $(LINUX_CONFIG)

CONF_H	= $(ROOTDIR)/lib/shared/include/linux/autoconf.h
UCONF_H	= $(ROOTDIR)/config/autoconf.h
BUSYCONF_H = $(ROOTDIR)/user/busybox/include/autoconf.h

CFLAGS  += -Os -fno-caller-saves -fno-reorder-blocks -fno-unwind-tables -fno-asynchronous-unwind-tables

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7620
endif
endif

ifdef CONFIG_RALINK_MT7621
ifdef CONFIG_MT7530_GSW
CFLAGS += -DCONFIG_MT7530_GSW -DCONFIG_RALINK_MT7621
endif
endif

ifdef CONFIG_RALINK_MT7628
ifdef CONFIG_RAETH_ESW
CFLAGS += -DCONFIG_RAETH_ESW -DCONFIG_RALINK_MT7628
endif
endif

NAME	= goahead

# User Management switch
UMSW	= -DUSER_MANAGEMENT_SUPPORT

# Digest Access switch
# DASW	= -DDIGEST_ACCESS_SUPPORT

# SSL switches
ifeq ("$(CONFIG_USER_GOAHEAD_SSL)", "y")
SSLPATCHFILE = matrix_ssl.o sslSocket.o
MATRIXDIR = $(ROOTDIR)/user/matrixssl-1.8.3
SSLINC = $(MATRIXDIR)
SSLLIB = $(MATRIXDIR)/src/libmatrixsslstatic.a
SSLSW = -DWEBS_SSL_SUPPORT -DMATRIX_SSL -I$(SSLINC)
endif

# If-Modified-Support switches (requires math library, libm.a)
IFMODSW = -DWEBS_IF_MODIFIED_SUPPORT
IFMODLIB = $(ROOTDIR)/lib/lib/libm.a

# Dependencies
DEPEND_FILES	= asp.o balloc.o base64.o cgi.o default.o  \
				  ejlex.o ejparse.o form.o \
				  h.o handler.o mime.o misc.o page.o \
				  ringq.o rom.o \
				  sock.o sockGen.o $(SSLPATCHFILE) \
				  security.o sym.o uemf.o url.o value.o \
				  md5c.o um.o umui.o websda.o emfdb.o \
				  webrom.o webs.o websuemf.o \
				  internet.o services.o utils.o wireless.o \
				  firewall.o management.o helpers.o procps.o $(NAME).o

# Enable web print debug commit nvram values
# CFLAGS += PRINT_DEBUG

# gahead flags
ifeq ("$(CONFIG_USER_GOAHEAD_IPV6)", "y")
CFLAGS += -DWF_USE_IPV6
endif
ifeq ("$(CONFIG_USER_GOAHEAD_HOSTNAME)", "y")
CFLAGS += -DGA_HOSTNAME_SUPPORT
endif

# external apps flags
ifneq ("$(CONFIG_RT2860V2_STA)", "")
DEPEND_FILES += station.o
endif
ifeq ("$(CONFIG_RALINKAPP_SWQOS)", "y")
DEPEND_FILES += qos.o
endif
ifneq ("$(CONFIG_USB)", "")
DEPEND_FILES += usb.o
endif
ifeq ("$(CONFIG_USER_STORAGE)", "y")
DEPEND_FILES += storage.o
endif

CFLAGS	+= -DWEBS -DOS="LINUX" -DLINUX $(UMSW) $(DASW) $(SSLSW) $(IFMODSW)
CFLAGS	+= -I$(ROOTDIR)/libnvram

LDFLAGS	+= $(SSLLIB) $(IFMODLIB)
LDLIBS	+= -lnvram
all:	$(NAME)

#
#	Primary link
#
$(NAME): clean_inet $(DEPEND_FILES)
	$(CC) -o $@ $(DEPEND_FILES) $(LDFLAGS) $(EXTRALIBS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S /bin/$(NAME)
ifeq ("$(CONFIG_USER_GOAHEAD_SSL)", "y")
	$(ROMFSINST) /etc/serverkey.pem
	$(ROMFSINST) /etc/servercert.pem
endif

clean:
	rm -f $(NAME) *.o

clean_inet:
	rm -f internet.o

#
#	Dependencies
#
asp.o:  webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

balloc.o: balloc.c uemf.h

base64.o:  base64.c webs.h wsIntrn.h  ej.h ejIntrn.h uemf.h

cgi.o:  webs.h wsIntrn.h uemf.h

default.o:  default.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

ejlex.o:  ejlex.c ej.h ejIntrn.h uemf.h

ejparse.o:  ejparse.c ej.h ejIntrn.h uemf.h

emfdb.o:  emfdb.h wsIntrn.h uemf.h

firewall.o: firewall.c webs.h firewall.h

procps.o: procps.c helpers.h webs.h procps.h

helpers.o: helpers.c webs.h helpers.h

form.o:  form.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

goahead.o: goahead.c uemf.h wsIntrn.h internet.h utils.h wireless.h firewall.h management.h qos.h $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

h.o:  h.c uemf.h

handler.o:  handler.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

internet.o: internet.c internet.h utils.h webs.h  $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

services.o: services.c services.h utils.h webs.h  $(CONF_H) $(UCONF_H) $(BUSYBOXCONF_H)

management.o: management.c management.h webs.h $(CONF_H) $(UCONF_H) 

matrix_ssl.o: matrix_ssl.c wsIntrn.h webs.h websSSL.h sslSocket.h

md5c.o:  md5.h wsIntrn.h uemf.h

mime.o:  mime.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

misc.o:  misc.c uemf.h

page.o:  page.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

ringq.o:  ringq.c uemf.h

rom.o:  rom.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

security.o:  security.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

sock.o:  sock.c uemf.h

sockGen.o:  sockGen.c uemf.h $(CONF_H)

sslSocket.o: sslSocket.c sslSocket.h

station.o: station.c station.h oid.h webs.h

usb.o: usb.c usb.h webs.h $(UCONF_H)

sym.o:  sym.c uemf.h

uemf.o:  uemf.c uemf.h

um.o:  webs.h wsIntrn.h um.h uemf.h

umui.o:  webs.h wsIntrn.h um.h uemf.h

url.o:  url.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

utils.o: utils.c utils.h webs.h $(CONF_H) $(UCONF_H)

value.o:  value.c uemf.h

webrom.o:  webrom.c webs.h wsIntrn.h uemf.h

webs.o:  webs.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h $(CONF_H)

websda.o:  webs.h wsIntrn.h websda.h uemf.h

websuemf.o:  websuemf.c webs.h wsIntrn.h ej.h ejIntrn.h uemf.h

websSSL.o:  websSSL.c websSSL.h wsIntrn.h ej.h ejIntrn.h uemf.h

wireless.o: wireless.c wireless.h internet.h utils.h webs.h $(CONF_H) $(UCONF_H)

qos.o: qos.h utils.h $(CONF_H)
