/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "cli_fw.h"

/* firewall functions */
int func_fw(int argc, char* argv[])
{
    char* cmd = NULL;

    if (argc>0)
    {
	cmd = argv[0];
    }

    if (STR_EQ(cmd, "filter"))
    {
	/* STUB */
    }
    else
    if (STR_EQ(cmd, "forward"))
    {
	/* STUB */
    }

    return 0;

}
