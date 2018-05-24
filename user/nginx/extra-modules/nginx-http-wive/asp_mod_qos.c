#include "asp_mod_qos.h"

parameter_fetch_t QoS_args[] =
	{
	{ T("QoSMode"),		"QoSMode",			0, T("0") },
	{ T("UpBW"),		"QoS_rate_up",			0, T("")  },
	{ T("UpBWL"),		"QoS_rate_limit_up",		0, T("")  },
	{ T("DownBW"),		"QoS_rate_down",		0, T("")  },
	{ T("DownBWL"),		"QoS_rate_limit_down",		0, T("")  },
	{ T("UpVPNBW"),		"QoS_rate_vpn_up",		0, T("")  },
	{ T("UpVPNBWL"),	"QoS_rate_vpn_limit_up",	0, T("")  },
	{ T("UpMDMBW"),		"QoS_rate_mdm_up",		0, T("")  },
	{ T("UpMDMBWL"),	"QoS_rate_mdm_limit_up",	0, T("")  },
	{ T("QoS_HPP"),		"QoS_high_pp",			0, T("")  },
	{ T("QoS_LPP"),		"QoS_low_pp",			0, T("")  },
	{ T("QoS_HDSCP"),	"QoS_high_dscp",		0, T("")  },
	{ T("QoS_LDSCP"),	"QoS_low_dscp",			0, T("")  },
	{ NULL,			NULL,				0, NULL   }  // Terminator
};

static void QoSSetup(webs_t* wp, char_t *path, char_t *query)
{
	char_t *reset = websGetVar(wp, "reset", "0");

	if (CHK_IF_DIGIT(reset, 1)) {
		nvram_fromdef(RT2860_NVRAM, 12, "QoSEnable", "QoSMode", "QoS_rate_up", "QoS_rate_limit_up",
						"QoS_rate_down", "QoS_rate_limit_down", "QoS_rate_vpn_up", "QoS_rate_vpn_limit_up",
						"QoS_high_pp", "QoS_low_pp", "QoS_high_dscp", "QoS_low_dscp");
	}
	else {
		char_t *QoS_type = websGetVar(wp, "QoSSelect", "0");
		if (QoS_type == NULL)
			QoS_type = "0";

		nvram_init(RT2860_NVRAM);
		ngx_nvram_bufset(wp, "QoSEnable", QoS_type);

		if (CHK_IF_DIGIT(QoS_type, 2))
			setupParameters(wp, QoS_args, 0);

		nvram_commit(RT2860_NVRAM);
		nvram_close(RT2860_NVRAM);
	}

	doSystem("service shaper restart && service iptables restart && service kext restart");

//	websHeader(wp);
	websDone(wp, 200);
}

void asp_mod_qos_init(void) {
	websFormDefine("QoSSetup", QoSSetup, ADMIN);
}
