/*
 *	utils.h -- System Utilitie Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.h,v 1.25 2008-03-17 09:28:40 yy Exp $
 */
#include        "sdk_version.h"				//version config
#include        "config/autoconf.h"			//user config
#include        "user/busybox/include/autoconf.h"	//busybox config
#include        <linux/autoconf.h>			//kernel config

#include        "linux/ralink_gpio.h"			//gpio config

#include	<linux/reboot.h>
#include	<sys/reboot.h>

#include 	"webs.h"
#include	"uemf.h"

#include        "libnvram.h"
#include        "oid.h"

#ifdef CONFIG_RALINKAPP_SWQOS
#include	"qos.h"
#endif

#define LED_ON				1
#define LED_OFF				0

#if defined (CONFIG_RALINK_MT7620)
#define PROCREG_GMAC	"/proc/mt7620/gmac"
#elif defined (CONFIG_RALINK_MT7621)
#define PROCREG_GMAC	"/proc/mt7621/gmac"
#elif defined (CONFIG_RALINK_MT7628)
#define PROCREG_GMAC	"/proc/mt7628/gmac"
#else
#define PROCREG_GMAC	"/proc/mt7620/gmac"
#endif

void reboot_now(void);
void outputTimerForReload(webs_t wp, long delay);
void formDefineUtilities(void);
int checkSemicolon(char *str);
int doSystem(char_t *fmt, ...);
char *getNthValue(int index, char *values);
char *setNthValue(int index, char *old_values, char *new_value);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);
int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
int setTimer(int mili, void ((*sigroutine)(int)));
void stopTimer(void);
int ledAlways(int gpio, int on);
unsigned int Sleep(unsigned int secs);
char *racat(char *s, int i);
void arplookup(char *ip, char *arp);
void websLongWrite(webs_t wp, char *longstr);
char *strip_space(char *str);
int netmask_aton(const char *ip);
void STFs(int nvram, int index, char *flash_key, char *value);
#ifdef CONFIG_USER_802_1X
void restart8021XDaemon(int nvram);
void updateFlash8021x(int nvram);
#endif
