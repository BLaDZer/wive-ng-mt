/* vim: ft=c ff=unix fenc=utf-8
 * file: cwmpd/src/modules/alias.c
 */

int cpe_set_alias(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    parameter_node_t *pn = NULL;
    char *tmp = NULL;
    /* TODO: save aliases */

    DM_TRACE_SET();
    pn = cwmp_get_parameter_path_node(cwmp->root, name);
    if (!pn || !(pn = pn->parent)) {
        return FAULT_CODE_9002;
    }

    tmp = strdup(value);
    if (!tmp) {
        cwmp_log_error("%s: strdup() failed: %s", __func__, strerror(errno));
        return FAULT_CODE_9002;
    }

    free(pn->alias);
    pn->alias = tmp;

    return FAULT_CODE_OK;
}

int cpe_get_alias(cwmp_t *cwmp, const char *name, char **value, char *args, pool_t *pool)
{
    parameter_node_t *pn = NULL;

    DM_TRACE_GET();
    pn = cwmp_get_parameter_path_node(cwmp->root, name);
    if (!pn || !(pn = pn->parent)) {
        return FAULT_CODE_9002;
    }

    *value = pool_pstrdup(pool, pn->alias);

    return FAULT_CODE_OK;
}

