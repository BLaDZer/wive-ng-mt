/* vim: set et: */

struct wlan_assoc
{
    char mac[18];
    char addr[40];
    bool authenticated;
};

struct wlanc_node {
    unsigned id;
    char name[16];

    struct wlan_assoc *assoc;
    unsigned assoc_count;

    struct wlanc_node *next;
};

static struct {
    struct wlanc_node *root;
    unsigned count;
} wlanc;

static void wlanc_free_all(struct wlanc_node *root)
{
    struct wlanc_node *n = root;

    while (root) {
        n = n->next;
        if (root->assoc) {
            free(root->assoc);
            root->assoc = NULL;
            root->assoc_count = 0u;
        }
        free(root);
        root = n;
    }
}

static struct wlanc_node * wlanc_nget(struct wlanc_node *root, const char *name, unsigned id)
{
    struct wlanc_node *n = NULL;

    for (n = root; n; n = n->next) {
        if (name) {
            if (!strcmp(n->name, name)) {
                break;
            }
        } else if (id != -1u) {
            if (n->id == id) {
                break;
            }
        }
    }

    if (!n) {
        cwmp_log_error(
                "InternetGatewayDevice...WLANConfiguration: "
                "can't get wlan with name=%s, id=%u",
                name ? name : "", id);
    }
    return n;
}

static struct wlanc_node * wlanc_nnew(struct wlanc_node *root, char *prefix_filter, char *name)
{
    struct wlanc_node *n = NULL;
    struct wlanc_node *l = NULL;
    unsigned id = 0u;
    size_t prefix_len = 0u;

    if (!name || !*name)
        return NULL;

    if (prefix_filter) {
        prefix_len = strlen(prefix_filter);
        if (strncmp(name, prefix_filter, prefix_len) != 0) {
            return NULL;
        }
        id = (unsigned)strtoul(name + prefix_len, NULL, 10);
    } else {
        /* get id from tail */
        size_t name_max = strlen(name) - 1;
        size_t name_len = name_max;
        for (; name_len > 0; --name_len) {
            if (name[name_len] >= '0' || name[name_len] <= '9') {
                id += (name[name_len] - '0') * ((name_max - name_len) * 10);
            } else {
                break;
            }
        }
    }

    for (n = root; n; n = n->next) {
        if (!strcmp(n->name, name)) {
            break;
        }
        if (n->id == id) {
            break;
        }
        l = n;
    }

    if (n) {
        return NULL;
    }

    n = calloc(1, sizeof(*n));
    if (!n) {
        cwmp_log_error("InternetGatewayDevice...WLANConfiguration: calloc(%d) failed: %s",
                sizeof(*n), strerror(errno));
        return NULL;
    }

    if (name) {
        strncpy(n->name, name, sizeof(n->name));
    }
    n->id = id;

    if (l) {
        l->next = n;
    }

    return n;
}

static unsigned wlanc_get_id_node(parameter_node_t *pn, const char *name, parameter_node_t **out)
{
    unsigned id = 0u;
    char *p = NULL;

    if (!pn || !(pn = pn->parent)) {
        return -1u;
    }

    /* find */
    for (; pn; pn = pn->parent) {
        id = (unsigned)strtoul(pn->name, &p, 10);
        if (!*p) {
            break;
        }
    }

    if (!pn) {
        cwmp_log_error(
                "InternetGatewayDevice...WLANConfiguration: "
                "can't get number from path: %s",
                name
                );
        return -1u;
    }

    if (id == 0) {
        cwmp_log_error(
                "InternetGatewayDevice...WLANConfiguration: "
                "invalid number '%u' in path: %s",
                id, name);
        return -1u;
    }

    if (out)
        *out = pn;

    return id - 1;
}

static unsigned wlanc_get_id(cwmp_t *cwmp, const char *name, parameter_node_t **out)
{
    parameter_node_t *pn = NULL;

    pn = cwmp_get_parameter_path_node(cwmp->root, name);
    if (!pn) {
        cwmp_log_error(
                "InternetGatewayDevice...WLANConfiguration: "
                "can't process path: %s",
                name);
        return (unsigned)-1;
    }

    return wlanc_get_id_node(pn, name, out);
}

BOOL prefix(const char *str, const char *pre)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

#define WLAN_NULL 0


enum wlan_security {
    WLAN_BASIC = 1,
    WLAN_WPA = 2,
    WLAN_11i = 4,
    WLAN_WPAand11i = 6
};

enum wlan_auth {
    WLAN_OPEN = 1,
    WLAN_PSK = 2,
    WLAN_EAP = 3,
    WLAN_SHARED = 4
};

enum wlan_encryption {
    WLAN_NO_ENCRYPTION = 1,
    WLAN_WEP = 2,
    WLAN_AES = 4,
    WLAN_TKIP = 8,
    WLAN_TKIPAES = 12
};

struct wlan_security_mode {
    enum wlan_security mode;
    enum wlan_auth authMode;
    enum wlan_encryption encrypt;
};

