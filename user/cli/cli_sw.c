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

    printf("IPv6 Mode:              ");

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

int func_sw_lan(int argc, char* argv[])
{
    char *hostname = nvram_get(RT2860_NVRAM, "HostName");
    char *lan_ipaddr = nvram_get(RT2860_NVRAM, "lan_ipaddr");
    char *lan_netmask = nvram_get(RT2860_NVRAM, "lan_netmask");

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
