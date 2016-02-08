/* vi: set sw=4 ts=4 sts=4: */
/*
 *	utils.c -- System Utilities
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: utils.c,v 1.99.2.1 2009-04-08 08:52:59 chhung Exp $
 */
#include	<time.h>
#include	<signal.h>
#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	<asm/types.h>
#include	<linux/if.h>
#include	<sys/ioctl.h>
#include	<sys/time.h>
#include	<sys/socket.h>
#include	<sys/sysinfo.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>

#include	"utils.h"
#include	"internet.h"
#include	"station.h"
#include	"helpers.h"

static int  getLangBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getStationBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgGeneralHTML(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getPlatform(int eid, webs_t wp, int argc, char_t **argv);
static int  getSdkVersion(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysUptime(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysDateTime(int eid, webs_t wp, int argc, char_t **argv);
static int  getPortStatus(int eid, webs_t wp, int argc, char_t **argv);
static int  gigaphy(int eid, webs_t wp, int argc, char_t **argv);
static void setOpMode(webs_t wp, char_t *path, char_t *query);
static void setWanPort(webs_t wp, char_t *path, char_t *query);
static void reboot_web(webs_t wp, char_t *path, char_t *query);

/*********************************************************************
 * System Utilities
 */
void reboot_now(void)
{
	doSystem("(sleep 2 && /etc/scripts/reboot.sh) > /dev/null 2>&1 &");
}

void arplookup(char *ip, char *arp)
{
    char buf[256];
    FILE *fp = fopen("/proc/net/arp", "r");
    if(!fp) {
        trace(0, T("no proc fs mounted!\n"));
        return;
    }

    strcpy(arp, "00:00:00:00:00:00");
    while(fgets(buf, sizeof(buf), fp)) {
        char ip_entry[32], hw_type[8],flags[8], hw_address[32];
	if (buf == NULL || buf[0] == '\n')
		continue;
        sscanf(buf, "%s %s %s %s", ip_entry, hw_type, flags, hw_address);
        if(!strcmp(ip, ip_entry)) {
            strcpy(arp, hw_address);
            break;
        }
    }

    fclose(fp);
    return;
}

/*
 * description: parse va and do system
 */
int doSystem(char_t *fmt, ...)
{
	va_list		vargs;
	char_t		*cmd = NULL;
	int			rc = 0;

	va_start(vargs, fmt);
	if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE) {
		trace(0, T("doSystem: lost data, buffer overflow\n"));
	}
	va_end(vargs);

	if (cmd) {
		trace(0, T("%s\n"), cmd);
		rc = system(cmd);
		bfree(B_L, cmd);
	}
	return rc;
}

/*
 * arguments: index - index of the Nth value
 *            values - un-parsed values
 * description: parse values delimited by semicolon, and return the value
 *              according to given index (starts from 0)
 * WARNING: the original values will be destroyed by strtok
 */
char *getNthValue(int index, char *values)
{
	int i;
	static char *tok;

	if (NULL == values)
		return NULL;
	for (i = 0, tok = strtok(values, ";");
			(i < index) && tok;
			i++, tok = strtok(NULL, ";")) {
		;
	}
	if (NULL == tok)
		return "";
	return tok;
}

/*
 * arguments: index - index of the Nth value (starts from 0)
 *            old_values - un-parsed values
 *            new_value - new value to be replaced
 * description: parse values delimited by semicolon,
 *              replace the Nth value with new_value,
 *              and return the result
 * WARNING: return the internal static string -> use it carefully
 */
char *setNthValue(int index, char *old_values, char *new_value)
{
	int i;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
		memset(buf[i], 0, 256);

	/* copy original values */
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
	      i < 8 && q != NULL                         ;
	      i++, p = q + 1, q = strchr(p, ';')         )
	{
		strncpy(buf[i], p, q - p);
	}
	strcpy(buf[i], p); /* the last one */

	/* replace buf[index] with new_value */
	strncpy(buf[index], new_value, 256);

	/* calculate maximum index */
	index = (i > index)? i : index;

	/* concatenate into a single string delimited by semicolons */
	strcat(ret, buf[0]);
	for (i = 1; i <= index; i++) {
		strncat(ret, ";", 2);
		strncat(ret, buf[i], 256);
	}

	p = ret;
	return p;
}

/*
 * check the existence of semicolon in str
 */
int checkSemicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

/*
 * substitution of getNthValue which dosen't destroy the original value
 */
int getNthValueSafe(int index, char *value, char delimit, char *result, int len)
{
    int i = 0, result_len = 0;
    char *begin, *end;

    if(!value || !result || !len)
        return -1;

    begin = value;
    end = strchr(begin, delimit);
    while(i < index && end != NULL){
        begin = end + 1;
        end = strchr(begin, delimit);
        i++;
    }

    /* no delimit */
    if(!end){
	if(i == index){
	    end = begin + strlen(begin);
	    result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
	} else
		return -1;
    } else
	result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);

    memcpy(result, begin, result_len);
    *(result + result_len ) = '\0';

	return 0;
}

