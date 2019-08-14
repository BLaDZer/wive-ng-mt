#include "asp_mod_utils.h"

static int isDemoMode(webs_t *wp, char** params, int nparams)
{
#if defined(DEMO_MODE)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int getFirmwareUpdateTimeAsp(webs_t *wp, char** params, int nparams)
{
	int reset_rwfs = (nparams > 0) && strToIntDef(params[0],0);
	return outWrite("%i", getFirmwareUpdateTime(reset_rwfs));
}

static int getUpdaterState(webs_t *wp, char** params, int nparams)
{
	char buf[256] = {0};
	read_from_file("/tmp/updater/state", buf, sizeof(buf));
	return outWrite("%s", buf);
}

static int getUpdaterImageName(webs_t *wp, char** params, int nparams)
{
	char buf[256] = {0};
	read_from_file("/tmp/updater/image_name", buf, sizeof(buf));
	return outWrite("%s", buf);
}


static int _getCfgGeneral(webs_t *wp, char* key, char** out)
{
    if (key == NULL) return NGX_ERROR;

    char* nvram_key = cstring_trim(wp->pool, key, ' ');
    if (nvram_key == NULL) return NGX_ERROR;

//    ELOG_DEBUG(wp->log, 0, "getCfgGeneral key = %s\n", nvram_key);

    char *val = ngx_nvram_get(wp, nvram_key);

//    if ((!val) && (strcmp(nvram_key, "Language") == 0)) {
//        ELOG_ERR(wp->log, 0, "Unknown lang %s. Set lang to en, %s \n", val, __FUNCTION__);
//        val = "en";
//    }

//    ELOG_DEBUG(wp->log, 0, "getCfgGeneral: %s = %s\n", nvram_key, val);

    *out = val;
    return NGX_OK;
}

static char* escapeJsString(char* str)
{
    if (str == NULL) return NULL;

    int len = strlen(str);
    char* out = calloc(len*2+1, sizeof(char)); // it means you should never add more than 2 escaping symbols per input symbol
    if (out == NULL) return NULL;

    char* in_ptr = str;
    char* out_ptr = out;
    while (in_ptr[0] != '\0') {
        char c = *in_ptr;
        if (c == '\'' || c == '\\' || c == '"')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = c;
            out_ptr++;
        }
        else if (c == '\t')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = 't';
            out_ptr++;
        }
        else if (c == '\r')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = 'r';
            out_ptr++;
        }
        else if (c == '\n')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = 'n';
            out_ptr++;
        }
        else if (c == '\v')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = 'v';
            out_ptr++;
        }
        else if (c == '\f')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = 'f';
            out_ptr++;
        }
        else if (c == '\b')
        {
            out_ptr[0] = '\\';
            out_ptr++;
            out_ptr[0] = 'b';
            out_ptr++;
        }
        else
        {
            out_ptr[0] = c;
            out_ptr++;
        }

        in_ptr++;
    }

    return out;
}


