#include "libnvram.h"

#include <syslog.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "oid.h"        /* for max bssid num */

//#define DEBUG

#ifdef DEBUG
static char libnvram_debug = 1;
#else
static char libnvram_debug = 0;
#endif

/* Config part in nvram */
static block_t fb[FLASH_BLOCK_NUM] =
{
	{
		.name = "2860",
		.flash_offset =  0x2000,
		.flash_max_len = ENV_BLK_SIZE*4,
		.valid = 0
	},
#if !defined(CONFIG_RT_SECOND_IF_NONE) && defined(CONFIG_KERNEL_NVRAM_SPLIT_INIC)
	{
		.name = "rtdev",
		.flash_offset = 0x6000,
		.flash_max_len = ENV_BLK_SIZE*2,
		.valid = 0
	},
#endif
};

#define RANV_PRINT(x, ...) do { if (libnvram_debug) printf("%s %d: " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)
#define RANV_ERROR(x, ...) do { fprintf(stderr,"%s %d: ERROR! " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)

/* x is the value returned if the check failed */
#define RANV_CHECK_INDEX(x) do { \
	if (index < 0 || index >= FLASH_BLOCK_NUM) { \
		RANV_PRINT("index(%d) is out of range\n", index); \
		return x; \
	} \
} while (0)

#define RANV_CHECK_VALID(x) do { \
	if (!fb[index].valid) { \
		RANV_PRINT("fb[%d] invalid, init again\n", index); \
		nvram_init(index); \
		if (!fb[index].valid) { \
		    RANV_PRINT("fb[%d] invalid, reinit not correct. return.\n", index); \
		    return x; \
		} \
	} \
} while (0)

int getNvramNum(void)
{
	return FLASH_BLOCK_NUM;
}

char *getNvramName(int index)
{
	RANV_CHECK_INDEX(NULL);
	return fb[index].name;
}

unsigned int getNvramIndex(char *name)
{
	int i;
	for (i = 0; i < FLASH_BLOCK_NUM; i++) {
		if (!strcmp(fb[i].name, name)) {
			return i;
		}
	}
	return -1;
}

/*
 * 1. read env from flash
 * 2. parse entries
 * 3. save the entries to cache
 */
int nvram_init(int index)
{
	int i, len;
	char *p, *q;
	int fd;
	nvram_ioctl_t nvr;

	/*
	 * read data from flash
	 */
	len = fb[index].flash_max_len - sizeof(fb[index].env.crc);

	fb[index].env.data = (char *)malloc(len);
	if ( !fb[index].env.data )
	{
		perror("malloc");
		return -1;
	}

	nvr.index = index;
	nvr.value = fb[index].env.data;

	fd = open(NV_DEV, O_RDONLY);
	if ( fd < 0 )
	{
		perror(NV_DEV);
		NVFREE(fb[index].env.data);
		return -1;
	}

	if ( ioctl(fd, RALINK_NVRAM_IOCTL_GETALL, &nvr) < 0 )
	{
		perror("ioctl");
    		if(fd)
		    close(fd);
		NVFREE(fb[index].env.data);
		return -1;
	}

        if(fd)
	    close(fd);

	//parse env to cache
	p = fb[index].env.data;
	for (i = 0; i < MAX_CACHE_ENTRY; i++)
	{
		/* Store var name */
		if (NULL == (q = strchr(p, '=')))
		{
			RANV_PRINT("parsed failed - cannot find '='\n");
			break;
		}
		*q = '\0'; //strip '='
		fb[index].cache[i].name = strdup(p);
		//printf("  %d '%s'->", i, p);

		/* Store var value */
		p = q + 1; //value
		if (NULL == (q = strchr(p, '\0')))
		{
			RANV_PRINT("parsed failed - cannot find '\\0'\n");
			NVFREE(fb[index].cache[i].name);
			break;
		}
		fb[index].cache[i].value = strdup(p);
		//printf("'%s'\n", p);

		p = q + 1; //next entry
		if (p - fb[index].env.data + 1 >= len) //end of block
			break;
		if (*p == '\0') //end of env
			break;
	}

	if (i == MAX_CACHE_ENTRY)
		RANV_PRINT("run out of env cache, please increase MAX_CACHE_ENTRY\n");

	fb[index].valid = 1;
	fb[index].dirty = 0;

	return 0;
}

void nvram_close(int index)
{
	int i;

	RANV_CHECK_INDEX();

	if (!fb[index].valid)
		return;

	if (fb[index].dirty)
		nvram_commit(index);

	//free env
	NVFREE(fb[index].env.data);

	//free cache
	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		NVFREE(fb[index].cache[i].name);
		NVFREE(fb[index].cache[i].value);
	}

	fb[index].valid = 0;
}

/*
 * return idx (0 ~ iMAX_CACHE_ENTRY)
 * return -1 if no such value or empty cache
 */
static int cache_idx(int index, char *name)
{
	int i;

	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		if (!fb[index].cache[i].name)
			return -1;
		if (!strcmp(name, fb[index].cache[i].name))
			return i;
	}

	return -1;
}

