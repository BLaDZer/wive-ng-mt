/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]wi-cat.ru
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../../linux/include/generated/compile.h"	/* for build configs and other */
#include "../../linux/include/generated/utsrelease.h"   /* for build configs and other */
#include "../mkimage/include/image.h"		/* For Uboot image header format */


#define STR_VALUE(val) #val
#define STR(name) STR_VALUE(name)

#define PATH_LEN 1024
#define MD5_LEN 32


int exec_and_get(char* cmd, char* output, int maxlen, int* read_len) {
    FILE *p = popen(cmd, "r");
    if (p == NULL) return -1;

    int i, ch;
    for (i = 0; (i < maxlen) && ((ch = fgetc(p)) != EOF); i++) {
        if (output) *output++ = ch;
    }

    if (output) *output = '\0';

    if (read_len) {
        *read_len = i;
    }

    return WEXITSTATUS(pclose(p));
}

int CalcFileMD5(char *file_name, char *md5_sum)
{
    int i, read_len;
    #define MD5SUM_CMD_FMT "md5sum %." STR(PATH_LEN) "s 2>/dev/null"
    char cmd[PATH_LEN + sizeof (MD5SUM_CMD_FMT)];
    sprintf(cmd, MD5SUM_CMD_FMT, file_name);
    #undef MD5SUM_CMD_FMT

    if (exec_and_get(cmd, md5_sum, MD5_LEN, &read_len)) {
        return 1;
    }

    if (read_len != MD5_LEN) {
        return 2;
    }

    for (i = 0; i < MD5_LEN; i++) {
        if (!isxdigit(md5_sum[i])) return 3;
    }

    return 0;
}


const char linux_banner[] =
	"Linux version " UTS_RELEASE " (" LINUX_COMPILE_BY "@"
	LINUX_COMPILE_HOST ") (" LINUX_COMPILER ") " UTS_VERSION "\n";

const char name[] = LINUX_COMPILE_BY;
const char model[] = LINUX_COMPILE_HOST;

int main(int argc, char** argv) {

    if (argc<2) {
	printf("usage: versiongen image_path\n");
	return 1;
    }

    char inbuf[1024] = {0};
    int read_len = 0;

    snprintf(inbuf, sizeof(inbuf), "echo -n \"%s\" | md5sum", linux_banner);
    char ver_md5[33] = {0};

    if (exec_and_get(inbuf, ver_md5, sizeof(ver_md5)-1, &read_len) || read_len != 32) {
	printf("Unable to get version md5 (%i)\n", read_len);
    }

    char file_md5[33] = {0};
    CalcFileMD5(argv[1], file_md5);

    printf("NAME: %s\n", name);
    printf("VERSION: %s", linux_banner);
    printf("MODEL: %s\n", model);

    return 0;
}
