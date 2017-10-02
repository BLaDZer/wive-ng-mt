#ifndef _CLI_WL_H_
#define _CLI_WL_H_

#include "cli.h"

int func_wl(int argc, char* argv[]);

int func_wl_stalist(int argc, char* argv[]);
int func_wl_stalist_iface(char* iface, int argc, char* argv[]);
int func_wl_stalist_report(int argc, char* argv[]);

int func_wl_disconnect(int argc, char* argv[]);
int func_wl_disconnect_iface(char* iface, char* mac);

int func_wl_set(int argc, char* argv[]);

int func_wl_status(int argc, char* argv[]);
int func_wl_status_report(int argc, char* argv[]);

int func_wl_scan(int argc, char* argv[]);
int func_wl_scan_report(char* iface, int argc, char* argv[]);

int func_wl_wds(int argc, char* argv[]);
int func_wl_wds_report(int argc, char* argv[]);

#endif