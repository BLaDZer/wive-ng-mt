/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/QueueManagement/QueueManagement.c
 */

int
cpe_reload_qos(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    DM_TRACE_RELOAD();
    system("service shaper restart && service iptables restart && service kext restart");
    return FAULT_CODE_OK;
}

