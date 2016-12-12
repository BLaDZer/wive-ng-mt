
int cpe_reload_all(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    char p[INI_BUFFERSIZE] = {0};
    int pid = 0;

    DM_TRACE_RELOAD();
    cwmp_conf_get("cwmpd:reload_script", p);

    if (p[0] == '\0') {
        strcpy(p, "/etc/scripts/internet.sh");
    }

    if (*p) {
        if ((pid = fork()) == 0) {
            sleep(3);
            if (execl(p, p, (char*)NULL) == -1) {
                cwmp_log_error("exec(%s) error: %s", p, strerror(errno));
            }
        } else if(pid > 0) {
            cwmp_log_info("forked for exec(%s)", p);
        } else {
            cwmp_log_error("fork() failed: %s", strerror(errno));
        }
    }

    return FAULT_CODE_OK;
}

#include "User/User.c"
#include "Time/Time.c"
#include "WANDevice/WANConnectionDevice/PortMapping.c"
#include "IPPingDiagnostics/IPPingDiagnostics.c"
#include "UploadDiagnostics/UploadDiagnostics.c"
#include "DownloadDiagnostics/DownloadDiagnostics.c"
#include "TraceRouteDiagnostics/TraceRouteDiagnostics.c"
#include "DeviceInfo/DeviceInfo.c"
#include "ManagementServer/ManagementServer.c"
#include "QueueManagement/QueueManagement.c"
#include "WANDevice/WANDevice.c"
#include "WANDevice/WANConnectionDevice/WANConnectionDevice.c"
#include "WANDevice/WANConnectionDevice/WANIPConnection.c"
#include "WANDevice/WANConnectionDevice/WANPPPConnection.c"
#include "Layer3Forwarding/Layer3Forwarding.c"
#include "WANDevice/WANEthernetInterfaceConfig/WANEthernetInterfaceConfig.c"
#include "LANDevice/LANHostConfigManagement/IPInterface/IPInterface.c"
#include "LANDevice/LANDevice.c"
#include "LANDevice/X_COM_IgmpSnoopingConfig/X_COM_IgmpSnoopingConfig.c"
#include "LANDevice/LANHostConfigManagement/LANHostConfigManagement.c"
#include "LANDevice/WLANConfiguration/WLANConfiguration.c"
#include "LANDevice/WLANConfiguration/WLANConfiguration_dlink101.c"
#include "LANDevice/WLANConfiguration/WLANConfiguration_dlink115.c"
#include "LANDevice/LANEthernetInterfaceConfig/LANEthernetInterfaceConfig.c"
#include "LANDevice/Hosts/Hosts.c"
#include "LANDevice/LANHostConfigManagement/DHCPStaticAddress/DHCPStaticAddress.c"

#include "Services/Services.c"
#include "Services/X_COM_IPTV/X_COM_IPTV.c"
