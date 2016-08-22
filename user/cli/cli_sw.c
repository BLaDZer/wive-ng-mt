/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/
#include "cli_sw.h"

int func_sw_lan(int argc, char* argv[])
{
    char *hostname = nvram_get(RT2860_NVRAM, "HostName");
    char *lan_ipaddr = nvram_get(RT2860_NVRAM, "lan_ipaddr");
    char *lan_netmask = nvram_get(RT2860_NVRAM, "lan_netmask");

    writeHeader("LAN Setup");

    if (hostname) 	printf(" Hostname:        %s\n", hostname);
    if (lan_ipaddr)	printf(" LAN IP Address:  %s\n", lan_ipaddr);
    if (lan_netmask)	printf(" LAN Netmask:     %s\n", lan_netmask);

    return 0;
}


int func_sw_wan(int argc, char* argv[])
{
    char *connMode = nvram_get(RT2860_NVRAM, "wanConnectionMode");
    char *wan_ipaddr = nvram_get(RT2860_NVRAM, "wan_ipaddr");
    char *wan_netmask = nvram_get(RT2860_NVRAM, "wan_netmask");
    char *wan_gateway = nvram_get(RT2860_NVRAM, "wan_gateway");
    char *wan_macaddr = nvram_get(RT2860_NVRAM, "WAN_MAC_ADDR");

    writeHeader("WAN Connection");

    if (connMode) 		printf(" Connection mode:     %s\n", connMode);

    if (STR_EQ(connMode, "STATIC"))
    {
	if (wan_ipaddr)		printf(" WAN IP Address:      %s\n", wan_ipaddr);
        if (wan_netmask)	printf(" WAN Netmask:         %s\n", wan_netmask);
        if (wan_gateway)	printf(" WAN Default Gateway: %s\n", wan_gateway);
	
    }

    if (wan_macaddr)		printf(" WAN MAC Address:     %s\n", wan_macaddr);

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
    {
	writeCmdHelp("lan","view lan settings");
	writeCmdHelp("wan","view wan settings");
    }


    return 0;
}
