/* vi: set sw=4 ts=4 sts=4 fdm=marker: */
/*
 *	internet.c -- Network Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: internet.c,v 1.167.2.8 2009-04-22 01:31:35 chhung Exp $
 */

#include	"helpers.h"
#include	"internet.h"
#include	"procps.h"
#include 	"firewall.h"
#include	"management.h"
#include	"station.h"

/*** VPN statuses ***/
typedef struct vpn_status_t
{
	const char_t *status;
	long          color;
} vpn_status_t;


const parameter_fetch_t vpn_args[] =
{
	{ T("vpn_server"),             "vpnServer",            0,       T("") },
	{ T("vpn_range"),              "vpnRange",             0,       T("") },
	{ T("vpn_user"),               "vpnUser",              0,       T("") },
	{ T("vpn_pass"),               "vpnPassword",          0,       T("") },
	{ T("vpn_mtu"),                "vpnMTU",               0,       T("") },
	{ T("vpn_type"),               "vpnType",              0,       T("") },
	{ T("vpn_mppe"),               "vpnMPPE",              1,       T("") },
	{ T("vpn_dgw"),                "vpnDGW",               0,       T("") },
	{ T("vpn_peerdns"),            "vpnPeerDNS",           1,       T("") },
	{ T("vpn_debug"),              "vpnDebug",             1,       T("") },
	{ T("vpn_nat"),                "vpnNAT",               1,       T("") },
	{ T("vpn_lcp"),                "vpnEnableLCP",         1,       T("") },
	{ T("vpn_auth_type"),          "vpnAuthProtocol",      0,       T("") },
	{ T("vpn_pppoe_iface"),        "vpnInterface",         0,       T("") },
	{ T("vpn_pppoe_service"),      "vpnService",           0,       T("") },
	{ T("vpn_pure_pppoe"),         "vpnPurePPPOE",         2,       T("") },
	{ T("vpn_lcp_errors"),         "vpnLCPFailure",        0,       T("") },
	{ T("vpn_lcp_interval"),       "vpnLCPInterval",       0,       T("") },
	{ T("vpn_test_reachable"),     "vpnTestReachable",     2,       T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

const parameter_fetch_t lanauth_args[] =
{
	{ T("vpn_type"),               "vpnType",              0,       T("") },
	{ T("vpn_pass"),               "vpnPassword",          0,       T("") },
	{ T("lanauth_access"),         "LANAUTH_LVL",          0,       T("") },

	{ NULL, NULL, 0, NULL } // Terminator
};

/*
 * VPN statuses
 */
const vpn_status_t vpn_statuses[] =
{

	{ "disabled",     0x808080        },
	{ "offline",      0xff0000        },
	{ "connecting",   0xff8000        },
	{ "online",       0x00ff00        }
};

#ifdef CONFIG_USER_KABINET
/*
 * LANAUTH status
 */
const vpn_status_t lanauth_statuses[] =
{

	{ "disabled",           0x808080        },
	{ "not started",        0x808080        },
	{ "offline",            0xff0000        },
	{ "kabinet networks",   0x33bb33        },
	{ "full access",        0x00ff00        }
};
#endif


#ifdef CONFIG_USER_KABINET
/* returns actual lanauth state+1 or 0 if lanauth process not found
*/
static int get_LANAUTHState()
{
	FILE *fp;
	char	result[96],*r;
	int i,state;

	fp = popen("ps|grep lanaut[h]", "r");
	if(!fp){
		syslog(LOG_ERR, "no ps util exist, %s", __FUNCTION__);
		return -1;
	}
	fgets(result, sizeof(result), fp);
	pclose(fp);
	for (i=0, state=0, r=result; *r && i<sizeof(result) ;i++,r++)
	{
		switch(state)
		{
			case 0:
				if (*r == '-')
					state++;
				break;
			case 1:
				if (*r == 'A')
					state++;
				else
					state = 0;
				break;
			case 2:
				if (*r == ' ' || *r == '\t')
					state = 3;
				else
					state = 0;
				break;
			case 3:
				if (*r == '0')
					return 1;
				else if (*r == '1')
					return 2;
				else if (*r == '2')
					return 3;
				else
					state = 0;
			default:
				break;
		}
	}
	return 0;
}
#endif

/*
 * Show PPTP VPN status
 */
static int vpnShowVPNStatus(int eid, webs_t wp, int argc, char_t **argv)
{
	int status = 0; // Status is 'disabled'
	const vpn_status_t *st_table = vpn_statuses;

	// Get value
	char *vpn_enabled = nvram_get(RT2860_NVRAM, "vpnEnabled");
	char *vpn_type = nvram_get(RT2860_NVRAM, "vpnType");
	if ((vpn_enabled==NULL) || (vpn_enabled[0]=='\0'))
		vpn_enabled = "off";

	// Do not perform other checks if VPN is turned off
	if (strcmp(vpn_enabled, "on")==0) {
#ifdef CONFIG_USER_KABINET
		if (CHK_IF_DIGIT(vpn_type, 3)) {
			status = (get_LANAUTHState() + 1) % 5;
			st_table = lanauth_statuses;
		} else {
#endif
			// Status is at least 'offline' now
			status++;

			// Try to find pppd or xl2tpd
			int found = procps_count("pppd");
			if (found==0)
				found = procps_count("xl2tpd");

			if (found>0) {
				// Now status is at least 'connecting'
				status++;

				// Try to search for 'pppXX' device
				FILE * fd = fopen(_PATH_PROCNET_DEV, "r");

				if (fd != NULL) {
					int ppp_id;
					char_t line[256];

					// Read all ifaces and check match
					while ((fgets(line, sizeof(line), fd)) != NULL) {
						// Filter only 'pppXX'
						if (sscanf(line, " ppp%d", &ppp_id)==1) {
							// Check if ppp interface has number at least 8
							if ((ppp_id >= 0) && (ppp_id <= 8)) {
								status++; // Status is set to 'connected'
								break; // Do not search more
							}
						}
					}

					fclose(fd);
				} else {
					syslog(LOG_WARNING, "cannot open %s (%s).\n", _PATH_PROCNET_DEV, strerror(errno));
				}
			} else if (found<0) {
				//syslog(LOG_WARNING, "cannot serach process 'pppd' or 'xl2tpd': %s\n", strerror(-found));
				;
			}
#ifdef CONFIG_USER_KABINET
		}
#endif
	}

	// Output connection status
	const vpn_status_t *st = &st_table[status];
	websWrite(
		wp,
		T("<b><font color=\"#%06x\">%s</font></b>\n"),
		st->color, st->status
	);

	return 0;
}

const char *vpn_ifaces[] =
{
	"LAN",
	"WAN",
	NULL // Terminator
};

static int vpnIfaceList(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t iface[32];
	const char **line;

	// Fetch VPN interface
	char *rrs  = nvram_get(RT2860_NVRAM, "vpnInterface");
	if (rrs!=NULL)
		strcpy(iface, rrs);
	else
		iface[0] = '\0';
	if (strlen(iface)<=0)
		strcpy(iface, "LAN");

	// Read all ifaces and check match
	for (line = vpn_ifaces; *line != NULL; line++)
	{
		// Write iface to output if it was found
		websWrite(wp, T("<option value=\"%s\"%s>%s</option>\n"),
			*line,
			(strcmp(*line, iface)==0) ? " selected=\"selected\"" : "",
			*line
		);
	}

	return 0;
}

static void formVPNSetup(webs_t wp, char_t *path, char_t *query)
{
	char_t *vpn_enabled, *submitUrl, *vpn_type;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 20, "vpnEnabled", "vpnType", "vpnServer", "vpnMPPE", "vpnPeerDNS", "vpnDebug",
			"vpnNAT", "vpnDGW", "vpnAuthProtocol", "vpnEnableLCP", "vpnPurePPPOE", "vpnLCPFailure", "vpnLCPInterval",
			"vpnTestReachable", "LANAUTH_LVL", "vpnMTU", "vpnUser", "vpnPassword", "vpnInterface", "vpnService");
		goto out;
	}

	vpn_enabled = websGetVar(wp, T("vpn_enabled"), T(""));
	vpn_type = websGetVar(wp, T("vpn_type"), T("0"));

	if (vpn_enabled[0] == '\0')
		vpn_enabled="off";

	nvram_init(RT2860_NVRAM);

	if (nvram_bufset(RT2860_NVRAM, "vpnEnabled", (void *)vpn_enabled)!=0)
	{
		syslog(LOG_ERR, "Set vpnEnabled error, %s", __FUNCTION__);
		return;
	}

	if ((strncmp(vpn_enabled, "on", 3)) || !CHK_IF_DIGIT(vpn_type, 0))
		nvram_bufset(RT2860_NVRAM, "vpnPurePPPOE", "0");

	if (strncmp(vpn_enabled, "on", 3))
		goto out_with_commit;

	const parameter_fetch_t *fetch = vpn_args;

#ifdef CONFIG_USER_KABINET
	if (CHK_IF_DIGIT(vpn_type, 3))
		fetch = lanauth_args;
#endif
	setupParameters(wp, fetch, 0);

out_with_commit:
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

out:
	//kill helpers firt sigterm second sigkill
	doSystem("killall -q W60vpnhelper");
	doSystem("killall -q vpnhelper");
	doSystem("killall -q -SIGKILL W60vpnhelper");
	doSystem("killall -q -SIGKILL vpnhelper");
	doSystem("service vpnhelper restart > /dev/console 2>&1");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

/*
 * arguments: type - 1 = write Primary DNS
 *                   2 = write Secondary DNS
 * description: write DNS ip address accordingly
 */
static int getDns(int eid, webs_t wp, int argc, char_t **argv)
{
	FILE *fp;
	char buf[80] = {0}, ns_str[11], dns[16] = {0};
	int type, idx = 0, req = 0;

	if (ejArgs(argc, argv, T("%d"), &type) == 1) {
		if (1 == type)
			req = 1;
		else if (2 == type)
			req = 2;
		else
			return websWrite(wp, T(""));
	}

	fp = fopen("/etc/resolv.conf", "r");
	if (NULL == fp)
		return websWrite(wp, T(""));
	while ((fgets(buf, sizeof(buf), fp)) != NULL)
	{
		if (sscanf(buf, "%s %s", ns_str, dns) != 2)
			continue;
		if (strcasecmp(ns_str, "nameserver") != 0)
			continue;

		idx++;
		if (idx == req) {
			websWrite(wp, T("%s"), dns);
			break;
		}
	}
	fclose(fp);
	return 0;
}

/*
 * arguments:
 * description: return 1 if hostname is supported
 */
static int getHostSupp(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef GA_HOSTNAME_SUPPORT
	ejSetResult(eid, "1");
#else
	ejSetResult(eid, "0");
#endif
	return 0;
}

static int getIgmpProxyBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_IGMP_PROXY
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getVPNBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_NF_CONNTRACK_PPTP) || defined(CONFIG_NF_CONNTRACK_PPTP_MODULE) || \
    defined(CONFIG_IP_NF_PPTP) || defined(CONFIG_IP_NF_PPTP_MODULE)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getWDSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_WDS) || defined(CONFIG_MT7610_AP_WDS) || defined(CONFIG_MT76X2_AP_WDS)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getMBSSIDBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_RT2860V2_AP_MBSS) || defined(CONFIG_MT7610_AP_MBSS) || defined(CONFIG_MT76X2_AP_MBSS)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USB
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv6Built(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_IPV6
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIPv66rdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_IPV6_SIT_6RD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getIP6to4Built(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined (CONFIG_IPV6_SIT) ||  defined (CONFIG_IPV6_SIT_MODULE)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getStorageBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_STORAGE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getFtpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_FTPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSmbBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_USER_SAMBA)
       return websWrite(wp, T("1"));
#else
       return websWrite(wp, T("0"));
#endif
}

