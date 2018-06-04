#ifndef _WIVE_H
#define _WIVE_H		1

#include <linux/autoconf.h>			//kernel config
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <sys/reboot.h>
#include <linux/reboot.h>
#include <linux/sockios.h>
#include <linux/ethtool.h>
#include <linux/if_ether.h>
#include <linux/wireless.h>
#include <linux/route.h>
#include <linux/sockios.h>
#include <dirent.h>
#include <syslog.h>

#include <pwd.h>
#include <crypt.h>
#include <shadow.h>

#include <libnvram.h>

#include "libwive_ethernet.h"
#include "libwive_gpio.h"
#include "libwive_helpers.h"
#include "libwive_network.h"
#include "libwive_procps.h"
#include "libwive_sysstat.h"
#include "libwive_system.h"
#include "libwive_wireless.h"
#include "config/autoconf.h"

#endif
