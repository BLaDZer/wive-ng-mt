EXEC=igmpproxy
IGMP_PROXY_OBJ= igmpproxy.o config.o confread.o request.o mcgroup.o rttable.o \
		igmp.o ifvc.o callout.o kern.o syslog.o lib.o mroute-api.o

-include $(LINUX_CONFIG)

CFLAGS += -Wno-unused-value
CFLAGS += -I$(LINUXDIR) -I$(LINUXDIR)/include
CFLAGS += -I$(LINUXDIR)/drivers/net/raeth
CONF_H  = $(LINUXDIR)/include/linux/autoconf.h

ifdef CONFIG_RT2860V2_AP_IGMP_SNOOP
# allow add snoop records to wifi drivers from igmpproxy
CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT
#CFLAGS+= -DWIFI_IGMPSNOOP_SUPPORT_AUTO_DISABLE
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