static int getPrinterSrvBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_P910ND
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUSBModemBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USB_MODESWITCH
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getTransmissionBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_TRANSMISSION
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDynamicRoutingBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_ZEBRA
    return websWrite(wp, T("1"));
#else
    return websWrite(wp, T("0"));
#endif
}

static int getSWQoSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_RALINKAPP_SWQOS
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDATEBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_DATE
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDDNSBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_INADYN
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLANAUTHBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_KABINET
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSysLogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_SYSLOGD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getETHTOOLBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_ETHTOOL
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write LAN ip address accordingly
 */
static int getLanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (getIfIp(getLanIfName(), if_addr) == -1) {
		//websError(wp, 500, T("getLanIp: calling getIfIp error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write LAN MAC address accordingly
 */
static int getLanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (getIfMac(getLanIfName(), if_mac) == -1) {
		//websError(wp, 500, T("getLanIp: calling getIfMac error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * description: write LAN subnet mask accordingly
 */
static int getLanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (getIfNetmask(getLanIfName(), if_net) == -1) {
		//websError(wp, 500, T("getLanNetmask: calling getIfNetmask error\n"));
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

static int getGWBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, T("1"));
}

static int getDnsmasqBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DNSMASQ
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getCdpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_CDP
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}


static int getLltdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_LLTD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getLldpdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_LLDPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getUpnpBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_MINIUPNPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getXupnpdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_XUPNPD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getSpotBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getRadvdBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_RADVD
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getDhcpv6Built(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_DHCP6_SERVER
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

/*
 * description: write real_wan ip address accordingly
 */
static int getIntIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (vpn_mode_enabled() == 1) {
	    /* if vpn enabled get ip from vpnif else from wan if */
	    if (getIfIp(getPPPIfName(), if_addr) == -1) {
		if (getIfIp(getWanIfName(), if_addr) == -1)
		    return websWrite(wp, T(""));
	    }
	} else {
	    /* if vpn disabled always get ip from wanif */
	    if (getIfIp(getWanIfName(), if_addr) == -1) {
		return websWrite(wp, T(""));
	    }
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write WAN ip address accordingly
 */
static int getWanIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_addr[16];

	if (getIfIp(getWanIfName(), if_addr) == -1) {
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_addr);
}

/*
 * description: write WAN MAC address accordingly
 */
static int getWanMac(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_mac[18];

	if (getIfMac(getWanIfName(), if_mac) == -1) {
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_mac);
}

/*
 * description: write WAN subnet mask accordingly
 */
static int getWanNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char if_net[16];

	if (getIfNetmask(getWanIfName(), if_net) == -1) {
		return websWrite(wp, T(""));
	}
	return websWrite(wp, T("%s"), if_net);
}

/*
 * description: write WAN default gateway accordingly
 */
static int getWanGateway(int eid, webs_t wp, int argc, char_t **argv)
{
	char   buff[256];
	char   ifname[16];
	int    nl = 0 ;
	struct in_addr dest;
	struct in_addr gw;
	int    flgs, ref, use, metric;
	unsigned long int d,g,m;
	int    find_default_flag = 0;

	char sgw[16];

	FILE *fp = fopen("/proc/net/route", "r");

	while ((fgets(buff, sizeof(buff), fp)) !=NULL) {
		if (nl) {
			int ifl = 0;
			while (buff[ifl]!=' ' && buff[ifl]!='\t' && buff[ifl]!='\0') {
				ifname[ifl] = buff[ifl];
				ifl++;
			}
			ifname[ifl] = 0;
			buff[ifl] = 0;    /* interface */
			if (sscanf(buff+ifl+1, "%lx%lx%X%d%d%d%lx",
						&d, &g, &flgs, &ref, &use, &metric, &m)!=7) {
				fclose(fp);
				return websWrite(wp, T("format error"));
			}

			if (flgs&RTF_UP) {
				dest.s_addr = d;
				gw.s_addr   = g;
				strcpy(sgw, (gw.s_addr==0 ? ifname : inet_ntoa(gw)));

				if (dest.s_addr == 0) {
					find_default_flag = 1;
					break;
				}
			}
		}
		nl++;
	}
	fclose(fp);

	if (find_default_flag == 1)
		return websWrite(wp, T("%s"), sgw);
	else
		return websWrite(wp, T(""));
}

/*
 *
 */

static int findRoutingRule(char *rrs, char *buf, const char *dest, const char *netmask, const char* gw, const char *iface)
{
	char c_dest[32], c_netmask[32], c_iface[32], c_gw[32];
	int index = 0;

	while (getNthValueSafe(index++, rrs, ';', buf, 256) != -1)
	{
		if ((getNthValueSafe(0, buf, ',', c_dest, sizeof(c_dest)) == -1))
			continue;
		if ((getNthValueSafe(1, buf, ',', c_netmask, sizeof(c_netmask)) == -1))
			continue;
		if ((getNthValueSafe(2, buf, ',', c_gw, sizeof(c_gw)) == -1))
			continue;
		if ((getNthValueSafe(4, buf, ',', c_iface, sizeof(c_iface)) == -1))
			continue;

		if ((strcmp(dest, c_dest)==0) && (strcmp(netmask, c_netmask)==0) &&
			(strcmp(iface, c_iface)==0) && (strcmp(gw, c_gw)==0))
			return index-1;
	}

	return -1;
}

