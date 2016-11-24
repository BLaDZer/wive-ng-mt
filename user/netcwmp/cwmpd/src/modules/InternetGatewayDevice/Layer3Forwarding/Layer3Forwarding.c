

int cpe_get_igd_l3f_defaultconnection(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    int isvpn = 0;
    char* val = NULL;

    DM_TRACE_GET();

    isvpn = cwmp_nvram_get_int("vpnDGW", 0);
    val = (isvpn!=0) ?
        "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.2" :
        "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1";

    *value = pool_pstrdup(pool, val);

    return FAULT_CODE_OK;
}

int cpe_set_igd_l3f_defaultconnection(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    int isvpn = 0;

    DM_TRACE_SET();
    isvpn |= strcmp(value, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.2") == 0;
    isvpn |= strcmp(value, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.2.") == 0;
    isvpn |= strcmp(value, "1") == 0;

    cwmp_nvram_set("vpnDGW", isvpn?"1":"0");

    return FAULT_CODE_OK;
}
