

int cpe_get_igd_l3f_defaultconnection(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{

    int isvpn =  cwmp_nvram_get_int("vpnDGW", 0);
    char* val = (isvpn!=0)?"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.2":"InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANIPConnection.1";
    cwmp_log_error("DEBUG2: cpe_get_igd_l3f_defaultconnection: value %s \n",val);

    *value = pool_pstrdup(pool, val);

    return FAULT_CODE_OK;
}

int cpe_set_igd_l3f_defaultconnection(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    int isvpn = 0;
    isvpn |= strcmp(value, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.2") == 0;
    isvpn |= strcmp(value, "InternetGatewayDevice.WANDevice.1.WANConnectionDevice.1.WANPPPConnection.2.") == 0;
    cwmp_nvram_set("vpnDGW", isvpn?"1":"0");

    return FAULT_CODE_OK;
}
