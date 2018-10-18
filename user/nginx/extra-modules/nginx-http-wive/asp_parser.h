#ifndef __ASP_PARSER_H__
#define __ASP_PARSER_H__

#define _GNU_SOURCE

#include "util.h"

#include "asp_mod_utils.h"
#include "asp_mod_wireless.h"
#include "asp_mod_internet.h"
#include "asp_mod_services.h"
#include "asp_mod_firewall.h"
#include "asp_mod_management.h"
#include "asp_mod_station.h"
#include "asp_mod_storage.h"
#include "asp_mod_qos.h"
#include "asp_mod_usb.h"

//#include "asp_mod_test.h"

enum PARSER_STATE {
    TEXT = 0,
    LBRACKET = 1,
    LPERCENT = 2,
    RPERCENT = 3
};

int asp_parser_init(ngx_pool_t *pool);
int asp_nvram_acl_init(ngx_pool_t * pool);

int asp_parser_parse_form(webs_t* wp, char_t* name, char_t* path, char_t* query);


int asp_state_machine(webs_t *wp, ngx_chain_t *in);
int parseInner(webs_t *wp, char* cmds);
int parseCmd(webs_t *wp, ngx_array_t *params_arr);



#endif // __ASP_PARSER_H__
