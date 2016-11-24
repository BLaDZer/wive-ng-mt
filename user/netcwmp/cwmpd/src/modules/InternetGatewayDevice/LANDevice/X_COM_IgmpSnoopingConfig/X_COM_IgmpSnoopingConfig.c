
//InternetGatewayDevice.LANDevice.X_COM_IgmpSnoopingConfig.Enabled
int cpe_get_igd_lan_igmp_enabled(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();

    int igmp_enabled = 0;
    igmp_enabled = cwmp_nvram_get_int("igmpEnabled", 0);
    *value = pool_pstrdup(pool, igmp_enabled?"1":"0");
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_igmp_enabled(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    cwmp_nvram_set("igmpEnabled", (value[0]=='0')?"0":"1");
    return FAULT_CODE_OK;
}

int cpe_get_igd_lan_igmp_version(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();

    int igmp_enabled = 0;
    igmp_enabled = cwmp_nvram_get_int("igmpEnabled", 0);
    *value = pool_pstrdup(pool, igmp_enabled?"2":"0");
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_igmp_version(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    cwmp_nvram_set("igmpEnabled", (value[0]=='0')?"0":"1");
    return FAULT_CODE_OK;
}

