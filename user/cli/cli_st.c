/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "cli_st.h"

int getCurrentTimeStr(char* timeStr)
{
    struct tm *utime;
    time_t usecs;

    time(&usecs);
    utime = localtime(&usecs);
    if (utime == NULL)
        return 1;

    sprintf(timeStr, "%02u:%02u:%02u %02u.%02u.%04u",utime->tm_hour, utime->tm_min, utime->tm_sec, utime->tm_mday, (utime->tm_mon + 1), (1900 + utime->tm_year));

    return 0;
}


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

int func_st_l2tp(int argc, char* argv[])
{
    int l2tp_enabled = nvram_get_int(RT2860_NVRAM, "l2tp_srv_enabled", 0);
    char* ip_local = nvram_get(RT2860_NVRAM, "l2tp_srv_ip_local");
    char* ip_range = nvram_get(RT2860_NVRAM, "l2tp_srv_ip_range");

    int l2tp_nat  = nvram_get_int(RT2860_NVRAM, "l2tp_srv_nat_enabled", 0);
    int l2tp_mppe = nvram_get_int(RT2860_NVRAM, "l2tp_srv_mppe_enabled", 0);
    int l2tp_lcp_adapt = nvram_get_int(RT2860_NVRAM, "l2tp_srv_lcp_adapt", 0);
    int l2tp_debug = nvram_get_int(RT2860_NVRAM, "l2tp_srv_debug", 0);
    int l2tp_proxyarp = nvram_get_int(RT2860_NVRAM, "l2tp_srv_proxyarp", 0);


    writeHeader("L2TP");

    printf("Status:                  %s \n", l2tp_enabled?"enabled":"disabled");
    printf("Local IP to use:         %s \n", ip_local);
    printf("Allocate from IP range:  %s \n", ip_range);

    printf("\n");

    printf("Adaptive LCP:            %s \n", l2tp_lcp_adapt?"yes":"no");
    printf("Enable NAT:              %s \n", l2tp_nat?"yes":"no");
    printf("Allow MPPE:              %s \n", l2tp_mppe?"yes":"no");
    printf("Allow Debug:             %s \n", l2tp_debug?"yes":"no");
    printf("Proxyarp:                %s \n", l2tp_proxyarp?"yes":"no");

    printf("\n");
    return 0;
}

int func_st_ntp(int argc, char* argv[])
{
    int ntp_enabled = nvram_get_int(RT2860_NVRAM, "NTPEnabled", 0);
    char* tz = nvram_get(RT2860_NVRAM, "TZ");
    char* ntp_server = nvram_get(RT2860_NVRAM, "NTPServerIP");

    writeHeader("NTP");

    printf("Status:                  %s \n", ntp_enabled?"enabled":"disabled");
    printf("Timezone:                %s \n", tz);
    printf("Server IP:               %s \n", ntp_server);

    printf("\n");
    return 0;
}

int func_st_ddns(int argc, char* argv[])
{
    char* ddns_provider = nvram_get(RT2860_NVRAM, "DDNSProvider");

    writeHeader("Dynamic DNS");
    printf("DDNS Provider:           %s \n", ddns_provider[0]?ddns_provider:"None");

    printf("\n");
    return 0;
}

int func_st_samba(int argc, char* argv[])
{
    int smb_enabled = nvram_get_int(RT2860_NVRAM, "SmbEnabled", 0);
    char* smb_netbios = nvram_get(RT2860_NVRAM, "SmbNetBIOS");
    char* smb_workgroup = nvram_get(RT2860_NVRAM, "WorkGroup");
    char* smb_serverstring = nvram_get(RT2860_NVRAM, "SmbString");
    int smb_oslevel = nvram_get_int(RT2860_NVRAM, "SmbOsLevel", 255);
    int smb_ts_enabled = nvram_get_int(RT2860_NVRAM, "SmbTimeserver", 0);

    writeHeader("Samba / WINS");

    printf("Status:                  %s \n", smb_enabled?"enabled":"disabled");

    printf("Workgroup:               %s \n", smb_workgroup);
    printf("Netbios name:            %s \n", smb_netbios);
    printf("Server string:           %s \n", smb_serverstring);
    printf("OS Level:                %i \n", smb_oslevel);
    printf("SMB Timeserver:          %s \n", smb_ts_enabled?"enabled":"disabled");

    printf("\n");
    return 0;
}

int func_st_dhcp(int argc, char* argv[])
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

int func_st_vpn(int argc, char* argv[])
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

int func_st_show(int argc, char* argv[])
{
    char timeStr[40];
    int opmode = nvram_get_int(RT2860_NVRAM, "OperationMode", -1);
    long uptime = getSystemUptime();
    struct mem_stats mem;
    struct cpu_stats cpu;

    writeHeader("Device Status");

    printf("Firmware version:       %s \n", VERSIONPKG);

    if (getCurrentTimeStr(timeStr) == 0)
    {
        printf("System time:            %s \n", timeStr);
    }

    if (uptime > 0 && (secondsToTimeIntervalStr(timeStr, sizeof(timeStr), uptime) == 0))
    {
        printf("Uptime:                 %s \n", timeStr);
    }


    printf("Operation Mode:         ", timeStr);
    switch (opmode)
    {
        case 0:
                printf("AP-Bridge \n");
            break;

        case 1:
                printf("AP-Gateway \n");
            break;

        case 2:
                printf("Client-Gateway \n");
            break;

        case 3:
                printf("Client-AP-Gateway \n");
            break;

        default:
                printf("Unknown");
    }

    printf("\n");

    if (getMemData(&mem) == 0)
    {
        printf("Memory total:           %lu \n", mem.total);
        printf("Memory free:            %lu \n", mem.free );
    }


    if (getCPUData(&cpu) == 0)
    {
        printf("CPU usage:              %.1f %% \n", cpu.busy*100.0f/cpu.total);
    }

    char* argv_empty[0];

    func_sw_lan(0, argv_empty);
    func_sw_ipv6(0, argv_empty);
    func_sw_wan(0, argv_empty);
    func_wl_status(0, argv_empty);

    printf("\n");
    return 0;
}


/* status functions */
int func_st(int argc, char* argv[])
{
    char* cmd = NULL;

    if (argc>0)
    {
	cmd = argv[0];
        argc--;
        argv++;
    }

    if (STR_EQ(cmd, "show") || STR_EQ(cmd, "device"))
    {
        return func_st_show(argc, argv);
    }
    else
    if (STR_EQ(cmd, "dhcp"))
    {
        return func_st_dhcp(argc, argv);
    }
    else
    if (STR_EQ(cmd, "l2tp"))
    {
        return func_st_l2tp(argc, argv);
    }
    else
    if (STR_EQ(cmd, "ntp"))
    {
        return func_st_ntp(argc, argv);
    }
    else
    if (STR_EQ(cmd, "ddns"))
    {
        return func_st_ddns(argc, argv);
    }
    else
    if (STR_EQ(cmd, "smb") || STR_EQ(cmd, "samba"))
    {
        return func_st_samba(argc, argv);
    }
    else
    if (STR_EQ(cmd, "vpn"))
    {
        return func_st_vpn(argc, argv);
    }
    else
    {
        writeCmdHelp("show","general device information");
        writeCmdHelp("dhcp","dhcp info");
        writeCmdHelp("l2tp","L2TP Server info");
        writeCmdHelp("ntp","Network time synchronization info");
        writeCmdHelp("ddns","Dynamic DNS info");
        writeCmdHelp("samba","Samba server settings");
        writeCmdHelp("vpn","vpn info");
        printf("\n");
    }


    return 0;

}