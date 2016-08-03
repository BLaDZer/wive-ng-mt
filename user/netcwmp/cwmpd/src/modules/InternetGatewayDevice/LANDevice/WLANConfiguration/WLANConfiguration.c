//FIXME: Multichannel auth functions!

BOOL prefix(const char *str, const char *pre)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

int cpe_get_igd_lan_wlan_bssid(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    cwmp_log_debug("DEBUG: cpe_get_igd_lan_wlan_bssid\n");
    char if_hw[18] = {0};

    char* ifstart = cwmp_nvram_pool_get(pool,"BssidIfName");
    char ifname[20] = {0};
    strcat(ifname,ifstart);
    strcat(ifname,args);

    if (getIfMac(ifname, if_hw, ':') == -1) *value = pool_pstrdup(pool,"00:00:00:00:00:00");
    else *value = pool_pstrdup(pool,if_hw);

    cwmp_log_debug("DEBUG cpe_get_igd_lan_wlan_bssid: BSSID%s %s \n",args,*value);

    return FAULT_CODE_OK;
}


int cpe_get_igd_lan_wlan_autochannel(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    cwmp_log_debug("DEBUG: cpe_get_igd_lan_wlan_autochannel\n");
    int chan =  cwmp_nvram_get_int("Channel", 0);
    int autoselect = cwmp_nvram_get_int("AutoChannelSelect",1);
    
    *value = pool_pstrdup(pool, ((chan==0) || autoselect)?"1":"0");

    return FAULT_CODE_OK;

}

int cpe_set_igd_lan_wlan_autochannel(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_autochannel\n");
    if (value == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_autochannel: undefined value!");
	return FAULT_CODE_9002;
    }

    if (value[0] == '1') {
	cwmp_nvram_set("Channel","0");
	cwmp_nvram_set("AutoChannelSelect","1");
    }
    else
    {
        int chan =  cwmp_nvram_get_int("Channel", 0);
	cwmp_nvram_set("AutoChannelSelect","0");
	if (chan == 0) cwmp_nvram_set("Channel", "9");
    }

    return FAULT_CODE_OK;
}

int cpe_get_igd_lan_wlan_channel(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    cwmp_log_debug("DEBUG: cpe_get_igd_lan_wlan_channel\n");
    char* chan =  cwmp_nvram_pool_get(pool, "Channel");
    if (chan == NULL) 
    {
	chan = "0";
    }

    int autoselect = cwmp_nvram_get_int("AutoChannelSelect",1);

    if (autoselect) 
    {
	*value = pool_pstrdup(pool, "0");
    }
    else
    {
	*value = pool_pstrdup(pool, chan);
    }
    

    return FAULT_CODE_OK;

}

int cpe_set_igd_lan_wlan_channel(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_channel\n");

    if (value == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_channel: undefined value!");
	return FAULT_CODE_9002;
    }

    cwmp_nvram_set("Channel",value);

    if (value[0] == '0' && value[1] == '\0') {
	cwmp_nvram_set("AutoChannelSelect", "1");
    }
    else
    {
	cwmp_nvram_set("AutoChannelSelect", "0");
    }

    return FAULT_CODE_OK;
}


int cpe_get_igd_lan_wlan_standard(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    cwmp_log_debug("DEBUG: cpe_get_igd_lan_wlan_standard\n");
    int standard =  cwmp_nvram_get_int("WirelessMode", 9);

    char* stdstr;

    switch (standard) {

	case 0: stdstr = "b/g/n";break;
	case 1: stdstr = "b";break;
	case 4: stdstr = "g";break;
	case 6: stdstr = "n";break;
	case 7: stdstr = "b/g/n";break;
	case 9: stdstr = "b/g/n";break;
	default: stdstr = "b/g/n";break;
    }
    
    *value = pool_pstrdup(pool, stdstr);
    cwmp_log_debug("DEBUG: cpe_get_igd_lan_wlan_standard %s\n",stdstr);

    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_standard(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_standard\n");

    char* valStr = "9";

    if (strcmp(value, "n") == 0) valStr="6";else
    if (strcmp(value, "g") == 0) valStr="4";else
    if (strcmp(value, "b") == 0) valStr="1";

    cwmp_nvram_set("WirelessMode", valStr);
    
    cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_standard %s \n", valStr);

    return FAULT_CODE_OK;
}



//InternetGatewayDevice.LANDevice.WLANConfiguration.BasicAuthenticationMode
int cpe_get_igd_lan_wlan_basicauthmode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* authMode = cwmp_nvram_pool_get(pool, "AuthMode");
    if (authMode == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_basicauthmode: undefined AuthMode param!");
	return FAULT_CODE_9002;
    }

    char* valStr = prefix(authMode,"Disable;")?"None":"EAPAuthentication";


    *value = pool_pstrdup(pool, valStr);

    cwmp_log_debug("cpe_get_igd_lan_wlan_basicauthmode: value is %s", valStr);
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_basicauthmode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    if (strcmp(value,"None") == 0 ) cwmp_nvram_set("AuthMode", "Disable;Disable;Disable");

    return FAULT_CODE_OK;
}


