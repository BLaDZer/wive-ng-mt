#include "cwmp/model.h"
#include "data_model.h"
#include "cwmp_module.h"
#include "InternetGatewayDevice/InternetGatewayDevice.c"

model_func_t ModelFunction[] =
{
    {"cpe_get_igd_di_manufacturer", cpe_get_igd_di_manufacturer},
    {"cpe_get_igd_di_manufactureroui", cpe_get_igd_di_manufactureroui},
    {"cpe_get_igd_di_productclass", cpe_get_igd_di_productclass},
/*    {"cpe_get_igd_di_serialnumber", cpe_get_igd_di_serialnumber},
    {"cpe_get_igd_di_specversion", cpe_get_igd_di_specversion},
    {"cpe_get_igd_di_hardwareversion", cpe_get_igd_di_hardwareversion},
    {"cpe_get_igd_di_softwareversion", cpe_get_igd_di_softwareversion},
*/
    {"cpe_get_igd_di_provisioningcode", cpe_get_igd_di_provisioningcode},
    {"cpe_get_igd_di_devicelog", cpe_get_igd_di_devicelog},

    {"cpe_get_igd_ms_username", cpe_get_igd_ms_username},
    {"cpe_get_igd_ms_password", cpe_get_igd_ms_password},
    {"cpe_get_igd_ms_connectionrequesturl", cpe_get_igd_ms_connectionrequesturl},
    {"cpe_get_igd_ms_connectionrequestusername", cpe_get_igd_ms_connectionrequestusername},
    {"cpe_get_igd_ms_connectionrequestpassword", cpe_get_igd_ms_connectionrequestpassword},
    {"cpe_set_igd_ms_connectionrequestusername", cpe_set_igd_ms_connectionrequestusername},
    {"cpe_set_igd_ms_connectionrequestpassword", cpe_set_igd_ms_connectionrequestpassword},

    {"cpe_get_igd_lan_igmp_enabled", cpe_get_igd_lan_igmp_enabled},
    {"cpe_set_igd_lan_igmp_enabled", cpe_set_igd_lan_igmp_enabled},
    {"cpe_get_igd_lan_igmp_version", cpe_get_igd_lan_igmp_version},
    {"cpe_set_igd_lan_igmp_version", cpe_set_igd_lan_igmp_version},

    {"cpe_get_igd_lan_hcm_dhcpenable", cpe_get_igd_lan_hcm_dhcpenable},
    {"cpe_set_igd_lan_hcm_dhcpenable", cpe_set_igd_lan_hcm_dhcpenable},

    {"cpe_get_igd_lan_wlan_standard", cpe_get_igd_lan_wlan_standard},
    {"cpe_set_igd_lan_wlan_standard", cpe_set_igd_lan_wlan_standard},

    {"cpe_get_igd_lan_wlan_channel", cpe_get_igd_lan_wlan_channel},
    {"cpe_set_igd_lan_wlan_channel", cpe_set_igd_lan_wlan_channel},

    {"cpe_get_igd_lan_wlan_autochannel", cpe_get_igd_lan_wlan_autochannel},
    {"cpe_set_igd_lan_wlan_autochannel", cpe_set_igd_lan_wlan_autochannel},

    {"cpe_get_igd_lan_wlan_basicauthmode", cpe_get_igd_lan_wlan_basicauthmode},
    {"cpe_set_igd_lan_wlan_basicauthmode", cpe_set_igd_lan_wlan_basicauthmode},

    {"cpe_get_igd_lan_wlan_wpaauthmode", cpe_get_igd_lan_wlan_wpaauthmode},
    {"cpe_set_igd_lan_wlan_wpaauthmode", cpe_set_igd_lan_wlan_wpaauthmode},

    {"cpe_get_igd_lan_wlan_ieeeauthmode", cpe_get_igd_lan_wlan_ieeeauthmode},
    {"cpe_set_igd_lan_wlan_ieeeauthmode", cpe_set_igd_lan_wlan_ieeeauthmode},

    {"cpe_get_igd_lan_wlan_beacontype", cpe_get_igd_lan_wlan_beacontype},
    {"cpe_set_igd_lan_wlan_beacontype", cpe_set_igd_lan_wlan_beacontype},

    {"cpe_get_igd_lan_wlan_bssid", cpe_get_igd_lan_wlan_bssid},

    {"cpe_get_igd_wan_ppp_authprot", cpe_get_igd_wan_ppp_authprot},
    {"cpe_set_igd_wan_ppp_authprot", cpe_set_igd_wan_ppp_authprot},

    {"cpe_get_igd_wan_ppp_servicename", cpe_get_igd_wan_ppp_servicename},

//    {"cpe_get_igd_services_iptv_igmpversion", cpe_get_igd_services_iptv_igmpversion},
//    {"cpe_set_igd_services_iptv_igmpversion", cpe_set_igd_services_iptv_igmpversion},

    {"cpe_refresh_igd_wandevice", cpe_refresh_igd_wandevice},
    {"cpe_refresh_igd_wanconnectiondevice", cpe_refresh_igd_wanconnectiondevice},
    {"cpe_refresh_igd_wanipconnection", cpe_refresh_igd_wanipconnection},

    {"cpe_get_igd_wan_ip_addrtype", cpe_get_igd_wan_ip_addrtype},
    {"cpe_set_igd_wan_ip_addrtype", cpe_set_igd_wan_ip_addrtype},

    {"cpe_get_igd_wan_ip_rxtxbytes", cpe_get_igd_wan_ip_rxtxbytes},

    {"cpe_get_igd_l3f_defaultconnection", cpe_get_igd_l3f_defaultconnection},
    {"cpe_set_igd_l3f_defaultconnection", cpe_set_igd_l3f_defaultconnection},

    {"cpe_get_conf_string", cpe_get_conf_string},
    {"cpe_set_conf_string", cpe_set_conf_string},

    {"cpe_get_nvram_string", cpe_get_nvram_string},
    {"cpe_set_nvram_string", cpe_set_nvram_string},

    {"cpe_get_nvram_bool", cpe_get_nvram_bool},
    {"cpe_set_nvram_bool", cpe_set_nvram_bool},

    {"cpe_get_nvram_bool_onoff", cpe_get_nvram_bool_onoff},
    {"cpe_set_nvram_bool_onoff", cpe_set_nvram_bool_onoff},

    {"cpe_get_nvram_int", cpe_get_nvram_int},
    {"cpe_set_nvram_int", cpe_set_nvram_int},

//    {"cpe_set_nvram_bool", cpe_set_nvram_bool},

    {"cpe_get_const_string", cpe_get_const_string},

    {"cpe_set_null", cpe_set_null},
    {"cpe_add_null", cpe_add_null},
};

