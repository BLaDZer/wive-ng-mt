#ifndef _CLI_H_
#define _CLI_H_

/* for GNU basename() */
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "libwive.h"

#include "utils.h"

#include "cli_wl.h"
#include "cli_sw.h"
#include "cli_st.h"

int main(int argc, char* argv[]);

#endif