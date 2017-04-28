#ifndef _CLI_UTILS_H_
#define _CLI_UTILS_H_

#include "cli.h"

#define STR_EQ(X,Y)	( X && Y && (strcmp(X,Y)==0) )
#define STR_EQ_HELP(X)  ( STR_EQ(X,"help") || STR_EQ(X,"?") )

enum STAGGER_PROTO_VERSION 
{
    UNKNOWN = 0,
    APRIL17 = 1
};

enum STAGGER_PROTO_VERSION getStaggerProtoVersion();
void writeHeader(char* text);
void writeCmdHelp(char* cmd, char* description);
int is_report(int argc, char* argv[]);
int reload_all_settings();


#endif//_CLI_UTILS_H_