static void nvram_set_tuple(const char *key, unsigned index, const char *value)
{
    char *v = NULL;
    char *e = NULL;
    char *s = NULL;
    unsigned i = 0;
    char *nv = NULL;

    size_t vlen = 0u;
    size_t len = 0u;
    size_t value_len = strlen(value);

    /* indexes started at 1 */
    index++;

    e = s = v = cwmp_nvram_get(key);
    vlen = strlen(v);
    while ((e = strchr(s, ';')) != NULL) {
        if (++i == index)
            break;
        s = ++e;
    }
    /* fix endpos */
    if (!e) {
        e = v + vlen;
        i++;
    }

    if (i != index) {
        cwmp_log_info("%s: grow list from %u i to %u", __func__, i, index);
        i = index - i;
        s = e;
    } else {
        i = 0;
    }

    /* format new nvram value */
    len = vlen - (e - s) + value_len + i + 1;
    nv = calloc(1, len);
    if (!nv) {
        cwmp_log_error("%s: calloc(%"PRIuPTR") failed: %s",
                __func__,
                len, strerror(errno));
        return;
    }

    if (i) {
        snprintf(nv, len, "%s%*.0s%s", v, i, "", value);
        memset(nv + vlen, ';', i);
    } else {
        snprintf(nv, len, "%.*s%s%s", (s - v), v, value, e);
    }
    cwmp_nvram_set(key, nv);
    free(nv);

    return;
}

static void nvram_wlan_normalize(unsigned index, struct wlan_security_mode *wsm)
{
    if (wsm->mode == WLAN_BASIC) {
        /* normalization for Basic mode */
        if (wsm->authMode != WLAN_OPEN && wsm->authMode != WLAN_SHARED) {
            cwmp_log_info(
                    "WLANConfiguration: "
                    "set BasicAuthenticationMode to None for index %u", index + 1);
            wsm->authMode = WLAN_OPEN;
        }
        if (wsm->encrypt != WLAN_NO_ENCRYPTION && wsm->encrypt != WLAN_WEP) {
            cwmp_log_info(
                    "WLANConfiguration: "
                    "set BasicEncryptionModes to None for index %u", index  + 1);
            wsm->encrypt = WLAN_NO_ENCRYPTION;
        }
    } else if (wsm->mode == WLAN_WPA ||
            wsm->mode == WLAN_11i ||
            wsm->mode == WLAN_WPAand11i) {
        /* WPA/WPA2 */
        const char *v = NULL;
        switch (wsm->mode) {
            case WLAN_WPA:
                v = "WPA";
                break;
            case WLAN_11i:
            case WLAN_WPAand11i:
                v = "IEEE11i";
                break;
            default:
                v = "";
        }

        if (wsm->authMode != WLAN_PSK && wsm->authMode != WLAN_EAP) {
            cwmp_log_info(
                    "WLANConfiguration: "
                    "set %sAuthenticationMode to PSKAuthentication for index %u", v, index + 1);
            wsm->authMode = WLAN_PSK;
        }
        if (wsm->encrypt != WLAN_AES && wsm->encrypt != WLAN_TKIP && wsm->encrypt != WLAN_TKIPAES) {
            cwmp_log_info(
                    "WLANConfiguration: "
                    "set %sEncryptionModes to AESEncryption for index %u", v, index + 1);
            wsm->encrypt = WLAN_AES;
        }
    } else {
        /* unknown? */
        wsm->mode = WLAN_BASIC;
        wsm->authMode = WLAN_OPEN;
        wsm->encrypt = WLAN_NO_ENCRYPTION;
        cwmp_log_info(
                "WLANConfiguration: "
                "BasicAuthenticationMode as default for index %u", index + 1);
    }
}

static bool nvram_wlan_load(unsigned index, struct wlan_security_mode *wsm)
{
    char auth[128] = {};
    char encr[128] = {};
    if (!nvram_get_tuple("AuthMode", index, auth, sizeof(auth))) {
        cwmp_log_error("WLANConfiguration: undefined nvram's AuthMode value for index %u", index + 1);
    }
    if (!nvram_get_tuple("EncrypType", index, encr, sizeof(encr))) {
        cwmp_log_error("WLANConfiguration: undefined nvram's EncrypType value for index %u", index + 1);
    }

    /* load encryption */
    if (!strcmp(encr, "NONE")) {
        wsm->encrypt = WLAN_NO_ENCRYPTION;
    } else if(!strcmp(encr, "WEP")) {
        wsm->encrypt = WLAN_WEP;
    } else if (!strcmp(encr, "AES")) {
        wsm->encrypt = WLAN_AES;
    } else if (!strcmp(encr, "TKIP")) {
        wsm->encrypt = WLAN_TKIP;
    } else if (!strcmp(encr, "TKIPAES")) {
        wsm->encrypt = WLAN_TKIPAES;
    } else {
        cwmp_log_error(
            "WLANConfiguration: "
            "unknown nvram's EncrypType value for index %u: '%s'",
            index + 1, auth);
    }

    /* load auth mode and security mode */
    if (!strcmp(auth, "OPEN")) {
        wsm->authMode = WLAN_OPEN;
        wsm->mode = WLAN_BASIC;
    } else if (!strcmp(auth, "SHARED")) {
        wsm->authMode = WLAN_SHARED;
        wsm->mode = WLAN_BASIC;
    } else if (!strcmp(auth, "WEPAUTO")) {
        wsm->authMode = WLAN_SHARED;
        wsm->mode = WLAN_BASIC;
    } else if (!strcmp(auth, "WPA")) {
        wsm->authMode = WLAN_EAP;
        wsm->mode = WLAN_WPA;
    } else if (!strcmp(auth, "WPAPSK")) {
        wsm->authMode = WLAN_PSK;
        wsm->mode = WLAN_WPA;
    } else if(!strcmp(auth, "WPAPSKWPA2PSK")) {
        wsm->authMode = WLAN_PSK;
        wsm->mode = WLAN_WPAand11i;
    } else if (!strcmp(auth, "WPA1WPA2")) {
        wsm->authMode = WLAN_EAP;
        wsm->mode = WLAN_WPAand11i;
    } else if (!strcmp(auth, "WPA2")) {
        wsm->authMode = WLAN_EAP;
        wsm->mode = WLAN_11i;
    } else if (!strcmp(auth, "WPA2PSK")) {
        wsm->authMode = WLAN_PSK;
        wsm->mode = WLAN_11i;
    } else {
        wsm->authMode = WLAN_OPEN;
        wsm->mode = WLAN_BASIC;
        wsm->encrypt = WLAN_NO_ENCRYPTION;
        cwmp_log_error(
                "WLANConfiguration: "
                "unknown nvram's AuthMode value for index %u: '%s'",
                index + 1, auth);
    }
    return true;
}

