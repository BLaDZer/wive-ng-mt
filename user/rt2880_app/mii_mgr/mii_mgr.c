/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if.h>
#include <linux/mii.h>
#include <linux/types.h>

#include <linux/autoconf.h>
#include "ra_ioctl.h"

void show_usage(void)
{
	printf("mii_mgr -g -p [phy number] -r [register number]\n");
	printf("  Get: mii_mgr -g -p 3 -r 4\n\n");
	printf("mii_mgr -s -p [phy number] -r [register number] -v [0xvalue]\n");
	printf("  Set: mii_mgr -s -p 4 -r 1 -v 0xff11\n\n");
}

int main(int argc, char *argv[])
{
	int sk, opt, ret;
	char options[] = "gsp:r:v:?t";
	int method = 0;
	struct ifreq ifr;
	ra_mii_ioctl_data mii;

        if (argc < 4) {
                show_usage();
                return 0;
        }

	strncpy(ifr.ifr_name, "eth2", 5);
	ifr.ifr_data = &mii;

	while ((opt = getopt(argc, argv, options)) != -1) {
		switch (opt) {
			case 'g':
				method = RAETH_MII_READ;
				break;
			case 's':
				method = RAETH_MII_WRITE;
				if (argc < 6) {
				    show_usage();
				    return 0;
				}
				break;
			case 'p':
				mii.phy_id = strtoul(optarg, NULL, 10);
				break;
			case 'r':
#if defined (CONFIG_RALINK_MT7621)
				if(mii.phy_id == 31) {
					mii.reg_num = strtol(optarg, NULL, 16);
				} else {
					mii.reg_num = strtol(optarg, NULL, 10);
				}
#else
				mii.reg_num = strtol(optarg, NULL, 10);
#endif
				break;
			case 'v':
				mii.val_in = strtol(optarg, NULL, 16);
				break;
			case '?':
				show_usage();
				break;
		}
	}

	sk = socket(AF_INET, SOCK_DGRAM, 0);
	if (sk < 0) {
		printf("Open socket failed\n");
		return -1;
	}

	ret = ioctl(sk, method, &ifr);
	if (ret < 0)
	    printf("mii_mgr: ioctl error\n");
	else
	    switch (method) {
		case RAETH_MII_READ:
			printf("Get: phy[%d].reg[%d] = %04x\n",
					mii.phy_id, mii.reg_num, mii.val_out);
			break;
		case RAETH_MII_WRITE:
			printf("Set: phy[%d].reg[%d] = %04x\n",
					mii.phy_id, mii.reg_num, mii.val_in);
			break;
	    }

	close(sk);
	return ret;
}
