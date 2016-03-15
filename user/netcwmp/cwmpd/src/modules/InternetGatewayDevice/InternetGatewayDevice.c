#include "DeviceInfo/DeviceInfo.c"
#include "ManagementServer/ManagementServer.c"
#include "WANDevice/WANDevice.c"
#include "WANDevice/WANConnectionDevice/WANConnectionDevice.c"
#include "WANDevice/WANConnectionDevice/WANIPConnection.c"
#include "WANDevice/WANConnectionDevice/WANPPPConnection.c"

#include "Layer3Forwarding/Layer3Forwarding.c"

#include "LANDevice/LANDevice.c"
#include "LANDevice/X_COM_IgmpSnoopingConfig/X_COM_IgmpSnoopingConfig.c"
#include "LANDevice/LANHostConfigManagement/LANHostConfigManagement.c"
#include "LANDevice/WLANConfiguration/WLANConfiguration.c"


#include "Services/Services.c"
#include "Services/X_COM_IPTV/X_COM_IPTV.c"


char* cpe_get_igd_device_summary(void * arg, void * pool)
{
    //pool_t * p = (pool_t *)pool;
    return	NULL;
}

char* cpe_get_igd_lan_device_number_of_entries(void * arg, void * pool)
{
    //pool_t * p = (pool_t *)pool;
    return NULL;
}





