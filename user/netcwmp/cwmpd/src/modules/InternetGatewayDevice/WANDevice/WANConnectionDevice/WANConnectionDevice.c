int cpe_get_wan_elc_status(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    int wan_port = 0;
    struct port_status ps = {};

    DM_TRACE_GET();
    /* get WAN status */
    wan_port = 4 - cwmp_nvram_get_int("wan_port", 5);
    if (wan_port == -1) {
        return FAULT_CODE_9002;
    }
    if (portstatus(&ps, wan_port)) {
        *value = "Unavailable";
    } else {
        if (ps.link) {
            *value = "Up";
        } else {
            *value = "Down";
        }
    }
    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_wanconnectiondevice(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
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
            cwmp_log_debug("DEBUG: refresh WANConnectionDevice node, delete param %s", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        int wan_index = get_index_after_paramname(param_node, "WANDevice");
        parameter_node_t * wan1conn_param;
        cwmp_model_copy_parameter(param_node, &wan1conn_param, 1);
        if(wan_index == 2)
        {
             parameter_node_t * wan2conn_param;
             cwmp_model_copy_parameter(param_node, &wan2conn_param, 2);
        }

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);
    }

    return FAULT_CODE_OK;
}


