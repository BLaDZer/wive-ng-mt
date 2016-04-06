#ifndef __CWMP_DATA_MODEL_H__
#define __CWMP_DATA_MODEL_H__

#include <cwmp/cwmp.h>

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
