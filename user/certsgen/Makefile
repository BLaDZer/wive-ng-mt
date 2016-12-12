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

all:
	# OK

clean:
	rm -f *.key *.pem

install:
	# OK

romfs:
ifdef NEEDGEN
	############# BUILD OPENSSL CERTS FOR APPS USE ######################
	openssl req -new -x509 -nodes -days 365 -newkey rsa:2048 -keyout ca.key -out ca.pem -subj "/C=RU/ST=Russia/L=EKB/O=Wive-NG/OU=CA/CN=ROUTER"
	mkdir -p $(ROMFSDIR)/etc/certs
	$(ROMFSINST) -S ca.key /etc/certs/ca.key
	$(ROMFSINST) -S ca.pem /etc/certs/ca.pem
endif