/* buffered get variable with/without copy to new memory block */
static char *__nvram_bufget(int index, char *name, int copy)
{
	int idx;
	/* Initial value should be NULL */
	static char *ret = NULL;
	int fd;
	nvram_ioctl_t nvr;

	RANV_CHECK_INDEX("");
	RANV_CHECK_VALID("");

	nvr.size  = MAX_VALUE_LEN;
	nvr.index = index;
	nvr.name  = name;

	fd = open(NV_DEV, O_RDONLY);
	if ( fd < 0 )
	{
		perror(NV_DEV);
		return "";
	}

	while (1)
	{
		nvr.value = malloc(nvr.size);

		if (nvr.value == NULL)
		{
			perror("malloc");
			return "";
		}

		// Perform request
		int result = ioctl(fd, RALINK_NVRAM_IOCTL_GET, &nvr);
		if (result >= 0)
			break;

		if (errno != EOVERFLOW) // Error is not caused by not-enough-space?
		{
			perror("ioctl");
			NVFREE(nvr.value);
			close(fd);
			return "";
		}

		// Calculate new buffer size
		NVFREE(nvr.value);
		nvr.size += ((MAX_VALUE_LEN - nvr.size % MAX_VALUE_LEN) % MAX_VALUE_LEN);
		if (nvr.size >= MAX_PERMITTED_VALUE_LEN)
		{
			perror("nvram_bufget overflow");
			NVFREE(nvr.value);
			close(fd);
			return "";
		}
	}
	close(fd);

	idx = cache_idx(index, name);

	if (-1 != idx) {
		if (fb[index].cache[idx].value) {
			//duplicate the value in case caller modify it
			NVFREE(fb[index].cache[idx].value);
			fb[index].cache[idx].value = strdup(nvr.value);
			NVFREE(nvr.value);
			if (!copy)
			    ret = fb[index].cache[idx].value;
			else
			    ret = strdup(fb[index].cache[idx].value);
			RANV_PRINT("bufget %d '%s'->'%s'\n", index, name, ret);
			//btw, we don't return NULL anymore!
			if (!ret)
			    ret = "";

		    return ret;
		}
	}

	//no default value set?
	//btw, we don't return NULL anymore!
	RANV_PRINT("bufget %d '%s'->''(empty) Warning!\n", index, name);
	NVFREE(nvr.value);
	return "";
}

/* get variable with copy to new memory block */
char *nvram_get_copy(int index, char *name)
{
	/* Initial value should be NULL */
	char *recv = NULL;

	recv = __nvram_bufget(index, name, 1);

	//btw, we don't return NULL anymore!
	if (!recv)
	    recv = "";

	return recv;
}

/* get variable without copy to new memory block */
char *nvram_get(int index, char *name)
{
	/* Initial value should be NULL */
	char *recv = NULL;

	recv = __nvram_bufget(index, name, 0);

	//btw, we don't return NULL anymore!
	if (!recv)
	    recv = "";

	return recv;
}

int nvram_bufset(int index, char *name, char *value)
{
	int idx;
	int fd;
	nvram_ioctl_t nvr;

	RANV_CHECK_INDEX(-1);
	RANV_CHECK_VALID(-1);

	nvr.index = index;
	nvr.name  = name;
	nvr.value = value;
	nvr.size  = strlen(value) + 1;

	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}

	if (ioctl(fd, RALINK_NVRAM_IOCTL_SET, &nvr) < 0)
	{
		if (errno == EOVERFLOW)
			perror("nvram_bufset overflow");
		else
			perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	idx = cache_idx(index, name);

	if (-1 == idx) {
		//find the first empty room
		for (idx = 0; idx < MAX_CACHE_ENTRY; idx++) {
			if (!fb[index].cache[idx].name)
				break;
		}
		//no any empty room
		if (idx == MAX_CACHE_ENTRY) {
			RANV_ERROR("run out of env cache, please increase MAX_CACHE_ENTRY\n");
			return -1;
		}
		fb[index].cache[idx].name = strdup(name);
		fb[index].cache[idx].value = strdup(value);
	} else {
		//abandon the previous value
		NVFREE(fb[index].cache[idx].value);
		fb[index].cache[idx].value = strdup(value);
	}

	RANV_PRINT("bufset %d '%s'->'%s'\n", index, name, value);
	fb[index].dirty = 1;
	return 0;
}

int nvram_set(int index, char *name, char *value)
{
	int rc;

	if (nvram_bufset(index, name, value) == -1 ) 
		rc = -1;
	else
	rc = nvram_commit(index);

	return rc;
}

size_t nvram_get_tuple(int index, char *key, unsigned elem_index, char *value, size_t value_size)
{
    char *v = NULL;
    char *e = NULL;
    char *s = NULL;
    unsigned i = 0;
    size_t len = 0u;

    /* indexes started at 1 */
    elem_index++;

    e = s = v = nvram_get(index, key);
    len = strlen(v);
    while ((e = strchr(s, ';')) != NULL) {
        if (++i == elem_index)
            break;
        /* next */
        s = ++e;
    }
    /* fix endpos */
    if (!e) {
        e = v + len;
        i++;
    }

    if (i != elem_index) {
        s = e;
        RANV_PRINT("invalid index: %u, maximum: %u", elem_index, i);
    }

    memset(value, 0u, value_size);
    len = (e - s);
    if (len && value) {
        snprintf(value, value_size, "%.*s", len, s);
    }
    return len;
}

void nvram_set_tuple(int index, char *key, unsigned elem_index, const char *value)
{
    char *v = NULL;
    char *e = NULL;
    char *s = NULL;
    unsigned i = 0;
    char *nv = NULL;

    size_t vlen = 0u;
    size_t len = 0u;
    size_t value_len = strlen(value);

    /* indexes started at 1 */
    elem_index++;

    e = s = v = nvram_get(index, key);
    vlen = strlen(v);
    while ((e = strchr(s, ';')) != NULL) {
        if (++i == elem_index)
            break;
        s = ++e;
    }
    /* fix endpos */
    if (!e) {
        e = v + vlen;
        i++;
    }

    if (i != elem_index) {
        i = elem_index - i;
        s = e;
    } else {
        i = 0;
    }

    /* format new nvram value */
    len = vlen - (e - s) + value_len + i + 1;
    nv = calloc(1, len);
    if (!nv) {
        RANV_PRINT("calloc(%zu) failed", len);
        return;
    }

    if (i) {
        snprintf(nv, len, "%s%*.0s%s", v, i, "", value);
        memset(nv + vlen, ';', i);
    } else {
        snprintf(nv, len, "%.*s%s%s", (s - v), v, value, e);
    }
    nvram_set(index, (char*)key, nv);
    free(nv);

    return;
}


/*
 * write flash from cache
 */
