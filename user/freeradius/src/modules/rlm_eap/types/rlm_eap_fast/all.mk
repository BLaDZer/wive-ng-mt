TARGETNAME	:= rlm_eap_fast

ifneq "$(OPENSSL_LIBS)" ""
TARGET		:= $(TARGETNAME).a
endif

SOURCES		:= $(TARGETNAME).c eap_fast.c eap_fast_crypto.c

SRC_INCDIRS	:= ${top_srcdir}/src/modules/rlm_eap/ ${top_srcdir}/src/modules/rlm_eap/libeap/
TGT_PREREQS	:= libfreeradius-eap.a
