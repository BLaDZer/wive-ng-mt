int cpe_get_igd_wan_ip_dnsservers(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();

    *value = cwmp_nvram_pool_get(pool, "wan_primary_dns");

    return FAULT_CODE_OK;
}

int cpe_set_igd_wan_ip_dnsservers(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    cwmp_nvram_set("wan_primary_dns", value);
    cwmp_nvram_set("wan_static_dns", "on");

    return FAULT_CODE_OK;
}


int cpe_set_igd_wan_ip_dnsenabled(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    if (value[0] == '1') cwmp_nvram_set("wan_static_dns", "off");

    return FAULT_CODE_OK;
}


int cpe_get_igd_wan_ip_rxtxbytes(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool) {
    struct port_counts pcs;
    int i;

    DM_TRACE_GET();

    portscounts(&pcs);

    for (i=0;i<6;i++) 
    {
        cwmp_log_debug("wan RX Count %i : %llu", i, pcs.rx_count[i]);
        cwmp_log_debug("wan TX Count %i : %llu", i, pcs.tx_count[i]);
    }

    int wan_port = cwmp_nvram_get_int("wan_port",0);
    if (wan_port>5) return FAULT_CODE_9002;

    char buf[24] = {0};

    if (args[0] == 'r')
    {
        snprintf(buf,sizeof(buf),"%llu", pcs.rx_count[4-wan_port]);
    }
    else
    {
        snprintf(buf,sizeof(buf),"%llu",pcs.tx_count[4-wan_port]);
    }

    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wan_ip_addrtype(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();

    const char* authType = cwmp_nvram_pool_get(pool, "wanConnectionMode");
    if (authType == NULL) {
        cwmp_log_error("cpe_get_igd_wan_ip_addrtype: undefined wanConnectionMode param!");
        return FAULT_CODE_9002;
    }

    char* tpstr = "Static";

    if (strcmp(authType,"STATIC") == 0) tpstr = "Static"; else
    if (strcmp(authType,"DHCP") == 0) tpstr = "DHCP"; else
    if (strcmp(authType,"ZERO") == 0) tpstr = "AutoIP";

    *value = pool_pstrdup(pool, tpstr);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wan_ip_addrtype(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    char* valStr;
    cwmp_log_debug("DEBUG: cpe_set_igd_wan_ip_addrtype: value is %s \n",value);

    switch (value[0]) {
        case 'S': valStr = "STATIC"; break;
        case 'D': valStr = "DHCP"; break;
        case 'A': valStr = "ZERO"; break;
        default: valStr = "STATIC";
    }

    cwmp_nvram_set("wanConnectionMode", valStr);

    return FAULT_CODE_OK;
}

int  cpe_refresh_igd_wanipconnection(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
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
            cwmp_log_debug("DEBUG: IP:refresh WANConnectionDevice node, delete param %s\n", tmp_param->name);
            tmp_node = tmp_param->next_sibling;
            cwmp_model_delete_parameter(tmp_param);
            tmp_param = tmp_node;
        }
        child_param->next_sibling = NULL;

        int wan_index = get_index_after_paramname(param_node, "WANDevice"); 
        int wan_conn_dev_index = get_index_after_paramname(param_node, "WANConnectionDevice"); 

        if(wan_index == 1)
        {
            parameter_node_t * ipconn_param;
            switch(wan_conn_dev_index)
            {
                case 1: 
                    cwmp_model_copy_parameter(param_node, &ipconn_param, 1);
                    break;

                case 2:
                    cwmp_model_copy_parameter(param_node, &ipconn_param, 1); 
                    cwmp_model_copy_parameter(param_node, &ipconn_param, 2);
                    break;
            }
        }
        else
        if(wan_index == 2)
        {
            //don't support
        }

        cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg); 
    }

    return FAULT_CODE_OK;
}