static void removeRoutingRuleNvram(const char *iface, const char *dest, const char *netmask, const char *gw)
{
	char rule[256];
	char c_iface[32], c_dest[32], c_netmask[32], c_gw[32];
	char *old = nvram_get(RT2860_NVRAM, "RoutingRules");

	// Check that NVRAM contains variable
	if (old == NULL)
	    return;

	// Create temporary buffer
	char *buf = (char *)calloc(1, strlen(old) + sizeof(char)*2);
	if (buf == NULL)
	    return;

	char *p = buf;
	buf[0] = '\0';

	char *head = old;
	char *tail = strchr(head, ';');
	if (tail == NULL)
		tail = head + strlen(head);

	while (tail != head)
	{
		do
		{
			// Create record
			int count = tail-head;
			memcpy(rule, head, count);
			rule[count] = '\0';

			// Get destination
			if ((getNthValueSafe(0, rule, ',', c_dest, sizeof(c_dest)) == -1))
				continue;
			// Get netmask
			if ((getNthValueSafe(1, rule, ',', c_netmask, sizeof(c_netmask)) == -1))
				continue;
			// Get gateway
			if ((getNthValueSafe(2, rule, ',', c_gw, sizeof(c_gw)) == -1))
				continue;
			// Get interface
			if ((getNthValueSafe(3, rule, ',', c_iface, sizeof(c_iface)) == -1))
				continue;

			// Check if rule not matches
			if (!((strcmp(iface, c_iface)==0) && (strcmp(dest, c_dest)==0) &&
			    (strcmp(netmask, c_netmask)==0) && (strcmp(gw, c_gw)==0))) {
				// Store rule
				if (strlen(buf) > 0)
				    *(p++) = ';';

				strcpy(p, rule);
				p += strlen(rule);
			}
			// else skip adding rule
		} while (0);

		// Move pointer to next entry
		head = tail;
		if (head[0] == ';')
			head++;
		tail = strchr(head, ';');
		if (tail == NULL)
			tail = head + strlen(head);
	}
	// Write new buffer to NVRAM
	nvram_set(RT2860_NVRAM, "RoutingRules", buf);
	free(buf);
}

static int addRoutingRule(char *buf, const char *dest, const char *netmask, const char *gw, const char *true_if)
{
	char cmd[256];

	// action, destination
	sprintf(cmd, "ip route $1 %s", dest);

	// netmask
	if (strcmp(netmask, "255.255.255.255")!=0)
		sprintf(cmd, "%s/%s", cmd, netmask);

	// interface
	sprintf(cmd, "%s dev %s", cmd, true_if);

	// gateway
	if (strlen(gw) && strcmp(gw, "0.0.0.0") != 0)
		sprintf(cmd, "%s via %s", cmd, gw);

	if (buf != NULL)
	{
		strcpy(buf, cmd);
		return 1;
	}
	else
	{
		FILE *fp;
		strcat(cmd, " 2>&1 ");
		printf("%s\n", cmd);

		fp = popen(cmd, "r");
		if (!fp)
			return 0;

		fgets(cmd, sizeof(cmd), fp);

		pclose(fp);
		return strlen(cmd) == 0;
	}
}

static void addRoutingRuleNvram(
	const char *iface, const char *dest, const char *netmask,
	const char *gw, const char *true_if, const char *cust_if, const char *comment)
{
	char cmd[256];

	char *rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		rrs = "";

	// Create new item
	sprintf(cmd, "%s,%s,%s,%s,%s,%s,%s", dest, netmask, gw, iface, true_if, cust_if, comment);

	int len = strlen(rrs);
	char *tmp = (char *)calloc(1, len + strlen(cmd) + sizeof(char)*2);
	char *p = tmp;
	if (tmp == NULL)
		return;

	// Write previous rules if exist
	if (len > 0)
	{
		strcpy(tmp, rrs);
		p += len;
		*(p++) = ';';
	}

	strcpy(p, cmd);

	// Set NVRAM variable
	nvram_set(RT2860_NVRAM, "RoutingRules", tmp);

	// Clear resource
	free(tmp);
}

static void rebuildVPNRoutes(char *src_rrs)
{
	// Check if we are not in bridge mode
	char *op_mode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(op_mode, "0") == 0)
		return;

	// Now generate file
	int nwritten=0, index=0;
	char one_rule[256], iface[32], dest[32], netmask[32], gw[32];

	char *rrs = (src_rrs != NULL) ? src_rrs : nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		return;

	FILE *fd = fopen(_PATH_PPP_ROUTES, "w");
	if (fd == NULL)
	{
		syslog(LOG_ERR, "open %s failed, %s", _PATH_PPP_ROUTES, __FUNCTION__);
		return;
	}

	fputs("#/bin/sh\n\n", fd);

	while (getNthValueSafe(index++, rrs, ';', one_rule, sizeof(one_rule)) != -1)
	{
		// Get & check interface
		if (getNthValueSafe(3, one_rule, ',', iface, sizeof(iface)) == -1)
			continue;
		if (strcmp(iface, "VPN") != 0)
			continue;

		// Get destination
		if (getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)
			continue;

		// Get netmask
		if (getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)
			continue;

		// Get gateway
		if (getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)
			continue;

		addRoutingRule(one_rule, dest, netmask, gw, "$2");
		fprintf(fd, "%s\n", one_rule);
		nwritten++;
	}

	fclose(fd);

	if (nwritten <= 0)
		unlink(_PATH_PPP_ROUTES);
	else
		chmod(_PATH_PPP_ROUTES, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
}

static void rebuildLANWANRoutes(char *src_rrs)
{
	// Get work mode
	char *op_mode = nvram_get(RT2860_NVRAM, "OperationMode");
	int isBridgeMode = (strcmp(op_mode, "0") == 0) ? 1 : 0;

	// Now generate file
	int nwritten=0, index=0;
	char one_rule[256], iface[32], dest[32], netmask[32], gw[32], custom_iface[32];

	char *rrs = (src_rrs != NULL) ? src_rrs : nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		return;

	FILE *fd = fopen(_PATH_LANWAN_ROUTES, "w");
	if (fd == NULL)
	{
		syslog(LOG_ERR, "open %s failed, %s", _PATH_LANWAN_ROUTES, __FUNCTION__);
		return;
	}

	fputs("#/bin/sh\n\n", fd);

	while (getNthValueSafe(index++, rrs, ';', one_rule, sizeof(one_rule)) != -1)
	{
		// Get & check interface
		if (getNthValueSafe(3, one_rule, ',', iface, sizeof(iface)) == -1)
			continue;
		if (strcmp(iface, "VPN") == 0)
			continue;
		else if ((strcmp(iface, "WAN")==0) && isBridgeMode)
		{
			syslog(LOG_ERR, "Skip WAN routing rule in the non-Gateway mode: %s,%s", one_rule, __FUNCTION__);
			continue;
		}

		// Get destination
		if (getNthValueSafe(0, one_rule, ',', dest, sizeof(dest)) == -1)
			continue;

		// Get netmask
		if (getNthValueSafe(1, one_rule, ',', netmask, sizeof(netmask)) == -1)
			continue;

		// Get gateway
		if (getNthValueSafe(2, one_rule, ',', gw, sizeof(gw)) == -1)
			continue;

		if ((getNthValueSafe(5, one_rule, ',', custom_iface, sizeof(custom_iface)) == -1))
			continue;

		// Check interface
		if (strcmp(iface, "WAN")==0)
			strcpy(custom_iface, "$3");
		else if (strcmp(iface, "LAN")==0)
			strcpy(custom_iface, "$2");
		else if (strcmp(iface, "Custom")==0)
		{
			if (strlen(custom_iface)<=0)
				strcpy(custom_iface, "$3");
		}
		else
		{
			strcpy(iface, "LAN");
			strcpy(custom_iface, "$2");
		}

		addRoutingRule(one_rule, dest, netmask, gw, custom_iface);
		fprintf(fd, "%s\n", one_rule);
		nwritten++;
	}

	fclose(fd);

	if (nwritten <= 0)
		unlink(_PATH_LANWAN_ROUTES);
	else
		chmod(_PATH_LANWAN_ROUTES, S_IXGRP | S_IXUSR | S_IRUSR | S_IWUSR | S_IRGRP);
}

void staticRoutingInit(void)
{
	// Get routing rules
	char *rrs = nvram_get(RT2860_NVRAM, "RoutingRules");

	// Remove static routes scripts
	doSystem("rm -f " _PATH_PPP_ROUTES);
	doSystem("rm -f " _PATH_LANWAN_ROUTES);

	// And rebuild VPN routes
	rebuildLANWANRoutes(rrs);
	rebuildVPNRoutes(rrs);
}

/*
 * description: get routing table
 */
