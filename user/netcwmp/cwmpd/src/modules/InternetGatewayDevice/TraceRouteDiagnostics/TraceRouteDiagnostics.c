/* vim: ft=c ff=unix fenc=utf-8
 * file: ./cwmpd/src/modules/InternetGatewayDevice/TraceRouteDiagnostics/TraceRouteDiagnostics.c
 */

enum trd_state {
    TRD_NONE,
    TRD_REQUESTED,
    TRD_COMPLETE,
    /* Error_CannotResolveHostName */
    TRD_ERROR_RESOLVE,
    /* Error_MaxHopCountExceeded */
    TRD_ERROR_EXCEEDED
};

struct trd_HopHost {
    char host[256];
    char addr[256];
    char times[16];
};

static struct trd {
    enum trd_state state;
    char iface[256];
    char host[256];
    /* NumberOfTries: 1-3 */
    unsigned number_of_tries;
    /* Timeout: 1- */
    unsigned timeout;
    /* DataBlockSize: 1-65535 */
    unsigned data_block_size;
    /* DSCP: 0-63 */
    unsigned dscp;
    /* MaxHopCount: 1-64 */
    unsigned max_hop_count;

    char *param_name;

    size_t hh_count;
    struct trd_HopHost *hh;
} trd = {
    .number_of_tries = 3,
    .timeout = 5000,
    .data_block_size = 38,
    .max_hop_count = 30
};

/*
 * return element count
 * result list in *hops
 */
size_t
trd_process(FILE *f, struct trd_HopHost *hops, size_t max_hops)
{
    struct trd_HopHost *hh = NULL;
    size_t lineno = 0u;
    int r = 0;
    char l[1024] = {};
    const char reg[] = "[[:space:]]*([0-9]*)[[:space:]]*"
        /* first column */
        "("
            "\\*|"
            "([-_0-9a-zA-Z\\.]+) \\(([0-9\\.]*)\\)[[:space:]]{1,}([0-9]*)[\\.0-9]* ms"
        ")[[:space:]]*"
        /* second column */
        "("
            "\\*[[:space:]]+|"
            "([-_0-9a-zA-Z\\.]+) \\(([0-9\\.]+)\\)[[:space:]]+([0-9]+)[\\.0-9]* ms[[:space:]]+|"
            "([0-9]*)[\\.0-9]* ms[[:space:]]+"
        ")*"
        /* third column */
        "("
            "\\*[[:space:]]*|"
            "([-_0-9a-zA-Z\\.]+) \\(([0-9\\.]+)\\)[[:space:]]+([0-9]+)[\\.0-9]* ms[[:space:]]*|"
            "([0-9]*)[\\.0-9]* ms[[:space:]]*"
        ")*"
        "\n";
    regex_t preg = {};
    regmatch_t pmatch[34] = {};
    size_t nmatch = sizeof(pmatch) / sizeof(*pmatch);

    if ((r = regcomp(&preg, reg, REG_EXTENDED)) != 0) {
        cwmp_log_error("regcomp(): %d", r);
        return 0;
    }

    while (fgets(l, sizeof(l), f) != NULL && lineno < max_hops) {
        cwmp_log_info("TraceRouteDiagnostics: %.*s",
                *l ? ((int)strlen(l) - 1) : 0, l);
        if (regexec(&preg, l, nmatch, pmatch, 0)) {
            if (!strncmp("traceroute", l, 10)) {
                /* skip header */
                continue;
            }
            cwmp_log_error(
                    "TraceRouteDiagnostics: line not matched: %.*s\n",
                       *l ? ((int)strlen(l) - 1) : 0, l);
            continue;
        }
        hh = &hops[lineno++];
        memset(hh, 0, sizeof(*hh));
        /* first column */
        if (pmatch[5].rm_so == -1) {
            /* no time */
            strcat(hh->times, "*");
        } else {
            /* copy host */
            snprintf(hh->host, sizeof(hh->host), "%.*s",
                    pmatch[3].rm_eo - pmatch[3].rm_so,
                    l + pmatch[3].rm_so);
            /* copy addr */
            snprintf(hh->addr, sizeof(hh->addr), "%.*s",
                    pmatch[4].rm_eo - pmatch[4].rm_so,
                    l + pmatch[4].rm_so);
            /* copy time */
            snprintf(hh->times, sizeof(hh->times),
                    "%.*s",
                    pmatch[5].rm_eo - pmatch[5].rm_so,
                    l + pmatch[5].rm_so);
        }
        for (r = 0; r < 2; r++) {
            /* second, third, <n> column */
            if (pmatch[6 + 5 * r].rm_so != -1) {
                size_t len = 0u;
                /* copy host && addr */
                if (!*hh->host && pmatch[7 + 5 * r].rm_so - 1) {
                    snprintf(hh->host, sizeof(hh->host), "%.*s",
                            pmatch[7 + 5 * r].rm_eo - pmatch[7 + 5 * r].rm_so,
                            l + pmatch[7 + 5 * r].rm_so);
                    snprintf(hh->addr, sizeof(hh->addr), "%.*s",
                            pmatch[8 + 5 * r].rm_eo - pmatch[8 + 5 * r].rm_so,
                            l + pmatch[8 + 5 * r].rm_so);
                }
                /* copy time */
                len = strlen(hh->times);
                if (pmatch[9 + 5 * r].rm_so != -1) {
                    /* copy from 'host (addr) time' */
                    snprintf(hh->times + len, sizeof(hh->times) - len,
                            ",%.*s",
                            pmatch[9 + 5 * r].rm_eo - pmatch[9 + 5 * r].rm_so,
                            l + pmatch[9 + 5 * r].rm_so);
                } else if (pmatch[10 + 5 * r].rm_so != -1) {
                    /* copy from 'time' */
                    snprintf(hh->times + len, sizeof(hh->times) - len,
                            ",%.*s",
                            pmatch[10 + 5 * r].rm_eo - pmatch[10 + 5 * r].rm_so,
                            l + pmatch[10 + 5 * r].rm_so);
                } else {
                    snprintf(hh->times + len, sizeof(hh->times) - len, ",*");
                }
            }
        }
    }
    regfree(&preg);
    return lineno;
}



