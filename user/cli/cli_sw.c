/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/
#include "cli_sw.h"

int showDHCPStaticTable()
{
    char* staticList = nvram_get_copy(RT2860_NVRAM, "dhcpStatic");

    char *ptr = strtok(staticList, ";");

    printf("|MAC Address      |IP Address     |Comment                                      \n");

    while (ptr)
    {
        if (strlen(ptr) > 164)
            continue;

        char mac[18];
        char ip[16];
        char comment[128];

        if (sscanf(ptr, "%17s %15s %127s", mac, ip, comment) == 3)
        {
            mac[17] = '\0';
            ip[15] = '\0';
            comment[127] = '\0';

            // MAC
            printf("|%17.17s", mac);
            // IP
            printf("|%15.15s", ip);
            // Comment
            printf("|%-45.45s", comment);
            printf("\n");
        }

        ptr = strtok(NULL, ";");
    }

    free(staticList);

    return 0;
}

int showDHCPClientTable()
{
    int rownum;
    struct in_addr addr;
    uint64_t written_at, curr, expired_abs;

    int row_len = 0;
    struct dyn_lease* leases = getDhcpClientList(&row_len, &written_at);

    if (leases == NULL)
        return 1;

    curr = time(NULL);

    printf("|Hostname                        |MAC Address      |IP Address     |Expires    \n");

    /* Output leases file */
    for (rownum=0; rownum<row_len; rownum++)
    {
        struct dyn_lease lease = leases[rownum];

        expired_abs = ntohl(lease.expires) + written_at;
        if (expired_abs < curr)
            continue;

        // Host
        printf("|%-32.32s", lease.hostname);
        // MAC
        printf("|%02x:%02x:%02x:%02x:%02x:%02x", lease.lease_mac[0], lease.lease_mac[1], lease.lease_mac[2], lease.lease_mac[3], lease.lease_mac[4], lease.lease_mac[5]);

        // IP
        addr.s_addr = lease.lease_nip;
        printf("|%15.15s", inet_ntoa(addr));

        printf("|");
        // Expire Date
        if (expired_abs > curr)
        {
            leasetime_t expires = expired_abs - curr;
            unsigned d = expires / (24*60*60);
            expires %= (24*60*60);
            unsigned h = expires / (60*60);
            expires %= (60*60);
            unsigned m = expires / 60;
            expires %= 60;

            if (d>0)
                printf("%02u:", d);
            else
                printf("   ");

            printf("%02u:%02u:%02u", h, m, (unsigned)expires);
        }
	else
	{
            printf("    expired");
	}

        printf("\n");
    }

	if (leases)
	    free(leases);

	return 0;

}

int func_sw_dhcp(int argc, char* argv[])
{
    int dhcpEnabled = nvram_get_int(RT2860_NVRAM, "dhcpEnabled", -1);

    writeHeader("DHCP");

    printf("Status:           %s \n", dhcpEnabled?"enabled":"disabled" );
    printf("DHCP Domain:      %s \n", nvram_get(RT2860_NVRAM, "dhcpDomain") );
    printf("Start IP Address: %s \n", nvram_get(RT2860_NVRAM, "dhcpStart") );
    printf("End IP Address:   %s \n", nvram_get(RT2860_NVRAM, "dhcpEnd") );
    printf("Subnet Mask:      %s \n", nvram_get(RT2860_NVRAM, "dhcpMask") );
    printf("Default Gateway:  %s \n", nvram_get(RT2860_NVRAM, "dhcpGateway") );
    printf("Lease Time (sec): %s \n", nvram_get(RT2860_NVRAM, "dhcpLease") );

    writeHeader("DHCP Clients");
    showDHCPClientTable();

    writeHeader("Static IP address assignment table");
    showDHCPStaticTable();

    printf("\n");

    return 0;
}

int func_sw_lan(int argc, char* argv[])
{
    char *hostname = nvram_get(RT2860_NVRAM, "HostName");
    char *lan_ipaddr = nvram_get(RT2860_NVRAM, "lan_ipaddr");
    char *lan_netmask = nvram_get(RT2860_NVRAM, "lan_netmask");

    int ipv6_mode = nvram_get_int(RT2860_NVRAM, "IPv6OpMode", 0);

    writeHeader("LAN Setup");

    if (hostname)
        printf("Hostname:        %s\n", hostname);
    if (lan_ipaddr)
        printf("LAN IP Address:  %s\n", lan_ipaddr);
    if (lan_netmask)
        printf("LAN Netmask:     %s\n", lan_netmask);

    printf("IPv6 Mode:       ");

    switch (ipv6_mode)
    {
        case 0:
            printf("Disabled \n");
            break;
        case 1:
            printf("Native dynamic/static IP connection \n");
            break;
        case 3:
            printf("Tunneling Connection (6TO4) \n");
            break;

        default:
            printf("Unknown \n");
    }

    printf("\n");
    return 0;
}


