/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/User/User.c
 */

int
cpe_reload_user(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    DM_TRACE_RELOAD();
    /* copied from ${ROOTDIR}/user/goahead/src/management.c:setSysAdm */
    system("service pass start");
    system("service inetd restart");
    system("service samba restart");
    return FAULT_CODE_OK;
}

int
cpe_set_user_name(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    if (!*value) {
        cwmp_log_warn("%s: username length must be greater then zero", value);
        return FAULT_CODE_9003;
    }

    if (strchr(value, ':')) {
        cwmp_log_warn("%s: invalid username '%s'", __func__, value);
        return FAULT_CODE_9003;
    }

    cwmp_nvram_set(args, value);
    return FAULT_CODE_OK;
}

int
cpe_set_user_mngmt_enable(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg)
{
    char *login = NULL;
    char *passwd = NULL;

    DM_TRACE_SET();

    if (*value == '0') {
        /* disable */
        cwmp_nvram_set("MngmtPassword", "");
        return FAULT_CODE_OK;
    }

    login = cwmp_nvram_get("MngmtLogin");
    passwd = cwmp_nvram_get("MngmtPassword");

    if (!*login) {
        char buf[256] = {};
        snprintf(buf, sizeof(buf), "%s.mngmt", cwmp_nvram_get("Login"));
        cwmp_nvram_set("MngmtLogin", buf);
    }

    if (!*passwd) {
        cwmp_nvram_set("MngmtPassword", cwmp_nvram_get("Password"));
    }

    return FAULT_CODE_OK;
}

int
cpe_get_user_mngmt_enable(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char *login = NULL;
    char *passwd = NULL;

    DM_TRACE_GET();
    login = cwmp_nvram_get("MngmtLogin");
    passwd = cwmp_nvram_get("MngmtPassword");

    if (*login && *passwd) {
        *value = "1";
    } else {
        *value = "0";
    }

    return FAULT_CODE_OK;
}

