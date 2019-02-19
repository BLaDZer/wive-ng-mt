/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]wi-cat.ru
*/

#include "cli.h"

int func_cli(char* cmd, int argc, char* argv[])
{
    if (argc > 0 && STR_EQ(cmd, "cli"))
    {
        cmd = argv[0];
        argc--;
        argv++;
	return func_cli(cmd, argc, argv);
    }
    else if (STR_EQ(cmd, "wl") || STR_EQ(cmd, "wlan"))
	return func_wl(argc, argv);
    else if (STR_EQ(cmd, "sw") || STR_EQ(cmd, "switch")) 
	return func_sw(argc, argv);
    else if (STR_EQ(cmd, "st") || STR_EQ(cmd, "status")) 
	return func_st(argc, argv);
    else
    {
	writeCmdHelp("wl", "wireless settings");
	writeCmdHelp("sw", "switch settings");
	writeCmdHelp("st", "device information");
        printf("\n");
    }

    return 0;
}


int main(int argc, char* argv[])
{
    char* cmd = basename(argv[0]);
    argc--;
    argv++;

    openlog ("cli", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);

    func_cli(cmd, argc, argv);

    return 0;
}
