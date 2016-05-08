-include $(LINUX_CONFIG)
-include $(CONFIG_CONFIG)
-include $(ARCH_CONFIG)

SRC_NAME=openssl-1.0.1

# openssl use CROSS_COMPILE prefix
CC=gcc $(CPUFLAGS)

COPTS=-Os -ffunction-sections -fdata-sections -Wl,--gc-sections -I$(DESTDIR)/include
COPTS+=$(if $(CONFIG_IPV6),-DOPENSSL_USE_IPV6=1,)
COPTS+=-DOPENSSL_SMALL_FOOTPRINT

LOPTS=-Wl,--gc-sections -L$(DESTDIR)/lib

OPENSSL_APPS = no-speed
OPENSSL_CIPHERS = no-camellia no-capieng no-cms no-gmp no-jpake no-rfc3779 no-seed \
                  no-ec2m no-err no-threads no-idea no-rc2 no-rc5 no-ripemd no-rmd160 \
                  no-static-engine no-hw no-md2 no-sse2 no-dso no-ts no-sha0 no-mdc2 no-krb5 \
                  no-smime no-ans1 no-cast no-whirlpool no-sctp no-srp no-ssl2 no-ssl3 no-ec

ifdef CONFIG_CRYPTO_DEV_LINUX
COPTS+=-DHAVE_CRYPTODEV
endif

# make only one thread
all: config_test
	$(MAKE) -j1 -C $(SRC_NAME)

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	./Configure \
		linux-mipsel \
		shared \
		--prefix=/ \
		--install_prefix=./filesystem \
		--openssldir=/etc/ssl \
		$(COPTS) \
		$(LOPTS) \
		$(OPENSSL_APPS) \
		$(OPENSSL_CIPHERS) ; \
	)

clean:
	$(MAKE) -C $(SRC_NAME) clean; \
	rm -f config_done
	rm -rf ./filesystem

shared:
	cp -fap $(SRC_NAME)/libssl.so* $(LIBCDIRSHARED)/lib
	cp -fap $(SRC_NAME)/libcrypto.so* $(LIBCDIRSHARED)/lib
	cp -faprL $(SRC_NAME)/include/* $(ROOTDIR)/lib/shared/include
	$(ROMFSINST) -S openssl.cnf /etc/openssl.cnf
