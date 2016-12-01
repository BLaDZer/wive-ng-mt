/* vim: set et: */
/************************************************************************
 * Id: cfg.c                                                            *
 *                                                                      *
 * TR069 Project:  A TR069 library in C                                 *
 * Copyright (C) 2013-2014  netcwmp.netcwmp group                                *
 *                                                                      *
 *                                                                      *
 * Email: netcwmp ( & ) gmail dot com                                *
 *                                                                      *
 ***********************************************************************/

#include <cwmp/cwmp.h>
#include <cwmp/pool.h>
#include <cwmp/log.h>
#include <cwmp/cfg.h>
#include <ini.h>

#ifndef MIN
# define MIN(x, y) (((x) > (y)) ? (y) : (x))
#endif

typedef struct conf_t conf_t;

struct conf_t {
    char * filename;
    FILE * fd;
    bool write_to_nvram;
    bool read_from_nvram;
};


static conf_t	* cwmp_conf_handle = NULL;

int cwmp_conf_open(const char * filename)
{
    char buf[INI_BUFFERSIZE] = {};

    cwmp_log_trace("%s(filename=\"%s\")", __func__, filename);
    cwmp_conf_handle = malloc(sizeof(conf_t));
    if (!cwmp_conf_handle) {
        cwmp_log_error("conf malloc faild.");
        return CWMP_ERROR;
    }
    cwmp_conf_handle->filename = TRstrdup(filename);
    /* basic values */
    ini_gets("cwmp", "cfg_write_to_nvram", NULL, buf, sizeof(buf), filename);
    if (*buf == '1') {
        cwmp_conf_handle->write_to_nvram = true;
        cwmp_conf_handle->read_from_nvram = true;
    }
    ini_gets("cwmp", "cfg_read_from_nvram", NULL, buf, sizeof(buf), filename);
    if (*buf == '1') {
        cwmp_conf_handle->read_from_nvram = true;
    }

    cwmp_log_info("CONF: write_to_nvram=%s, read_from_nvram=%s",
            cwmp_conf_handle->write_to_nvram ? "true" : "false",
            cwmp_conf_handle->read_from_nvram ? "true" : "false");

    return CWMP_OK;
}

void cwmp_conf_split(char * name, char **s , char **k)
{
    *s = strchr(name, ':');
    if(*s == NULL) {
        k = &name;
        *s = "cwmp";
    } else {
        *s[0] = 0;
        *k = *s+1;
        *s = name;
    }
}

int cwmp_conf_get(const char * key, char *value)
{
    char *s, *k;
    char name[INI_BUFFERSIZE] = {};

    char nvram_name[sizeof(name) + 3] = {};
    const char *nvram_val = NULL;
    //char value[INI_BUFFERSIZE] = {};

    cwmp_log_trace("%s(\"%s\", %p) ->", __func__, key, (void*)value);

    if(key == NULL) {
        return CWMP_ERROR;
    }

    if (cwmp_conf_handle == NULL) {
        cwmp_log_error("%s: config file handle is not initialized!", __func__);
        return CWMP_ERROR;
    }

    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    /* 'evn' only from config file */
    if (!TRstrcmp(s, "env") || !cwmp_conf_handle->read_from_nvram) {
        ini_gets(s, k, NULL, value, INI_BUFFERSIZE, cwmp_conf_handle->filename);
        cwmp_log_debug("%s(\"%s\", %p) = \"%s\": readed from %s",
                __func__, key, (void*)value, value,
                cwmp_conf_handle->filename);
        return CWMP_OK;
    }
    /* get nvram value */
    TRsnprintf(nvram_name, sizeof(nvram_name), "%s_%s", s, k);
    nvram_val = cwmp_nvram_get(nvram_name);
    if (!*nvram_val) {
        char _val[512] = {};
        ini_gets(s, k, NULL, _val, sizeof(_val), cwmp_conf_handle->filename);
        if (*_val) {
            cwmp_log_debug("%s(\"%s\") = \"%s\": write to nvram",
                    __func__, key, _val);
            cwmp_nvram_set(nvram_name, _val);
        } else {
            cwmp_log_debug("%s(\"%s\", %p) = empty value",
                    __func__, key, (void*)value);
        }
        TRstrncpy(value, _val, INI_BUFFERSIZE);
    } else {
        size_t _nv_sz = strlen(nvram_val);
        TRstrncpy(value, nvram_val, MIN(INI_BUFFERSIZE, _nv_sz));
        cwmp_log_debug("%s(\"%s\", %p) = \"%s\": readed from nvram",
                __func__, key, (void*)value, value);
    }

    return CWMP_OK;
}

