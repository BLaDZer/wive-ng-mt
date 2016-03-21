int cpe_get_igd_wan_ppp_servicename(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{

    char* sername =  cwmp_nvram_pool_get(pool, "vpnService");
    if (sername == NULL) 
    {
	*value = pool_pstrdup(pool, "");
	return FAULT_CODE_OK;
    }

    *value = sername;

    return FAULT_CODE_OK;
}



int cpe_get_igd_wan_ppp_authprot(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{

    int authProt =  cwmp_nvram_get_int("vpnAuthProtocol", 0);

    char* stdstr;

    switch (authProt) {

	case 1: stdstr = "PAP";break;
	case 2: stdstr = "CHAP";break;
	case 3: stdstr = "MS-CHAP";break;
	default: stdstr = "AUTO";break;
    }
    
    *value = pool_pstrdup(pool, stdstr);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wan_ppp_authprot(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    char* valStr = "0";

    if (strcmp(value,"PAP") == 0) valStr = "1"; else
    if (strcmp(value,"CHAP") == 0) valStr = "2"; else
    if (strcmp(value,"MS-CHAP") == 0) valStr = "3";

    cwmp_nvram_set("vpnAuthProtocol", valStr);

    return FAULT_CODE_OK;
}
