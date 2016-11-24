/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/Time/Time.c
 */

int
cpe_get_time_status(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char *v = NULL;

    DM_TRACE_GET();
    v = cwmp_nvram_get("NTPEnabled");

    if (*v == '1' || *v == 't') {
        *value = "Synchronized";
        return FAULT_CODE_OK;
    } else {
        *value = "Disabled";
        return FAULT_CODE_OK;
    }

    return FAULT_CODE_OK;
}

int
cpe_get_time_localtime(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char tz[6] = {};
    char f[42] = {};
    struct tm *tm = NULL;
    time_t t = 0u;
    size_t len = 0u;

    DM_TRACE_GET();

    t = time(NULL);
    tm = localtime(&t);

    strftime(tz, sizeof(tz), "%z", tm);
    len = strftime(f, sizeof(f), "%Y-%m-%dT%H:%M:%S", tm);

    len += (sizeof(tz) + 3);
    *value = pool_palloc(pool, len);
    if (!*value) {
        return FAULT_CODE_9002;
    }

    snprintf(*value, len, "%s%.3s%.2s", f, tz, &tz[3]);

    return FAULT_CODE_OK;
}

int
cpe_get_time_zonename(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    struct tm *tm = NULL;
    time_t t = 0u;

    const size_t len = 20;

    DM_TRACE_GET();

    t = time(NULL);
    tm = localtime(&t);

    *value = pool_palloc(pool, len);
    if (!*value) {
        return FAULT_CODE_9002;
    }

    strftime(*value, len, "%Z", tm);

    return FAULT_CODE_OK;
}

