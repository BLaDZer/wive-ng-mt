/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/IPPingDiagnostics/IPPingDiagnostics.c
 */

enum ping_state {
    PING_NONE,
    PING_REQUESTED,
    PING_COMPLETE,
    PING_ERROR_RESOLVE,
    PING_ERROR_INTERNAL,
    PING_ERROR_OTHER,
    PING_NOT_A_STATE
};

static const char *ping_state_string[] =
    {
        "None",
        "Requested",
        "Complete",
        "Error_CannotResolveHostName",
        "Error_Internal",
        "Error_Other"
    };

static struct ping_values_t {
    enum ping_state state;

    char iface[256];
    char host[256];

    unsigned repeat; /* range: 1- (default: 1) */
    unsigned timeout; /* range: 1- (default: 1) */
    unsigned data_size; /* range: 1-65535 (default: 1) */
    unsigned dscp; /* range: 0-63 (default: 0) */

    struct {
        unsigned success;
        unsigned failure;
        unsigned average;
        unsigned minimum;
        unsigned maximum;
    } r;
} ping_values = {.repeat = 1u, .timeout = 1u, .data_size = 1u};

static const char *
state_to_string(enum ping_state state)
{
    const size_t n =
        sizeof(ping_state_string) / sizeof(*ping_state_string);
    if ((size_t)state < n) {
        return ping_state_string[state];
    }
    return "?";
}

static int
set_integer(const char *name, unsigned *target, const char *value,
        unsigned range_min, unsigned range_max)
{
    unsigned long v = 0u;
    char *pend = NULL;
    v = strtoul(value, &pend, 10);
    if (pend && *pend) {
        cwmp_log_error("IPPingDiagnostics.%s not a number: '%s'",
                name, value);
        return FAULT_CODE_OK; /* return FAULT_CODE_9007; */
    }
    if (v < range_min || v > range_max) {
        cwmp_log_error("IPPingDiagnostics.%s value '%s' not in range %u-%u",
               name, value, range_min, range_max);
        return FAULT_CODE_OK; /* return FAULT_CODE_9007; */
    }
    *target = (unsigned)v;
    return FAULT_CODE_OK;
}

static enum ping_state
string_to_state(const char *string)
{
    enum ping_state state = PING_NONE;
    const size_t l = sizeof(*ping_state_string);
    for (; state != PING_NOT_A_STATE; state++) {
        if (!strncmp(string, ping_state_string[state], l))
            break;
    }
    return state;
}

/* ping execution */
enum ping_version {
    PING_UNKNOWN,
    PING_IPUTILS,
    PING_BUSYBOX
};

static void
read_ping_data(FILE *f)
{
    long transmitted = 0u;
    long received = 0u;
    float minimum = 0.f;
    float average = 0.f;
    float maximum = 0.f;

    char *line = NULL;
    size_t size = 0u;
    size_t len = 0u;

    enum ping_version pv = PING_UNKNOWN;
    int line_stage = 0;

    while(getline(&line, &size, f) != -1) {
        len = strlen(line);
        /* check header */
        if (len && pv == PING_UNKNOWN) {
            if (!strncmp(&line[len - 11], "data bytes\n", 11)) {
                /* busybox: "PING localhost (127.0.0.1): 56 data bytes\n" */
                pv = PING_BUSYBOX;
            } else if (!strncmp(&line[len - 15], "bytes of data.\n", 15)) {
                /* iputils: "PING localhost (127.0.0.1) 56(84) bytes of data.\n" */
                pv = PING_IPUTILS;
            } else {
                cwmp_log_error("IPPingDiagnostics: Unknown ping version");
                break;
            }
        }

        if (line_stage == 1) {
            line_stage++;
            if (pv == PING_IPUTILS) {
                /* 2 packets transmitted, 2 received, 0% packet loss, time 999ms */
                sscanf(line,
                        "%ld packets transmitted, %ld received",
                        &transmitted, &received);
            } else if (pv == PING_BUSYBOX) {
                /* 2 packets transmitted, 2 packets received, 0% packet loss */
                sscanf(line,
                        "%ld packets transmitted, %ld packets received",
                        &transmitted, &received);
            }
        } else if (line_stage == 2) {
            if (pv == PING_IPUTILS) {
                /* rtt min/avg/max/mdev = 0.052/0.053/0.054/0.001 ms */
                sscanf(line, "rtt min/avg/max/mdev = %f/%f/%f",
                        &minimum, &average, &maximum);
            } else if (pv == PING_BUSYBOX) {
                /* round-trip min/avg/max = 0.201/0.264/0.327 ms */
                sscanf(line,
                        "round-trip min/avg/max = %f/%f/%f",
                        &minimum, &average, &maximum);
            }
        } else if (line_stage == 0) {
            if (!strncmp("---", line, 3)) {
                line_stage++;
            }
        }
    }

    if (line)
        free(line);

    cwmp_log_debug("ping data: min/avg/max: %f/%f/%f, "
            "transmitted/received: %ld/%ld\n",
            minimum, average, maximum, transmitted, received);

    ping_values.r.success = (unsigned)received;
    ping_values.r.failure = (unsigned)(transmitted - received);
    ping_values.r.minimum = (unsigned)minimum;
    ping_values.r.average = (unsigned)average;
    ping_values.r.maximum = (unsigned)maximum;
}

/* */

