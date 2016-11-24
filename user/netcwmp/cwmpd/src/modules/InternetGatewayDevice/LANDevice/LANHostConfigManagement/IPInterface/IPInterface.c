/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/LANDevice/LANHostConfigManagement/IPInterface/IPInterface.c
 */

int
cpe_get_lhcm_ipi_addr_type(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    int dhcp = 0;

    DM_TRACE_GET();
    dhcp = cwmp_nvram_get_int("dhcpEnabled", 0);

    if (dhcp) {
        *value = "DHCP";
    } else {
        *value = "Static";
    }

    return FAULT_CODE_OK;
}

int
cpe_set_lhcm_ipi_addr_type(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    if (!strcmp("DHCP", value)) {
        cwmp_nvram_set("dhcpEnabled", "1");
    } else if (!strcmp("Static", value)) {
        cwmp_nvram_set("dhcpEnabled", "0");
    } else {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }

    return FAULT_CODE_OK;
}

