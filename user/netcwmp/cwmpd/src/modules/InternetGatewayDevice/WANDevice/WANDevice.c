int cpe_get_igd_wan_ip(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    /* copied from cpe_get_igd_ms_connectionrequesturl() */
    char* local_ip = NULL;

    DM_TRACE_GET();
    local_ip = getIntIp(pool);

    if (local_ip == 0) {
        cpe_get_localip("br0", local_ip);
    }

    if (local_ip == 0) {
        local_ip = cwmp_nvram_pool_get(cwmp->pool, "wan_ipaddr");
    }

    if (local_ip == 0) {
        local_ip = cwmp_nvram_pool_get(cwmp->pool, "lan_ipaddr");
    }

    if (local_ip == 0) {
        cwmp_log_error("Incorrect local ip");
        return FAULT_CODE_9002;
    }

    *value = PSTRDUP(local_ip);
    return FAULT_CODE_OK;
}

int cpe_set_igd_wan_ip(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    cwmp_nvram_set("wan_ipaddr",value);

    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_wandevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    DM_TRACE_REFRESH();

    if(!param_node)
    {
        return FAULT_CODE_9002;
    }
    parameter_node_t * tmp_param, *tmp_node, *child_param;
    child_param = param_node->child;
    if(child_param)
    {
        for(tmp_param=child_param->next_sibling; tmp_param; )
        {
            cwmp_log_info("refresh WANDevice node, delete param %s", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        parameter_node_t * wan1_param;
        cwmp_model_copy_parameter(param_node, &wan1_param, 1);

        parameter_node_t * wan2_param;
        cwmp_model_copy_parameter(param_node, &wan2_param, 2);

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    }

    return FAULT_CODE_OK;
}
