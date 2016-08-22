/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "cli.h"

void writeHeader(char* text) 
{
    printf(" * %s \n", text);
    printf("---------------------------------------------------------------\n");
}

void writeCmdHelp(char* cmd, char* description)
{
    printf(" %-20s -- %s\n",cmd, description);
}


int main(int argc, char* argv[])
{
    char* cmd = basename(argv[0]);
    argc--;
    argv++;

    if (argc > 1 && STR_EQ(cmd, "cli"))
	return main(argc, argv);
    else if (STR_EQ(cmd, "wl")) 
	return func_wl(argc, argv);
    else if (STR_EQ(cmd, "sw")) 
	return func_sw(argc, argv);
    else if (STR_EQ(cmd, "fw")) 
	return func_fw(argc, argv);
    else
    {
	writeCmdHelp("wl", "wireless settings");
	writeCmdHelp("sw", "switch settings");
	writeCmdHelp("fw", "firewall settings");
    }

    return 0;
}
