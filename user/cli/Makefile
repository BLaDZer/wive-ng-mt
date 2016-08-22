.EXPORT_ALL_VARIABLES:

CFLAGS  += -Wall -Wextra -Wshadow -Wformat=2 -Wfloat-equal -Wlogical-op -Wcast-qual -Wno-unused-parameter -Wno-sign-compare -Werror -Wfatal-errors
CFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS += -Wl,--gc-sections
LDLIBS  += -lwive -lnvram

CLI_EXEC = cli
CLI_OBJS = cli.o cli_fw.o cli_sw.o cli_wl.o

all: $(CLI_EXEC)

$(CLI_EXEC): $(CLI_OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(CLI_EXEC) $(CLI_OBJS)

install:
	echo ok

romfs:
	$(ROMFSINST) -S /bin/$(CLI_EXEC)
	$(ROMFSINST) -s /bin/cli /bin/fw
	$(ROMFSINST) -s /bin/cli /bin/sw
	$(ROMFSINST) -s /bin/cli /bin/wl

clean:
	rm -f *.o $(CLI_EXEC)