int nvram_commit(int index)
{
	int fd;
	nvram_ioctl_t nvr;

	RANV_PRINT("--> nvram_commit %d\n", index);

	nvr.index = index;
	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_NVRAM_IOCTL_COMMIT, &nvr) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	fb[index].dirty = 0;
	return 0;
}

/*
 * clear flash by writing all 1's value
 */
int nvram_clear(int index)
{
	int fd;
	nvram_ioctl_t nvr;
	nvr.index = index;
	fd = open(NV_DEV, O_RDONLY);
	if (fd < 0) {
		perror(NV_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_NVRAM_IOCTL_CLEAR, &nvr) < 0) {
		perror("ioctl");
		close(fd);
		return -1;
	}
	close(fd);

	fb[index].dirty = 0;
	return 0;
}

/*
 * read reqested options by defaults file and wirte to nvram
 */
int nvram_fromdef(int idx_nvram, int num, ...)
{
	va_list vargs;
	char buf[BUFSZ];
	char *p, *tmp;
	int result = 0;
	int found = 0;
	int n = 0;

	FILE *fp = fopen(DEFAULT_NVRAM, "r");
	if(!fp) {
		RANV_ERROR("error open default settings\n");
		return -1;
	}
	while(fgets(buf, sizeof(buf), fp)) {
		if (buf == NULL || buf[0] == '\n' || buf[0] == '#')
			continue;
		if (!strncmp(buf, "Default\n", 8)) {
			found = 1;
			break;
		}
	}
	if (!found) {
		RANV_ERROR("file format error!\n");
		result = -1;
		goto out;
	}
	if (nvram_init(idx_nvram) == -1) {
		RANV_ERROR("cant init nvram\n");
		result = -1;
		goto out;
	}
	while(fgets(buf, sizeof(buf), fp)) {
		if (buf == NULL || buf[0] == '\n' || buf[0] == '#')
			continue;
		if (!(p = strchr(buf, '='))) {
			RANV_ERROR("file format error!\n");
			goto out_with_commit;
		}
		buf[strlen(buf) - 1] = '\0';
		*p++ = '\0';

		va_start(vargs, num);
		for(n = num; n > 0; n--) {
			tmp = va_arg(vargs, char *);
			if (!strncmp(buf, tmp, strlen(buf))) {
				nvram_bufset(idx_nvram, buf, p);
				break;
			}
		}
		va_end(vargs);
	}
out_with_commit:
	nvram_commit(idx_nvram);
	nvram_close(idx_nvram);
out:
	fclose(fp);
	return result;
}

int nvram_renew(int mode, char *fname)
{
	FILE *fp;
	char buf[BUFSZ], *p;
	int found = 0, need_commit = 0;

	fp = fopen(fname, "ro");
	if (!fp)
	{
		perror("fopen");
		return -1;
	}

	//find "Default" first
	while (fgets(buf, sizeof(buf), fp))
	{
		if (buf[0] == '\n' || buf[0] == '#')
			continue;
		if (!strncmp(buf, "Default\n", 8))
		{
			found = 1;
			break;
		}
	}

	if (!found)
	{
		printf("file format error!\n");
		fclose(fp);
		return -1;
	}

	if (nvram_init(mode) == -1)
	{
		fclose(fp);
		return -1;
	}

	while (fgets(buf, sizeof(buf), fp))
	{
		if (buf[0] == '\n' || buf[0] == '#')
			continue;
		if (!(p = strchr(buf, '=')))
		{
			if (need_commit)
			{
				nvram_commit(mode);
				need_commit = 0;
			}
			printf("%s file format error!\n", fname);
			goto out;
		}
		buf[strlen(buf) - 1] = '\0'; //remove carriage return
		*p++ = '\0'; //seperate the string
		nvram_bufset(mode, buf, p);
		need_commit = 1;
	}

	nvram_commit(mode);
out:
	fclose(fp);
	nvram_close(mode);
	return 0;
}

void nvram_buflist(int index)
{
	int i;

	RANV_CHECK_INDEX();
	RANV_CHECK_VALID();

	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		if (!fb[index].cache[i].name)
			break;
		printf("%s=%s\n", fb[index].cache[i].name, fb[index].cache[i].value);
	}
}

int nvram_show(int mode)
{
	nvram_init(mode);
	nvram_buflist(mode);
	nvram_close(mode);
	return 0;
}

static int MacValid(char *str)
{
	int i, len = strlen(str);
	if(len != 17)
		return 0;

	for(i=0; i<5; i++){
		if( (!isxdigit( str[i*3])) || (!isxdigit( str[i*3+1])) || (str[i*3+2] != ':') )
			return 0;
	}
	return (isxdigit(str[15]) && isxdigit(str[16])) ? 1: 0;
}

/* strToIntDef - Convert whole char* string to integer or use default value
 *
 * arg: value - input string
 * arg: def - default value
 * return: converted value
 */
static int strToIntDef(char* value, int def)
{
    char* endptr = value;

    if (value == NULL) 
        return def;

    long val = strtol(value, &endptr, 10);

    if (endptr[0] != '\0')
        return def;

    if (val > INT_MAX || val < INT_MIN)
        return def;

    return (int)val;
}

int nvram_get_int(int index, char *name, int def)
{
	char* value = nvram_get(index, name);
	return strToIntDef(value, def);
}