static bool nvram_wlan_save(unsigned index, struct wlan_security_mode *wsm)
{
    const char *auth = NULL;
    const char *encr = NULL;
    struct wlan_security_mode wsm_orig = {};
    nvram_wlan_load(index, &wsm_orig);

    /* merge */
    if (wsm->authMode != WLAN_NULL) {
        switch (wsm->authMode) {
            case WLAN_OPEN:
            case WLAN_SHARED:
                wsm->mode = WLAN_BASIC;
                break;
            case WLAN_PSK:
            case WLAN_EAP:
                wsm->mode = WLAN_WPAand11i;
                break;
        }
        wsm->encrypt = wsm_orig.encrypt;
    } else if (wsm->encrypt != WLAN_NULL) {
        switch (wsm->encrypt) {
            case WLAN_NO_ENCRYPTION:
            case WLAN_WEP:
                wsm->mode = WLAN_BASIC;
                break;

            case WLAN_AES:
            case WLAN_TKIP:
            case WLAN_TKIPAES:
                wsm->mode = wsm_orig.mode;
                if (wsm->mode != WLAN_WPA && wsm->mode != WLAN_11i && wsm->mode != WLAN_WPAand11i)
                {
                    wsm->mode = WLAN_WPAand11i;
                }
                break;
        }
        wsm->authMode = wsm_orig.authMode;
    } else if (wsm->mode != WLAN_NULL) {
        wsm->encrypt = wsm_orig.encrypt;
        wsm->authMode = wsm_orig.authMode;
    }

    nvram_wlan_normalize(index, wsm);

    /* nvram AuthMode */
    switch (wsm->authMode) {
        case WLAN_OPEN:
            auth = "OPEN";
            break;
        case WLAN_SHARED:
            auth = "SHARED";
            break;
        case WLAN_PSK:
            if (wsm->mode == WLAN_WPA) {
                auth = "WPAPSK";
            } else if (wsm->mode == WLAN_11i) {
                auth = "WPA2PSK";
            } else if (wsm->mode == WLAN_WPAand11i) {
                auth = "WPAPSKWPA2PSK";
            }
            break;
        case WLAN_EAP:
            if (wsm->mode == WLAN_WPA) {
                auth = "WPA";
            } else if (wsm->mode == WLAN_11i) {
                auth = "WPA2";
            } else if (wsm->mode == WLAN_WPAand11i) {
                auth = "WPA1WPA2";
            }
            break;
    }
    /* nvram EncrypType */
    switch(wsm->encrypt) {
        case WLAN_NO_ENCRYPTION:
            encr = "NONE";
            break;
        case WLAN_WEP:
            encr = "WEP";
            break;
        case WLAN_AES:
            encr = "AES";
            break;
        case WLAN_TKIP:
            encr = "TKIP";
            break;
        case WLAN_TKIPAES:
            encr = "TKIPAES";
            break;
    }

    /* write */
    nvram_set_tuple("AuthMode", index, auth);
    nvram_set_tuple("EncrypType", index, encr);

    return true;
}

