#include "asp_parser.h"

int asp_parser_init(ngx_pool_t *pool)
{
    asp_func_array = ngx_array_create(pool, 16, sizeof(asp_func_t));
    asp_form_array = ngx_array_create(pool, 16, sizeof(asp_form_t));

    asp_nvram_acl_array = ngx_array_create(pool, 16, sizeof(asp_nvram_acl_t));
    asp_nvram_acl_init(pool);

    asp_mod_utils_init();
    asp_mod_wireless_init();
    asp_mod_internet_init();
    asp_mod_services_init();
    asp_mod_firewall_init();
    asp_mod_management_init();
    asp_mod_station_init();
    asp_mod_storage_init();
    asp_mod_qos_init();
    asp_mod_usb_init();

//    asp_mod_test_init();

    return NGX_OK;
}

int asp_nvram_acl_init(ngx_pool_t * pool)
{
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE *fp = fopen("/etc/nginx/nvram_acl.conf", "r");
    if (fp == NULL)
    {
        return NGX_ERROR;
    }


    while ((read = getline(&line, &len, fp)) != -1) {
        int flags = 0;
//        printf("Retrieved line of length %zu :\n", read);
//        printf("%s", line);
        char* val = cstring_split_inplace(line, "\t");
        if (val != NULL)
        {
            while (*val != '\0')
            {
                char c = val[0];

                     if (c == 'r') flags |= USER_R;
                else if (c == 'w') flags |= USER_W;
                else if (c == 'x') flags |= USER_X;
                else if (c == 'R') flags |= ADMIN_R;
                else if (c == 'W') flags |= ADMIN_W;
                else if (c == 'X') flags |= ADMIN_X;

                val++;
            }

            if (strlen(line)>0)
            {
                char* name = cstring_copy(pool, line);
                if (name != NULL)
                {
                    asp_nvram_acl_t* acl = (asp_nvram_acl_t*)ngx_array_push(asp_nvram_acl_array);
                    acl->name = name;
                    acl->flags = flags;
                }
            }
        }
    }

    fclose(fp);

    free(line);
    return NGX_OK;
}

int asp_state_machine(webs_t *wp, ngx_chain_t *in) {
    u_char *p;
    ngx_chain_t *cl;
    enum PARSER_STATE state = TEXT;
    ELOG_DEBUG(wp->log, 0, "State machine START\n");

    ngx_array_t* inner_buf = NULL;

    for (cl = in; cl; cl = cl->next) {

        char* w;

        for (p = cl->buf->pos; p < cl->buf->last; p++) {

            char c = *p;
//            ELOG_DEBUG(wp->log, 0, "State machine outer: state %i, char %c\n", state, c);

            switch (state)
            {
                case TEXT:
                    if (c == '<') state = LBRACKET;
                    else 
                    {
                        w = (char*)ngx_array_push(wp->out);
                        *w = c;
                        state = TEXT;
                    }
                    break;

                case LBRACKET:
                    if (c == '%')
                    {
                        if (inner_buf != NULL)
                        {
                            ngx_array_destroy(inner_buf);
                        }
                        inner_buf = ngx_array_create(wp->pool, 128, sizeof(char));
                        state = LPERCENT;
                    }
                    else 
                    {
                        w = (char*)ngx_array_push_n(wp->out, 2);
                        *w = '<';
                        w++;
                        *w = c;
                        state = TEXT;
                    }
                    break;

                case LPERCENT:
                    {
                        if (c == '%')
                        {
                            state = RPERCENT;
                        }
                        else
                        {
                            w = (char*)ngx_array_push(inner_buf);
                            *w = c;
                            state = LPERCENT;
                        }
                    }
                    break;

                case RPERCENT:
                    if (c == '>')
                    {
                        w = (char*)ngx_array_push(inner_buf);
                        *w = '\0'; // zero-cap end

                        char* ptr2 = cstring_trim(wp->pool,(char*)inner_buf->elts, ' ');
                        if (ptr2 == NULL)
                        {
                            state = TEXT;
                            break;
                        }

                        ngx_array_t* inner_outarr = ngx_array_create(wp->pool, 128, sizeof(char));
                        parseInner(wp, ptr2, inner_outarr);

                        int inner_len = inner_outarr->nelts;

                        if (inner_len > 0) // copy to buffer
                        {
                            w = (char*)ngx_array_push_n(wp->out, inner_len);
                            memcpy(w, inner_outarr->elts, inner_len);
                        }

                        state = TEXT;
                    }
                    else 
                    {
                        w = (char*)ngx_array_push(inner_buf);
                        *w = c;

                        state = LPERCENT;
                    }
                    break;

            }
        }

    }

    if (inner_buf != NULL)
    {
        ngx_array_destroy(inner_buf);
    }

    return state;
}

