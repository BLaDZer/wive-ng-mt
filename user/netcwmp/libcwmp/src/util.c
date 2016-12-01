/************************************************************************
 * Id: util.c                                                           *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/


#include "cwmp/util.h"
#include "cwmp/log.h"
#include "cwmp/cfg.h"
#include "cwmp/md5.h"

#include <paths.h>
#include <sys/types.h>
#include <signal.h>

/* TODO: to libwive */
#define UPGRADE_FILE _PATH_TMP "/z_upgrade_firmware"

const char * upgrade_status_to_string(enum upgrade_status s)
{
	switch (s) {
		case UPGRADE_NONE:
			return NULL;
		case UPGRADE_DOWNLOAD:
			return "download";
		case UPGRADE_CHECK:
			return "check";
		case UPGRADE_PREPARE:
			return "prepare";
		case UPGRADE_WRITE:
			return "upgrade";
	}
	return NULL;
}

enum upgrade_status upgrade_get_status()
{
	char status[32] = {};
	int pid = -1;
	FILE *f = NULL;
	if (access(UPGRADE_FILE, F_OK)) {
		/* file not exists */
		return UPGRADE_NONE;
	}

	if (access(UPGRADE_FILE, R_OK)) {
		/* file exists, but not readable */
		cwmp_log_critical(
				"firmware upgrade: file '%s' not available for reading: %s",
				UPGRADE_FILE, strerror(errno));
		/* unknown behavior */
		return UPGRADE_DOWNLOAD;
	}

	f = fopen(UPGRADE_FILE, "r");
	if (!f) {
		cwmp_log_critical(
				"firmware upgrade: file '%s' not available for reading: %s",
				UPGRADE_FILE, strerror(errno));
		/* unknown behavior */
		return UPGRADE_DOWNLOAD;
	}
	fscanf(f, "%d:%31s", &pid, status);
	fclose(f);

	/* check program state */
	if (kill(pid, 0)) {
		/* program not running */
		unlink(UPGRADE_FILE);
		return UPGRADE_NONE;
	}

	if (!strcmp(status, "download")) {
		return UPGRADE_DOWNLOAD;
	} else if (!strcmp(status, "check")) {
		return UPGRADE_CHECK;
	} else if (!strcmp(status, "prepare")) {
		return UPGRADE_PREPARE;
	} else if (!strcmp(status, "upgrade")) {
		return UPGRADE_WRITE;
	}

	return UPGRADE_NONE;
}

bool upgrade_set_status(enum upgrade_status s)
{
	enum upgrade_status so = UPGRADE_NONE;
	const char *status = NULL;
	FILE *f = NULL;

	status = upgrade_status_to_string(s);

	if ((so = upgrade_get_status()) != UPGRADE_NONE) {
/*		cwmp_log_error(
				"firmware upgrade: set status to '%s' failed: "
				"already in status: %s",
				status, upgrade_status_to_string(so));*/
	}

	if (s == UPGRADE_NONE) {
		if (!access(UPGRADE_FILE, F_OK)) {
			/* file exists */
			if (!unlink(UPGRADE_FILE)) {
				/* delete file ok */
				return true;
			}
			/* delete file failed */
			return false;
		}
		/* file not exists */
		return true;
	}

	f = fopen(UPGRADE_FILE, "w");
	if (!f) {
		cwmp_log_error(
				"firmware upgrade: file %s not available for writing: %s",
				UPGRADE_FILE, strerror(errno));
		return false;
	}

	fprintf(f, "%d:%s", getpid(), status);
	fclose(f);
	return true;
}


static const char base64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

