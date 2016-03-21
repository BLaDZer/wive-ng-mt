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
/*
static const char base64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static char * base64_encode(const char *src)
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
*/
static void cwmp_hex_to_string(char *to, const unsigned char *p, size_t len)
{
    const char  *hex = "0123456789abcdef";

    for (;len--; p++)
    {
        *to++ = hex[p[0] >> 4];
        *to++ = hex[p[0] & 0x0f];

    }
    *to = '\0';
}



void MD5(char *buf, ...)
{
    unsigned char   hash[HASHLEN];
    unsigned char   *p;
    va_list ap;
    MD5_CTX ctx;

    MD5Init(&ctx);

    va_start(ap, buf);
    while ((p = va_arg(ap, unsigned char *)) != NULL)
    {
        MD5Update(&ctx, p, strlen((char *) p));
    }
    va_end(ap);

    MD5Final(hash, &ctx);

    cwmp_hex_to_string(buf, hash, sizeof(hash));
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


int getIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s\n", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s\n", __FUNCTION__);
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

	close(skfd);
	return 0;
}


char* getWanIfName(pool_t * pool)
{
    int mode = cwmp_nvram_get_int("OperationMode", -1);
    int apc_cli_mode = cwmp_nvram_get_int("ApCliBridgeOnly", 0);

    char *if_name = WAN_DEF;
    char wan_if[16]; /* max 16 char in wan if name */
    FILE *fp;

    /* try read fron file exported from init.d */
    fp = fopen("/tmp/wan_if_name", "r");
    if (fp) {
        /* get first wan_if in file */
        while (fgets(wan_if, sizeof(wan_if), fp)) {
	if (wan_if == NULL || wan_if[0] == '\n')
	    continue;
	if ((strstr(wan_if, ETH_SIG) != NULL) || (strstr(wan_if, BR_SIG) != NULL)) {
	    fclose(fp);
	    return strip_space(wan_if);
	}
        }
        fclose(fp);
    }

    if_name = WAN_DEF;

    switch (mode) 
    {
	case 0: if_name = "br0";break;
//	case 1: case 4: if_name = WAN_DEF;break;
	case 2: if_name = "ra0";break;
	case 3:
	    if (apc_cli_mode == 1) {
		if_name = "br0";				/* Client-AP-Bridge */
	    } else {
		char *apc_cli_wanif = cwmp_nvram_pool_get(pool, "ApCliIfName");
		if (apc_cli_wanif != NULL)
		{
	    	    if_name = apc_cli_wanif;			/* Client-AP-Gateway 2.4Ghz/5GHz */
		}
	    }
	    break;
    }

    return if_name;
}


char* getPPPIfName(void)
{
        FILE *fp;
    char ppp_if[16]; /* max 16 char in vpn if name */

    fp = fopen("/tmp/vpn_if_name", "r");
    if (fp) {
        /* get first ppp_if in file */
        while (fgets(ppp_if, sizeof(ppp_if), fp)) {
	if (ppp_if == NULL || ppp_if[0] == '\n')
	    continue;
	if (strstr(ppp_if, VPN_SIG) != NULL) {
	    fclose(fp);
	    return strip_space(ppp_if);
	}
        }
        fclose(fp);
    }

    return VPN_DEF;
}


char* getIntIp(pool_t * pool)
{
    char if_addr[16];
    int vpn_mode_enabled = cwmp_nvram_get_bool_onoff("vpnEnabled", 0);
    int vpnDGW = cwmp_nvram_get_int("vpnDGW", 0);

    if (vpn_mode_enabled && vpnDGW) {
        if (getIfIp(getPPPIfName(), if_addr) != -1) {
	return pool_pstrdup(pool, if_addr);
        }
    }

    /* if vpn disabled always get ip from wanif */
    if (getIfIp(getWanIfName(pool), if_addr) != -1) {
	cwmp_log_error("getIntIp R %s", if_addr);
	return pool_pstrdup(pool, if_addr);
    }

    cwmp_log_error("getIntIp RETURN 0");

    return 0;
}


/* Port statistics */
int getHWStatistic(unsigned long long* rxtx_count) {
#ifdef CONFIG_RAETH_SNMPD
	char buf[1024];
	FILE *fp;
#endif
	rxtx_count[0] = rxtx_count[1] = rxtx_count[2] = rxtx_count[3] = rxtx_count[4] = rxtx_count[5] = rxtx_count[6] = rxtx_count[7] = rxtx_count[8] = rxtx_count[9] = rxtx_count[10] = rxtx_count[11] = 0;

#ifdef CONFIG_RAETH_SNMPD
	fp = fopen(PROCREG_SNMP, "r");
	if (fp == NULL) {
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp)) {
		if (buf == NULL || buf[0] == '\n')
		    continue;
		if (6 == sscanf(buf, "rx64 counters: %llu %llu %llu %llu %llu %llu\n", &rxtx_count[0], &rxtx_count[1], &rxtx_count[2], &rxtx_count[3], &rxtx_count[4], &rxtx_count[5]))
		    continue;
		if (6 == sscanf(buf, "tx64 counters: %llu %llu %llu %llu %llu %llu\n", &rxtx_count[6], &rxtx_count[7], &rxtx_count[8], &rxtx_count[9], &rxtx_count[10], &rxtx_count[11]))
		    break;
	}
	fclose(fp);
#endif

	int i;
	for (i=0;i<12;i++) cwmp_log_error("RX/TX Count %i : %llu", i, rxtx_count[i]);

	return 0;
}

int getIfMac(char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s\n", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s\n", __FUNCTION__);
		return -1;
	}

	ptr = (char *)&ifr.ifr_addr.sa_data;
	sprintf(if_hw, "%02X%02X%02X%02X%02X%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
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
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel_RootFS", offset, len, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#else
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel", offset,  CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;

    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s RootFS", offset + CONFIG_MTD_KERNEL_PART_SIZ, len - CONFIG_MTD_KERNEL_PART_SIZ, filename);
    status = system(cmd);
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	err++;
#endif
#elif defined(CONFIG_RT2880_ROOTFS_IN_RAM)
    snprintf(cmd, sizeof(cmd), "/bin/mtd_write -o %d -l %d write %s Kernel", offset, len, filename);
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

	char* buffer = ReadFile(filename, &file_size);
	if (buffer == NULL) 
	{
	    cwmp_log_error("Check image error: unable to read image file: %s", filename);
	    return 1;
	}

#if defined(CONFIG_RT2880_ROOTFS_IN_FLASH)
	if(file_size > MAX_IMG_SIZE || file_size < MIN_FIRMWARE_SIZE){
		cwmp_log_error("Check image error: size incompatible image. Size: %d", (int)file_size);
    		return 2;
	}
#endif

	// check image
	if (!checkimage(filename, 0, (int)file_size))
	{
		cwmp_log_error("Check image error: corrupted or uncompatable image. Size: %d", (int)file_size);
		return 3;
	}

	// flash write
	if (mtd_write_firmware(filename, 0, (int)file_size) == -1) {
		cwmp_log_error("MTD_WRITE ERROR: NEED RESTORE OVER RECOVERY MODE!!!");
		return -1;
	}

//	sleep (3);
//	reboot(RB_AUTOBOOT);
	return 0;
}
//#else
//#error "no upload support defined!"
//#endif