int
cpe_reload_trd(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    FILE *f = NULL;
    char cmd[300] = {};
    parameter_node_t *pn = NULL;
    parameter_node_t *npn = NULL;
    size_t i = 0u;
    time_t starttime = 0;
    time_t endtime = 0;

    DM_TRACE_RELOAD();
    time(&starttime);

    if (trd.hh) {
        free(trd.hh);
        trd.hh_count = 0u;
    }

    if (!trd.param_name ||
            !(pn = cwmp_get_parameter_path_node(cwmp->root, trd.param_name))) {
        cwmp_log_error(
                "TraceRouteDiagnostics: no parameter name (invalid path: %s)",
                trd.param_name);
        return FAULT_CODE_9002;
    }

    if (!(pn = cwmp_get_parameter_node(pn->parent, "TraceRouteDiagnostics.RouteHops"))) {
        cwmp_log_error(
                "TraceRouteDiagnostics: can't get TraceRouteDiagnostics.RouteHops node");
        return FAULT_CODE_9002;
    }

    /* free list */
    cwmp_model_delete_object_child(cwmp, pn);

    /* validate values */
    if (!trd.max_hop_count || trd.max_hop_count > 64) {
        cwmp_log_error(
                "TraceRouteDiagnostics: MaxHopCount not in range 1-64, value=%u",
                trd.max_hop_count);
        trd.state = TRD_ERROR_EXCEEDED;
        return FAULT_CODE_9002;
    }

    if (!trd.number_of_tries || trd.number_of_tries > 3) {
        cwmp_log_error(
                "TraceRouteDiagnostics: NumberOfTries not in range 1-3, value=%u",
                trd.number_of_tries);
        trd.state = TRD_ERROR_RESOLVE;
        return FAULT_CODE_9002;
    }

    if (!trd.timeout) {
        cwmp_log_error(
                "TraceRouteDiagnostics: Timeout must be > 1");
        trd.state = TRD_ERROR_RESOLVE;
        return FAULT_CODE_9002;
    }

    if (trd.dscp > 63) {
        cwmp_log_error(
                "TraceRouteDiagnostics: DSCP not in range 0-63, value=%u",
                trd.dscp);
        trd.state = TRD_ERROR_RESOLVE;
        return FAULT_CODE_9002;
    }

    if (!*trd.host) {
        cwmp_log_error("TraceRouteDiagnostics: Host not defined");
        trd.state = TRD_ERROR_RESOLVE;
        return FAULT_CODE_9002;
    }

    /* run */
    snprintf(cmd, sizeof(cmd),
            "traceroute -q %u -w %u -t %u -m %u '%s'",
            trd.number_of_tries,
            trd.timeout < 1000 ? 1 : (trd.timeout / 1000),
            trd.dscp,
            trd.max_hop_count,
            trd.host);

    cwmp_log_info("TraceRouteDiagnostics: run %s", cmd);
    if (!(f = popen(cmd, "r"))) {
        cwmp_log_error("TraceRouteDiagnostics: popen(%s) failed: %s",
                cmd, strerror(errno));
        trd.state = TRD_ERROR_RESOLVE;
        return FAULT_CODE_9002;
    }

    /* allocate */
    trd.hh = calloc(trd.max_hop_count, sizeof(struct trd_HopHost));
    if (!trd.hh) {
        cwmp_log_error("TraceRouteDiagnostics: calloc(%d) failed: %s",
                trd.max_hop_count * sizeof(struct trd_HopHost),
                strerror(errno));
        pclose(f);
        trd.state = TRD_ERROR_RESOLVE;
        return FAULT_CODE_9002;
    }

    trd.hh_count = trd_process(f, trd.hh, trd.max_hop_count);

    if (pclose(f) || !trd.hh_count) {
        trd.state = TRD_ERROR_RESOLVE;
        /* normal status */
        return FAULT_CODE_OK;
    }

    /* populate list */
    for (i = 0u; i < trd.hh_count; i++) {
        cwmp_model_copy_parameter(pn, &npn, i + 1);
    }

    time(&endtime);
    cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, starttime, endtime);

    return FAULT_CODE_OK;
}