int func_sw_wan(int argc, char* argv[])
{
    char dns_addr[16] = {0};

    char *connMode = nvram_get(RT2860_NVRAM, "wanConnectionMode");
    char *wan_ipaddr = nvram_get(RT2860_NVRAM, "wan_ipaddr");
    char *wan_netmask = nvram_get(RT2860_NVRAM, "wan_netmask");
    char *wan_gateway = nvram_get(RT2860_NVRAM, "wan_gateway");
    char *wan_macaddr = nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");

    char *wan_static_dns = nvram_get(RT2860_NVRAM, "wan_static_dns");

    writeHeader("WAN Connection");

    if (connMode)        printf("Connection mode:       %s \n", connMode);

    if (STR_EQ(connMode, "STATIC"))
    {
        if (wan_ipaddr)  printf("WAN IP Address:        %s \n", wan_ipaddr);
        if (wan_netmask) printf("WAN Netmask:           %s \n", wan_netmask);
        if (wan_gateway) printf("WAN Default Gateway:   %s \n", wan_gateway);
    }

    if (wan_macaddr)     printf("WAN MAC Address:       %s \n", wan_macaddr);

    writeHeader("DNS");

    if (STR_EQ(wan_static_dns, "on"))
    {
        printf("DNS Mode:              Static \n");
    }
    else
    {
        printf("DNS Mode:              Dynamic \n");
    }

    if (getDNSAddressStr(1, dns_addr) == 0)
        printf("Primary DNS Address:   %s \n", dns_addr);

    if (getDNSAddressStr(2, dns_addr) == 0)
        printf("Secondary DNS Address: %s \n", dns_addr);

    printf("\n");
    return 0;
}

int func_sw_vpn(int argc, char* argv[])
{
    writeHeader("VPN");

    printf("VPN Status:       %s \n",getVPNStatusStr());

    printf("VPN Type:         ");

    int vpn_type = nvram_get_int(RT2860_NVRAM, "vpnType", -1);
    switch (vpn_type)
    {
        case 0:
                printf("PPPoE client \n");
                printf("PPPoE interface:  %s \n", nvram_get(RT2860_NVRAM, "vpnInterface") );
                printf("\n");

                printf("AC Name:          %s \n", nvram_get(RT2860_NVRAM, "vpnServer") );
                printf("Service name:     %s \n", nvram_get(RT2860_NVRAM, "vpnService") );

            break;

        case 1:
                printf("PPTP client \n");
                printf("Host/IP/DNS name: %s \n", nvram_get(RT2860_NVRAM, "vpnServer") );
            break;

        case 2:
                printf("L2TP client \n");
            break;

        case 3:
                printf("KABINET Authorization \n");
            break;

        default: printf("Unknown \n"); break;
    }

    if (vpn_type != 3)
    {
        int auth_method = nvram_get_int(RT2860_NVRAM, "vpnAuthProtocol",0);

        printf("VPN Auth method:  ");
        switch (auth_method)
        {
            case 0:  printf("auto    \n"); break;
            case 1:  printf("PAP     \n"); break;
            case 2:  printf("CHAP    \n"); break;
            case 3:  printf("MSCHAP  \n"); break;
            default: printf("Unknown \n");
        }

        printf("Username:         %s \n", nvram_get(RT2860_NVRAM, "vpnUser") );
        printf("Default gateway:  %s \n", nvram_get_int(RT2860_NVRAM, "vpnDGW", 0)?"enable":"disable" );
    }

    printf("\n");
    return 0;
}

int func_sw_switch(int argc, char* argv[])
{
    writeHeader("Interface status");

    int i;
    int elem_count = 0;
    struct nic_counts* ncs = nicscounts(&elem_count);

    printf("Name            |RX Bytes  |TX Bytes  |RX Packets|TX Packets\n");

    for (i=0;i<elem_count;i++)
    {
        struct nic_counts nc = ncs[i];
        if (nc.ifname[0] == '\0')
            break;

        if (!nc.is_available) {
            // not extracted - print n/a
            printf("n/a");
            continue;
        }
        else
        {
            char *rx_tmpstr, *tx_tmpstr;
                // scale bytes to K/M/G/Tb
            rx_tmpstr = scale((uint64_t)nc.rx_bytes);
            tx_tmpstr = scale((uint64_t)nc.tx_bytes);

            if (rx_tmpstr && tx_tmpstr)
            {
                printf("%-16.16s|%10.10s|%10.10s|%10llu|%10llu\n", nc.ifname, rx_tmpstr, tx_tmpstr, nc.rx_packets, nc.tx_packets );
                free(rx_tmpstr);
                free(tx_tmpstr);
            }
            else
            {
                printf("n/a");
            }
        }

    }

    free(ncs);

    writeHeader("Port status");

    int port;

    printf("Num|Link|Speed|Duplex\n");
    for (port=4; port>-1; port--)
    {
        struct port_status pst;
        portstatus(&pst, port);
        printf("%3d|%4.4s|%5d|%6.6s\n", pst.portnum, pst.link!=0?"YES":"NO", pst.speed, (pst.duplex == 1) ? "Full" : "Half");
    }

    printf("\n");
    return 0;
}

/* switch functions */
int func_sw(int argc, char* argv[])
{
    char* cmd = NULL;

    if (argc>0)
    {
        cmd = argv[0];
        argc--;
        argv++;
    }

    if (STR_EQ(cmd, "lan"))
    {
        return func_sw_lan(argc, argv);
    }
    else
    if (STR_EQ(cmd, "wan"))
    {
        return func_sw_wan(argc, argv);
    }
    else
    if (STR_EQ(cmd, "vpn"))
    {
        return func_sw_vpn(argc, argv);
    }
    else
    if (STR_EQ(cmd, "dhcp"))
    {
        return func_sw_dhcp(argc, argv);
    }
    else
    if (STR_EQ(cmd, "switch"))
    {
        return func_sw_switch(argc, argv);
    }
    else
    {
        writeCmdHelp("lan","view lan settings");
        writeCmdHelp("wan","view wan settings");
        writeCmdHelp("vpn","view vpn settings");
        writeCmdHelp("dhcp","view dhcp server settings");
        writeCmdHelp("switch","switch info");
        printf("\n");
    }

    return 0;
}