/*
 *  argument:  [IN]     index -- the index array of deleted items(begin from 0)
 *             [IN]     count -- deleted itmes count.
 *             [IN/OUT] value -- original string/return string
 *             [IN]     delimit -- delimitor
 */
int deleteNthValueMulti(int index[], int count, char *value, char delimit)
{
	char *begin, *end;
	int i=0,j=0;
	int need_check_flag=0;
	char *buf = strdup(value);

	begin = buf;

	end = strchr(begin, delimit);
	while(end != NULL){
		if(i == index[j]){
			memset(begin, 0, end - begin );
			if(index[j] == 0)
				need_check_flag = 1;
			j++;
			if(j >=count)
				break;
		}
		begin = end;
		end = strchr(begin+1, delimit);
		i++;
	}

	if(!end && index[j] == i)
		memset(begin, 0, strlen(begin));

	if(need_check_flag){
		for(i=0; i<strlen(value); i++){
			if(buf[i] == '\0')
				continue;
			if(buf[i] == ';')
				buf[i] = '\0';
			break;
		}
	}

	for(i=0, j=0; i<strlen(value); i++){
		if(buf[i] != '\0'){
			value[j++] = buf[i];
		}
	}
	value[j] = '\0';

	free(buf);
	return 0;
}

int getNums(char *value, char delimit)
{
    char *pos = value;
    int count = 1;
    if (!pos || !(*pos))
        return 0;

    while ((pos = strchr(pos, delimit)) != NULL) {
        ++pos;
        ++count;
    }
    return count;
}

/*
 * nanosleep(2) don't depend on signal SIGALRM and could cooperate with
 * other SIGALRM-related functions(ex. setitimer(2))
 */
unsigned int Sleep(unsigned int secs)
{
	int rc;
	struct timespec ts, remain;
	ts.tv_sec  = secs;
	ts.tv_nsec = 0;

sleep_again:
	rc = nanosleep(&ts, &remain);
	if(rc == -1 && errno == EINTR){
		ts.tv_sec = remain.tv_sec;
		ts.tv_nsec = remain.tv_nsec;
		goto sleep_again;
	}
	return 0;
}

/*
 * configure LED blinking with proper frequency (privatly use only)
 *   on: number of ticks that LED is on
 *   off: number of ticks that LED is off
 *   blinks: number of on/off cycles that LED blinks
 *   rests: number of on/off cycles that LED resting
 *   times: stop blinking after <times> times of blinking
 * where 1 tick == 100 ms
 */