int nvram_load_default(void)
{
	/* default macs is OK */
	int mac_err = 0;
        char *LAN_MAC_ADDR = NULL, *WAN_MAC_ADDR = NULL, *WLAN_MAC_ADDR = NULL, *WLAN2_MAC_ADDR = NULL, *CHECKMAC = NULL;
        char  *MngmtLogin = NULL, *MngmtPassword = NULL, *MngmtStoreSettings = NULL, *cwmpdEnabled = NULL;

	/* copy old remotemanagment settings if enabled */
	MngmtStoreSettings = nvram_get_copy(RT2860_NVRAM, "MngmtStoreSettings");
	if (atoi(MngmtStoreSettings) == 1) {
	    MngmtLogin		= nvram_get_copy(RT2860_NVRAM, "MngmtLogin");
	    MngmtPassword	= nvram_get_copy(RT2860_NVRAM, "MngmtPassword");
	    cwmpdEnabled	= nvram_get_copy(RT2860_NVRAM, "cwmpdEnabled");

	    printf("Store remote managment user settings.\n");
	}

	/* copy old values to new memory block */
	WLAN2_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "WLAN2_MAC_ADDR");
	WLAN_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "WLAN_MAC_ADDR");
	WAN_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "WAN_MAC_ADDR");
	LAN_MAC_ADDR	= nvram_get_copy(RT2860_NVRAM, "LAN_MAC_ADDR");
	CHECKMAC	= nvram_get_copy(RT2860_NVRAM, "CHECKMAC");

	printf("Store current MAC adresses in mem: %s, %s, %s, %s. CHECKMAC: %s\n",
		WAN_MAC_ADDR, LAN_MAC_ADDR, WLAN_MAC_ADDR, WLAN2_MAC_ADDR, CHECKMAC);

	printf("Clear nvram.\n");
	nvram_clear(RT2860_NVRAM);
	printf("Load defaults nvram.\n");
	nvram_renew(RT2860_NVRAM, "/etc/default/nvram_default");

	printf("Reinit nvram before commit.\n");
	if (nvram_init(RT2860_NVRAM) == -1) {
		printf("nvram_load_default: init error!.\n");
		return -1;
	}

	printf("Restore saved in mem MAC adresses : %s, %s, %s, %s. CHECKMAC: %s\n",
		WAN_MAC_ADDR, LAN_MAC_ADDR, WLAN_MAC_ADDR, WLAN2_MAC_ADDR, CHECKMAC);

	if ((strlen(WLAN2_MAC_ADDR) > 0) && MacValid(WLAN2_MAC_ADDR))
	    nvram_bufset(RT2860_NVRAM, "WLAN2_MAC_ADDR", WLAN2_MAC_ADDR);
	else
	    mac_err++;
	if ((strlen(WLAN_MAC_ADDR) > 0) && MacValid(WLAN_MAC_ADDR))
	    nvram_bufset(RT2860_NVRAM, "WLAN_MAC_ADDR", WLAN_MAC_ADDR);
	else
	    mac_err++;
	if ((strlen(WAN_MAC_ADDR) > 0) && MacValid(WAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR",  WAN_MAC_ADDR);
	else
	    mac_err++;
	if ((strlen(LAN_MAC_ADDR) > 0) && MacValid(LAN_MAC_ADDR))
    	    nvram_bufset(RT2860_NVRAM, "LAN_MAC_ADDR",  LAN_MAC_ADDR);
	else
	    mac_err++;

	/* mac restore ok ? */
	if (mac_err == 0) {
	    printf("MAC adresses restored OK, restore checkmac flag.\n");
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", CHECKMAC);
	} else {
	    printf("MAC adresses not correct, set checkmac flag to yes and generate new. err_count=%d.\n", mac_err);
    	    nvram_bufset(RT2860_NVRAM, "CHECKMAC", "YES");
	}

	/* restore remotemanagment */
	if (atoi(MngmtStoreSettings) == 1) {
	    printf("Restore remote managment user settings.\n");
	    if  (strlen(MngmtLogin) > 0 && strlen(MngmtPassword) > 0) {
		nvram_bufset(RT2860_NVRAM, "MngmtLogin", MngmtLogin);
		nvram_bufset(RT2860_NVRAM, "MngmtPassword", MngmtPassword);

		/* allow remote ssh from WAN */
		nvram_bufset(RT2860_NVRAM, "RemoteSSH", "2");
	    }

	    /* restore cwmpd settings */
	    nvram_bufset(RT2860_NVRAM, "cwmpdEnabled", cwmpdEnabled);

	    /* set keep remote managment flag */
	    nvram_bufset(RT2860_NVRAM, "MngmtStoreSettings", "1");
	}

	/* set wive flag */
	nvram_bufset(RT2860_NVRAM, "IS_WIVE", "YES");
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

        return 0;
}

#define FPRINT_DAT(x) fprintf(fp, #x"=%s\n", nvram_get(mode, #x));

int gen_wifi_config(int mode, int genmode)
{
	FILE *fp = NULL;
	int  i, ssid_num = 0;
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	int inic = 0;
#endif
	char w_mode[64], wmm_enable[64];

	if (genmode == RT2860_NVRAM) {
		system("mkdir -p /etc/Wireless/RT2860");
		fp = fopen("/etc/Wireless/RT2860/RT2860.dat", "w+");
		printf("Build config for fist WiFi module.\n");
	} else if (genmode == RTINIC_NVRAM) {
		system("mkdir -p /etc/Wireless/iNIC");
		fp = fopen("/etc/Wireless/iNIC/iNIC_ap.dat", "w+");
		/* after select file for write back to native 2860 mode */
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
		/* use one source offset */
		inic = 1;
#endif
		printf("Build config for second WiFi module.\n");
	} else {
		printf("gen_wifi_config: mode unknown...\n");
		return 0;
	}

	if (!fp){
		printf("gen_wifi_config: fopen failed.\n");
		return 0;
	}

	if (nvram_init(mode) == -1) {
		printf("gen_wifi_config: nvram init failed...\n");
		fclose(fp);
		return -1;
	}

	fprintf(fp, "#The word of \"Default\" must not be removed\n");
	fprintf(fp, "Default\n");

	// MAC adresses per devices
	if (genmode == RT2860_NVRAM)
	    fprintf(fp, "MacAddress=%s\n", nvram_get(mode, "WLAN_MAC_ADDR"));
	else if (genmode == RTINIC_NVRAM)
	    fprintf(fp, "MacAddress=%s\n", nvram_get(mode, "WLAN2_MAC_ADDR"));

	// Limit numbers ssid
	ssid_num = atoi(nvram_get(mode, "BssidNum"));
	if (ssid_num > MAX_NUMBER_OF_BSSID || ssid_num <= 0)
		ssid_num = MAX_NUMBER_OF_BSSID;

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
#endif
	    // WirelessMode -> need move per ssid to goahead
	    bzero(w_mode, sizeof(w_mode));
	    sprintf(w_mode, "%s", nvram_get(mode, "WirelessMode"));
	    for (i = 1; i < ssid_num; i++)
		sprintf(w_mode+strlen(w_mode), ";%s", nvram_get(mode, "WirelessMode"));
	    fprintf(fp, "WirelessMode=%s\n", w_mode);

	    FPRINT_DAT(RadioOn);
	    FPRINT_DAT(TxPower);
	    FPRINT_DAT(Channel);
	    FPRINT_DAT(AutoChannelSelect);
	    FPRINT_DAT(AutoChannelSkipList);
	    FPRINT_DAT(ACSCheckTime);
	    FPRINT_DAT(BasicRate);
	    FPRINT_DAT(SSID1);
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	} else {
	    // WirelessMode -> need move per ssid to goahead
	    bzero(w_mode, sizeof(w_mode));
	    sprintf(w_mode, "%s", nvram_get(mode, "WirelessModeINIC"));
	    for (i = 1; i < ssid_num; i++)
		sprintf(w_mode+strlen(w_mode), ";%s", nvram_get(mode, "WirelessModeINIC"));
	    fprintf(fp, "WirelessMode=%s\n", w_mode);

	    fprintf(fp, "RadioOn=%s\n", nvram_get(mode, "RadioOnINIC"));
	    fprintf(fp, "TxPower=%s\n", nvram_get(mode, "TxPowerINIC"));
	    fprintf(fp, "Channel=%s\n", nvram_get(mode, "ChannelINIC"));
	    fprintf(fp, "AutoChannelSelect=%s\n", nvram_get(mode, "AutoChannelSelectINIC"));
	    fprintf(fp, "AutoChannelSkipList=%s\n", nvram_get(mode, "AutoChannelSkipListINIC"));
	    fprintf(fp, "ACSCheckTime=%s\n", nvram_get(mode, "ACSCheckTimeINIC"));
	    fprintf(fp, "BasicRate=%s\n", nvram_get(mode, "BasicRateINIC"));
	    fprintf(fp, "SSID1=%s\n", nvram_get(mode, "SSID1INIC"));
	}
#endif
	// Stub
	fprintf(fp, "FixedTxMode=\n");

	// WmmCapable -> need move per ssid to goahead
	bzero(wmm_enable, sizeof(wmm_enable));
	sprintf(wmm_enable, "%s", nvram_get(mode, "WmmCapable"));
	for (i = 1; i < ssid_num; i++)
		sprintf(wmm_enable+strlen(wmm_enable), ";%s", nvram_get(mode, "WmmCapable"));
	fprintf(fp, "WmmCapable=%s\n", wmm_enable);

	FPRINT_DAT(APAifsn);
	FPRINT_DAT(APCwmin);
	FPRINT_DAT(APCwmax);
	FPRINT_DAT(APTxop);
	FPRINT_DAT(APACM);
	FPRINT_DAT(BSSAifsn);
	FPRINT_DAT(BSSCwmin);
	FPRINT_DAT(BSSCwmax);
	FPRINT_DAT(BSSTxop);
	FPRINT_DAT(BSSACM);

	FPRINT_DAT(CountryRegion);
	FPRINT_DAT(CountryRegionABand);
	FPRINT_DAT(CountryCode);
	FPRINT_DAT(RDRegion);
	FPRINT_DAT(BssidNum);
	FPRINT_DAT(SSID2);
	FPRINT_DAT(SSID3);
	FPRINT_DAT(SSID4);
	FPRINT_DAT(SSID5);
	FPRINT_DAT(SSID6);
	FPRINT_DAT(SSID7);
	FPRINT_DAT(SSID8);

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (inic)
	    fprintf(fp, "BeaconPeriod=%s\n", nvram_get(mode, "BeaconPeriodINIC"));
	else
#endif
	    FPRINT_DAT(BeaconPeriod);

	FPRINT_DAT(DtimPeriod);
	FPRINT_DAT(DisableOLBC);
	FPRINT_DAT(BGProtection);
	FPRINT_DAT(RTSThreshold);
	FPRINT_DAT(TxPreamble);
	FPRINT_DAT(TxPower);
	FPRINT_DAT(TxBurst);
	FPRINT_DAT(BurstMode);
	FPRINT_DAT(PktAggregate);
	FPRINT_DAT(FragThreshold);
	FPRINT_DAT(ShortSlot);
	FPRINT_DAT(NoForwardingBTNBSSID);
	FPRINT_DAT(IdleTimeout);

	FPRINT_DAT(APSDCapable);
	FPRINT_DAT(AckPolicy);
	FPRINT_DAT(DLSCapable);
	FPRINT_DAT(NoForwarding);
	FPRINT_DAT(NoForwardingMBCast);
	FPRINT_DAT(StationKeepAlive);
	FPRINT_DAT(EntryLifeCheck);
	FPRINT_DAT(HideSSID);
	FPRINT_DAT(AuthMode);
	FPRINT_DAT(EncrypType);
    	FPRINT_DAT(RekeyMethod);
	FPRINT_DAT(RekeyInterval);

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (inic)
	    fprintf(fp, "WPAPSK1=%s\n", nvram_get(mode, "WPAPSK1INIC"));
	else
#endif
	    FPRINT_DAT(WPAPSK1);

	FPRINT_DAT(WPAPSK2);
	FPRINT_DAT(WPAPSK3);
	FPRINT_DAT(WPAPSK4);
	FPRINT_DAT(WPAPSK5);
	FPRINT_DAT(WPAPSK6);
	FPRINT_DAT(WPAPSK7);
	FPRINT_DAT(WPAPSK8);

	FPRINT_DAT(DefaultKeyID);
	FPRINT_DAT(Key1Type);
	FPRINT_DAT(Key1Str1);
	FPRINT_DAT(Key1Str2);
	FPRINT_DAT(Key1Str3);
	FPRINT_DAT(Key1Str4);
	FPRINT_DAT(Key1Str5);
	FPRINT_DAT(Key1Str6);
	FPRINT_DAT(Key1Str7);
	FPRINT_DAT(Key1Str8);
	FPRINT_DAT(Key2Type);
	FPRINT_DAT(Key2Str1);
	FPRINT_DAT(Key2Str2);
	FPRINT_DAT(Key2Str3);
	FPRINT_DAT(Key2Str4);
	FPRINT_DAT(Key2Str5);
	FPRINT_DAT(Key2Str6);
	FPRINT_DAT(Key2Str7);
	FPRINT_DAT(Key2Str8);
	FPRINT_DAT(Key3Type);
	FPRINT_DAT(Key3Str1);
	FPRINT_DAT(Key3Str2);
	FPRINT_DAT(Key3Str3);
	FPRINT_DAT(Key3Str4);
	FPRINT_DAT(Key3Str5);
	FPRINT_DAT(Key3Str6);
	FPRINT_DAT(Key3Str7);
	FPRINT_DAT(Key3Str8);
	FPRINT_DAT(Key4Type);
	FPRINT_DAT(Key4Str1);
	FPRINT_DAT(Key4Str2);
	FPRINT_DAT(Key4Str3);
	FPRINT_DAT(Key4Str4);
	FPRINT_DAT(Key4Str5);
	FPRINT_DAT(Key4Str6);
	FPRINT_DAT(Key4Str7);
	FPRINT_DAT(Key4Str8);

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (inic)
	    fprintf(fp, "HT_BW=%s\n", nvram_get(mode, "HT_BWINIC"));
	else
#endif
	    FPRINT_DAT(HT_BW);
	FPRINT_DAT(HT_HTC);
	FPRINT_DAT(HT_RDG);
	FPRINT_DAT(HT_OpMode);
	FPRINT_DAT(HT_MpduDensity);
	FPRINT_DAT(HT_AutoBA);
	FPRINT_DAT(HT_BADecline);
	FPRINT_DAT(HT_AMSDU);
	FPRINT_DAT(HT_BAWinSize);
	FPRINT_DAT(HT_GI);
	FPRINT_DAT(HT_STBC);
	FPRINT_DAT(HT_LDPC);
	FPRINT_DAT(HT_MCS);
	FPRINT_DAT(HT_PROTECT);
	FPRINT_DAT(HT_DisallowTKIP);
	FPRINT_DAT(HT_40MHZ_INTOLERANT);
	FPRINT_DAT(HT_MIMOPSMode);

#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
#endif
	    FPRINT_DAT(HT_TxStream);
	    FPRINT_DAT(HT_RxStream);
	    FPRINT_DAT(HT_EXTCHA);
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	} else {
	    fprintf(fp, "HT_EXTCHA=%s\n", nvram_get(mode, "HT_EXTCHAINIC"));
	    fprintf(fp, "HT_TxStream=%s\n", nvram_get(mode, "HT_TxStreamINIC"));
	    fprintf(fp, "HT_RxStream=%s\n", nvram_get(mode, "HT_RxStreamINIC"));
	}
