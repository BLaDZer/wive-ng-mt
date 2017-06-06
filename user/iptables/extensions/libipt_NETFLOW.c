/*
 * iptables helper for NETFLOW target
 * <abc@telekom.ru>
 *
 *
 *   This file is part of NetFlow exporting module.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <xtables.h>

static struct option opts[] = {
  { 0 }
};

static void help(void)
{
	printf("NETFLOW target\n");
}

static int parse(int c, char **argv, int invert, unsigned int *flags,
	     const void  *entry,
	     struct xt_entry_target **targetinfo)

{
	return 1;
}

static void final_check(unsigned int flags)
{
}

static void save(const void *ip, const struct xt_entry_target *match)
{
}

static void print(const void *ip,
      const struct xt_entry_target *target,
      int numeric)
{
	printf("NETFLOW ");
}

static struct xtables_target netflow = { 
	.next		= NULL,
	.name		= "NETFLOW",
	.version	= XTABLES_VERSION,
	.size           = XT_ALIGN(0),
	.userspacesize  = XT_ALIGN(0),
	.help		= &help,
	.parse		= &parse,
	.final_check    = &final_check,
	.print		= &print,
	.save		= &save,
	.extra_opts     = opts
};

#ifndef _init
#define _init __attribute__((constructor)) _INIT
#endif
void _init(void)
{
	xtables_register_target(&netflow);
}
