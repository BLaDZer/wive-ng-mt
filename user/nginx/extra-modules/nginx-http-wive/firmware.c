#include "firmware.h"

/* ========================================================================
 * Table of CRC-32's of all single-byte values (made by make_crc_table)
 */
static const unsigned long crc_table[256] = {
  0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
  0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
  0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
  0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
  0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
  0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
  0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
  0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
  0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
  0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
  0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
  0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
  0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
  0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
  0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
  0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
  0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
  0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
  0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
  0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
  0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
  0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
  0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
  0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
  0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
  0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
  0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
  0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
  0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
  0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
  0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
  0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
  0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
  0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
  0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
  0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
  0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
  0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
  0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
  0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
  0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
  0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
  0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
  0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
  0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
  0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
  0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
  0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
  0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
  0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
  0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
  0x2d02ef8dL
};

/* ========================================================================= */
#define DO1(buf) crc = crc_table[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
static unsigned long crc32 (unsigned long crc, const unsigned char *buf,  unsigned int len)
{
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if (len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

static unsigned int getMTDPartSize(char *part)
{
	char buf[128], name[32], size[32], dev[32], erase[32];
	unsigned int result=0;
	FILE *fp = fopen("/proc/mtd", "r");
	if(!fp){
		fprintf(stderr, "mtd support not enable?");
		return 0;
	}
	while((fgets(buf, sizeof(buf), fp)) != NULL){
		sscanf(buf, "%s %s %s %s", dev, size, erase, name);
		if(!strcmp(name, part)){
			result = strtol(size, NULL, 16);
			break;
		}
	}
	fclose(fp);
	return result;
}

const char* valid_content_type[]=
{
	"application/mac-binary",
	"application/macbinary",
	"application/octet-stream",
	"application/x-binary",
	"application/x-macbinary",
	NULL
};

int check_binary_content_type(const char *content_type)
{
	const char **ptr = valid_content_type;
	while (*ptr != NULL)
	{
		// Check content type validity
		if (strcasecmp(*ptr, content_type)==0)
			return 1;

		ptr++;
	}

	return 0;
}

int getFirmwareUpdateTime(int reset_rwfs)
{
	return 8*(IMAGE1_SIZE/0x100000) + 10 + (reset_rwfs?10:0);
}


int mtd_write_firmware(char *filename, int offset, int len)
{
    char cmd[512];
    int status;
    int err=0;

    system("echo -n flashing>/tmp/updater/state");

/* check image size before erase flash and write image */
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s Kernel_RootFS &", offset, len, filename);
    status = doSystem("%s", cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#else
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s Kernel &", offset,  CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = doSystem("%s", cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;

    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s RootFS &", offset + CONFIG_MTD_KERNEL_PART_SIZ, len - CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = doSystem("%s", cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#endif
#elif defined(CONFIG_RT2880_ROOTFS_IN_RAM)
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s Kernel &", offset, len, filename);
    status = doSystem("%s", cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#else
    fprintf(stderr, "goahead: no CONFIG_RT2880_ROOTFS defined, %s\n", __FUNCTION__);
#endif
    if (err == 0)
        return 0;

    system("echo -n error>/tmp/updater/state");
    fprintf(stderr, "mtd_write return error - image oversized or uncorrect!!!%d, %s", len, __FUNCTION__);
    return -1;
}

/*
 *  taken from "mkimage -l" with few modified....
 */
static int checkimage(webs_t* wp, char *imagefile, int offset, int len)
{
	struct stat sbuf;

	int  data_len;
	unsigned char *data;
	unsigned char *ptr;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		fprintf(stderr,"Bad size: \"%s\" is no valid image\n", imagefile);
		return 1;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		fprintf(stderr,"Can't open %s: %s\n", imagefile, strerror(errno));
		return 2;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		fprintf(stderr,"Can't stat %s: %s\n", imagefile, strerror(errno));
		return 3;
	}

	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		fprintf(stderr,"Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 4;
	}
	ptr += offset;

	/*
	 *  handle Header CRC32
	 */
	memcpy (hdr, ptr, sizeof(image_header_t));

	if (ntohl(hdr->ih_magic) != IH_MAGIC)
	{
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"Bad Magic Number: \"%s\" is no valid image\n", imagefile);
		return 5;
	}

	data = (unsigned char *)hdr;

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = htonl(0);	/* clear for re-calculation */

	if (crc32 (0, data, sizeof(image_header_t)) != checksum)
	{
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: \"%s\" has bad header checksum!\n", imagefile);
		return 6;
	}

	/*
	 *  handle Data CRC32
	 */
	data = (unsigned char *)(ptr + sizeof(image_header_t));
	data_len  = len - sizeof(image_header_t) ;

	if (crc32 (0, data, data_len) != ntohl(hdr->ih_dcrc))
	{
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: \"%s\" has corrupted data!\n", imagefile);
		return 7;
	}

	/*
	 * compare MTD partition size and image size
	 */
#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH)
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
	if(len > MAX_IMG_SIZE || (unsigned)len > getMTDPartSize("\"Kernel_RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) is bigger than Kernel_RootFS MTD partition.\n", len);
		return 8;
	}
#else
	if(len > MAX_IMG_SIZE || len < CONFIG_MTD_KERNEL_PART_SIZ){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) size doesn't make sense.\n", len);
		return 9;
	}

	if((len - CONFIG_MTD_KERNEL_PART_SIZ) > getMTDPartSize("\"RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) is bigger than RootFS MTD partition.\n", len - CONFIG_MTD_KERNEL_PART_SIZ);
		return 10;
	}