int parseInner(webs_t *wp, char* cmds, ngx_array_t *out)
{
    int state = 0;

    char* ptr;
    char* w;

    ngx_array_t* param_buf = ngx_array_create(wp->pool, 32, sizeof(char));

    ngx_array_t *params = ngx_array_create(wp->pool, 10, sizeof(char*));
    for(ptr=cmds;ptr[0];ptr++)
    {
        char c = *ptr;
//        ELOG_DEBUG(wp->log, 0, "State machine inner: state %i, char %c\n", state, c);

        if (state == 0)
        {
            if (c == '(')
            {
                char *cmd_param = ngx_array_to_cstring(wp->pool, param_buf);
                char **param_ptr = ngx_array_push(params);
                if (param_ptr == NULL)
                {
                    ELOG_ERR(wp->log, 0, "ERROR !!! Unable to push array!");
                }
                else
                {
                    *param_ptr = cmd_param;
                }

                ngx_array_destroy(param_buf);
                param_buf = ngx_array_create(wp->pool, 32, sizeof(char));
                state = 1;
            }
            else
            {
                w = ngx_array_push(param_buf);
                if (w == NULL)
                {
                    ELOG_ERR(wp->log, 0, "ERROR !!! Unable to push array!");
                }
                else
                {
                    *w = c;
                }
                state = 0;
            }
        }
        else
        if (state == 1)
        {
            if (c == ')')
            {
                char **param_ptr = ngx_array_push(params);

                char* cparam_buf = ngx_array_to_cstring(wp->pool, param_buf);
                if (cparam_buf == NULL)
                {
                    ELOG_ERR(wp->log, 0, "cparam_buf is NULL! \n");
                }

                char *arg_param = cstring_trim(wp->pool, cparam_buf, ' ');
                if (arg_param == NULL)
                {
                    ELOG_ERR(wp->log, 0, "arg_param is NULL! \n");
                }

                char * arg_param2 = cstring_trim(wp->pool, arg_param, '"');
                if (arg_param2 == NULL)
                {
                    ELOG_ERR(wp->log, 0, "arg_param2 is NULL! \n");
                }

                if (param_ptr == NULL)
                {
                    ELOG_ERR(wp->log, 0, "ERROR !!! Unable to push array!");
                }
                else
                {
                    *param_ptr = arg_param2;
                }

                ngx_array_destroy(param_buf);
                param_buf = ngx_array_create(wp->pool, 32, sizeof(char));
                state = 2;
            }
            else
            if (c == ',')
            {

                char *arg_param = cstring_trim(wp->pool, ngx_array_to_cstring(wp->pool, param_buf), ' ');
                char **param_ptr = ngx_array_push(params);

                if (param_ptr == NULL)
                {
                    ELOG_ERR(wp->log, 0, "ERROR !!! Unable to push array!");
                }
                else
                {
                    *param_ptr = cstring_trim(wp->pool, arg_param, '"');
                }

                ngx_array_destroy(param_buf);
                param_buf = ngx_array_create(wp->pool, 32, sizeof(char));
                state = 1;
            }
            else
            {
                w = ngx_array_push(param_buf);
                if (w == NULL)
                {
                    ELOG_ERR(wp->log, 0, "ERROR !!! Unable to push array!");
                }
                else
                {
                    *w = c;
                }
                state = 1;
            }
        }
        else
        if (state == 2)
        {
            if (c == ';')
            {
                parseCmd(wp, out, params);
                params = ngx_array_create(wp->pool, 10, sizeof(char*));
                state = 0;
            }
        }

//        ptr++;
    }


    return 0;
}

int parseCmd(webs_t *wp, ngx_array_t *out, ngx_array_t *params_arr)
{
//    TRACE_FUNCTION(wp->log);

    if (!params_arr || params_arr->nelts < 1)
    {
        return NGX_ERROR;
    }

    char **params = ((char**)(params_arr->elts));
    int nparams = params_arr->nelts-1;

    char* cmd = params[0];
    params++; // skip cmd val

    ELOG_DEBUG(wp->log, 0, "parseCmd: cmd = %s\n", cmd);

    asp_func_t* asp_func;

    int func_found = 0;

    for (asp_func=(asp_func_t*)asp_func_array->elts;asp_func<((asp_func_t*)asp_func_array->elts+asp_func_array->nelts);asp_func++)
    {
        if (strcmp(cmd, asp_func->name) == 0) 
        {
            (*(asp_func->ptr))(wp, params, nparams);
            func_found = 1;
            break;
        }
    }

    if (func_found == 0)
    {
        ELOG_CRIT(wp->log, 0, "Unable to parse command: %s !\n", cmd);
    }

    return NGX_OK;
}

int asp_parser_parse_form(webs_t* wp, char_t* name, char_t* path, char_t* query)
{
    ELOG_DEBUG(wp->log, 0, "parseForm: name = %s\n", name);

    asp_form_t* asp_form;

    // iterate asp form array
    for (asp_form=(asp_form_t*)asp_form_array->elts;asp_form<((asp_form_t*)asp_form_array->elts+asp_form_array->nelts);asp_form++)
    {
        if (strcmp(name, asp_form->name) == 0) 
        {
            (*(asp_form->ptr))(wp, path, query);
        }
    }

    return NGX_OK;

}

