#ifndef _WIVE_NETWORK_H
#define _WIVE_NETWORK_H 	1

/* utils for goahead, cli, nvram, netcwmpd */

#define _PATH_PPP_ROUTES		"/etc/routes_ppp_replace"
#define _PATH_LANWAN_ROUTES		"/etc/routes_replace"

#define _PATH_PFW			"/etc"
#define _PATH_PFW_FILE			_PATH_PFW "/portforward"
#define _PATH_PFW_FILE_VPN		_PATH_PFW "/portforward_vpn"
#define _PATH_PFW_FILE_ACCESS		_PATH_PFW "/portforward_access"
#define _PATH_MACIP_FILE		_PATH_PFW "/macipfilter"
#define _PATH_MACIPINPUT_FILE		_PATH_PFW "/macipinputfilter"
#define _PATH_WEBS_FILE			_PATH_PFW "/websfilter"

#define WEB_FILTER_CHAIN		"web_filter"
#define IPPORT_FILTER_CHAIN		"macipport_filter"

#define PORT_FORWARD_PRE_CHAIN		"port_forward_pre"
#define PORT_FORWARD_POST_CHAIN		"port_forward_post"

#define PORT_FORWARD_PRE_CHAIN_VPN	"port_forward_pre_vpn"
#define PORT_FORWARD_POST_CHAIN_VPN	"port_forward_post_vpn"

#define PROTO_UNKNOWN			0
#define PROTO_TCP			1
#define PROTO_UDP			2
#define PROTO_TCP_UDP			3
#define PROTO_ICMP			4
#define PROTO_NONE			5

#define ACTION_DROP			0
#define ACTION_ACCEPT			1

#define BR_SIG	"br"
#define ETH_SIG	"eth"
#define VPN_SIG	"ppp"
#define VPN_DEF "ppp0"

#ifdef CONFIG_RAETH_BOTH_GMAC		/* dual phy/rgmii mode */
#define WAN_DEF "eth3"
#else
#define WAN_DEF "eth2.2"		/* internal ralink esw with vlan parted mode */
#endif

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

/*** VPN statuses ***/
typedef struct vpn_status_t
{
    const char*   statusStr;
    long          color;
} vpn_status_t;

struct PortFilteringRule  {
    int protocol;
    char mac_address[32];
    char dip[32], dprf[8], dprt[8], interface[8], sim[32], dim[32];
    char sip[32], sprf[8], sprt[8], comment[16], action[4];
};

struct PortForwardRule  {
    int protocol, nat_loopback;
    char ip_address[32], prf[8], prt[8], rprf[8], rprt[8], comment[64], interface[8];
};

struct RoutingRule {
    char interface[IFNAMSIZ];
    struct in_addr dest, gw, netmask;
    int flgs, ref, use, metric;
    char iftype[32];
    char comment[32];
    int internal_id;
    int category;
};

#ifdef CONFIG_IPV6
struct RoutingRuleIPv6 {
    char dest[INET6_ADDRSTRLEN];
    int dest_prefix;

    char src[INET6_ADDRSTRLEN];
    int src_prefix;

    char next_hop[INET6_ADDRSTRLEN];

    unsigned int metric;
    unsigned int ref;
    unsigned int use;
    unsigned int flgs;

    char interface[IFNAMSIZ];
};
#endif

/*** Busybox leases.h ***/
uint64_t hton64(uint64_t v);

#define ntoh64(v) hton64(v)
typedef uint32_t leasetime_t;
typedef int32_t signed_leasetime_t;

struct dyn_lease {
    /* "nip": IP in network order */
    /* Unix time when lease expires. Kept in memory in host order.
     * When written to file, converted to network order
     * and adjusted (current time subtracted) */
    leasetime_t expires;
    uint32_t lease_nip;
    /* We use lease_mac[6], since e.g. ARP probing uses
     * only 6 first bytes anyway. We check received dhcp packets
     * that their hlen == 6 and thus chaddr has only 6 significant bytes
     * (dhcp packet has chaddr[16], not [6])
     */
    uint8_t lease_mac[6];
    char hostname[20];
    uint8_t pad[2];
    /* total size is a multiply of 4 */
} __attribute__((__packed__));

int isMacValid(char *str);
int isIpValid(char *str);
int isIpNetmaskValid(char *s);

int vpn_mode_enabled(void);
char* getLanIfName(void);
char* getWanIfName(void);
char *getLanWanNamebyIf(const char *ifname);
char* getPPPIfName(void);

int getIfIsReady(const char *ifname);
int getIfIp(const char *ifname, char *if_addr);
int getIfMac(const char *ifname, char *if_hw, char separator);
int getIfNetmask(const char *ifname, char *if_net);

#ifdef CONFIG_IPV6
int getIfIPv6(const char *ifname, char *if_addr, char *netmask);
int getIPv6IntIPAddr(char* address, char* mask);
int getIPv6ExtIPAddr(char* address, char* mask);
#endif

void arplookup(char *ip, char *arp);

int parse_portforward_rule(char* rulestr, struct PortForwardRule* rule, int rule_num);
int parse_portfiltering_rule(char* rulestr, struct PortFilteringRule* rule, int rule_num);

struct RoutingRuleIPv6* parseRoutingTableIPv6(int *res_rule_num);
struct RoutingRule* parseRoutingTable(char* rules_str, int *res_rule_num);
int removeRoutingRuleNvram(const char *iftype, const char *dest, const char *netmask, const char *gw);
void addRoutingRuleNvram(const char *iftype, const char *dest, const char *netmask, const char *gw, const char *ifname, const char *custom_if, const char *comment);

int getVPNStatusCode();
const char* getVPNStatusStr();

struct dyn_lease* getDhcpClientList(int *rownum, uint64_t *written_at);
int getDNSAddressStr(int index, char* out_buf);

int getWANGateway(char* sgw);
int getRuleNums(char *rules);

void static_routing_rebuild_etc(void);
void firewall_rebuild_etc(void);

#endif
