
//InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPServerEnable
int cpe_get_igd_lan_hcm_dhcpenable(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* dhcpEnabled = cwmp_nvram_pool_get(pool, "dhcpEnabled");
    if (dhcpEnabled == NULL) {
    cwmp_log_error("cpe_get_igd_lan_hcm_dhcpenabled: undefined dhcpEnabled param!");
	return FAULT_CODE_9002;
    }

    int val = (dhcpEnabled[0] == '1');
    const char* valStr = val?"1":"0";

    *value = pool_pstrdup(pool, valStr);
    cwmp_log_error("cpe_get_igd_lan_hcm_dhcpenabled: value is %s", *value);
    return FAULT_CODE_OK;
}

//InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPServerEnable
int cpe_set_igd_lan_hcm_dhcpenable(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    cwmp_nvram_set("dhcpEnabled",value);
    return FAULT_CODE_OK;
}


/*
//InternetGatewayDevice.LANDevice.{i}.LANHostConfigManagement.DHCPLeaseTime
int cpe_get_igd_lan_hcm_dhcpleasetime(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    return cpe_get_nvram_int(cwmp, name, value, args, pool);
    

}

int cpe_set_igd_lan_hcm_dhcpleasetime(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{

}
*/