#include "cwmp/model.h"
#include "cwmp/xmlet.h"
#include "cwmp/log.h"
#include "cwmp_private.h"

int cwmp_model_copy_parameter_object(parameter_node_t * object_param, parameter_node_t * new_node, int instance_number)
{
	char tmp[42] = {};
    parameter_node_t * tmp_node, *curr_node, *next_newnode;
    if(!object_param || !new_node)
    {
        return CWMP_ERROR;
    }
    if(instance_number > 0)
    {
        memcpy(new_node, object_param, sizeof(parameter_node_t));
		/* TODO: load alias */
		TRsnprintf(tmp, sizeof(tmp), "cpe-%d", instance_number);
        new_node->name = TRstrdup(TRitoa(instance_number));
		new_node->alias = TRstrdup(tmp);

        new_node->child = NULL;
        new_node->next_sibling = NULL;
        for(tmp_node=object_param; tmp_node; tmp_node = tmp_node->next_sibling)
        {
            if(!tmp_node->next_sibling)
            {
                tmp_node->next_sibling = new_node;
                break;
            }

        }
    }


    for(tmp_node=object_param->child, curr_node=NULL; tmp_node; tmp_node=tmp_node->next_sibling)
    {
        next_newnode = MALLOC(sizeof(parameter_node_t));
        memcpy(next_newnode, tmp_node, sizeof(parameter_node_t));
        next_newnode->name = TRstrdup(tmp_node->name);
		next_newnode->alias = TRstrdup(tmp_node->alias);
        next_newnode->parent = new_node;
        next_newnode->child = NULL;
        next_newnode->next_sibling = NULL;

        if(!curr_node)
        {
            new_node->child = next_newnode;
        }
        else
        {
            curr_node->next_sibling = next_newnode;
        }

        curr_node = next_newnode;

        cwmp_model_copy_parameter_object(tmp_node, next_newnode, 0);


    }

    return CWMP_OK;
}

int cwmp_model_copy_parameter(parameter_node_t * param, parameter_node_t ** new_param, int instance_number){

    int rv;

	cwmp_log_trace("%s(param=%p [name=%s], new_param=%p, instance_number=%d",
			__func__, (void*)param, (param ? param->name : NULL),
			(void*)new_param, instance_number);

    if(!param)
        return CWMP_OK;


    parameter_node_t * object_param = param->child; //object param like InternetGatewayDevice.WANDevice.{i}

    parameter_node_t * new_node = MALLOC(sizeof(parameter_node_t));

    if (!new_node)
    {
        cwmp_log_error("cwmp model copy parameter malloc new node failed ...");
        return CWMP_ERROR;
    }

    new_node->name = NULL;
	new_node->alias = NULL;
    new_node->rw = 0;
    new_node->inform = 0;
    new_node->inform_sort = 0;
    new_node->type = 0;
    new_node->value = NULL;
    new_node->parent = param;

    rv = cwmp_model_copy_parameter_object(object_param, new_node, instance_number);
    *new_param = new_node;
    if(rv != CWMP_OK)
    {
        cwmp_log_error("cwmp_model: copy parameter failed");
        cwmp_model_free_parameter(new_node);
        *new_param = NULL;
    }

    return rv;
}

int cwmp_model_free_parameter(parameter_node_t * param)
{
    parameter_node_t     *tmp_param = NULL;
    parameter_node_t     *next_param = NULL;

    if(!param)
    {
        return CWMP_OK;
    }

    for(tmp_param=param->child; tmp_param; tmp_param=next_param)
    {
        next_param = tmp_param->next_sibling;
        cwmp_model_delete_parameter(tmp_param);
    }

	free(param->alias);
    free(param->name);
    free(param);

    return CWMP_OK;
}