static int gpioLedSet(int gpio, unsigned int on, unsigned int off,
		unsigned int blinks, unsigned int rests, unsigned int times)
{
	int fd;
	ralink_gpio_led_info led;

	//parameters range check
	if (gpio < 0 || gpio >= RALINK_GPIO_NUMBER ||
			on > RALINK_GPIO_LED_INFINITY ||
			off > RALINK_GPIO_LED_INFINITY ||
			blinks > RALINK_GPIO_LED_INFINITY ||
			rests > RALINK_GPIO_LED_INFINITY ||
			times > RALINK_GPIO_LED_INFINITY) {
		return -1;
	}
	led.gpio = gpio;
	led.on = on;
	led.off = off;
	led.blinks = blinks;
	led.rests = rests;
	led.times = times;

	fd = open(GPIO_DEV, O_RDONLY | O_NONBLOCK);
	if (fd < 0)
		return -1;

	if (ioctl(fd, RALINK_GPIO_LED_SET, &led) < 0) {
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int ledAlways(int gpio, int on)
{
	if (on)
		return gpioLedSet(gpio, RALINK_GPIO_LED_INFINITY, 0, 1, 1, RALINK_GPIO_LED_INFINITY);
	else
		return gpioLedSet(gpio, 0, RALINK_GPIO_LED_INFINITY, 1, 1, RALINK_GPIO_LED_INFINITY);
}

/*
 * concatenate a string with an integer
 * ex: racat("SSID", 1) will return "SSID1"
 */
char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

char *strip_space(char *str)
{
	while (isspace(*str))
		str++;
	return str;
}

/*
 * description: return LAN interface name
 */
char* getLanIfName(void)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");
	static char *if_name = "br0";
	FILE *fp;
	char lan_if[16]; /* max 16 char in lan if name */

	/* set default */
	if (mode == NULL)
		return if_name;

	/* try read fron file exported from init.d */
	fp = fopen("/tmp/lan_if_name", "r");
	if (fp) {
	    /* get first lan_if in file */
	    while (fgets(lan_if, sizeof(lan_if), fp)) {
		if (lan_if == NULL || lan_if[0] == '\n')
			continue;
		if ((strstr(lan_if, ETH_SIG) != NULL) || (strstr(lan_if, BR_SIG) != NULL)) {
		    fclose(fp);
		    return strip_space(lan_if);
		}
	    }
	    fclose(fp);
	}

	/* in ethernet converter mode lan_if = eth2 */
	if (!strncmp(mode, "2", 2))
	    if_name = "eth2";
	else
	    if_name = "br0";

	return if_name;
}

/*
 * description: return WAN interface name depend by opmode
 */
char* getWanIfName(void)
{
	char *mode = nvram_get(RT2860_NVRAM, "OperationMode");
	char *apc_cli_mode = nvram_get(RT2860_NVRAM, "ApCliBridgeOnly");
	char *apc_cli_wanif = nvram_get(RT2860_NVRAM, "ApCliIfName");
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

	/* set default */
	if (mode == NULL)
		return WAN_DEF;

	if (!strncmp(mode, "0", 2))					/* bridge mode */
		if_name = "br0";
	else if (!strncmp(mode, "1", 2) || !strncmp(mode, "4", 2))	/* gw and chillispot mode */
		if_name = WAN_DEF;
	else if (!strncmp(mode, "2", 2))				/* ethernet converter mode */
		if_name = "ra0";
	else if (!strncmp(mode, "3", 2)) {				/* apcli mode */
		    if (!strncmp(apc_cli_mode, "1", 2)) {
			if_name = "br0";				/* Client-AP-Bridge */
		    } else {
			if_name = apc_cli_wanif;			/* Client-AP-Gateway 2.4Ghz/5GHz */
		    }
		}

	return if_name;
}

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
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

/*
 * arguments: ifname  - interface name
 *            if_addr - a 18-byte buffer to store mac address
 * description: fetch mac address according to given interface name
 */
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
	sprintf(if_hw, "%02X:%02X:%02X:%02X:%02X:%02X",
			(ptr[0] & 0377), (ptr[1] & 0377), (ptr[2] & 0377),
			(ptr[3] & 0377), (ptr[4] & 0377), (ptr[5] & 0377));

	close(skfd);
	return 0;
}

/*
 * arguments: ifname - interface name
 *            if_net - a 16-byte buffer to store subnet mask
 * description: fetch subnet mask associated to given interface name
 */
int getIfNetmask(char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s\n", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s\n", __FUNCTION__);
		return -1;
	}
	strcpy(if_net, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
	close(skfd);
	return 0;
}

/*
 * description: return vpn interface name or VPN_DEF as default
 */
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

/*
 * description: return 1 if vpn enables or 0 if disabled
 */
int vpn_mode_enabled(void)
{
	char *cm = nvram_get(RT2860_NVRAM, "vpnEnabled");

        if (!strncmp(cm, "on", 3))
	    return 1;
        else
	    return 0;
}

/*
 * description: get the value "WAN" or "LAN" the interface is belong to.
 */
char *getLanWanNamebyIf(char *ifname)
{
	if(!strcmp(ifname, getLanIfName()))
		return "LAN";
	if(!strcmp(ifname, getWanIfName()))
		return "WAN";
	if (vpn_mode_enabled() == 1 && strstr(ifname, getPPPIfName()) != NULL)
		return "VPN";

	return "LAN";
}

/*********************************************************************
 * Web Related Utilities
 */