int get_index_after_paramname(parameter_node_t * param, const char * tag_name)
{
    parameter_node_t * parent;
    parameter_node_t * tmp;
    for(parent=param->parent, tmp = param; parent; tmp = parent, parent = parent->parent)
    {
        if(TRstrcmp(parent->name, tag_name) == 0)
        {
             if(is_digit(tmp->name) == 0)
             {
                return TRatoi(tmp->name);   
             }
        }
    }
    return -1;
}


void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile)
{  

    cwmp_model_load_xml(cwmp, xmlfile, ModelFunction, sizeof(ModelFunction)/sizeof(model_func_t));
}

//constant string getter
int cpe_get_const_string(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    *value = pool_pstrdup(pool, args);
    return FAULT_CODE_OK;
}

//config string getter
int cpe_get_conf_string(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    char* nvval = cwmp_conf_pool_get(pool, args);
    if (nvval == NULL) {
        cwmp_log_error("cpe_get_conf_string: undefined param (%s)!",args);
	return FAULT_CODE_9002;
    }

    *value = nvval;
//    cwmp_log_error("cpe_get_conf_string: value is %s",*value);
    return FAULT_CODE_OK;
}

//conf string setter
int cpe_set_conf_string(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    if (value == NULL) 
    {
        cwmp_log_error("cpe_set_conf_string: param (%s) value is NULL", name);
	return FAULT_CODE_9002;
    }

    cwmp_conf_set(args,value);
    return FAULT_CODE_OK;
}



//nvram string getter
int cpe_get_nvram_string(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    char* nvval = cwmp_nvram_pool_get(pool, args);
    if (nvval == NULL) {
        cwmp_log_error("cpe_get_nvram_string: undefined param (%s)!",args);
	return FAULT_CODE_9002;
    }

    *value = nvval;
//    cwmp_log_error("cpe_get_nvram_string: value is %s",*value);
    return FAULT_CODE_OK;
}

