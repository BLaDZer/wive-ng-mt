#ifndef _CLI_H_
#define _CLI_H_

/* for GNU basename() */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include "libwive.h"

#define STR_EQ(X,Y)	( X && Y && (strcmp(X,Y)==0) )

#include "cli_wl.h"
#include "cli_sw.h"
#include "cli_fw.h"


void writeHeader(char* text);
void writeCmdHelp(char* cmd, char* description);
int main(int argc, char* argv[]);

#endif