char * cwmp_base64_encode(const char *src)
{
    char *str, *dst;
    size_t l;
    int t, r;

    l = strlen(src);
    if ((str = malloc(((l + 2) / 3) * 4 + 1)) == 0)
        return (void*)0;
    dst = str;
    r = 0;

    while (l >= 3)
    {
        t = (src[0] << 16) | (src[1] << 8) | src[2];
        dst[0] = base64[(t >> 18) & 0x3f];
        dst[1] = base64[(t >> 12) & 0x3f];
        dst[2] = base64[(t >> 6) & 0x3f];
        dst[3] = base64[(t >> 0) & 0x3f];
        src += 3;
        l -= 3;
        dst += 4;
        r += 4;
    }

    switch (l)
    {
    case 2:
        t = (src[0] << 16) | (src[1] << 8);
        dst[0] = base64[(t >> 18) & 0x3f];
        dst[1] = base64[(t >> 12) & 0x3f];
        dst[2] = base64[(t >> 6) & 0x3f];
        dst[3] = '=';
        dst += 4;
        r += 4;
        break;
    case 1:
        t = src[0] << 16;
        dst[0] = base64[(t >> 18) & 0x3f];
        dst[1] = base64[(t >> 12) & 0x3f];
        dst[2] = dst[3] = '=';
        dst += 4;
        r += 4;
        break;
    case 0:
        break;
    }

    *dst = 0;
    return (str);
}

void cwmp_hex_to_string(char *to, const unsigned char *p, size_t len)
{
    const char  *hex = "0123456789abcdef";

    for (;len--; p++)
    {
        *to++ = hex[p[0] >> 4];
        *to++ = hex[p[0] & 0x0f];

    }
    *to = '\0';
}

void cwmp_string_to_hex(char *in, char *out, size_t len)
{
    const char hex[16] = "0123456789ABCDEF";
    size_t i = 0;;
    size_t in_len = 0;

    if (!out || !len || !in)
        return;
    in_len = strlen(in);

    for (i = 0u; i < len - 1 && i < in_len * 2; i++) {
        if (!(i % 2)) {
            /* first nibble */
            out[i] = hex[in[i / 2] >> 4 & 0xf];
        } else {
            /* second nibble */
            out[i] = hex[in[i / 2] & 0xf];
        }
    }

    out[i] = '\0';
}

void MD5(char *buf, ...)
{
    unsigned char   *p;
    va_list ap;
    MD5_CTX ctx;

    MD5Init(&ctx);
	/*cwmp_log_debug("MD5(begin, target=%p)", buf);*/
    va_start(ap, buf);
    while ((p = va_arg(ap, unsigned char *)) != NULL)
    {
		/*cwmp_log_debug("MD5(input): '%s', %d", p, strlen((char*)p));*/
        MD5Update(&ctx, p, strlen((char *) p));
    }
    va_end(ap);
	/*cwmp_log_debug("MD5(end)");*/

    MD5Final((unsigned char*)buf, &ctx);
}