void formDefineUtilities(void)
{
	websAspDefine(T("getCfgGeneral"), getCfgGeneral);
	websAspDefine(T("getCfgGeneralHTML"), getCfgGeneralHTML);
	websAspDefine(T("getCfgNthGeneral"), getCfgNthGeneral);
	websAspDefine(T("getCfgZero"), getCfgZero);
	websAspDefine(T("getCfgNthZero"), getCfgNthZero);
	websAspDefine(T("getLangBuilt"), getLangBuilt);
	websAspDefine(T("getPlatform"), getPlatform);
	websAspDefine(T("getStationBuilt"), getStationBuilt);
	websAspDefine(T("getSdkVersion"), getSdkVersion);
	websAspDefine(T("getSysUptime"), getSysUptime);
	websAspDefine(T("getSysDateTime"), getSysDateTime);
	websAspDefine(T("getPortStatus"), getPortStatus);
	websFormDefine(T("setOpMode"), setOpMode);
	websFormDefine(T("setWanPort"), setWanPort);
	websFormDefine(T("reboot"), reboot_web);
	websAspDefine(T("gigaphy"), gigaphy);
}


/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2)
		return websWrite(wp, T("Insufficient args\n"));

	value = nvram_get(RT2860_NVRAM, field);

	if ((!value) && (strcmp(field, "Language") == 0)) {
	    syslog(LOG_ERR, "Unknown lang %s. Set lang to en, %s\n", value, __FUNCTION__);
	    value = "en";
	}

        if (type == 1) {
                if (!value)
                        return websWrite(wp, T(""));
                return websWrite(wp, T("%s"), value);
        }

	if (!value)
	    ejSetResult(eid, "");
	else
	    ejSetResult(eid, value);

    return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneralHTML(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2)
		return websWrite(wp, T("Insufficient args\n"));

	value = nvram_get(RT2860_NVRAM, field);

	if ((!value) && (strcmp(field, "Language") == 0))
	{
		syslog(LOG_ERR, "Unknown lang %s. Set lang to en, %s\n", value, __FUNCTION__);
		value = "en";
	}

	if (type == 1)
	{
		if (!value)
			return websWrite(wp, T(""));

		int result = 0;
		for (; *value != '\0'; value++)
		{
			switch (*value)
			{
				case '&':
					result += websWrite(wp, T("&amp;"));
					break;
				case '\"':
					result += websWrite(wp, T("&quot;"));
					break;
				case '<':
					result += websWrite(wp, T("&lt;"));
					break;
				case '>':
					result += websWrite(wp, T("&gt;"));
					break;
				default:
					result += websWrite(wp, T("%c"), *value);
					break;
			}
		}
		return result;
	}

	if (!value)
		ejSetResult(eid, "");
	else
		ejSetResult(eid, value);

	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (NULL == value)
			return websWrite(wp, T(""));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T(""));
		return websWrite(wp, T("%s"), nth);
	}
	if (NULL == value)
		ejSetResult(eid, "");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfgZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return websWrite(wp, T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return websWrite(wp, T("0"));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return websWrite(wp, T("0"));
		return websWrite(wp, T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}

static int getLangBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *lang;

	if (ejArgs(argc, argv, T("%s"), &lang) < 1) {
		return websWrite(wp, T("0"));
	}
	if (!strncmp(lang, "en", 3))
		return websWrite(wp, T("1"));
	else if (!strncmp(lang, "ru", 3))
#ifdef CONFIG_USER_GOAHEAD_LANG_RU
		return websWrite(wp, T("1"));
#else
		return websWrite(wp, T("0"));
#endif
	else	return websWrite(wp, T("0"));
}

/*
 * description: write the current system platform accordingly
 */
static int getPlatform(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RALINK_MT7620) && defined(CONFIG_RAETH_ESW)
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, MT7610 1T1R 5GHz, 100FDX"));
#elif defined(CONFIG_RT_SECOND_IF_MT7612E)
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, MT7612 2T2R 5GHz, 100FDX"));
#else
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, 100FDX"));
#endif
#elif defined(CONFIG_RALINK_MT7621) && defined(CONFIG_MT7530_GSW)
    return websWrite(wp, T("MT7621 1000FDX MT76x2 2T2R dualband"));
#else
    return websWrite(wp, T("Unknown switch mode"));
#endif
    return 0;
}

static int getStationBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int gigaphy(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RALINK_MT7621)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write RT288x SDK version
 */
static int getSdkVersion(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("%s"), RT288X_SDK_VERSION);
}

/*
 * description: write System Uptime
 */
