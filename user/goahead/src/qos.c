#include    <stdlib.h>
#include    <sys/ioctl.h>
#include    <string.h>
#include    <dirent.h>

#include    "utils.h"
#include    "internet.h"
#include    "firewall.h"
#include    "helpers.h"


static void QoSSetup(webs_t wp, char_t *path, char_t *query);
void formDefineQoS(void) {
	websFormDefine(T("QoSSetup"), QoSSetup);
}
const parameter_fetch_t QoS_args[] =
	{
	{ T("UpBW"), "QoS_rate_up", 0, T("") },
	{ T("UpBWL"), "QoS_rate_limit_up", 0, T("") },
	{ T("DownBW"), "QoS_rate_down", 0, T("") },
	{ T("DownBWL"), "QoS_rate_limit_down", 0, T("") },
	{ T("UpVPNBW"), "QoS_rate_vpn_up", 0, T("") },
	{ T("UpVPNBWL"), "QoS_rate_vpn_limit_up", 0, T("") },
	{ T("QoS_HPP"), "QoS_high_pp", 0, T("") },
	{ T("QoS_LPP"), "QoS_low_pp", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void QoSSetup(webs_t wp, char_t *path, char_t *query)
{
	char *submitUrl;

	char_t *QoS_type = websGetVar(wp, T("QoSSelect"), T("0"));
	if (QoS_type == NULL)
		QoS_type = "0";

	char_t *simpleqos = websGetVar(wp, T("simple_qos"), T("0"));
	if (strcmp(simpleqos, "on") != 0)
		simpleqos = "off";
	simpleqos = (strcmp(simpleqos, "on") == 0) ? "1" : "0";

	nvram_init(RT2860_NVRAM);
	nvram_bufset(RT2860_NVRAM, "QoSEnable", QoS_type);
	nvram_bufset(RT2860_NVRAM, "simple_qos", simpleqos);

	if (CHK_IF_DIGIT(QoS_type, 2))
		setupParameters(wp, QoS_args, 0);

	nvram_close(RT2860_NVRAM);
	doSystem("service shaper restart && service iptables restart && service kext restart");

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	if (submitUrl != NULL)
		websRedirect(wp, submitUrl);
	else
		websDone(wp, 200);
}
