/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/InternetGatewayDevice/LANDevice/LANHostConfigManagement/DHCPStaticAddress/DHCPStaticAddress.c
 */

struct dhcpstatic {
    char mac[18];
    char addr[40];
    char description[128];
    bool deleted;
};

static int dhcpstatic_counter;
static int dhcpstatic_size;
static struct dhcpstatic *dhcpstatic;

/* helpers */
static char *
dhcpstatic_format(struct dhcpstatic *rules, size_t count)
{
    char *out = NULL;
    size_t n = 0;
    size_t size = 190 * count;
    size_t len = 0;

    if (!count) {
        return strdup("");
    }

    /* 190 bytes on rule */
    out = calloc(count, 190);
    if (!out) {
        cwmp_log_error("DHCPStaticAddress: "
                "calloc(\"%"PRIuPTR"\") failed: %s\n",
                size, strerror(errno));
        return NULL;
    }

    for (n = 0u; n < count; n++) {
        if (!*rules[n].mac || !*rules[n].addr || rules[n].deleted)
            continue;
        len += snprintf(out + len, size - len, "%s %s %s%s",
                rules[n].mac, rules[n].addr,
                rules[n].description,
                ((n + 1 == count) ? "" : ";"));
    }
    return out;
}

static char *
dhcpstatic_parse(const char *in, struct dhcpstatic *rule)
{
    int rc = 0;
    regex_t preg = {};
    regmatch_t pmatch[5] = {};
    char *next = NULL;
    char pattern[] = "([0-9A-Fa-f:]{17}) ([0-9\\.]{7,15}) ([^;]*)";

    if (!in || !*in)
        return NULL;

    rc = regcomp(&preg, pattern, REG_EXTENDED);
    if (rc != 0) {
        cwmp_log_trace("DHCPStaticAddress: "
                "regcomp(\"%s\") failed: %d\n", pattern, rc);
        return NULL;
    }

    rc = regexec(&preg, in, sizeof(pmatch) / sizeof(*pmatch), pmatch, 0);
    if (rc != 0) {
        cwmp_log_error("DHCPStaticAddress: "
                "regexec(\"%s\", \"%s\") failed: %d\n",
                pattern, in, rc);
        regfree(&preg);
        return NULL;
    }

    next = (char *)in + pmatch[0].rm_eo;

    if (rule) {
        memset(rule, 0u, sizeof(*rule));
        /* copy mac */
        snprintf(rule->mac, sizeof(rule->mac), "%.*s",
                pmatch[1].rm_eo - pmatch[1].rm_so,
                in + pmatch[1].rm_so);
        /* copy addr */
        snprintf(rule->addr, sizeof(rule->addr), "%.*s",
                pmatch[2].rm_eo - pmatch[2].rm_so,
                in + pmatch[2].rm_so);
        /* copy description */
        snprintf(rule->description, sizeof(rule->description), "%.*s",
                pmatch[3].rm_eo - pmatch[3].rm_so,
                in + pmatch[3].rm_so);
    }

    return next;
}

static size_t
dhcpstatic_count(const char *in)
{
    const char *p = in;
    size_t c = 0u;
    while ((p = dhcpstatic_parse(p, NULL)) != NULL) {
        c++;
    }
    return c;
}

/* export */