static int getRoutingTable(int eid, webs_t wp, int argc, char_t **argv)
{
	char   buff[512];
	int    nl = 0, index;
	char   ifname[32], interface[128];
	struct in_addr dest, gw, netmask;
	char   dest_str[32], gw_str[32], netmask_str[32], comment[32];
	int    flgs, ref, use, metric;
	int   *running_rules = NULL;
	char   one_rule[256];
	unsigned long int d,g,m;
	char *rrs;
	int  rule_count;
	FILE *fp = fopen("/proc/net/route", "r");
	if (!fp)
		return -1;

	// Determine work mode
	char *op_mode = nvram_get(RT2860_NVRAM, "OperationMode");
	int isBridgeMode = (strcmp(op_mode, "0") == 0) ? 1 : 0;

	// Get routing table
	rrs = nvram_get(RT2860_NVRAM, "RoutingRules");
	if (rrs == NULL)
		rrs = "";
	rule_count = getNums(rrs, ';');

	if (rule_count)
	{
		running_rules = calloc(1, sizeof(int) * rule_count);
		if (!running_rules)
		{
			fclose(fp);
			return -1;
		}
	}

	// true_interface[0], destination[1], gateway[2], netmask[3], flags[4], ref[5], use[6],
	// metric[7], category[8], interface[9], idle[10], comment[11], new[12]
	while ((fgets(buff, sizeof(buff), fp)) != NULL)
	{
		if (nl > 0)
		{
			if (sscanf(buff, "%s%lx%lx%X%d%d%d%lx", ifname, &d, &g, &flgs, &ref, &use, &metric, &m) != 8)
			{
				syslog(LOG_ERR, "format error, %s", __FUNCTION__);
				free(running_rules);
				fclose(fp);
				return 0;
			}
			dest.s_addr = d;
			gw.s_addr = g;
			netmask.s_addr = m;

			if (!(flgs & 0x1))	// skip not usable
				continue;

			strncpy(dest_str, inet_ntoa(dest), sizeof(dest_str));
			strncpy(gw_str, inet_ntoa(gw), sizeof(gw_str));
			strncpy(netmask_str, inet_ntoa(netmask), sizeof(netmask_str));

			if (nl > 1)
				websWrite(wp, T(",\n"));
			websWrite(wp, T("[ '%s', '%s', '%s', '%s', %d, %d, %d, %d"), ifname, dest_str, gw_str, netmask_str, flgs, ref, use, metric); // 0-7

			// check if internal routing rules
			strcpy(comment, "");
			if ((index=findRoutingRule(rrs, one_rule, dest_str, netmask_str, gw_str, ifname)) != -1)
			{
				if (index < rule_count)
					running_rules[index] = 1;
				else
					syslog(LOG_ERR, "fatal error in %s", __FUNCTION__);

				websWrite(wp, T(", %d, "), index); // 8
				if (getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) != -1)
					websWrite(wp, T("'%s'"), interface); // 9
				else
					websWrite(wp, T("'LAN'")); // 9
				if (getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)
					comment[0] = '\0';
			}
			else
				websWrite(wp, T(", -1, '%s'"), getLanWanNamebyIf(ifname)); // 8-9

			websWrite(wp, T(", 0, '%s', 0 ]"), comment); // 10-12
		}
		nl++;
	}

	for (index=0; index < rule_count; index++)
	{
		if (running_rules[index])
			continue;

		if(getNthValueSafe(index, rrs, ';', one_rule, sizeof(one_rule)) == -1)
			continue;

		if(getNthValueSafe(0, one_rule, ',', dest_str, sizeof(dest_str)) == -1)
			continue;

		if(getNthValueSafe(1, one_rule, ',', netmask_str, sizeof(netmask_str)) == -1)
			continue;

		if(getNthValueSafe(2, one_rule, ',', gw_str, sizeof(gw_str)) == -1)
			continue;

		if(getNthValueSafe(3, one_rule, ',', interface, sizeof(interface)) == -1)
			continue;

		if(getNthValueSafe(4, one_rule, ',', ifname, sizeof(ifname)) == -1)
			continue;

		if(getNthValueSafe(6, one_rule, ',', comment, sizeof(comment)) == -1)
			continue;

		if (nl > 0)
			websWrite(wp, T(",\n"));

		websWrite(wp, T("[ '%s', '%s', '%s', '%s', 0, 0, 0, 0, %d, '%s', %d, '%s', 0 ]"),
			ifname, dest_str, gw_str, netmask_str, index, interface,
			((strcmp(interface, "VPN")==0) ? isBridgeMode : 1),
			comment);
		nl++;
	}

	fclose(fp);

	if (running_rules)
		free(running_rules);
	return 0;
}

static void editRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *trans = websGetVar(wp, T("routingTableDiff"), T(""));
	char rec[256];
	char true_iface[32], destination[32], gateway[32], netmask[32], iface[32], c_iface[32], comment[64], action[4];
	int i = 0, iaction;
	char_t *submitUrl;

	websHeader(wp);
	websWrite(wp, T("<h3>Edit routing table:</h3><br>\n"));

	while (getNthValueSafe(i++, trans, ';', rec, sizeof(rec)) != -1)
	{
		// Get true interface
		if ((getNthValueSafe(0, rec, ',', c_iface, sizeof(c_iface)) == -1))
			continue;
		// Get destination
		if ((getNthValueSafe(1, rec, ',', destination, sizeof(destination)) == -1))
			continue;
		// Get gateway
		if ((getNthValueSafe(2, rec, ',', gateway, sizeof(gateway)) == -1))
			continue;
		// Get netmask
		if ((getNthValueSafe(3, rec, ',', netmask, sizeof(netmask)) == -1))
			continue;
		// Get interface
		if ((getNthValueSafe(9, rec, ',', iface, sizeof(iface)) == -1))
			continue;
		// Get comment
		if ((getNthValueSafe(11, rec, ',', comment, sizeof(comment)) == -1))
			continue;
		// Get action
		if ((getNthValueSafe(12, rec, ',', action, sizeof(action)) == -1))
			continue;

		// Check action
		iaction = atoi(action);
		if (! ((iaction == 1) || (iaction == 2)))
			continue;

		// Check interface
		strcpy(true_iface, c_iface);
		if (strcmp(iface, "WAN")==0)
			strcpy(true_iface, getWanIfName());
		else if (strcmp(iface, "LAN")==0)
			strcpy(true_iface, getLanIfName());
		else if (strcmp(iface, "Custom")==0)
		{
			if (strlen(true_iface)<=0)
				strcpy(true_iface, getWanIfName());
		}
		else if (strcmp(iface, "VPN")==0)
		{
			strcpy(true_iface, "ppp+");
		}
		else
		{
			strcpy(iface, "LAN");
			strcpy(true_iface, getLanIfName());
		}

		if (iaction == 1) // Add route
		{
			addRoutingRuleNvram(iface, destination, netmask, gateway, true_iface, c_iface, comment);
			websWrite(wp, T("Add route: %s, %s, %s, %s, %s<br>\n"), iface, destination, netmask, gateway, true_iface);
		}
		else if (iaction == 2) // Remove route
		{
			removeRoutingRuleNvram(iface, destination, netmask, gateway);
			websWrite(wp, T("Delete route: %s, %s, %s, %s<br>\n"), iface, destination, netmask, gateway);
		}
	}

	staticRoutingInit();

	// run script to rebuild routing
	//configure system
	doSystem("internet.sh");

	// Write OK
	websWrite(wp, T("<script language=\"JavaScript\" type=\"text/javascript\">ajaxReloadDelayedPage(10000, '/internet/routing.asp', true);</script>\n"));
	websFooter(wp);

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

#ifdef CONFIG_USER_ZEBRA
static void dynamicRouting(webs_t wp, char_t *path, char_t *query)
{
	char_t *rip;
	char_t *submitUrl;
	char *RIPEnable;

	rip = websGetVar(wp, T("RIPSelect"), T(""));
	if(!rip || !strlen(rip))
		return;

	RIPEnable = nvram_get(RT2860_NVRAM, "RIPEnable");
	if(!RIPEnable || !strlen(RIPEnable))
		RIPEnable = "0";

	if(!gstrcmp(rip, "0") && !strcmp(RIPEnable, "1"))
		nvram_set(RT2860_NVRAM, "RIPEnable", rip);
	else if(!gstrcmp(rip, "1") && !strcmp(RIPEnable, "0"))
		nvram_set(RT2860_NVRAM, "RIPEnable", rip);

	doSystem("service dynroute restart");

#ifdef PRINT_DEBUG
	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>Dynamic Routing:</h3><br>\n"));
	websWrite(wp, T("RIPEnable %s<br>\n"), rip);
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}
#endif

/*
 * description: setup internet according to nvram configurations
 *              (assume that nvram_init has already been called)
 *              return value: 0 = successful, -1 = failed
 */
void initInternet(void)
{
#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	char *opmode;
#endif
	//first generate user routes and fierwall scripts
	staticRoutingInit();
	firewall_rebuild_etc();

	//configure system
	doSystem("internet.sh");

#if defined(CONFIG_RT2860V2_STA) || defined(CONFIG_RT2860V2_STA_MODULE) || defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	//automatically connect to AP according to the active profile
	opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (!strcmp(opmode, "2")) {
	    initStaProfile();
	    initStaConnection();
	}
#endif
}