#endif
#ifndef CONFIG_RT_SECOND_IF_NONE
	// VHT
	FPRINT_DAT(VHT_BW);
	FPRINT_DAT(VHT_BW_SIGNAL);
	FPRINT_DAT(VHT_DisallowNonVHT);
	FPRINT_DAT(VHT_LDPC);
	FPRINT_DAT(VHT_SGI);
	FPRINT_DAT(VHT_STBC);
#endif
	FPRINT_DAT(AccessPolicy0);
	FPRINT_DAT(AccessControlList0);
	FPRINT_DAT(AccessPolicy1);
	FPRINT_DAT(AccessControlList1);
	FPRINT_DAT(AccessPolicy2);
	FPRINT_DAT(AccessControlList2);
	FPRINT_DAT(AccessPolicy3);
	FPRINT_DAT(AccessControlList3);
	FPRINT_DAT(AccessPolicy4);
	FPRINT_DAT(AccessControlList4);
	FPRINT_DAT(AccessPolicy5);
	FPRINT_DAT(AccessControlList5);
	FPRINT_DAT(AccessPolicy6);
	FPRINT_DAT(AccessControlList6);
	FPRINT_DAT(AccessPolicy7);
	FPRINT_DAT(AccessControlList7);
	FPRINT_DAT(RADIUS_Key1);
	FPRINT_DAT(RADIUS_Key2);
	FPRINT_DAT(RADIUS_Key3);
	FPRINT_DAT(RADIUS_Key4);
	FPRINT_DAT(RADIUS_Key5);
	FPRINT_DAT(RADIUS_Key6);
	FPRINT_DAT(RADIUS_Key7);
	FPRINT_DAT(RADIUS_Key8);
	FPRINT_DAT(MaxStaNum);

	FPRINT_DAT(WirelessEvent);  /* send log message through wireless event for debug purpose */