int
cpe_get_dhcpstatic_count(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    char buf[42] = {};
    DM_TRACE_GET();
    snprintf(buf, sizeof(buf), "%d", dhcpstatic_counter);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int
cpe_refresh_dhcpstatic(cwmp_t *cwmp, parameter_node_t *param_node, callback_register_func_t callback_reg)
{
    const char *p = NULL;
    parameter_node_t *pn = NULL;

    DM_TRACE_REFRESH();
    /* remove old list  */
    cwmp_model_delete_object_child(cwmp, param_node);
    dhcpstatic_counter = 0;
    dhcpstatic_size = 0;
    if (dhcpstatic) {
        free(dhcpstatic);
        dhcpstatic = NULL;
    }

    p = cwmp_nvram_get("dhcpStatic");
    if (!p || !*p) {
	cwmp_log_info("%s: empty nvram value dhcpStatic", __func__);
        return FAULT_CODE_OK;
    }

    dhcpstatic_size = dhcpstatic_count(p);
    if (dhcpstatic_size == 0) {
        cwmp_log_error("%s: invalid dhcpStatic value: \"%s\"", __func__, p);
        return FAULT_CODE_9002;
    }

    dhcpstatic = calloc(dhcpstatic_size, sizeof(struct dhcpstatic));
    if (!dhcpstatic) {
        cwmp_log_error("%s: calloc(%d) failed: %s",
                __func__, dhcpstatic_size * sizeof(struct dhcpstatic),
                   strerror(errno));
        return FAULT_CODE_9002;
    }

    while ((p = dhcpstatic_parse(p, &dhcpstatic[dhcpstatic_counter])) != NULL) {
        cwmp_log_debug(
                "%s: no=%d, mac=\"%s\", addr=\"%s\", description=\"%s\"",
                __func__,
                dhcpstatic_counter,
                dhcpstatic[dhcpstatic_counter].mac,
                dhcpstatic[dhcpstatic_counter].addr,
                dhcpstatic[dhcpstatic_counter].description
                );
        dhcpstatic_counter++;
        cwmp_model_copy_parameter(param_node, &pn, dhcpstatic_counter);
    }

    return FAULT_CODE_OK;
}

int
cpe_add_dhcpstatic(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg)
{
    parameter_node_t *pn = NULL;
    void *tmp = NULL;

    DM_TRACE_ADD();
    tmp = realloc(dhcpstatic, sizeof(*dhcpstatic) * (dhcpstatic_size + 1));
    if (!tmp) {
        cwmp_log_error("%s: realloc(%d) failed: %s",
                __func__,
                sizeof(*dhcpstatic) * (dhcpstatic_size + 1),
                strerror(errno));
        return FAULT_CODE_9002;
    }
    memset(&dhcpstatic[dhcpstatic_size], 0, sizeof(*dhcpstatic));
    cwmp_model_copy_parameter(param_node, &pn, ++dhcpstatic_size);
    dhcpstatic_counter++;
    return FAULT_CODE_OK;
}

int
cpe_reload_dhcpstatic(cwmp_t *cwmp, callback_register_func_t callback_reg)
{
    char *p = NULL;

    DM_TRACE_RELOAD();
    if (!dhcpstatic_size) {
        cwmp_log_debug("%s: nothing to save", __func__);
        return FAULT_CODE_OK;
    }

    if ((p = dhcpstatic_format(dhcpstatic, dhcpstatic_size)) == NULL) {
        cwmp_log_error("%s: can't format dhcpStatic value", __func__);
        return FAULT_CODE_9002;
    }

    cwmp_nvram_set("dhcpStatic", p);

    free(p);

    system("/etc/init.d/dhcpd restart");
    return FAULT_CODE_OK;
}

static long
dhcpstatic_no_from_path(cwmp_t *cwmp, const char *name)
{
    parameter_node_t *pn = NULL;
    long no = 0;

    pn = cwmp_get_parameter_path_node(cwmp->root, name);
    if (!pn || !(pn = pn->parent)) {
        cwmp_log_error("%s: can't get rule's number", name);
        return -1;
    }

    no = strtol(pn->name, NULL, 10);

    if (!no) {
        cwmp_log_error("%s: node name '%s' not a valid number", name, pn->name);
        return -1;
    }

    if ((unsigned long)no > dhcpstatic_size || dhcpstatic[no].deleted) {
        cwmp_log_error("%s: invalid rule number: %lu", name, no);
        return -1;
    }

    return (no - 1);
}

int
cpe_get_dhcpstatic_chaddr(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    long no = 0;
    DM_TRACE_GET();
    if ((no = dhcpstatic_no_from_path(cwmp, name)) == -1) {
        return FAULT_CODE_9002;
    }
    cwmp_log_debug("chaddr get: no=%ld, value=%s", no, dhcpstatic[no].mac);
    *value = pool_pstrdup(pool, dhcpstatic[no].mac);
    return FAULT_CODE_OK;
}

int
cpe_get_dhcpstatic_yiaddr(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    long no = 0;
    DM_TRACE_GET();
    if ((no = dhcpstatic_no_from_path(cwmp, name)) == -1) {
        return FAULT_CODE_9002;
    }
    *value = pool_pstrdup(pool, dhcpstatic[no].addr);
    return FAULT_CODE_OK;
}

int
cpe_set_dhcpstatic_yiaddr(cwmp_t *cwmp, const char *name, char *value, int length, char *args, callback_register_func_t callback_reg)
{
    long no = 0;
    DM_TRACE_SET();
    if ((no = dhcpstatic_no_from_path(cwmp, name)) == -1) {
        return FAULT_CODE_9002;
    }
    snprintf(dhcpstatic[no].addr, sizeof(dhcpstatic[no].addr), "%s", value);
    return FAULT_CODE_OK;
}

int
cpe_set_dhcpstatic_chaddr(cwmp_t *cwmp, const char *name, char *value, int length, char *args, callback_register_func_t callback_reg)
{
    long no = 0;
    DM_TRACE_SET();
    if ((no = dhcpstatic_no_from_path(cwmp, name)) == -1) {
        return FAULT_CODE_9002;
    }
    snprintf(dhcpstatic[no].mac, sizeof(dhcpstatic[no].mac), "%s", value);
    return FAULT_CODE_OK;
}

int
cpe_del_dhcpstatic(cwmp_t *cwmp, parameter_node_t *param_node, int instance_number, callback_register_func_t callback_reg)
{
    char name[4098] = {};
    long no = 0;
    DM_TRACE_DEL();
    /* FIXME: add a 'char *name' and 'param_node_t *param_node'
     * to set_func, get_func, add_func, del_func API
     */
    cwmp_get_parameter_fullpath(param_node, name, sizeof(name));
    if ((no = dhcpstatic_no_from_path(cwmp, name)) == -1) {
        return FAULT_CODE_9002;
    }
    dhcpstatic[no].deleted = true;
    dhcpstatic_counter--;

    return FAULT_CODE_OK;
}

