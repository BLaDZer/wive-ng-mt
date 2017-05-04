/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/
#include "cli_wl.h"

char* wirelessModeNameToId(char* val)
{
    if (STR_EQ(val,"b-g"))
    {
        return "0";
    }
    else
    if (STR_EQ(val,"b"))
    {
        return "1";
    }
    else
    if (STR_EQ(val,"a"))
    {
        return "2";
    }
    else
    if (STR_EQ(val,"g"))
    {
        return "4";
    }
    else
    if (STR_EQ(val,"n"))
    {
        return "6";
    }
    else
    if (STR_EQ(val,"g-n"))
    {
        return "7";
    }
    else
    if (STR_EQ(val,"a-an"))
    {
        return "8";
    }
    else
    if (STR_EQ(val,"b-g-n"))
    {
        return "9";
    }
    else
    if (STR_EQ(val,"an"))
    {
        return "11";
    }
    else
    if (STR_EQ(val,"a-an-ac"))
    {
        return "14";
    }
    else
    if (STR_EQ(val,"an-ac"))
    {
        return "15";
    }

    return NULL;
}

char* wirelessModeIdToName(char* val)
{
    if (STR_EQ(val,"0"))
    {
        return "b-g";
    }
    else
    if (STR_EQ(val,"1"))
    {
        return "b";
    }
    else
    if (STR_EQ(val,"2"))
    {
        return "a";
    }
    else
    if (STR_EQ(val,"4"))
    {
        return "g";
    }
    else
    if (STR_EQ(val,"6"))
    {
        return "n";
    }
    else
    if (STR_EQ(val,"7"))
    {
        return "g-n";
    }
    else
    if (STR_EQ(val,"8"))
    {
        return "a-an";
    }
    else
    if (STR_EQ(val,"9"))
    {
        return "b-g-n";
    }
    else
    if (STR_EQ(val,"11"))
    {
        return "an";
    }
    else
    if (STR_EQ(val,"14"))
    {
        return "a-an-ac";
    }
    else
    if (STR_EQ(val,"15"))
    {
        return "an-ac";
    }


    return NULL;
}




