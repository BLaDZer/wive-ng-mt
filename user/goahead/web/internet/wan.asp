<!DOCTYPE html>
<html>
	<head>
		<title>Wide Area Network (WAN) Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			function initTranslation() {
				_TR("wTitle",					"wan title");
				_TR("wIntroduction",			"wan introduction");
				_TR("wConnectionType",			"wan connection type");
				_TR("wConnection",				"wan connection");
				_TR("wConnTypeStatic",			"wan connection type static");
				_TR("wConnTypeDhcp",			"wan connection type dhcp");
				_TR("wConnTypeZero",			"wan connection type zero");
				_TR("wStaticMode",				"wan static mode");
				_TR("wStaticDns",				"wan static dns");
				_TR("wStaticIp",				"inet ip");
				_TR("wStaticNetmask",			"inet netmask");
				_TR("wStaticGateway",			"inet gateway");
				_TR("wStaticDnsProfile",		"inet dns profile");
				_TR("dnsProfileManual",			"inet dns profile manual");
				_TR("dnsProfileGoogle",			"inet dns profile google");
				_TR("dnsProfileYandex",			"inet dns profile yandex");
				_TR("dnsProfileSky",			"inet dns profile sky");
				_TR("dnsProfileOpen",			"inet dns profile open");
				_TR("dnsProfileAdguard",		"inet dns profile adguard");
				_TR("wStaticDnsYandexProfile",	"inet dns profile yandex title");
				_TR("dnsProfileYandexBasic",	"inet dns profile yandex basic");
				_TR("dnsProfileYandexSafe",		"inet dns profile yandex safe");
				_TR("dnsProfileYandexFamily",	"inet dns profile yandex family");
				_TR("wStaticDnsAdguardProfile",	"inet dns profile adguard title");
				_TR("dnsProfileAdguardDefault",	"inet dns profile adguard default");
				_TR("dnsProfileAdguardFamily",	"inet dns profile adguard family");
				_TR("wStaticPriDns",			"inet pri dns");
				_TR("wStaticSecDns",			"inet sec dns");
				_TR("wDhcpMode",				"wan dhcp mode");
				_TR("wAdditionalOptions",		"wan additional options");
				_TR("wReqFromDHCP",				"wan request from dhcp");
				_TR("wDHCPVendorClass",			"wan dhcp vendor class");
				_TR("wMTU",						"wan mtu");
				_TR("wAuto",					"inet auto");
				_TR("wCustom",					"inet custom");
				_TR("wNatEnabled",				"wan nat enabled");
				_TR("wMacAddress",				"inet mac");
				_TR("wMacAddr",					"wan mac");
				_TR("wApply",					"button apply");
				_TR("wCancel",					"button cancel");
				_TR("wReset",					"button reset");
				_TR("WanMacRestore",			"button restore factory");
			}

			function initValues() {
				var form							= document.wanCfg;
				form.staticIp.value					= NVRAM_wan_ipaddr;
				form.staticNetmask.value			= NVRAM_wan_netmask;
				form.staticGateway.value			= NVRAM_wan_gateway;
				form.dhcpReqIP.value				= NVRAM_dhcpRequestIP;
				form.dhcpVendorClass.value			= NVRAM_dhcpVendorClass;
				form.wStaticDnsProfile.value		= NVRAM_wan_static_dns_profile;
				form.wStaticDnsYandexProfile.value	= NVRAM_wan_static_dns_profile_yandex; 
				form.wStaticDnsAdguardProfile.value	= NVRAM_wan_static_dns_profile_adguard; 
				form.staticPriDns.value				= PRIMARY_DNS;
				form.staticSecDns.value				= SECONDARY_DNS;
				form.wanMac.value					= NVRAM_WAN_MAC_ADDR;

				displayElement(document.getElementById("natRowDisplay"), NVRAM_OperationMode != "0");
				form.natEnabled.checked 			= (NVRAM_natEnabled == '1');
				form.connectionType.value 			= NVRAM_wanConnectionMode;

				form.wan_mtu.value = NVRAM_wan_manual_mtu;
				for (var i = 0; i < form.wan_mtu_type.options.length; i++)
					if (form.wan_mtu_type.options[i].value == NVRAM_wan_manual_mtu) {
						form.wan_mtu_type.value = form.wan_mtu_type.options[i].value;
						break;
					}

				connectionTypeSwitch(form);
				dnsSwitchClick(form);
				wanMtuChange(form);
				showWarning();
				initTranslation();
			}

			function CheckValues(form) {
				var c_type	= form.connectionType.value;
				var wan_mtu	= +form.wan_mtu.value;

				if (c_type == "STATIC") {
					form.wStaticDnsEnable.disabled = false;
					if (!validateIP(form.staticIp, true)) {
						form.staticIp.focus();
						return false;
					}
					if (!validateIPMask(form.staticNetmask, true)) {
						form.staticNetmask.focus();
						return false;
					}
					if (form.staticGateway.value != "") {
						if (!validateIP(form.staticGateway, true)) {
							form.staticGateway.focus();
							return false;
						}
						if (form.staticGateway.value == form.staticIp.value) {
							alert(_("inet wan gw same"));
							form.staticGateway.focus();
							return false;
						}
					}
					if ((NVRAM_OperationMode != "0") && (form.staticIp.value == NVRAM_lan_ipaddr)) {
							alert(_("wan warning same lan"));
							form.staticIp.focus();
							return false;
					}
				}
				else if (c_type == "DHCP") {
					if (form.dhcpReqIP.value != "") {
						if (!validateIP(form.dhcpReqIP, true)) {
							form.dhcpReqIP.focus();
							return false;
						}
					}
					else if (c_type == "ZERO")
						form.wStaticDnsEnable.checked = true;
				}
				if (form.wStaticDnsEnable.checked) {
					if (!validateIP(form.staticPriDns, true)) {
						form.staticPriDns.focus();
						return false;
					}
					if (form.staticSecDns.value != '')
						if (!validateIP(form.staticSecDns, true)) {
								form.staticSecDns.focus();
								return false;
						}
				}
				if (!validateNum(form.wan_mtu.value)) {
					alert(_("inet invalid mtu"));
					form.wan_mtu_type.focus();
					return false;
				}
				if (!validateMAC(form.wanMac.value)) {
					alert(_("inet invalid mac"));
					form.wanMac.focus();
					return false;
				}
				if ((wan_mtu < 80) && (wan_mtu != 0)) {
					alert(_("inet invalid mtu"));
					return false;
				}
				form.wan_mtu.value = +form.wan_mtu.value + "";
				if (form.wanMac.value != NVRAM_WAN_MAC_ADDR)
					if (!confirm(_('wan reboot confirm')))
						ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);
					else {
						form.reboot.value = "1";
						ajaxPostForm(null, form, 'timerReloader', _("message config"), ajaxShowProgress)
					}
				else
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);

				return true;
			}

			function connectionTypeSwitch(form) {
				var conn_type = form.connectionType.value;
				displayElement('staticDHCP', conn_type == 'STATIC');
				displayElement(['dhcpReqIPRow', 'dhcpVendorRow'], conn_type == 'DHCP');
				displayElement('staticDNSAssignRow', conn_type != 'ZERO');

				if (conn_type == 'STATIC') {
					form.wStaticDnsEnable.checked = true;
					form.wStaticDnsEnable.disabled = true;
				}
				else if (conn_type == 'ZERO') {
					form.wStaticDnsEnable.checked = true;
				}
				else {
					form.wStaticDnsEnable.disabled = false;
					form.wStaticDnsEnable.checked = NVRAM_wan_static_dns == 'on';
				}

				dnsSwitchClick(form);
			}

			function wanMtuChange(form) {
				if (form.wan_mtu_type.value == '1') {
					form.wan_mtu.style.display = '';
					form.wan_mtu.setAttribute('class', 'half');
					form.wan_mtu_type.setAttribute('class', 'half');
				}
				else {
					form.wan_mtu_type.setAttribute('class', 'mid');
					form.wan_mtu.style.display = 'none';
					form.wan_mtu.value = form.wan_mtu_type.value;
				}
			}
			
			function dnsSwitchClick(form) {
				displayElement('staticDNSprofile',			form.wStaticDnsEnable.checked);
				displayElement(['priDNSrow', 'secDNSrow'],	form.wStaticDnsEnable.checked && form.wStaticDnsProfile.value == 'manual');
				displayElement('staticDNSyandexProfile',	form.wStaticDnsEnable.checked && form.wStaticDnsProfile.value == 'yandex');
				displayElement('staticDNSadguardProfile',	form.wStaticDnsEnable.checked && form.wStaticDnsProfile.value == 'adguard');
				document.getElementById('wStaticDnsProfile_learne').innerHTML = '';
				switch (form.wStaticDnsProfile.value) {
					case 'google':
										form.staticPriDns.value = '8.8.8.8';
										form.staticSecDns.value = '8.8.4.4';
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://developers.google.com/speed/public-dns/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'yandex':		switch (form.wStaticDnsYandexProfile.value) {
											case 'basic':
															form.staticPriDns.value = '77.88.8.8';
															form.staticSecDns.value = '77.88.8.1';
															break;
											case 'safe':
															form.staticPriDns.value = '77.88.8.88';
															form.staticSecDns.value = '77.88.8.2';
															break;															
											case 'family':
															form.staticPriDns.value = '77.88.8.7';
															form.staticSecDns.value = '77.88.8.3';
															break;															
										}
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://dns.yandex.ru/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'sky':
										form.staticPriDns.value = '193.58.251.251';
										form.staticSecDns.value = '';
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://www.skydns.ru/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'open':
										form.staticPriDns.value = '208.67.222.123';
										form.staticSecDns.value = '208.67.220.123';
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://www.opendns.com/home-internet-security/" target="_blank">' + _('services status about') + '</a>';
										break;

					case 'adguard':		switch (form.wStaticDnsAdguardProfile.value) {
											case 'default':
															form.staticPriDns.value = '176.103.130.130';
															form.staticSecDns.value = '176.103.130.131';
															break;
											case 'family':
															form.staticPriDns.value = '176.103.130.132';
															form.staticSecDns.value = '176.103.130.134';
															break;															
										}
										document.getElementById('wStaticDnsProfile_learne').innerHTML = '<a href="https://adguard.com/adguard-dns/overview.html" target="_blank">' + _('services status about') + '</a>';
										break;
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<form name="rebootForm" style="display: none;" method="GET" action="/goform/restoremac" >
			<iframe id="rebootReloader" name="rebootReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
		</form>
		<table class="body">
			<tr>
				<td><h1 id="wTitle"></h1>
					<p id="wIntroduction"></p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="wanCfg" action="/goform/setWan">
					<table class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" id="wConnection">WAN connection type</td>
							</tr>
							<tr>
								<td class="head" id="wConnectionType">Connection type</td>
								<td><select name="connectionType" class="mid" onChange="connectionTypeSwitch(this.form);">
									<option id="wConnTypeStatic" value="STATIC" selected="selected">Static Mode (fixed IP)</option>
									<option id="wConnTypeDhcp"   value="DHCP">DHCP (Auto Config)</option>
									<option id="wConnTypeZero"   value="ZERO">Zeroconf</option>
								</select></td>
							</tr>
						</tbody>
					</table>
					<!-- ================= STATIC Mode ================= -->
					<table id="staticDHCP" class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" id="wStaticMode">Static Mode</td>
							</tr>
							<tr>
								<td class="head" id="wStaticIp">IP Address</td>
								<td><input name="staticIp" class="mid"></td>
							</tr>
							<tr>
								<td class="head" id="wStaticNetmask">Subnet Mask</td>
								<td><input name="staticNetmask" class="mid"></td>
							</tr>
							<tr>
								<td class="head" id="wStaticGateway">Default Gateway</td>
								<td><input name="staticGateway" class="mid"></td>
							</tr>
						</tbody>
					</table>
					<table class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" id="wAdditionalOptions">Additional Options</td>
							</tr>
							<tr id="dhcpReqIPRow">
								<td class="head" id="wReqFromDHCP">Request IP from DHCP (optional)</td>
								<td><input name="dhcpReqIP" class="mid"></td>
							</tr>
							<tr id="dhcpVendorRow">
								<td class="head" id="wDHCPVendorClass">Vendor class identifier (optional)</td>
								<td><input name="dhcpVendorClass" class="mid"></td>
							</tr>
							<tr>
								<td class="head" id="wMTU">WAN MTU</td>
								<td><input name="wan_mtu" type="text" class="half" style="display:none;">
									<select name="wan_mtu_type" onChange="wanMtuChange(this.form);" class="mid">
										<option value="0" id="wAuto">AUTO</option>
										<option value="1" selected="selected" id="wCustom">Custom</option>
										<option value="1500">1500</option>
										<option value="1492">1492</option>
										<option value="1460">1460</option>
										<option value="1440">1440</option>
										<option value="1400">1400</option>
										<option value="1300">1300</option>
										<option value="1200">1200</option>
										<option value="1000">1000</option>
									</select>
								</td>
							</tr>
							<tr id="staticDNSAssignRow">
								<td class="head" id="wStaticDns">Assign static DNS Server</td>
								<td><input name="wStaticDnsEnable" type="checkbox" onClick="dnsSwitchClick(this.form);" ></td>
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
							<tr id="priDNSrow">
								<td class="head" id="wStaticPriDns">Primary DNS Server</td>
								<td><input name="staticPriDns" class="mid"></td>
							</tr>
							<tr id="secDNSrow">
								<td class="head" id="wStaticSecDns">Secondary DNS Server</td>
								<td><input name="staticSecDns" class="mid"></td>
							</tr>
							<tr id="natRowDisplay">
								<td class="head" id="wNatEnabled">Enable NAT</td>
								<td><input name="natEnabled" type="checkbox"></td>
							</tr>
						</tbody>
					</table>
					<table class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" id="wMacAddress">MAC address</td>
							</tr>
							<tr id="MACrow">
								<td class="head" id="wMacAddr">WAN MAC address</td>
								<td>
									<input name="wanMac" id="wanMac" class="mid">
									<input type="button" value="Restore Factory" id="WanMacRestore" name="restoremac" onClick="this.form.reboot.value = '1'; ajaxPostForm(_('wan reboot confirm'), document.rebootForm, 'rebootReloader', _('message config'), ajaxShowProgress);">
								</td>
							</tr>
						</tbody>
					</table>
					<table class="buttons">
						<tr>
							<td>
								<input type="submit" class="normal" value="Apply" id="wApply" onClick="return CheckValues(this.form);"><input type="button" class="normal" value="Cancel" id="wCancel" onClick="window.location.reload();"><input type="button" class="normal" value="Reset" id="wReset" onClick="resetValues(this.form, 30);">
								<input type="hidden" value="0" name="reboot">
								<input type="hidden" value="0" name="reset">
							</td>
						</tr>
					</table>
					<div class="whitespace"></div>
				</form>
			</tr>
		</table>
	</body>
</html>