int cpe_get_igd_wlanc_autochannel(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();

    int chan =  cwmp_nvram_get_int("Channel", 0);
    int autoselect = cwmp_nvram_get_int("AutoChannelSelect",1);

    *value = pool_pstrdup(pool, ((chan==0) || autoselect)?"1":"0");

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_autochannel(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    if (value == NULL) {
        cwmp_log_error("%s: undefined value!", __func__);
        return FAULT_CODE_9002;
    }

    if (value[0] == '1') {
        cwmp_nvram_set("Channel","0");
        cwmp_nvram_set("AutoChannelSelect","1");
    }
    else
    {
        int chan =  cwmp_nvram_get_int("Channel", 0);
        cwmp_nvram_set("AutoChannelSelect","0");
        if (chan == 0) cwmp_nvram_set("Channel", "9");
    }

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_channel(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char* chan = NULL;

    DM_TRACE_GET();

    chan = cwmp_nvram_pool_get(pool, "Channel");
    if (chan == NULL)
    {
        chan = "0";
    }

    int autoselect = cwmp_nvram_get_int("AutoChannelSelect",1);

    if (autoselect)
    {
        *value = pool_pstrdup(pool, "0");
    }
    else
    {
        *value = pool_pstrdup(pool, chan);
    }

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_channel(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    if (value == NULL) {
        cwmp_log_error("%s: undefined value!", __func__);
        return FAULT_CODE_9002;
    }

    cwmp_nvram_set("Channel",value);

    if (value[0] == '0' && value[1] == '\0') {
        cwmp_nvram_set("AutoChannelSelect", "1");
    }
    else
    {
        cwmp_nvram_set("AutoChannelSelect", "0");
    }

    return FAULT_CODE_OK;
}


int cpe_get_igd_wlanc_standard(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char* stdstr;

    DM_TRACE_GET();

    int standard = cwmp_nvram_get_int("WirelessMode", 9);

    switch (standard) {

        case 0: stdstr = "g";break;
        case 1: stdstr = "b";break;
        case 4: stdstr = "g-only";break;
        case 6: stdstr = "n";break;
        case 7: stdstr = "b/g/n";break;
        case 9: stdstr = "b/g/n";break;
        default: stdstr = "b/g/n";break;
    }

    *value = pool_pstrdup(pool, stdstr);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_standard(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();

    char* valStr = "9";

    if (strcmp(value, "n") == 0) valStr="6";else
    if (strcmp(value, "g") == 0) valStr="0";else
    if (strcmp(value, "g-only") == 0) valStr="4";else
    if (strcmp(value, "b") == 0) valStr="1";

    cwmp_nvram_set("WirelessMode", valStr);

    return FAULT_CODE_OK;
}

//InternetGatewayDevice.LANDevice.WLANConfiguration.BasicAuthenticationMode
int cpe_get_igd_wlanc_basicauthmode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_GET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!nvram_wlan_load(index, &wsm)) {
        cwmp_log_error("%s: unable to load wlan", __func__);
        return FAULT_CODE_9002;
    }

    if (wsm.mode == WLAN_BASIC) {
        *value = "None";
        return FAULT_CODE_OK;
    }

    switch (wsm.authMode) {
        case WLAN_OPEN:
            *value = "None";
            break;
        case WLAN_SHARED:
            *value = "SharedAuthentication";
            break;
        case WLAN_EAP:
        case WLAN_PSK: // FIXME: check if this is correct
            *value = "EAPAuthentication";
            break;
        default:
            cwmp_log_error("%s: unknown authentication mode: %d",
                    __func__, wsm.authMode);
            return FAULT_CODE_9002;
    }

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_basicauthmode(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_SET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {

        return FAULT_CODE_9002;
    }

    if (!strcmp(value,"None")) {
        /* Open */
        wsm.authMode = WLAN_OPEN;
    } else if (!strcmp(value, "SharedAuthentication")) {
        /* Shared */
        wsm.authMode = WLAN_SHARED;
    } else if (!strcmp(value, "EAPAuthentication")) {
        /* not supported */
        cwmp_log_info("%s: (index %u) Radius auth not supported",
                __func__, index);
        wsm.authMode = WLAN_OPEN;
    } else {
        cwmp_log_error("%s: (index %u) invalid value: '%s'",
                __func__, index, value);
        return FAULT_CODE_9007;
    }
    nvram_wlan_save(index, &wsm);

    return FAULT_CODE_OK;
}

/* BasicEncryptionModes */
int cpe_get_igd_wlanc_basicencryption(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_GET();
    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!nvram_wlan_load(index, &wsm)) {
        return FAULT_CODE_9002;
    }

    if (wsm.mode != WLAN_BASIC) {
        *value = "None";
        return FAULT_CODE_OK;
    }

    switch (wsm.encrypt) {
        case WLAN_NO_ENCRYPTION:
            *value = "None";
            break;
        case WLAN_WEP:
            *value = "WEPEncryption";
            break;
        default:
            cwmp_log_error("%s: unknown encryption: %d",
                    __func__, wsm.encrypt);
            return FAULT_CODE_9002;
    }

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_basicencryption(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_SET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!strcmp(value, "None")) {
        wsm.encrypt = WLAN_NO_ENCRYPTION;
    } else if (!strcmp(value, "WEPEncryption")) {
        wsm.encrypt = WLAN_WEP;
    } else {
        cwmp_log_error("%s: invalid value: '%s'", __func__, value);
        return FAULT_CODE_9007;
    }

    nvram_wlan_save(index, &wsm);
    return FAULT_CODE_OK;
}

/* WPAAuthenticationMode */
int cpe_get_igd_wlanc_wpaauthmode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_GET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!nvram_wlan_load(index, &wsm)) {
        cwmp_log_error("%s: unable to load wlan", __func__);
        return FAULT_CODE_9002;
    }

    if (wsm.mode != WLAN_WPA && wsm.mode != WLAN_WPAand11i) {
        *value = "EAPAuthentication";
        return FAULT_CODE_OK;
    }

    switch (wsm.authMode) {
        case WLAN_PSK:
            *value = "PSKAuthentication";
            break;
        case WLAN_EAP:
            *value = "EAPAuthentication";
            break;
        default:
            return FAULT_CODE_9002;
    }
    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_wpaauthmode(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};
    /* only WPA1 */

    DM_TRACE_SET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!strcmp(value, "PSKAuthentication")) {
        wsm.authMode = WLAN_PSK;
    } else if (!strcmp(value, "EAPAuthentication")) {
        wsm.authMode = WLAN_EAP;
    } else {
        cwmp_log_error("%s: unknown value: %s", __func__, value);
        return FAULT_CODE_9007;
    }

    nvram_wlan_save(index, &wsm);

    return FAULT_CODE_OK;
}

