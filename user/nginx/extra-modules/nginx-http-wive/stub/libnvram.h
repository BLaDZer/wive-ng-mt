#ifndef _NVRAM_H
#define _NVRAM_H 	1
#define DEMO_MODE

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "autoconf.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_hash.h>
#include <ngx_md5.h>

// FROM util.h !
typedef struct keyval_t {
    char* key;
    char* val;
} keyval_t;

enum UserRole {
    DENY = 0,
    USER = 1,
    ADMIN = 2
};

// FROM sessionid.h !
typedef struct auth_session_t {
    char address[65];
    char sessionid[65];
    char username[65];
    enum UserRole role; // default is Deny
    time_t start_time;
#ifdef DEMO_MODE
    ngx_array_t* nvram_keyval;
#endif
} auth_session_t;


#define NV_DEV				"nvram.conf"
#define DEFAULT_NVRAM                   "nvram_default.conf"
#define DEFAULT_FLASH_ZONE_NAME		"2860"

#define BUFSZ				1024
#define ENV_BLK_SIZE			0x1000

#define RALINK_NVRAM_IOCTL		0x1000
#define RALINK_NVRAM_IOCTL_GET		RALINK_NVRAM_IOCTL + 0x01
#define RALINK_NVRAM_IOCTL_GETALL	RALINK_NVRAM_IOCTL + 0x02
#define RALINK_NVRAM_IOCTL_SET		RALINK_NVRAM_IOCTL + 0x03
#define RALINK_NVRAM_IOCTL_COMMIT	RALINK_NVRAM_IOCTL + 0x04
#define RALINK_NVRAM_IOCTL_CLEAR	RALINK_NVRAM_IOCTL + 0x05

#define NVFREE(x) do { if (x != NULL) {free(x); x=NULL;} } while(0)

/* split 2860 and RTDEV configs by offset */
#if !defined(CONFIG_RT_SECOND_IF_NONE) && defined(CONFIG_KERNEL_NVRAM_SPLIT_INIC)
#define FLASH_BLOCK_NUM 2
#else
#define FLASH_BLOCK_NUM 1
#endif

#define RT2860_NVRAM    		0
#define RTINIC_NVRAM    		1

/*
typedef struct keyval_t {
    char* key;
    char* val;
} keyval_t;
*/

typedef struct environment_s {
	unsigned long crc;		//CRC32 over data bytes
	char *data;
} environment_t;

typedef struct cache_environment_s {
	char *name;
	char *value;
} cache_t;

#define MAX_CACHE_ENTRY 1000
typedef struct block_s {
	char *name;
	environment_t env;			//env block
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

int nvram_set_session(auth_session_t* sess);

int getNvramNum(void);
char *getNvramName(int index);
unsigned int getNvramIndex(char *name);

int nvram_init(int index);
void nvram_close(int index);

int nvram_set(int index, char *name, char *value);
int nvram_bufset(int index, char *name, char *value);
char *nvram_get(int index, char *name);
char *nvram_get_copy(int index, char *name);
int nvram_get_int(int index, char *name, int def);

size_t nvram_get_tuple(int index, char *key, unsigned elem_index, char *value, size_t value_size);
void nvram_set_tuple(int index, char *key, unsigned elem_index, const char *value);

int nvram_commit(int index);
int nvram_clear(int index);
int nvram_renew(int mode, char *fname);
int nvram_show(int mode);
int nvram_fromdef(int idx_nvram, int num, ...);

int nvram_iter(int mode, void* extra_ptr, void (*callback)(void* extra_ptr, char* name, char* val) );
void nvram_bufiter( int index, void* extra_ptr, void (*callback)(void* extra_ptr, char* name, char* val) );

int gen_wifi_config(int mode, int genmode);
int nvram_load_default(void);

#endif
