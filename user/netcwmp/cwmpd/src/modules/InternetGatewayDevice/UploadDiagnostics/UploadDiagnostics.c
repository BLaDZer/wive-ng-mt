/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/UploadDiagnostics/UploadDiagnostics.c
 */

enum ud_state {
    /* None (READONLY) */
    UD_NONE,
    /* Requested */
    UD_REQUESTED,
    /* Completed (READONLY) */
    UD_COMPLETED,
    /* Error_InitConnectionFailed (READONLY) */
    UD_ERROR_INIT,
    /* Error_NoResponse (READONLY) */
    UD_ERROR_RESPONSE,
    /* Error_PasswordRequestFailed (READONLY) */
    UD_ERROR_PASSWORD,
    /* Error_LoginFailed (READONLY) */
    UD_ERROR_LOGIN,
    /* Error_NoTransferMode (READONLY) */
    UD_ERROR_TRANSFER_MODE,
    /* Error_NoPASV (READONLY) */
    UD_ERROR_NOPASV,
    /* Error_NoCWD (READONLY) */
    UD_ERROR_NOCWD,
    /* Error_NoSTOR (READONLY) */
    UD_ERROR_NOSTOR,
    /* Error_NoTransferComplete (READONLY) */
    UD_ERROR_NOTRANSFER,
};

static struct udiagnostics {
    enum ud_state state;
    char url[256];
    char iface[256];
    unsigned long length;
    struct http_statistics hs;
} udiagnostics;

struct thread_udiagnostics {
    struct udiagnostics ud;
    cwmp_t *cwmp;
    callback_register_func_t callback_reg;
    pthread_t thread;
    time_t starttime;
    time_t endtime;
};

static int
udiagnostics_cb(cwmp_t *cwmp, struct thread_udiagnostics *ud)
{
    assert(cwmp != NULL);
    assert(ud != NULL);
    void *r = NULL;

    /* copy result */
    memcpy(&udiagnostics, &ud->ud, sizeof(udiagnostics));

    /* set flag */
    cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0,
            ud->starttime, ud->endtime);

    pthread_join(ud->thread, &r);
    free(ud);
    return 0;
}

static void *
thread_udiagnostics(struct thread_udiagnostics *ud)
{
    assert(ud != NULL);
    time(&ud->starttime);

    http_send_diagnostics(ud->ud.length, ud->ud.url, &ud->ud.hs);

    time(&ud->endtime);
    (*ud->callback_reg)(ud->cwmp,
            (callback_func_t)&udiagnostics_cb, ud->cwmp, ud);

    return NULL;
}

int
cpe_reload_ud(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    struct thread_udiagnostics *ud = NULL;
    int terr = 0;

    DM_TRACE_RELOAD();
    /* check values */
    if (!*udiagnostics.url) {
        udiagnostics.state = UD_ERROR_INIT;
        cwmp_log_error("DownloadDiagnostics.DownloadURL: empty url");
        goto err;
    }

    /* FIXME: UploadDiagnostics.Interface not supported */

    ud = calloc(1, sizeof(*ud));
    if (!ud) {
        cwmp_log_error("%s: calloc(%d) failed: %s",
                __func__, sizeof(*ud), strerror(errno));
        goto err;
    }
    memcpy(&ud->ud, &udiagnostics, sizeof(udiagnostics));
    ud->cwmp = cwmp;
    ud->callback_reg = callback_reg;
    /* run thread */
    terr = pthread_create(&ud->thread, NULL, (void*)&thread_udiagnostics, ud);
    if (terr != 0) {
        cwmp_log_error("%s: pthread_create() failed: %s",
                   __func__, strerror(terr));
        goto err;
    }
    return FAULT_CODE_OK;
err:
    if (ud)
        free(ud);
    udiagnostics.state = UD_ERROR_INIT;
    cwmp_event_set_value(cwmp, INFORM_DIAGNOSTICSCOMPLETE, 1, NULL, 0, 0, 0);
    return FAULT_CODE_9002;
}

