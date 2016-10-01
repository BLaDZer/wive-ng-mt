/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/
#include "cli_sw.h"

int func_sw_ipv6(int argc, char* argv[])
{
    int ipv6_mode = nvram_get_int(RT2860_NVRAM, "IPv6OpMode", 0);
    int over_vpn = nvram_get_int(RT2860_NVRAM, "Ipv6InVPN", 0);
    int dhcpc_enabled = nvram_get_int(RT2860_NVRAM, "IPv6Dhcpc", 0);
    int allow_forward = nvram_get_int(RT2860_NVRAM, "IPv6AllowForward", 0);

    char* dns_primary = nvram_get(RT2860_NVRAM, "IPv6DNSPrimary");
    char* dns_secondary = nvram_get(RT2860_NVRAM, "IPv6DNSSecondary");

    char* gateway = nvram_get(RT2860_NVRAM, "IPv6GWAddr");

    int radvd_enabled = nvram_get_int(RT2860_NVRAM, "radvdEnabled", 0);
    int dhcpv6_enabled = nvram_get_int(RT2860_NVRAM, "dhcpv6Enabled", 0);

    char address[INET6_ADDRSTRLEN] = "";
    char mask[16] = "";

    if (is_report(argc, argv))
    {
        getIPv6IntIPAddr(address, mask);

        printf("IPv6 Internal Address\t%s\n", address);
        printf("IPv6 Internal Netmask\t%s\n", mask);

        address[0] = '\0';
        mask[0] = '\0';

        getIPv6ExtIPAddr(address, mask);

        printf("IPv6 External Address\t%s\n", address);
        printf("IPv6 External Netmask\t%s\n", mask);

        printf("IPv6 Default gateway\t%s\n", gateway);
        printf("IPv6 Primary DNS\t%s\n", dns_primary);
        printf("IPv6 Secondary DNS\t%s\n", dns_secondary);

        printf("IPv6 Mode\t");
        switch (ipv6_mode) {
            case 0: printf("disabled\n"); break;
            case 1: printf("native\n"); break;
            case 2: printf("6to4\n"); break;
            default: printf("unknown\n");
        }

        printf("IPv6 over VPN\t%s\n",over_vpn?"TRUE":"FALSE");
        printf("IPv6 autoconfig by DHCP/RA\t%s\n",dhcpc_enabled?"1":"0");
        printf("Access to LAN from internet\t%s\n",allow_forward?"1":"0");

        printf("Router Advertisement(radvd)\t%s\n",radvd_enabled?"1":"0");
        printf("Dynamic IPv6 configuration(dhcp6s)\t%s\n",dhcpv6_enabled?"1":"0");

        printf("COMMIT\tipv6\n");

        return 0;
    }

    writeHeader("IPv6");

    printf("Internal IPv6 Address:              ");

    if (getIPv6IntIPAddr(address, mask) == 0)
    {
        printf("%s/%s", address, mask);
    }

    printf("\n");

    printf("External IPv6 Address:              ");

    if (getIPv6ExtIPAddr(address, mask) == 0)
    {
        printf("%s/%s", address, mask);
    }
    printf("\n");

    printf("IPv6 Mode:                          ");

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

    printf("IPv6 over VPN:                      %s \n",over_vpn?"yes":"no");
    printf("IPv6 autoconfig by DHCP/RA:         %s \n",dhcpc_enabled?"yes":"no");
    printf("Access to LAN from internet:        %s \n",allow_forward?"yes":"no");

    if (!dhcpc_enabled)
    {
        printf("\n");
        printf("Default gateway:                    %s \n", gateway);
        printf("Primary DNS Address:                %s \n", dns_primary);
        printf("Secondary DNS Address:              %s \n", dns_secondary);
    }

    printf("\n");

    printf("Router Advertisement(radvd):        %s \n",radvd_enabled?"enabled":"disabled");
    printf("Dynamic IPv6 configuration(dhcp6s): %s \n",dhcpv6_enabled?"enabled":"disabled");

    printf("\n");
    return 0;
}