static void getMyMAC(webs_t wp, char_t *path, char_t *query)
{
	char myMAC[32];

	arplookup(wp->ipaddr, myMAC);
	websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\n"));
	websWrite(wp, WEBS_CACHE_CONTROL_STRING);
	websWrite(wp, T("\n"));
	websWrite(wp, T("%s"), myMAC);
	websDone(wp, 200);
}

/* goform/setLan */
static void setLan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ip, *nm, *start_ip, *end_ip, *dgw;
	char_t	*gw = NULL, *pd = NULL, *sd = NULL;
	char_t *lan2enabled, *lan2_ip, *lan2_nm;
	char_t *submitUrl;
#ifdef GA_HOSTNAME_SUPPORT
	char_t	*host;
#endif
	char	*opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	char	*wan_ip = nvram_get(RT2860_NVRAM, "wan_ipaddr");
	char	*ctype = nvram_get(RT2860_NVRAM, "connectionType");
	char    *dhcpEnabled = nvram_get(RT2860_NVRAM, "dhcpEnabled");
	char_t  *dhcpGateway = nvram_get(RT2860_NVRAM, "dhcpGateway");
	char_t	*old_start_ip = nvram_get(RT2860_NVRAM, "dhcpStart");
	char_t	*old_end_ip = nvram_get(RT2860_NVRAM, "dhcpEnd");
	char_t	*old_mask = nvram_get(RT2860_NVRAM, "dhcpMask");

	ip = websGetVar(wp, T("lanIp"), T(""));
	nm = websGetVar(wp, T("lanNetmask"), T(""));
	lan2enabled = websGetVar(wp, T("lan2enabled"), T(""));
	lan2_ip = websGetVar(wp, T("lan2Ip"), T(""));
	lan2_nm = websGetVar(wp, T("lan2Netmask"), T(""));
	start_ip = websGetVar(wp, T("dhcpStart"), T(""));
	end_ip = websGetVar(wp, T("dhcpEnd"), T(""));
	dgw = websGetVar(wp, T("dhcpGateway"), T(""));
#ifdef GA_HOSTNAME_SUPPORT
	host = websGetVar(wp, T("hostname"), T("0"));
#endif
	/*
	 * check static ip address:
	 * lan and wan ip should not be the same except in bridge mode
	 */
	if (strncmp(ctype, "STATIC", 7))
	{
		if (strcmp(opmode, "0") && !strncmp(ip, wan_ip, 15)) {
			websError(wp, 200, "IP address is identical to WAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			if (strcmp(opmode, "0") && !strncmp(lan2_ip, wan_ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to WAN");
				return;
			}
			else if (strcmp(opmode, "0") && !strncmp(lan2_ip, ip, 15)) {
				websError(wp, 200, "LAN2 IP address is identical to LAN1");
				return;
			}
		}
	}

	nvram_init(RT2860_NVRAM);

	// configure gateway and dns (WAN) at bridge mode
	if (!strncmp(opmode, "0", 2))
	{
		gw = websGetVar(wp, T("lanGateway"), T(""));
		pd = websGetVar(wp, T("lanPriDns"), T(""));
		sd = websGetVar(wp, T("lanSecDns"), T(""));
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
		nvram_bufset(RT2860_NVRAM, "wan_static_dns", "on");
	}

	nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
	nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
	nvram_bufset(RT2860_NVRAM, "Lan2Enabled", lan2enabled);
	nvram_bufset(RT2860_NVRAM, "lan2_ipaddr", lan2_ip);
	nvram_bufset(RT2860_NVRAM, "lan2_netmask", lan2_nm);
#ifdef GA_HOSTNAME_SUPPORT
	nvram_bufset(RT2860_NVRAM, "HostName", host);
#endif
	if (CHK_IF_DIGIT(dhcpEnabled, 1)) {
		if (strncmp(old_start_ip, start_ip, 15))
			nvram_bufset(RT2860_NVRAM, "dhcpStart", start_ip);
		if (strncmp(old_end_ip, end_ip, 15))
			nvram_bufset(RT2860_NVRAM, "dhcpEnd", end_ip);
		if (strncmp(old_mask, nm, 15))
			nvram_bufset(RT2860_NVRAM, "dhcpMask", nm);
		if (strncmp(dhcpGateway, dgw, 15))
			nvram_bufset(RT2860_NVRAM, "dhcpGateway", dgw);
	}
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>LAN Interface Setup</h3><br>\n"));
#ifdef GA_HOSTNAME_SUPPORT
	websWrite(wp, T("Hostname: %s<br>\n"), host);
#endif
	websWrite(wp, T("IP: %s<br>\n"), ip);
	websWrite(wp, T("Netmask: %s<br>\n"), nm);
	websWrite(wp, T("LAN2 Enabled: %s<br>\n"), lan2enabled);
	websWrite(wp, T("LAN2 IP: %s<br>\n"), lan2_ip);
	websWrite(wp, T("LAN2 Netmask: %s<br>\n"), lan2_nm);
	if (!strncmp(opmode, "0", 2))
	{
		websWrite(wp, T("Gateway: %s<br>\n"), gw);
		websWrite(wp, T("PriDns: %s<br>\n"), pd);
		websWrite(wp, T("SecDns: %s<br>\n"), sd);
	}
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = "http://%s%s", ip, websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
	doSystem("internet.sh");
#if defined(CONFIG_USER_SAMBA)
	doSystem("service samba restart");
#endif
}

static void restoremac(webs_t wp, char_t *path, char_t *query)
{
	/* Output timer for reloading */
	outputTimerForReload(wp, "", 80000);

	doSystem("fs factory_mac > /dev/console 2>&1");

	/* Reboot */
	reboot_now();
}

/* goform/setWan */
static void setWan(webs_t wp, char_t *path, char_t *query)
{
	char_t	*ctype, *req_ip, *dhcpVen;
	char_t	*ip, *nm, *gw, *mac, *oldmac;
	char_t	*eth, *user, *pass;
	char_t	*nat_enable;
	char_t	*vpn_srv, *vpn_mode;
	char_t	*l2tp_srv, *l2tp_mode;

	char_t *wan_mtu;
	char_t *submitUrl;
	char_t *st_en, *pd, *sd;
	int     i, flag = 1;

	char	*opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	char	*lan_ip = nvram_get(RT2860_NVRAM, "lan_ipaddr");
	char	*lan2enabled = nvram_get(RT2860_NVRAM, "Lan2Enabled");

	ctype = ip = nm = gw = eth = user = pass = mac = vpn_srv = vpn_mode = l2tp_srv = l2tp_mode = NULL;

	ctype = websGetVar(wp, T("connectionType"), T("0"));
	req_ip = websGetVar(wp, T("dhcpReqIP"), T(""));
	dhcpVen = websGetVar(wp, T("dhcpVendorClass"), T(""));

	if (!strncmp(ctype, "STATIC", 7) || !strcmp(opmode, "0"))
	{
		FILE *fd;

		//always treat bridge mode having static wan connection
		ip = websGetVar(wp, T("staticIp"), T(""));
		nm = websGetVar(wp, T("staticNetmask"), T("0"));
		gw = websGetVar(wp, T("staticGateway"), T(""));

		nvram_set(RT2860_NVRAM, "wanConnectionMode", ctype);
		if (inet_addr(ip) == -1)
		{
			websError(wp, 200, "invalid IP Address");
			return;
		}
		/*
		 * lan and wan ip should not be the same except in bridge mode
		 */
		if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan_ip, 15))
		{
			websError(wp, 200, "IP address is identical to LAN");
			return;
		}
		if (!strcmp(lan2enabled, "1"))
		{
			char *lan2_ip = nvram_get(RT2860_NVRAM, "lan2_ipaddr");
			if (NULL != opmode && strcmp(opmode, "0") && !strncmp(ip, lan2_ip, 15))
			{
				websError(wp, 200, "IP address is identical to LAN2");
				return;
			}
		}

		if (inet_addr(nm) == -1)
		{
			websError(wp, 200, "invalid Subnet Mask");
			return;
		}

		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "wan_ipaddr", ip);
		nvram_bufset(RT2860_NVRAM, "wan_netmask", nm);
		/*
		 * in Bridge Mode, lan and wan are bridged together and associated with
		 * the same ip address
		 */
		if (NULL != opmode && !strcmp(opmode, "0"))
		{
			nvram_bufset(RT2860_NVRAM, "lan_ipaddr", ip);
			nvram_bufset(RT2860_NVRAM, "lan_netmask", nm);
		}
		nvram_bufset(RT2860_NVRAM, "wan_gateway", gw);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);

		// Reset /etc/resolv.conf
		fd = fopen("/etc/resolv.conf", "w");
		if (fd != NULL)
			fclose(fd);
	}
	else if (strncmp(ctype, "DHCP", 5) == 0)
	{
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "wanConnectionMode", ctype);
		nvram_bufset(RT2860_NVRAM, "dhcpRequestIP", req_ip);
		nvram_bufset(RT2860_NVRAM, "dhcpVendorClass", dhcpVen);
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}
	else if (strncmp(ctype, "ZERO", 5) == 0)
	{
		nvram_set(RT2860_NVRAM, "wanConnectionMode", ctype);
	}
	else
	{
		websHeader(wp);
		websWrite(wp, T("<h2>Unknown Connection Type: %s</h2><br>\n"), ctype);
		websFooter(wp);
		websDone(wp, 200);
		return;
	}

	// Primary/Seconfary DNS set
	st_en = websGetVar(wp, T("wStaticDnsEnable"), T("off"));
	pd = websGetVar(wp, T("staticPriDns"), T(""));
	sd = websGetVar(wp, T("staticSecDns"), T(""));

	mac = websGetVar(wp, T("wanMac"), T(""));
	oldmac = nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");
	if (strlen(mac) == 17) {
		for (i = 0; i < strlen(mac); i++) {
			switch (i % 3) {
				case 2:
					if (mac[i] == '-')
						mac[i] = ':';
					flag &= (mac[i] == ':');
					break;
				default:
					flag &= ( (mac[i] >= '0' && mac[i] <= '9') ||
						(mac[i] >= 'a' && mac[i] <= 'f') ||
						(mac[i] >= 'A' && mac[i] <= 'F') );
			}
		}
	} else if (strlen(mac) == 0)
		mac = oldmac;

	if (!flag) {
		websError(wp, 200, "Invalid MAC Address, restore from factory!");
		doSystem("fs factory_mac > /dev/console 2>&1");
		return;
	}

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "wan_static_dns", st_en);

	if (strcmp(st_en, "on") == 0)
	{
		nvram_bufset(RT2860_NVRAM, "wan_primary_dns", pd);
		nvram_bufset(RT2860_NVRAM, "wan_secondary_dns", sd);
	}

	// NAT
	if (strcmp(opmode, "0") != 0)
	{
		nat_enable = websGetVar(wp, T("natEnabled"), T("off"));
		nat_enable = (strcmp(nat_enable, "on") == 0) ? "1" : "0";
		nvram_bufset(RT2860_NVRAM, "natEnabled", nat_enable);
	}

	// MTU for WAN
	wan_mtu = websGetVar(wp, T("wan_mtu"), T("0"));
	nvram_bufset(RT2860_NVRAM, "wan_manual_mtu", wan_mtu);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
