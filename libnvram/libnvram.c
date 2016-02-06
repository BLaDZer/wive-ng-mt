#include "libnvram.h"

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
		free(fb[index].env.data);
		return -1;
	}

	if ( ioctl(fd, RALINK_NVRAM_IOCTL_GETALL, &nvr) < 0 )
	{
		perror("ioctl");
    		if(fd)
		    close(fd);
		free(fb[index].env.data);
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
			FREE(fb[index].cache[i].name);
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
	FREE(fb[index].env.data);

	//free cache
	for (i = 0; i < MAX_CACHE_ENTRY; i++) {
		FREE(fb[index].cache[i].name);
		FREE(fb[index].cache[i].value);
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
		free(nvr.value);
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
			free(nvr.value);
			close(fd);
			return "";
		}

		// Calculate new buffer size
		free(nvr.value);
		nvr.size += ((MAX_VALUE_LEN - nvr.size % MAX_VALUE_LEN) % MAX_VALUE_LEN);
		if (nvr.size >= MAX_PERMITTED_VALUE_LEN)
		{
			perror("nvram_bufget overflow");
			free(nvr.value);
			close(fd);
			return "";
		}
	}
	close(fd);

	idx = cache_idx(index, name);

	if (-1 != idx) {
		if (fb[index].cache[idx].value) {
			//duplicate the value in case caller modify it
			FREE(fb[index].cache[idx].value);
			fb[index].cache[idx].value = strdup(nvr.value);
			FREE(nvr.value);
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
	FREE(nvr.value);
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
		FREE(fb[index].cache[idx].value);
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