/* WPAEncryptionModes */
int cpe_set_igd_wlanc_wpaencryption(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_SET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!strcmp(value, "TKIPEncryption")) {
        wsm.encrypt = WLAN_TKIP;
    } else if (!strcmp(value, "AESEncryption")) {
        wsm.encrypt = WLAN_AES;
    } else if (!strcmp(value, "TKIPandAESEncryption")) {
        wsm.encrypt = WLAN_TKIPAES;
    } else {
        cwmp_log_trace(
                "%s: invalid value '%s', supports only: "
                "TKIPEncryption, AESEncryption, TKIPandAESEncryption",
                __func__, value);
        return FAULT_CODE_9007;
    }
    nvram_wlan_save(index, &wsm);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_wpaencryption(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_GET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!nvram_wlan_load(index, &wsm)) {
        return FAULT_CODE_9002;
    }

    if (!(wsm.mode & WLAN_WPA)) {
        /* default value */
        *value = "AESEncryption";
        return FAULT_CODE_OK;
    }

    switch (wsm.encrypt) {
        case WLAN_TKIP:
            *value = "TKIPEncryption";
            break;
        case WLAN_AES:
            *value = "AESEncryption";
            break;
        case WLAN_TKIPAES:
            *value = "TKIPandAESEncryption";
            break;
        default:
            return FAULT_CODE_9002;
    }

    return FAULT_CODE_OK;
}

/* IEEE11iAuthenticationMode */
int cpe_get_igd_wlanc_ieeeauthmode(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};
    /* WPA2 only */
    DM_TRACE_GET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!nvram_wlan_load(index, &wsm)) {
        cwmp_log_error("%s: unable to load wlan", __func__);
        return FAULT_CODE_9002;
    }

    if (!(wsm.mode & WLAN_11i)) {
        *value = "EAPAuthentication";
        return FAULT_CODE_OK;
    }

    switch (wsm.authMode) {
        case WLAN_PSK:
            *value = "PSKAuthentication";
            break;
        case WLAN_EAP:
            *value = "EAPAuthentication";
            break;
        default:
            return FAULT_CODE_9002;
    }

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_ieeeauthmode(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned index = 0u;
    struct wlan_security_mode wsm = {};
    /* WPA2 */
    DM_TRACE_SET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!strcmp(value, "PSKAuthentication")) {
        wsm.authMode = WLAN_PSK;
    } else if (!strcmp(value, "EAPAuthentication")) {
        /* Radius auth */
        wsm.authMode = WLAN_EAP;
    } else {
        cwmp_log_error("%s: unknown value: %s", __func__, value);
        return FAULT_CODE_9007;
    }
    nvram_wlan_save(index, &wsm);

    return FAULT_CODE_OK;
}