#ifdef PRINT_DEBUG
	if ( !submitUrl[0] ) {
	    // debug print
	    websHeader(wp);
	    websWrite(wp, T("<h2>Mode: %s</h2><br>\n"), ctype);
	    if (!strncmp(ctype, "STATIC", 7))
	    {
		websWrite(wp, T("IP Address: %s<br>\n"), ip);
		websWrite(wp, T("Subnet Mask: %s<br>\n"), nm);
		websWrite(wp, T("Default Gateway: %s<br>\n"), gw);
	    }
	    websFooter(wp);
	    websDone(wp, 200);
	}
#endif
	if (strcmp(oldmac, mac) != 0) {
		nvram_init(RT2860_NVRAM);
		nvram_bufset(RT2860_NVRAM, "WAN_MAC_ADDR", mac);
		nvram_bufset(RT2860_NVRAM, "CHECKMAC", "NO");
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
		char_t *reboot_flag = websGetVar(wp, T("reboot"), T("0"));
		if (CHK_IF_DIGIT(reboot_flag, 1)) {
			/* Output timer for reloading */
			outputTimerForReload(wp, "" /* submitUrl */, 80000);

			/* Reboot */
			reboot_now();
		}
	}
	websRedirect(wp, submitUrl);

	/* Prevent deadloop at WAN apply change if VPN started */
	doSystem("ip route flush cache && service vpnhelper stop && service wan stop");
	initInternet();
}

#ifdef CONFIG_IPV6
const parameter_fetch_t service_ipv6_flags[] = 
{
	{ T("dhcp6c_enable"), "IPv6Dhcpc", 2, T("off") },
	{ T("ipv6_allow_forward"), "IPv6AllowForward", 2, T("off") },
	{ T("ipv6_Ipv6InVPN"), "Ipv6InVPN", 2, T("off") },
#ifdef CONFIG_USER_RADVD
	{ T("radvdEnbl"), "radvdEnabled", 0, T("0") },
#endif
#ifdef CONFIG_USER_DHCP6_SERVER
	{ T("dhcpv6Enbl"), "dhcpv6Enabled", 0, T("0") },
#endif
	{ NULL, NULL, 0, NULL } // Terminator
};

/* goform/setIPv6 */
static void setIPv6(webs_t wp, char_t *path, char_t *query)
{
	char_t *opmode, *submitUrl;
	char_t *ipaddr, *prefix_len, *wan_ipaddr, *wan_prefix_len, *srv_ipaddr;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	ipaddr = prefix_len = wan_ipaddr = wan_prefix_len = srv_ipaddr = NULL;

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 12, "IPv6OpMode", "IPv6IPAddr",
			"IPv6PrefixLen", "IPv6WANIPAddr", "IPv6WANPrefixLen",
			"IPv6GWAddr", "IPv6SrvAddr", "IPv6Dhcpc", "IPv6AllowForward",
			"Ipv6InVPN", "radvdEnabled", "dhcpv6Enabled");
		goto out;
	}

	opmode = websGetVar(wp, T("ipv6_opmode"), T("0"));

	nvram_init(RT2860_NVRAM);

	if (!strcmp(opmode, "1")) {
		ipaddr = websGetVar(wp, T("ipv6_lan_ipaddr"), T("2a02:2560"));
		prefix_len = websGetVar(wp, T("ipv6_lan_prefix_len"), T("32"));
		wan_ipaddr = websGetVar(wp, T("ipv6_wan_ipaddr"), T(""));
		wan_prefix_len = websGetVar(wp, T("ipv6_wan_prefix_len"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_static_gw"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6WANIPAddr", wan_ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6WANPrefixLen", wan_prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6GWAddr", srv_ipaddr);
#if defined (CONFIG_IPV6_SIT) ||  defined (CONFIG_IPV6_SIT_MODULE)
#if defined (CONFIG_IPV6_SIT_6RD)
	} else if (!strcmp(opmode, "2")) {
		ipaddr = websGetVar(wp, T("ipv6_6rd_prefix"), T(""));
		prefix_len = websGetVar(wp, T("ipv6_6rd_prefix_len"), T(""));
		srv_ipaddr = websGetVar(wp, T("ipv6_6rd_border_ipaddr"), T(""));
		nvram_bufset(RT2860_NVRAM, "IPv6IPAddr", ipaddr);
		nvram_bufset(RT2860_NVRAM, "IPv6PrefixLen", prefix_len);
		nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", srv_ipaddr);
#endif
	} else if (!strcmp(opmode, "3")) {
		ipaddr = websGetVar(wp, T("IPv6SrvAddr"), T("192.88.99.1"));
		nvram_bufset(RT2860_NVRAM, "IPv6SrvAddr", ipaddr);
#endif
	}
	nvram_bufset(RT2860_NVRAM, "IPv6OpMode", opmode);

	setupParameters(wp, service_ipv6_flags, 0);

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);
out:
#ifdef PRINT_DEBUG
	//debug print
	websHeader(wp);
	websWrite(wp, T("<h3>IPv6 Setup</h3><br>\n"));
	websWrite(wp, T("ipv6_opmode: %s<br>\n"), opmode);
	websWrite(wp, T("dhcp6c_enable: %s<br>\n"), dhcp6c_enable);
	websWrite(wp, T("ipv6_allow_forward: %s<br>\n"), ipv6_allow_forward);
	if (!strcmp(opmode, "1")) {
	    websWrite(wp, T("ipv6_lan_ipaddr: %s<br>\n"), ipaddr);
	    websWrite(wp, T("ipv6_lan_prefix_len: %s<br>\n"), prefix_len);
	    websWrite(wp, T("ipv6_wan_ipaddr: %s<br>\n"), wan_ipaddr);
	    websWrite(wp, T("ipv6_wan_prefix_len: %s<br>\n"), wan_prefix_len);
	    websWrite(wp, T("ipv6_static_gw: %s<br>\n"), srv_ipaddr);
#if defined (CONFIG_IPV6_SIT_6RD)
	} else if (!strcmp(opmode, "2")) {
	    websWrite(wp, T("ipv6_6rd_prefix: %s<br>\n"), ipaddr);
	    websWrite(wp, T("ipv6_6rd_prefix_len: %s<br>\n"), prefix_len);
	    websWrite(wp, T("ipv6_6rd_border_ipaddr: %s<br>\n"), srv_ipaddr);
#endif
#if defined (CONFIG_IPV6_SIT) ||  defined (CONFIG_IPV6_SIT_MODULE)
	} else if (!strcmp(opmode, "3")) {
	    websWrite(wp, T("IPv6SrvAddr: %s<br>\n"), ipaddr);
#endif
	}
	// Write OK
	websWrite(wp, T("<script language=\"JavaScript\" type=\"text/javascript\">ajaxReloadDelayedPage(10000, '/internet/ipv6.asp', true);</script>\n"));
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
	doSystem("internet.sh");
}

