/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/
#include "cli_wl.h"

int doAPScan(char* iface)
{
    int ent_count = 0;
    struct WLAN_AP_ENTRY *entries = wlanAPScan(iface, &ent_count);

    if (entries == NULL)
    {
        return 1;
    }

    printf("Ch|SSID                            |BSSID            |Security              |Signal|W-Mode |ExtCH|NT \n");

    int n;
    for (n=0;n<ent_count;n++)
    {
        printf("%-2u", entries[n].chan);
        printf("|%-32.32s", entries[n].ssid);
        printf("|%02X:%02X:%02X:%02X:%02X:%02X", entries[n].bssid[0], entries[n].bssid[1], entries[n].bssid[2], entries[n].bssid[3], entries[n].bssid[4], entries[n].bssid[5]);
        printf("|%-22.22s", entries[n].security);
        printf("|%-6u", entries[n].signal_percent);
        printf("|%-7.7s", entries[n].wmode);
        printf("|%-5.5s", entries[n].extch);
        printf("|%-3.3s", entries[n].nt);
        printf("\n");
    }

    free(entries);

    return 0;
}

void printMacEntry(RT_802_11_MAC_ENTRY* pe)
{
        printf("%-3d", pe->Aid);
        printf("|%02X:%02X:%02X:%02X:%02X:%02X", pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
        printf("|%02u:%02u:%02u", (pe->ConnectedTime / (unsigned)3600), ((pe->ConnectedTime % (unsigned)3600) / (unsigned)60), (pe->ConnectedTime % (unsigned)60));
        printf("|%-3s", (pe->Psm == 0) ? "NO" : "YES");
        printf("|%-4s", (pe->MimoPs == 0) ? "NO" : "YES");
        printf("|%-3d", getMCS(pe->TxRate));
        printf("|%-2.2s", getBW(pe->TxRate.field.BW));
        printf("|%-3s", (pe->TxRate.field.ShortGI == 0) ? "NO" : "YES");
        printf("|%-4s", (pe->TxRate.field.STBC == 0) ? "NO" : "YES");
        printf("|%-4s", (pe->TxRate.field.ldpc == 0) ? "NO" : "YES");
        printf("|%-4.4s", getPhyMode(pe->TxRate.field.MODE));
        printf("|%-4d", getWlanRate(pe->TxRate));

//#if defined(CONFIG_RT_FIRST_IF_RT2860) || defined(CONFIG_RT_FIRST_IF_MT7620) || defined(CONFIG_RT_FIRST_IF_MT7602E) || defined(CONFIG_RT_FIRST_IF_MT7603E)
        printf("|%-4d %-4d", (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
        printf("|%-3d %-3d  ", ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1));
/*#else
        printf("|%-4d %-4d %-4d", (int)(pe->AvgRssi0), (int)(pe->AvgRssi1), (int)(pe->AvgRssi2));
        printf("|%-3d %-3d %-3d", ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1), ConvertRssiToSignalQuality(pe->AvgRssi2));
#endif*/
        char *rx_scaled_text = scale(pe->RxBytes);
        char *tx_scaled_text = scale(pe->TxBytes);
        printf("|%8.8s", rx_scaled_text);
        printf("|%8.8s", tx_scaled_text);
        free(rx_scaled_text);
        free(tx_scaled_text);

        printf("\n");

}

int showStationList(char* iface)
{
    int i;
    RT_802_11_MAC_TABLE table;

    if (getWlanStationTableIf(&table, iface) != 0)
    {
        return 1;
    }

    printf("AID|MAC              |C. TIME |PSM|MMPS|MCS|BW|SGI|STBC|LDPC|MODE|RATE|RSSI     |S.QUALITY|RX      |TX      \n");

    for (i = 0; i < table.Num; i++) 
    {
        RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
        printMacEntry(pe);
    }

    printf("\n");
    return 0;

}

/* wlan functions */
int func_wl(int argc, char* argv[])
{
    char* cmd = (argc>0) ? argv[0] : NULL;
    argc--;
    argv++;

    if (!cmd || STR_EQ_HELP(cmd))
    {
        writeCmdHelp("scan 2.4/5/<if>", "scan and show remote AP list");
        writeCmdHelp("stalist 2.4/5/<if>", "show connected client stations");
        writeCmdHelp("status","view wlan status");
        writeCmdHelp("wds","view wds status");
        printf("\n");
    }
    else
    if (STR_EQ(cmd, "stalist"))
    {
        return func_wl_stalist(argc, argv);
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
    if (STR_EQ(cmd, "wds"))
    {
        return func_wl_wds(argc, argv);
    }

    return 0;
}

int func_wl_wds(int argc, char* argv[])
{
    writeHeader("WDS");

    int wds_enabled = nvram_get_int(RT2860_NVRAM, "WdsEnable", 0);
    printf("WDS status:            %s\n",wds_enabled?"enabled":"disabled");

    if (wds_enabled)
    {
        char wdsifname[IFNAMSIZ] = {0};
        int n;

        printf("WDS client list: \n");
        printf("AID|MAC              |C. TIME |PSM|MMPS|MCS|BW|SGI|STBC|LDPC|MODE|RATE|RSSI     |S.QUALITY|RX      |TX      \n");

        for (n=0;n<4;n++)
        {
            RT_802_11_MAC_ENTRY entry = {0};
            snprintf(wdsifname, IFNAMSIZ-1, "wds%i",n);

            if (getWlanMacEntry(wdsifname, &entry) != 0)
            {
                continue;
            }

            printMacEntry(&entry);
        }
    }

    printf("\n");
    return 0;
}

int func_wl_status(int argc, char* argv[])
{
    writeHeader("WLAN Status");

    int radio_status = nvram_get_int(RT2860_NVRAM, "RadioOn", 0);
    printf("Module status:         %s\n", (radio_status>0)?"enabled":"disabled");

    char mac[18] = {0};

    getWlanCurrentMacAddr(mac, 1);

    printf("BSSID:                 %s\n", mac);

    int chan_num = getWlanChannelNum(1);
    if (chan_num > 0)
    {
        printf("Channel:               %i\n", chan_num);
    }
    else
    {
        printf("Channel:               auto\n");
    }

// * * * APCli * * *

    char addr[ETH_ALEN] = {0};

    int ap_ret = getWlanAPMac("apcli0", addr);
#ifndef CONFIG_RT_SECOND_IF_NONE
    if (ap_ret <= 0) ap_ret = getWlanAPMac("apclii0", addr);
#endif

    printf("APCli status:          ");

    if (ap_ret<0) 
        printf("error (%i) \n",ap_ret);
    else if (ap_ret==0) 
        printf("disconnected\n");
    else 
        printf("connected (%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx)\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    printf("\n");
    return 0;
}

int func_wl_scan(int argc, char* argv[])
{
    char* cmd = (argc>0) ? argv[0] : NULL;
    argc--;
    argv++;

    if (!cmd)
    {
        if (doAPScan("ra0"))
        {
            return 1;
        }

#ifndef CONFIG_RT_SECOND_IF_NONE
        if (doAPScan("rai0"))
        {
            return 2;
        }
#endif
    }
    else
    if (STR_EQ_HELP(cmd))
    {
        writeCmdHelp("scan <if>", "scan at specified interface");
        writeCmdHelp("scan 2.4", "scan at 2.4GHz");
#ifndef CONFIG_RT_SECOND_IF_NONE
        writeCmdHelp("scan 5", "scan at 5GHz");
#endif
    }
    else
    {
        char* iface = cmd;

        if (STR_EQ(iface, "2.4"))
        {
            iface = "ra0";
        }
#ifndef CONFIG_RT_SECOND_IF_NONE
        else
        if (STR_EQ(iface, "5"))
        {
            iface = "rai0";
        }
#endif

        if (doAPScan(iface))
        {
            return 3;
        }
    }


    printf("\n");
    return 0;
}

int func_wl_stalist(int argc, char* argv[])
{
    char* cmd = (argc>0) ? argv[0] : NULL;
    argc--;
    argv++;

    if (!cmd) // show every band
    {
        showStationList("ra0");
#ifndef CONFIG_RT_SECOND_IF_NONE
        printf("\n\n");
        showStationList("rai0");
#endif
    }
    else
    if (STR_EQ_HELP(cmd))
    {
        writeCmdHelp("stalist <if>", "show connected client stations at specified interface");
        writeCmdHelp("stalist 2.4", "show connected client stations at 2.4GHz");
#ifndef CONFIG_RT_SECOND_IF_NONE
        writeCmdHelp("stalist 5", "show connected client stations at 5GHz");
#endif
    }
    else
    if (STR_EQ(cmd,"2.4"))
    {
        showStationList("ra0");
    }
#ifndef CONFIG_RT_SECOND_IF_NONE
    else
    if (STR_EQ(cmd,"5"))
    {
        showStationList("rai0");
    }
#endif
    else
    {
        showStationList(cmd);
    }


    printf("\n");
    return 0;
}
