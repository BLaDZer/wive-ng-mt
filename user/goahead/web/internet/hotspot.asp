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
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="hotspot_profiles.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("internet");
			Butterlate.setTextDomain("hint");
			Butterlate.setTextDomain("buttons");

			var old_profile 	= '<% getCfgZero(1, "chilli_profile"); %>';
			var chilli_built 	= '<% getChilliBuilt(); %>' == '1';
			var nodog_built 	= '<% getNoDogBuilt(); %>' == '1';

			function initTranslation() {
				_TR("sTitle",				"hotspot title");
				_TR("sIntroduction",			"hotspot introduction");
				_TR("spotSetup",			"hotspot setup");
				_TR("spotProfile",			"hotspot profile");
				_TR("sIp",				"inet ip");
				_TR("sNetmask",				"inet netmask");
				_TR("sStartIP",				"lan dhcp start");
				_TR("sEndIP",				"lan dhcp end");
				_TR("sPriDns",				"inet pri dns");
				_TR("sSecDns",				"inet sec dns");
				_TR("sDropDNS",				"hotspot drop dns");
				_TR("sC2Cisolate",			"hotspot clisolate");
				_TR("sMaxClients",			"hotspot max clients");
				_TR("sDomain",				"hotspot domain");
				_TR("sLease",				"lan dhcp lease");
				_TR("sRadServer1",			"hotspot radius auth");
				_TR("sRadServer2",			"hotspot radius acct");
				_TR("sRadSecret",			"hotspot radius secret");
				_TR("sNasId",				"hotspot radius nas");
				_TR("sRadLocId",			"hotspot radius location id");
				_TR("sRadLocName",			"hotspot radius location name");
				_TR("sRadCoaPort",			"hotspot radius coa port");
				_TR("sRadCoaNoIpCheck",			"hotspot radius no check ip");
				_TR("sUamServer",			"hotspot uam server");
				_TR("sUamHomepage",			"hotspot uam homepage");
				_TR("sUamSecret",			"hotspot uam secret");
				_TR("sUamAllowed",			"hotspot uam allowed");
				_TR("sUamDomain",			"hotspot uam domain");
				_TR("sUamAnyDNS",			"hotspot uam any dns");
				_TR("sMacAllowed",			"hotspot uam mac");
				_TR("GatewayIPRange",			"hotspot nodog ip range");
				_TR("RedirectURL",			"hotspot nodog url");
				_TR("MaxClients",			"hotspot nodog max clients");
				_TR("ClientIdleTimeout",		"hotspot nodog idle timeout");
				_TR("ClientForceTimeout",		"hotspot nodog force timeout");
				_TR("AuthenticateImmediately",		"hotspot nodog auth");
				_TR("MACMechanism",			"hotspot nodog mechanism");
				_TR("BlockedMACList",			"hotspot nodog blocked list");
				_TR("AllowedMACList",			"hotspot nodog allowed list");
				_TR("TrustedMACList",			"hotspot nodog trusted list");
				_TR("PasswordAuthentication",		"hotspot nodog pass");
				_TR("block",				"hotspot nodog block");
				_TR("allow",				"hotspot nodog allow");
				_TR("Password",				"vpn password");
				_TR("UsernameAuthentication",		"hotspot nodog user");
				_TR("Username",				"vpn username");
				_TR("PasswordAttempts",			"hotspot nodog pass attempts");
				_TRV("sApply",				"button apply");
				_TRV("sCancel",				"button cancel");
				_TRV("sReset",				"button reset");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					if (elements[i].id == "disable")
						elements[i].innerHTML = _("button disable");
					else if (elements[i].id == "enable")
						elements[i].innerHTML = _("button enable");
			}

			function initValues() {
				var form	= document.spotCfg;
				var nat_fp	= defaultNumber('<% getCfgGeneral(1, "offloadMode"); %>', "1");
				var enabled = form.spotEnable;

				form.AuthenticateImmediately.value		= ('<% getCfgGeneral(1, "nodog_AuthenticateImmediately"); %>' == "1") ? "1" : "0";
				form.MACMechanism.value 			= ('<% getCfgGeneral(1, "nodog_MACMechanism"); %>' == "allow") ? "allow" : "block";
				form.PasswordAuthentication.value		= ('<% getCfgGeneral(1, "nodog_PasswordAuthentication"); %>' == "1") ? "1" : "0";
				form.UsernameAuthentication.value		= ('<% getCfgGeneral(1, "nodog_UsernameAuthentication"); %>' == "1") ? "1" : "0";
				form.GatewayIPRange.value			= '<% getCfgGeneral(1, "nodog_GatewayIPRange"); %>';
				form.RedirectURL.value				= '<% getCfgGeneral(1, "nodog_RedirectURL"); %>';
				form.MaxClients.value				= '<% getCfgGeneral(1, "nodog_MaxClients"); %>';
				form.ClientIdleTimeout.value			= '<% getCfgGeneral(1, "nodog_ClientIdleTimeout"); %>';
				form.ClientForceTimeout.value			= '<% getCfgGeneral(1, "nodog_ClientForceTimeout"); %>';
				form.BlockedMACList.value			= '<% getCfgGeneral(1, "nodog_BlockedMACList"); %>';
				form.AllowedMACList.value			= '<% getCfgGeneral(1, "nodog_AllowedMACList"); %>';
				form.TrustedMACList.value			= '<% getCfgGeneral(1, "nodog_TrustedMACList"); %>';
				form.Password.value				= '<% getCfgGeneral(1, "nodog_Password"); %>';
				form.Username.value				= '<% getCfgGeneral(1, "nodog_Username"); %>';
				form.PasswordAttempts.value			= '<% getCfgGeneral(1, "nodog_PasswordAttempts"); %>';
				form.chilliEnable.value				= '<% getCfgZero(1, "chilli_enable"); %>';
				form.nodogEnable.value				= '<% getCfgZero(1, "nodogsplash_enable"); %>';

				document.getElementById('sC2Cisolate').title	= _("hotspot clisolate full");
				document.getElementById('sDropDNS').title	= _("hotspot drop dns full");

				enabled.options.length = 0;
				addOption(enabled, _("button disable"), "0");
				if(chilli_built)
					addOption(enabled, "ChilliSpot", "1");
				if(nodog_built)
					addOption(enabled, "NoDogSplash", "2");

				if(chilli_built && (form.chilliEnable.value == "on"))
					enabled.value = "1";
				else if(nodog_built && (form.nodogEnable.value == "1"))
					enabled.value = "2";
				else
					enabled.value = "0";

				var prof = form.spotProfile;
				prof.options.length = 0;
				for(var i = 0; i < Profiles.length; i++) {
					addOption(prof, Profiles[i][1], Profiles[i][0]);
					if(Profiles[i][0] == old_profile)
						prof.value = old_profile;
				}
				if(old_profile == "0")
					prof.options.selectedIndex == 0;

				ModeOnChange(form);
				showWarning();
				initTranslation();
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
				if (form.spotProfile.value != "manual" && form.spotProfile.value != old_profile && old_profile != "manual" && old_profile != "0") {
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
						insertVal(form, "sIp",			(!tmp[2]) ? "<% getSpotIp(); %>" : tmp[2],					tmp[28+2]);
						insertVal(form, "sNetmask",		(!tmp[3]) ? "<% getSpotNetmask(); %>" : tmp[3],					tmp[28+3]);
						insertVal(form, "sStartIP",		(!tmp[4]) ? "<% getCfgGeneral(1, "chilli_dhcpstart"); %>" : tmp[4],		tmp[28+4]);
						insertVal(form, "sEndIP",		(!tmp[5]) ? "<% getCfgGeneral(1, "chilli_dhcpend"); %>" : tmp[5],		tmp[28+5]);
						insertVal(form, "sPriDns",		(!tmp[6]) ? "<% getCfgGeneral(1, "chilli_dns1"); %>" : tmp[6],			tmp[28+6]);
						insertVal(form, "sSecDns",		(!tmp[7]) ? "<% getCfgGeneral(1, "chilli_dns2"); %>" : tmp[7],			tmp[28+7]);
						insertVal(form, "sDomain",		(!tmp[8]) ? "<% getCfgGeneral(1, "chilli_domain"); %>" : tmp[8],		tmp[28+8]);
						insertVal(form, "sLease",		(!tmp[9]) ? "<% getCfgGeneral(1, "chilli_lease"); %>" : tmp[9],			tmp[28+9]);
						insertVal(form, "sRadServer1",		(!tmp[10]) ? "<% getCfgGeneral(1, "chilli_radiusserver1"); %>" : tmp[10],	tmp[28+10]);
						insertVal(form, "sRadServer2",		(!tmp[11]) ? "<% getCfgGeneral(1, "chilli_radiusserver2"); %>" : tmp[11],	tmp[28+11]);
						insertVal(form, "sRadSecret",		(!tmp[12]) ? "<% getCfgGeneral(1, "chilli_radiussecret"); %>" : tmp[12],	tmp[28+12]);
						insertVal(form, "sNasId",		(!tmp[13]) ? "<% getCfgGeneral(1, "chilli_radiusnasid"); %>" : tmp[13],		tmp[28+13]);
						insertVal(form, "sRadLocId",		(!tmp[14]) ? "<% getCfgGeneral(1, "chilli_radiuslocationid"); %>" : tmp[14],	tmp[28+14]);
						insertVal(form, "sRadLocName",		(!tmp[15]) ? "<% getCfgGeneral(1, "chilli_radiuslocationname"); %>" : tmp[15],	tmp[28+15]);
						insertVal(form, "sRadCoaPort",		(!tmp[16]) ? "<% getCfgGeneral(1, "chilli_coaport"); %>" : tmp[16],		tmp[28+16]);
						insertVal(form, "sRadCoaNoIpCheck",	(!tmp[17]) ? "<% getCfgGeneral(1, "chilli_coanoipcheck"); %>" : tmp[17],	tmp[28+17]);
						if(!tmp[17])
							form.sRadCoaNoIpCheck.value = ("<% getCfgGeneral(1, "chilli_coanoipcheck"); %>" == "on") ? "on" : "off";
						insertVal(form, "sUamServer",		(!tmp[18]) ? "<% getCfgGeneral(1, "chilli_uamserver"); %>" : tmp[18],		tmp[28+18]);
						insertVal(form, "sUamHomepage",		(!tmp[19]) ? "<% getCfgGeneral(1, "chilli_uamhomepage"); %>" : tmp[19],		tmp[28+19]);
						insertVal(form, "sUamSecret",		(!tmp[20]) ? "<% getCfgGeneral(1, "chilli_uamsecret"); %>" : tmp[20],		tmp[28+20]);
						insertVal(form, "sUamAllowed",		(!tmp[21]) ? "<% getCfgGeneral(1, "chilli_uamallowed"); %>" : tmp[21],		tmp[28+21]);
						insertVal(form, "sUamDomain",		(!tmp[22]) ? "<% getCfgGeneral(1, "chilli_uamdomain"); %>" : tmp[22],		tmp[28+22]);
						insertVal(form, "sUamAnyDNS",		(!tmp[23]) ? "<% getCfgGeneral(1, "chilli_uamanydns"); %>" : tmp[23],		tmp[28+23]);
						if(!tmp[23])
							form.sUamAnyDNS.value = ("<% getCfgGeneral(1, "chilli_uamanydns"); %>" == "on") ? "on" : "off";
						insertVal(form, "sMacAllowed",		(!tmp[24]) ? "<% getCfgGeneral(1, "chilli_macallowed"); %>" : tmp[24],		tmp[28+24]);
						insertVal(form, "sC2Cisolate",		(!tmp[25]) ? ("<% getCfgGeneral(1, "chilli_clisolate"); %>" == "") ? 0 : "<% getCfgGeneral(1, "chilli_clisolate"); %>" : tmp[25],		tmp[28+25]);
						insertVal(form, "sDropDNS",		(!tmp[26]) ? ("<% getCfgGeneral(1, "chilli_dnsparanoia"); %>" == "") ? 0 : "<% getCfgGeneral(1, "chilli_dnsparanoia"); %>" : tmp[26],		tmp[28+26]);
						insertVal(form, "sMaxClients",		(!tmp[27]) ? ("<% getCfgGeneral(1, "chilli_maxclients"); %>" == "") ? 384 : "<% getCfgGeneral(1, "chilli_maxclients"); %>" : tmp[27],		tmp[28+27]);
					}
				}
			}

			function ModeOnChange(form) {
				displayElement(["row_chilli", "row_sIp", "row_sNetmask", "row_sStartIP", "row_sEndIP", "row_sPriDns", "row_sSecDns", "row_sDomain", "row_sLease", "row_sRadServer1", "row_sRadServer2", "row_sRadSecret", "row_sNasId", "row_sRadLocId", "row_sRadLocName", "row_sRadCoaPort", "row_sRadCoaNoIpCheck", "row_sUamServer", "row_sUamHomepage", "row_sUamSecret", "row_sUamAllowed", "row_sUamDomain", "row_sUamAnyDNS", "row_sMacAllowed", "row_sC2Cisolate", "row_sDropDNS", "row_sMaxClients"], (form.spotEnable.value == "1"));
				displayElement(["row_GatewayIPRange", "row_RedirectURL", "row_MaxClients", "row_ClientIdleTimeout", "row_ClientForceTimeout", "row_AuthenticateImmediately", "row_MACMechanism", "row_TrustedMACList", "row_AllowedMACList", "row_BlockedMACList", "row_PasswordAuthentication", "row_Password", "row_UsernameAuthentication", "row_Username", "row_PasswordAttempts"], (form.spotEnable.value == "2"));

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
				displayElement("row_Password", (form.PasswordAuthentication.value*1) == 1);
				displayElement("row_Username", (form.UsernameAuthentication.value*1) == 1);
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
						case 'RedirectURL':			text += _("hint nodog url");				break;
						case 'MaxClients':			text += _("hint nodog max clients");			break;
						case 'ClientIdleTimeout':		text += _("hint nodog idle");				break;
						case 'ClientForceTimeout':		text += _("hint nodog force");				break;
						case 'AuthenticateImmediately':		text += _("hint nodog auth immediately");		break;
						case 'MACMechanism':			text += _("hint nodog mac"); 				break;
						case 'BlockedMACList':			text += _("hint nodog blocked");			break;
						case 'AllowedMACList':			text += _("hint nodog allowed");			break;
						case 'TrustedMACList':			text += _("hint nodog trusted");			break;
						case 'PasswordAuthentication':		text += _("hint nodog auth pass");			break;
						case 'Password':			text += _("hint nodog pass");				break;
						case 'UsernameAuthentication':		text += _("hint nodog auth user");			break;
						case 'Username':			text += _("hint nodog user");				break;
						case 'PasswordAttempts':		text += _("hint nodog attempts");			break;
						default:				show = false;
					}
				else
					show = false;

				if (show) {
					text += '</div>';
					row.innerHTML = text;
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td>
					<h1 id="sTitle"></h1>
					<p id="sIntroduction"></p>
					<hr />
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="spotCfgReset" action="/goform/resetHotspot"><input type="hidden" id="profile" name="profile"></form>
					<form method="POST" name="spotCfg" action="/goform/setHotspot" onSubmit="return CheckValue(this);">
						<table class="form">
							<tr onMouseOver="showHint('spot_enable');" onMouseOut="hideHint();">
								<td class="title" id="spotSetup" style="width:40%">Hotspot Setup</td>
								<td class="title" style="text-align:right; width: 60%"><select name="spotEnable" id="spotEnable" class="normal" onChange="ModeOnChange(this.form);"></select></td>
							</tr>
							<!-- Coova setup -->
							<tr id="row_chilli" onMouseOver="showHint('spot_profile');" onMouseOut="hideHint();">
								<td class="head" id="spotProfile">Chilli profile</td>
								<td>
									<select name="spotProfile" class="normal" onChange="ProfileOnChange(this.form);"></select>
								</td>
							</tr>
							<tr id="row_sIp" onMouseOver="showHint('spot_ip');" onMouseOut="hideHint();">
								<td class="head" id="sIp">Subnet Address</td>
								<td><input class="normal" class="normal" name="sIp" maxlength="15"></td>
							</tr>
							<tr id="row_sNetmask" onMouseOver="showHint('spot_mask');" onMouseOut="hideHint();">
								<td class="head" id="sNetmask">Subnet Mask</td>
								<td><input class="normal" name="sNetmask" maxlength="15"></td>
							</tr>
							<tr id="row_sStartIP" onMouseOver="showHint('spot_startip');" onMouseOut="hideHint();">
								<td class="head" id="sStartIP">Start IP</td>
								<td><input class="normal" name="sStartIP" maxlength="3"></td>
							</tr>
							<tr id="row_sEndIP" onMouseOver="showHint('spot_endip');" onMouseOut="hideHint();">
								<td class="head" id="sEndIP">End IP</td>
								<td><input class="normal" name="sEndIP" maxlength="3"></td>
							</tr>
							<tr id="row_sC2Cisolate" onMouseOver="showHint('spot_c2cisolate');" onMouseOut="hideHint();">
								<td class="head" id="sC2Cisolate">Use /32 mask for clients</td>
								<td>
									<select name="sC2Cisolate" class="normal">
										<option value="0" id="disable">Disable</option>
										<option value="1" id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_sMaxClients" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
								<td class="head" id="sMaxClients">Maximum Clients</td>
								<td><input class="normal" name="sMaxClients" maxlength="4"></td>
							</tr>
							<tr id="row_sPriDns" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
								<td class="head" id="sPriDns">Primary DNS Server</td>
								<td><input class="normal" name="sPriDns" maxlength="15"></td>
							</tr>
							<tr id="row_sSecDns" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
								<td class="head" id="sSecDns">Secondary DNS Server</td>
								<td><input class="normal" name="sSecDns" maxlength="15"></td>
							</tr>
							<tr id="row_sDropDNS" onMouseOver="showHint('spot_dropdns');" onMouseOut="hideHint();">
								<td class="head" id="sDropDNS">Drop DNS responses</td>
								<td>
									<select name="sDropDNS" class="normal">
										<option value="0" id="disable">Disable</option>
										<option value="1" id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_sDomain" onMouseOver="showHint('spot_domain');" onMouseOut="hideHint();">
								<td class="head" id="sDomain">Domain name</td>
								<td><input class="normal" name="sDomain" maxlength="64"></td>
							</tr>
							<tr id="row_sLease" onMouseOver="showHint('spot_lease');" onMouseOut="hideHint();">
								<td class="head" id="sLease">Lease time (sec)</td>
								<td><input class="normal" name="sLease" maxlength="8"></td>
							</tr>
							<tr id="row_sRadServer1" onMouseOver="showHint('spot_auth');" onMouseOut="hideHint();">
								<td class="head" id="sRadServer1">Radius server auth address</td>
								<td><input class="normal" name="sRadServer1" maxlength="256"></td>
							</tr>
							<tr id="row_sRadServer2" onMouseOver="showHint('spot_acct');" onMouseOut="hideHint();">
								<td class="head" id="sRadServer2">Radius server acct address</td>
								<td><input class="normal" name="sRadServer2" maxlength="256"></td>
							</tr>
							<tr id="row_sRadSecret" onMouseOver="showHint('spot_rad_secret');" onMouseOut="hideHint();">
								<td class="head" id="sRadSecret">Radius server secret</td>
								<td><input class="normal" name="sRadSecret" maxlength="256" type="password"></td>
							</tr>
							<tr id="row_sNasId" onMouseOver="showHint('spot_nasid');" onMouseOut="hideHint();">
								<td class="head" id="sNasId">Radius NAS ID</td>
								<td><input class="normal" name="sNasId" maxlength="256"></td>
							</tr>
							<tr id="row_sRadLocId" onMouseOver="showHint('spot_locid');" onMouseOut="hideHint();">
								<td class="head" id="sRadLocId">Radius Location ID</td>
								<td><input class="normal" name="sRadLocId" maxlength="15"></td>
							</tr>
							<tr id="row_sRadLocName" onMouseOver="showHint('spot_locname');" onMouseOut="hideHint();">
								<td class="head" id="sRadLocName">Radius Location Name</td>
								<td><input class="normal" name="sRadLocName" maxlength="15"></td>
							</tr>
							<tr id="row_sRadCoaPort" onMouseOver="showHint('spot_port');" onMouseOut="hideHint();">
								<td class="head" id="sRadCoaPort">Radius CoA/DM port</td>
								<td><input class="normal" name="sRadCoaPort" maxlength="15"></td>
							</tr>
							<tr id="row_sRadCoaNoIpCheck" onMouseOver="showHint('spot_ipcheck');" onMouseOut="hideHint();">
								<td class="head" id="sRadCoaNoIpCheck">No IP check for CoA/DM request</td>
								<td>
									<select name="sRadCoaNoIpCheck" class="normal">
										<option value="off" id="disable">Disable</option>
										<option value="on" id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_sUamServer" onMouseOver="showHint('spot_uamserver');" onMouseOut="hideHint();">
								<td class="head" id="sUamServer">UAM server URL</td>
								<td><input class="normal" name="sUamServer" maxlength="256"></td>
							</tr>
							<tr id="row_sUamHomepage" onMouseOver="showHint('spot_uamhomepage');" onMouseOut="hideHint();">
								<td class="head" id="sUamHomepage">UAM homepage URL</td>
								<td><input class="normal" name="sUamHomepage" maxlength="256"></td>
							</tr>
							<tr id="row_sUamSecret" onMouseOver="showHint('spot_uamsecret');" onMouseOut="hideHint();">
								<td class="head" id="sUamSecret">UAM secret</td>
								<td><input class="normal" name="sUamSecret" maxlength="256" type="password"></td>
							</tr>
							<tr id="row_sUamAllowed" onMouseOver="showHint('spot_uamallowed');" onMouseOut="hideHint();">
								<td class="head" id="sUamAllowed">UAM allowed hosts</td>
								<td><input class="normal" name="sUamAllowed" maxlength="1024"></td>
							</tr>
							<tr id="row_sUamDomain" onMouseOver="showHint('spot_uamdomain');" onMouseOut="hideHint();">
								<td class="head" id="sUamDomain">UAM allowed domains</td>
								<td><input class="normal" name="sUamDomain" maxlength="1024"></td>
							</tr>
							<tr id="row_sUamAnyDNS" onMouseOver="showHint('spot_uamdns');" onMouseOut="hideHint();">
								<td class="head" id="sUamAnyDNS">Allow all DNS requests</td>
								<td>
									<select name="sUamAnyDNS" class="normal">
										<option value="off" id="disable">Disable</option>
										<option value="on" id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_sMacAllowed" onMouseOver="showHint('spot_macallowed');" onMouseOut="hideHint();">
								<td class="head" id="sMacAllowed">Allowed MAC</td>
								<td><input class="normal" name="sMacAllowed" maxlength="512"></td>
							</tr>
							<!-- Nodog setup -->
							<tr id="row_GatewayIPRange" onMouseOver="showHint('GatewayIPRange');" onMouseOut="hideHint();">
								<td class="head" id="GatewayIPRange">Range of IP addresses</td>
								<td><input class="normal" name="GatewayIPRange" maxlength="128"></td>
							</tr>
							<tr id="row_RedirectURL" onMouseOver="showHint('RedirectURL');" onMouseOut="hideHint();">
								<td class="head" id="RedirectURL">Redirected to URL after authentication</td>
								<td><input class="normal" name="RedirectURL" maxlength="128"></td>
							</tr>
							<tr id="row_MaxClients" onMouseOver="showHint('MaxClients');" onMouseOut="hideHint();">
								<td class="head" id="MaxClients">Maximum number of users</td>
								<td><input class="normal" name="MaxClients" maxlength="128"></td>
							</tr>
							<tr id="row_ClientIdleTimeout" onMouseOver="showHint('ClientIdleTimeout');" onMouseOut="hideHint();">
								<td class="head" id="ClientIdleTimeout">Inactivity before a user is 'deauthenticated'</td>
								<td><input class="normal" name="ClientIdleTimeout" maxlength="128"></td>
							</tr>
							<tr id="row_ClientForceTimeout" onMouseOver="showHint('ClientForceTimeout');" onMouseOut="hideHint();">
								<td class="head" id="ClientForceTimeout">Minutes before a user is 'deauthenticated'</td>
								<td><input class="normal" name="ClientForceTimeout" maxlength="128"></td>
							</tr>
							<tr id="row_AuthenticateImmediately" onMouseOver="showHint('AuthenticateImmediately');" onMouseOut="hideHint();">
								<td class="head" id="AuthenticateImmediately">Authenticate Immediately</td>
								<td>
									<select name="AuthenticateImmediately" class="normal">
										<option value=0 id="disable">Disable</option>
										<option value=1 id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_MACMechanism" onMouseOver="showHint('MACMechanism');" onMouseOut="hideHint();">
								<td class="head" id="MACMechanism">MAC Mechanism</td>
								<td>
									<select name="MACMechanism" class="normal" onChange="MACMechanismOnChange(this.form);">
										<option value="block" id="block">Block</option>
										<option value="allow" id="allow">Allow</option>
									</select>
								</td>
							</tr>
							<tr id="row_BlockedMACList" onMouseOver="showHint('BlockedMACList');" onMouseOut="hideHint();">
								<td class="head" id="BlockedMACList">List Blocked MAC Addresses</td>
								<td><input class="normal" name="BlockedMACList" maxlength="128"></td>
							</tr>
							<tr id="row_AllowedMACList" onMouseOver="showHint('AllowedMACList');" onMouseOut="hideHint();">
								<td class="head" id="AllowedMACList">List Allowed MAC Addresses</td>
								<td><input class="normal" name="AllowedMACList" maxlength="128"></td>
							</tr>
							<tr id="row_TrustedMACList" onMouseOver="showHint('TrustedMACList');" onMouseOut="hideHint();">
								<td class="head" id="TrustedMACList">List Trusted MAC Addresses</td>
								<td><input class="normal" name="TrustedMACList" maxlength="128"></td>
							</tr>
							<tr id="row_PasswordAuthentication" onMouseOver="showHint('PasswordAuthentication');" onMouseOut="hideHint();">
								<td class="head" id="PasswordAuthentication">Password Authentication</td>
								<td>
									<select name="PasswordAuthentication" class="normal" onChange="MACMechanismOnChange(this.form);">
										<option value=0 id="disable">Disable</option>
										<option value=1 id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_Password" onMouseOver="showHint('Password');" onMouseOut="hideHint();">
								<td class="head" id="Password">Password</td>
								<td><input class="normal" type="password" name="Password" maxlength="128"></td>
							</tr>
							<tr id="row_UsernameAuthentication" onMouseOver="showHint('UsernameAuthentication');" onMouseOut="hideHint();">
								<td class="head" id="UsernameAuthentication">Username Authentication</td>
								<td>
									<select name="UsernameAuthentication" class="normal" onChange="MACMechanismOnChange(this.form);">
										<option value=0 id="disable">Disable</option>
										<option value=1 id="enable">Enable</option>
									</select>
								</td>
							</tr>
							<tr id="row_Username" onMouseOver="showHint('Username');" onMouseOut="hideHint();">
								<td class="head" id="Username">Username</td>
								<td><input class="normal" name="Username" maxlength="128"></td>
							</tr>
							<tr id="row_PasswordAttempts" onMouseOver="showHint('PasswordAttempts');" onMouseOut="hideHint();">
								<td class="head" id="PasswordAttempts">Password Attempts</td>
								<td><input class="normal" name="PasswordAttempts" maxlength="128"></td>
							</tr>
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
				</td>
			</tr>
		</table>
	</body>
</html>
