int cpe_set_igd_wlanc_standard_dlink101(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    char* valStr = "9";

    if (strcmp(value, "n") == 0) valStr="6";else
    if (strcmp(value, "g") == 0) valStr="4";else
    if (strcmp(value, "b") == 0) valStr="1";

    cwmp_nvram_set("WirelessMode", valStr);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_standard_dlink101(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char* stdstr;

    DM_TRACE_GET();

    int standard = cwmp_nvram_get_int("WirelessMode", 9);

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

    return FAULT_CODE_OK;
}