/*
 * arguments: ifname  - interface name
 *            if_addr - buffer to store ip address
 *            netmask - buffer to store prefix
 * description: fetch ipv6 address
 */
int getIfIPv6(const char *ifname, char *if_addr, char *netmask)
{
	FILE *fp;
	unsigned char ipv6[16];
	int scope, prefix;
	char address[INET6_ADDRSTRLEN] = "";
	char dname[IFNAMSIZ] = "";

	char_t *opmode = nvram_get(RT2860_NVRAM, "IPv6OpMode");

	if (!strcmp(opmode, "0")) {
		return -1;
	}

	if (NULL == (fp = fopen(_PATH_PROCNET_IPV6, "r"))) {
		syslog(LOG_ERR, "getIPv6IntAddr: open /proc/net/if_inet6 error, %s", __FUNCTION__);
		return -1;
	}

	while (19 == fscanf(fp, "%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx%2hhx %*x %x %x %*x %s",
		&ipv6[0], &ipv6[1], &ipv6[2], &ipv6[3], &ipv6[4], &ipv6[5], &ipv6[6], &ipv6[7], &ipv6[8], &ipv6[9],
		&ipv6[10], &ipv6[11], &ipv6[12], &ipv6[13], &ipv6[14], &ipv6[15], &prefix, &scope, dname)) {
		if (strcmp(ifname, dname) != 0) {
			continue;
		}
		if (inet_ntop(AF_INET6, ipv6, address, sizeof(address)) == NULL) {
			continue;
		}
		if (scope == IPV6_ADDR_GLOBAL) {
			break;
		}
	}
	fclose(fp);

	if (strlen(address)<=0) {
		return -255;
	} else {
		strcpy(if_addr, address);
		sprintf(netmask, "%d", prefix);
		return 0;
	}
}

static int  getIPv6IntAddr(int eid, webs_t wp, int argc, char_t **argv) {
	char address[INET6_ADDRSTRLEN] = "";
	char lanif[IFNAMSIZ] = "";
	char mask[16] = "";

	strcpy(lanif, getLanIfName());

	if (getIfIPv6(lanif, address, mask) != 0)
		return websWrite(wp, T(""));
	else
		return websWrite(wp, T("%s/%s"), address, mask);
}

static int  getIPv6ExtAddr(int eid, webs_t wp, int argc, char_t **argv) {
	char address[INET6_ADDRSTRLEN] = "";
	char tmpif[IFNAMSIZ] = "";
	char wanif[IFNAMSIZ] = "";
	char mask[16] = "";
	FILE *fp;

	char_t *opmode = nvram_get(RT2860_NVRAM, "IPv6OpMode");

	if (NULL == (fp = fopen("/tmp/six_wan_if_name", "r"))) {
		if (!strcmp(opmode, "1")) {
			if (vpn_mode_enabled() == 1)
				strcpy(wanif, getPPPIfName());
			else
				strcpy(wanif, getWanIfName());
		} else if (!strcmp(opmode, "2")) {
			strcpy(wanif, "6rd");
		} else if (!strcmp(opmode, "3")) {
			strcpy(wanif, "sit0");
		}
	} else {
		while ((fgets(tmpif, sizeof(tmpif), fp)) != NULL) {
			if ((strstr(tmpif, ETH_SIG) != NULL) || (strstr(tmpif, BR_SIG) != NULL) ||
				(strstr(tmpif, SIXRD_SIG) != NULL) || (strstr(tmpif, SIX2FOUR_SIG) != NULL)) {
				strcpy(wanif, strip_space(tmpif));
				break;
			}
		}
		fclose(fp);
	}

	if (getIfIPv6(wanif, address, mask) != 0) {
		return websWrite(wp, T(""));
	} else {
		return websWrite(wp, T("%s/%s"), address, mask);
	}
}
#endif

#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
#ifdef CONFIG_USER_CHILLISPOT
// ChilliSpot variables
const parameter_fetch_t chilli_vars[] =
{
	{ T("chilliEnable"),			"chilli_enable",			0,       T("") },
	{ T("spotProfile"),			"chilli_profile",			0,       T("manual") },
	{ T("sPriDns"),			"chilli_dns1",			0,       T("") },
	{ T("sSecDns"),			"chilli_dns2",			0,       T("") },
	{ T("sDomain"),			"chilli_domain",		0,       T("") },
	{ T("sStartIP"),		"chilli_dhcpstart",		0,       T("") },
	{ T("sEndIP"),			"chilli_dhcpend",		0,       T("") },
	{ T("sLease"),			"chilli_lease",			0,       T("") },
	{ T("sRadServer1"),		"chilli_radiusserver1",		0,       T("") },
	{ T("sRadServer2"),		"chilli_radiusserver2",		0,       T("") },
	{ T("sRadSecret"),		"chilli_radiussecret",		0,       T("") },
	{ T("sNasId"),			"chilli_radiusnasid",		0,       T("") },
	{ T("sRadLocId"),		"chilli_radiuslocationid",	0,       T("") },
	{ T("sRadLocName"),	"chilli_radiuslocationname",	0,       T("") },
	{ T("sRadCoaPort"),		"chilli_coaport",		0,       T("") },
	{ T("sRadCoaNoIpCheck"),	"chilli_coanoipcheck",		0,       T("") },
	{ T("sUamServer"),		"chilli_uamserver",		0,       T("") },
	{ T("sUamHomepage"),		"chilli_uamhomepage",		0,       T("") },
	{ T("sUamSecret"),		"chilli_uamsecret",		0,       T("") },
	{ T("sUamAllowed"),		"chilli_uamallowed",		0,       T("") },
	{ T("sUamDomain"),		"chilli_uamdomain",		0,       T("") },
	{ T("sUamAnyDNS"),		"chilli_uamanydns",		0,       T("") },
	{ T("sMacAllowed"),		"chilli_macallowed",		0,       T("") },
	{ NULL, NULL, 0, 0 } // Terminator
};
#endif

#ifdef CONFIG_USER_NODOGSPLASH
// NoDogSplash variables
const parameter_fetch_t nodog_vars[] =
{
	{ T("nodogEnable"),			"nodogsplash_enable",			0,       T("") },
	{ T("GatewayIPRange"),			"nodog_GatewayIPRange",			0,       T("0.0.0.0/0") },
	{ T("RedirectURL"),			"nodog_RedirectURL",			0,       T("http://wive-ng.sf.net") },
	{ T("MaxClients"),			"nodog_MaxClients",		0,       T("32") },
	{ T("ClientIdleTimeout"),			"nodog_ClientIdleTimeout",			0,       T("10") },
	{ T("ClientForceTimeout"),		"nodog_ClientForceTimeout",		0,       T("360") },
	{ T("AuthenticateImmediately"),		"nodog_AuthenticateImmediately",		0,       T("no") },
	{ T("MACMechanism"),		"nodog_MACMechanism",		0,       T("block") },
	{ T("BlockedMACList"),		"nodog_BlockedMACList",		0,       T("") },
	{ T("AllowedMACList"),			"nodog_AllowedMACList",		0,       T("") },
	{ T("TrustedMACList"),		"nodog_TrustedMACList",	0,       T("") },
	{ T("PasswordAuthentication"),	"nodog_PasswordAuthentication",	0,       T("no") },
	{ T("Password"),		"nodog_Password",		0,       T("nodog") },
	{ T("UsernameAuthentication"),	"nodog_UsernameAuthentication",		0,       T("no") },
	{ T("Username"),		"nodog_Username",		0,       T("guest") },
	{ T("PasswordAttempts"),		"nodog_PasswordAttempts",		0,       T("5") },
	{ NULL, NULL, 0, 0 } // Terminator
};
#endif

#ifdef CONFIG_USER_CHILLISPOT
/*
 * description: write hotspot network ip address
 */
static int getSpotIp(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");

	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return websWrite(wp, T("172.16.0.0"));
	}
	slashPos[0] = '\0';
	return websWrite(wp, T("%s"), nvdata);
}

/*
 * description: write hotspot network netmask
 */
static int getSpotNetmask(int eid, webs_t wp, int argc, char_t **argv)
{
	char_t *nvdata = nvram_get(RT2860_NVRAM, "chilli_net");
	unsigned int imask;
	struct in_addr mask;

	char* slashPos = strchr(nvdata, '/');
	if (slashPos == NULL) {
		return websWrite(wp, T("255.255.255.0"));
	}
	imask = atoi(slashPos+1);
	if (!imask)
		imask = 24;
	mask.s_addr = ntohl(0 - (1 << (32 - imask)));

	return websWrite(wp, T("%s"), inet_ntoa(mask));
}
#endif

