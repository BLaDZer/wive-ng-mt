/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/DownloadDiagnostics/DownloadDiagnostics.c
 */

enum dd_state {
    /* None (READONLY) */
    DD_NONE,
    /* Requested */
    DD_REQUESTED,
    /* Completed (READONLY) */
    DD_COMPLETED,
    /* Error_InitConnectionFailed (READONLY) */
    DD_ERROR_INIT,
    /* Error_NoResponse (READONLY) */
    DD_ERROR_RESPONSE,
    /* Error_PasswordRequestFailed (READONLY) */
    DD_ERROR_PASSWORD,
    /* Error_LoginFailed (READONLY) */
    DD_ERROR_LOGIN,
    /* Error_NoTransferMode (READONLY) */
    DD_ERROR_TXMODE,
    /* Error_NoPASV (READONLY) */
    DD_ERROR_PASV,
    /* Error_IncorrectSize (READONLY) */
    DD_ERROR_SIZE,
    /* Error_Timeout (READONLY) */
    DD_ERROR_TIMEOUT
};

static struct ddiagnostics {
    enum dd_state state;
    char url[256];
    char iface[256];
    unsigned epri;
    unsigned dscp;
    struct http_statistics hs;
} ddiagnostics = {};

struct thread_ddiagnostics {
    struct ddiagnostics dd;
    cwmp_t *cwmp;
    callback_register_func_t callback_reg;
    pthread_t thread_id;
    time_t starttime;
    time_t endtime;
};

static int ddiagnostics_cb(cwmp_t *cwmp, struct thread_ddiagnostics *dd)
{
    assert(dd != NULL);
    /* copy result */
    memcpy(&ddiagnostics, &dd->dd, sizeof(ddiagnostics));

    /* set flag */
    cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0,
            dd->starttime, dd->endtime);

    free(dd);
    return 0;
}

static void * thread_ddiagnostics(struct thread_ddiagnostics *dd)
{
    time(&dd->starttime);
    cwmp_log_debug("%s: run(%p)", __func__, (void*)dd);

    /* download */
    if (http_receive_file(dd->dd.url, NULL, &dd->dd.hs) != CWMP_OK) {
        dd->dd.state = DD_ERROR_RESPONSE;
    } else {
        dd->dd.state = DD_COMPLETED;
    }
    time(&dd->endtime);

    (*dd->callback_reg)(dd->cwmp,
            (callback_func_t)&ddiagnostics_cb, dd->cwmp, dd);
    return NULL;
}

int cpe_reload_dd(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    struct thread_ddiagnostics *dd = NULL;
    int terr = 0;

    DM_TRACE_RELOAD();
    assert(callback_reg != NULL);
    /* check values */
    if (ddiagnostics.state != DD_REQUESTED) {
        cwmp_log_error(
                "DownloadDiagnostics.DiagnosticsState: state != 'Requested'");
        goto err;
    }

    if (ddiagnostics.dscp > 63) {
        cwmp_log_error("DownloadDiagnostics.DSCP: value not in range 0-63: %u",
                ddiagnostics.dscp);
        goto err;
    }

    if (ddiagnostics.epri > 7) {
        cwmp_log_error(
                "DownloadDiagnostics.EthernetPriority: "
                "value not in range 0-7: %u", ddiagnostics.epri);
        goto err;
    }

    if (!*ddiagnostics.url) {
        cwmp_log_error("DownloadDiagnostics.DownloadURL: empty url");
        goto err;
    }

    /* fix unsupported values */
    memset(ddiagnostics.iface, 0u, sizeof(ddiagnostics.iface));
    ddiagnostics.dscp = 0u;
    ddiagnostics.epri = 0u;

    /* fixme: DownloadDiagnostics.Interface not supported */

    /* copy data */
    dd = calloc(1, sizeof(*dd));
    if (!dd) {
        cwmp_log_error("%s: calloc(%d) failed: %s",
                __func__, sizeof(*dd), strerror(errno));
        goto err;
    }
    memcpy(&dd->dd, &ddiagnostics, sizeof(ddiagnostics));
    dd->cwmp = cwmp;
    dd->callback_reg = callback_reg;

    /* execute thread */
    terr = pthread_create(&dd->thread_id, NULL, (void*)&thread_ddiagnostics, dd);
    if (terr != 0) {
        cwmp_log_error("%s: pthread_create() failed: %s",
                   __func__, strerror(terr));
        goto err;
    }
    return FAULT_CODE_OK;
err:
    if (dd)
        free(dd);
    ddiagnostics.state = DD_ERROR_INIT;
    cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, 0, 0);
    return FAULT_CODE_9002;
}