static int getCfgGeneral(webs_t *wp, char** params, int nparams)
{
    if (nparams < 2)
    {
        return outWrite(T("Insufficient args\n"));
    }

    char* def = "";

    if (nparams > 2)
    {
        def = params[2];
    }

    // Deprecated zero-type function usage
    int type = strToIntDef(params[0], 0);
    if (type == 0)
    {
        ELOG_ERR(wp->log, 0, "STUB TYPE-0 FUNCTION USED: %s \n", __FUNCTION__);
        return NGX_ERROR;
    }

    char* val = NULL;
    int ret = _getCfgGeneral(wp, params[1], &val);
    if (ret != NGX_OK || val == NULL || val[0] == '\0')
    {
        char* esc_def = escapeJsString(def);
        outWrite("%s", esc_def);
        free(esc_def);
        return NGX_ERROR;
    }

    char* esc_val = escapeJsString(val);
    outWrite("%s", esc_val);
    free(esc_val);
    return NGX_OK;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> ""
 */
static int getCfgGeneralHTML(webs_t *wp, char** params, int nparams)
{
    if (nparams < 2) 
    {
        return outWrite(T("Insufficient args\n"));
    }

    // Deprecated zero-type function usage
    int type = strToIntDef(params[0], 0);
    if (type == 0)
    {
        ELOG_ERR(wp->log, 0, "STUB TYPE-0 FUNCTION USED: %s \n", __FUNCTION__);
        return NGX_ERROR;
    }

    char* val = NULL;

    int ret = _getCfgGeneral(wp, params[1], &val);
    if (ret != NGX_OK || val == NULL)
    {
        return NGX_ERROR;
    }

    ngx_str_t str;
    str.data = (unsigned char*)val;
    str.len = strlen(val);

    ngx_str_t escaped_str = n_escape_html(wp->pool, str);
    outWrite("%.*s", escaped_str.len, escaped_str.data);
    return NGX_OK;

// Previous html-escaping function:
/*
	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2)
		return outWrite(T("Insufficient args\n"));

	value = nvram_get(RT2860_NVRAM, field);

	if ((!value) && (strcmp(field, "Language") == 0))
	{
		syslog(LOG_ERR, "Unknown lang %s. Set lang to en, %s", value, __FUNCTION__);
		value = "en";
	}

	if (type == 1)
	{
		if (!value)
			return outWrite(T(""));

		int result = 0;
		for (; *value != '\0'; value++)
		{
			switch (*value)
			{
				case '&':
					result += outWrite(T("&amp;"));
					break;
				case '\"':
					result += outWrite(T("&quot;"));
					break;
				case '<':
					result += outWrite(T("&lt;"));
					break;
				case '>':
					result += outWrite(T("&gt;"));
					break;
				default:
					result += outWrite(T("%c"), *value);
					break;
			}
		}
		return result;
	}

	if (!value)
		ejSetResult(eid, "");
	else
		ejSetResult(eid, value);

	return 0;*/
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "")
 *              parse it and return the Nth value delimited by semicolon
 */
static int getCfgNthGeneral(webs_t *wp, char** params, int nparams)
{

    if (nparams < 3) 
    {
        return outWrite(T("Insufficient args\n"));
    }

    // Deprecated zero-type function usage
    int type = strToIntDef(params[0], 0);
    if (type == 0)
    {
        ELOG_ERR(wp->log, 0, "STUB TYPE-0 FUNCTION USED: %s \n", __FUNCTION__);
        return NGX_ERROR;
    }

    char* field = params[1];
    int idx = strToIntDef(params[2], -1);

    if (idx == -1)
    {
        return NGX_ERROR;
    }

    char* val = NULL;

    int ret = _getCfgGeneral(wp, field, &val);
    if (ret != NGX_OK || val == NULL)
    {
        return NGX_ERROR;
    }

    char* esc_val = escapeJsString(val);
    outWrite("%s", esc_val);
    free(esc_val);
    return NGX_OK;
}

static int writeIfCfgZeroEq(webs_t *wp, char** params, int nparams)
{
    if (nparams < 3)
    {
        return outWrite(T("Insufficient args\n"));
    }

    char* key = params[0];
    char* desired_val = params[1];
    char* output_text = params[2];
    char* else_text = NULL;

    if (nparams > 3)
    {
        else_text = params[3];
    }

    char* val = NULL;
    int ret = _getCfgGeneral(wp, key, &val);

    if (ret != NGX_OK || val == NULL)
    {
        val = "0";
    }

    if (val[0] == '\0')
    {
        val = "0";
    }

    if (strcmp(val, desired_val) == 0)
    {
        char* esc_val = escapeJsString(output_text);
        outWrite("%s", esc_val);
        free(esc_val);
    }
    else if (else_text != NULL)
    {
        char* esc_val = escapeJsString(else_text);
        outWrite("%s", esc_val);
        free(esc_val);
    }

    return NGX_OK;
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 * description: read general configurations from nvram
 *              if value is NULL -> "0"
 */
static int getCfgZero(webs_t *wp, char** params, int nparams)
{
    if (nparams < 2)
    {
        return outWrite(T("Insufficient args\n"));
    }

    // Deprectaed zero-type function usage
    int type = strToIntDef(params[0], 0);
    if (type == 0)
    {
        ELOG_ERR(wp->log, 0, "STUB TYPE-0 FUNCTION USED: %s \n", __FUNCTION__);
        return NGX_ERROR;
    }

    char* val = NULL;
    int ret = _getCfgGeneral(wp, params[1], &val);

    if (ret != NGX_OK || val == NULL)
    {
        outWrite("0");
        return NGX_ERROR;
    }

    if (val[0] == '\0')
    {
        outWrite("0");
        return NGX_OK;
    }

    char* esc_val = escapeJsString(val);
    outWrite("%s", esc_val);
    free(esc_val);

    return NGX_OK;

/*	int type;
	char_t *field;
	char *value;

	if (ejArgs(argc, argv, T("%d %s"), &type, &field) < 2) {
		return outWrite(T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return outWrite(T("0"));
		return outWrite(T("%s"), value);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;*/
}

/*
 * arguments: type - 0 = return the configuration of 'field' (default)
 *                   1 = write the configuration of 'field'
 *            field - parameter name in nvram
 *            idx - index of nth
 * description: read general configurations from nvram (if value is NULL -> "0")
 *              parse it and return the Nth value delimited by semicolon
 */
/*
static int getCfgNthZero(webs_t *wp, char** params, int nparams)
{
	int type, idx;
	char_t *field;
	char *value;
	char *nth;

	if (ejArgs(argc, argv, T("%d %s %d"), &type, &field, &idx) < 3) {
		return outWrite(T("Insufficient args\n"));
	}
	value = (char *) nvram_get(RT2860_NVRAM, field);
	if (1 == type) {
		if (!strcmp(value, ""))
			return outWrite(T("0"));
		nth = getNthValue(idx, value);
		if (NULL == nth)
			return outWrite(T("0"));
		return outWrite(T("%s"), nth);
	}
	if (!strcmp(value, ""))
		ejSetResult(eid, "0");
	nth = getNthValue(idx, value);
	if (NULL == nth)
		ejSetResult(eid, "0");
	ejSetResult(eid, value);
	return 0;
}
*/

static int getLangBuilt(webs_t *wp, char** params, int nparams)
{
    char *lang;

    if (nparams < 1) 
    {
        outWrite(T("0"));
        return NGX_OK;
    }

    lang = cstring_trim(wp->pool, params[0], '"');

    if (!strncmp(lang, "en", 3))
    {
        outWrite(T("1"));
    }
    else if (!strncmp(lang, "ru", 3))
    {
#ifdef CONFIG_USER_WEB_LANG_RU
        outWrite(T("1"));
#else
        outWrite(T("0"));
#endif
    }
    else outWrite(T("0"));

    return NGX_OK;
}

static int getLangDictionary(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_USER_WEB_LANG_RU
	char* lang = nvram_get(RT2860_NVRAM, "Language");

	if (!strncmp(lang, "ru", 3))
		return outWrite(T("RU"));
	else
#endif
		return outWrite(T("EN"));
}

/*
 * description: write the current system platform accordingly
 */
static int getPlatform(webs_t *wp, char** params, int nparams)
{
    return outWrite("%s", T(getSystemPlatform()));
}

static int getStationBuilt(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT76X2_STA) || defined(CONFIG_MT76X2_STA_MODULE)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

static int gigaphy(webs_t *wp, char** params, int nparams)
{
#if defined(CONFIG_MT7530_GSW)
	return outWrite(T("1"));
#else
	return outWrite(T("0"));
#endif
}

/*
 * description: write RT288x SDK version
 */
static int getSdkVersion(webs_t *wp, char** params, int nparams)
{
	return outWrite(T("%s"), VERSIONPKG);
}

/*
 * description: write System Uptime
 */
static int getSysUptime(webs_t *wp, char** params, int nparams)
{
	struct sysinfo sinf;

	if (sysinfo(&sinf) == 0)
		return outWrite(T("{ \"day\":\"%d\", \"hour\":\"%d\", \"min\":\"%d\", \"sec\":\"%d\" }"),
			sinf.uptime/(unsigned)86400,
			(sinf.uptime%(unsigned)86400)/(unsigned)3600,
			(sinf.uptime%(unsigned)3600)/(unsigned)60,
			sinf.uptime%(unsigned)60);
	else
		return -1;
}

/*
 * description: write Current date, time
 */
static int getSysDateTime(webs_t *wp, char** params, int nparams)
{
	struct tm *utime;
	time_t usecs;

	time(&usecs);
	utime = localtime(&usecs);
	if (utime == NULL)
	    return -1;

	return outWrite(T("%02u:%02u:%02u %02u.%02u.%04u"),
				utime->tm_hour, utime->tm_min, utime->tm_sec,
                utime->tm_mday, (utime->tm_mon + 1), (1900 + utime->tm_year));
}

/* goform/setOpMode */
static void setOpMode(webs_t* wp, char_t *path, char_t *query)
{
	char *mode, *old_mode;

	nvram_init(RT2860_NVRAM);

	/* get modes */
	mode = websGetVar(wp, T("opMode"), T("0"));
	old_mode = nvram_get(RT2860_NVRAM, "OperationMode");

	/* new OperationMode */
	if (strncmp(mode, old_mode, 2))
	{
		ngx_nvram_bufset(wp, "OperationMode", mode);

		/* do not allow dns from dhcp in ap-bridge mode */
		char* dns_profile = ngx_nvram_get(wp, "wan_static_dns_profile");
		if (!strncmp(mode, "0", 2) && !strcmp(dns_profile, "auto")) {
			ngx_nvram_bufset(wp, "wan_static_dns_profile", "manual");
			ngx_nvram_bufset(wp, "wan_static_dns", "on");
		}

		/* from or to ap client mode */
		if (!strncmp(mode, "3", 2))
			ngx_nvram_bufset(wp, "ApCliEnable", "1");
		else if (!strncmp(old_mode, "3", 2)) {
			/* from apcli to other mode need drop bridge only flag to */
			ngx_nvram_bufset(wp, "ApCliEnable", "0");
			ngx_nvram_bufset(wp, "ApCliBridgeOnly", "0");
		}
		nvram_commit(RT2860_NVRAM);
	}
	nvram_close(RT2860_NVRAM);

	/* Output timer for reloading */
        websHeader(wp);
	outputTimerForReload(wp, 60000);
        websFooter(wp);

	/* Reboot */
        wp->on_response_ok = DO_REBOOT;
}

static int getEthernetPortCount(webs_t *wp, char** params, int nparams)
{
#ifdef CONFIG_RTESW_SWITCH_ONEPORT
	return outWrite(T("1"));
#elif CONFIG_RTESW_SWITCH_TWOPORT
	return outWrite(T("2"));
#elif CONFIG_RTESW_SWITCH_THRPORT
	return outWrite(T("3"));
#else
	return outWrite(T("5"));
#endif
}

static void setEthernetPort(webs_t* wp, char_t *path, char_t *query)
{
	int i;
	char w_name[20];
	char_t *w_port = websGetVar(wp, T("wan_port"), T("0"));
	char_t *l_port = websGetVar(wp, T("lan_port"), T("near"));
	char_t *reboot = websGetVar(wp, T("reboot"), T("0"));
	char_t *reset  = websGetVar(wp, T("reset"), T("near"));

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 7, "wan_port", "lan_port", "port1_swmode", "port2_swmode", "port3_swmode", "port4_swmode", "port5_swmode");
		nvram_fromdef(RT2860_NVRAM, 5, "port1_fcmode", "port2_fcmode", "port3_fcmode", "port4_fcmode", "port5_fcmode");
	}
	else {
		nvram_init(RT2860_NVRAM);

		/* Set-up WAN port */
		if ((w_port != NULL) && (strlen(w_port) == 1)) {
			if ((w_port[0] >= '0') && (w_port[0] <= '4'))
				ngx_nvram_bufset(wp, "wan_port", w_port);
		}

		/* Set-up first LAN port */
		if ((l_port != NULL) && ((strcmp(l_port, "near") == 0) || (strcmp(l_port, "distant") == 0)))
			ngx_nvram_bufset(wp, "lan_port", l_port);

		/* Now test values */
		for (i = 1; i <= 5; i++) {
			snprintf(w_name, sizeof(w_name), "port%d_swmode", i);
			w_port = websGetVar(wp, w_name, T("auto"));
			if ((w_port != NULL) && (strlen(w_port)>0))
				ngx_nvram_bufset(wp, w_name, w_port);
			snprintf(w_name, sizeof(w_name), "port%d_fcmode", i);
			w_port = websGetVar(wp, w_name, T("auto"));
			if ((w_port != NULL) && (strlen(w_port)>0))
				ngx_nvram_bufset(wp, w_name, w_port);
		}

		/* Commit & close NVRAM */
		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	websDone(wp, 200);

	if (CHK_IF_DIGIT(reboot, 1)) {
                wp->on_response_ok = DO_REBOOT;
	}
}

/* goform/reboot */
static void reboot_web(webs_t* wp, char_t *path, char_t *query)
{
    /* Output timer for reloading */
    websHeader(wp);
    outputTimerForReload(wp, 60000);
    websFooter(wp);
    websDone(wp, 200);

    /* only by save and reboot logic must save rwfs */
    doSystem("fs save");

    /* Reboot */
    wp->on_response_ok = DO_REBOOT;
}

static void settingsUploadForm(webs_t* wp, char_t *path, char_t *query)
{
    ELOG_INFO(wp->request->connection->log, 0, "Settings update has been started.\n");

    upload_html_header(wp);

    char err_msg[512];
    long file_size = 0;

    char *filename = websGetVar(wp, "filename.path", NULL);
    char *filesize = websGetVar(wp, "filename.size", NULL);

    if (filesize != NULL)
    {
        file_size = strToIntDef(filesize, 0);
    }

    if (filename == NULL)
    {
        upload_html_error(wp, "No settings filename provided!");
        return; //1;
    }

    FILE *fd = fopen(filename, "r");
    if (fd == NULL)
    {
        upload_html_error(wp, "Unable to open settings file!");
        return;// 4;
    }

    fseek(fd, 0L, SEEK_END);
    unsigned long sz = ftell(fd);
    fclose(fd);

    if (sz != (unsigned long)file_size)
    {
        sprintf(err_msg, "Wrong settings filesize %lu != %ld!", sz, file_size);
        ELOG_DEBUG(wp->request->connection->log, 0, "%s", err_msg);
        upload_html_error(wp, err_msg);
        return;// 2;
    }

    // Find parameter containing file
    char* find = websGetVar(wp, "filename.name", NULL);
    if (find == NULL)
    {
        upload_html_error(wp, "No settings file!");
        return;// 7;
    }

    // settings import
    int errcode = import_settings(filename, 0, (int)file_size);
    if (errcode != 0)
    {
        sprintf(err_msg, "Settings import error (err=%i)", errcode);
        ELOG_DEBUG(wp->request->connection->log, 0, "%s", err_msg);
        upload_html_error(wp, err_msg);

        return;// -1;
    }

    ELOG_INFO(wp->request->connection->log, 0, "Settings import OK\n");

    upload_html_success(wp, 60);
    wp->on_response_ok = DO_REBOOT;

    return;// 0;
}


static void rwfsUploadForm(webs_t* wp, char_t *path, char_t *query)
{
    ELOG_INFO(wp->request->connection->log, 0, "RWFS update has been started.\n");

    upload_html_header(wp);

    char err_msg[512];
    long file_size = 0;

    char *filename = websGetVar(wp, "filename.path", NULL);
    char *filesize = websGetVar(wp, "filename.size", NULL);

    if (filesize != NULL)
    {
        file_size = strToIntDef(filesize, 0);
    }

    if (filename == NULL)
    {
        upload_html_error(wp, "No rwfs filename provided!");
        return;// 1;
    }

    FILE *fd = fopen(filename, "r");
    if (fd == NULL)
    {
        upload_html_error(wp, "Unable to open RWFS file!");
        return;// 4;
    }

    fseek(fd, 0L, SEEK_END);
    unsigned long sz = ftell(fd);
    fclose(fd);

    if (sz != (unsigned long)file_size)
    {
        sprintf(err_msg, "Wrong rwfs filesize %lu != %ld!", sz, file_size);
        ELOG_DEBUG(wp->request->connection->log, 0, "%s", err_msg);
        upload_html_error(wp, err_msg);

        return;// 2;
    }

    // Find parameter containing file
    char* find = websGetVar(wp, "filename.name", NULL);
    if (find == NULL)
    {
        upload_html_error(wp, "No RWFS file!");
        return;// 7;
    }

    // rwfs import
    int errcode = import_rwfs(filename, 0, (int)file_size);
    if (errcode != 0)
    {
        sprintf(err_msg,"RWFS import error %i", errcode);
        ELOG_DEBUG(wp->request->connection->log, 0, "%s", err_msg);
        upload_html_error(wp, err_msg);

        return;// -1;
    }

    ELOG_INFO(wp->request->connection->log, 0, "RWFS import OK\n");
    upload_html_success(wp, 60);
    wp->on_response_ok = DO_REBOOT;

    return;// 0;
}

static void firmwareUploadAutoForm(webs_t* wp, char_t *path, char_t *query)
{
	char* cmd = websGetVar(wp, "cmd", "");
	FILE* fp = fopen("/tmp/updater/command", "wt");
	if (fp) {
		fwrite(cmd, 1, strlen(cmd), fp);
		fclose(fp);
	}

	websDone(wp, 200);
}

static void firmwareUploadForm(webs_t* wp, char_t *path, char_t *query)
{
    ELOG_DEBUG(wp->request->connection->log, 0, "firmwareUploadForm");

    upload_html_header(wp);

    if (wp->args == NULL)
    {
        upload_html_error(wp, "No firmware file available!");
        return;
    }

    int errcode = firmware_upgrade(wp);

    if (errcode == 0)
    {
//      wp->do_reboot = 1;
    }
    else
    {
        ELOG_DEBUG(wp->request->connection->log, 0, "firmware upgrade error: %i!\n", errcode);
    }

    websDone(wp, 200);
}


static void shadowAuth(webs_t* wp, char_t *path, char_t *query)
{
    char buf[512] = {0};

    // body cleanup
    ngx_array_destroy(wp->out);
    wp->out = ngx_array_create(wp->pool, 128, sizeof(char));

    char_t *username  = websGetVar(wp, T("username"), NULL);
    char_t *password  = websGetVar(wp, T("password"), NULL);

    char* client_addr = ngx_to_cstring(wp->pool, wp->request->connection->addr_text);


    if (!username || !password || check_shadow_pass(username, password) != 0 )
    {
        if (username == NULL) username = "";
        ELOG_WARN(wp->request->connection->log, 0, "Authentication failed: user='%s'\n", username);
        websRedirect(wp, "/login.asp");
        websAddHeader(wp, "Set-Cookie", "Wrong-Pass=1; Max-Age=10; Path=/");
        return;
    }

    ELOG_INFO(wp->request->connection->log, 0, "Authentication successful: user=%s\n", username);

    auth_session_t* auth_session = NULL;

    auth_session = createAuthSession(client_addr, username);

    snprintf(buf, sizeof(buf), "sessionid=%s; Max-Age=3600; Path=/", auth_session->sessionid);
    buf[sizeof(buf)-1] = '\0';
    websAddHeader(wp, "Set-Cookie", buf);


    char* submitUrl = websGetVar(wp, T("submit-url"), T("/"));   // hidden page
    websRedirect(wp, submitUrl);

    return;

// DEPRECATED COOKIE-HASH PRIMITIVE AUTH ALGO
/*
    char* username;
    char* password;

    username = getCookie(wp->request, "Auth-Username");
    password = getCookie(wp->request, "Auth-Password");

    if (username && password && check_shadow_pass(username, password) == 0 )
    {
        websRedirect(wp, "/");
        return;
    }

    if (getBasicAuthCredentials(wp, &username, &password) != 0)
    {
        goto unauthorized;
    }

    if (check_shadow_pass(username, password) != 0 )
    {
        goto unauthorized;
    }

// successfully authorized
//    wp->auth_username = username;
//    wp->auth_role = get_user_role(username);

    char* crpass = crypt_pass(username, password);

    char* reauth_flag = getCookie(wp->request, "Reauth");

    if (reauth_flag && reauth_flag[0] == '1')
    {

        snprintf(buf, sizeof(buf), "Auth-Username=%s; Max-Age=3600; Path=/", username);
        buf[sizeof(buf)-1] = '\0';
        websAddHeader(wp, "Set-Cookie", buf);

        snprintf(buf, sizeof(buf), "Auth-Password=%s; Max-Age=3600; Path=/", crpass);
        buf[sizeof(buf)-1] = '\0';
        websAddHeader(wp, "Set-Cookie", buf);

        websAddHeader(wp, "Set-Cookie", "Reauth=0; Max-Age=10; Path=/");
        websRedirect(wp, "/");
    }
    else
    {
        websAddHeader(wp, "Set-Cookie", "Reauth=1; Max-Age=10; Path=/");
    }

//    char* submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page

    websAddHeader(wp, "WWW-Authenticate", "Basic realm=\"ROUTER\"");
    outWrite("Unauthorized");
    websDone(wp, 401);
//    websRedirect(wp, "/goform/auth");

    return;

unauthorized:
    wp->auth_username = NULL;
    wp->auth_role = DENY;

//    snprintf(buf, sizeof(buf), "Basic realm=\"realm_%lu\"", (unsigned long)time(NULL));
//    websAddHeader(wp, "WWW-Authenticate", buf);
*/
//    websAddHeader(wp, "WWW-Authenticate", "Basic realm=\"ROUTER\"");
//    outWrite("Unauthorized");
//    websDone(wp, 200); // 401

//    return;


/*
    if (wp->request->headers_in.authorization != NULL)
    {
        outWrite("OK");
        websDone(wp, 200);
        return;
    }

    websAddHeader(wp, "WWW-Authenticate", "Basic realm=\"My server\"");
    outWrite("Unauthorized");

    websDone(wp, 401);*/
}

static int webLogout(webs_t *wp, char** params, int nparams)
{
    if (wp->auth_session != NULL)
    {
        wp->auth_session->start_time = 0;
    }

//    websAddHeader(wp, "Set-Cookie", "sessionid=; Expires=Thu, 01 Jan 1970 00:00:01 GMT; Path=/");
    return NGX_OK;
}


static int getAuthUsername(webs_t *wp, char** params, int nparams)
{
    if (wp->auth_session == NULL)
    {
        return NGX_OK;
    }

    outWrite("%s", wp->auth_session->username);
    return NGX_OK;
}

static int getAuthRole(webs_t *wp, char** params, int nparams)
{
    if (wp->auth_session == NULL)
    {
        outWrite("0");
        return NGX_OK;
    }

    outWrite("%d", wp->auth_session->role);
    return NGX_OK;
}

/* goform/ping */
static void form_ping(webs_t* wp, char_t *path, char_t *query)
{
	/* do nothing */
}


/*********************************************************************
 * Web Related Utilities
 */
void asp_mod_utils_init()
{
        aspDefineFunc("isDemoMode", isDemoMode, EVERYONE);

        aspDefineFunc("getUpdaterState", getUpdaterState, EVERYONE);
        aspDefineFunc("getUpdaterImageName", getUpdaterImageName, EVERYONE);

        aspDefineFunc("getCfgGeneral", getCfgGeneral, EVERYONE);
        aspDefineFunc("getAuthUsername", getAuthUsername, EVERYONE);
        aspDefineFunc("getAuthRole", getAuthRole, EVERYONE);

        aspDefineFunc("writeIfCfgZeroEq", writeIfCfgZeroEq, EVERYONE);

	aspDefineFunc("getCfgGeneralHTML", getCfgGeneralHTML, EVERYONE);
	aspDefineFunc("getCfgNthGeneral", getCfgNthGeneral, EVERYONE);
	aspDefineFunc("getCfgZero", getCfgZero, EVERYONE);
//	aspDefineFunc("getCfgNthZero", getCfgNthZero, EVERYONE);
	aspDefineFunc("getLangBuilt", getLangBuilt, EVERYONE);
	aspDefineFunc("getLangDictionary", getLangDictionary, EVERYONE);
	aspDefineFunc("getPlatform", getPlatform, EVERYONE);
	aspDefineFunc("getStationBuilt", getStationBuilt, EVERYONE);
	aspDefineFunc("getSdkVersion", getSdkVersion, EVERYONE);
	aspDefineFunc("getSysUptime", getSysUptime, EVERYONE);
	aspDefineFunc("getSysDateTime", getSysDateTime, EVERYONE);
	aspDefineFunc("getEthernetPortCount", getEthernetPortCount, EVERYONE);
	websFormDefine(T("setOpMode"), setOpMode, ADMIN);
	websFormDefine(T("setEthernetPort"), setEthernetPort, ADMIN);
	websFormDefine(T("reboot"), reboot_web, EVERYONE);
	aspDefineFunc("gigaphy", gigaphy, EVERYONE);

	websFormDefine("auth", shadowAuth, EVERYONE);
	aspDefineFunc("webLogout", webLogout, EVERYONE);

	aspDefineFunc("getFirmwareUpdateTime", getFirmwareUpdateTimeAsp, EVERYONE);
	websFormDefine("firmwareUploadForm", firmwareUploadForm, ADMIN);
	websFormDefine("firmwareUploadAutoForm", firmwareUploadAutoForm, ADMIN);
	websFormDefine("rwfsUploadForm", rwfsUploadForm, ADMIN);
	websFormDefine("settingsUploadForm", settingsUploadForm, ADMIN);

	websFormDefine("ping", form_ping, EVERYONE);

}