#endif
#elif defined(CONFIG_RT2880_ROOTFS_IN_RAM)
	if(len > MAX_IMG_SIZE || (unsigned int)len > getMTDPartSize("\"Kernel\"")){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) is bigger than Kernel MTD partition, %s\n", len, __FUNCTION__);
		return 11;
	}
#else
#error "goahead: no CONFIG_RT2880_ROOTFS defined!"
#endif
	munmap(ptr, len);
	close(ifd);

	return 0;
}

void upload_html_header(webs_t* wp)
{
        websSetContentType(wp, "text/html");

        outWrite("\n<html>\n<head>\n"
		"<title>Import Settings</title>\n"
		"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
	);

	char data[2048];

	// Copy /js/ajax.js to stdout
	FILE *fd = fopen("/web/js/ajax.js", "r");
	if (fd != NULL)
	{
		size_t count;
		outWrite("<script type=\"text/javascript\">\n");
		outWrite("// Here is script copied from file /js/ajax.js\n");

		while ((count =          fread(data, sizeof(char), sizeof(data)/sizeof(char), fd)) > 0)
                {
                    data[count] = '\0';
                    outWrite("%s",data);
//			fwrite(data, sizeof(char), count, stdout);
                }

		fclose(fd);
	}
	else
		outWrite("<script type=\"text/javascript\" src=\"/js/ajax.js\">\n");

	// Output end of javascript
	outWrite
	(
		"</script>\n"
		"</head>\n"
		"<body>\n"
		"<h1>Update Settings</h1>\n"
	);

}

void upload_html_success(webs_t* wp, int timeout)
{
	outWrite
	(
		"<p>Done</p>\n"
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"ajaxReloadDelayedPage(%d000);\n"
		"</script>"
		"</body></html>\n\n",
		timeout
	);

	websDone(wp, 200);
}

void upload_html_error(webs_t* wp, char* err)
{
	outWrite
	(
		"<p>%s</p>\n"
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"alert('%s');\n"
		"ajaxReloadDelayedPage(0);\n"
		"</script>\n"
		"</body></html>\n\n",
		err, err
	);
}



int firmware_upgrade(webs_t* wp)
{
        ELOG_INFO(wp->request->connection->log, 0, "Firmware upgrade has been started.\n");

        char err_msg[512];
        long file_size = 0;
        int reset_rwfs = 0;

        char *filename = websGetVar(wp, "filename.path", NULL);
        char *filesize = websGetVar(wp, "filename.size", NULL);

        if (filesize != NULL)
        {
            file_size = strToIntDef(filesize, 0);
        }

        if (filename == NULL)
        {
            sprintf(err_msg, "No firmware filename provided!");
            ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
            upload_html_error(wp, err_msg);
            return 1;
        }

        FILE *fd = fopen(filename, "r");
        if (fd == NULL)
        {
            sprintf(err_msg, "Unable to open firmware!");
            ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
            upload_html_error(wp, err_msg);

            return 4;
        }

        fseek(fd, 0L, SEEK_END);
        unsigned long sz = ftell(fd);
        fclose(fd);

        if (sz != (unsigned long)file_size)
        {
            sprintf(err_msg, "Wrong firmware filesize %lu != %ld!", sz, file_size);
            ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
            upload_html_error(wp, err_msg);

            return 2;
        }

	// Find parameter containing NVRAM reset flag
	char_t *find = websGetVar(wp, "reset_rwfs", "off");
	if (find != NULL)
	{
            reset_rwfs = (strcasecmp(find, "on")==0);
	}

	// Find parameter containing file
	find = websGetVar(wp, "filename.name", NULL);
	if (find != NULL)
	{
		// Check if parameter is correct
                char * ctype = websGetVar(wp, "filename.content_type", NULL);
		if (ctype == NULL)
		{
                    sprintf(err_msg, "Unknown content-type for binary data");
                    ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
                    upload_html_error(wp, err_msg);
                    return 5;
		}

		if (!check_binary_content_type(ctype))
		{
                        sprintf(err_msg, "Unsupported content-type for binary data: %s", ctype);
                        ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
                        upload_html_error(wp, err_msg);

                        return 6;
		}

	} else {
                sprintf(err_msg, "No firmware binary file!");
                ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
                upload_html_error(wp, err_msg);

                return 7;
	}

#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH)
	if(file_size > MAX_IMG_SIZE || file_size < MIN_FIRMWARE_SIZE){
                sprintf(err_msg, "Check image error: oversized uncompatable image. Size: %d", (int)file_size);
                ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
                upload_html_error(wp, err_msg);
                return 8;
	}
#endif

        int errcode2 = checkimage(wp, filename, 0, (int)file_size);
	// check image
	if (errcode2 != 0)
	{
                sprintf(err_msg, "Check image error: corrupted or uncompatable image. Size: %d (err=%i)", (int)file_size, errcode2);
                ELOG_ERR(wp->request->connection->log, 0, "%s", err_msg);
                upload_html_error(wp, err_msg);
                return 9;
	}

	// reset NVRAM if needed
	// start web timer and crash rwfs BEFORE flash destroy
	if (reset_rwfs)
	{
		doSystem("%s", "fs restore");
	}

	upload_html_success(wp, getFirmwareUpdateTime(reset_rwfs));

	// flash write
        ELOG_INFO(wp->request->connection->log, 0, "Firmware upgrade started\n");
        websDone(wp, 200);

        doSystem("%s", "unload_all.sh");
        mtd_write_firmware(filename, 0, (int)file_size);

	return 0;
}