int cwmp_model_delete_parameter(parameter_node_t * param)
{
    parameter_node_t     *tmp_param = NULL;
    parameter_node_t     *parent = NULL;

	cwmp_log_trace("%s(param=%p [name=%s])",
			__func__, (void*)param, (param ? param->name : NULL));

    if(!param)
    {
        return CWMP_OK;
    }
    parent = param->parent;
    for(tmp_param=parent->child; tmp_param; tmp_param=tmp_param->next_sibling)
    {
        if(tmp_param->next_sibling == param)
        {
            tmp_param->next_sibling = param->next_sibling;
            break;
        }
    }

    return cwmp_model_free_parameter(param);
}

void* cwmp_model_find_func(model_func_t * func_list, int func_count, const char * func_name)
{
    if(!func_name || func_count <= 0)
        return NULL;

    model_func_t * f = func_list;
    int i;
    for(i=0; i<func_count; i++, f++)
    {
        if(TRstrcasecmp(func_name, f->name) == 0)
        {
            return f->func;
        }
    }
    return NULL;


}

int cwmp_model_create_parameter(parameter_node_t * param, xmlnode_t * node, model_func_t * func_list, int func_count,  pool_t * pool);

int cwmp_model_init_parameter(parameter_node_t * param, xmlnode_t * node, model_func_t * func_list, int func_count, pool_t * pool)
{
    //	FUNCTION_TRACE();
    param->get = NULL;
    param->set = NULL;
    param->notify = NULL;
    param->add = NULL;
    param->del = NULL;
    param->refresh = NULL;
    param->reload = NULL;
    param->args = NULL;
	char tmp[42] = {};

    if(!node)
    {
        return CWMP_OK;
    }

    char *nodename = node->nodeName;
    char *name, *value;
    if(! TRstrcasecmp(nodename, DEVICE_MODEL_OBJECT_NODE))
    {
        param->type = TYPE_OBJECT;
    }
    else
    {
        char * type = cwmp_xml_get_node_attribute(node, "type");
        if(!type)
        {
            param->type = TYPE_STRING;
        }
        else
        {
            param->type = cwmp_get_type_value(type);
        }
    }
    name = cwmp_xml_get_node_attribute(node, "name");
    if(!name)
    {
        return CWMP_ERROR;
    }

    param->name = TRstrdup(name);
	/* TODO: load alias */
	snprintf(tmp, sizeof(tmp), "cpe-%s", param->name);
	param->alias = TRstrdup(tmp);

    value = cwmp_xml_get_node_attribute(node, "rw");
    if(value)
    {
        param->rw = TRatoi(value);
    }

    value = cwmp_xml_get_node_attribute(node, "inform");
    if(value)
    {
        param->inform = TRatoi(value);
    }

    value = cwmp_xml_get_node_attribute(node, "inform_sort");
    if(value)
    {
        param->inform_sort = TRatoi(value);
    }

	value = cwmp_xml_get_node_attribute(node, "reload_func");
	if (value)
	{
		param->reload = (parameter_reload_handler_pt) cwmp_model_find_func(func_list, func_count, value);
	}

    if(param->type == TYPE_OBJECT)
    {
        value = cwmp_xml_get_node_attribute(node, "add_func");
        if(value)
        {
            param->add = (parameter_add_handler_pt)cwmp_model_find_func(func_list, func_count, value);//dlsym(cwmp->dev_lib, value);;


        }

        value = cwmp_xml_get_node_attribute(node, "del_func");
        if(value)
        {
            param->del = (parameter_del_handler_pt)cwmp_model_find_func(func_list, func_count, value);//dlsym(cwmp->dev_lib, value);;
        }

        value = cwmp_xml_get_node_attribute(node, "refresh_func");
        if(value)
        {
            param->refresh = (parameter_refresh_handler_pt)cwmp_model_find_func(func_list, func_count, value);//dlsym(cwmp->dev_lib, value);;
        }
    }
    else
    {
        value = cwmp_xml_get_node_attribute(node, "get_func");
        if(value)
        {
            param->get = (parameter_get_handler_pt) cwmp_model_find_func(func_list, func_count, value);//dlsym(cwmp->dev_lib, value);;
        }

        value = cwmp_xml_get_node_attribute(node, "set_func");
        if(value)
        {
            param->set = (parameter_set_handler_pt)cwmp_model_find_func(func_list, func_count, value);//dlsym(cwmp->dev_lib, value);;
        }

        value = cwmp_xml_get_node_attribute(node, "notify_func");
        if(value)
        {
            param->notify = (parameter_notify_handler_pt)cwmp_model_find_func(func_list, func_count, value);//dlsym(cwmp->dev_lib, value);;
        }

        value = cwmp_xml_get_node_attribute(node, "args");
        if(value)
        {
	    //FIXME
            param->args = pool_pstrdup(pool,value);
        }
    }

    return CWMP_OK;

}