/* goform/setHotspot */
static void setHotspot(webs_t wp, char_t *path, char_t *query)
{
	char_t *enabled = websGetVar(wp, T("spotEnable"), T("0"));
	char_t *submitUrl;
	char_t *reset = websGetVar(wp, T("reset"), T("0"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 39, "chilli_enable", "chilli_profile", "chilli_dns1",
			"chilli_dns2", "chilli_domain", "chilli_dhcpstart", "chilli_dhcpend",
			"chilli_lease", "chilli_radiusserver1", "chilli_radiusserver2",
			"chilli_radiussecret", "chilli_radiusnasid", "chilli_radiuslocationid",
			"chilli_radiuslocationname", "chilli_coaport", "chilli_coanoipcheck",
			"chilli_uamserver", "chilli_uamhomepage", "chilli_uamsecret", "chilli_uamallowed",
			"chilli_uamdomain", "chilli_uamanydns", "chilli_macallowed", "nodogsplash_enable",
			"nodog_GatewayIPRange", "nodog_RedirectURL", "nodog_MaxClients",
			"nodog_ClientIdleTimeout", "nodog_ClientForceTimeout", "nodog_AuthenticateImmediately",
			"nodog_MACMechanism", "nodog_BlockedMACList", "nodog_AllowedMACList",
			"nodog_TrustedMACList", "nodog_PasswordAuthentication", "nodog_Password",
			"nodog_UsernameAuthentication", "nodog_Username", "nodog_PasswordAttempts");
		goto out;
	}
#ifdef CONFIG_USER_CHILLISPOT
	char_t *ip = websGetVar(wp, T("sIp"), T("192.168.182.0"));
	char_t *amask = websGetVar(wp, T("sNetmask"), T("255.255.255.0"));
	struct in_addr iip;
	struct in_addr imask;

	iip.s_addr = inet_addr(ip);
	imask.s_addr = inet_addr(amask);
	int h_mask=ntohl(imask.s_addr);
	int i;
	for (i = 30; i > 0; i--) {
		if (h_mask >= 0 - (1 << (32 - i)))
			break;
	}
	if (!i) i = 24;
	iip.s_addr &= ntohl(0 - (1 << (32 - i)));

	char_t subnet[20];
	sprintf(subnet, "%s/%d", inet_ntoa(iip), i);
#endif

	nvram_init(RT2860_NVRAM);

#ifdef CONFIG_USER_CHILLISPOT
	if(CHK_IF_DIGIT(enabled, 1)) {
		setupParameters(wp, chilli_vars, 0);
		if (nvram_bufset(RT2860_NVRAM, "chilli_net", (void *)subnet)!=0) //!!!
			syslog(LOG_ERR, "set chilli_net nvram error, %s", __FUNCTION__);
	}
	else
#endif
#ifdef CONFIG_USER_NODOGSPLASH
	if(CHK_IF_DIGIT(enabled, 2))
		setupParameters(wp, nodog_vars, 0);
	else
#endif
	if(CHK_IF_DIGIT(enabled, 0)) {
#ifdef CONFIG_USER_CHILLISPOT
		nvram_bufset(RT2860_NVRAM, "chilli_enable", "no");
#endif
#ifdef CONFIG_USER_NODOGSPLASH
		nvram_bufset(RT2860_NVRAM, "nodogsplash_enable", "0");
#endif
	}

	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

out:
#ifdef PRINT_DEBUG
	//debug print
	websHeader(wp);
	websWrite(wp, T("Wait till device will be reconfigured...<br>\n"));
	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
	doSystem("services_restart.sh all");
}
#endif // HOTSPOT

static int getChilliBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_CHILLISPOT
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

static int getNoDogBuilt(int eid, webs_t wp, int argc, char_t **argv)
{
#ifdef CONFIG_USER_NODOGSPLASH
	return websWrite(wp, T("1"));
#else
	return websWrite(wp, T("0"));
#endif
}

void formDefineInternet(void) {
	websAspDefine(T("getDns"), getDns);
	websAspDefine(T("getHostSupp"), getHostSupp);
	websAspDefine(T("getIgmpProxyBuilt"), getIgmpProxyBuilt);
	websAspDefine(T("getVPNBuilt"), getVPNBuilt);
	websAspDefine(T("getLanIp"), getLanIp);
	websAspDefine(T("getLanMac"), getLanMac);
	websAspDefine(T("getLanNetmask"), getLanNetmask);
	websAspDefine(T("getDnsmasqBuilt"), getDnsmasqBuilt);
	websAspDefine(T("getGWBuilt"), getGWBuilt);
	websAspDefine(T("getCdpBuilt"), getCdpBuilt);
	websAspDefine(T("getLltdBuilt"), getLltdBuilt);
	websAspDefine(T("getLldpdBuilt"), getLldpdBuilt);
	websAspDefine(T("getUpnpBuilt"), getUpnpBuilt);
	websAspDefine(T("getXupnpdBuilt"), getXupnpdBuilt);
	websAspDefine(T("getIntIp"), getIntIp);
	websAspDefine(T("getWanIp"), getWanIp);
	websAspDefine(T("getWanMac"), getWanMac);
	websAspDefine(T("getWanNetmask"), getWanNetmask);
	websAspDefine(T("getWanGateway"), getWanGateway);
	websAspDefine(T("getRoutingTable"), getRoutingTable);
	websAspDefine(T("getWDSBuilt"), getWDSBuilt);
	websAspDefine(T("getMBSSIDBuilt"), getMBSSIDBuilt);
	websAspDefine(T("getUSBBuilt"), getUSBBuilt);
	websAspDefine(T("getStorageBuilt"), getStorageBuilt);
	websAspDefine(T("getFtpBuilt"), getFtpBuilt);
	websAspDefine(T("getSmbBuilt"), getSmbBuilt);
	websAspDefine(T("getPrinterSrvBuilt"), getPrinterSrvBuilt);
	websAspDefine(T("getUSBModemBuilt"), getUSBModemBuilt);
	websFormDefine(T("setLan"), setLan);
	websFormDefine(T("setWan"), setWan);
	websAspDefine(T("getRadvdBuilt"), getRadvdBuilt);
	websAspDefine(T("getDhcpv6Built"), getDhcpv6Built);
	websAspDefine(T("getIPv6Built"), getIPv6Built);
	websAspDefine(T("getIPv66rdBuilt"), getIPv66rdBuilt);
	websAspDefine(T("getIP6to4Built"), getIP6to4Built);
#ifdef CONFIG_IPV6
	websFormDefine(T("setIPv6"), setIPv6);
	websAspDefine(T("getIPv6IntAddr"), getIPv6IntAddr);
	websAspDefine(T("getIPv6ExtAddr"), getIPv6ExtAddr);
#endif
	websFormDefine(T("getMyMAC"), getMyMAC);
	websFormDefine(T("editRouting"), editRouting);
	websAspDefine(T("getTransmissionBuilt"), getTransmissionBuilt);

#ifdef CONFIG_USER_ZEBRA
	websFormDefine(T("dynamicRouting"), dynamicRouting);
#endif
	websAspDefine(T("getDynamicRoutingBuilt"), getDynamicRoutingBuilt);
	websAspDefine(T("getSWQoSBuilt"), getSWQoSBuilt);
	websAspDefine(T("getDATEBuilt"), getDATEBuilt);
	websAspDefine(T("getDDNSBuilt"), getDDNSBuilt);
	websAspDefine(T("getSpotBuilt"), getSpotBuilt);

	websAspDefine(T("getLANAUTHBuilt"), getLANAUTHBuilt);

	websAspDefine(T("getSysLogBuilt"), getSysLogBuilt);
	websAspDefine(T("getETHTOOLBuilt"), getETHTOOLBuilt);

	websAspDefine(T("vpnShowVPNStatus"), vpnShowVPNStatus);
	websAspDefine(T("vpnIfaceList"), vpnIfaceList);
	websFormDefine(T("formVPNSetup"), formVPNSetup);

	websFormDefine(T("restoremac"), restoremac);

#ifdef CONFIG_USER_CHILLISPOT
	websAspDefine(T("getSpotIp"), getSpotIp);
	websAspDefine(T("getSpotNetmask"), getSpotNetmask);
#endif
#if defined(CONFIG_USER_CHILLISPOT) || defined(CONFIG_USER_NODOGSPLASH)
	websFormDefine(T("setHotspot"), setHotspot);
#endif
	websAspDefine(T("getChilliBuilt"), getChilliBuilt);
	websAspDefine(T("getNoDogBuilt"), getNoDogBuilt);
}
