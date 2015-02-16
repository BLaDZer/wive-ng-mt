#
# (C) Copyright 2002
# Wolfgang Denk, DENX Software Engineering, wd@denx.de.
#
# See file CREDITS for list of people who contributed to this
# project.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston,
# MA 02111-1307 USA
#

CFLAGS+=$(SSP_ALL_CFLAGS) -DUSE_HOSTCC -fPIC -D_LARGEFILE64_SOURCE=1
CFLAGS+=-I$(LIBCDIRSHARED)/include

LIBNVRAM=libnvram.a
LIBNVRAM_SHARED=libnvram.so
LIBNVRAM_SHARED_FULLNAME=libnvram.so

OBJS	:= nvram_env.o
HEADERS := nvram.h $(ROOTDIR)/lib/shared/include/linux/autoconf.h

all: $(LIBNVRAM)
	make shared

$(OBJS): %.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(STRIPTOOL) -x -R .note -R .comment $*.o

$(LIBNVRAM) ar-target: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBNVRAM) $(OBJS)

shared:
	$(LD) -shared -soname=$(LIBNVRAM_SHARED) \
		-o $(LIBNVRAM_SHARED_FULLNAME) --whole-archive $(LIBNVRAM) \
		--no-whole-archive \
		-L$(TOPDIR)lib -lc;
	cp -frv libnvram.so ../lib/lib
	cp -frv libnvram.so $(LIBCDIRSHARED)/lib

$(OBJS): $(HEADERS) Makefile

clean:
	$(RM) *.[oa] $(LIBNVRAM_SHARED)* $(LIBNVRAM_SHARED_FULLNAME)*