int cwmp_model_create_child_parameter(parameter_node_t * child_param, xmlnode_t * child_node, model_func_t * func_list, int func_count, pool_t * pool)
{
    cwmp_log_trace("%s(child_param=%p [name=%s], child_node=%p, func_list=%p, func_count=%d, pool=%p)",
			__func__, (void*)child_param, (child_param ? child_param->name : NULL),
			(void*)child_node, (void*)func_list, func_count, (void*)pool);

	cwmp_model_create_parameter(child_param, child_node, func_list, func_count, pool);

    xmlnode_t * next_node =  XmlNodeGetNextSibling(child_node);
    parameter_node_t * last_sibling = child_param;
    while(next_node)
    {
        parameter_node_t * next_param = (parameter_node_t *)pool_pcalloc(pool, sizeof(parameter_node_t));
        cwmp_model_create_parameter(next_param, next_node, func_list, func_count, pool);

        next_node = XmlNodeGetNextSibling(next_node);

        next_param->parent = child_param->parent;
        last_sibling->next_sibling = next_param;
        last_sibling = next_param;
    }

    return CWMP_OK;
}

int cwmp_model_create_parameter(parameter_node_t * param, xmlnode_t * node, model_func_t * func_list, int func_count,  pool_t * pool)
{
    cwmp_model_init_parameter(param, node, func_list, func_count, pool);

    xmlnode_t * child_node = XmlNodeGetFirstChild(node);
    if(!child_node) {
		cwmp_log_debug("param name %s: %p, %p",
				param->name, param->get, param->set);
        return CWMP_OK;
    }

    parameter_node_t * child_param = (parameter_node_t *)pool_pcalloc(pool, sizeof(parameter_node_t));
    child_param->parent = param;
	param->child = child_param;

    cwmp_model_create_child_parameter(child_param, child_node, func_list, func_count,  pool);

	cwmp_log_debug("param name %s (child: %s): %p, %p",
			param->name, child_param->name, param->get, param->set);


    return CWMP_OK;

}

static int cwmp_model_init_object(cwmp_t * cwmp, parameter_node_t *param)
{
    parameter_node_t     *node = NULL;

	cwmp_log_trace("%s(cwmp=%p, param=%p [name=%s])",
			__func__, (void*)cwmp, (void*)param,
			(param ? param->name : NULL));

    if(!param)
    {
        return CWMP_ERROR;
    }

    if(param->type == TYPE_OBJECT && param->refresh)
    {
        //µ÷ÓÃrefreshº¯Êý
        if(param->refresh)
        {
            param->refresh(cwmp, param, callback_register_task);
        }
    }

    for (node = param->child; node; node = node->next_sibling)
    {
        cwmp_model_init_object(cwmp, node);
    }

    return CWMP_OK;
}