int cpe_get_igd_lan_wlan_wpaauthmode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* authMode = cwmp_nvram_pool_get(pool, "AuthMode");
    if (authMode == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_wpaauthmode: undefined AuthMode param!");
	return FAULT_CODE_9002;
    }
    
    
    char* valStr = "EAPAuthentication";//prefix(authMode,"Disable;")?"None":"EAPAuthentication";
    if (prefix(authMode,"WPAPSK;")) valStr = "PSKAuthentication"; else
    if (prefix(authMode,"WPA2PSK;")) valStr = "PSKAuthentication"; else
    if (prefix(authMode,"WPAPSKWPA2PSK;")) valStr = "PSKAuthentication";

    *value = pool_pstrdup(pool, valStr);

    cwmp_log_debug("DEBUG: cpe_get_igd_lan_wlan_wpaauthmode value is %s \n", valStr);
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_wpaauthmode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    //FIXME: STUB

    cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_wpaauthmode\n");
    return FAULT_CODE_OK;
}

int cpe_get_igd_lan_wlan_ieeeauthmode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* authMode = cwmp_nvram_pool_get(pool, "AuthMode");
    if (authMode == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_ieeeauthmode: undefined AuthMode param!");
	return FAULT_CODE_9002;
    }

    char* valStr = "EAPAuthentication";//prefix(authMode,"Disable;")?"None":"EAPAuthentication";
    if (prefix(authMode,"WPAPSK;")) valStr = "PSKAuthentication";
    if (prefix(authMode,"WPA2PSK;")) valStr = "PSKAuthentication";
    if (prefix(authMode,"WPAPSKWPA2PSK;")) valStr = "PSKAuthentication";

    *value = pool_pstrdup(pool, valStr);

    cwmp_log_debug("cpe_get_igd_lan_wlan_ieeeauthmode: value is %s", valStr);
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_ieeeauthmode(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();
    //FIXME: STUB

    cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_ieeeauthmode\n");
    return FAULT_CODE_OK;
}


int cpe_get_igd_lan_wlan_beacontype(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    FUNCTION_TRACE();

    const char* authMode = cwmp_nvram_pool_get(pool, "AuthMode");
    if (authMode == NULL) {
	cwmp_log_error("cpe_get_igd_lan_wlan_beacontype: undefined AuthMode param!");
	return FAULT_CODE_9002;
    }

    char* valStr = "Basic";

    if (prefix(authMode,"WPA1WPA2;")) valStr = "WPAand11i"; else
    if (prefix(authMode,"WPAPSKWPA2PSK;")) valStr = "WPAand11i"; else
    if (prefix(authMode,"WPA2PSK;")) valStr = "11i"; else
    if (prefix(authMode,"WPA2;")) valStr = "11i"; else
    if (prefix(authMode,"WPA;")) valStr = "WPA"; else
    if (prefix(authMode,"WPAPSK;")) valStr = "WPA";


    *value = pool_pstrdup(pool, valStr);

    cwmp_log_debug("cpe_get_igd_lan_wlan_beacontype: value is %s", valStr);
    return FAULT_CODE_OK;
}

int cpe_set_igd_lan_wlan_beacontype(cwmp_t * cwmp, const char * name, const char * value, int length, callback_register_func_t callback_reg)
{
    FUNCTION_TRACE();

    if (value == 0) cwmp_log_debug("DEBUG: cpe_set_igd_lan_wlan_beacontype VALUE IS NULL\n");

    char* authStr = "WPAPSKWPA2PSK;WPAPSKWPA2PSK;WPAPSKWPA2PSK";
    
    if (strcmp(value,"WPAand11i") == 0) authStr = "WPAPSKWPA2PSK;WPAPSKWPA2PSK;WPAPSKWPA2PSK"; else
    if (strcmp(value,"11i") == 0) authStr = "WPA2PSK;WPA2PSK;WPA2PSK"; else
    if (strcmp(value,"WPA") == 0) authStr = "WPAPSK;WPAPSK;WPAPSK"; else
    if (strcmp(value,"Basic") == 0) authStr = "Disable;Disable;Disable";

    cwmp_nvram_set("AuthMode", authStr);

    cwmp_log_debug("cpe_set_igd_lan_wlan_beacontype: set value %s\n", authStr);
    return FAULT_CODE_OK;
}