int
cpe_get_trd_state(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    switch(trd.state) {
        case TRD_NONE:
            *value = "None";
            break;
        case TRD_REQUESTED:
            *value = "Requested";
            break;
        case TRD_COMPLETE:
            *value = "Complete";
            break;
        case TRD_ERROR_RESOLVE:
            *value = "Error_CannotResolveHostName";
            break;
        case TRD_ERROR_EXCEEDED:
            *value = "Error_MaxHopCountExceeded";
            break;
        default:
            cwmp_log_error("TraceRouteDiagnostics: unknown state: %d",
                    trd.state);
            return FAULT_CODE_9002;
    };
    return FAULT_CODE_OK;
}

int
cpe_set_trd_state(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (strcmp(value, "Requested")) {
        cwmp_log_error(
                "TraceRouteDiagnostics.DiagnosticsState invalid value: %s. "
                "Only 'Requested' allowed",
                value);
        return FAULT_CODE_9007;
    }
    trd.state = TRD_REQUESTED;
    /* reassurance */
    if (trd.param_name) {
        free(trd.param_name);
        trd.param_name = NULL;
    }
    trd.param_name = strdup(name);
    return FAULT_CODE_OK;
}

int
cpe_get_trd_iface(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, trd.iface);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_iface(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    parameter_node_t *pn = NULL;
    DM_TRACE_SET();
    /* not support? */
    pn = cwmp_get_parameter_node(cwmp->root, value);
    if (!pn) {
        cwmp_log_error("TraceRouteDiagnostics.Interface invalid value: '%s'",
                value);
        return FAULT_CODE_9007;
    }

    strncpy(trd.iface, value, sizeof(trd.iface));
    return FAULT_CODE_OK;
}

int
cpe_get_trd_host(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, trd.host);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_host(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (!length) {
        cwmp_log_error("TraceRouteDiagnostics.Host zero-length host not allowed");
        return FAULT_CODE_9007;
    }
    if (strchr(value, '\'') || strchr(value, '`')) {
        cwmp_log_error("TraceRouteDiagnostics.Host invalid value: %s", value);
        return FAULT_CODE_9007;
    }
    strncpy(trd.host, value, sizeof(trd.host));
    return FAULT_CODE_OK;
}