int
cpe_set_ud_iface(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    parameter_node_t *pn = NULL;
    DM_TRACE_SET();

    pn = cwmp_get_parameter_path_node(cwmp->root, value);
    if (!pn) {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    snprintf(udiagnostics.iface, sizeof(udiagnostics.iface), "%s", value);
    return FAULT_CODE_OK;
}

int
cpe_set_ud_url(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (!*value) {
        cwmp_log_error("%s: empty value not allowed", name);
    }
    snprintf(udiagnostics.url, sizeof(udiagnostics.url), "%s", value);
    return FAULT_CODE_OK;
}

int
cpe_set_ud_length(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned long val = 0ul;
    char *e = NULL;
    DM_TRACE_SET();
    val = strtoul(value, &e, 10);
    if (e && *e) {
        cwmp_log_error("%s: value not a number: %s", __func__, value);
        return FAULT_CODE_9007;
    }

    udiagnostics.length = val;
    return FAULT_CODE_OK;
}

int
cpe_set_ud_state(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (!strcmp(value, "Requested")) {
        udiagnostics.state = UD_REQUESTED;
    } else {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    return FAULT_CODE_OK;
}

int
cpe_get_ud_length(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%lu", udiagnostics.length);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_get_ud_url(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, udiagnostics.url);
    return FAULT_CODE_OK;
}

int
cpe_get_ud_iface(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    *value = pool_pstrdup(pool, udiagnostics.iface);
    return FAULT_CODE_OK;
}

int
cpe_get_ud_state(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    DM_TRACE_GET();
    switch (udiagnostics.state)
    {
        case UD_NONE:
            *value = "None";
            break;
        case UD_REQUESTED:
            *value = "Requested";
            break;
        case UD_COMPLETED:
            *value = "Completed";
            break;
        case UD_ERROR_INIT:
            *value = "Error_InitConnectionFailed";
            break;
        case UD_ERROR_RESPONSE:
            *value = "Error_NoResponse";
            break;
        case UD_ERROR_PASSWORD:
            *value = "Error_PasswordRequestFailed";
            break;
        case UD_ERROR_LOGIN:
            *value = "Error_LoginFailed";
            break;
        case UD_ERROR_TRANSFER_MODE:
            *value = "Error_NoTransferMode";
            break;
        case UD_ERROR_NOPASV:
            *value = "Error_NoPASV";
            break;
        case UD_ERROR_NOCWD:
            *value = "Error_NoCWD";
            break;
        case UD_ERROR_NOSTOR:
            *value = "Error_NoSTOR";
            break;
        case UD_ERROR_NOTRANSFER:
            *value = "Error_NoTransferComplete";
            break;
        default:
            *value = "None";

    }
    return FAULT_CODE_OK;
}

int
cpe_get_ud(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
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

    if (!strcmp("ROMTime", pn->name)) {
        if (udiagnostics.hs.request.tv_sec) {
            tm = gmtime(&udiagnostics.hs.request.tv_sec);
            usec = udiagnostics.hs.request.tv_usec;
        }
    } else if (!strcmp("BOMTime", pn->name)) {
        if (udiagnostics.hs.transmission_tx.tv_sec) {
            tm = gmtime(&udiagnostics.hs.transmission_tx.tv_sec);
            usec = udiagnostics.hs.transmission_tx.tv_usec;
        }
    } else if (!strcmp("EOMTime", pn->name)) {
        if (udiagnostics.hs.transmission_tx_end.tv_sec) {
            tm = gmtime(&udiagnostics.hs.transmission_tx_end.tv_sec);
            usec = udiagnostics.hs.transmission_tx_end.tv_usec;
        }
    } else if (!strcmp("TotalBytesSent", pn->name)) {
        snprintf(buf, sizeof(buf), "0");
    } else if (!strcmp("TCPOpenRequestTime", pn->name)) {
        if (udiagnostics.hs.tcp_connect.tv_sec) {
            tm = gmtime(&udiagnostics.hs.tcp_connect.tv_sec);
            usec = udiagnostics.hs.tcp_connect.tv_usec;
        }
    } else if (!strcmp("TCPOpenResponseTime", pn->name)) {
        if (udiagnostics.hs.tcp_response.tv_sec) {
            tm = gmtime(&udiagnostics.hs.tcp_response.tv_sec);
            usec = udiagnostics.hs.tcp_response.tv_usec;
        }
    }

    if (tm) {
        len = strftime(buf, sizeof(buf), "%F%T", tm);
        snprintf(buf + len, sizeof(buf) - len, ".%ld", usec);
    }

    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