#if defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE) || defined(CONFIG_MT76X3_STA) || defined(CONFIG_MT76X3_STA_MODULE)
	FPRINT_DAT(PSMode);
	FPRINT_DAT(AutoConnect);
	FPRINT_DAT(FastConnect);
	FPRINT_DAT(AutoRoaming);
#endif
#if defined(CONFIG_MT7610_AP_EXT_CHANNEL_LIST) || defined(CONFIG_MT76X2_AP_EXT_CHANNEL_LIST) || defined(CONFIG_MT76X3_AP_EXT_CHANNEL_LIST)
	FPRINT_DAT(ChannelGeography);
#endif
#if defined(CONFIG_MT7610_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X2_AP_VIDEO_TURBINE) || defined(CONFIG_MT76X3_AP_VIDEO_TURBINE)
	FPRINT_DAT(VideoTurbine);
	FPRINT_DAT(VideoClassifierEnable);
	FPRINT_DAT(VideoHighTxMode);
	FPRINT_DAT(VideoTxLifeTimeMode);
#endif
#if defined(CONFIG_MT7610_AP_IGMP_SNOOP) || defined(CONFIG_MT76X2_AP_IGMP_SNOOP) || defined(CONFIG_MT76X3_AP_IGMP_SNOOP)
	FPRINT_DAT(M2UEnabled);
