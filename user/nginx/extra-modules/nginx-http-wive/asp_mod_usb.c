/* vi: set sw=4 ts=4 sts=4: */
/*
 *	usb.c -- USB Application Settings
 *
 *	Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *	$Id: usb.c,v 1.7.2.1 2009-04-08 11:27:39 chhung Exp $
 */

#include	"asp_mod_usb.h"

#ifdef CONFIG_USER_P910ND
static void printersrv(webs_t* wp, char_t *path, char_t *query);
#endif
#ifdef CONFIG_USB_MODESWITCH
static void usbmodem(webs_t* wp, char_t *path, char_t *query);
static int modemShowStatus(webs_t *wp, char** params, int nparams);
#endif

#ifdef CONFIG_USER_P910ND
static void printersrv(webs_t* wp, char_t *path, char_t *query)
{
	char_t *enable;
	char_t *bidirect;
	char_t *submitUrl;

	// fetch from web input
	enable = websGetVar(wp, T("enabled"), T(""));
	bidirect = websGetVar(wp, T("bdenabled"), T(""));
	// set to nvram
	nvram_init(RT2860_NVRAM);
	ngx_nvram_bufset(wp, "PrinterSrvEnabled", enable);
	ngx_nvram_bufset(wp, "PrinterSrvBidir", bidirect);
	nvram_commit(RT2860_NVRAM);
	nvram_close(RT2860_NVRAM);

#ifdef PRINT_DEBUG
	// debug print
//	websHeader(wp);
	websWrite(wp, T("<h2>Printer Server Settings</h2><br>\n"));
	websWrite(wp, T("enabled: %s<br>\n"), enable);
//	websFooter(wp);
	websDone(wp, 200);
#else
	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
#endif
}
#endif

#ifdef CONFIG_USB_MODESWITCH

parameter_fetch_t usb_modem_args[] =
{
	{ T("modem_type"), "MODEMTYPE", 0, T("0") },
	{ T("modem_port"), "WMODEMPORT", 0, T("AUTO") },
	{ T("modem_speed"), "MODEMSPEED", 0, T("AUTO") },
	{ T("modem_mtu"), "MODEMMTU", 0, T("AUTO") },
	{ T("modem_user"), "MODEMUSERNAME", 0, T("") },
	{ T("modem_pass"), "MODEMPASSWORD", 0, T("") },
	{ T("modem_dialn"), "MODEMDIALNUMBER", 0, T("") },
	{ T("modem_apn"), "MODEMAPN", 0, T("") },
	{ T("at_enabled"), "MODEMATENABLED", 2, T("off") },
	{ T("mdebug_enabled"), "MODEMDEBUG", 2, T("off") },
	{ T("modem_at1"), "MODEMAT1", 0, T("") },
	{ T("modem_at2"), "MODEMAT2", 0, T("") },
	{ T("modem_at3"), "MODEMAT3", 0, T("") },
	{ NULL, NULL, 0, NULL } // Terminator
};

static void usbmodemrestart(void)
{
	doSystem("service iptables restart");
#if (CONFIG_RALINK_GPIO_PWR_USB > 0)
	doSystem("service modemhelper stop");
	doSystem("service modemhelper poweroffon");
#else
	doSystem("service modemhelper restart");
#endif
}

static void usbmodem(webs_t* wp, char_t *path, char_t *query)
{
	char_t *submitUrl;
	char_t *submit;

	submit = websGetVar(wp, T("hiddenButton"), T(""));

	if (0 == strcmp(submit, "apply"))
		{
			char_t *modem_enabled = websGetVar(wp, T("modem_enabled"), T("0"));
			if (modem_enabled == NULL)
				modem_enabled = "0";

			nvram_init(RT2860_NVRAM);
			ngx_nvram_bufset(wp, "MODEMENABLED", modem_enabled);

			if (CHK_IF_DIGIT(modem_enabled, 1))
				setupParameters(wp, usb_modem_args, 0);

			nvram_commit(RT2860_NVRAM);
			nvram_close(RT2860_NVRAM);
			usbmodemrestart();
		}
	else if (0 == strcmp(submit, "connect"))
		{
			usbmodemrestart();
		}
	else if (0 == strcmp(submit, "disconnect"))
		{
			doSystem("service modemhelper stop");
		}

	submitUrl = websGetVar(wp, T("submit-url"), T(""));   // hidden page
	websRedirect(wp, submitUrl);
}

/*** USB modem statuses ***/
typedef struct modem_status_t
{
	const char_t *status;
	long          color;
} modem_status_t;

/*** Modem statuses ***/
const modem_status_t modem_statuses[] =
{

	{ "disabled",     0x808080        },
	{ "offline",      0xff0000        },
//	{ "connecting",   0xff8000        },
	{ "online",       0x00ff00        }
};
/*** Show Modem Connect status ***/
static int modemShowStatus(webs_t *wp, char** params, int nparams)
{
	int status = 0; // Status is 'disabled'
	const modem_status_t *st_table = modem_statuses;

	// Get value
	int modem_enabled = nvram_get_int(RT2860_NVRAM, "MODEMENABLED", 0);

	// Do not perform other checks if modem is turned off
	if (modem_enabled == 1)
	{

			// Status is at least 'offline' now
			status++;

			// Try to search for 'modem' of 'wwan0" devices
			FILE * fd = fopen(_PATH_PROCNET_DEV, "r");

			if (fd != NULL)
			{
				char_t line[256];

				// Read all ifaces and check match
				while ((fgets(line, sizeof(line), fd)) != NULL)
				{
					if(strstr(line,"modem") || strstr(line,"wwan0") || strstr(line,"weth0"))
					{
						status++; // Status is set to 'connected'
						break; // Do not search more
					}
				}
				fclose(fd);
			}
			else
			{
				syslog(LOG_WARNING, "cannot open %s (%s).", _PATH_PROCNET_DEV, strerror(errno));
			}
	}

	// Output connection status
	const modem_status_t *st = &st_table[status];
	websWrite(
		wp,
		T("<b>Status: <font color=\"#%06x\">%s</font></b>\n"),
		st->color, st->status
	);

	return 0;
}
#endif

int initUSB(void)
{
	return 0;
}

void asp_mod_usb_init(void) {
#ifdef CONFIG_USER_P910ND
	websFormDefine("printersrv", printersrv, EVERYONE);
#endif
#ifdef CONFIG_USB_MODESWITCH
	websFormDefine("usbmodem", usbmodem, EVERYONE);
	aspDefineFunc("modemShowStatus", modemShowStatus, EVERYONE);
#endif
}
