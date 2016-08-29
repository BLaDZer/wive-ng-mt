#ifndef _WIVE_NETWORK_H
#define _WIVE_NETWORK_H 	1

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


struct RoutingRule {
    char interface[IFNAMSIZ];
    struct in_addr dest, gw, netmask;
    int flgs, ref, use, metric;
    char iftype[32];
    char comment[32];
    int internal_id;
    int category;
};

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

int getIfIsReady(const char *ifname);
int getIfIp(const char *ifname, char *if_addr);
int getIfMac(const char *ifname, char *if_hw, char separator);
int getIfNetmask(const char *ifname, char *if_net);

#ifdef CONFIG_IPV6
int getIfIPv6(const char *ifname, char *if_addr, char *netmask);
#endif

void arplookup(char *ip, char *arp);

struct RoutingRule* parseRoutingTable(char* rules_str, int *res_rule_num);
int removeRoutingRuleNvram(const char *iftype, const char *dest, const char *netmask, const char *gw);
void addRoutingRuleNvram(const char *iftype, const char *dest, const char *netmask, const char *gw, const char *ifname, const char *custom_if, const char *comment);

int getVPNStatusCode();
const char* getVPNStatusStr();

struct dyn_lease* getDhcpClientList(int *rownum, uint64_t *written_at);
int getDNSAddressStr(int index, char* out_buf);

#endif
