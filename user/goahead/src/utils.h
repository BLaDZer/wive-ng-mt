/*
 *	utils.h -- System Utilitie Header
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.h,v 1.25 2008-03-17 09:28:40 yy Exp $
 */
#ifndef _h_UTILS
#define _h_UTILS 1

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

void formDefineUtilities(void);
void outputTimerForReload(webs_t wp, char_t *url, long delay);
void reboot_now(void);
int checkSemicolon(char *str);
int doSystem(char_t *fmt, ...);
char *getNthValue(int index, char *values);
char *setNthValue(int index, char *old_values, char *new_value);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);
int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
int ledAlways(int gpio, int on);
unsigned int Sleep(unsigned int secs);
char *racat(char *s, int i);
void arplookup(char *ip, char *arp);
char *strip_space(char *str);
unsigned int ConvertRssiToSignalQuality(long RSSI);

#endif
