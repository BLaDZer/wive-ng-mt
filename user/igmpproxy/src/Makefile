EXEC=igmpproxy
IGMP_PROXY_OBJ= igmpproxy.o config.o confread.o request.o mcgroup.o rttable.o \
		igmp.o ifvc.o callout.o kern.o syslog.o lib.o mroute-api.o

-include $(LINUX_CONFIG)

CFLAGS += -Wno-unused-value
CFLAGS += -I$(LINUXDIR) -I$(LINUXDIR)/include -I$(ROOTDIR)/lib/shared/include
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(ROOTDIR)/lib/shared/include/linux/autoconf.h

ifdef CONFIG_RT2860V2_AP_IGMP_SNOOP
# auto enable Multicast to Unicast in wifi drivers by user req
CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT
# auto disable Multicast to Unicast in wifi drivers if route table empty
CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT_AUTO_DISABLE
# allow add snoop records to wifi drivers from igmpproxy (2.7.x.x drivers have TxRing issue, not enable it)
#CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT_STATIC_MANAGMENT
endif

ifdef CONFIG_RALINK_RT6855
ifdef CONFIG_RAETH_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT -DCONFIG_RALINK_RT6855
endif
endif

ifdef CONFIG_RALINK_RT6855A
ifdef CONFIG_RAETH_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT -DCONFIG_RALINK_RT6855A
endif
endif

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT -DCONFIG_RALINK_MT7620
endif
endif

all: $(EXEC)

igmpproxy: build.h $(IGMP_PROXY_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(IGMP_PROXY_OBJ)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	rm -f *.o *.asm build.h $(EXEC)

build.h	:
	echo '#define BUILD "' `date +%y%m%d` '"' >build.h