void
string_randomize(char *buffer, size_t size)
{
	const char base[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz0123456789+";
	size_t i;

	for (i = 0u; i < size; i++) {
		buffer[i] = base[rand() % (sizeof(base) - 1)];
	}
}

void convert_to_hex(const char *Bin, char *Hex)
{
    unsigned short i;
    unsigned char j;
    for (i = 0; i < 16; i++)
    {
        j = (Bin[i] >> 4) & 0xf;
        if (j <= 9)
            Hex[i*2] = (j + '0');
        else
            Hex[i*2] = (j + 'a'-10);
        j = Bin[i] & 0xf;
        if (j <= 9)
            Hex[i*2+1] = (j + '0');
        else
            Hex[i*2+1] = (j + 'a'-10);
    }
    Hex[32] = '\0';
}

//FIXME: Split to separate library
char *strip_space(char *str)
{
    while (isspace(*str))
	    str++;
    return str;
}

char* getIntIp(pool_t * pool)
{
    char if_addr[16];
    int vpn_mode_enabled = cwmp_nvram_get_bool_onoff("vpnEnabled", 0);
    int vpnDGW = cwmp_nvram_get_int("vpnDGW", 0);

    if (vpn_mode_enabled && vpnDGW) {
        if (getIfIp(getPPPIfName(), if_addr) != -1) {
            cwmp_log_debug("getIntIp R %s", if_addr);
            return pool_pstrdup(pool, if_addr);
        }
    }

    /* if vpn disabled always get ip from wanif */
    if (getIfIp(getWanIfName(), if_addr) != -1) {
        cwmp_log_debug("getIntIp R %s", if_addr);
        return pool_pstrdup(pool, if_addr);
    }


    return 0;
}

size_t nvram_get_tuple(const char *key, unsigned index, char *value, size_t value_size)
{
    char *v = NULL;
    char *e = NULL;
    char *s = NULL;
    unsigned i = 0;
    size_t len = 0u;

    /* indexes started at 1 */
    index++;

    e = s = v = cwmp_nvram_get(key);
    len = strlen(v);
    while ((e = strchr(s, ';')) != NULL) {
        if (++i == index)
            break;
        /* next */
        s = ++e;
    }
    /* fix endpos */
    if (!e) {
        e = v + len;
        i++;
    }

    if (i != index) {
        s = e;
        cwmp_log_error("%s: invalid index: %u, maximum: %u",
                __func__, index, i);
    }

    memset(value, 0u, value_size);
    len = (e - s);
    if (len && value) {
        snprintf(value, value_size, "%.*s", len, s);
    }
    return len;
}


////////////////////////////////////////////////////////

//#ifdef UPLOAD_FIRMWARE_SUPPORT

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

char* ReadFile(char *name, unsigned long *fileLen)
{
    FILE *file;
    char *buffer;
//    unsigned long fileLen;

    //Open file
    file = fopen(name, "rb");
    if (!file)
    {
	fprintf(stderr, "Unable to open file %s", name);
	return NULL;
    }

    //Get file length
    fseek(file, 0, SEEK_END);
    *fileLen=ftell(file);
    fseek(file, 0, SEEK_SET);

    //Allocate memory
    buffer=(char *)malloc((*fileLen)+1);
    if (!buffer)
    {
	fprintf(stderr, "Memory error!");
                                fclose(file);
	return NULL;
    }

    //Read file contents into buffer
    fread(buffer, *fileLen, 1, file);
    fclose(file);

    return buffer;
}

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
	while(fgets(buf, sizeof(buf), fp)){
		sscanf(buf, "%s %s %s %s", dev, size, erase, name);
		if(!strcmp(name, part)){
			result = strtol(size, NULL, 16);
			break;
		}
	}
	fclose(fp);
	return result;
}

static int mtd_write_firmware(char *filename, int offset, int len)
{
    char cmd[512];
    int status;
    int err=0;

/* check image size before erase flash and write image */
#ifdef CONFIG_RT2880_ROOTFS_IN_FLASH
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s Kernel_RootFS", offset, len, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#else
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s Kernel", offset,  CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;

    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s RootFS", offset + CONFIG_MTD_KERNEL_PART_SIZ, len - CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#endif
#elif defined(CONFIG_RT2880_ROOTFS_IN_RAM)
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -r -o %d -l %d write %s Kernel", offset, len, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#else
    fprintf(stderr, "goahead: no CONFIG_RT2880_ROOTFS defined, %s\n", __FUNCTION__);
#endif
    if (err == 0)
        return 0;

    fprintf(stderr, "mtd_write return error - image oversized or uncorrect!!!%d, %s", len, __FUNCTION__);
    return -1;
}

/*
 *  taken from "mkimage -l" with few modified....
 */
