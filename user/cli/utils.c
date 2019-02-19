/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]wi-cat.ru
*/
#include "utils.h"

enum STAGGER_PROTO_VERSION getStaggerProtoVersion()
{
    return APRIL17;
}

int reload_all_settings()
{
    char* p = "/etc/scripts/internet.sh";
    int pid = 0;

    if (*p) {
        if ((pid = fork()) == 0) {

            int fd = open("/dev/null", O_WRONLY);
            dup2(fd,1);

            sleep(3);
            if (execl(p, p, (char*)NULL) == -1) {
                printf("exec(%s) error: %s \n", p, strerror(errno));
                close(fd);
                return 1;
            }

            close(fd);
            return 0;

        } else if(pid > 0) {
            return 0;
        } else {
            printf("fork() failed: %s \n", strerror(errno));
            return 3;
        }
    }

    return 0;
}


void writeHeader(char* text) 
{
    printf("     \n");
    printf(" - %s \n", text);
    printf("---------------------------------------------------------------\n");
}

int is_report(int argc, char* argv[])
{
    if (argc > 0 && STR_EQ(argv[0],"report"))
        return 1;

    return 0;
}

void writeCmdHelp(char* cmd, char* description)
{
    printf(" %-20s -- %s\n",cmd, description);
}
