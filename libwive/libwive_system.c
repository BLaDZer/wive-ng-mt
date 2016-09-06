/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

/* reboot_now -  Reboot the system
 *
 */
void reboot_now(void)
{
	/* always flush stdout buffer before reboot */
	fflush(stdout);
	system("(sleep 2 && /etc/scripts/reboot.sh) > /dev/null 2>&1 &");
}

const char* getSystemPlatform()
{
#if defined(CONFIG_RALINK_MT7620) && defined(CONFIG_RAETH_ESW)
#if defined(CONFIG_RT_SECOND_IF_MT7610E)
    return "MT7620 2T2R 2.4GHz, MT7610 1T1R 5GHz, 100FDX";
#elif defined(CONFIG_RT_SECOND_IF_MT7612E)
    return "MT7620 2T2R 2.4GHz, MT7612 2T2R 5GHz, 100FDX";
#else
    return "MT7620 2T2R 2.4GHz, 100FDX";
#endif
#elif defined(CONFIG_RALINK_MT7621) && defined(CONFIG_MT7530_GSW)
#ifdef CONFIG_MT76X3_AP
    return "MT7621 1000FDX MT7603 MT7612 2T2R dualband";
#else
    return "MT7621 1000FDX MT7602 MT7612 2T2R dualband";
#endif
#else
    return "Unknown system platform";
#endif
    return 0;
}

long getSystemUptime()
{
    struct sysinfo sinf;

    if (sysinfo(&sinf) == 0)
        return sinf.uptime;

    return 0;

}
