#ifndef __CWMP_DATA_MODEL_H__
#define __CWMP_DATA_MODEL_H__

#include <cwmp/cwmp.h>

#define DM_TRACE_REFRESH() \
    cwmp_log_trace("%s(cwmp=%p, param_node=%p [name=%s], callback_reg=%p)",\
            __func__, (void*)cwmp, (void*)param_node,\
            (param_node ? param_node->name : ""),\
            (void*)callback_reg);

#define DM_TRACE_SET() \
    cwmp_log_trace("%s(cwmp=%p, name=\"%s\", value=\"%s\", length=%d, args=\"%s\", callback_reg=%p)",\
            __func__, (void*)cwmp, name, value, length, args, (void*)callback_reg);

#define DM_TRACE_GET() \
    cwmp_log_trace("%s(cwmp=%p, name=\"%s\", value=%p, args=\"%s\", pool=%p)",\
            __func__, (void*)cwmp, name, (void*)value, args, (void*)pool);

#define DM_TRACE_RELOAD() \
    cwmp_log_trace("%s(cwmp=%p, callback_reg=%p)",\
            __func__, (void*)cwmp, (void*)callback_reg);

#define DM_TRACE_DEL() \
    cwmp_log_trace("%s(cwmp=%p, param_node=%p [name=%s], instance_number=%d, callback_reg=%p)",\
            __func__, (void*)cwmp, (void*)param_node,\
            (param_node ? param_node->name : ""),\
            instance_number, (void*)callback_reg);

#define DM_TRACE_ADD() \
    cwmp_log_trace("%s(cwmp=%p, param_node=%p [name=%s], pinstance_number=%p, callback_reg=%p)",\
            __func__, (void*)cwmp, (void*)param_node,\
            (param_node ? param_node->name : ""),\
            (void*)pinstance_number, (void*)callback_reg);


int get_index_after_paramname(parameter_node_t * param, const char * tag_name);
void cwmp_model_load(cwmp_t * cwmp, const char * xmlfile);

int cpe_get_conf_string(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);
int cpe_set_conf_string(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg);

int cpe_get_const_string(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);

int cpe_get_nvram_string(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);
int cpe_set_nvram_string(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg);

int cpe_get_nvram_string_or_empty(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);

int cpe_get_nvram_bool(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);
int cpe_set_nvram_bool(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg);

int cpe_get_nvram_bool_onoff(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);
int cpe_set_nvram_bool_onoff(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg);

int cpe_get_nvram_int(cwmp_t * cwmp, const char * name, char ** value, char * args, pool_t * pool);
int cpe_set_nvram_int(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg);

int cpe_set_null(cwmp_t * cwmp, const char * name, const char * value, int length, char * args, callback_register_func_t callback_reg);
int cpe_add_null(cwmp_t * cwmp, parameter_node_t * param_node, int *pinstance_number, callback_register_func_t callback_reg);


#endif