static int getSysUptime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct sysinfo sinf;

	if (sysinfo(&sinf) == 0)
	{
	    return websWrite(wp, T("%d day%s, %d hour%s, %d min%s, %d sec%s"),
			sinf.uptime/(unsigned)86400, ((sinf.uptime/(unsigned)86400) == 1)? "" : "s",
    			(sinf.uptime%(unsigned)86400)/(unsigned)3600, (((sinf.uptime%(unsigned)86400)/(unsigned)3600) == 1)? "" : "s",
			(sinf.uptime%(unsigned)3600)/(unsigned)60, (((sinf.uptime%(unsigned)3600)/(unsigned)60) == 1)? "" : "s",
			sinf.uptime%(unsigned)60, ((sinf.uptime%(unsigned)60) == 1)? "" : "s");
	} else
	    return -1;
}

static int getSysDateTime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);
	if (utime == NULL)
	    return -1;

	return websWrite(wp, T("%02u:%02u:%02u %02u.%02u.%04u"),
				utime->tm_hour, utime->tm_min, utime->tm_sec,
                utime->tm_mday, (utime->tm_mon + 1), (1900 + utime->tm_year));
}

static int getPortStatus(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_USER_ETHTOOL)
#if defined(CONFIG_RAETH_ESW) || defined(CONFIG_MT7530_GSW)
	int port, rc;
	FILE *fp;
	char buf[1024];
	int first = 1;

	for (port=4; port>-1; port--)
	{
		char *pos;
		char link = '0';
		int speed = 100;
		char duplex = 'F';
		FILE *proc_file = fopen(PROCREG_GMAC, "w");
		if (!proc_file) {
			syslog(LOG_ERR, "no proc, %s\n", __FUNCTION__);
			return -1;
		}
		fprintf(proc_file, "%d", port);
		fclose(proc_file);

		if ((fp = popen("ethtool eth2", "r")) == NULL) {
			syslog(LOG_ERR, "no ethtool, %s\n", __FUNCTION__);
			return -1;
		}

		rc = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (rc < 1) {
			syslog(LOG_ERR, "no ethtool pipe read, %s\n", __FUNCTION__);
			return -1;
		} else {
			/* get Link status */
			if ((pos = strstr(buf, "Link detected: ")) != NULL)
			{
				pos += strlen("Link detected: ");
				if(*pos == 'y')
					link = '1';
			}
			/* get speed */
			if ((pos = strstr(buf, "Speed: ")) != NULL)
			{
				pos += strlen("Speed: ");
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == 'M')
					speed = 10;
				if(*pos == '1' && *(pos+1) == '0' && *(pos+2) == '0' && *(pos+3) == '0' && *(pos+4) == 'M')
					speed = 1000;
			}
			/* get duplex */
			if ((pos = strstr(buf, "Duplex: ")) != NULL)
			{
				pos += strlen("Duplex: ");
				if(*pos == 'H')
					duplex = 'H';
			}

			websWrite(wp, T("%s%c,%d,%c"), (first) ? "" : ";", link, speed, duplex);
			first = 0;
		}
	}
#endif
#endif
	return 0;
}

void redirect_wholepage(webs_t wp, const char *url)
{
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/html\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("<html><head><script language=\"JavaScript\">"));
	websWrite(wp, T("parent.location.replace(\"%s\");"), url);
	websWrite(wp, T("</script></head></html>"));
}

