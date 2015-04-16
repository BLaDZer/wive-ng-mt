EXEC = snmpd
OBJS = snmpd.o

CFLAGS += -DCONFIG_USER_SNMPD_MODULE_SYSTEM -DCONFIG_USER_SNMPD_MODULE_IFACE -DCONFIG_USER_SNMPD_MODULE_IP -DCONFIG_USER_SNMPD_MODULE_TCP
CFLAGS += -DCONFIG_USER_SNMPD_MODULE_ICMP -DCONFIG_USER_SNMPD_MODULE_UDP -DCONFIG_USER_SNMPD_MODULE_RALINK_ESW
CFLAGS += -I../snmplib -I../modules

USELIBS = ../modules/libmodules.a ../snmplib/libsnmplib.a

all: $(EXEC)

test:

$(EXEC): $(OBJS) $(USELIBS)
	$(CC) $(LDFLAGS) -static -o $@ $(OBJS) $(USELIBS) $(LDLIBS)

romfs:
	$(ROMFSINST) -S init.d/W90snmpd /etc/rc.d/W90snmpd
	$(ROMFSINST) -s /etc/rc.d/W90snmpd /etc/init.d/snmpd
	$(ROMFSINST) /bin/$(EXEC)

clean:
	-rm -f $(EXEC) *.gdb *.elf *.o