//nvram bool getter
int cpe_get_nvram_bool(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* nvval = cwmp_nvram_pool_get(pool, args);
    if (nvval == NULL) {
    cwmp_log_error("cpe_get_nvram_bool: undefined param (%s)!",args);
	return FAULT_CODE_9002;
    }

    int val = (nvval[0] == '1');
    const char* valStr = val?"1":"0";

    *value = pool_pstrdup(pool, valStr);
//    cwmp_log_error("cpe_get_igd_lan_hcm_dhcpenabled: value is %s", *value);
//    cwmp_log_error("cpe_get_nvram_string: value is %s",*value);
    return FAULT_CODE_OK;

}

//nvram bool getter
int cpe_get_nvram_bool_onoff(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{

    FUNCTION_TRACE();

    const char* nvval = cwmp_nvram_pool_get(pool, args);
    if (nvval == NULL) {
	cwmp_log_error("cpe_get_nvram_bool: undefined param (%s)!",args);
	return FAULT_CODE_9002;
    }

    if (nvval[0] == '\0') {
	cwmp_log_error("cpe_get_nvram_bool: zero param (%s)!",args);
	return FAULT_CODE_9002;
    }

    int val = (nvval[0] == 'o' && nvval[1] == 'n');
    const char* valStr = val?"1":"0";

    *value = pool_pstrdup(pool, valStr);
//    cwmp_log_error("cpe_get_igd_lan_hcm_dhcpenabled: value is %s", *value);
//    cwmp_log_error("cpe_get_nvram_string: value is %s",*value);
    return FAULT_CODE_OK;

}




//nvram bool getter
int cpe_get_nvram_int(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* nvval = cwmp_nvram_pool_get(pool, args);
    if (nvval == NULL) 
    {
	cwmp_log_error("cpe_get_nvram_bool: undefined param (%s)!",args);
	return FAULT_CODE_9002;
    }

    long val = strtol(nvval, NULL, 10);
    char valStr[256];// = pool_palloc(pool, strlen(val));
    snprintf(&valStr[0], 256, "%li",val);
    

    *value = pool_pstrdup(pool, &valStr);
//    cwmp_log_error("cpe_get_igd_lan_hcm_dhcpenabled: value is %s", *value);
//    cwmp_log_error("cpe_get_nvram_string: value is %s",*value);
    return FAULT_CODE_OK;

}



//nvram string setter
int cpe_set_nvram_string(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    cwmp_log_error("DEBUG2: cpe_set_nvram_string: %s %s %s", name, value, args);
    if (value == NULL) 
    {
        cwmp_log_error("cpe_set_nvram_string: param (%s) value is NULL", name);
	return FAULT_CODE_9002;
    }

    cwmp_nvram_set(args,value);
    return FAULT_CODE_OK;
}

//nvram int setter
int cpe_set_nvram_int(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    if (value == NULL) 
    {
        cwmp_log_error("cpe_set_nvram_string: param (%s) value is NULL", name);
	return FAULT_CODE_9002;
    }

    long val = strtol(value, NULL, 10);
    char valStr[256];
    snprintf(&valStr[0], 256, "%li",val);

    cwmp_nvram_set(args,valStr);
    return FAULT_CODE_OK;
}


//nvram bool setter
int cpe_set_nvram_bool(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    if (value == NULL) 
    {
        cwmp_log_error("cpe_set_nvram_string: param (%s) value is NULL", name);
	return FAULT_CODE_9002;
    }

    long val = strtol(value, NULL, 10);
    cwmp_nvram_set(args,val?"1":"0");
    return FAULT_CODE_OK;
}

//nvram bool setter
int cpe_set_nvram_bool_onoff(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    if (value == NULL) 
    {
        cwmp_log_error("cpe_set_nvram_string: param (%s) value is NULL", name);
	return FAULT_CODE_9002;
    }

    long val = strtol(value, NULL, 10);
    if (val > 1) val = 1;
    if (val < 0) val = 0;
    char valStr[4] = {0};

    strcpy(&valStr[0], val?"on":"off");
//    snprintf(&valStr, 256, "%li",val);

    cwmp_nvram_set(args,valStr);
    return FAULT_CODE_OK;
}


int cpe_set_null(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    return FAULT_CODE_OK;
}

int cpe_add_null(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
    *pinstance_number = 1;

    return FAULT_CODE_OK;
}