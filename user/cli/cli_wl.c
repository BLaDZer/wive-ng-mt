/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/
#include "cli_wl.h"

/* wlan functions */
int func_wl(int argc, char* argv[])
{
    char* cmd = NULL;

    if (argc>0)
    {
	cmd = argv[0];
    }

    if (STR_EQ(cmd, "stalist"))
    {
	argc--;
	argv++;
	char* iface = "ra0";
	if (argc>0) 
	{
	    if (STR_EQ(argv[0],"2.4"))
	    {
		iface = "ra0";
	    }
	    else
	    if (STR_EQ(argv[0],"5"))
	    {
		iface = "rai0";
	    }
	    else
	    {
		iface = argv[0];
	    }

	    return func_wl_stalist(iface, argc, argv);
    	}
	else
	{
		writeCmdHelp("stalist 2.4", "show connected client stations at 2.4GHz");
		writeCmdHelp("stalist 5", "show connected client stations at 5GHz");
		writeCmdHelp("stalist <if>", "show connected client stations at specified interface");
	}

	return 0;

    }
    else
    if (STR_EQ(cmd, "scan"))
    {
	return func_wl_scan(argc, argv);
    }
    else
    if (STR_EQ(cmd, "status"))
    {
	return func_wl_status(argc, argv);
    }
    else
    {
	writeCmdHelp("scan", "scan and show remote AP list");
	writeCmdHelp("stalist", "show connected client stations");
	writeCmdHelp("status","view wlan settings");
    }

    return 0;
}

int func_wl_status(int argc, char* argv[])
{
    int n;
    int radio_status = nvram_get_int(RT2860_NVRAM, "RadioOn", 0);
    printf("Module status: %s\n", (radio_status>0)?"enabled":"disabled");

    char mac[18] = {0};

    getWlanCurrentMacAddr(mac, 1);

    printf("BSSID: %s\n", mac);

    int chan_num = getWlanChannelNum(1);
    if (chan_num > 0)
    {
	printf("Channel: %i\n", chan_num);
    }
    else
    {
	printf("Channel: auto\n");
    }

    char addr[ETH_ALEN] = {0};

    int ap_ret = getWlanAPMac("apcli0", addr);

    printf("APCli status: ");

    if (ap_ret<0) 
	printf("error (%i)\n",ap_ret);
    else if (ap_ret==0) 
	printf("disconnected\n");
    else
	printf("connected (%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx)\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    if (nvram_get_int(RT2860_NVRAM, "WdsEnable", 0) > 0)
    {
	char wdsifname[IFNAMSIZ] = {0};

	for (n=0;n<4;n++)
	{
	    RT_802_11_MAC_ENTRY entry = {0};
	    snprintf(wdsifname, IFNAMSIZ-1, "wds%i",n);

	    if (getWlanWDSEntry(wdsifname, &entry) != 0)
	    {
		continue;
	    }

	    printf("| %3i ", entry.Aid);
	    printf("| %02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx ", entry.Addr[0],entry.Addr[1],entry.Addr[2],entry.Addr[3],entry.Addr[4],entry.Addr[5]);
	    printf("| %02u:%02u:%02u ", (entry.ConnectedTime / (unsigned)3600), ((entry.ConnectedTime % (unsigned)3600) / (unsigned)60), (entry.ConnectedTime % (unsigned)60));
	    printf("| %s ", (entry.Psm==1)?"YES":"NO");
	    printf("| %10llu ", entry.RxBytes);
	    printf("| %10llu ", entry.TxBytes);
	    printf("|\n");
        }
    }


    return 0;
}

int func_wl_scan(int argc, char* argv[])
{
    char data[8192];
    if (wlanAPScanText("ra0", data, 8192))
    {
	return 1;
    }

/* // Parsed wl-scan version for future use

    int ent_count = 0;
    struct WLAN_AP_ENTRY *entries = wlanAPScan("ra0", &ent_count);

    if (entries == NULL)
    {
	return 2;
    }

    int n;
    for (n=0;n<ent_count;n++)
    {
	puts(entries[n].ssid);
    }

    free(entries);
*/

    printf("%s\n",data);
    return 0;
}

int func_wl_stalist(char* iface, int argc, char* argv[])
{
    int i;
    RT_802_11_MAC_TABLE table;
    if (getWlanStationTableIf(&table, iface) != 0)
    {
	return 1;
    }

    printf("| AID  | MAC               | Connected time | PSM | MIMO PS | TX MCS | Bandwidth | SGI | STBC | LDPC | HT/VHT Modes | TX Rate | RSSI           | Quality        | RX Bytes   | TX Bytes   | \n");

    for (i = 0; i < table.Num; i++) {
	RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);

	    printf("| %-4d ", pe->Aid);
	    printf("| %02X:%02X:%02X:%02X:%02X:%02X ", pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
	    printf("| %8u:%02u:%02u ", (pe->ConnectedTime / (unsigned)3600), ((pe->ConnectedTime % (unsigned)3600) / (unsigned)60), (pe->ConnectedTime % (unsigned)60));
	    printf("| %3s ", (pe->Psm == 0) ? "NO" : "YES");
	    printf("| %7s ", (pe->MimoPs == 0) ? "NO" : "YES");
	    printf("| %6d ", getMCS(pe->TxRate));
	    printf("| %9s ", getBW(pe->TxRate.field.BW));
	    printf("| %3s ", (pe->TxRate.field.ShortGI == 0) ? "NO" : "YES");
	    printf("| %4s ", (pe->TxRate.field.STBC == 0) ? "NO" : "YES");
	    printf("| %3s  ", (pe->TxRate.field.ldpc == 0) ? "NO" : "YES");
	    printf("| %12s ", getPhyMode(pe->TxRate.field.MODE));
	    printf("| %7d ", getWlanRate(pe->TxRate));

#if defined(CONFIG_RT_FIRST_IF_RT2860) || defined(CONFIG_RT_FIRST_IF_MT7620) || defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_FIRST_IF_MT7603E)
	    printf("| %7d %6d ", (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
	    printf("| %7d %6d ", ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1));
#else
	    printf("| %4d %4d %4d ", (int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));
	    printf("| %4d %4d %4d ", ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1), ConvertRssiToSignalQuality(pe->AvgRssi2));
#endif
	    printf("| %10llu ", (long long unsigned)pe->RxBytes);
	    printf("| %10llu ", (long long unsigned)pe->TxBytes);

	    printf("|\n");
    }

    return 0;
}
