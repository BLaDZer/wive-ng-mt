CFLAGS += -Os -fPIC -fomit-frame-pointer -ffunction-sections -fdata-sections
LDFLAGS += -ffunction-sections -fdata-sections -Wl,--gc-sections

# If you want to support multiple radius server for multiple bssid, add following line
CFLAGS += -DMULTIPLE_RADIUS

# If you want to support acl by mac radius server add following line, and ieee802_11_auth.o in OBJS
CFLAGS += -DRADIUS_MAC_ACL_SUPPORT

# If you want to debug daemon, add following line
CFLAGS += -DDBG -DSYSLOG

OBJS =	rtdot1x.o eloop.o eapol_sm.o radius.o md5.o  \
	config.o ieee802_1x.o  \
	sta_info.o radius_client.o \
	ieee802_11_auth.o

all: rtdot1xd

rtdot1xd: $(OBJS)
	$(CC) -o rt2860apd $(OBJS)

clean:
	rm -f core *.o rt2860apd

romfs:
	$(ROMFSINST) rt2860apd /bin/rt2860apd
	ln -sf rt2860apd $(ROMFSDIR)/bin/rtinicapd
	$(ROMFSINST) -S init.d/W65apd /etc/rc.d/W65apd
	$(ROMFSINST) -s /etc/rc.d/W65apd /etc/init.d/apd


-include $(OBJS:%.o=%.d)