#endif
#if defined(CONFIG_MT7610_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X2_AP_MCAST_RATE_SPECIFIC) || defined(CONFIG_MT76X3_AP_MCAST_RATE_SPECIFIC)
	FPRINT_DAT(McastPhyMode);
	FPRINT_DAT(McastMcs);
#endif
#if defined(CONFIG_MT7610_AP_CARRIER) || defined(CONFIG_MT76X2_AP_CARRIER) || defined(CONFIG_MT76X3_AP_CARRIER)
	FPRINT_DAT(CarrierDetect);
#endif
#if defined(CONFIG_MT76X2_AP_TXBF_SUPPORT)
	FPRINT_DAT(ITxBfEn);
	FPRINT_DAT(ITxBfTimeout);
	FPRINT_DAT(ETxBfEnCond);
	FPRINT_DAT(ETxBfeeEn);
	FPRINT_DAT(ETxBfTimeout);
	FPRINT_DAT(ETxBfNoncompress);
	FPRINT_DAT(ETxBfIncapable);
#endif
#if defined(CONFIG_RT_SECOND_IF_RANGE_5GHZ) /* IEEE80211H variable enable spectrum managment and/or DFS (depended by DFS builtin) need for all dualband builds */
	FPRINT_DAT(IEEE80211H);
#endif
#if defined(CONFIG_MT7610_AP_DFS) || defined(CONFIG_MT76X2_AP_DFS)
	FPRINT_DAT(CSPeriod);
	FPRINT_DAT(DfsIndoor);
	FPRINT_DAT(SymRoundFromCfg);
	FPRINT_DAT(BusyIdleFromCfg);
	FPRINT_DAT(DfsRssiHighFromCfg);
	FPRINT_DAT(DfsRssiLowFromCfg);
	FPRINT_DAT(DFSParamFromConfig);
	FPRINT_DAT(FCCParamCh0);
	FPRINT_DAT(FCCParamCh1);
	FPRINT_DAT(FCCParamCh2);
	FPRINT_DAT(FCCParamCh3);
	FPRINT_DAT(CEParamCh0);
	FPRINT_DAT(CEParamCh1);
	FPRINT_DAT(CEParamCh2);
	FPRINT_DAT(CEParamCh3);
	FPRINT_DAT(JAPParamCh0);
	FPRINT_DAT(JAPParamCh1);
	FPRINT_DAT(JAPParamCh2);
	FPRINT_DAT(JAPParamCh3);
	FPRINT_DAT(JAPW53ParamCh0);
	FPRINT_DAT(JAPW53ParamCh1);
	FPRINT_DAT(JAPW53ParamCh2);
	FPRINT_DAT(JAPW53ParamCh3);
#endif
#if defined(CONFIG_MT7610_AP_ED_MONITOR) || defined(CONFIG_MT76X2_AP_ED_MONITOR) || defined(CONFIG_MT76X3_AP_ED_MONITOR)
	FPRINT_DAT(EDCCA_ED_TH);
	FPRINT_DAT(ED_MODE);
	FPRINT_DAT(EDCCA_FALSE_CCA_TH);
	FPRINT_DAT(ED_LEARN_TH);
	FPRINT_DAT(EDCCA_BLOCK_CHECK_TH);
#endif
#if defined(CONFIG_MT76X2_AP_GREENAP) || defined(CONFIG_MT76X3_AP_GREENAP)
	FPRINT_DAT(GreenAP);
#endif
#if defined(CONFIG_MT7610_AP_80211N_DRAFT3) || defined(CONFIG_MT76X2_AP_80211N_DRAFT3) || defined(CONFIG_MT76X3_AP_80211N_DRAFT3)
	FPRINT_DAT(HT_BSSCoexistence);
	FPRINT_DAT(HT_BSSCoexApCntThr);
#endif
#if defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS) || defined(CONFIG_MT76X3_AP_WDS)
	FPRINT_DAT(WdsEnable);
	FPRINT_DAT(WdsPhyMode);
	FPRINT_DAT(WdsTxMcs);
	FPRINT_DAT(WdsEncrypType);
	FPRINT_DAT(WdsList);
	FPRINT_DAT(Wds0Key);
	FPRINT_DAT(Wds1Key);
	FPRINT_DAT(Wds2Key);
	FPRINT_DAT(Wds3Key);
#endif
#if defined(CONFIG_MT7610_AP_APCLI) || defined(CONFIG_MT76X2_AP_APCLI) || defined(CONFIG_MT76X3_AP_APCLI)
	FPRINT_DAT(ApCliEnable);
	FPRINT_DAT(ApCliSsid);
	FPRINT_DAT(ApCliBssid);
	FPRINT_DAT(ApCliAuthMode);
	FPRINT_DAT(ApCliEncrypType);
	FPRINT_DAT(ApCliDefaultKeyID);
	FPRINT_DAT(ApCliWPAPSK);
	FPRINT_DAT(ApCliTxMode);
	FPRINT_DAT(ApCliTxMcs);
	FPRINT_DAT(ApCliAPSDCapable);
