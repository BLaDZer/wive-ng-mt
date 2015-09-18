CFLAGS+=-Os -fPIC -fomit-frame-pointer -Isrc -Ilibhttpd
#-ffunction-sections -fdata-sections
LDFLAGS+=-pthread
#-ffunction-sections -fdata-sections -Wl,--gc-sections
LDLIBS=

STRIP=no

NDS_OBJS=src/auth.o src/client_list.o src/commandline.o src/conf.o \
	src/debug.o src/firewall.o src/fw_iptables.o src/gateway.o src/http.o \
	src/httpd_thread.o src/ndsctl_thread.o src/safe.o src/tc.o src/util.o

LIBHTTPD_OBJS=libhttpd/api.o libhttpd/ip_acl.o \
	libhttpd/protocol.o libhttpd/version.o

.PHONY: all clean install checkastyle fixstyle

all: nodogsplash ndsctl

%.o : %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

nodogsplash: $(NDS_OBJS) $(LIBHTTPD_OBJS)
	$(CC) $(LDFLAGS) -o nodogsplash $+ $(LDLIBS)

ndsctl: src/ndsctl.o
	$(CC) $(LDFLAGS) -o ndsctl $+ $(LDLIBS)

clean:
	rm -f nodogsplash ndsctl src/*.o libhttpd/*.o
	rm -rf dist

install:
#ifeq(yes,$(STRIP))
	strip nodogsplash
	strip ndsctl
#endif
	mkdir -p $(DESTDIR)/usr/bin/
	cp ndsctl $(DESTDIR)/usr/bin/
	cp nodogsplash $(DESTDIR)/usr/bin/
	mkdir -p $(DESTDIR)/etc/nodogsplash/htdocs/images
	cp resources/nodogsplash.conf $(DESTDIR)/etc/nodogsplash/
	cp resources/splash.html $(DESTDIR)/etc/nodogsplash/htdocs/
	cp resources/infoskel.html $(DESTDIR)/etc/nodogsplash/htdocs/
	cp resources/splash.jpg $(DESTDIR)/etc/nodogsplash/htdocs/images/
	$(ROMFSINST) -S init.d/W89nodogsplash /etc/rc.d/W89nodogsplash
	$(ROMFSINST) -s /etc/rc.d/W89nodogsplash /etc/init.d/nodogsplash

romfs:
	$(ROMFSINST) -S ndsctl /bin/ndsctl
	$(ROMFSINST) -S nodogsplash /bin/nodogsplash
	mkdir -p $(ROMFSDIR)/etc/nodogsplash $(ROMFSDIR)/etc/nodogsplash/htdocs $(ROMFSDIR)/etc/nodogsplash/pages $(ROMFSDIR)/etc/nodogsplash/htdocs/images
	cp -aprvf resources/*.conf $(ROMFSDIR)/etc/nodogsplash/
	cp -aprvf resources/*.html $(ROMFSDIR)/etc/nodogsplash/htdocs/
	cp -aprvf resources/*.jpg $(ROMFSDIR)/etc/nodogsplash/htdocs/images/

checkastyle:
	@command -v astyle >/dev/null 2>&1 || \
	{ echo >&2 "We need 'astyle' but it's not installed. Aborting."; exit 1; }

fixstyle: checkastyle
	@echo "\033[1;34mChecking style ...\033[00m"
	@astyle --lineend=linux --suffix=none --style=kr --indent=force-tab \
	--formatted --recursive "src/*.c" "src/*.h"|grep formatted \
	&& echo "\033[1;33mPrevious files have been corrected\033[00m" \
	|| echo "\033[0;32mAll files are ok\033[00m"

DEBVERSION=$(shell dpkg-parsechangelog | grep ^Version |cut -f2 -d\  | sed -e 's/-[0-9]*$$//' )
deb: clean
	mkdir -p dist/nodogsplash-$(DEBVERSION)
	tar --exclude dist --exclude ".git*" -cf - . | (cd dist/nodogsplash-$(DEBVERSION) && tar xf -)
	cd dist && tar cjf nodogsplash_$(DEBVERSION).orig.tar.bz2 nodogsplash-$(DEBVERSION)
	cd dist/nodogsplash-$(DEBVERSION) && dpkg-buildpackage -us -uc
	rm -rf dist/nodogsplash-$(DEBVERSION)