void outputTimerForReload(webs_t wp, char_t *url, long delay)
{
	char lan_if_addr[32];
	const char *lan_if_ip;

	if (getIfIp(getLanIfName(), lan_if_addr) != -1)
		lan_if_ip = lan_if_addr;
	else
	{
		lan_if_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
		if (lan_if_ip == NULL)
			lan_if_ip = "192.168.1.1";
	}

	char_t *http_port = nvram_get(RT2860_NVRAM, "RemoteManagementPort");

	websHeader(wp);
	if (strcmp(http_port, "80") == 0)
	{
		websWrite
		(
			wp,
			T(
				"<script language=\"JavaScript\" type=\"text/javascript\">\n"
				"ajaxReloadDelayedPage(%ld, \"http://%s%s\");\n"
				"</script>"),
			delay, lan_if_ip, url
		);
	} else {
		websWrite
		(
			wp,
			T(
				"<script language=\"JavaScript\" type=\"text/javascript\">\n"
				"ajaxReloadDelayedPage(%ld, \"http://%s:%s%s\");\n"
				"</script>"),
			delay, lan_if_ip, http_port, url
		);
	}
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/setOpMode */
static void setOpMode(webs_t wp, char_t *path, char_t *query)
{
	char *mode, *old_mode;

	nvram_init(RT2860_NVRAM);

	/* get modes */
	mode = websGetVar(wp, T("opMode"), T("0"));
	old_mode = nvram_get(RT2860_NVRAM, "OperationMode");

	/* new OperationMode */
	if (strncmp(mode, old_mode, 2))
	{
		nvram_bufset(RT2860_NVRAM, "OperationMode", mode);
		/* from or to ap client mode */
		if (!strncmp(mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
		else if (!strncmp(old_mode, "3", 2)) {
			/* from apcli to other mode need drop bridge only flag to */
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
			nvram_bufset(RT2860_NVRAM, "ApCliBridgeOnly", "0");
		}
		nvram_commit(RT2860_NVRAM);
	}
	nvram_close(RT2860_NVRAM);

	/* Output timer for reloading */
	outputTimerForReload(wp, "", 80000);

	/* Reboot */
	reboot_now();
}

static void setWanPort(webs_t wp, char_t *path, char_t *query)
{
	int i;
	char w_name[20];
	char* w_port = websGetVar(wp, T("wan_port"), T("0"));
	char* l_port = websGetVar(wp, T("lan_port"), T("near"));
	char* tv_port = websGetVar(wp, T("tv_stbEnabled"), T("off"));
	char* sip_port = websGetVar(wp, T("sip_stbEnabled"), T("off"));
	char* tv_port_mcast = websGetVar(wp, T("tv_stbMcast"), T("off"));
	char* sip_port_mcast = websGetVar(wp, T("sip_stbMcast"), T("off"));
	char* tv_portVLAN = websGetVar(wp, T("tv_stbVLAN"), T(""));
	char* sip_portVLAN = websGetVar(wp, T("sip_stbVLAN"), T(""));

	nvram_init(RT2860_NVRAM);

	/* Set-up WAN port */
	if ((w_port != NULL) && (strlen(w_port) == 1))
	{
		if ((w_port[0] >= '0') && (w_port[0] <= '4'))
			nvram_bufset(RT2860_NVRAM, "wan_port", w_port);
	}

	/* Set-up first LAN port */
	if ((l_port != NULL) && ((strcmp(l_port, "near") == 0) || (strcmp(l_port, "distant") == 0)))
		nvram_bufset(RT2860_NVRAM, "lan_port", l_port);

	/* Now test values */
	for (i=1; i<=5; i++)
	{
		snprintf(w_name, sizeof(w_name), "port%d_swmode", i);
		w_port = websGetVar(wp, w_name, T("auto"));
		if ((w_port != NULL) && (strlen(w_port)>0))
			nvram_bufset(RT2860_NVRAM, w_name, w_port);
	}

	nvram_bufset(RT2860_NVRAM, "tv_port", (strcmp(tv_port, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "sip_port", (strcmp(sip_port, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "tv_port_mcast", (strcmp(tv_port_mcast, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "sip_port_mcast", (strcmp(sip_port_mcast, "on")==0) ? "1" : "0");
	nvram_bufset(RT2860_NVRAM, "tv_portVLAN", tv_portVLAN);
	nvram_bufset(RT2860_NVRAM, "sip_portVLAN", sip_portVLAN);

	/* Commit & close NVRAM */
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	char_t *reboot_flag = websGetVar(wp, T("reboot"), T("0"));
	char_t *submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (CHK_IF_DIGIT(reboot_flag, 1)) {
		/* Output timer for reloading */
		outputTimerForReload(wp, "" /* submitUrl */, 80000);

		/* Reboot */
		reboot_now();
	} else {
		websRedirect(wp, submitUrl);
	}
}

/* goform/reboot */
static void reboot_web(webs_t wp, char_t *path, char_t *query)
{
	/* Output timer for reloading */
	outputTimerForReload(wp, "", 80000);

	/* only by save and reboot logic must save rwfs */
	doSystem("fs save > /dev/null 2>&1");

	/* Reboot */
	reboot_now();
}

unsigned int ConvertRssiToSignalQuality(long RSSI)
{
	unsigned int signal_quality;
	if (RSSI >= -50)
		signal_quality = 100;
	else if (RSSI >= -80)    // between -50 ~ -80dbm
		signal_quality = (unsigned int)(24 + (RSSI + 80) * 2.6);
	else if (RSSI >= -90)   // between -80 ~ -90dbm
		signal_quality = (unsigned int)((RSSI + 90) * 2.6);
	else    // < -84 dbm
		signal_quality = 0;

	return signal_quality;
}