static int checkimage(char *imagefile, int offset, int len)
{
	struct stat sbuf;

	int  data_len;
	char *data;
	unsigned char *ptr;
	unsigned long checksum;

	image_header_t header;
	image_header_t *hdr = &header;

	int ifd;

	if ((unsigned)len < sizeof(image_header_t)) {
		fprintf(stderr,"Bad size: \"%s\" is no valid image\n", imagefile);
		return 0;
	}

	ifd = open(imagefile, O_RDONLY);
	if(!ifd){
		fprintf(stderr,"Can't open %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	if (fstat(ifd, &sbuf) < 0) {
		close(ifd);
		fprintf(stderr,"Can't stat %s: %s\n", imagefile, strerror(errno));
		return 0;
	}

	ptr = (unsigned char *) mmap(0, sbuf.st_size, PROT_READ, MAP_SHARED, ifd, 0);
	if ((caddr_t)ptr == (caddr_t)-1) {
		close(ifd);
		fprintf(stderr,"Can't mmap %s: %s\n", imagefile, strerror(errno));
		return 0;
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
		return 0;
	}

	data = (char *)hdr;

	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = htonl(0);	/* clear for re-calculation */

	if (crc32 (0u, (unsigned char*) data, sizeof(image_header_t)) != checksum)
	{
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: \"%s\" has bad header checksum!\n", imagefile);
		return 0;
	}

	/*
	 *  handle Data CRC32
	 */
	data = (char *)(ptr + sizeof(image_header_t));
	data_len  = len - sizeof(image_header_t) ;

	if (crc32 (0, (unsigned char *)data, data_len) != ntohl(hdr->ih_dcrc))
	{
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: \"%s\" has corrupted data!\n", imagefile);
		return 0;
	}

	/*
	 * compare MTD partition size and image size
	 */
#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH)
#ifdef CONFIG_ROOTFS_IN_FLASH_NO_PADDING
	if(len > MAX_IMG_SIZE || len > getMTDPartSize("\"Kernel_RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) is bigger than Kernel_RootFS MTD partition.\n", len);
		return 0;
	}
#else
	if(len > MAX_IMG_SIZE || len < CONFIG_MTD_KERNEL_PART_SIZ){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) size doesn't make sense.\n", len);
		return 0;
	}

	if((len - CONFIG_MTD_KERNEL_PART_SIZ) > getMTDPartSize("\"RootFS\"")){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) is bigger than RootFS MTD partition.\n", len - CONFIG_MTD_KERNEL_PART_SIZ);
		return 0;
	}
#endif
#elif defined(CONFIG_RT2880_ROOTFS_IN_RAM)
	if(len > MAX_IMG_SIZE || len > getMTDPartSize("\"Kernel\"")){
		munmap(ptr, len);
		close(ifd);
		fprintf(stderr,"*** ERROR: the image file(0x%x) is bigger than Kernel MTD partition, %s\n", len, __FUNCTION__);
		return 0;
	}
#else
#error "goahead: no CONFIG_RT2880_ROOTFS defined!"
#endif
	munmap(ptr, len);
	close(ifd);

	return 1;
}

int firmware_upgrade(char* filename)
{
	unsigned long file_size = 0;

	cwmp_log_trace("%s(filename='%s')", __func__, filename);

	if (!upgrade_set_status(UPGRADE_CHECK)) {
		return 3;
	}

	char* buffer = ReadFile(filename, &file_size);
	if (buffer == NULL)
	{
	    cwmp_log_error("Check image error: unable to read image file: %s", filename);
		upgrade_set_status(UPGRADE_NONE);
	    return 1;
	}

#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH)
	if(file_size > MAX_IMG_SIZE || file_size < MIN_FIRMWARE_SIZE){
		cwmp_log_error("Check image error: size incompatible image. Size: %d", (int)file_size);
		upgrade_set_status(UPGRADE_NONE);
		return 2;
	}
#endif

	// check image
	if (!checkimage(filename, 0, (int)file_size))
	{
		cwmp_log_error("Check image error: corrupted or uncompatable image. Size: %d", (int)file_size);
		upgrade_set_status(UPGRADE_NONE);
		return 3;
	}

	upgrade_set_status(UPGRADE_PREPARE);
	system("fs restore > /dev/null 2>&1");

	// flash write
	upgrade_set_status(UPGRADE_WRITE);
	if (mtd_write_firmware(filename, 0, (int)file_size) == -1) {
		cwmp_log_error("MTD_WRITE ERROR: NEED RESTORE OVER RECOVERY MODE!!!");
		upgrade_set_status(UPGRADE_NONE);
		return -1;
	}

	upgrade_set_status(UPGRADE_NONE);

//	sleep (3);
//	reboot(RB_AUTOBOOT);
	return 0;
}
//#else
//#error "no upload support defined!"
//#endif

