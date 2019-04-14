.EXPORT_ALL_VARIABLES:

CFLAGS  += -Wall -Wextra -Wshadow -Wformat=2 -Wfloat-equal -Wlogical-op -Wcast-qual -Wno-unused-parameter -Wno-sign-compare -Werror -Wfatal-errors

VGEN_EXEC = versiongen
VGEN_OBJS = versiongen.o

# allways rebuild
all: clean $(VGEN_EXEC)

$(VGEN_EXEC): $(VGEN_OBJS)
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) $(LDFLAGS) $(LDLIBS) -o $(VGEN_EXEC) $(VGEN_OBJS)

install:
	echo ok

clean:
	rm -f *.o $(VGEN_EXEC)
