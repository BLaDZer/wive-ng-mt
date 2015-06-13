#ifndef _NVRAM_H
#define _NVRAM_H 	1

/* need clenup and build flag */
#define NEED_REINIT 0xFDEAD

/* nvram parse blocks */
#define ENV_BLK_SIZE 0x1000
#define RALINK_NVRAM_DEVNAME "nvram"
#define RALINK_NVRAM_MTDNAME "Config"

/* split 2860 and RTDEV configs by offset */
#if !defined(CONFIG_RT_SECOND_IF_NONE) && defined(CONFIG_KERNEL_NVRAM_SPLIT_INIC)
#define FLASH_BLOCK_NUM 2
#else
#define FLASH_BLOCK_NUM 1
#endif

#define RANV_PRINT(x, ...) do { if (ra_nvram_debug) printk("\n%s %d: " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)
#define RANV_ERROR(x, ...) do { printk("%s %d: ERROR! " x, __FILE__, __LINE__, ## __VA_ARGS__); } while(0)

#define KFREE(x) do { if (x != NULL) {kfree(x); x=NULL;} } while(0)

//x is the value returned if the check failed
#define RANV_CHECK_INDEX(x) do { \
        if (index < 0 || index >= FLASH_BLOCK_NUM) { \
                RANV_PRINT("index(%d) is out of range\n", index); \
            	    return x; \
        } \
} while (0)

#define RANV_CHECK_VALID() do { \
        if (!fb[index].valid) { \
                RANV_PRINT("fb[%d] invalid, init again\n", index); \
		init_nvram_block(index); \
        } \
} while (0)

typedef struct environment_s {
	unsigned long crc;		//CRC32 over data bytes
	char *data;
} env_t;

typedef struct cache_environment_s {
	char *name;
	char *value;
} cache_t;

#define MAX_CACHE_ENTRY 1000
typedef struct block_s {
	char *name;
	env_t env;			//env block
	cache_t	cache[MAX_CACHE_ENTRY];	//env cache entry by entry
	unsigned long flash_offset;
	unsigned long flash_max_len;	//ENV_BLK_SIZE

	char valid;
	char dirty;
} block_t;

#define MAX_VALUE_LEN (ENV_BLK_SIZE * 5)
#define MAX_PERMITTED_VALUE_LEN  (MAX_VALUE_LEN * 2)
typedef struct nvram_ioctl_s {
	int index;
	int ret;
	char *name;
	char *value;
	int size;
} nvram_ioctl_t;

#define RALINK_NVRAM_IOCTL_GET		0x01
#define RALINK_NVRAM_IOCTL_GETALL	0x02
#define RALINK_NVRAM_IOCTL_SET		0x03
#define RALINK_NVRAM_IOCTL_COMMIT	0x04
#define RALINK_NVRAM_IOCTL_CLEAR	0x05

extern int ra_mtd_write_nm(char *name, loff_t to, size_t len, const u_char *buf);
extern int ra_mtd_read_nm(char *name, loff_t from, size_t len, u_char *buf);
#endif
