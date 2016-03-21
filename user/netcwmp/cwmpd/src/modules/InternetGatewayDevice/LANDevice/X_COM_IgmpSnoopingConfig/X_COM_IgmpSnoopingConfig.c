
//InternetGatewayDevice.LANDevice.X_COM_IgmpSnoopingConfig.Enabled
int cpe_get_igd_lan_igmp_enabled(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();
    int igmp_enabled = cwmp_nvram_get_int("igmpEnabled", 0);

    *value = pool_pstrdup(pool, igmp_enabled?"1":"0");
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_igmp_enabled(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    cwmp_nvram_set("igmpEnabled", (value[0]=='0')?"0":"1");

    return FAULT_CODE_OK;
}

int cpe_get_igd_lan_igmp_version(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{

    FUNCTION_TRACE();
    int igmp_enabled = cwmp_nvram_get_int("igmpEnabled", 0);

    *value = pool_pstrdup(pool, igmp_enabled?"2":"0");
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_igmp_version(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    cwmp_nvram_set("igmpEnabled", (value[0]=='0')?"0":"1");
    return FAULT_CODE_OK;
}




/*
int cpe_get_igd_lan_wlan_standard(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{

    const char* authMode = cwmp_nvram_pool_get(pool, "WirelessMode");
    if (authMode == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_standard: undefined WirelessMode param!");
	return FAULT_CODE_9002;
    }

    int standard =  cwmp_nvram_get_int("WirelessMode");

    char* stdstr;

    switch (standard) {

	case 0: stdstr = "g";break;
	case 1: stdstr = "b";break;
	case 4: stdstr = "g";break;
	case 6: stdstr = "n";break;
	case 7: stdstr = "n";break;
	case 9: stdstr = "b/g/n";break;
	default: stdstr = "b/g/n";break;
    }
    
    *value = pool_pstrdup(pool, stdstr);

    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_standard(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    
    

    return FAULT_CODE_OK;
}
*/