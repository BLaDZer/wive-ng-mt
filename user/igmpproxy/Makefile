DIRS = src

all:
	for i in $(DIRS) ; do make -C $$i $@ || exit $?; done

romfs:
	for i in $(DIRS) ; do make -C $$i $@ ; done
	$(ROMFSINST) igmpproxy.conf /etc/igmpproxy.conf

clean:
	for i in $(DIRS) ; do make -C $$i clean ; done