#endif
#if defined(CONFIG_MT7610_AP_IDS) || defined(CONFIG_MT76X2_AP_IDS) || defined(CONFIG_MT76X3_AP_IDS)
	FPRINT_DAT(IdsEnable);
	FPRINT_DAT(AuthFloodThreshold);
	FPRINT_DAT(AssocReqFloodThreshold);
	FPRINT_DAT(ReassocReqFloodThreshold);
	FPRINT_DAT(ProbeReqFloodThreshold);
	FPRINT_DAT(DisassocFloodThreshold);
	FPRINT_DAT(DeauthFloodThreshold);
	FPRINT_DAT(EapReqFloodThreshold);
#endif
	/* 802.11x support */
	FPRINT_DAT(IEEE8021X);
	FPRINT_DAT(quiet_interval);
	FPRINT_DAT(session_timeout_interval);
	FPRINT_DAT(PreAuth);
	FPRINT_DAT(PMKCachePeriod);
	FPRINT_DAT(own_ip_addr);
	FPRINT_DAT(EAPifname);
	FPRINT_DAT(PreAuthifname);
	FPRINT_DAT(RADIUS_Server);
	FPRINT_DAT(RADIUS_Port);
	FPRINT_DAT(RADIUS_Key);

	/* LNA Control, not apply from read dat file now, need add in profile instead of iwpriv usage in future */
	FPRINT_DAT(DyncVgaEnable);	/* enable/disable dynamic LNA gain */
	FPRINT_DAT(SkipLongRangeVga);	/* skip tune gain for long distanse clients */
	FPRINT_DAT(VgaClamp);		/* limit dynamic maximum gain to reduce impact interference (1 - -4dB, 2 - -8dB, 3 - -12dB, 4 -16dB) */
#ifdef CONFIG_BAND_STEERING
	FPRINT_DAT(BandSteering);
	/* Band Steering, not apply from read dat file now, need add in profile instead of iwpriv usage in future */
	FPRINT_DAT(BndStrgRssiDiff);	/* if Rssi2.4G > Rssi5G by RssiDiff, then allow client to connect 2.4G */
	FPRINT_DAT(BndStrgRssiLow);	/* if Rssi5G < RssiLow, then this client cannot connect to 5G */
	FPRINT_DAT(BndStrgAge);		/* Entry Age Time (ms) */
	FPRINT_DAT(BndStrgHoldTime);	/* Time for holding 2.4G connection rsp (ms) */
	FPRINT_DAT(BndStrgCheckTime);	/* Time for deciding if a client is 2.4G only (ms) */
#endif
	/* Basic Roaming, need add in profile instead of iwpriv usage in future
	 *
	 * ApProbeRspTimes       - range 0 - 10 times, limit probe reqest per client, default 3
	 * AuthRspFail           - range 0 - -100 dBm, reject auth req due to weak signal, default 0 (off)
	 * AuthRspRssi           - range 0 - -100 dBm, ignore auth req due to weak signal, default 0 (off)
	 * AssocReqRssiThres     - range 0 - -100 dBm, reject assoc req due to weak signal, default 0 (off)
	 * AssocRspIgnor         - range 0 - -100 dBm, ignore assoc req due to weak signal, default 0 (off)
	 * ProbeRspRssi          - range 0 - -100 dBm, auto disonnect sta if rssi low at probe requests, default 0 (off)
	 * KickStaRssiLow        - range 0 - -100 dBm, auto disonnect sta if rssi low (active clients), default 0 (off)
	 * KickStaRssiLow        - range 0 - -100 dBm, auto disonnect sta if rssi low (PSM clients), default 0 (off)
	 * KickStaRssiLowDelay   - range 0 -  200 seconds, if in this interval all data frames have low rssi - kick STA, default 5
	 *
	*/
	FPRINT_DAT(ApProbeRspTimes);
	FPRINT_DAT(AuthRspFail);
	FPRINT_DAT(AuthRspRssi);
	FPRINT_DAT(AssocReqRssiThres);
	FPRINT_DAT(AssocRspIgnor);
	FPRINT_DAT(KickStaRssiLow);
	FPRINT_DAT(KickStaRssiLowPSM);
	FPRINT_DAT(KickStaRssiLowDelay);
	FPRINT_DAT(ProbeRspRssi);
#if defined(CONFIG_MT7610_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11R_FT_SUPPORT)
	/* Fast roaming auth transitions config */
	FPRINT_DAT(FtSupport);  	/* Enable Fast BSS Transition */
	FPRINT_DAT(FtRic);		/* Enable FT resource request */
	FPRINT_DAT(FtOtd);		/* Support Over-the-DS Fast BSS Transition (over LAN/WDS, need iappd daemon? Default support only Over-the-Air only) */
					/* Mobility domain ID of Fast Bss. */
	FPRINT_DAT(FtMdId1);
	FPRINT_DAT(FtMdId2);
	FPRINT_DAT(FtMdId3);
	FPRINT_DAT(FtMdId4);
					/* R(N) Key Handler Identification. */
	FPRINT_DAT(FtR0khId1);
	FPRINT_DAT(FtR0khId2);
	FPRINT_DAT(FtR0khId3);
	FPRINT_DAT(FtR0khId4);
#endif
#if  defined(CONFIG_MT7610_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X2_AP_DOT11K_RRM_SUPPORT) || defined(CONFIG_MT76X3_AP_DOT11K_RRM_SUPPORT)
	FPRINT_DAT(RRMEnable);  	/* Enable Resource Radio Managment */
#ifndef CONFIG_KERNEL_NVRAM_SPLIT_INIC
	if (!inic) {
	    FPRINT_DAT(RegulatoryClass);
	} else {
	    fprintf(fp, "RegulatoryClass=%s\n", nvram_get(mode, "RegulatoryClassINIC"));
	}
#else
	FPRINT_DAT(RegulatoryClass);
#endif
#endif /* END RRM */
#ifdef CONFIG_MT76X3_AP_SMART_CARRIER_SENSE
	FPRINT_DAT(SCSEnable);
#endif
	fclose(fp);
	nvram_close(mode);
	sync();
	return 0;
}
