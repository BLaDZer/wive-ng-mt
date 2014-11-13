#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/autoconf.h>  //kernel config

#include "mtd.h"

#if defined (CONFIG_RALINK_RT6855A) || defined (CONFIG_RALINK_MT7621)
#define LAN_OFFSET    0xE000
#define WAN_OFFSET    0xE006
#elif defined (CONFIG_RALINK_RT3883)
#define LAN_OFFSET    0x04
#define WAN_OFFSET    0x0A
#else
#define LAN_OFFSET    0x28
#define WAN_OFFSET    0x2E
#endif

#define MACADDR_LEN 	6

#define MEMGETINFO	_IOR('M', 1, struct mtd_info_user)
#define MEMERASE	_IOW('M', 2, struct erase_info_user)
#define MEMUNLOCK	_IOW('M', 6, struct erase_info_user)

static int mtd_open(const char *name, int flags)
{
	FILE *fp;
	char dev[80];
	int i, ret;

	if ((fp = fopen("/proc/mtd", "r"))) {
		while (fgets(dev, sizeof(dev), fp)) {
			if (sscanf(dev, "mtd%d:", &i) && strstr(dev, name)) {
				snprintf(dev, sizeof(dev), "/dev/mtd%d", i);
				ret = open(dev, flags);
				if (ret < 0) {
					snprintf(dev, sizeof(dev), "/dev/mtd/%d", i);
					ret = open(dev, flags);
				}
				fclose(fp);
				return ret;
			}
		}
		fclose(fp);
	}
	return -1;
}

static int mtd_read(char *side)
{
	int fd;
	int i = 0, ret =0;
	unsigned char mac_addr[MACADDR_LEN];

	fd = mtd_open("Factory", O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
	    fprintf(stderr, "Could not open mtd device\n");
	    return -1;
	}

	if (!strcmp(side, "wan"))
	    lseek(fd, WAN_OFFSET, SEEK_SET);
	else
	    lseek(fd, LAN_OFFSET, SEEK_SET);

	ret = read(fd, mac_addr, MACADDR_LEN);
	if (ret == -1) {
	    fprintf(stderr, "Reading from mtd failed\n");
	    close(fd);
	    return -1;
	}

	for (i = 0; i < MACADDR_LEN; i++) {
	    printf("%02X", mac_addr[i]);
	    if (i < MACADDR_LEN-1)
		printf(":");
	    else
		printf("\n");
	}

	close(fd);
	return 0;
}

static int mtd_write(char *side, char **value)
{
	int size = 0;
	int i;
	struct mtd_info_user mtdInfo;
	struct erase_info_user mtdEraseInfo;
	int fd;
	unsigned char *buf, *ptr;

	fd = mtd_open("Factory", O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "Could not open mtd device\n");
		return -1;
	}

	if (ioctl(fd, MEMGETINFO, &mtdInfo)) {
		fprintf(stderr, "Could not get mtd device info\n");
		close(fd);
		return -1;
	}

	mtdEraseInfo.length = mtdInfo.erasesize;
	buf = malloc(sizeof(unsigned char) * mtdInfo.erasesize);
	if (!buf) {
		fprintf(stderr, "No memory left\n");
		close(fd);
		exit(1);
	}

	size = mtdInfo.erasesize;
	if(read(fd, buf, size) != size) {
		fprintf(stderr, "Could not read Factory part\n");
		goto write_fail;
	}

	if (!strcmp(side, "wan"))
	    ptr = buf + WAN_OFFSET;
	else
	    ptr = buf + LAN_OFFSET;

	/* replace mac in buffer */
	for (i = 0; i < MACADDR_LEN; i++, ptr++) {
	    if (value[i] == NULL) {
		fprintf(stderr, "Incorrect command line format. Use eth_mac w <lan|wan> <MACADDR[0]> <MACADDR[1]> (space separator of octets).\n");
		goto write_fail;
	    }
	    *ptr = strtoul(value[i], NULL, 16);
	}

	printf("Erase part, recrate and write new factory.\n");
	for (mtdEraseInfo.start = 0; mtdEraseInfo.start < mtdInfo.size; mtdEraseInfo.start += mtdInfo.erasesize) {
		if(ioctl(fd, MEMUNLOCK, &mtdEraseInfo)) {
			fprintf(stderr, "Failed to unlock Factory part at 0x%x\n", mtdEraseInfo.start);
			goto write_fail;
		}

		if(ioctl(fd, MEMERASE, &mtdEraseInfo)) {
			fprintf(stderr, "Failed to erase block on Factory part at 0x%x\n", mtdEraseInfo.start);
			goto write_fail;
		}
	}

	lseek(fd, 0, SEEK_SET);
	if (write(fd, buf, size) != size) {
	    fprintf(stderr, "write() Factory part failed\n");
	    goto write_fail;
	}
	printf("Write new Factory OK\n");

	close(fd);
	return 0;
write_fail:
	close(fd);
	free(buf);
	return -1;
}

static void usage(char **str)
{
	printf("How to use:\n");
	printf("\tread:   %s r <lan|wan>\n", str[0]);
	printf("\twrite:  %s w <lan|wan> <MACADDR[0]> <MACADDR[1]> (space separator of octets)...\n", str[0]);
}

int main(int argc,char **argv)
{
	char op;

	if (argc < 3)
		goto CmdFail;

	op = *(argv[1]);
	switch (op) {
	case 'r':
		if (mtd_read(argv[2]) < 0)
			goto Fail;
		break;
	case 'w':
		if (argc < 4)
			goto CmdFail;
		if (mtd_write(argv[2], argv+3) < 0)
			goto Fail;
		break;
	default:
		goto CmdFail;
	}

	return 0;
CmdFail:
	usage(argv);
Fail:
	return -1;
}
