<!DOCTYPE html>
<html>
	<head>
		<title>Local Area Network (LAN) Settings</title>
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
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("buttons");
			Butterlate.setTextDomain("services");

			function initTranslation() {
				_TR("lTitle",			"lan title");
				_TR("lIntroduction",	"lan introduction");
				_TR("lSetup",			"lan setup");
				_TR("lHostname",		"inet hostname");
				_TR("lGateway",			"inet gateway");
				_TR("lPriDns",			"inet pri dns");
				_TR("lSecDns",			"inet sec dns");
				_TR("lIp",				"inet ip");
				_TR("lNetmask",			"inet netmask");
				_TR("lEnable",			"button enable");
				_TR("lDisable",			"button disable");
				_TR("wStaticDnsProfile",	"inet dns profile");
				_TR("dnsProfileManual",		"inet dns profile manual");
				_TR("dnsProfileGoogle",		"inet dns profile google");
				_TR("dnsProfileYandex",		"inet dns profile yandex");
				_TR("dnsProfileSky",		"inet dns profile sky");
				_TR("dnsProfileOpen",		"inet dns profile open");
				_TR("dnsProfileAdguard",		"inet dns profile adguard");
				_TR("wStaticDnsYandexProfile",	"inet dns profile yandex title");
				_TR("dnsProfileYandexBasic",	"inet dns profile yandex basic");
				_TR("dnsProfileYandexSafe",	"inet dns profile yandex safe");
				_TR("dnsProfileYandexFamily",	"inet dns profile yandex family");
				_TR("wStaticDnsAdguardProfile",	"inet dns profile adguard title");
				_TR("dnsProfileAdguardDefault",	"inet dns profile adguard default");
				_TR("dnsProfileAdguardFamily",	"inet dns profile adguard family");
				_TRV("lApply",			"button apply");
				_TRV("lCancel",			"button cancel");
				_TRV("lReset",			"button reset");
			}

			function initValues() {
				var form = document.lanCfg;
			
				form.hostname.value					= NVRAM_HostName;
				form.lanIp.value					= LAN_IP;
				form.lanNetmask.value				= LAN_NETMASK;
				form.lanGateway.value				= NVRAM_wan_gateway;
				form.lanPriDns.value				= PRIMARY_DNS;
				form.lanSecDns.value				= SECONDARY_DNS;
				form.dhcpStart.value				= NVRAM_dhcpStart;
				form.dhcpEnd.value					= NVRAM_dhcpEnd;
				form.dhcpGateway.value				= NVRAM_dhcpGateway;
				form.wStaticDnsProfile.value		= NVRAM_wan_static_dns_profile;
				form.wStaticDnsYandexProfile.value	= NVRAM_wan_static_dns_profile_yandex;
				form.wStaticDnsAdguardProfile.value	= NVRAM_wan_static_dns_profile_adguard; 

				displayElement( [ 'brGateway', 'brPriDns', 'brSecDns', 'staticDNSprofile', 'staticDNSyandexProfile', 'staticDNSadguardProfile' ], NVRAM_OperationMode == '0' );

				dnsSwitchClick(form);
				showWarning();
				initTranslation();
			}

			function checkValues(form) {
				var dhcp_edit = 0;

				if ((form.hostname.value == "") || (form.hostname.value.match(/[^A-Z0-9_-]/gi))) {
					alert(_("inet hostname wrong"));
					form.hostname.focus();
					return false;
				}
				if (!validateIP(form.lanIp, true)) {
					form.lanIp.focus();
					return false;
				}
				if (!validateIPMask(form.lanNetmask, true)) {
					form.lanNetmask.focus();
					return false;
				}
				if (NVRAM_OperationMode != '0' && form.lanIp.value == NVRAM_wan_ipaddr) {
					alert(_("inet lan wan same"));
					form.lanIp.focus();
					return false;
				}
				if (NVRAM_OperationMode == "0") {
					if (!validateIP(form.lanGateway, true)) {
						form.lanGateway.focus();
						return false;
					}
					if (form.lanGateway.value == form.lanIp.value) {
						alert(_("inet lan gw same"));
						form.lanGateway.focus();
						return false;
					}
					
					if (!validateIP(form.lanPriDns, true)) {
						form.lanPriDns.focus();
						return false;
					}
					if (form.lanSecDns.value.length > 0)
						if (!validateIP(form.lanSecDns, true)) {
							form.lanSecDns.focus();
							return false;
						}
				}

				if ((form.lanIp.value != LAN_IP || form.lanNetmask.value != LAN_NETMASK && NVRAM_dhcpEnabled == '1') && NVRAM_OperationMode != '0') {
					if (confirm(_("lan accept dhcp opts"))) {
						var ip = form.lanIp.value.split(".");
						var mask = form.lanNetmask.value.split(".");
						var ip_start = [];
						var ip_end = [];
						for (var i = 0; i < 4; i++) {
							ip_start[i] = ip[i] & mask[i];
							ip_end[i] = ip[i] | (~ mask[i] & 0xff);
						}
						ip_start[3] = ((ip_end[3] - ip[3]) > ip[3]) ? ip[3] : ip_start[3];
						ip_end[3] = ((ip_end[3] - ip[3]) > ip[3]) ? ip_end[3] : ip[3];
						form.dhcpStart.value = ip_start[0] + "." + ip_start[1] + "." + ip_start[2] + "." + (ip_start[3] * 1 + 1);
						form.dhcpEnd.value = ip_end[0] + "." + ip_end[1] + "." + ip_end[2] + "." + (ip_end[3] - 1);
						if (form.dhcpGateway.value == LAN_IP)
							form.dhcpGateway.value = form.lanIp.value;
					}
				}

				if (form.lanIp.value != LAN_IP)
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 30, 'http://' + form.lanIp.value);
				else
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);
				return false;
			}

			function dnsSwitchClick(form) {
				displayElement(['brPriDns', 'brSecDns'],	NVRAM_OperationMode == '0' && form.wStaticDnsProfile.value == 'manual');
				displayElement('staticDNSyandexProfile',	NVRAM_OperationMode == '0' && form.wStaticDnsProfile.value == 'yandex');
				displayElement('staticDNSadguardProfile',	NVRAM_OperationMode == '0' && form.wStaticDnsProfile.value == 'adguard');
				document.getElementById('wStaticDnsProfile_learne').innerHTML = '';
				switch (form.wStaticDnsProfile.value) {
					case 'google':
										form.lanPriDns.value = '8.8.8.8';
										form.lanSecDns.value = '8.8.4.4';
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://developers.google.com/speed/public-dns/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'yandex':		switch (form.wStaticDnsYandexProfile.value) {
											case 'basic':
															form.lanPriDns.value = '77.88.8.8';
															form.lanSecDns.value = '77.88.8.1';
															break;
											case 'safe':
															form.lanPriDns.value = '77.88.8.88';
															form.lanSecDns.value = '77.88.8.2';
															break;
											case 'family':
															form.lanPriDns.value = '77.88.8.7';
															form.lanSecDns.value = '77.88.8.3';
															break;
										}
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://dns.yandex.ru/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'sky':
										form.lanPriDns.value = '193.58.251.251';
										form.lanSecDns.value = '';
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://www.skydns.ru/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'open':
										form.lanPriDns.value = '208.67.222.123';
										form.lanSecDns.value = '208.67.220.123';
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://www.opendns.com/home-internet-security/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'adguard':		switch (form.wStaticDnsAdguardProfile.value) {
											case 'default':
															form.lanPriDns.value = '176.103.130.130';
															form.lanSecDns.value = '176.103.130.131';
															break;
											case 'family':
															form.lanPriDns.value = '176.103.130.132';
															form.lanSecDns.value = '176.103.130.134';
															break;															
										}
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://adguard.com/adguard-dns/overview.html" target="_blank">' + _('services status about') + '</a>';
										break;
				}
			}

			function resetValues(form, time) {
				if (confirm(_('message reset confirm'))) {
					form.reset.value = "1";
					ajaxShowTimer(form, 'timerReloader', _('message apply'), time, 'http://192.168.1.1');
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
			<td><h1 id="lTitle"></h1>
				<p id="lIntroduction"></p>
				<hr>
				<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
				<form method="POST" name="lanCfg" action="/goform/setLan">
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="lSetup">LAN Interface Setup</td>
						</tr>
						<tr>
							<td class="head" id="lHostname" style="width: 40%">Hostname</td>
							<td style="width: 60%"><input name="hostname" class="mid"></td>
						</tr>
						<tr>
							<td class="head" id="lIp" style="width: 40%">IP Address</td>
							<td style="width: 60%"><input name="lanIp" class="mid" maxlength="15"></td>
						</tr>
						<tr>
							<td class="head" id="lNetmask" style="width: 40%">Subnet Mask</td>
							<td style="width: 60%"><input name="lanNetmask" class="mid" maxlength="15"></td>
						</tr>
						<tr id="brGateway">
							<td class="head" id="lGateway" style="width: 40%">Default Gateway</td>
							<td style="width: 60%"><input name="lanGateway" class="mid" maxlength="15"></td>
						</tr>
						<tr id="staticDNSprofile">
							<td class="head" id="wStaticDnsProfile">DNS Profile</td>
							<td><div style="float: left">
								<select name="wStaticDnsProfile" onChange="dnsSwitchClick(this.form);" class="mid">
									<option id="dnsProfileManual" value="manual">Manual</option>
									<option id="dnsProfileGoogle" value="google">Google DNS</option>
									<option id="dnsProfileYandex" value="yandex">Yandex DNS</option>
									<option id="dnsProfileSky" value="sky">Sky DNS</option>
									<option id="dnsProfileOpen" value="open">Open DNS</option>
									<option id="dnsProfileAdguard" value="adguard">AdGuard DNS</option>
								</select>
								</div>
								<div id="wStaticDnsProfile_learne" style="float: left; margin: 3px 10px"></div>
							</td>
						</tr>
						<tr id="staticDNSyandexProfile">
							<td class="head" id="wStaticDnsYandexProfile">Yandex Profile</td>
							<td>
								<select name="wStaticDnsYandexProfile" onChange="dnsSwitchClick(this.form);" class="mid">
									<option id="dnsProfileYandexBasic" value="basic">Basic</option>
									<option id="dnsProfileYandexSafe" value="safe">Safe</option>
									<option id="dnsProfileYandexFamily" value="family">Family</option>
								</select>
							</td>
						</tr>
						<tr id="staticDNSadguardProfile">
							<td class="head" id="wStaticDnsAdguardProfile">AdGuard Profile</td>
							<td>
								<select name="wStaticDnsAdguardProfile" onChange="dnsSwitchClick(this.form);" class="mid">
									<option id="dnsProfileAdguardDefault" value="default">Default</option>
									<option id="dnsProfileAdguardFamily" value="family">Family</option>
								</select>
							</td>
						</tr>
						<tr id="brPriDns">
							<td class="head" id="lPriDns" style="width: 40%">Primary DNS Server</td>
							<td style="width: 60%"><input name="lanPriDns" class="mid" maxlength="15"></td>
						</tr>
						<tr id="brSecDns">
							<td class="head" id="lSecDns" style="width: 40%">Secondary DNS Server</td>
							<td style="width: 60%"><input name="lanSecDns" class="mid" maxlength="15"></td>
						</tr>
					</table>
					<table class="buttons">
						<tr>
							<td><input type="submit" class="normal" value="Apply"  id="lApply"  onClick="return checkValues(this.form);">&nbsp;&nbsp;
								<input type="button" class="normal" value="Cancel" id="lCancel" onClick="window.location.reload();">&nbsp;&nbsp;
								<input type="button" class="normal" value="Reset"  id="lReset"  onClick="resetValues(this.form, 30);">
								<input type="hidden" name="reset" value="0">
								<input name="dhcpStart" type="hidden">
								<input name="dhcpEnd" type="hidden">
								<input name="dhcpGateway" type="hidden">
							</td>
						</tr>
					</table>
				</form>
				<div class="whitespace">&nbsp;</div>
			</tr>
		</table>
	</body>
</html>