int doAPScan(char* iface)
{
    int ent_count = 0;
    struct WLAN_AP_ENTRY *entries = wlanAPScan(iface, &ent_count);

    if (entries == NULL)
    {
        return 1;
    }

    printf("Ch |SSID                            |BSSID            |Security              |Signal|W-Mode  |ExtCH|NT \n");

    int n;
    for (n=0;n<ent_count;n++)
    {
        printf("%-3u", entries[n].chan);
        printf("|%-32.32s", entries[n].ssid);
        printf("|%02X:%02X:%02X:%02X:%02X:%02X", entries[n].bssid[0], entries[n].bssid[1], entries[n].bssid[2], entries[n].bssid[3], entries[n].bssid[4], entries[n].bssid[5]);
        printf("|%-22.22s", entries[n].security);
        printf("|%-6u", entries[n].signal_percent);
        printf("|%-8.8s", entries[n].wmode);
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
        printf("|%-5.5s", getPhyMode(pe->TxRate.field.MODE));
        printf("|%-4d", getWlanRate(pe->TxRate));
        printf("|%-4d %-4d", (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
        printf("|%-3d %-3d  ", ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1));

        char *rx_scaled_text = scale(pe->RxBytes);
        char *tx_scaled_text = scale(pe->TxBytes);
        printf("|%9.9s", rx_scaled_text);
        printf("|%9.9s", tx_scaled_text);
        free(rx_scaled_text);
        free(tx_scaled_text);

        printf("\n");

}

void printMacEntry_report(RT_802_11_MAC_ENTRY* pe, int band_num)
{
        char mac[18] = {0};

        printf("STALISTELEM\t");

        getWlanCurrentMacAddr(mac, band_num);

        printf("%s\t", mac);
        printf("%02X:%02X:%02X:%02X:%02X:%02X\t", pe->Addr[0], pe->Addr[1], pe->Addr[2], pe->Addr[3], pe->Addr[4], pe->Addr[5]);
        printf("%u\t", pe->ConnectedTime);
        printf("%u\t", pe->Psm);
        printf("%u\t", pe->MimoPs);
        printf("%u\t", getMCS(pe->TxRate));
        printf("%s\t", getBW(pe->TxRate.field.BW));
        printf("%u\t", pe->TxRate.field.ShortGI);
        printf("%u\t", pe->TxRate.field.STBC);
        printf("%u\t", pe->TxRate.field.ldpc);
        printf("%s\t", getPhyMode(pe->TxRate.field.MODE));
        printf("%u\t", getWlanRate(pe->TxRate));

        printf("%d\t%d\t", (int)(pe->AvgRssi0), (int)(pe->AvgRssi1));
        printf("%d\t%d\t", ConvertRssiToSignalQuality(pe->AvgRssi0), ConvertRssiToSignalQuality(pe->AvgRssi1));

        printf("%llu\t", pe->RxBytes);
        printf("%llu\t", pe->TxBytes);

        printf("\n");
}


int showStationList_report(char* iface)
{
    int i;
    RT_802_11_MAC_TABLE table;
    int band_num = (strcmp(iface,"ra0") == 0)?1:2;

    if (getWlanStationTableIf(&table, iface) != 0)
    {
        return 1;
    }

    printf("STALIST\n");


    for (i = 0; i < table.Num; i++) 
    {
        RT_802_11_MAC_ENTRY *pe = &(table.Entry[i]);
        printMacEntry_report(pe, band_num);
    }

    return 0;
}


int showStationList(char* iface)
{
    int i;
    RT_802_11_MAC_TABLE table;

    if (getWlanStationTableIf(&table, iface) != 0)
    {
        return 1;
    }

    printf("AID|MAC              |C. TIME |PSM|MMPS|MCS|BW|SGI|STBC|LDPC|MODE |RATE|RSSI     |S.QUALITY|RX       |TX       \n");

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
        writeCmdHelp("set 2.4/5 <param>", "change wlan parameters");
        writeCmdHelp("disconnect <band> <mac>", "disconnect client station by MAC");
        writeCmdHelp("disconnect <band> all", "disconnect all client stations");
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
    if (STR_EQ(cmd, "set"))
    {
        return func_wl_set(argc, argv);
    }
    else
    if (STR_EQ(cmd, "wds"))
    {
        return func_wl_wds(argc, argv);
    }
    else
    if (STR_EQ(cmd, "disconnect"))
    {
        return func_wl_disconnect(argc, argv);
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

int func_wl_status_report(int argc, char* argv[])
{
    int radio_status = nvram_get_int(RT2860_NVRAM, "RadioOn", 0);
    int auto_channel = nvram_get_int(RT2860_NVRAM, "AutoChannelSelect", 0);
    int tx_power = nvram_get_int(RT2860_NVRAM, "TxPower", 100);

    char mac1[18] = {0};
    char mac2[18] = {0};
    int chan_num;

    char addr[ETH_ALEN] = {0};

    int ap_ret = getWlanAPMac("apcli0", addr);
    getWlanCurrentMacAddr(mac1, 1);
#ifndef CONFIG_RT_SECOND_IF_NONE
    if (ap_ret <= 0) ap_ret = getWlanAPMac("apclii0", addr);
    getWlanCurrentMacAddr(mac2, 2);
#endif

    /* FIXME: remove fallback */

    chan_num  = getWlanChannelNum_ioctl(1);
    

    printf("BSSID\t%s\n", mac1);
    printf("BSSID 2.4\t%s\n", mac1);
    printf("Channel\t%i\n", chan_num);
    printf("Channel 2.4\t%i\n", chan_num);
    printf("Ext channel 2.4\t%i\n", nvram_get_int(RT2860_NVRAM, "HT_EXTCHA", 0));

    printf("AutoChannel 2.4\t%i\n", auto_channel);
    printf("Tx Power 2.4\t%i\n", tx_power);

#ifndef CONFIG_RT_SECOND_IF_NONE
    printf("BSSID 5\t%s\n", mac2);
    chan_num  = getWlanChannelNum_ioctl(2);
    printf("Channel 5\t%i\n", chan_num);
    printf("Ext channel 5\t%i\n", nvram_get_int(RT2860_NVRAM, "HT_EXTCHAINIC", 0));

    auto_channel = nvram_get_int(RT2860_NVRAM, "AutoChannelSelectINIC", 0);
    printf("AutoChannel 5\t%i\n", auto_channel);

    tx_power = nvram_get_int(RT2860_NVRAM, "TxPowerINIC", 100);
    printf("Tx Power 5\t%i\n", tx_power);
#endif

    printf("Radio Enabled\t%s\n", radio_status?"1":"0");
    printf("APCli Status\t%i\n", ap_ret);
    printf("APCli Address\t%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    printf("SSID to SSID isolation\t%s\n", nvram_get_int(RT2860_NVRAM, "NoForwardingBTNBSSID", 0)?"1":"0");
    printf("LAN to WLAN isolation\t%s\n", nvram_get_int(RT2860_NVRAM, "LanWifiIsolate", 0)?"1":"0");

    printf("SSID List\n");

    char* hidden_ssid = nvram_get(RT2860_NVRAM, "HideSSID");
    int hidden_ssid_len = strlen(hidden_ssid);

    char* isolated_ssid = nvram_get(RT2860_NVRAM, "NoForwarding");
    int isolated_ssid_len = strlen(isolated_ssid);

    char* bcisolated_ssid = nvram_get(RT2860_NVRAM, "NoForwardingMBCast");
    int bcisolated_ssid_len = strlen(bcisolated_ssid);

    int ssid_num;
    char ssid_nvram_id[6] = {0};

    int bssid_num = nvram_get_int(RT2860_NVRAM, "BssidNum", 0);
    int band_num = 1;

#ifndef CONFIG_RT_SECOND_IF_NONE
    int mbssid_5ghz = strcmp(nvram_get(RT2860_NVRAM, "BssidIfName"), "rai") == 0;

    for (band_num=1; band_num<=2; band_num++)
#endif
    for (ssid_num=1; ssid_num <= bssid_num; ssid_num++)
    {
        char* mac = (band_num==1)?mac1:mac2;

        sprintf(ssid_nvram_id, "SSID%i", ssid_num);

#ifndef CONFIG_RT_SECOND_IF_NONE
        if (ssid_num > 1)
        {
            if (mbssid_5ghz && band_num == 1) break;
            else
            if (!mbssid_5ghz && band_num == 2) break;
        }
        else
        {
            if (band_num == 2)
            {
                sprintf(ssid_nvram_id, "SSID%iINIC", ssid_num);
            }
        }
#endif

        char* ssid_name = nvram_get(RT2860_NVRAM, ssid_nvram_id);
        if (ssid_name[0] == '\0')
            break;

        int is_hidden = 0;
        int is_client_isolated = 0;
        int is_broad_isolated = 0;
        int str_ind = (ssid_num-1)*2;

        if ((str_ind < hidden_ssid_len) && (hidden_ssid[str_ind] == '1'))
        {
            is_hidden = 1;
        }

        if ((str_ind < isolated_ssid_len) && (isolated_ssid[str_ind] == '1'))
        {
            is_client_isolated = 1;
        }

        if ((str_ind < bcisolated_ssid_len) && (bcisolated_ssid[str_ind] == '1'))
        {
            is_broad_isolated = 1;
        }

        printf("SSID Elem\t");
        printf("%i\t",ssid_num);
        printf("%s\t",mac);
        printf("%s\t",ssid_name);
        printf("%i\t",is_hidden);
        printf("%i\t",is_client_isolated);
        printf("%i\t",is_broad_isolated);
        printf("\n");
    }

    printf("COMMIT\twlan\n");

    return 0;
}

int func_wl_set24(int argc, char* argv[])
{
    char* cmd = NULL;
    char* val = NULL;

    while (argc>1)
    {
        cmd = argv[0];
        val = argv[1];
        argc -= 2;
        argv += 2;

        if (STR_EQ(cmd, "tx-power"))
        {
            int power_val = strToIntDef(val, 0);
            if (power_val > 0 && power_val <= 100)
            {
                char power_str[8] = {0};
                sprintf((char*)power_str, "%i", power_val);
                nvram_set(RT2860_NVRAM, "TxPower", power_str);
            }
            else
            {
                printf("tx-power parameter out of range.");
            }
        }
        else
        if (STR_EQ(cmd, "mode"))
        {
            char* modeId = wirelessModeNameToId(val);
            if (modeId)
            {
                nvram_set(RT2860_NVRAM, "WirelessMode", modeId);
            }
        }
        else
        if (STR_EQ(cmd, "channel"))
        {
            int chan_num = strToIntDef(val, 0);

            if (chan_num >= 0)
            {
                nvram_set(RT2860_NVRAM, "channel", val);
                nvram_set(RT2860_NVRAM, "AutoChannelSelect", (chan_num == 0)?"1":"0");
            }
            else
            {
                printf("channel parameter out of range.");
            }
        }
        else
        if (STR_EQ(cmd, "enabled"))
        {
            nvram_set(RT2860_NVRAM, "RadioOn", STR_EQ(val,"on")?"1":"0");
        }
        else
        if (argc > 0 && STR_EQ(cmd, "ssid") && STR_EQ(val,"1"))
        {
            nvram_set(RT2860_NVRAM, "SSID1", argv[0]);
            argv++;
            argc--;
        }
        else
        if (argc > 0 && STR_EQ(cmd, "hidden"))
        {
            int ssid_num = strToIntDef(val, 0);
            if (ssid_num <= 0 || ssid_num > 8)
            {
                goto help;
            }

            nvram_set_tuple(RT2860_NVRAM, "HideSSID", ssid_num-1, STR_EQ(argv[0],"on")?"1":"0");
            argv++;
            argc--;
        }
        else
        if (argc > 0 && STR_EQ(cmd, "isolate-clients"))
        {
            int ssid_num = strToIntDef(val, 0);
            if (ssid_num <= 0 || ssid_num > 8)
            {
                goto help;
            }

            nvram_set_tuple(RT2860_NVRAM, "NoForwarding", ssid_num-1, STR_EQ(argv[0],"on")?"1":"0");
            argv++;
            argc--;
        }
        else
        if (argc > 0 && STR_EQ(cmd, "isolate-broadcast"))
        {
            int ssid_num = strToIntDef(val, 0);
            if (ssid_num <= 0 || ssid_num > 8)
            {
                goto help;
            }

            nvram_set_tuple(RT2860_NVRAM, "NoForwardingMBCast", ssid_num-1, STR_EQ(argv[0],"on")?"1":"0");
            argv++;
            argc--;
        }
        else
        {
            printf("Unknown command '%s'!\n", cmd);
            goto help;
        }
    }

help:
    writeCmdHelp("enabled on/off","enable or disable radio module");
    writeCmdHelp("channel <num>","change current wlan channel");
    writeCmdHelp("tx-power 5/10/20/40/70/100","change TX Power level (%%)");
    writeCmdHelp("mode b/g/n/b-g/g-n/b-g-n","change wlan 2.4GHz network mode");
    writeCmdHelp("ssid 1 <name>","change first ssid name");
    writeCmdHelp("hidden 1 <name>","change first ssid hidden mode");
    writeCmdHelp("isolate-clients 1 <name>","change first ssid client isolation");
    writeCmdHelp("isolate-broadcast 1 <name>","change first ssid broadcast isolation");

    return 0;

}

int func_wl_set5(int argc, char* argv[])
{
    char* cmd = NULL;
    char* val = NULL;

    while (argc>1)
    {
        cmd = argv[0];
        val = argv[1];
        argc -= 2;
        argv += 2;

        if (STR_EQ(cmd, "tx-power"))
        {
            int power_val = strToIntDef(val, 0);
            if (power_val > 0 && power_val <= 100)
            {
                char power_str[8] = {0};
                sprintf((char*)power_str, "%i", power_val);
                nvram_set(RT2860_NVRAM, "TxPowerINIC", power_str);
            }
            else
            {
                printf("tx-power parameter out of range.");
            }
        }
        else
        if (STR_EQ(cmd, "mode"))
        {
            char* modeId = wirelessModeNameToId(val);
                if (modeId)
            {
                nvram_set(RT2860_NVRAM, "WirelessModeINIC", modeId);
            }
        }
        else
        if (STR_EQ(cmd, "channel"))
        {
            int chan_num = strToIntDef(val, 0);
            if (chan_num >= 0)
            {
                nvram_set(RT2860_NVRAM, "channelINIC", val);
                nvram_set(RT2860_NVRAM, "AutoChannelSelectINIC", (chan_num == 0)?"1":"0");
            }
            else
            {
                printf("channel parameter out of range.");
            }
        }
        else
        if (STR_EQ(cmd, "enabled"))
        {
            nvram_set(RT2860_NVRAM, "RadioOnINIC", STR_EQ(val,"on")?"1":"0");
        }
        else
        if (argc > 0 && STR_EQ(cmd, "ssid") && STR_EQ(val,"1"))
        {
            nvram_set(RT2860_NVRAM, "SSID1INIC", argv[0]);
            argv++;
            argc--;
        }
        else
        {
            printf("Unknown command '%s'!\n", cmd);
            goto help;
        }
    }

    return 0;

help:
    writeCmdHelp("2.4 <command> <value>","change wlan 2.4GHz band parameter");
    writeCmdHelp("5 <command> <value>","change wlan 5GHz band parameter");
    writeCmdHelp("5 enabled on/off","enable or disable radio module");
    writeCmdHelp("5 channel <num>","change current wlan channel");
    writeCmdHelp("5 tx-power 5/10/20/40/70/100","change TX Power level (%%)");
    writeCmdHelp("5 mode a/a-an/an/a-an-ac/an-ac","change wlan 5GHz network mode");
    writeCmdHelp("5 ssid 1 <name>","change first ssid name");

    return 0;

}

int func_wl_set(int argc, char* argv[])
{
    char* band = NULL;
    char* cmd = NULL;
    char* val = NULL;

    if (argc < 2)
    {
        goto help;
    }

    band = argv[0];
    argc--;
    argv++;

    while (argc>1)
    {
        cmd = argv[0];
        val = argv[1];
        argc -= 2;
        argv += 2;

        if (STR_EQ(band, "2.4"))
        {
            return func_wl_set24(argc,argv);
        }
#ifndef CONFIG_RT_SECOND_IF_NONE
        else
        if (STR_EQ(band, "5"))
        {
            return func_wl_set5(argc,argv);
        }
#endif
        else goto help;

    }

    return 0;

help:
    writeCmdHelp("2.4 <command> <value>","change wlan 2.4GHz band parameter");
    writeCmdHelp("5 <command> <value>","change wlan 5GHz band parameter");

    return 0;
}


int func_wl_status(int argc, char* argv[])
{
    int ssid_num;
    char ssid_nvram_id[6] = {0};

    if (is_report(argc, argv))
    {
        argc--;
        argv++;
        return func_wl_status_report(argc, argv);
    }

    writeHeader("WLAN Status");

    int radio_status = nvram_get_int(RT2860_NVRAM, "RadioOn", 0);
    printf("Module status:          %s\n", (radio_status>0)?"enabled":"disabled");


    char mac[18] = {0};

    getWlanCurrentMacAddr(mac, 1);
    printf("BSSID:                  %s\n", mac);

    int chan_num = getWlanChannelNum_ioctl(1);
    if (chan_num > 0)
    {
        printf("Channel:                %i\n", chan_num);
    }
    else
    {
        printf("Channel:                auto\n");
    }

// * * * APCli * * *

    char addr[ETH_ALEN] = {0};

    int ap_ret = getWlanAPMac("apcli0", addr);
#ifndef CONFIG_RT_SECOND_IF_NONE
    if (ap_ret <= 0) ap_ret = getWlanAPMac("apclii0", addr);
#endif

    printf("APCli status:           ");

    if (ap_ret<0) 
        printf("error (%i) \n",ap_ret);
    else if (ap_ret==0) 
        printf("disconnected\n");
    else 
        printf("connected (%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx)\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    writeHeader("WLAN Settings");

    printf("SSID List: \n");
    printf("Num|SSID                            |Hidden|Cl.Isolate|Br.Isolate \n");

    char* hidden_ssid = nvram_get(RT2860_NVRAM, "HideSSID");
    int hidden_ssid_len = strlen(hidden_ssid);

    char* isolated_ssid = nvram_get(RT2860_NVRAM, "NoForwarding");
    int isolated_ssid_len = strlen(isolated_ssid);

    char* bcisolated_ssid = nvram_get(RT2860_NVRAM, "NoForwardingMBCast");
    int bcisolated_ssid_len = strlen(bcisolated_ssid);

    int bssid_num = nvram_get_int(RT2860_NVRAM, "BssidNum", 0);

#ifndef CONFIG_RT_SECOND_IF_NONE
    int band_num;
    int mbssid_5ghz = (strcmp(nvram_get(RT2860_NVRAM, "BssidIfName"), "rai") == 0);

    for (band_num=1; band_num <= 2; band_num++)
#endif
    for (ssid_num=1; ssid_num <= bssid_num; ssid_num++)
    {
        sprintf(ssid_nvram_id, "SSID%i", ssid_num);

#ifndef CONFIG_RT_SECOND_IF_NONE
        if (ssid_num > 1)
        {
            if (mbssid_5ghz && band_num == 1) break;
            else
            if (!mbssid_5ghz && band_num == 2) break;
        }
        else
        {
            if (band_num == 2)
            {
                sprintf(ssid_nvram_id, "SSID%iINIC", ssid_num);
            }
        }
#endif

        char* ssid_name = nvram_get(RT2860_NVRAM, ssid_nvram_id);
        if (ssid_name[0] == '\0')
            break;

        int is_hidden = 0;
        int is_client_isolated = 0;
        int is_broad_isolated = 0;
        int str_ind = (ssid_num-1)*2;

        if ((str_ind < hidden_ssid_len) && (hidden_ssid[str_ind] == '1'))
        {
            is_hidden = 1;
        }

        if ((str_ind < isolated_ssid_len) && (isolated_ssid[str_ind] == '1'))
        {
            is_client_isolated = 1;
        }

        if ((str_ind < bcisolated_ssid_len) && (bcisolated_ssid[str_ind] == '1'))
        {
            is_broad_isolated = 1;
        }

        printf("%-3i|%-32.32s|%6.6s|%10.10s|%10.10s", ssid_num, ssid_name, is_hidden?"YES":"NO",is_client_isolated?"YES":"NO",is_broad_isolated?"YES":"NO");
        printf("\n");
    }

    printf("\n");

    printf("SSID to SSID isolation: %s \n", nvram_get_int(RT2860_NVRAM, "NoForwardingBTNBSSID", 0)?"yes":"no");
    printf("LAN to WLAN isolation:  %s \n", nvram_get_int(RT2860_NVRAM, "LanWifiIsolate", 0)?"yes":"no");

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
	printf("BAND 2.4GHz:\n");
        if (doAPScan("ra0"))
        {
            return 1;
        }

#ifndef CONFIG_RT_SECOND_IF_NONE
	printf("\n");
	printf("BAND 5GHz:\n");
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
	    printf("BAND 2.4GHz:\n");
            iface = "ra0";
        }
#ifndef CONFIG_RT_SECOND_IF_NONE
        else
        if (STR_EQ(iface, "5"))
        {
	    printf("BAND 5GHz:\n");
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

int func_wl_stalist_report(int argc, char* argv[])
{
#ifndef CONFIG_RT_SECOND_IF_NONE
        showStationList_report("rai0");
#endif
        return showStationList_report("ra0");
}

int func_wl_stalist_iface(char* iface, int argc, char* argv[])
{
    char* cmd = (argc>0) ? argv[0] : NULL;
    argc--;
    argv++;

    if (cmd && STR_EQ_HELP(cmd))
    {
        writeCmdHelp("stalist <if>", "show connected client stations");
        printf("\n");
    }
    else
    {
        showStationList(iface);
    }

    return 0;

}

int func_wl_disconnect(int argc, char* argv[])
{
    char* cmd = (argc>0) ? argv[0] : NULL;
    argc--;
    argv++;
    char* mac = (argc>0) ? argv[0] : NULL;
    argc--;
    argv++;

    if (!cmd || STR_EQ_HELP(cmd))
    {
        writeCmdHelp("disconnect <band> <mac>", "disconnect client station by MAC");
        writeCmdHelp("disconnect <band> all", "disconnect all client stations");
        printf("\n");
        return 0;
    }

    int ret_code = 0;

    if (STR_EQ(cmd, "2.4"))
    {
        ret_code = func_wl_disconnect_iface("ra0", mac);
        if (ret_code > 0) 
        {
            printf("WLAN Disconnect %s from 2.4GHz error %i! \n", mac, ret_code);
            return 1;
        }
    }
#ifndef CONFIG_RT_SECOND_IF_NONE
    else
    if (STR_EQ(cmd, "5"))
    {
        ret_code = func_wl_disconnect_iface("rai0", mac);
        if (ret_code > 0) 
        {
            printf("WLAN Disconnect %s from 5GHz error %i! \n", mac, ret_code);
            return 1;
        }
    }
#endif
    else
    {
        if (mac != NULL)
        {
            ret_code = func_wl_disconnect_iface(cmd, mac);
            if (ret_code > 0) 
            {
                printf("WLAN Disconnect %s from %s error %i! \n", mac, cmd, ret_code);
                return 1;
            }
        }
        else
        {
            ret_code = func_wl_disconnect_iface("ra0", cmd);
            int is_err = 0;

            if (ret_code > 0) 
            {
                printf("WLAN Disconnect %s from 2.4GHz error %i! \n", cmd, ret_code);
                is_err = 1;
            }

#ifndef CONFIG_RT_SECOND_IF_NONE
            ret_code = func_wl_disconnect_iface("rai0", cmd);
            if (ret_code > 0) 
            {
                printf("WLAN Disconnect %s from 5GHz error %i! \n", cmd, ret_code);
                is_err = 1;
            }
#endif

            if (is_err)
            {
                return 1;
            }
        }
    }

    return 0;

}

int func_wl_disconnect_iface(char* iface, char* mac)
{
    int ret;

    if (mac == NULL)
    {
        return 1;
    }

    if (STR_EQ(mac, "all"))
    {
        ret = wlanDisconnectAllStations(iface);
        if (ret != 0)
        {
            return 2;
        }
    }
    else
    if (strlen(mac) == 17)
    {
        ret = wlanDisconnectStation(iface, mac);
        if (ret != 0)
        {
            return 2;
        }
    }
    else
    {
        return 3;
    }

    return 0;

}


int func_wl_stalist(int argc, char* argv[])
{

    if (is_report(argc, argv))
    {
        argc--;
        argv++;
        return func_wl_stalist_report(argc,argv);
    }

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
        func_wl_stalist_iface("ra0", argc, argv);
    }
#ifndef CONFIG_RT_SECOND_IF_NONE
    else
    if (STR_EQ(cmd,"5"))
    {
        func_wl_stalist_iface("rai0", argc, argv);
    }
#endif
    else
    {
        func_wl_stalist_iface(cmd, argc, argv);
    }

    printf("\n");
    return 0;
}