int func_sw_lan(int argc, char* argv[])
{
    char *hostname = nvram_get(RT2860_NVRAM, "HostName");
    char *lan_ipaddr = nvram_get(RT2860_NVRAM, "lan_ipaddr");
    char *lan_netmask = nvram_get(RT2860_NVRAM, "lan_netmask");

    if (is_report(argc, argv))
    {
        printf("LAN Hostname\t%s\n", hostname);
        printf("LAN IP Address\t%s\n", lan_ipaddr);
        printf("LAN Netmask\t%s\n", lan_netmask);
        printf("COMMIT\tlan\n");
        return 0;
    }

    writeHeader("LAN Setup");

    if (hostname)
        printf("Hostname:               %s\n", hostname);
    if (lan_ipaddr)
        printf("LAN IP Address:         %s\n", lan_ipaddr);
    if (lan_netmask)
        printf("LAN Netmask:            %s\n", lan_netmask);

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

    char wan_phy_addr[16] = {0};
    char wan_phy_netmask[16] = {0};
    char wan_phy_gw[16] = {0};

    if (is_report(argc, argv))
    {
        getIfIp(getWanIfName(), wan_phy_addr);
        getIfNetmask(getWanIfName(), wan_phy_netmask);
        getWANGateway(wan_phy_gw);

        printf("Connection mode\t%s\n", connMode);
        printf("Physical WAN IP Addr\t%s\n", wan_phy_addr);
        printf("Physical WAN Netmask\t%s\n", wan_phy_netmask);
        printf("Physical WAN Gateway\t%s\n", wan_phy_gw);

        if (STR_EQ(connMode, "STATIC"))
        {
            if (wan_ipaddr)  printf("WAN IP Address\t%s\n", wan_ipaddr);
            if (wan_netmask) printf("WAN Netmask\t%s\n", wan_netmask);
            if (wan_gateway) printf("WAN Default Gateway\t%s\n", wan_gateway);
        }

        if (wan_macaddr)     printf("WAN MAC Address\t%s\n", wan_macaddr);

        printf("WAN Static DNS\t%s\n", STR_EQ(wan_static_dns, "on")?"1":"0");

        if (getDNSAddressStr(1, dns_addr) == 0)
            printf("Primary DNS Address\t%s\n", dns_addr);

        if (getDNSAddressStr(2, dns_addr) == 0)
            printf("Secondary DNS Address\t%s\n", dns_addr);

        printf("COMMIT\twan\n");

        return 0;
    }

    writeHeader("WAN Connection");

    if (connMode)        printf("Connection mode:       %s \n", connMode);

    if (getIfIp(getWanIfName(), wan_phy_addr) != -1) 
    {
        printf("Physical WAN IP Addr:  %s \n", wan_phy_addr);
    }

    if (getIfNetmask(getWanIfName(), wan_phy_netmask) != -1) 
    {
        printf("Physical WAN Netmask:  %s \n", wan_phy_netmask);
    }

    int gw_err = getWANGateway(wan_phy_gw);
    if (gw_err == 0)
    {
        printf("Physical WAN Gateway:  %s \n", wan_phy_gw);
    }

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


int func_sw_interface(int argc, char* argv[])
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

        if (argc>0 && strcmp(nc.ifname,argv[0]) != 0)
        {
            continue;
        }

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
    return 0;
}

int func_sw_port(int argc, char* argv[])
{
    writeHeader("Port status");
    int port;
    struct port_counts pcs;
    int one_port = -1;

    if (argc > 0)
    {
        one_port = strToIntDef(argv[0], -1);
    }

    portscounts(&pcs);

    printf("Num|Link|Speed|Duplex|RX Bytes  |TX Bytes  \n");
    for (port=4; port>-1; port--)
    {
        if (one_port != -1 && port != one_port)
        {
            continue;
        }

        struct port_status pst;
        portstatus(&pst, port);

        char *rx_str = scale((uint64_t)pcs.rx_count[port]);
        char *tx_str = scale((uint64_t)pcs.tx_count[port]);

        printf("%3d|%4.4s|%5d|%6.6s|%10.10s|%10.10s\n", pst.portnum, pst.link!=0?"YES":"NO", pst.speed, (pst.duplex == 1) ? "Full" : "Half", rx_str, tx_str);

        free(rx_str);
        free(tx_str);
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
    if (STR_EQ(cmd, "ipv6"))
    {
        return func_sw_ipv6(argc, argv);
    }
    else
    if (STR_EQ(cmd, "int") || STR_EQ(cmd, "interface"))
    {
        return func_sw_interface(argc, argv);
    }
    else
    if (STR_EQ(cmd, "port"))
    {
        return func_sw_port(argc, argv);
    }
    else
    {
        writeCmdHelp("lan","view lan settings");
        writeCmdHelp("wan","view wan settings");
        writeCmdHelp("ipv6","view ipv6 status");
        writeCmdHelp("port","view port info");
        writeCmdHelp("interface","view interface info");
        printf("\n");
    }

    return 0;
}