int cwmp_conf_set(const char * key, const char * value)
{
    char * s, *k;
    char name[INI_BUFFERSIZE] = {};
    char nvram_name[sizeof(name) + 3] = {};

    cwmp_log_trace("%s(\"%s\", \"%s\")", __func__, key, value);

    if(key == NULL) {
        return CWMP_ERROR;
    }
    if (cwmp_conf_handle == NULL) {
        cwmp_log_error("%s: config file handle is not initialized!", __func__);
        return CWMP_ERROR;
    }

    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    if (!TRstrcmp(s, "env") || !cwmp_conf_handle->write_to_nvram) {
        cwmp_log_debug("%s(\"%s\", \"%s\"): write to %s",
                __func__, key, value, cwmp_conf_handle->filename);
        return ini_puts(s, k, value, cwmp_conf_handle->filename);
    } else {
        snprintf(nvram_name, sizeof(nvram_name), "%s_%s", s, k);
        cwmp_log_debug("%s(\"%s\", \"%s\"): write to nvram",
                __func__, key, value);
        return cwmp_nvram_set(nvram_name, value);
    }
}

char * cwmp_conf_pool_get(pool_t * pool, const char * key)
{
    char value[INI_BUFFERSIZE] = {0};

    cwmp_log_trace("%s(pool=%p, \"%s\")", __func__, (void*)pool, key);

    cwmp_conf_get(key, value);

    return pool_pstrdup(pool, value);
}

int cwmp_conf_get_int_def(const char * key, int def)
{
    char val[INI_BUFFERSIZE] = {};

    cwmp_log_trace("%s(\"%s\")", __func__, key);

    cwmp_conf_get(key, val);
    return strToIntDef(val, def);
}


int cwmp_nvram_set(const char * key, const char * value)
{
    cwmp_log_debug("%s(\"%s\", \"%s\")", __func__, key, value);
    //FIXME: libnvram check const!
    return nvram_set(RT2860_NVRAM, (char*) key, (char*) value);
}

char *cwmp_nvram_get(const char * key)
{
    char *nvval = NULL;
    //FIXME: libnvram check const!
    nvval = nvram_get(RT2860_NVRAM, (char*) key);
    cwmp_log_debug("%s(\"%s\") = \"%s\"", __func__, key, nvval);
    return nvval;
}

char * cwmp_nvram_pool_get(pool_t * pool, const char * key)
{
    //FIXME: libnvram check const!
    char* val = nvram_get(RT2860_NVRAM, (char*)key);
    cwmp_log_debug("%s(\"%s\") = \"%s\"", __func__, key, val);
    return pool_pstrdup(pool,val);
}



int cwmp_nvram_get_int(const char * key, int def)
{
    char *val;
    val = cwmp_nvram_get(key);

    if (strlen(val) == 0) {
        return def;
    }

    return strtol(val, NULL, 10);

}

int cwmp_nvram_get_bool_onoff(const char * key, int def)
{
    char *val;
    val = cwmp_nvram_get(key);

    if (strlen(val) == 0) {
        return def;
    }

    if (strcmp(val, "on") == 0) {
        return 1;
    } else if (strcmp(val, "off") == 0) {
        return 0;
    }

    return def;

}

