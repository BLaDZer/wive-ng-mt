/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/WANDevice/WANEthernetInterfaceConfig/WANEthernetInterfaceConfig.c
 */

int
cpe_get_wan_eic_stats(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    int wan_port = 0;
    struct port_counts pc = {};
    parameter_node_t *pn = NULL;
    char buf[42] = {'0'};

    DM_TRACE_GET();

    pn = cwmp_get_parameter_path_node(cwmp->root, name);
    if (!pn) {
        return FAULT_CODE_9002;
    }

    wan_port = 4 - cwmp_nvram_get_int("wan_port", 5);
    if (wan_port < 0) {
        return FAULT_CODE_9002;
    }

    portscounts(&pc);

    if (!strcmp("BytesSent", pn->name)) {
        snprintf(buf, sizeof(buf), "%lld", pc.tx_count[wan_port]);
    } else if (!strcmp("BytesReceived", pn->name)) {
        snprintf(buf, sizeof(buf), "%lld", pc.rx_count[wan_port]);
    } else if (!strcmp("PacketsSent", pn->name)) {
        /* not supported */
    } else if (!strcmp("PacketsReceived", pn->name)) {
        /* not supported */
    }
    *value = pool_pstrdup(pool, buf);

    return FAULT_CODE_OK;
}


int
cpe_get_eic_mac(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();
    return FAULT_CODE_OK;
}


static int
_get_wan_eic_duplex_mbr_status(cwmp_t *cwmp, char *name, char **value, pool_t *pool)
{
    int wan_port = 0;
    struct port_status ps = {};
    char buf[42] = {};

    wan_port = 4 - cwmp_nvram_get_int("wan_port", 5);
    if (wan_port == -1) {
        return FAULT_CODE_9002;
    }

    portstatus(&ps, wan_port);

    if (!strcmp(name, "MaxBitRate")) {
        snprintf(buf, sizeof(buf), "%d", ps.speed);
        *value = pool_pstrdup(pool, buf);
    } else if (!strcmp(name, "DuplexMode")) {
        if (ps.duplex == DUPLEX_HALF) {
            *value = "Half";
        } else if (ps.duplex == DUPLEX_FULL) {
            *value = "Full";
        } else {
            *value = "Auto";
        }
    } else if (!strcmp(name, "Status")) {
        if (ps.link) {
            *value = "Up";
        } else {
            *value = "NoLink";
        }
    }

    return FAULT_CODE_OK;
}

int
cpe_get_eic_status(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();
    return _get_wan_eic_duplex_mbr_status(cwmp, "Status", value, pool);
}

int
cpe_get_wan_eic_mbr(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();
    return _get_wan_eic_duplex_mbr_status(cwmp, "MaxBitRate", value, pool);
}

int
cpe_get_wan_eic_duplex(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();
    return _get_wan_eic_duplex_mbr_status(cwmp, "DuplexMode", value, pool);
}

static int
_set_wan_eic_duplex_mbr(cwmp_t *cwmp, const char *name, int new_speed, char new_duplex)
{
    int wan_port = 0;
    char key[42] = {};
    char buf[42] = {};
    char *swmode = NULL;

    int speed = 100;
    char duplex = 'f';

    wan_port = 4 - cwmp_nvram_get_int("wan_port", -5);
    if (wan_port == -1) {
        return FAULT_CODE_9002;
    }

    /* get old value */
    snprintf(key, sizeof(key), "port%d_swmode", wan_port);
    swmode = cwmp_nvram_get(key);
    if (!*swmode) {
        cwmp_log_error("%s: unexpected empty nvram value for '%s'", name, key);
        return FAULT_CODE_9002;
    }

    sscanf(swmode, "%d%c", &speed, &duplex);

    if (new_speed != 0) {
        speed = new_speed;
    }
    if (new_duplex != '\0') {
        duplex = new_duplex;
    }

    /* set new value */
    snprintf(buf, sizeof(buf), "%d%c", speed, duplex);
    cwmp_nvram_set(key, buf);

    return FAULT_CODE_OK;
}

int
cpe_set_wan_eic_duplex(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    char duplex = 'f';

    DM_TRACE_SET();

    if (!strcmp("Half", value)) {
        duplex = 'h';
    } else if (!strcmp("Full", value)) {
        duplex = 'f';
    } else {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    }
    return _set_wan_eic_duplex_mbr(cwmp, name, 0, duplex);
}

int
cpe_set_wan_eic_mbr(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    int speed = 0;

    DM_TRACE_SET();

    speed = strtoul(value, NULL, 0);
    if (!speed) {
        cwmp_log_error("%s: invalid value: '%s'", name, value);
        return FAULT_CODE_9007;
    } else {
        switch (speed) {
            case 10:
            case 100:
                break;
            default:
                cwmp_log_error("%s: unsupported value: '%s'. Allowed: 10, 100",
                        name, value);
                return FAULT_CODE_9007;
        }
    }

    return _set_wan_eic_duplex_mbr(cwmp, name, speed, '\0');
}

