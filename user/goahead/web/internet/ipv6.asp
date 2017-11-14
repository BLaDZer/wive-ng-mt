<!DOCTYPE html>
<html>
	<head>
		<title>IPv6 Settings</title>
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
			Butterlate.setTextDomain("buttons");
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("services");

			function initTranslation() {
				_TR("v6Title",				"ipv6 title");
				_TR("v6Introduction",		"ipv6 introduction");
				_TR("v6ConnType",			"ipv6 connection type");
				_TR("v6OpMode",				"ipv6 operation mode");
				_TR("v6Disable",			"button disable");
				_TR("v6Static",				"ipv6 static");
				_TR("IPv6Dhcpc",			"ipv6 dhcp6c");
				_TR("IPv6AllowForward", 	"ipv6 allow forward");
				_TR("v6StaticIPSetup",		"ipv6 static ip");
				_TR("v6StaticIPAddrLan",	"ipv6 static lan address");
				_TR("v6StaticIPAddrWan",	"ipv6 static wan address");
				_TR("v6StaticGW",			"ipv6 static gateway");
				_TR("v6StaticDNSprimary",	"ipv6 static dns primary");
				_TR("v6StaticDNSsecondary", "ipv6 static dns secondary");
				_TR("v66rdSetup",			"ipv6 6rd enabled");
				_TR("v66rdPrefix",			"ipv6 6rd prefix");
				_TR("v66rdBorderIPAddr",	"ipv6 6rd relay address");
				_TR("6to4Setup",			"ipv6 6to4 enabled");
				_TR("v66to4SrvIpaddr",		"ipv6 6to4 server address");
				_TR("v6invpn",				"ipv6 Ipv6InVPN");

				_TR("v6services",			"ipv6 services");
				_TR("v6servicename",		"ipv6 service name");
				_TR("v6value",				"ipv6 service value");
				_TR("v6details",			"services status details");
				_TR("v6about",				"services status about");
				_TR("v6status",				"ipv6 service status");
				_TR("v6Radvd",				"ipv6 radvd");
				_TR("v6RadvdD",				"button disable");
				_TR("v6RadvdE",				"button enable");
				_TR("v6Dhcpv6",				"ipv6 dhcp6s");
				_TR("v6Dhcpv6D",			"button disable");
				_TR("v6Dhcpv6E",			"button enable");

				_TRV("v6Apply",				"button apply");
				_TRV("v6Cancel",			"button cancel");
				_TRV("v6Reset",				"button reset");
			}

			function initValues() {
				var form = document.ipv6_cfg;

				if (BUILD_IPV6_6RD == '1')
					form.ipv6_opmode.options[2] = new Option(_("ipv6 6rd"), "2");
				if (BUILD_IPV6_TO4 == '1')
					form.ipv6_opmode.options[form.ipv6_opmode.options.length] = new Option(_("ipv6 6to4"), "3");

				if (NVRAM_IPv6OpMode == '1')
					form.ipv6_opmode.options.selectedIndex = 1;
				else if (NVRAM_IPv6OpMode == '2')
					form.ipv6_opmode.options.selectedIndex = 2;
				else if (NVRAM_IPv6OpMode == '3')
					form.ipv6_opmode.options.selectedIndex = form.ipv6_opmode.options.length - 1;

				form.dhcp6c_enable.checked 				= NVRAM_IPv6Dhcpc == '1';

				form.ipv6_allow_forward.checked			= NVRAM_IPv6AllowForward == '1';
				form.ipv6_Ipv6InVPN.checked				= NVRAM_Ipv6InVPN == '1';

				form.radvdEnbl.options.selectedIndex 	= +NVRAM_radvdEnabled;
				form.dhcpv6Enbl.options.selectedIndex 	= +NVRAM_dhcpv6Enabled;

				addOption(form.ipv6_manual_mtu_type, _('inet auto'), 0);
				addOption(form.ipv6_manual_mtu_type, _('routing custom'), 1);
				for (var i = 1500; i >= 1280; i -= 10)
					addOption(form.ipv6_manual_mtu_type, i, i);

				document.getElementById('ipv6_manual_mtu').value		= NVRAM_IPv6ManualMTU;
				document.getElementById('ipv6_manual_mtu_type').value	= 1;
				for (var i = 0; i < form.ipv6_manual_mtu_type.options.length; i++)
					if (form.ipv6_manual_mtu_type.options[i].value == NVRAM_IPv6ManualMTU) {
						form.ipv6_manual_mtu_type.value = form.ipv6_manual_mtu_type.options[i].value;
						break;
					}

				SwitchOpMode(form);
				ipv6MtuChange();

				displayElement( 'radvd', BUILD_RADVD == '1');
				displayElement( 'dhcpv6', BUILD_DHCPV6 == '1');

				initTranslation();
				showWarning();

				displayServiceStatus(
					[
						[ NVRAM_radvdEnabled,	'radvd',	'radvd',	null,	'www.litech.org/radvd'			 ],
						[ NVRAM_dhcpv6Enabled,	'dhcpv6',	'dhcp6s',	null,	'wide-dhcpv6.sourceforge.net'	 ]
					]
				);
			}

			function checkValues(form) {
				if ((form.ipv6_opmode.value == "1") && (!form.dhcp6c_enable.checked)) {
					var ipv6_lan = form.ipv6_lan_ipaddr.value.replace(/\s+/g, '');
					var ipv6_lan_prefix = form.ipv6_lan_prefix_len.value.replace(/\s+/g, '');
					var ipv6_wan = form.ipv6_wan_ipaddr.value.replace(/\s+/g, '');
					var ipv6_wan_prefix = form.ipv6_wan_prefix_len.value.replace(/\s+/g, '');
					var ipv6_gw = form.ipv6_static_gw.value.replace(/\s+/g, '');
					var ipv6_dns_primary = form.ipv6_static_dns_primary.value.replace(/\s+/g, '');
					var ipv6_dns_secondary = form.ipv6_static_dns_secondary.value.replace(/\s+/g, '');
					
					if (!ipaddr.IPv6.isValid(ipv6_lan)) {
						alert(_("ipv6 invalid addr"));
						form.ipv6_lan_ipaddr.focus();
						form.ipv6_lan_ipaddr.select();
						return false;
					}
					if (ipv6_lan_prefix == "" || ipv6_lan_prefix > 128 || ipv6_lan_prefix < 0) {
						alert(_("ipv6 invalid prefix"));
						form.ipv6_lan_prefix_len.focus();
						form.ipv6_lan_prefix_len.select();
						return false;
					}
					if (!ipaddr.IPv6.isValid(ipv6_wan)) {
						alert(_("ipv6 invalid addr"));
						form.ipv6_wan_ipaddr.focus();
						form.ipv6_wan_ipaddr.select();
						return false;
					}
					if (ipv6_wan_prefix == "" || ipv6_wan_prefix > 128 || ipv6_wan_prefix < 0) {
						alert(_("ipv6 invalid prefix"));
						form.ipv6_wan_prefix_len.focus();
						form.ipv6_wan_prefix_len.select();
						return false;
					}
					if (!ipaddr.IPv6.isValid(ipv6_gw)) {
						alert(_("ipv6 invalid addr"));
						form.ipv6_static_gw.focus();
						form.ipv6_static_gw.select();
						return false;
					}
					if (!ipaddr.IPv6.isValid(ipv6_dns_primary)) {
						alert(_("ipv6 invalid addr"));
						form.ipv6_static_dns_primary.focus();
						form.ipv6_static_dns_primary.select();
						return false;
					}
					if (!ipaddr.IPv6.isValid(ipv6_dns_secondary)) {
						alert(_("ipv6 invalid addr"));
						form.ipv6_static_dns_secondary.focus();
						form.ipv6_static_dns_secondary.select();
						return false;
					}
					if (!validateNum(document.getElementById('ipv6_manual_mtu').value) ||
					    (document.getElementById('ipv6_manual_mtu_type').value != 0 && (document.getElementById('ipv6_manual_mtu').value > 1500 || document.getElementById('ipv6_manual_mtu').value < 80))) {
						alert(_("inet invalid mtu"));
						document.getElementById('ipv6_manual_mtu').select();
						document.getElementById('ipv6_manual_mtu').focus();
						return false;
					}
					else {
						document.getElementById('ipv6_manual_mtu').value = +document.getElementById('ipv6_manual_mtu').value;
					}
					var addr = ipaddr.parse(ipv6_lan); form.ipv6_lan_ipaddr.value = addr.toString(); 
					addr = ipaddr.parse(ipv6_wan); form.ipv6_wan_ipaddr.value = addr.toString();
					addr = ipaddr.parse(ipv6_gw); form.ipv6_static_gw.value = addr.toString();
					addr = ipaddr.parse(ipv6_dns_primary); form.ipv6_static_dns_primary.value = addr.toString();
					addr = ipaddr.parse(ipv6_dns_secondary); form.ipv6_static_dns_secondary.value = addr.toString();
					
				} else if (form.ipv6_opmode.value == "1") {
					if (!validateNum(document.getElementById('ipv6_manual_mtu').value) ||
					    (document.getElementById('ipv6_manual_mtu_type').value != 0 && (document.getElementById('ipv6_manual_mtu').value > 1500 || document.getElementById('ipv6_manual_mtu').value < 80))) {
						alert(_("inet invalid mtu"));
						document.getElementById('ipv6_manual_mtu').select();
						document.getElementById('ipv6_manual_mtu').focus();
						return false;
					}
					else {
						document.getElementById('ipv6_manual_mtu').value = +document.getElementById('ipv6_manual_mtu').value;
					}
				} else if (form.ipv6_opmode.value == '2') {
					var v6rd_prefix = form.ipv6_6rd_prefix.value.replace(/\s+/g, '');
					var v6rd_prefix_len = form.ipv6_6rd_prefix_len.value.replace(/\s+/g, '');
					var v6rd_border_ipaddr = form.ipv6_6rd_border_ipaddr.value.replace(/\s+/g, '');

					if (!ipaddr.IPv6.isValid(v6rd_prefix)) {
						alert(_("ipv6 invalid addr"));
						form.ipv6_6rd_prefix.focus();
						form.ipv6_6rd_prefix.select();
						return false;
					}
					if (v6rd_prefix_len == "" || v6rd_prefix_len > 32 || v6rd_prefix_len < 0) {
						alert(_("ipv6 invalid prefix"));
						form.ipv6_6rd_prefix_len.focus();
						form.ipv6_6rd_prefix_len.select();
						return false;
					}
					if (!ipaddr.IPv4.isValid(v6rd_border_ipaddr)) {
						alert(_("ipv6 invalid ipv4"));
						form.ipv6_6rd_border_ipaddr.focus();
						form.ipv6_6rd_border_ipaddr.select();
						return false;
					}
				} else if (form.ipv6_opmode.value == '3') {
					var v6SrvAddr = form.IPv6SrvAddr.value.replace(/\s+/g, '');
					if (!ipaddr.IPv4.isValid(v6SrvAddr)) {
						alert(_("ipv6 invalid ipv4"));
						form.IPv6SrvAddr.focus();
						form.IPv6SrvAddr.select();
						return false;
					}
				} else if (form.ipv6_opmode.value == "0") {
					form.radvdEnbl.options.selectedIndex = 0;
					form.dhcpv6Enbl.options.selectedIndex = 0;
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);
				return true;
			}

			function SwitchOpMode(form) {
				var opmode = form.ipv6_opmode.value;

				enableElements( [ form.ipv6_allow_forward ], opmode != '0');
				enableElements( [ form.ipv6_Ipv6InVPN ], opmode != '0' && NVRAM_vpnEnabled == 'on');
				enableElements( [ form.dhcp6c_enable ], opmode == '1');
				enableElements( [ form.ipv6_lan_ipaddr, form.ipv6_lan_prefix_len, form.ipv6_wan_ipaddr, form.ipv6_wan_prefix_len, form.ipv6_static_gw ], opmode == '1' && !form.dhcp6c_enable.checked);
				enableElements( [ form.ipv6_6rd_prefix, form.ipv6_6rd_prefix_len, form.ipv6_6rd_border_ipaddr ], BUILD_IPV6_6RD == '1' && opmode == '2');
				enableElements( [ form.IPv6SrvAddr ], opmode == '3');

				if (opmode == '1') {
					form.ipv6_lan_ipaddr.value				= NVRAM_IPv6IPAddr;
					form.ipv6_lan_prefix_len.value			= NVRAM_IPv6PrefixLen;
					form.ipv6_wan_ipaddr.value				= NVRAM_IPv6WANIPAddr;
					form.ipv6_wan_prefix_len.value			= NVRAM_IPv6WANPrefixLen;
					form.ipv6_static_gw.value				= NVRAM_IPv6GWAddr;
					form.ipv6_static_dns_primary.value		= NVRAM_IPv6DNSPrimary;
					form.ipv6_static_dns_secondary.value	= NVRAM_IPv6DNSSecondary;
				} else if (opmode == '2') {
					form.ipv6_6rd_prefix.value				= NVRAM_IPv6IPAddr;
					form.ipv6_6rd_prefix_len.value			= NVRAM_IPv6PrefixLen;
					form.ipv6_6rd_border_ipaddr.value		= NVRAM_IPv6SrvAddr;
				} else if (opmode == '3')
					form.IPv6SrvAddr.value					= NVRAM_IPv6SrvAddr;

				displayElement( 'IPv6AllowForwardRowDisplay', opmode != '0');
				displayElement( 'v6invpn_tr', opmode != '0' && NVRAM_vpnEnabled == 'on');
				displayElement( [ 'dhcp6cRowDisplay', 'v6StaticMTU_tr' ], opmode == '1');
				displayElement( 'v6StaticTable', opmode == '1' && !form.dhcp6c_enable.checked);
				displayElement( 'v66rdTable', BUILD_IPV6_6RD == '1' && opmode == '2');
				displayElement( '6to4Table', opmode == '3');
				displayElement( 'daemons', opmode != '0' && (BUILD_RADVD == '1' || BUILD_DHCPV6 == '1'));
			}

			function ipv6MtuChange() {
				if (document.getElementById('ipv6_manual_mtu_type').value == '1') {
					showElement('ipv6_manual_mtu');
					document.getElementById('ipv6_manual_mtu').focus();
				}
				else {
					hideElement('ipv6_manual_mtu');
					document.getElementById('ipv6_manual_mtu').value = document.getElementById('ipv6_manual_mtu_type').value;
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="v6Title">IPv6 Setup</h1>
					<p id="v6Introduction"></p>
					<hr />
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form id="ipv6_cfg" name="ipv6_cfg" method="POST" action="/goform/setIPv6">
						<table class="form">
							<tr id="v6ConnType_tr">
								<td id="v6ConnType" class="title" colspan="2">IPv6 Connection Type</td>
							</tr>
							<tr id="v6OpMode_tr">
								<td id="v6OpMode" class="head" width="45%">IPv6 Operation Mode</td>
								<td width="55%"><select id="ipv6_opmode" name="ipv6_opmode" size="1" onChange="SwitchOpMode(this.form);">
									<option value="0" id="v6Disable">Disable</option>
									<option value="1" id="v6Static">Native dynamic/static IP Connection</option>
								</select></td>
							</tr>
							<tr id="v6invpn_tr">
								<td id="Ipv6InVPN" class="head" width="45%">IPv6 over VPN</td>
								<td width="55%"><input name="ipv6_Ipv6InVPN" type="checkbox"></td>
							</tr>
							<tr id="dhcp6cRowDisplay">
								<td id="IPv6Dhcpc" class="head" width="45%">IPv6 autoconfigure by dhcp/ra</td>
								<td width="55%"><input name="dhcp6c_enable" type="checkbox" onClick="SwitchOpMode(this.form);"></td>
							</tr>
							<tr id="IPv6AllowForwardRowDisplay">
								<td id="IPv6AllowForward" class="head" width="45%">Allow access to LAN from internet</td>
								<td width="55%"><input name="ipv6_allow_forward" type="checkbox"></td>
							</tr>
							<tr id="v6StaticMTU_tr">
								<td id="v6StaticMTU" class="head" width="45%">MTU</td>
								<td width="55%">
									<input id="ipv6_manual_mtu" name="ipv6_manual_mtu" type="text" style="width: 130px; display: none;">
									<select style="width: 130px" id="ipv6_manual_mtu_type" name="ipv6_manual_mtu_type" size="1" onChange="ipv6MtuChange();"></select>
								</td>
							</tr>
						</table>
						<!-- STATIC/DynaMIC IP -->
						<table id="v6StaticTable" class="form" style="visibility: hidden;">
							<tr>
								<td id="v6StaticIPSetup" class="title" colspan="2">IPv6 Static IP Setup</td>
							</tr>
							<tr>
								<td id="v6StaticIPAddrLan" class="head" width="45%">LAN IPv6 Address / Subnet Prefix Length</td>
								<td width="55%"><input name="ipv6_lan_ipaddr" maxlength=39 size=39> / <input name="ipv6_lan_prefix_len" maxlength=3 size=2></td>
							</tr>
							<tr>
								<td id="v6StaticIPAddrWan" class="head" width="45%">WAN IPv6 Address / Subnet Prefix Length</td>
								<td width="55%"><input name="ipv6_wan_ipaddr" maxlength=39 size=39> / <input name="ipv6_wan_prefix_len" maxlength=3 size=2></td>
							</tr>
							<tr>
								<td id="v6StaticGW" class="head" width="45%">Default Gateway</td>
								<td width="55%"><input name="ipv6_static_gw" maxlength=39 size=39></td>
							</tr>
							<tr>
								<td id="v6StaticDNSprimary" class="head" width="45%">Primary DNS</td>
								<td width="55%"><input name="ipv6_static_dns_primary" maxlength=39 size=39></td>
							</tr>
							<tr>
								<td id="v6StaticDNSsecondary" class="head" width="45%">Secondary DNS</td>
								<td width="55%"><input name="ipv6_static_dns_secondary" maxlength=39 size=39></td>
							</tr>
						</table>
						<!-- 6RD -->
						<table id="v66rdTable" class="form" style="visibility: hidden;">
							<tr>
								<td id="v66rdSetup" class="title" colspan="2">Tunneling Connection (6RD) Setup</td>
							</tr>
							<tr>
								<td id="v66rdPrefix" class="head" width="45%">ISP 6rd Prefix / Prefix Length</td>
								<td width="55%"><input name="ipv6_6rd_prefix" maxlength=39 size=39> / <input name="ipv6_6rd_prefix_len" maxlength=3 size=2></td>
							</tr>
							<tr>
								<td id="v66rdBorderIPAddr" class="head" width="45%">ISP Border Relay IPv4 Address</td>
								<td width="55%"><input name="ipv6_6rd_border_ipaddr" maxlength=15 size=15></td>
							</tr>
						</table>
						<!-- 6to4 -->
						<table id="6to4Table" class="form" style="visibility: hidden;">
							<tr>
								<td id="6to4Setup" class="title" colspan="2">Tunneling Connection (6to4) Setup</td>
							</tr>
							<tr>
								<td id="v66to4SrvIpaddr" class="head" width="45%"> IPv4 to IPv6 server address </td>
								<td width="55%"><input name="IPv6SrvAddr" maxlength=15 size=15></td>
							</tr>
						</table>
						<!-- Settings daemons for lan -->
						<table class="form" id="daemons" style="visibility: hidden;">
							<col style="width: 45%"/>
							<col style="width: 19%"/>
							<col style="width: 19%"/>
							<col style="width: 17%"/>
							<tbody>
								<tr>
									<td class="title" colspan="4" id="v6services">Services IPv6</td>
								</tr>
								<tr>
									<td class="title" id="v6servicename">Service name</td>
									<td class="title" id="v6value">Value</td>
									<td class="title" id="v6details">Details</td>
									<td class="title" id="v6status">Status</td>
								</tr>
								<tr id="radvd">
									<td class="head" id="v6Radvd" width="45%">Router Advertisement</td>
									<td>
										<select name="radvdEnbl" style="width: 130px">
											<option value="0" id="v6RadvdD">Disable</option>
											<option value="1" id="v6RadvdE">Enable</option>
										</select>
									</td>
									<td>&nbsp;</td>
									<td>&nbsp;</td>
								</tr>
								<tr id="dhcpv6">
									<td class="head" id="v6Dhcpv6">Dynamic IPv6 configuration</td>
									<td>
										<select name="dhcpv6Enbl" style="width: 130px">
											<option value="0" id="v6Dhcpv6D">Disable</option>
											<option value="1" id="v6Dhcpv6E">Enable</option>
										</select>
									</td>
									<td>&nbsp;</td>
									<td>&nbsp;</td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr align="center">
								<td>
									<input type="submit" class="normal" value="Apply" id="v6Apply" onClick="return checkValues(this.form);">&nbsp;&nbsp;
									<input type="button" class="normal" value="Cancel" id="v6Cancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="normal" value="Reset" id="v6Reset" onClick="resetValues(this.form, 30);">
									<input type="hidden" name="reset" value="0">
								</td>
							</tr>
						</table>
					</form>
					<div class="whitespace">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
