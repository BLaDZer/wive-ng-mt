int cpe_set_igd_wlanc_standard_dlink115(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    char* valStr = "9";

    if (strcmp(value, "n") == 0) valStr="9";else
    if (strcmp(value, "g") == 0) valStr="0";else
    if (strcmp(value, "g-only") == 0) valStr="4";else
    if (strcmp(value, "n-only") == 0) valStr="6";else
    if (strcmp(value, "b") == 0) valStr="1";

    cwmp_nvram_set("WirelessMode", valStr);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_standard_dlink115(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char* stdstr;

    DM_TRACE_GET();

    int standard = cwmp_nvram_get_int("WirelessMode", 9);

    switch (standard) {

        case 0: stdstr = "g";break;
        case 1: stdstr = "b";break;
        case 4: stdstr = "g-only";break;
        case 6: stdstr = "n-only";break;
        case 7: stdstr = "n";break;
        case 9: stdstr = "n";break;
        default: stdstr = "n";break;
    }

    *value = pool_pstrdup(pool, stdstr);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_dlink115_wparenewal(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned id = -1u;
    int rekey_int;
    char rekey_str[16] = {0};

    DM_TRACE_SET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

// value check
    rekey_int = strToIntDef((char*)value, 3600);
    snprintf(rekey_str, sizeof(rekey_str)-1,"%i", rekey_int);

    nvram_set_tuple("RekeyInterval", id, rekey_str);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_dlink115_wparenewal(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    char v[16] = {0};

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    nvram_get_tuple("RekeyInterval", id, v, sizeof(v));
    *value = pool_pstrdup(pool, v);

    return FAULT_CODE_OK;
}
