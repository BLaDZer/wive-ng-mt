#ifndef _CLI_SW_H_
#define _CLI_SW_H_

#include "cli.h"

int func_sw(int argc, char* argv[]);
int func_sw_ipv6(int argc, char* argv[]);
int func_sw_lan(int argc, char* argv[]);
int func_sw_wan(int argc, char* argv[]);
int func_sw_port(int argc, char* argv[]);
int func_sw_interface(int argc, char* argv[]);

int func_sw_lan_set(int argc, char* argv[]);
int func_sw_lan_apply(int argc, char* argv[]);

int func_sw_wan_set(int argc, char* argv[]);
int func_sw_wan_apply(int argc, char* argv[]);

int func_sw_ipv6_set(int argc, char* argv[]);
int func_sw_ipv6_apply(int argc, char* argv[]);


#endif