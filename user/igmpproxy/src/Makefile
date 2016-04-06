-include $(LINUX_CONFIG)

CFLAGS+=-Wall -Wextra -Wshadow -Wformat=2 -Wfloat-equal -Wlogical-op -Wcast-qual

EXEC=igmpproxy
IGMP_PROXY_OBJ= igmpproxy.o config.o confread.o request.o mcgroup.o rttable.o \
		igmp.o ifvc.o callout.o kern.o syslog.o lib.o mroute-api.o

ifdef CONFIG_RT2860V2_AP_IGMP_SNOOP
    WIFISNOOP:=y
endif
ifdef CONFIG_MT7610_AP_IGMP_SNOOP
    WIFISNOOP:=y
endif
ifdef CONFIG_MT76X2_AP_IGMP_SNOOP
    WIFISNOOP:=y
endif

ifdef WIFISNOOP
CFLAGS+= -I../../goahead
# auto enable Multicast to Unicast in wifi drivers by user req
CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT
# auto disable Multicast to Unicast in wifi drivers if route table empty
CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT_AUTO_DISABLE
# allow add snoop records to wifi drivers from igmpproxy (2.7.x.x drivers have TxRing issue, do not enable it)
# CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT_STATIC_MANAGMENT
endif

ifdef CONFIG_RALINK_MT7620
ifdef CONFIG_RAETH_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT -DCONFIG_RALINK_MT7620
endif
endif

ifdef CONFIG_RALINK_MT7621
ifdef CONFIG_MT7530_GSW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT -DCONFIG_RALINK_MT7621
endif
endif

ifdef CONFIG_RALINK_MT7628
ifdef CONFIG_RAETH_ESW
IGMP_PROXY_OBJ+= ralink.o rtGSW.o
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth -DRALINK_ESW_SUPPORT -DCONFIG_RALINK_MT7628
endif
endif

all: $(EXEC)

igmpproxy: $(IGMP_PROXY_OBJ)
	$(CC) $(LDFLAGS) -o $@ $(IGMP_PROXY_OBJ)

romfs:
	$(ROMFSINST) -S /bin/$(EXEC)

clean:
	rm -f *.o *.asm $(EXEC)
