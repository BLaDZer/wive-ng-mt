##########################################################################
# pregenerate openssl cers for use at some apps
##########################################################################

NEEDGEN=
ifdef CONFIG_USER_CWMPD
    NEEDGEN=y
endif
ifdef CONFIG_USER_CHILLISPOT
    NEEDGEN=y
endif
ifdef CONFIG_USER_RADIUS
    NEEDGEN=y
endif

all:
	# OK

clean:
	rm -f *.key *.pem dh

install:
	# OK

romfs:
ifdef NEEDGEN
	############# BUILD OPENSSL CERTS FOR APPS USE ######################
	mkdir -p $(ROMFSDIR)/etc/certs
	openssl req -new -x509 -nodes -days 365 -newkey rsa:2048 -keyout ca.key -out ca.pem -subj "/C=RU/ST=Russia/L=EKB/O=Wive-NG/OU=CA/CN=ROUTER"
	$(ROMFSINST) -S ca.key /etc/certs/ca.key
	$(ROMFSINST) -S ca.pem /etc/certs/ca.pem
ifdef CONFIG_USER_RADIUS
	openssl req -new -x509 -nodes -days 365 -newkey rsa:2048 -keyout server.key -out server.pem -subj "/C=RU/ST=Russia/L=EKB/O=Wive-NG/OU=CA/CN=ROUTER"
	openssl dhparam 512 > dh
	$(ROMFSINST) -S server.key /etc/certs/server.key
	$(ROMFSINST) -S server.pem /etc/certs/server.pem
	$(ROMFSINST) -S dh /etc/certs/dh
endif
endif
