<!DOCTYPE html>
<html>
	<head>
		<title>Hotspot</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script src="hotspot_profiles.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("internet");
			Butterlate.setTextDomain("hint");
			Butterlate.setTextDomain("buttons");
			Butterlate.setTextDomain("services");

			var SPOT_IP		= '<% getSpotIp(); %>';
			var SPOT_NETMASK	= '<% getSpotNetmask(); %>';

			function initTranslation() {
				_TR("sTitle",					"hotspot title");
				_TR("sIntroduction",			"hotspot introduction");
				_TR("spotSetup",				"hotspot setup");
				_TR("spotType",					"hotspot type");
				_TR("spotProfile",				"hotspot profile");
				_TR("sIp",						"inet ip");
				_TR("sNetmask",					"inet netmask");
				_TR("sStartIP",					"lan dhcp start");
				_TR("sEndIP",					"lan dhcp end");
				_TR("sPriDns",					"inet pri dns");
				_TR("sSecDns",					"inet sec dns");
				_TR("sHttpsRedirect",			"hotspot https redirect");
				_TR("sDropDNS",					"hotspot drop dns");
				_TR("sC2Cisolate",				"hotspot clisolate");
				_TR("sMaxClients",				"hotspot max clients");
				_TR("sDomain",					"hotspot domain");
				_TR("sLease",					"lan dhcp lease");
				_TR("sRadServer1",				"hotspot radius auth");
				_TR("sRadServer2",				"hotspot radius acct");
				_TR("sRadSecret",				"hotspot radius secret");
				_TR("sNasId",					"hotspot radius nas");
				_TR("sRadLocId",				"hotspot radius location id");
				_TR("sRadLocName",				"hotspot radius location name");
				_TR("sRadCoaPort",				"hotspot radius coa port");
				_TR("sRadCoaNoIpCheck",			"hotspot radius no check ip");
				_TR("sUamServer",				"hotspot uam server");
				_TR("sUamHomepage",				"hotspot uam homepage");
				_TR("sUamSecret",				"hotspot uam secret");
				_TR("sUamAllowed",				"hotspot uam allowed");
				_TR("sUamDomain",				"hotspot uam domain");
				_TR("sUamAnyDNS",				"hotspot uam any dns");
				_TR("sMacAllowed",				"hotspot uam mac");
				_TR("GatewayIPRange",			"hotspot nodog ip range");
				_TR("RedirectURL",				"hotspot nodog url");
				_TR("MaxClients",				"hotspot nodog max clients");
				_TR("ClientIdleTimeout",		"hotspot nodog idle timeout");
				_TR("ClientForceTimeout",		"hotspot nodog force timeout");
				_TR("AuthenticateImmediately",	"hotspot nodog auth");
				_TR("MACMechanism",				"hotspot nodog mechanism");
				_TR("BlockedMACList",			"hotspot nodog blocked list");
				_TR("AllowedMACList",			"hotspot nodog allowed list");
				_TR("TrustedMACList",			"hotspot nodog trusted list");
				_TR("PasswordAuthentication",	"hotspot nodog pass");
				_TR("block",					"hotspot nodog block");
				_TR("allow",					"hotspot nodog allow");
				_TR("Password",					"vpn password");
				_TR("UsernameAuthentication",	"hotspot nodog user");
				_TR("Username",					"vpn username");
				_TR("PasswordAttempts",			"hotspot nodog pass attempts");
				_TRV("sApply",					"button apply");
				_TRV("sCancel",					"button cancel");
				_TRV("sReset",					"button reset");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					if (elements[i].id == "disable")
						elements[i].innerHTML = _("button disable");
					else if (elements[i].id == "enable")
						elements[i].innerHTML = _("button enable");
			}

			function initValues() {
				var form	= document.spotCfg;
				var enabled = form.spotEnable;

				form.AuthenticateImmediately.value	= NVRAM_nodog_AuthenticateImmediately;
				form.MACMechanism.value 			= NVRAM_nodog_MACMechanism;
				form.PasswordAuthentication.value	= NVRAM_nodog_PasswordAuthentication;
				form.UsernameAuthentication.value	= NVRAM_nodog_UsernameAuthentication;
				form.GatewayIPRange.value			= NVRAM_nodog_GatewayIPRange;
				form.RedirectURL.value				= NVRAM_nodog_RedirectURL;
				form.MaxClients.value				= NVRAM_nodog_MaxClients;
				form.ClientIdleTimeout.value		= NVRAM_nodog_ClientIdleTimeout;
				form.ClientForceTimeout.value		= NVRAM_nodog_ClientForceTimeout;
				form.BlockedMACList.value			= NVRAM_nodog_BlockedMACList;
				form.AllowedMACList.value			= NVRAM_nodog_AllowedMACList;
				form.TrustedMACList.value			= NVRAM_nodog_TrustedMACList;
				form.Password.value					= NVRAM_nodog_Password;
				form.Username.value					= NVRAM_nodog_Username;
				form.PasswordAttempts.value			= NVRAM_nodog_PasswordAttempts;
				form.chilliEnable.value				= NVRAM_chilli_enable;
				form.nodogEnable.value				= NVRAM_nodogsplash_enable;
				
				document.getElementById('sC2Cisolate').title	= _("hotspot clisolate full");
				document.getElementById('sDropDNS').title		= _("hotspot drop dns full");

				enabled.options.length = 0;
				addOption(enabled, _("button disable"), "0");
				if(BUILD_CHILLI == '1')
					addOption(enabled, "ChilliSpot", "1");
				if(BUILD_NODOG == '1')
					addOption(enabled, "NoDogSplash", "2");

				if(BUILD_CHILLI == '1' && (form.chilliEnable.value == 'on'))
					enabled.value = "1";
				else if(BUILD_NODOG == '1' && (form.nodogEnable.value == '1'))
					enabled.value = "2";
				else
					enabled.value = "0";

				var prof = form.spotProfile;
				prof.options.length = 0;
				for(var i = 0; i < Profiles.length; i++) {
					if (Profiles[i][1] == "hotspot manual")
						Profiles[i][1] = _("hotspot manual");
					addOption(prof, Profiles[i][1], Profiles[i][0]);
					if(Profiles[i][0] == NVRAM_chilli_profile)
						prof.value = NVRAM_chilli_profile;
				}
				if(NVRAM_chilli_profile == '0')
					prof.options.selectedIndex == 0;

				ModeOnChange(form);
				showWarning();
				initTranslation();
				displayServiceStatus();
			}

			function CheckValue(form) {
				if(form.spotEnable.value == "1")
					for(var i = 0; i < Profiles.length; i++) {
						var tmp = Profiles[i];
						if (tmp[0] == form.spotProfile.value) {
							// Check Network
							if (!tmp[2] && !validateIP(form.sIp, true)) {
								alert(_("ipv6 invalid ipv4"));
								form.sIp.focus();
								form.sIp.select();
								return false;
								}
							// Check Netmask
							if (!tmp[3] && !validateIP(form.sNetmask, true)) {
								alert(_("ipv6 invalid ipv4"));
								form.sNetmask.focus();
								form.sNetmask.select();
								return false;
							}
							// Check start IP
							if (!tmp[4] && !checkDigitRange(form.sStartIP.value, 0, 254)) {
								alert(_("hotspot expects number"));
								form.sStartIP.focus();
								form.sStartIP.select();
								return false;
							}
							// Check end IP
							if (!tmp[5] && !checkDigitRange(form.sEndIP.value, 0, 254)) {
								alert(_("hotspot expects number"));
								form.sEndIP.focus();
								form.sEndIP.select();
								return false;
							}
							if (!tmp[5] && form.sEndIP.value < form.sStartIP.value) {
								alert(_("hotspot endip low"));
								form.sEndIP.focus();
								form.sEndIP.select();
								return false;
							}
							// Check Primary DNS
							if (!tmp[6] && !validateIP(form.sPriDns, true)) {
								alert(_("ipv6 invalid ipv4"));
								form.sPriDns.focus();
								form.sPriDns.select();
								return false;
							}
							// Check Secondery DNS
							if (!tmp[7] && !validateIP(form.sSecDns, true)) {
								alert(_("ipv6 invalid ipv4"));
								form.sSecDns.focus();
								form.sSecDns.select();
								return false;
							}
							// Check Max Clients
							if (!tmp[27] && !checkDigitRange(form.sMaxClients.value, 50, 2000)) {
								alert(_("hotspot max clients range"));
								form.sMaxClients.focus();
								form.sMaxClients.select();
								return false;
							}
							// Check Domain
							if (!tmp[8] && form.sDomain.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sDomain.focus();
								form.sDomain.select();
								return false;
							}
							// Check sLease
							if (!tmp[9] && !/^\d+$/.test(form.sLease.value)) {
								alert(_("hotspot expects number"));
								form.sLease.focus();
								form.sLease.select();
								return false;
							}
							// Check sRadServer1
							if (!tmp[10] && form.sRadServer1.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sRadServer1.focus();
								form.sRadServer1.select();
								return false;
							}
							// Check sRadServer2
							if (!tmp[11] && form.sRadServer2.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sRadServer2.focus();
								form.sRadServer2.select();
								return false;
							}
							// Check sRadSecret
							if (!tmp[12] && form.sRadSecret.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sRadSecret.focus();
								form.sRadSecret.select();
								return false;
							}
							// Check sNasId
							if (!tmp[13] && form.sNasId.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sNasId.focus();
								form.sNasId.select();
								return false;
							}
							// Check sRadLocId
							if (!tmp[14] && form.sRadLocId.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sRadLocId.focus();
								form.sRadLocId.select();
								return false;
							}
							// Check sRadLocName
							if (!tmp[15] && form.sRadLocName.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sRadLocName.focus();
								form.sRadLocName.select();
								return false;
							}
							// Check sRadCoaPort
							if (tmp[16] && !/^\d+$/.test(form.sRadCoaPort.value)) {
								alert(_("hotspot expects number"));
								form.sRadCoaPort.focus();
								form.sRadCoaPort.select();
								return false;
							}
							// Check sUamServer
							if (!tmp[18] && form.sUamServer.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sUamServer.focus();
								form.sUamServer.select();
								return false;
							}
							// Check sUamHomepage
							if (!tmp[19] && form.sUamHomepage.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sUamHomepage.focus();
								form.sUamHomepage.select();
								return false;
							}
							// Check sUamSecret
							if (!tmp[20] && form.sUamSecret.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sUamSecret.focus();
								form.sUamSecret.select();
								return false;
							}
							// Check sUamAllowed
							if (!tmp[21] && form.sUamAllowed.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sUamAllowed.focus();
								form.sUamAllowed.select();
								return false;
							}
							// Check sUamDomain
							if (!tmp[22] && form.sUamDomain.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sUamDomain.focus();
								form.sUamDomain.select();
								return false;
							}
							// Check sMacAllowed
							if (!tmp[24] && form.sMacAllowed.value.indexOf(" ") >= 0) {
								alert(_("hotspot dont space"));
								form.sMacAllowed.focus();
								form.sMacAllowed.select();
								return false;
							} else if (form.sMacAllowed.value.length > 0 && !validateMACList(form.sMacAllowed.value.split(","), false)) {
								alert(_("hotspot mac list"));
								form.sMacAllowed.focus();
								form.sMacAllowed.select();
								return false;
							}
						}
					}
					else if(form.spotEnable.value == "2")
						// Check sMacAllowed
						if (form.RedirectURL.value.indexOf(" ") >= 0) {
							alert(_("hotspot dont space"));
							form.RedirectURL.focus();
							form.RedirectURL.select();
							return false;
						} else if (!/^\d+$/.test(form.MaxClients.value)) {
							alert(_("hotspot expects number"));
							form.MaxClients.focus();
							form.MaxClients.select();
							return false;
						} else if (!/^\d+$/.test(form.ClientIdleTimeout.value)) {
							alert(_("hotspot expects number"));
							form.ClientIdleTimeout.focus();
							form.ClientIdleTimeout.select();
							return false;
						} else if (!/^\d+$/.test(form.ClientForceTimeout.value)) {
							alert(_("hotspot expects number"));
							form.ClientForceTimeout.focus();
							form.ClientForceTimeout.select();
							return false;
						} else if (form.MACMechanism.value == "block" && form.BlockedMACList.value.indexOf(" ") >= 0) {
							alert(_("hotspot dont space"));
							form.BlockedMACList.focus();
							form.BlockedMACList.select();
							return false;
						} else if (form.BlockedMACList.value.length > 0 && !validateMACList(form.BlockedMACList.value.split(","), false)) {
							alert(_("hotspot mac list"));
							form.BlockedMACList.focus();
							form.BlockedMACList.select();
							return false;
						} else if (form.MACMechanism.value == "allow" && form.AllowedMACList.value.indexOf(" ") >= 0) {
							alert(_("hotspot dont space"));
							form.AllowedMACList.focus();
							form.AllowedMACList.select();
							return false;
						} else if (form.AllowedMACList.value.length > 0 && !validateMACList(form.AllowedMACList.value.split(","), false)) {
							alert(_("hotspot mac list"));
							form.AllowedMACList.focus();
							form.AllowedMACList.select();
							return false;
						} else if (form.TrustedMACList.value.indexOf(" ") >= 0) {
							alert(_("hotspot dont space"));
							form.TrustedMACList.focus();
							form.TrustedMACList.select();
							return false;
						} else if (form.TrustedMACList.value.length > 0 && !validateMACList(form.TrustedMACList.value.split(","), false)) {
							alert(_("hotspot mac list"));
							form.TrustedMACList.focus();
							form.TrustedMACList.select();
							return false;
						} else if (form.Password.value.indexOf(" ") >= 0) {
							alert(_("hotspot dont space"));
							form.Password.focus();
							form.Password.select();
							return false;
						} else if (form.Username.value.indexOf(" ") >= 0) {
							alert(_("hotspot dont space"));
							form.Username.focus();
							form.Username.select();
							return false;
						} else if (!/^\d+$/.test(form.PasswordAttempts.value)) {
							alert(_("hotspot expects number"));
							form.PasswordAttempts.focus();
							form.PasswordAttempts.select();
							return false;
						}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function ProfileOnChange(form) {
				if (form.spotProfile.value != "manual" && form.spotProfile.value != NVRAM_chilli_profile && NVRAM_chilli_profile != 'manual' && NVRAM_chilli_profile != '0') {
					if (confirm(_("hotspot profile change"))) {
						var br			= getBrowser();
						resetform		= document.spotCfgReset;
						resetform.profile.value	= form.spotProfile.value;
						ajaxShowTimer(resetform, 'timerReloader', _('message apply'), 15);
						if ((br.browser != "ie") || (br.browser != "edge") || (br.browser != "ie11") || (br.browser != "firefox"))
							resetform.submit();
					}
				}
				for(var i = 0; i < Profiles.length; i++) {
					var tmp = Profiles[i];
					if (tmp[0] == form.spotProfile.value) {
						insertVal(form, "sIp",				(!tmp[2])  ? SPOT_IP 							: tmp[2],	tmp[29+2]);
						insertVal(form, "sNetmask",			(!tmp[3])  ? SPOT_NETMASK						: tmp[3],	tmp[29+3]);
						insertVal(form, "sStartIP",			(!tmp[4])  ? NVRAM_chilli_dhcpstart				: tmp[4],	tmp[29+4]);
						insertVal(form, "sEndIP",			(!tmp[5])  ? NVRAM_chilli_dhcpend				: tmp[5],	tmp[29+5]);
						insertVal(form, "sPriDns",			(!tmp[6])  ? NVRAM_chilli_dns1					: tmp[6],	tmp[29+6]);
						insertVal(form, "sSecDns",			(!tmp[7])  ? NVRAM_chilli_dns2					: tmp[7],	tmp[29+7]);
						insertVal(form, "sDomain",			(!tmp[8])  ? NVRAM_chilli_domain				: tmp[8],	tmp[29+8]);
						insertVal(form, "sLease",			(!tmp[9])  ? NVRAM_chilli_lease					: tmp[9],	tmp[29+9]);
						insertVal(form, "sRadServer1",		(!tmp[10]) ? NVRAM_chilli_radiusserver1 		: tmp[10],	tmp[29+10]);
						insertVal(form, "sRadServer2",		(!tmp[11]) ? NVRAM_chilli_radiusserver2 		: tmp[11],	tmp[29+11]);
						insertVal(form, "sRadSecret",		(!tmp[12]) ? NVRAM_chilli_radiussecret			: tmp[12],	tmp[29+12]);
						insertVal(form, "sNasId",			(!tmp[13]) ? NVRAM_chilli_radiusnasid			: tmp[13],	tmp[29+13]);
						insertVal(form, "sRadLocId",		(!tmp[14]) ? NVRAM_chilli_radiuslocationid		: tmp[14],	tmp[29+14]);
						insertVal(form, "sRadLocName",		(!tmp[15]) ? NVRAM_chilli_radiuslocationname	: tmp[15],	tmp[29+15]);
						insertVal(form, "sRadCoaPort",		(!tmp[16]) ? NVRAM_chilli_coaport				: tmp[16],	tmp[29+16]);
						insertVal(form, "sRadCoaNoIpCheck",	(!tmp[17]) ? NVRAM_chilli_coanoipcheck			: tmp[17],	tmp[29+17]);
						insertVal(form, "sUamServer",		(!tmp[18]) ? NVRAM_chilli_uamserver				: tmp[18],	tmp[29+18]);
						insertVal(form, "sUamHomepage",		(!tmp[19]) ? NVRAM_chilli_uamhomepage			: tmp[19],	tmp[29+19]);
						insertVal(form, "sUamSecret",		(!tmp[20]) ? NVRAM_chilli_uamsecret				: tmp[20],	tmp[29+20]);
						insertVal(form, "sUamAllowed",		(!tmp[21]) ? NVRAM_chilli_uamallowed			: tmp[21],	tmp[29+21]);
						insertVal(form, "sUamDomain",		(!tmp[22]) ? NVRAM_chilli_uamdomain				: tmp[22],	tmp[29+22]);
						insertVal(form, "sUamAnyDNS",		(!tmp[23]) ? NVRAM_chilli_uamanydns				: tmp[23],	tmp[29+23]);
						insertVal(form, "sMacAllowed",		(!tmp[24]) ? NVRAM_chilli_macallowed			: tmp[24],	tmp[29+24]);
						insertVal(form, "sC2Cisolate",		(!tmp[25]) ? NVRAM_chilli_clisolate				: tmp[25],	tmp[29+25]);
						insertVal(form, "sDropDNS",			(!tmp[26]) ? NVRAM_chilli_dnsparanoia			: tmp[26],	tmp[29+26]);
						insertVal(form, "sMaxClients",		(!tmp[27]) ? NVRAM_chilli_maxclients			: tmp[27],	tmp[29+27]);
						insertVal(form, "sHttpsRedirect",	(!tmp[28]) ? NVRAM_chilli_https					: tmp[28],	tmp[29+28]);
					}
				}
			}

			function ModeOnChange(form) {
				displayElement(["row_chilli", "row_sIp", "row_sNetmask", "row_sStartIP", "row_sEndIP", "row_sPriDns", "row_sSecDns", "row_sDomain", "row_sLease", "row_sRadServer1", "row_sRadServer2", "row_sRadSecret", "row_sNasId", "row_sRadLocId", "row_sRadLocName", "row_sRadCoaPort", "row_sRadCoaNoIpCheck", "row_sUamServer", "row_sUamHomepage", "row_sUamSecret", "row_sUamAllowed", "row_sUamDomain", "row_sUamAnyDNS", "row_sMacAllowed", "row_sC2Cisolate", "row_sDropDNS", "row_sMaxClients", "row_sHttpsRedirect"], form.spotEnable.value == '1');
				displayElement(["row_GatewayIPRange", "row_RedirectURL", "row_MaxClients", "row_ClientIdleTimeout", "row_ClientForceTimeout", "row_AuthenticateImmediately", "row_MACMechanism", "row_TrustedMACList", "row_AllowedMACList", "row_BlockedMACList", "row_PasswordAuthentication", "row_Password", "row_UsernameAuthentication", "row_Username", "row_PasswordAttempts"], form.spotEnable.value == '2');

				form.chilliEnable.value = (form.spotEnable.value == "1") ? "on" : "off";
				form.nodogEnable.value = (form.spotEnable.value == "2") ? "1" : "0";

				if (form.spotEnable.value == "1")
					ProfileOnChange(form);
				else if (form.spotEnable.value == "2")
					MACMechanismOnChange(form);
			}

			function MACMechanismOnChange(form) {
				displayElement("row_AllowedMACList", form.MACMechanism.value == "allow");
				displayElement("row_BlockedMACList", form.MACMechanism.value == "block");
				displayElement("row_Password", +form.PasswordAuthentication.value == 1);
				displayElement("row_Username", +form.UsernameAuthentication.value == 1);
			}

			function insertVal(form, row, val, disp) {
				form.elements[row].value = val;
				displayElement("row_" + row, disp);
			}

			function hideHint() {
				var row = document.getElementById('hint_row');
				row.innerHTML = '';
			}

			function showHint(key){
				var row	 = document.getElementById('hint_row');
				var text = '<div class="hint"><font color="#0000ff"><b>' + _("hint title") + ':</b></font>&nbsp;';
				var show = true;

				if (key == 'spot_enable')
					text += _("hint spot enable");

			/*
				// Hints for Chilli
				if (document.spotCfg.spotEnable.value == "1")
					switch(key) {
						case 'spot_profile':			text += _("hint spot profile");				break;
						case 'spot_ip':				text += _("hint spot ip");				break;
						case 'spot_mask':			text += _("hint spot mask");				break;
						case 'spot_startip':			text += _("hint spot startip");				break;
						case 'spot_endip':			text += _("hint spot endip");				break;
						case 'spot_dns':			text += _("hint spot dns");				break;
						case 'spot_domain':			text += _("hint spot domain");				break;
						case 'spot_lease':			text += _("hint spot lease");				break;
						case 'spot_auth':			text += _("hint spot auth");				break;
						case 'spot_acct':			text += _("hint spot acct");				break;
						case 'spot_rad_secret':			text += _("hint spot rad secret");			break;
						case 'spot_nasid':			text += _("hint spot nasid");				break;
						case 'spot_locid':			text += _("hint spot locid");				break;
						case 'spot_locname':			text += _("hint spot locname");				break;
						case 'spot_port':			text += _("hint spot port");				break;
						case 'spot_ipcheck':			text += _("hint spot ipcheck");				break;
						case 'spot_uamserver':			text += _("hint spot uamserver");			break;
						case 'spot_uamhomepage':		text += _("hint spot uamhomepage");			break;
						case 'spot_uamsecret':			text += _("hint spot uamsecret");			break;
						case 'spot_uamallowed':			text += _("hint spot uamallowed");			break;
						case 'spot_uamdomain':			text += _("hint spot uamdomain");			break;
						case 'spot_uamdns':			text += _("hint spot uamdns");				break;
						case 'spot_macallowed':			text += _("hint spot macallowed");			break;
						default:				show = false;
					}
			  else */
				if (document.spotCfg.spotEnable.value == "2") // hints for nodog
					switch(key) {
						case 'GatewayIPRange':			text += _("hint nodog ip range");			break;
						case 'RedirectURL':				text += _("hint nodog url");				break;
						case 'MaxClients':				text += _("hint nodog max clients");		break;
						case 'ClientIdleTimeout':		text += _("hint nodog idle");				break;
						case 'ClientForceTimeout':		text += _("hint nodog force");				break;
						case 'AuthenticateImmediately':	text += _("hint nodog auth immediately");	break;
						case 'MACMechanism':			text += _("hint nodog mac"); 				break;
						case 'BlockedMACList':			text += _("hint nodog blocked");			break;
						case 'AllowedMACList':			text += _("hint nodog allowed");			break;
						case 'TrustedMACList':			text += _("hint nodog trusted");			break;
						case 'PasswordAuthentication':	text += _("hint nodog auth pass");			break;
						case 'Password':				text += _("hint nodog pass");				break;
						case 'UsernameAuthentication':	text += _("hint nodog auth user");			break;
						case 'Username':				text += _("hint nodog user");				break;
						case 'PasswordAttempts':		text += _("hint nodog attempts");			break;
						default:						show = false;
					}
				else
					show = false;

				if (show) {
					text += '</div>';
					row.innerHTML = text;
				}
			}

			// Display server status
			function displayServiceHandler(response) {
				var daemons = response.split(',');

				if (NVRAM_chilli_enable == 'off' && NVRAM_nodogsplash_enable == '0')
					document.getElementById('spotSetup_status').innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
				else
					document.getElementById('spotSetup_status').innerHTML = (daemons.indexOf('chilli') >= 0 || daemons.indexOf('nodogsplash') >= 0) ?
						'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
						'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
				setTimeout('displayServiceStatus();', 5000);
			}

			// Get server status
			function displayServiceStatus() {
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="sTitle"></h1>
					<p id="sIntroduction"></p>
					<hr />
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="spotCfgReset" action="/goform/resetHotspot"><input type="hidden" id="profile" name="profile"></form>
					<form method="POST" name="spotCfg" action="/goform/setHotspot" onSubmit="return CheckValue(this);">
						<table class="form">
							<col style="width: 40%" />
							<col style="width: 50%" />
							<col style="width: 10%" />
							<tbody>
								<tr onMouseOver="showHint('spot_enable');" onMouseOut="hideHint();">
									<td colspan="3" class="title" id="spotSetup">Hotspot Setup</td>
								</tr>
								<tr id="row_spotSetup">
									<td class="head" id="spotType">Hotspot type</td>
									<td>
										<select name="spotEnable" id="spotEnable" class="mid" onChange="ModeOnChange(this.form);">
										</select>
									</td>
									<td id="spotSetup_status" style="text-align: center"></td>
								</tr>
								<!-- Coova setup -->
								<tr id="row_chilli" onMouseOver="showHint('spot_profile');" onMouseOut="hideHint();">
									<td class="head" id="spotProfile">Chilli profile</td>
									<td colspan="2">
										<select name="spotProfile" class="mid" onChange="ProfileOnChange(this.form);"></select>
									</td>
								</tr>
								<tr id="row_sIp" onMouseOver="showHint('spot_ip');" onMouseOut="hideHint();">
									<td class="head" id="sIp">Subnet Address</td>
									<td colspan="2"><input class="mid" name="sIp" maxlength="15"></td>
								</tr>
								<tr id="row_sNetmask" onMouseOver="showHint('spot_mask');" onMouseOut="hideHint();">
									<td class="head" id="sNetmask">Subnet Mask</td>
									<td colspan="2"><input class="mid" name="sNetmask" maxlength="15"></td>
								</tr>
								<tr id="row_sStartIP" onMouseOver="showHint('spot_startip');" onMouseOut="hideHint();">
									<td class="head" id="sStartIP">Start IP</td>
									<td colspan="2"><input class="mid" name="sStartIP" maxlength="3"></td>
								</tr>
								<tr id="row_sEndIP" onMouseOver="showHint('spot_endip');" onMouseOut="hideHint();">
									<td class="head" id="sEndIP">End IP</td>
									<td colspan="2"><input class="mid" name="sEndIP" maxlength="3"></td>
								</tr>
								<tr id="row_sC2Cisolate" onMouseOver="showHint('spot_c2cisolate');" onMouseOut="hideHint();">
									<td class="head" id="sC2Cisolate">Use /32 mask for clients</td>
									<td colspan="2">
										<select name="sC2Cisolate" class="mid">
											<option value="0" id="disable">Disable</option>
											<option value="1" id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_sMaxClients" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
									<td class="head" id="sMaxClients">Maximum Clients</td>
									<td colspan="2"><input class="mid" name="sMaxClients" maxlength="4"></td>
								</tr>
								<tr id="row_sLease" onMouseOver="showHint('spot_lease');" onMouseOut="hideHint();">
									<td class="head" id="sLease">Lease time (sec)</td>
									<td colspan="2"><input class="mid" name="sLease" maxlength="8"></td>
								</tr>
								<tr id="row_sPriDns" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
									<td class="head" id="sPriDns">Primary DNS Server</td>
									<td colspan="2"><input class="mid" name="sPriDns" maxlength="15"></td>
								</tr>
								<tr id="row_sSecDns" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
									<td class="head" id="sSecDns">Secondary DNS Server</td>
									<td colspan="2"><input class="mid" name="sSecDns" maxlength="15"></td>
								</tr>
								<tr id="row_sDropDNS" onMouseOver="showHint('spot_dropdns');" onMouseOut="hideHint();">
									<td class="head" id="sDropDNS">Drop DNS responses</td>
									<td colspan="2">
										<select name="sDropDNS" class="mid">
											<option value="0" id="disable">Disable</option>
											<option value="1" id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_sUamAnyDNS" onMouseOver="showHint('spot_uamdns');" onMouseOut="hideHint();">
									<td class="head" id="sUamAnyDNS">Allow all DNS requests</td>
									<td colspan="2">
										<select name="sUamAnyDNS" class="mid">
											<option value="0" id="disable">Disable</option>
											<option value="1" id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_sDomain" onMouseOver="showHint('spot_domain');" onMouseOut="hideHint();">
									<td class="head" id="sDomain">Domain name</td>
									<td colspan="2"><input class="mid" name="sDomain" maxlength="64"></td>
								</tr>
								<tr id="row_sRadServer1" onMouseOver="showHint('spot_auth');" onMouseOut="hideHint();">
									<td class="head" id="sRadServer1">Radius server auth address</td>
									<td colspan="2"><input class="mid" name="sRadServer1" maxlength="256"></td>
								</tr>
								<tr id="row_sRadServer2" onMouseOver="showHint('spot_acct');" onMouseOut="hideHint();">
									<td class="head" id="sRadServer2">Radius server acct address</td>
									<td colspan="2"><input class="mid" name="sRadServer2" maxlength="256"></td>
								</tr>
								<tr id="row_sRadSecret" onMouseOver="showHint('spot_rad_secret');" onMouseOut="hideHint();">
									<td class="head" id="sRadSecret">Radius server secret</td>
									<td colspan="2"><input class="mid" name="sRadSecret" maxlength="256" type="password"></td>
								</tr>
								<tr id="row_sNasId" onMouseOver="showHint('spot_nasid');" onMouseOut="hideHint();">
									<td class="head" id="sNasId">Radius NAS ID</td>
									<td colspan="2"><input class="mid" name="sNasId" maxlength="256"></td>
								</tr>
								<tr id="row_sRadLocId" onMouseOver="showHint('spot_locid');" onMouseOut="hideHint();">
									<td class="head" id="sRadLocId">Radius Location ID</td>
									<td colspan="2"><input class="mid" name="sRadLocId" maxlength="15"></td>
								</tr>
								<tr id="row_sRadLocName" onMouseOver="showHint('spot_locname');" onMouseOut="hideHint();">
									<td class="head" id="sRadLocName">Radius Location Name</td>
									<td colspan="2"><input class="mid" name="sRadLocName" maxlength="15"></td>
								</tr>
								<tr id="row_sRadCoaPort" onMouseOver="showHint('spot_port');" onMouseOut="hideHint();">
									<td class="head" id="sRadCoaPort">Radius CoA/DM port</td>
									<td colspan="2"><input class="mid" name="sRadCoaPort" maxlength="15"></td>
								</tr>
								<tr id="row_sUamServer" onMouseOver="showHint('spot_uamserver');" onMouseOut="hideHint();">
									<td class="head" id="sUamServer">UAM server URL</td>
									<td colspan="2"><input class="mid" name="sUamServer" maxlength="256"></td>
								</tr>
								<tr id="row_sUamHomepage" onMouseOver="showHint('spot_uamhomepage');" onMouseOut="hideHint();">
									<td class="head" id="sUamHomepage">UAM homepage URL</td>
									<td colspan="2"><input class="mid" name="sUamHomepage" maxlength="256"></td>
								</tr>
								<tr id="row_sUamSecret" onMouseOver="showHint('spot_uamsecret');" onMouseOut="hideHint();">
									<td class="head" id="sUamSecret">UAM secret</td>
									<td colspan="2"><input class="mid" name="sUamSecret" maxlength="256" type="password"></td>
								</tr>
								<tr id="row_sUamAllowed" onMouseOver="showHint('spot_uamallowed');" onMouseOut="hideHint();">
									<td class="head" id="sUamAllowed">UAM allowed hosts</td>
									<td colspan="2"><input class="mid" name="sUamAllowed" maxlength="1024"></td>
								</tr>
								<tr id="row_sUamDomain" onMouseOver="showHint('spot_uamdomain');" onMouseOut="hideHint();">
									<td class="head" id="sUamDomain">UAM allowed domains</td>
									<td colspan="2"><input class="mid" name="sUamDomain" maxlength="1024"></td>
								</tr>
								<tr id="row_sMacAllowed" onMouseOver="showHint('spot_macallowed');" onMouseOut="hideHint();">
									<td class="head" id="sMacAllowed">Allowed MAC</td>
									<td colspan="2"><input class="mid" name="sMacAllowed" maxlength="512"></td>
								</tr>
								<tr id="row_sHttpsRedirect" onMouseOver="showHint('spot_httpsredirect');" onMouseOut="hideHint();">
									<td class="head" id="sHttpsRedirect">HTTPS Redirect</td>
									<td colspan="2">
										<select name="sHttpsRedirect" class="mid">
											<option value="0" id="disable">Disable</option>
											<option value="1" id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_sRadCoaNoIpCheck" onMouseOver="showHint('spot_ipcheck');" onMouseOut="hideHint();">
									<td class="head" id="sRadCoaNoIpCheck">No IP check for CoA/DM request</td>
									<td colspan="2">
										<select name="sRadCoaNoIpCheck" class="mid">
											<option value="0" id="disable">Disable</option>
											<option value="1" id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<!-- Nodog setup -->
								<tr id="row_GatewayIPRange" onMouseOver="showHint('GatewayIPRange');" onMouseOut="hideHint();">
									<td class="head" id="GatewayIPRange">Range of IP addresses</td>
									<td colspan="2"><input class="mid" name="GatewayIPRange" maxlength="128"></td>
								</tr>
								<tr id="row_RedirectURL" onMouseOver="showHint('RedirectURL');" onMouseOut="hideHint();">
									<td class="head" id="RedirectURL">Redirected to URL after authentication</td>
									<td colspan="2"><input class="mid" name="RedirectURL" maxlength="128"></td>
								</tr>
								<tr id="row_MaxClients" onMouseOver="showHint('MaxClients');" onMouseOut="hideHint();">
									<td class="head" id="MaxClients">Maximum number of users</td>
									<td colspan="2"><input class="mid" name="MaxClients" maxlength="128"></td>
								</tr>
								<tr id="row_ClientIdleTimeout" onMouseOver="showHint('ClientIdleTimeout');" onMouseOut="hideHint();">
									<td class="head" id="ClientIdleTimeout">Inactivity before a user is 'deauthenticated'</td>
									<td colspan="2"><input class="mid" name="ClientIdleTimeout" maxlength="128"></td>
								</tr>
								<tr id="row_ClientForceTimeout" onMouseOver="showHint('ClientForceTimeout');" onMouseOut="hideHint();">
									<td class="head" id="ClientForceTimeout">Minutes before a user is 'deauthenticated'</td>
									<td colspan="2"><input class="mid" name="ClientForceTimeout" maxlength="128"></td>
								</tr>
								<tr id="row_AuthenticateImmediately" onMouseOver="showHint('AuthenticateImmediately');" onMouseOut="hideHint();">
									<td class="head" id="AuthenticateImmediately">Authenticate Immediately</td>
									<td colspan="2">
										<select name="AuthenticateImmediately" class="mid">
											<option value=0 id="disable">Disable</option>
											<option value=1 id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_MACMechanism" onMouseOver="showHint('MACMechanism');" onMouseOut="hideHint();">
									<td class="head" id="MACMechanism">MAC Mechanism</td>
									<td colspan="2">
										<select name="MACMechanism" class="mid" onChange="MACMechanismOnChange(this.form);">
											<option value="block" id="block">Block</option>
											<option value="allow" id="allow">Allow</option>
										</select>
									</td>
								</tr>
								<tr id="row_BlockedMACList" onMouseOver="showHint('BlockedMACList');" onMouseOut="hideHint();">
									<td class="head" id="BlockedMACList">List Blocked MAC Addresses</td>
									<td colspan="2"><input class="mid" name="BlockedMACList" maxlength="128"></td>
								</tr>
								<tr id="row_AllowedMACList" onMouseOver="showHint('AllowedMACList');" onMouseOut="hideHint();">
									<td class="head" id="AllowedMACList">List Allowed MAC Addresses</td>
									<td colspan="2"><input class="mid" name="AllowedMACList" maxlength="128"></td>
								</tr>
								<tr id="row_TrustedMACList" onMouseOver="showHint('TrustedMACList');" onMouseOut="hideHint();">
									<td class="head" id="TrustedMACList">List Trusted MAC Addresses</td>
									<td colspan="2"><input class="mid" name="TrustedMACList" maxlength="128"></td>
								</tr>
								<tr id="row_PasswordAuthentication" onMouseOver="showHint('PasswordAuthentication');" onMouseOut="hideHint();">
									<td class="head" id="PasswordAuthentication">Password Authentication</td>
									<td colspan="2">
										<select name="PasswordAuthentication" class="mid" onChange="MACMechanismOnChange(this.form);">
											<option value=0 id="disable">Disable</option>
											<option value=1 id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_Password" onMouseOver="showHint('Password');" onMouseOut="hideHint();">
									<td class="head" id="Password">Password</td>
									<td colspan="2"><input class="mid" type="password" name="Password" maxlength="128"></td>
								</tr>
								<tr id="row_UsernameAuthentication" onMouseOver="showHint('UsernameAuthentication');" onMouseOut="hideHint();">
									<td class="head" id="UsernameAuthentication">Username Authentication</td>
									<td colspan="2">
										<select name="UsernameAuthentication" class="mid" onChange="MACMechanismOnChange(this.form);">
											<option value=0 id="disable">Disable</option>
											<option value=1 id="enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="row_Username" onMouseOver="showHint('Username');" onMouseOut="hideHint();">
									<td class="head" id="Username">Username</td>
									<td colspan="2"><input class="mid" name="Username" maxlength="128"></td>
								</tr>
								<tr id="row_PasswordAttempts" onMouseOver="showHint('PasswordAttempts');" onMouseOut="hideHint();">
									<td class="head" id="PasswordAttempts">Password Attempts</td>
									<td colspan="2"><input class="mid" name="PasswordAttempts" maxlength="128"></td>
								</tr>
							</tbody>
						</table>
						<table class="button">
							<tr>
								<td>
									<input type="submit" class="normal" value="Apply" id="sApply">&nbsp;&nbsp;
									<input type="reset" class="normal" value="Cancel" id="sCancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="normal" value="Reset" id="sReset" onClick="resetValues(this.form);">
									<input type="hidden" name="reset" value="0">
									<input type="hidden" name="chilliEnable">
									<input type="hidden" name="nodogEnable">
								</td>
							</tr>
						</table>
					</form>
					<div id="hint_row">&nbsp;</div>
					<div class="whitespace">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