int cwmp_model_refresh_object(cwmp_t * cwmp, parameter_node_t *param, int flag, callback_register_func_t callback_reg)
{
    parameter_node_t     *node = NULL;

	cwmp_log_trace("%s(cwmp=%p, param=%p [name=%s], flag=%d, callback_reg=%p)",
			__func__, (void*)cwmp,
			(void*)param, (param ? param->name : NULL),
			flag, (void*)callback_reg);

    if(!param)
    {
        return CWMP_ERROR;
    }

    if(param->refresh && flag == 1)
    {
        if(param->refresh)
        {
            param->refresh(cwmp, param, callback_reg);
        }
    }

    for (node = param->child; node; node = node->next_sibling)
    {
        if(TRstrcmp(param->name, "{i}") != 0)
        {
            cwmp_model_refresh_object(cwmp, node, 1, callback_reg);
        }
    }

    return CWMP_OK;
}

int cwmp_model_delete_object_child(cwmp_t *cwmp, parameter_node_t *param)
{

	parameter_node_t *pn = NULL;
	cwmp_log_trace("%s(cwmp=%p, param=%p [name=%s])",
			__func__, (void*)cwmp, (void*)param, param ? param->name : "");

	if (!param)
		return CWMP_ERROR;

	param = param->child;
	while (param) {
		pn = param;
		param = param->next_sibling;

		if (!strcmp(pn->name, "{i}"))
			continue;

		cwmp_model_delete_parameter(pn);
		pn = NULL;
	}
	return CWMP_OK;
}

int cwmp_model_load_parameter(cwmp_t * cwmp, xmldoc_t * doc, model_func_t * func_list, int func_count)
{
    pool_t * pool = cwmp->pool;
    xmlnode_t *  root_node;
    xmlnode_t *  model_node;

	cwmp_log_trace("%s(cwmp=%p, doc=%p, func_list=%p, func_count=%d)",
			__func__, (void*)cwmp, (void*)doc, (void*)func_list, func_count);

    ASSERT(doc != NULL);

    root_node = XmlNodeGetFirstChild(& doc->node);
    if (! root_node)
    {
        cwmp_log_error("xml document root is null!");
        return CWMP_ERROR;
    }

    cwmp_log_debug("model load: xml node name is %s", root_node->nodeName);
    model_node = cwmp_xml_get_child_with_name(root_node, DEVICE_MODEL_NODE);
    if (model_node == NULL)
    {
        cwmp_log_error("device model node not found!");
        return CWMP_ERROR;
    }

    parameter_node_t * root_param = pool_pcalloc(pool, sizeof(parameter_node_t));

    cwmp_model_create_parameter(root_param, model_node, func_list, func_count, pool);

    cwmp->root = root_param->child;

    cwmp_model_init_object(cwmp, cwmp->root);
    cwmp_log_debug("DEBUG: cwmp_model_load_parameter OK");
    return CWMP_OK;
}

int cwmp_model_load_xml(cwmp_t * cwmp, const char * xmlfile, model_func_t * func_list, int func_count)
{
    xmldoc_t *  doc;
    size_t xmllen, nread ;
    FILE * fp = fopen(xmlfile, "rb");

	cwmp_log_trace("%s(cwmp=%p, xmlfile=\"%s\", func_list=%p, func_count=%d)",
			__func__, (void*)cwmp, xmlfile, (void*)func_list, func_count);

    if(!fp)
    {
        cwmp_log_error("xmlfile is NULL");
        return CWMP_ERROR;
    }

    fseek(fp, 0, SEEK_END);
    xmllen = ftell(fp);


    char * buf = (char*)MALLOC(sizeof(char)*(xmllen+1));
    if(!buf)
    {
        cwmp_log_error("model load: malloc fail");
        goto finish;
    }
    fseek(fp, 0, SEEK_SET);
    nread = fread(buf, 1, xmllen, fp);
    buf[nread] = 0;
    pool_t * pool = pool_create(POOL_DEFAULT_SIZE);
    doc = XmlParseBuffer(pool, buf);
    if (!doc)
    {
        cwmp_log_error("model load: create doc faild");
        goto finish;
    }

    cwmp_model_load_parameter(cwmp, doc, func_list, func_count);

finish:
    free(buf);
    fclose(fp);
    pool_destroy(pool);
    return CWMP_OK;
}
