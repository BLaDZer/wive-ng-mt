#ifndef __FIRMWARE_H__
#define __FIRMWARE_H__

#include "util.h"
#include "asp_mod_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/reboot.h>

#ifndef DEMO_MODE
#include <linux/autoconf.h>  				/* kernel config		*/
#include "../../../tools/mkimage/include/image.h"	/* For Uboot image header format */
/* for calculate max image size */
#include "../../../linux/drivers/mtd/ralink/ralink-flash.h"
#else
#include "stub/autoconf.h"  				/* kernel config stub */
#include "stub/image.h"					/* For Uboot image header format */
#include "stub/ralink-flash.h"
#endif


#define RFC_ERROR 		"RFC1867 ...."
#define MEM_SIZE        	1024
#define MEM_HALF        	512
#define MIN_FIRMWARE_SIZE       2097152 		/* minium firmware size(2MB) */


void upload_html_header(webs_t* wp);
void upload_html_success(webs_t* wp, int timeout);
void upload_html_error(webs_t* wp, char* err);
int firmware_upgrade(webs_t* wp);
int mtd_write_firmware(char *filename, int offset, int len);

#endif//__FIRMWARE_H__
