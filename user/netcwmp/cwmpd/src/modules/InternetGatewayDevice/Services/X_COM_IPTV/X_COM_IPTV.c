
//InternetGatewayDevice.Services.X_COM_IPTV.IGMPVersion
int cpe_get_igd_services_iptv_igmpversion(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();
    //FIXME
    *value = cwmp_conf_pool_get(pool, "cwmp:cpe_manufacture");
    cwmp_log_debug("cpe_get_igd_di_manufacturer: value is %s", *value);
    return	FAULT_CODE_OK;
}

int cpe_set_igd_services_iptv_igmpversion(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    return	FAULT_CODE_OK;
}
