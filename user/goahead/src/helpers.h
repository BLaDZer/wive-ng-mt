#ifndef HELPERS_H_
#define HELPERS_H_ 1

#include "sdk_version.h"			//version config
#include "config/autoconf.h"			//user config
#include "user/busybox/include/autoconf.h"	//busybox config

#include <linux/autoconf.h>			//kernel config
#include <syslog.h>

#include "libnvram.h"
#include "libnvram_utils.h"

#include "libwive.h"

#include "webs.h"
#include "uemf.h"

#ifdef CONFIG_RALINKAPP_SWQOS
#include "qos.h"
#endif

#define SPLITTER_BUFFER_QUANTITY        1024
#define SPLITTER_TOKEN_QUANTITY         32

//----------------------------------------------
// System defines

#define LED_ON				1
#define LED_OFF				0

#if defined (CONFIG_RALINK_MT7620)
#define PROCREG_DIR			"mt7620"
#elif defined (CONFIG_RALINK_MT7621)
#define PROCREG_DIR			"mt7621"
#elif defined (CONFIG_RALINK_MT7628)
#define PROCREG_DIR			"mt7628"
#else
#define PROCREG_DIR			"mt7620"
#endif

#ifdef CONFIG_RAETH_SNMPD
#define PROCREG_SNMP			("/proc/" PROCREG_DIR "/snmp")
#endif
#define PROCREG_GMAC			("/proc/" PROCREG_DIR "/gmac")

#define IOCTL_IF "eth2"

#define IPT_SHORT_ACCOUNT

#define _PATH_PROCNET_DEV		"/proc/net/dev"
#define _PATH_IPT_ACCOUNTING_FILE	"/proc/net/ipt_account/mynetwork"

#ifdef CONFIG_IPV6
#define _PATH_PROCNET_IPV6		"/proc/net/if_inet6"

#define SIXRD_SIG			"6rd"
#define SIX2FOUR_SIG			"sit"

#define IPV6_ADDR_GLOBAL        	0x0000U
#define IPV6_ADDR_LOOPBACK      	0x0010U
#define IPV6_ADDR_LINKLOCAL     	0x0020U
#define IPV6_ADDR_SITELOCAL     	0x0040U
#define IPV6_ADDR_COMPATv4      	0x0080U
#endif

#define RULE_MODE_DISABLE		0
#define RULE_MODE_DROP			1
#define RULE_MODE_ACCEPT		2

#define TXBYTE				0
#define TXPACKET			1
#define RXBYTE				2
#define RXPACKET			3

#define DMZ_CHAIN			"DMZ"

#define PROC_MEM_STATISTIC		"/proc/meminfo"
#define PROC_CPU_STATISTIC		"/proc/stat"

// Optimizer macros
#define CHK_IF_DIGIT(var, val)  \
	((var != NULL) && (var[0] == (val + '0')) && (var[1] == '\0'))

#define CHK_GET_DIGIT(var) \
	(((var[0] >= '0') && (var[1] <= '9') && (var[1] == '\0')) ? (var[0] - '0') : -1)

#define CHK_IF_CHECKED(var)  \
	(((var != NULL) && ((var[0] == 'o') || (var[0] == 'O')) && ((var[1] == 'n') || (var[1] == 'N')) && (var[2] == '\0')) ? 1 : 0)

#define CHK_IF_SET(var) \
	((var != NULL) && (var[0] != '\0'))

#define CHK_IF_EMPTYSTR(var) \
	((var == NULL) || (var[0] == '\0'))

/* STF means "Save To Flash" ...*/
#define STF(nvram, index, flash_key)    STFs(nvram, index, #flash_key, flash_key)

/* LFF means "Load From Flash" ...*/
#define LFF(result, nvram, x, n)								\
				do{		char tmp[128];					\
				    if(! ( x  = nvram_get(nvram, #x)) )				\
					tmp[0] = '\0';						\
				    else{							\
					if( getNthValueSafe(n, x, ';', tmp, 128) != -1){	\
					    strncat(result, tmp, 4096);			\
					}							\
				    }strncat(result, "\r", 4096);				\
				}while(0)

/* Load from Web */
#define LFW(x, y) do{ if(! ( x = websGetVar(wp, T(#y), T("")))) return;	}while(0)

//----------------------------------------------
/* Special functions */
typedef struct replacement_t
{
	const char *key;
	const char *value;
} replacement_t;

typedef struct parameter_fetch_t
{
	char *web_param;
	char *nvram_param;
	int is_switch;
	char *dfl_param;
} parameter_fetch_t;

typedef struct string_split_t
{
	size_t  buf_size;
	size_t  pointers;
	size_t  found;
	char   *buf;
	char  **items;
} string_split_t;

// Set-up parameters in NVRAM
void setupParameters(webs_t wp, parameter_fetch_t *fetch, int transaction);

// String splitting tools
int initSplitter(string_split_t *buf);
int splitString(string_split_t *buf, const char *string, char splitter);
int freeSplitter(string_split_t *buf);

void outputTimerForReload(webs_t wp, char_t *url, long delay);
int doSystem(char_t *fmt, ...);
void reboot_now(void);
#endif /* HELPERS_H_ */