/* internal values */
static void
perform_ping(cwmp_t *cwmp)
{
    char buf[512] = {};
    FILE *f = NULL;
    char iface_info[256] = {};

    if (*ping_values.iface) {
        snprintf(iface_info, sizeof(iface_info),
                ", Interface=\"%s\"", ping_values.iface);
    }

    cwmp_log_debug("IPPingDiagnostics run("
            "Host=\"%s\"%s, "
            "NumberOfRepetitions=%u, "
            "Timeout=%u, "
            "DataBlockSize=%u, "
            "DSCP=%u)",
            ping_values.host,
            iface_info,
            ping_values.repeat,
            ping_values.timeout,
            ping_values.data_size,
            ping_values.dscp);
    memset(&ping_values.r, 0u, sizeof(ping_values.r));

    /* check values:
     * ACS not waited Fault Codes, check values and set DiagnosticsState
     */
    if (!*ping_values.host) {
        cwmp_log_warn("IPPingDiagnostics: no host defined");
        ping_values.state = PING_ERROR_RESOLVE;
        return;
    }

    if (ping_values.data_size < 1 || ping_values.data_size > 65535) {
        cwmp_log_warn(
                "IPPingDiagnostics.DataBlockSize value %u not in range 1-65535",
                ping_values.data_size);
        return;
    }
    if (ping_values.repeat < 1) {
        cwmp_log_warn(
                "IPPingDiagnostics.NumberOfRepetitions invalid value: %u",
                ping_values.repeat);
        return;
    }
    if (ping_values.dscp > 63) {
        cwmp_log_warn("IPPingDiagnostics.DSCP value %u not in range 0-63",
                ping_values.dscp);
        return;
    }

    /* FIXME: iface not used */

    /* run popen */
    snprintf(buf, sizeof(buf), "ping -q '%s' -c '%u' -W '%u' -s '%u'",
            ping_values.host,
            ping_values.repeat,
            ping_values.timeout,
            ping_values.data_size);

    f = popen(buf, "r");
    read_ping_data(f);
    if (f) {
        fclose(f);
    } else {
        cwmp_log_error("IPPingDiagnostics: popen() -> %s", strerror(errno));
    }
}

static int
ping_cb(cwmp_t *cwmp, void *arg2)
{
    time_t starttime = 0;
    time_t endtime = 0;

    starttime = time(NULL);
    perform_ping(cwmp);
    endtime = time(NULL);

    cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, starttime, endtime);
    return 0;
}

/* result values */
int
cpe_get_igd_ping_success(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.r.success);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_failure(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.r.failure);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_average(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.r.average);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_minimum(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.r.minimum);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_maximum(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.r.maximum);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

/* request values */
int
cpe_set_igd_ping_state(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    enum ping_state state = string_to_state(value);

    DM_TRACE_SET();

    assert(callback_reg != NULL);

    if (state != PING_REQUESTED) {
        cwmp_log_error("IPPingDiagnostics.DiagnosticsState invalid value: '%s'", value);
        return FAULT_CODE_OK; /* return FAULT_CODE_9007; */
    }
    ping_values.state = state;

    callback_reg(cwmp, (callback_func_t)&ping_cb, (void*)cwmp, NULL);
    return FAULT_CODE_OK;
}

int
cpe_set_igd_ping_dscp(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    return set_integer("DSCP", &ping_values.dscp, value, 0, 63);
}

int
cpe_set_igd_ping_host(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (!length) {
        cwmp_log_error("IPPingDiagnostics.Host zero-length host not allowed");
        return FAULT_CODE_OK; /* return FAULT_CODE_9007; */
    }
    if (strchr(value, '\'') || strchr(value, '`')) {
        cwmp_log_error("IPPingDiagnostics.Host invalid value: %s", value);
        return FAULT_CODE_OK; /* return FAULT_CODE_9007; */
    }
    strncpy(ping_values.host, value, sizeof(ping_values.host));
    return FAULT_CODE_OK;
}

int
cpe_set_igd_ping_iface(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    parameter_node_t *p = NULL;
    DM_TRACE_SET();
    if (!length) {
        memset(ping_values.iface, 0u, sizeof(ping_values.iface));
        return FAULT_CODE_OK;
    }

    p = cwmp_get_parameter_node(cwmp->root, value);
    if (!p) {
        cwmp_log_error("IPPingDiagnostics.Interface invalid value: '%s'",
                value);
        return FAULT_CODE_OK; /* return FAULT_CODE_9007; */
    }

    strncpy(ping_values.iface, value, sizeof(ping_values.iface));
    return FAULT_CODE_OK;
}

int
cpe_set_igd_ping_repeat(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    return set_integer("NumberOfRepetitions", &ping_values.repeat, value, 1, (unsigned)-1);
}

int
cpe_set_igd_ping_data_size(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    return set_integer("DataBlockSize", &ping_values.data_size, value, 1, 65535);
}

int
cpe_set_igd_ping_timeout(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    return set_integer("Timeout", &ping_values.timeout, value, 1, (unsigned)-1);
}

int
cpe_get_igd_ping_state(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, state_to_string(ping_values.state));
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_dscp(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char dscp[8] = {};
    DM_TRACE_GET();
    snprintf(dscp, sizeof(dscp), "%u", ping_values.dscp);
    *value = pool_pstrdup(pool, dscp);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_host(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, ping_values.host);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_iface(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, ping_values.iface);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_repeat(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.repeat);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_data_size(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.data_size);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

int
cpe_get_igd_ping_timeout(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char v[32] = {};
    DM_TRACE_GET();
    snprintf(v, sizeof(v), "%u", ping_values.timeout);
    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

