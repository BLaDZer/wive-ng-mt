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


typedef struct conf_t conf_t;

struct conf_t {
        char * filename;
        FILE * fd;
};


static conf_t	* cwmp_conf_handle = NULL;

int cwmp_conf_open(const char * filename)
{
    FUNCTION_TRACE();
    cwmp_conf_handle = malloc(sizeof(cwmp_conf_handle));
    if (!cwmp_conf_handle)
    {
        cwmp_log_error("conf malloc faild.\n");
        return CWMP_ERROR;
    }
    cwmp_conf_handle->filename = TRstrdup(filename);
    return CWMP_OK;
}

void cwmp_conf_split(char * name, char **s , char **k)
{
    *s = strchr(name, ':');
    if(*s == NULL)
    {
        k = &name;
        *s = "cwmp";
    }
    else
    {
        *s[0] = 0;
        *k = *s+1;
        *s = name;
    }
}

int cwmp_conf_get(const char * key, char *value)
{
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};
    //char value[INI_BUFFERSIZE] = {0};
    FUNCTION_TRACE();
    if(key == NULL)
    {
        return CWMP_ERROR;
    }
    if (cwmp_conf_handle == NULL)
    {
	cwmp_log_error("cwmp_conf_get: config file handle is not initialized!");
	return CWMP_ERROR;
    }

    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    ini_gets(s,k,NULL,value,INI_BUFFERSIZE, cwmp_conf_handle->filename);
    return CWMP_OK;
}

int cwmp_conf_set(const char * key, const char * value)
{
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};
    FUNCTION_TRACE();
    if(key == NULL)
    {
        return CWMP_ERROR;
    }
    if (cwmp_conf_handle == NULL)
    {
	cwmp_log_error("cwmp_conf_get: config file handle is not initialized!");
	return CWMP_ERROR;
    }

    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    return ini_puts(s, k, value, cwmp_conf_handle->filename);
}

char * cwmp_conf_pool_get(pool_t * pool, const char * key)
{
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};
    char value[INI_BUFFERSIZE] = {0};
    //FUNCTION_TRACE();
    if(key == NULL)
    {
        return NULL;
    }
    TRstrncpy(name, key, INI_BUFFERSIZE);

    cwmp_conf_split(name, &s, &k);

    ini_gets(s,k,NULL,value,INI_BUFFERSIZE, cwmp_conf_handle->filename);

    return pool_pstrdup(pool, value);
}

int cwmp_conf_get_int(const char * key)
{
    char * s, *k;
    char name[INI_BUFFERSIZE] = {0};

    FUNCTION_TRACE();
    if(key == NULL)
    {
        return 0;
    }
    TRstrncpy(name, key, INI_BUFFERSIZE);
    cwmp_conf_split(name, &s, &k);

    return (int)ini_getl(s,k,0,cwmp_conf_handle->filename);
}


int cwmp_nvram_set(const char * key, const char * value)
{
//    char keybuf[1024];
//    sprintf(keybuf,"nvram:%s",key);
//    return cwmp_conf_set(keybuf, value);
    cwmp_log_error("DEBUG2: cwmp_nvram_set: %s=%s \n", key, value);
    return nvram_set(RT2860_NVRAM, key, value);
}



int cwmp_nvram_get(const char * key, char *value) 
{
    char* nvval;
    //char keybuf[1024];
    //sprintf(keybuf,"nvram:%s",key);

//    return cwmp_conf_get(keybuf, value);
    nvval = nvram_get(RT2860_NVRAM, key);
    cwmp_log_error("DEBUG2: cwmp_nvram_get: %s=%s (%i) \n", key, nvval, nvval);

    strcpy(value, nvval);
    return CWMP_OK;//strlen(nvval);
}

char * cwmp_nvram_pool_get(pool_t * pool, const char * key) 
{
//    char keybuf[1024];
//    sprintf(keybuf,"nvram:%s",key);
//    return cwmp_conf_pool_get(pool, keybuf);
    char* val = nvram_get(RT2860_NVRAM, key);
    cwmp_log_error("DEBUG2: cwmp_nvram_pool_get: %s=%s (%i) \n",key,val, val);

    return pool_pstrdup(pool,val);
}



int cwmp_nvram_get_int(const char * key, int def)
{
    char valbuf[256];
    cwmp_nvram_get(key,&valbuf);

    if (strlen(valbuf) == 0) {
	return def;
    }

    return strtol(&valbuf, NULL, 10);
    
}

int cwmp_nvram_get_bool_onoff(const char * key, int def)
{
    char valbuf[256];
    cwmp_nvram_get(key,&valbuf);

    if (strlen(valbuf) == 0) {
	return def;
    }

    if (strcmp(valbuf,"on") == 0) return 1;
    else if (strcmp(valbuf,"off") == 0) return 0;

    return def;
    
}