int
cpe_get_trd_tries(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", trd.number_of_tries);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_tries(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long n = 0lu;
    DM_TRACE_SET();
    /* traceroute option: -q N */
    n = strtoul(value, NULL, 10);
    if (n < 1 || n > 3) {
        cwmp_log_error(
                "TraceRouteDiagnostics.NumberOfTries invalid value: "
                "'%s', value not in range 1-3", value);
        return FAULT_CODE_9007;
    }

    trd.number_of_tries = (unsigned)n;

    return FAULT_CODE_OK;
}

int
cpe_get_trd_timeout(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", trd.number_of_tries);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_timeout(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long n = 0lu;
    DM_TRACE_SET();
    /* traceroute option: -w SEC */
    n = strtoul(value, NULL, 10);
    if (n < 1) {
        cwmp_log_error("TraceRouteDiagnostics.Timeout must be greater then zero. (value: %s)", value);
        return FAULT_CODE_9007;
    }
    trd.timeout = (unsigned)n;
    return FAULT_CODE_OK;
}

int
cpe_get_trd_dbs(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", trd.data_block_size);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_dbs(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    /* DataBlockSize, not supported */
    return FAULT_CODE_OK;
}

int
cpe_get_trd_dscp(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", trd.dscp);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_dscp(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long n = 0lu;
    DM_TRACE_SET();
    /* traceroute option: -t N */
    n = strtoul(value, NULL, 10);
    if (n > 63) {
        cwmp_log_error(
                "TraceRouteDiagnostics.DSCP invalid value: %s. "
                "Not in range 0-63", value);
        return FAULT_CODE_9007;
    }
    trd.dscp = (unsigned)n;
    return FAULT_CODE_OK;
}

int
cpe_get_trd_mhc(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", trd.max_hop_count);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_set_trd_mhc(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long n = 0lu;
    DM_TRACE_SET();
    /* traceroute option: -m N */
    n = strtoul(value, NULL, 10);
    if (n < 1 || n > 64) {
        cwmp_log_error(
                "TraceRouteDiagnostics.MaxHopCount invalid value: %s. "
                "Not in range 1-64", value);
        return FAULT_CODE_9007;
    }
    trd.max_hop_count = (unsigned)n;
    return FAULT_CODE_OK;
}

int
cpe_get_trd_response(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    /* FIXME: Not understand what this means:
     * Result parameter indicating the response time in milliseconds
     * the most recent trace route test. If a route could not be determined,
     * this value MUST be zero.
     */
    *value = "0";
    return FAULT_CODE_OK;
}


int
cpe_get_trd_hop_count(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();

    snprintf(buf, sizeof(buf), "%zu", trd.hh_count);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}


/* get result data:
 *        HopHost
 *        HopHostAddress
 *        HopErrorCode
 *        HopRTTimes
 */
int
cpe_get_trd_hop(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char *param = NULL;
    parameter_node_t *pn = NULL;
    size_t no = 0u;
    struct trd_HopHost *hh = NULL;

    DM_TRACE_GET();
    pn = cwmp_get_parameter_path_node(cwmp->root, name);

    if (pn) {
        param = pn->name;
    }

    if (!pn || !(pn = pn->parent)) {
        cwmp_log_error("TraceRouteDiagnostics: invalid node path: %s", name);
        return FAULT_CODE_9002;
    }

    no = strtoul(pn->name, NULL, 10);
    if (!no || no > trd.hh_count) {
        cwmp_log_error(
                "TraceRouteDiagnostics: invalid hop number: '%s'",
                pn->name);
    }

    hh = &trd.hh[no - 1];

    if (!strcmp("HopHost", param)) {
        *value = pool_pstrdup(pool, hh->host);
    } else if (!strcmp("HopHostAddress", param)) {
        *value = pool_pstrdup(pool, hh->addr);
    } else if (!strcmp("HopErrorCode", param)) {
        /* not supported */
        *value = "0";
    } else if (!strcmp("HopRTTimes", param)) {
        *value = pool_pstrdup(pool, hh->times);
    }

    return FAULT_CODE_OK;
}



