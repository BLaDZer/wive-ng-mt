.EXPORT_ALL_VARIABLES:

-include $(LINUX_CONFIG)

IPOBJ=ip.o ipaddress.o ipaddrlabel.o iproute.o iprule.o ipnetns.o \
    rtm_map.o iptunnel.o ip6tunnel.o tunnel.o ipneigh.o ipntable.o iplink.o \
    ipmaddr.o ipmonitor.o ipmroute.o ipprefix.o iptuntap.o \
    iplink_vlan.o link_veth.o

ifdef CONFIG_L2TP_V3
CFLAGS	+= -DL2TP_V3_SUPPORT
IPOBJ	+= ipl2tp.o
endif

#    link_gre.o iplink_can.o iplink_macvlan.o iplink_macvtap.o \
#    ipxfrm.o xfrm_state.o xfrm_policy.o xfrm_monitor.o \

RTMONOBJ=rtmon.o

include ../Config

ifeq ($(IP_CONFIG_SETNS),y)
	CFLAGS += -DHAVE_SETNS
endif

ALLOBJ=$(IPOBJ) $(RTMONOBJ)
SCRIPTS=ifcfg rtpr routel routef
TARGETS=ip rtmon

all: $(TARGETS) $(SCRIPTS)

ip: $(IPOBJ) $(LIBNETLINK)


rtmon: $(RTMONOBJ)

install: all
	install -m 0755 $(TARGETS) $(DESTDIR)$(SBINDIR)
	install -m 0755 $(SCRIPTS) $(DESTDIR)$(SBINDIR)

clean:
	rm -f $(ALLOBJ) $(TARGETS)

SHARED_LIBS ?= y
ifeq ($(SHARED_LIBS),y)

LDLIBS += -ldl
LDFLAGS += -Wl,-export-dynamic

else

ip: static-syms.o
static-syms.o: static-syms.h
static-syms.h: $(wildcard *.c)
	files="$^" ; \
	for s in `grep -B 3 '\<dlsym' $$files | sed -n '/snprintf/{s:.*"\([^"]*\)".*:\1:;s:%s::;p}'` ; do \
		sed -n '/'$$s'[^ ]* =/{s:.* \([^ ]*'$$s'[^ ]*\) .*:extern char \1[] __attribute__((weak)); if (!strcmp(sym, "\1")) return \1;:;p}' $$files ; \
	done > $@

endif