int cpe_get_igd_wlanc_beacontype(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned index = -1u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_GET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!nvram_wlan_load(index, &wsm)) {
        cwmp_log_error("%s: unable to load wlan", __func__);
        return FAULT_CODE_9002;
    }

    switch (wsm.mode) {
        case WLAN_WPA:
            *value = "WPA";
            break;
        case WLAN_11i:
            *value = "11i";
            break;
        case WLAN_BASIC:
            *value = "Basic";
            break;
        case WLAN_WPAand11i:
            *value = "WPAand11i";
            break;
    }

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_beacontype(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned index = -1u;
    struct wlan_security_mode wsm = {};

    DM_TRACE_SET();

    if ((index = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!strcmp(value, "WPAand11i")) {
        /* WPA1WPA2 */
        wsm.mode = WLAN_WPAand11i;
    } else if (!strcmp(value, "11i")) {
        /* WPA2* */
        wsm.mode = WLAN_11i;
    } else if (!strcmp(value, "WPA")) {
        /* WPA* */
        wsm.mode = WLAN_WPA;
    } else if (!strcmp(value, "Basic")) {
        /* OPEN */
        wsm.mode = WLAN_BASIC;
    } else if(!strcmp(value, "None")) {
        /* disable station */
        /* TODO: ... */
        cwmp_log_info("%s: (index: %u) disabling WLAN not supported in this case",
                __func__, index);
        wsm.mode = WLAN_BASIC;
    } else {
        cwmp_log_error("%s: (index: %u) unknown value: '%s'",
                __func__, index, value);
    }
    nvram_wlan_save(index, &wsm);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_possiblechannels(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    int v = 0;

    DM_TRACE_GET();
    v = cwmp_nvram_get_int("CountryRegion", -1);

    switch (v) {
        case 0:
            *value = "1-11";
            break;
        case 1:
            *value = "1-13";
            break;
        case 2:
            *value = "10-11";
            break;
        case 3:
            *value = "10-13";
            break;
        case 4:
            *value = "14";
            break;
        case 5:
            *value = "1-14";
            break;
        case 6:
            *value = "3-9";
            break;
        case 7:
            *value = "5-13";
            break;
        default:
            cwmp_log_error("%s: error get CountryRegion nvram value", __func__);
            return FAULT_CODE_9002;
    }

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_status(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char *v = NULL;
    DM_TRACE_GET();
    v = cwmp_nvram_get("RadioOn");
    if (*v == '0') {
        *value = "Disabled";
    } else if (*v == '1') {
        *value = "Up";
    } else {
        *value = "Error";
        cwmp_log_error("%s: nvram RadioOn invalid value: '%s'", __func__, v);
    }
    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_enabled(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    DM_TRACE_SET();
    if (*value == '0') {
        cwmp_nvram_set("RadioOn", "0");
    } else if (*value == '1') {
        cwmp_nvram_set("RadioOn", "1");
    } else {
        cwmp_log_error("%s: invalid value: '%s'", __func__, value);
        return FAULT_CODE_9007;
    }
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_enabled(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    char *v = NULL;
    DM_TRACE_GET();
    v = cwmp_nvram_get("RadioOn");
    if (*v == '0') {
        *value = "0";
    } else if (*v == '1') {
        *value = "1";
    } else {
        *value = "0";
        cwmp_log_error("%s: nvram RadioOn invalid value: '%s'", __func__, v);
    }
    return FAULT_CODE_OK;
}

static void igd_lan_wlan_arp(const char *n, struct wlan_assoc *wa)
{
    char arp_addr[40];
    char arp_mac[18];
    FILE *f = fopen("/proc/net/arp", "r");
    int r = EOF;

    if (!f) {
        cwmp_log_error(
                "%s: fopen(\"/proc/net/arp\", \"r\") failure: %s",
                n, strerror(errno));
        return;
    }

	/* skip header */
	fscanf(f, "IP address HW type Flags HW address Mask Device");
	/* process */
	while ((r = fscanf(f, "%s %*x %*x %s %*s %*s",
				arp_addr, arp_mac)) != EOF) {
		/* skip invalid matching */
		if (r != 2)
			continue;
        if (!strcmp(wa->mac, arp_mac)) {
            memcpy(wa->addr, arp_addr, sizeof(wa->addr));
            break;
        }
    }
    fclose(f);
}

int cpe_refresh_igd_wlanc_associated(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    unsigned id = -1u;
    struct wlanc_node *w = NULL;
    char name[1024] = {};

	RT_802_11_MAC_TABLE table24 = {};
	RT_802_11_MAC_ENTRY *pe = NULL;
    int row_no = 0;
    parameter_node_t *pn = NULL;

    DM_TRACE_REFRESH();

    cwmp_get_parameter_fullpath(param_node, name, sizeof(name));

    /* get number of WLANConfiguration */
    if ((id = wlanc_get_id_node(param_node, name, &pn)) == -1u) {
        id = wlanc_get_id_node(pn, name, &pn);
    }

    if (id == -1) {
        return FAULT_CODE_9002;
    }

    /* get wlanc struct */
    w = wlanc_nget(wlanc.root, NULL, id);
    if (!w) {
        return FAULT_CODE_9002;
    }

    /* delete */
    cwmp_model_delete_object_child(cwmp, param_node);
    if (w->assoc) {
        free(w->assoc);
        w->assoc = NULL;
        w->assoc_count = 0u;
    }

    /* populate 2.4GHz */
	getWlanStationTable(&table24, 1);

    w->assoc = calloc(table24.Num, sizeof(*w->assoc));
    if (!w->assoc) {
        cwmp_log_error("%s: calloc(%d) failed: %s",
                __func__, table24.Num * sizeof(*w->assoc), strerror(errno));
        return FAULT_CODE_9002;
    }
    w->assoc_count = table24.Num;

    for (row_no = 0; row_no < table24.Num; row_no++) {
        pe = &(table24.Entry[row_no]);
        snprintf(w->assoc[row_no].mac,
                sizeof(w->assoc[row_no].mac),
                "%02x:%02x:%02x:%02x:%02x:%02x",
                pe->Addr[0], pe->Addr[1], pe->Addr[2],
                pe->Addr[3], pe->Addr[4], pe->Addr[5]);
        /* FIXME: too simple */
        igd_lan_wlan_arp(__func__, &w->assoc[row_no]);
		cwmp_model_copy_parameter(param_node, &pn, row_no + 1);
    }
    return FAULT_CODE_OK;
}

static struct wlan_assoc* igd_lan_wlan_no_from_path(cwmp_t *cwmp, const char *name)
{
    unsigned assoc_id = -1u;
    unsigned wlan_id = -1u;
    struct wlanc_node *w = NULL;
	parameter_node_t *pn = NULL;
	long no = 0;

    /* get assoc id */
    if ((assoc_id = wlanc_get_id(cwmp, name, &pn)) == -1u) {
        return NULL;
    }
    /* get WLANConfiguration id */
    if ((wlan_id = wlanc_get_id_node(pn, name, NULL)) == -1u) {
        return NULL;
    }

    /* get wlanc node */
    if (!(w = wlanc_nget(wlanc.root, NULL, wlan_id))) {
        return NULL;
    }

    /* check assoc */
	if (assoc_id >= w->assoc_count) {
		cwmp_log_error("%s: invalid rule number: %lu", name, no);
		return NULL;
	}

	return &w->assoc[assoc_id];
}


int cpe_get_igd_wlanc_assoc_mac(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    struct wlan_assoc *wa = NULL;
    DM_TRACE_GET();
    if (!(wa = igd_lan_wlan_no_from_path(cwmp, name))) {
        return FAULT_CODE_9002;
    }
    *value = pool_pstrdup(pool, wa->mac);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_assoc_addr(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    struct wlan_assoc *wa = NULL;
    DM_TRACE_GET();
    if (!(wa = igd_lan_wlan_no_from_path(cwmp, name))) {
        return FAULT_CODE_9002;
    }
    *value = pool_pstrdup(pool, wa->addr);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_assoc_state(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    DM_TRACE_GET();
    *value = "1";
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_associated_count(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    struct wlanc_node *w = NULL;
    char buf[42] = {};
    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (!(w = wlanc_nget(wlanc.root, NULL, id))) {
        return FAULT_CODE_9002;
    }

    snprintf(buf, sizeof(buf), "%u", w->assoc_count);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_key(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned id = -1u;
    char k[42] = {};

    DM_TRACE_SET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    snprintf(k, sizeof(k), "WPAPSK%u", id + 1);
    cwmp_nvram_set(k, value);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_key(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    char k[42] = {};

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    snprintf(k, sizeof(k), "WPAPSK%u", id + 1);
    *value = cwmp_nvram_pool_get(pool, k);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_wepkey_index(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned id = -1u;

    DM_TRACE_SET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    nvram_set_tuple("DefaultKeyID", id, value);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_wepkey_index(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    char v[42] = {};

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    nvram_get_tuple("DefaultKeyID", id, v, sizeof(v));

    *value = pool_pstrdup(pool, v);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_wepkey(cwmp_t *cwmp, const char *name, const char *value, int length, char *args, callback_register_func_t callback_reg)
{
    char tkey[42] = {};
    char key[42] = {};
    char *end = NULL;
    parameter_node_t *pn = NULL;
    unsigned wlan_id = 1;
    unsigned key_id = 0u;

    DM_TRACE_SET();
    if ((key_id = wlanc_get_id(cwmp, name, &pn)) == -1u) {
        return FAULT_CODE_9002;
    }

    if ((wlan_id = wlanc_get_id_node(pn, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    snprintf(tkey, sizeof(tkey), "Key%uType", key_id + 1);
    snprintf(key, sizeof(key), "Key%uStr%u", key_id + 1, wlan_id + 1);

    if (length != 10 && length != 26) {
        cwmp_log_trace("%s: invalid value length: %d, must be equal 10 or 26",
                __func__, length);
        return FAULT_CODE_9007;
    }

    strtoul(value, &end, 16);
    if (end && *end) {
        cwmp_log_error("%s: invalid hex: '%s' at symbol number %"PRIuPTR,
            __func__, value, end - value);
        return FAULT_CODE_9007;
    }

    nvram_set_tuple(tkey, wlan_id, "0");
    cwmp_nvram_set(key, value);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_wepkey(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned wlan_id = 1;
    char tkey[42] = {};
    char key[42] = {};
    char *val = NULL;
    char hex[27];
    char tval[42] = {};
    unsigned key_id = 0u;
    parameter_node_t *pn = NULL;

    DM_TRACE_GET();
    if ((key_id = wlanc_get_id(cwmp, name, &pn)) == -1u) {
        return FAULT_CODE_9002;
    }

    if ((wlan_id = wlanc_get_id_node(pn, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    snprintf(tkey, sizeof(tkey), "Key%uType", key_id + 1);
    snprintf(key, sizeof(key), "Key%uStr%u", key_id + 1, wlan_id + 1);

    nvram_get_tuple(tkey, wlan_id, tval, sizeof(tval));
    if (*tval == '0') {
        /* hex value */
        *value = cwmp_nvram_pool_get(pool, key);
    } else {
        /* ASCII value */
        val = cwmp_nvram_get(key);
        if (!*val) {
            return FAULT_CODE_OK;
        }
        cwmp_string_to_hex(val, hex, sizeof(hex));
        *value = pool_pstrdup(pool, hex);
    }
    return FAULT_CODE_OK;
}

static bool get_igd_lan_wlan_txrx(cwmp_t *cwmp, const char *name, struct nic_counts *result)
{
    unsigned id = -1u;
    int count = 0;
    int i = 0;
    struct nic_counts *nc = NULL;
    struct wlanc_node *w = NULL;

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return false;
    }

    if (!(w = wlanc_nget(wlanc.root, NULL, id))) {
        return false;
    }

    nc = nicscounts(&count);
    for (i = 0; i < count; i++) {
        if (!strcmp(nc[i].ifname, w->name)) {
            memcpy(result, &nc[i], sizeof(*result));
            goto success;
        }
    }

    if (nc)
        free(nc);
    return false;
success:
    free(nc);
    return true;
}

int cpe_get_igd_wlanc_tx_bytes(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    struct nic_counts nc = {};
    char buf[42] = {};
    DM_TRACE_GET();
    if (!get_igd_lan_wlan_txrx(cwmp, name, &nc)) {
        cwmp_log_error("%s: can't get counter for WLAN interface", __func__);
        return FAULT_CODE_9002;
    }
    snprintf(buf, sizeof(buf), "%llu", nc.tx_bytes);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_rx_bytes(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    struct nic_counts nc = {};
    char buf[42] = {};
    DM_TRACE_GET();
    if (!get_igd_lan_wlan_txrx(cwmp, name, &nc)) {
        cwmp_log_error("%s: can't get counter for WLAN interface", __func__);
        return FAULT_CODE_9002;
    }
    snprintf(buf, sizeof(buf), "%llu", nc.rx_bytes);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_tx_packets(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    struct nic_counts nc = {};
    char buf[42] = {};
    DM_TRACE_GET();
    if (!get_igd_lan_wlan_txrx(cwmp, name, &nc)) {
        cwmp_log_error("%s: can't get counter for WLAN interface", __func__);
        return FAULT_CODE_9002;
    }
    snprintf(buf, sizeof(buf), "%llu", nc.tx_packets);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_rx_packets(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    struct nic_counts nc = {};
    char buf[42] = {};
    DM_TRACE_GET();
    if (!get_igd_lan_wlan_txrx(cwmp, name, &nc)) {
        cwmp_log_error("%s: can't get counter for WLAN interface", __func__);
        return FAULT_CODE_9002;
    }
    snprintf(buf, sizeof(buf), "%llu", nc.rx_packets);
    *value = pool_pstrdup(pool, buf);
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_stats(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    parameter_node_t *pn = NULL;
    struct nic_counts nc = {};
    char buf[42] = "0";

    DM_TRACE_GET();
    if (!get_igd_lan_wlan_txrx(cwmp, name, &nc)) {
        cwmp_log_error("%s: can't get counter for WLAN interface", __func__);
        return FAULT_CODE_9002;
    }

    pn  = cwmp_get_parameter_path_node(cwmp->root, name);

    if (!strcmp("ErrorsSent", pn->name)) {
        snprintf(buf, sizeof(buf), "%llu", nc.tx_errs);
    } else if (!strcmp("ErrorsReceived", pn->name)) {
        snprintf(buf, sizeof(buf), "%llu", nc.rx_errs);
    } else if (!strcmp("UnicastPacketsSent", pn->name)) {
    } else if (!strcmp("UnicastPacketsReceived", pn->name)) {
    } else if (!strcmp("DiscardPacketsSent", pn->name)) {
        snprintf(buf, sizeof(buf), "%llu", nc.tx_drops);
    } else if (!strcmp("DiscardPacketsReceived", pn->name)) {
        snprintf(buf, sizeof(buf), "%llu", nc.rx_drops);
    } else if (!strcmp("MulticastPacketsSent", pn->name)) {
    } else if (!strcmp("MulticastPacketsReceived", pn->name)) {
        snprintf(buf, sizeof(buf), "%llu", nc.rx_multi);
    } else if (!strcmp("BroadcastPacketsSent", pn->name)) {
    } else if (!strcmp("BroadcastPacketsReceived", pn->name)) {
    } else if (!strcmp("UnknownProtoPacketsReceived", pn->name)) {
    } else {
        cwmp_log_error("%s: unknown node name: %s", __func__, pn->name);
        return FAULT_CODE_9002;
    }
    *value = pool_pstrdup(pool, buf);

    return FAULT_CODE_OK;
}

int cpe_get_wlanc_name(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    struct wlanc_node *w = NULL;

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    w = wlanc_nget(wlanc.root, NULL, id);
    if (!w) {
        return FAULT_CODE_9002;
    }

    *value = pool_pstrdup(pool, w->name);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_ssidadv(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned id = -1u;

    DM_TRACE_SET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    if (*value == '1') {
        nvram_set_tuple("HideSSID", id, "0");
    } else {
        nvram_set_tuple("HideSSID", id, "1");
    }
    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_ssidadv(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    char v[82] = {};

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    nvram_get_tuple("HideSSID", id, v, sizeof(v));
    if (*v == '1') {
        *value = "0";
    } else {
        *value = "1";
    }

    return FAULT_CODE_OK;
}

/* *** dynamic config *** */

int cpe_refresh_wlanc(cwmp_t * cwmp, parameter_node_t * param_node, callback_register_func_t callback_reg)
{
    int i = 0;
    int count = 0;
    struct nic_counts *nc = NULL;
    struct wlanc_node *w = NULL;
    parameter_node_t *new_param = NULL;

    DM_TRACE_REFRESH();
    /* free */
    cwmp_model_delete_object_child(cwmp, param_node);
    wlanc_free_all(wlanc.root);

    wlanc.root = NULL;
    wlanc.count = 0u;

    /* generate */
    nc = nicscounts(&count);
    for (i = 0; i < count; i++) {
        /* FIXME: hardcoded name */
        w = wlanc_nnew(wlanc.root, "ra", nc[i].ifname);
        if (!w) {
            continue;
        }
        cwmp_log_debug("%s: new parameter id:%i, name: %s", __func__, w->id + 1, nc[i].ifname);
        cwmp_model_copy_parameter(param_node, &new_param, w->id + 1);
        wlanc.count++;
        if (!wlanc.root) {
            wlanc.root = w;
        }
    }

    cwmp_model_refresh_object(cwmp, param_node, 0, callback_reg);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_bssid(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    struct wlanc_node *w = NULL;
    char mac[18] = {};

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    w = wlanc_nget(wlanc.root, NULL, id);
    if (!w) {
        return FAULT_CODE_9002;
    }

    if (getIfMac(w->name, mac, ':') == -1)
        *value = pool_pstrdup(pool,"00:00:00:00:00:00");
    else {
        *value = pool_pstrdup(pool, mac);
    }

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_ssid(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    char ssid_id[128] = {};
    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    snprintf(ssid_id, sizeof(ssid_id), "SSID%u", id + 1);
    *value = cwmp_nvram_pool_get(pool, ssid_id);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_ssid(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned id = -1u;
    char ssid_id[128] = {};

    DM_TRACE_SET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    snprintf(ssid_id, sizeof(ssid_id), "SSID%u", id + 1);
    cwmp_nvram_set(ssid_id, value);

    return FAULT_CODE_OK;
}

int cpe_set_igd_wlanc_pskfailures(cwmp_t * cwmp, const char * name, const char * value, int length, char *args, callback_register_func_t callback_reg)
{
    unsigned id = -1u;

    DM_TRACE_SET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    nvram_set_tuple("RekeyInterval", id, value);

    return FAULT_CODE_OK;
}

int cpe_get_igd_wlanc_pskfailures(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool)
{
    unsigned id = -1u;
    char v[82] = {};

    DM_TRACE_GET();

    if ((id = wlanc_get_id(cwmp, name, NULL)) == -1u) {
        return FAULT_CODE_9002;
    }

    nvram_get_tuple("RekeyInterval", id, v, sizeof(v));

    *value = pool_pstrdup(pool, v);
    return FAULT_CODE_OK;
}

