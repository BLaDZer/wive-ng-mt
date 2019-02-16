<!DOCTYPE html>
<html>
	<head>
		<title>Wide Area Network (WAN) Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
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
			function initValues() {
				var form					= document.wanCfg;
				form.staticIp.value				= NVRAM_wan_ipaddr;
				form.staticNetmask.value			= NVRAM_wan_netmask;
				form.staticGateway.value			= NVRAM_wan_gateway;
				form.dhcpReqIP.value				= NVRAM_dhcpRequestIP;
				form.dhcpVendorClass.value			= NVRAM_dhcpVendorClass;
				form.wStaticDnsProfile.value			= NVRAM_wan_static_dns_profile;
				form.wStaticDnsYandexProfile.value		= NVRAM_wan_static_dns_profile_yandex; 
				form.wStaticDnsAdguardProfile.value		= NVRAM_wan_static_dns_profile_adguard; 
				form.staticPriDns.value				= NVRAM_wan_primary_dns;
				form.staticSecDns.value				= NVRAM_wan_secondary_dns;
				form.wanMac.value				= NVRAM_WAN_MAC_ADDR;

				if (NVRAM_wan_static_dns != "on") {
					form.wStaticDnsProfile.value = (NVRAM_wanConnectionMode == "DHCP")?"auto":"manual";
				}

				displayElement(document.getElementById("natRowDisplay"), NVRAM_OperationMode != "0");
				form.natEnabled.value				= (NVRAM_natEnabled == '1')?"on":"off";
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
				init_translation_model();
				disableControlsByAuth();
			}

			function CheckValues(form) {
				var c_type	= form.connectionType.value;
				var wan_mtu	= +form.wan_mtu.value;

				if (c_type == "STATIC") {
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
				}
				if (form.wStaticDnsProfile.value != "auto") {
					if (!validateIP(form.staticPriDns, true)) {
						form.staticPriDns.focus();
						return false;
					}
					if (form.staticPriDns.value == "0.0.0.0") {
						alert(_("inet invalid ip"));
						form.staticPriDns.focus();
						return false;
					}

					if (form.staticSecDns.value != '') {
						if (!validateIP(form.staticSecDns, true)) {
								form.staticSecDns.focus();
								return false;
						}

						if (form.staticSecDns.value == "0.0.0.0") {
							alert(_("inet invalid ip"));
							form.staticSecDns.focus();
							return false;
						}
					}
				}
				if (!validateNum(form.wan_mtu.value)) {
					alert(_("inet invalid mtu"));
					form.wan_mtu_type.focus();
					return false;
				}
				if (!validateUnicastMAC(form.wanMac.value)) {
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
//						ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);
						return false;
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
				displayElement(['dns_profile_auto', 'dhcpReqIPRow', 'dhcpVendorRow'], conn_type == 'DHCP');

				if ((conn_type == 'STATIC' || conn_type == 'ZERO') && form.wStaticDnsProfile.value == "auto") {
					form.wStaticDnsProfile.value = "manual";
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
				displayElement(['priDNSrow', 'secDNSrow'],	form.wStaticDnsProfile.value == 'manual');
				displayElement('staticDNSyandexProfile',	form.wStaticDnsProfile.value == 'yandex');
				displayElement('staticDNSadguardProfile',	form.wStaticDnsProfile.value == 'adguard');
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
				<td><h1 data-tr="wan title"></h1>
					<p data-tr="wan introduction"></p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="wanCfg" action="/goform/setWan">
					<table class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" data-tr="wan connection">WAN connection type</td>
							</tr>
							<tr>
								<td class="head" data-tr="wan connection type">Connection type</td>
								<td><select name="connectionType" class="mid auth-disable-user" onChange="connectionTypeSwitch(this.form);" >
									<option data-tr="wan connection type static" value="STATIC" selected="selected">Static Mode (fixed IP)</option>
									<option data-tr="wan connection type dhcp"   value="DHCP">DHCP (Auto Config)</option>
									<option data-tr="wan connection type zero"   value="ZERO">Zeroconf</option>
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
								<td class="title" colspan="2" data-tr="wan static mode">Static Mode</td>
							</tr>
							<tr>
								<td class="head" data-tr="inet ip">IP Address</td>
								<td><input name="staticIp" class="mid auth-disable-user"></td>
							</tr>
							<tr>
								<td class="head" data-tr="inet netmask">Subnet Mask</td>
								<td><input name="staticNetmask" class="mid auth-disable-user"></td>
							</tr>
							<tr>
								<td class="head" data-tr="inet gateway">Default Gateway</td>
								<td><input name="staticGateway" class="mid auth-disable-user"></td>
							</tr>
						</tbody>
					</table>
					<table class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" data-tr="wan additional options">Additional Options</td>
							</tr>

							<tr class="auth-hide-user">
								<td class="head" data-tr="wan mtu">WAN MTU</td>
								<td><input name="wan_mtu" type="text" class="half" style="display:none;">
									<select name="wan_mtu_type" onChange="wanMtuChange(this.form);" class="mid">
										<option value="0" data-tr="inet auto">AUTO</option>
										<option value="1" selected="selected" data-tr="inet custom">Custom</option>
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

							<tr id="staticDNSprofile">
								<td class="head" data-tr="inet dns profile">DNS Profile</td>
								<td><div style="float: left">
									<select name="wStaticDnsProfile" onChange="dnsSwitchClick(this.form);" class="mid auth-disable-user">
										<option data-tr="inet dns profile dhcp"    value="auto" id="dns_profile_auto">Auto (DHCP)</option>
										<option data-tr="inet dns profile manual"  value="manual">Manual</option>
										<option data-tr="inet dns profile google"  value="google">Google DNS</option>
										<option data-tr="inet dns profile yandex"  value="yandex">Yandex DNS</option>
										<option data-tr="inet dns profile sky"     value="sky">Sky DNS</option>
										<option data-tr="inet dns profile open"    value="open">Open DNS</option>
										<option data-tr="inet dns profile adguard" value="adguard">AdGuard DNS</option>
									</select>
									</div>
									<div id="wStaticDnsProfile_learne" style="float: left; margin: 3px 10px"></div>
								</td>
							</tr>
							<tr id="staticDNSyandexProfile">
								<td class="head" data-tr="inet dns profile yandex title">Yandex Profile</td>
								<td>
									<select name="wStaticDnsYandexProfile" onChange="dnsSwitchClick(this.form);" class="mid auth-disable-user">
										<option data-tr="inet dns profile yandex basic"  value="basic">Basic</option>
										<option data-tr="inet dns profile yandex safe"   value="safe">Safe</option>
										<option data-tr="inet dns profile yandex family" value="family">Family</option>
									</select>
								</td>
							</tr>
							<tr id="staticDNSadguardProfile">
								<td class="head" data-tr="inet dns profile adguard title">AdGuard Profile</td>
								<td>
									<select name="wStaticDnsAdguardProfile" onChange="dnsSwitchClick(this.form);" class="mid auth-disable-user">
										<option data-tr="inet dns profile adguard default" value="default">Default</option>
										<option data-tr="inet dns profile adguard family"  value="family">Family</option>
									</select>
								</td>
							</tr>
							<tr id="priDNSrow">
								<td class="head" data-tr="inet pri dns">Primary DNS Server</td>
								<td><input name="staticPriDns" class="mid auth-disable-user"></td>
							</tr>
							<tr id="secDNSrow">
								<td class="head" data-tr="inet sec dns">Secondary DNS Server</td>
								<td><input name="staticSecDns" class="mid auth-disable-user"></td>
							</tr>
							<tr id="natRowDisplay">
								<td class="head" data-tr="wan nat enabled">Enable NAT</td>
								<td><select name="natEnabled" class="mid auth-disable-user">
									<option value="on" data-tr="inet auto">Auto</option>
									<option value="off" data-tr="button disable">Disable</option>
								</select></td>
							</tr>


							<tr id="dhcpReqIPRow" class="auth-hide-user">
								<td class="head" data-tr="wan request from dhcp">Request IP from DHCP (optional)</td>
								<td><input name="dhcpReqIP" class="mid"></td>
							</tr>
							<tr id="dhcpVendorRow" class="auth-hide-user">
								<td class="head" data-tr="wan dhcp vendor class">Vendor class identifier (optional)</td>
								<td><input name="dhcpVendorClass" class="mid"></td>
							</tr>
						</tbody>
					</table>

					<table class="form">
						<col style="width: 40%" />
						<col style="width: 60%" />
						<tbody>
							<tr>
								<td class="title" colspan="2" data-tr="inet mac">MAC address</td>
							</tr>
							<tr id="MACrow">
								<td class="head" data-tr="wan mac">WAN MAC address</td>
								<td>
									<input name="wanMac" id="wanMac" class="mid auth-disable-user">
									<input type="button" value="Restore Factory" class="auth-disable-user" data-tr="button restore factory" name="restoremac" onClick="this.form.reboot.value = '1'; ajaxPostForm(_('wan reboot confirm'), document.rebootForm, 'rebootReloader', _('message config'), ajaxShowProgress);">
								</td>
							</tr>
						</tbody>
					</table>

					<table class="buttons auth-hide-user">
						<tr>
							<td>
								<input type="submit" class="normal" value="Apply"  data-tr="button apply"  onClick="return CheckValues(this.form);">
								<input type="button" class="normal" value="Cancel" data-tr="button cancel" onClick="window.location.reload();">
								<input type="button" class="normal" value="Reset"  data-tr="button reset"  onClick="resetValues(this.form, 30);">
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