int cpe_get_dd_dscp(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", ddiagnostics.dscp);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_dd_epri(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%u", ddiagnostics.epri);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_dd_iface(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, ddiagnostics.iface);
    return FAULT_CODE_OK;
}

int cpe_get_dd_result(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    parameter_node_t *pn = NULL;
    char buf[42] = {};
    struct tm *tm = NULL;
    suseconds_t usec = 0u;
    size_t len = 0;

    DM_TRACE_GET();
    pn = cwmp_get_parameter_path_node(cwmp->root, name);
    if (!pn) {
        return FAULT_CODE_9005;
    }
    /* TODO: ... */

    if (!strcmp(pn->name, "ROMTime")) {
        tm = gmtime(&ddiagnostics.hs.request.tv_sec);
        usec = ddiagnostics.hs.request.tv_usec;
    } else if (!strcmp(pn->name, "BOMTime")) {
        tm = gmtime(&ddiagnostics.hs.transmission_rx.tv_sec);
        usec = ddiagnostics.hs.transmission_rx.tv_usec;
    } else if (!strcmp(pn->name, "EOMTime")) {
        tm = gmtime(&ddiagnostics.hs.transmission_rx_end.tv_sec);
        usec = ddiagnostics.hs.transmission_rx_end.tv_usec;
    } else if (!strcmp(pn->name, "TestBytesReceived")) {
        snprintf(buf, sizeof(buf), "%"PRIu64, ddiagnostics.hs.bytes_rx);
    } else if (!strcmp(pn->name, "TotalBytesReceived")) {
        /* FIXME: unsupported */
    } else if (!strcmp(pn->name, "TCPOpenRequestTime")) {
        tm = gmtime(&ddiagnostics.hs.tcp_connect.tv_sec);
        usec = ddiagnostics.hs.tcp_connect.tv_usec;
    } else if (!strcmp(pn->name, "TCPOpenResponseTime")) {
        tm = gmtime(&ddiagnostics.hs.tcp_response.tv_sec);
        usec = ddiagnostics.hs.tcp_response.tv_usec;
    }
    if (tm) {
        len = strftime(buf, sizeof(buf), "%FT%T", tm);
        snprintf(buf + len, sizeof(buf) - len, ".%ld", usec);
    }

    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_dd_state(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    switch (ddiagnostics.state) {
        case DD_NONE:
            *value = "None";
            break;
        case DD_REQUESTED:
            *value = "Requested";
            break;
        case DD_COMPLETED:
            *value = "Completed";
            break;
        case DD_ERROR_INIT:
            *value = "Error_InitConnectionFailed";
            break;
        case DD_ERROR_RESPONSE:
            *value = "Error_NoResponse";
            break;
        case DD_ERROR_PASSWORD:
            *value = "Error_PasswordRequestFailed";
            break;
        case DD_ERROR_LOGIN:
            *value = "Error_LoginFailed";
            break;
        case DD_ERROR_TXMODE:
            *value = "Error_NoTransferMode";
            break;
        case DD_ERROR_PASV:
            *value = "Error_NoPASV";
            break;
        case DD_ERROR_SIZE:
            *value = "Error_IncorrectSize";
            break;
        case DD_ERROR_TIMEOUT:
            *value = "Error_Timeout";
            break;
        default:
            *value = "None";
    }
    return FAULT_CODE_OK;
}

int cpe_get_dd_url(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, ddiagnostics.url);
    return FAULT_CODE_OK;
}

int cpe_set_dd_dscp(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long val = 0ul;
    DM_TRACE_SET();
    val = strtoul(value, NULL, 10);
    if (val > 63 || !*value) {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    ddiagnostics.dscp = (unsigned)val;
    return FAULT_CODE_OK;
}

int cpe_set_dd_epri(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long val = 0ul;
    DM_TRACE_SET();
    val = strtoul(value, NULL, 10);
    if (val > 7 || !*value) {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    return FAULT_CODE_OK;
}

int cpe_set_dd_iface(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    parameter_node_t *pn = NULL;
    DM_TRACE_SET();

    pn = cwmp_get_parameter_path_node(cwmp->root, value);
    if (!pn) {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    snprintf(ddiagnostics.iface, sizeof(ddiagnostics.iface), "%s", value);
    return FAULT_CODE_OK;
}

int cpe_set_dd_state(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (!strcmp(value, "Requested")) {
        ddiagnostics.state = DD_REQUESTED;
    } else {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    return FAULT_CODE_OK;
}

int cpe_set_dd_url(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (!*value) {
        cwmp_log_error("%s: empty value not allowed", name);
    }
    snprintf(ddiagnostics.url, sizeof(ddiagnostics.url), "%s", value);
    return FAULT_CODE_OK;
}
