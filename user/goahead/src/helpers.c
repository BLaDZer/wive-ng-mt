#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

#include "helpers.h"

#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const char *sizes[]   = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" };
static const uint64_t  exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;

char *scale(uint64_t size)
{
    char *result = (char *)balloc(B_L, sizeof(char) * 20);
    uint64_t  multiplier = exbibytes;
    int i;

    for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
    {
        if (size < multiplier)
            continue;
        if (size % multiplier == 0)
            sprintf(result, "%" PRIu64 " %s", size / multiplier, sizes[i]);
        else
            sprintf(result, "%.1f %s", (float) size / multiplier, sizes[i]);
        return result;
    }
    strcpy(result, "0");
    return result;
}

// Sizes table
const char *normalSizes[] =
{
	"B",
	"kB",
	"MB",
	"GB",
	"TB",
	"PB",
	"HB",
	"ZB"
};

// Normalize size
const char *normalizeSize(long long *size)
{
	const char **result = normalSizes;
	while ((*size)>(128*1024))
	{
		*size /= 1024;
		result++;
	}
	return *result;
}

void setupParameters(webs_t wp, const parameter_fetch_t *fetch, int transaction)
{
	if (transaction)
		nvram_init(RT2860_NVRAM);

	while (fetch->web_param != NULL)
	{
		// Get variable
		char_t *str = websGetVar(wp, (char_t *)fetch->web_param, (char_t *)fetch->dfl_param);
		if (fetch->is_switch) // Check if need update a switch
		{
			if (strcmp(str, "on") != 0)
				str = "off";
			if (fetch->is_switch == 2)
				str = (strcmp(str, "on") == 0) ? "1" : "0";
		}

		if (nvram_bufset(RT2860_NVRAM, (char_t *)fetch->nvram_param, (void *)str)!=0) //!!!
			syslog(LOG_ERR, "set %s nvram error, %s", fetch->nvram_param, __FUNCTION__);
#ifdef PRINT_DEBUG
		printf("%s value : %s\n", fetch->nvram_param, str);
#endif
		fetch++;
	}

	if (transaction)
	{
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
}

// String splitting tools
int initSplitter(string_split_t *buf)
{
	buf->buf = (char *)malloc(SPLITTER_BUFFER_QUANTITY);
	if (buf->buf == NULL)
		return errno;
	buf->items = (char **)malloc(SPLITTER_TOKEN_QUANTITY * sizeof(char *));
	if (buf->items == NULL)
	{
		free(buf->buf);
		buf->buf = NULL;
		return errno;
	}

	buf->buf_size = SPLITTER_BUFFER_QUANTITY;
	buf->pointers = SPLITTER_TOKEN_QUANTITY;
	buf->found    = 0;

	return 0;
}

int splitString(string_split_t *buf, const char *string, char splitter)
{
	// First reset
	buf->found = 0;
	if (string == NULL)
		return 0;

	// Calculate character buffer size
	size_t size = strlen(string) + 1;

	// Check if need to realloc buffer
	if (buf->buf_size < size)
	{
		free(buf->buf);
		size_t amount = size + (SPLITTER_BUFFER_QUANTITY - size%SPLITTER_BUFFER_QUANTITY);
		buf->buf = (char *)malloc(amount);
		if (buf->buf == NULL)
			return errno;
		buf->buf_size = amount;
	}

	// copy string
	memcpy(buf->buf, string, size);

	// calculate splitters
	buf->found = 1;
	char *p = buf->buf;
	while (*p != '\0')
	{
		if (*(p++) == splitter)
			buf->found++;
	}

	// Check if need to realloc pointers
	if (buf->pointers < buf->found)
	{
		free(buf->items);
		size_t amount = buf->found + (SPLITTER_TOKEN_QUANTITY - buf->found%SPLITTER_TOKEN_QUANTITY);
		buf->items = (char **)malloc(amount * sizeof(char *));
		if (buf->items == NULL)
			return errno;
		buf->pointers = amount;
	}

	// Make split
	p = buf->buf;
	char **it = buf->items;
	*(it++) = p; // Assign first token

	while (*p != '\0')
	{
		if (*p == splitter)
		{
			*(p++) = '\0';
			*(it++) = p;
		}
		else
			p++;
	}

	return 0;
}

int freeSplitter(string_split_t *buf)
{
	if (buf->buf != NULL)
	{
		free(buf->buf);
		buf->buf = NULL;
	}
	if (buf->items != NULL)
	{
		free(buf->items);
		buf->items = NULL;
	}
	return 0;
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
 * check the existence of semicolon in str
 */
int checkSemicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_get(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}

void reboot_now(void)
{
	doSystem("(sleep 2 && /etc/scripts/reboot.sh) > /dev/null 2>&1 &");
}

void arplookup(char *ip, char *arp)
{
    char buf[256];
    FILE *fp = fopen("/proc/net/arp", "r");
    if(!fp) {
	syslog(LOG_ERR, "no proc , %s", __FUNCTION__);
        return;
    }

    strcpy(arp, "00:00:00:00:00:00");
    while((fgets(buf, sizeof(buf), fp)) != NULL) {
        char ip_entry[32], hw_type[8],flags[8], hw_address[32];
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
	va_list	vargs;
	char_t	*cmd = NULL;
	int	rc = 0;

	va_start(vargs, fmt);
	if (fmtValloc(&cmd, WEBS_BUFSIZE, fmt, vargs) >= WEBS_BUFSIZE)
		syslog(LOG_ERR, "lost data, buffer overflow , %s", __FUNCTION__);

	va_end(vargs);

	if (cmd) {
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
	    while ((fgets(lan_if, sizeof(lan_if), fp)) != NULL) {
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
	    while ((fgets(wan_if, sizeof(wan_if), fp)) != NULL) {
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
int getIfIp(const char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
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
int getIfMac(const char *ifname, char *if_hw)
{
	struct ifreq ifr;
	char *ptr;
	int skfd;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if(ioctl(skfd, SIOCGIFHWADDR, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
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
int getIfNetmask(const char *ifname, char *if_net)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "open socket failed, %s", __FUNCTION__);
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ - 1);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	ifr.ifr_addr.sa_family = AF_INET;
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) < 0) {
		close(skfd);
		syslog(LOG_ERR, "ioctl call failed, %s", __FUNCTION__);
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
	    while ((fgets(ppp_if, sizeof(ppp_if), fp)) != NULL) {
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
char *getLanWanNamebyIf(const char *ifname)
{
	if(!strcmp(ifname, getLanIfName()))
		return "LAN";
	if(!strcmp(ifname, getWanIfName()))
		return "WAN";
	if (vpn_mode_enabled() == 1 && strstr(ifname, getPPPIfName()) != NULL)
		return "VPN";

	return "LAN";
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

/******************************************************************************/
/*
	Disable the Nagle algorithm for less latency in RPC
	http://www.faqs.org/rfcs/rfc896.html
	http://www.w3.org/Protocols/HTTP/Performance/Nagle/
*/
void setSocketNodelayReuse(int sock)
{
	int tmp = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&tmp, sizeof(tmp));
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&tmp, sizeof(tmp));
}
