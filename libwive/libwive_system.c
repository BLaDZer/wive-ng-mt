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
	sync();
	fflush(stdout);
	system("(sleep 2 && /etc/scripts/reboot.sh) > /dev/null 2>&1 &");

	/* wait 15sec ad if not rebooted from script call kernel for reboot directly */
	sleep(15);
	fflush(stdout);
	syscall(SYS_reboot,LINUX_REBOOT_MAGIC1,LINUX_REBOOT_MAGIC2,LINUX_REBOOT_CMD_RESTART,NULL);
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
#elif CONFIG_MT7615_AP
#ifdef CONFIG_MT7615_AP_DBDC_MODE
    return "MT7621 1000FDX MT7615DN 2T2R dualband";
#else
    return "MT7621 1000FDX MT7615N 4T4R dualband";
#endif
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

/* getShadowPassword - get shadow password structure of user
 *
 * arg: username
 * return: passwd* or NULL
 */
struct passwd* getShadowPassword(char* username)
{
    struct passwd* pwd;
    struct spwd* spwd;

    pwd = getpwnam(username); // static memory block, please do not free() it
    spwd = getspnam(username);

    if (pwd == NULL) return NULL; // empty passwd line, abort

    if (spwd != NULL)
    {
        if (strlen(spwd->sp_pwdp) > 0) // shadow isn't empty
        {
            pwd->pw_passwd = spwd->sp_pwdp; // replace passwd data with shadow
        }
    }

    return pwd;
}

/* cryptShadowPassword - encrypt any password in shadow-compatible salted format
 *
 * arg: username - from whom the salt will be obtained
 * arg: password
 * return: encrypted password string or NULL
 */
char* cryptShadowPassword(char* username, char* password)
{
    struct passwd* pwd = getShadowPassword(username);
    if (pwd == NULL) return NULL; // empty passwd line, abort

    return crypt(password, pwd->pw_passwd); // static memory block, please do not free() it
}
