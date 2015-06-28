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
#include	<sys/time.h>
#include	<sys/socket.h>
#include	<sys/sysinfo.h>
#include	<netinet/in.h>
#include	<arpa/inet.h>

#include	"utils.h"
#include	"internet.h"
#include	"station.h"

static int  getLangBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getStationBuilt(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgGeneralHTML(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthGeneral(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getCfgNthZero(int eid, webs_t wp, int argc, char_t **argv);
static int  getPlatform(int eid, webs_t wp, int argc, char_t **argv);
static int  getSdkVersion(int eid, webs_t wp, int argc, char_t **argv);
static int  getMemAmount(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysUptime(int eid, webs_t wp, int argc, char_t **argv);
static int  getSysDateTime(int eid, webs_t wp, int argc, char_t **argv);
static int  getPortStatus(int eid, webs_t wp, int argc, char_t **argv);
static int  getStaDriverVer(int eid, webs_t wp, int argc, char_t **argv);
static int  getStaMacAddrw(int eid, webs_t wp, int argc, char_t **argv);
static int  gigaphy(int eid, webs_t wp, int argc, char_t **argv);
static void setOpMode(webs_t wp, char_t *path, char_t *query);
static void setWanPort(webs_t wp, char_t *path, char_t *query);

/*********************************************************************
 * System Utilities
 */
void reboot_now(void)
{
	Sleep(2);
	sync();
	/* send dhcpc release */
	doSystem("killall -q -SIGUSR2 udhcpc > /dev/null 2>&1");
	/* always send output to dev/null, fix html_error output */
	doSystem("/etc/scripts/wifi_unload.sh > /dev/null 2>&1");
	sync();
	Sleep(2);
	doSystem("(sleep 5 && reboot) > /dev/null 2>&1 &");
}

void arplookup(char *ip, char *arp)
{
    char buf[256];
    FILE *fp = fopen("/proc/net/arp", "r");
    if(!fp){
        trace(0, T("no proc fs mounted!\n"));
        return;
    }
    strcpy(arp, "00:00:00:00:00:00");
    while(fgets(buf, 256, fp)){
        char ip_entry[32], hw_type[8],flags[8], hw_address[32];
        sscanf(buf, "%s %s %s %s", ip_entry, hw_type, flags, hw_address);
        if(!strcmp(ip, ip_entry)){
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

    while(i<index && end){
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
	while(end){
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
 * The setitimer() is Linux-specified.
 */
int setTimer(int microsec, void ((*sigroutine)(int)))
{
	struct itimerval value, ovalue;

	signal(SIGALRM, sigroutine);
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = microsec;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = microsec;
	return setitimer(ITIMER_REAL, &value, &ovalue);
}

void stopTimer(void)
{
	struct itimerval value, ovalue;

	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = 0;
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = 0;
	setitimer(ITIMER_REAL, &value, &ovalue);
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
	if (fd < 0) {
		perror(GPIO_DEV);
		return -1;
	}
	if (ioctl(fd, RALINK_GPIO_LED_SET, &led) < 0) {
		perror("ioctl");
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

void websLongWrite(webs_t wp, char *longstr)
{
    char tmp[513] = {0};
    int len = strlen(longstr);
    char *end = longstr + len;

    while(longstr < end){
        strncpy(tmp, longstr, 512);
        websWrite(wp, T("%s"), tmp);
        longstr += 512;
    }
    return;
}

char *strip_space(char *str)
{
	while( *str == ' ')
		str++;
	return str;
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
	websAspDefine(T("getMemAmount"), getMemAmount);
	websAspDefine(T("getSysUptime"), getSysUptime);
	websAspDefine(T("getSysDateTime"), getSysDateTime);
	websAspDefine(T("getPortStatus"), getPortStatus);
	websAspDefine(T("getStaDriverVer"), getStaDriverVer);
	websAspDefine(T("getStaMacAddrw"), getStaMacAddrw);
	websFormDefine(T("setOpMode"), setOpMode);
	websFormDefine(T("setWanPort"), setWanPort);
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
	    printf("goahead: Unknown lang %s. Set lang to en.\n", value);
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
		printf("goahead: Unknown lang %s. Set lang to en.\n", value);
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
#if defined(CONFIG_MT7610_AP) || defined(CONFIG_MT7610_AP_MODULE)
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, MT7610 1T1R 5GHz, 100FDX"));
#elif defined(CONFIG_MT76X2_AP) || defined(CONFIG_MT76X2_AP_MODULE)
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, MT7612 2T2R 5GHz, 100FDX"));
#else
    return websWrite(wp, T("MT7620 2T2R 2.4GHz, 100FDX"));
#endif
#elif defined(CONFIG_RALINK_MT7621) && defined(CONFIG_MT7530_GSW)
    return websWrite(wp, T("MT7621 1000FDX"));
#else
    return websWrite(wp, T("Unknown switch mode"));
#endif
    return 0;
}

static int getStationBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE)
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
 * description: check ram size
 */
static int getMemAmount(int eid, webs_t wp, int argc, char_t **argv)
{
	char line[256];
	char key[64];
	long long value;

	FILE *fp = fopen("/proc/meminfo", "r");

	// Now open /proc/meminfo and output neccessary information
	if (fp != NULL)
	{
		while ( fgets(line, sizeof(line), fp) )
		{
			if (sscanf(line, "%s %lld", key, &value) == 2)
			{
				if (strcmp(key, "MemTotal:") == 0)
				{
					fclose(fp);
					return websWrite(wp, T("%ld"), (long)value);
				}
			}
		}

		fclose(fp);
	}

	return websWrite(wp, T("0"), RT288X_SDK_VERSION);
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
    }
    else return 0;
}

static int getSysDateTime(int eid, webs_t wp, int argc, char_t **argv)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);

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
		if (!proc_file)
			return 0;

		fprintf(proc_file, "%d", port);
		fclose(proc_file);

		if ((fp = popen("ethtool eth2", "r")) == NULL)
			return 0;

		rc = fread(buf, 1, 1024, fp);
		pclose(fp);
		if (rc != -1)
		{
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

int netmask_aton(const char *ip)
{
	int i, a[4], result = 0;
	sscanf(ip, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]);
	for(i=0; i<4; i++){	/* this is dirty */
		if(a[i] == 255){
			result += 8;
			continue;
		}
		if(a[i] == 254)
			result += 7;
		if(a[i] == 252)
			result += 6;
		if(a[i] == 248)
			result += 5;
		if(a[i] == 240)
			result += 4;
		if(a[i] == 224)
			result += 3;
		if(a[i] == 192)
			result += 2;
		if(a[i] == 128)
			result += 1;
		break;
	}
	return result;
}

static void outputTimerForReload(webs_t wp, long delay)
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

	websHeader(wp);
	websWrite
	(
		wp,
		T(
		"<script language=\"JavaScript\" type=\"text/javascript\">\n"
		"ajaxReloadDelayedPage(%ld, \"http://%s\");\n"
		"</script>"),
		delay, lan_if_ip
	);
	websFooter(wp);
	websDone(wp, 200);
}

/* goform/setOpMode */
static void setOpMode(webs_t wp, char_t *path, char_t *query)
{
	char	*mode, *old_mode;

	nvram_init(RT2860_NVRAM);

	/* get modes */
	mode = websGetVar(wp, T("opMode"), T("0"));
	old_mode = nvram_bufget(RT2860_NVRAM, "OperationMode");

	/* new OperationMode */
	if (strncmp(mode, old_mode, 2))
	{
		nvram_bufset(RT2860_NVRAM, "OperationMode", mode);
#ifdef CONFIG_USER_CHILLISPOT
		if (!strncmp(mode, "4", 2))
			nvram_bufset(RT2860_NVRAM, "dhcpEnabled", "0");
#endif
		/* from or to ap client mode */
		if (!strncmp(mode, "3", 2))
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "1");
		else if (!strncmp(old_mode, "3", 2)) {
			/* from apcli to other mode need drop bridge only flag to */
			nvram_bufset(RT2860_NVRAM, "ApCliEnable", "0");
			nvram_bufset(RT2860_NVRAM, "ApCliBridgeOnly", "0");
		}
		nvram_commit(RT2860_NVRAM);
#ifdef CONFIG_USER_802_1X
		updateFlash8021x(RT2860_NVRAM);
#endif
	}
	nvram_close(RT2860_NVRAM);

	outputTimerForReload(wp, 80000);
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

	/* Output timer for reloading */
	outputTimerForReload(wp, 50000);

	/* Reboot */
	reboot_now();
}

void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}

/*
 * description: write station driver version
 */
static int getStaDriverVer(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE)
	unsigned char DriverVersionInfo[8];
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);

	/* Driver */
	if (OidQueryInformation(RT_OID_VERSION_INFO, s, "ra0", &DriverVersionInfo, sizeof(DriverVersionInfo)) >= 0) {
		websWrite(wp, "%s", DriverVersionInfo);
	}
	else
		websWrite(wp, "&nbsp;");

	close(s);
#else
	websWrite(wp, "STA driver not compiled &nbsp;");
#endif
	return 0;
}

/*
 * description: write station mac address
 */
static int getStaMacAddrw(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE)
	unsigned char CurrentAddress[6];
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (OidQueryInformation(OID_802_3_CURRENT_ADDRESS, s, "ra0", &CurrentAddress, sizeof(CurrentAddress)) >= 0)
		websWrite(wp, "%02X-%02X-%02X-%02X-%02X-%02X", CurrentAddress[0], CurrentAddress[1],
				CurrentAddress[2], CurrentAddress[3], CurrentAddress[4], CurrentAddress[5]);
	else
		websWrite(wp, "&nbsp;");

	close(s);
#else
	websWrite(wp, "&nbsp;");
#endif
	return 0;
}
