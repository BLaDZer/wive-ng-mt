<!DOCTYPE html>
<html>
	<head>
		<title>Wide Area Network (WAN) Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("buttons");

			function initTranslation()
			{
				_TR("wTitle",				"wan title");
				_TR("wIntroduction",		"wan introduction");

				_TR("wConnectionType",		"wan connection type");
				_TR("wConnection",			"wan connection");
				_TR("wConnTypeStatic",		"wan connection type static");
				_TR("wConnTypeDhcp",		"wan connection type dhcp");
				_TR("wConnTypeZero",		"wan connection type zero");

				_TR("wStaticMode",			"wan static mode");
				_TR("wStaticDns",			"wan static dns");
				_TR("wStaticIp",			"inet ip");
				_TR("wStaticNetmask",		"inet netmask");
				_TR("wStaticGateway",		"inet gateway");
				_TR("wStaticPriDns",		"inet pri dns");
				_TR("wStaticSecDns",		"inet sec dns");

				_TR("wDhcpMode",			"wan dhcp mode");
				_TR("wAdditionalOptions",	"wan additional options");
				_TR("wReqFromDHCP",			"wan request from dhcp");
				_TR("wDHCPVendorClass",		"wan dhcp vendor class");
				_TR("wMTU",					"wan mtu");
				_TR("wAuto",				"inet auto");
				_TR("wCustom",				"routing custom");
				_TR("wNatEnabled",			"wan nat enabled");
				_TR("wMacAddress",			"inet mac");
				_TR("wMacAddr",				"wan mac");

				_TRV("wApply",				"button apply");
				_TRV("wCancel",				"button cancel");
				_TRV("wReset",				"button reset");
				_TRV("WanMacRestore",		"button restore factory");
			}

			function initValues()
			{
				var form		= document.wanCfg;
				var mode 		= '<% getCfgGeneral(1, "wanConnectionMode"); %>';
				var nat			= '<% getCfgZero(1, "natEnabled"); %>';
				var opMode		= '<% getCfgZero(1, "OperationMode"); %>';
				var static_dns	= '<% getCfgZero(1, "wan_static_dns"); %>';
				var wan_mtu		= defaultNumber("<% getCfgGeneral(1, "wan_manual_mtu"); %>", '0');
				var wanIp		= '<% getCfgGeneral(1, "wan_ipaddr"); %>';
				var wanNetmask	= '<% getCfgGeneral(1, "wan_netmask"); %>';
				var wanGateway	= '<% getCfgGeneral(1, "wan_gateway"); %>';
				var dhcpReqIP	= '<% getCfgGeneral(1, "dhcpRequestIP"); %>';
				var dhcpVendor	= '<% getCfgGeneral(1, "dhcpVendorClass"); %>';
				var priDNS		= '<% getDns(1); %>';
				var secDNS		= '<% getDns(2); %>';
				var wanMac		= '<% getCfgGeneral(1, "WAN_MAC_ADDR"); %>';

				form.staticIp.value 		= wanIp;
				form.staticNetmask.value	= wanNetmask;
				form.staticGateway.value	= wanGateway;
				form.dhcpReqIP.value		= dhcpReqIP;
				form.dhcpVendorClass.value	= dhcpVendor;
				form.staticPriDns.value		= priDNS;
				form.staticSecDns.value		= secDNS;
				form.wanMac.value		= wanMac;
				
				displayElement(document.getElementById("natRowDisplay"), opMode != "0");
				form.natEnabled.checked = (nat == "1");

				form.connectionType.value = mode;
				form.wStaticDnsEnable.checked = (static_dns == "on");

				form.wan_mtu.value = wan_mtu;
				for (var i=0; i < form.wan_mtu_type.options.length; i++)
					if (form.wan_mtu_type.options[i].value == wan_mtu)
					{
						form.wan_mtu_type.value = form.wan_mtu_type.options[i].value;
						break;
					}

				connectionTypeSwitch(form);
				wanMtuChange(form);
				showWarning();
				initTranslation();
			}

			function CheckValues(form)
			{
				var c_type	= form.connectionType.value;
				var wan_mtu	= form.wan_mtu.value * 1;
				var opMode	= '<% getCfgZero(1, "OperationMode"); %>';
				var lanIp	= '<% getCfgGeneral(1, "lan_ipaddr"); %>';
				var lan2Ip	= '<% getCfgGeneral(1, "lan2_ipaddr"); %>';

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
					if (form.staticGateway.value != "") 
						if (!validateIP(form.staticGateway, true)) {
							form.staticGateway.focus();
							return false;
						}
					if ((opMode != "0") && ((form.staticIp.value == lanIp) || (form.staticIp.value == lan2Ip))) {
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
				if (form.wanMac.value != "<% getCfgGeneral(1, "WAN_MAC_ADDR"); %>") {
					if (!ajaxPostForm(_('wan reboot confirm'), form, 'timerReloader', _("message config"), ajaxShowProgress)) {
						ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
						form.reboot.value = "0";
					}
				}
				else {
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
					form.reboot.value = "0";
				}
				return true;
			}

			function connectionTypeSwitch(form)
			{
				var conn_type = form.connectionType.value;
				displayElement('staticDHCP', conn_type == 'STATIC');
				displayElement(['dhcpReqIPRow', 'dhcpVendorRow'], conn_type == 'DHCP');
				displayElement('staticDNSAssignRow', conn_type != 'ZERO');

				if (conn_type == 'STATIC') {
					form.wStaticDnsEnable.checked = true;
					form.wStaticDnsEnable.disabled = true;
				}
				else {
					form.wStaticDnsEnable.disabled = false;
				}

				dnsSwitchClick(form);
			}

			function dnsSwitchClick(form)
			{
				displayElement( ['priDNSrow', 'secDNSrow' ], (form.wStaticDnsEnable.checked) || (form.connectionType.value == 'ZERO'));
			}

			function wanMtuChange(form)
			{
				if (form.wan_mtu_type.value == '1')
				{
					form.wan_mtu.style.display = '';
					form.wan_mtu.setAttribute('class', 'half');
					form.wan_mtu_type.setAttribute('class', 'half');
				}
				else
				{
					form.wan_mtu_type.setAttribute('class', 'mid');
					form.wan_mtu.style.display = 'none';
					form.wan_mtu.value = form.wan_mtu_type.value;
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<form name="rebootForm" style="display: none;" method="GET" action="/goform/restoremac" >
			<iframe id="rebootReloader" name="rebootReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
		</form>
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td><h1 id="wTitle"></h1>
					<p id="wIntroduction"></p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="wanCfg" action="/goform/setWan" onSubmit="return CheckValues(this);">
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="wConnection">WAN connection type</td>
						</tr>
						<tr>
							<td class="head"><b id="wConnectionType" style="width: 40%">Connection type</b></td>
							<td style="width: 60%"><select name="connectionType" class="full" onChange="connectionTypeSwitch(this.form);">
								<option id="wConnTypeStatic" value="STATIC" selected="selected">Static Mode (fixed IP)</option>
								<option id="wConnTypeDhcp"   value="DHCP">DHCP (Auto Config)</option>
								<option id="wConnTypeZero"   value="ZERO">Zeroconf</option>
							</select></td>
						</tr>
					</table>
					<!-- ================= STATIC Mode ================= -->
					<table id="staticDHCP" class="form">
						<tr>
							<td class="title" colspan="2" id="wStaticMode">Static Mode</td>
						</tr>
						<tr>
							<td class="head" id="wStaticIp" style="width: 40%">IP Address</td>
							<td style="width: 60%"><input name="staticIp" class="mid"></td>
						</tr>
						<tr>
							<td class="head" id="wStaticNetmask" style="width: 40%">Subnet Mask</td>
							<td style="width: 60%"><input name="staticNetmask" class="mid"></td>
						</tr>
						<tr>
							<td class="head" id="wStaticGateway" style="width: 40%">Default Gateway</td>
							<td style="width: 60%"><input name="staticGateway" class="mid"></td>
						</tr>
					</table>
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="wAdditionalOptions">Additional Options</td>
						</tr>
						<tr id="dhcpReqIPRow">
							<td class="head" id="wReqFromDHCP" style="width: 40%">Request IP from DHCP (optional)</td>
							<td style="width: 60%"><input name="dhcpReqIP" class="mid"></td>
						</tr>
						<tr id="dhcpVendorRow">
							<td class="head" id="wDHCPVendorClass" style="width: 40%">Vendor class identifier (optional)</td>
							<td style="width: 60%"><input name="dhcpVendorClass" class="mid"></td>
						</tr>
						<tr>
							<td class="head" id="wMTU" style="width: 40%">WAN MTU</td>
							<td style="width: 60%"><input name="wan_mtu" type="text" class="half" style="display:none;">
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
							<td class="head" id="wStaticDns" style="width: 40%">Assign static DNS Server</td>
							<td style="width: 60%"><input name="wStaticDnsEnable" type="checkbox" onClick="dnsSwitchClick(this.form);" ></td>
						</tr>
						<tr id="priDNSrow" style="display:none;" >
							<td class="head" id="wStaticPriDns" style="width: 40%">Primary DNS Server</td>
							<td style="width: 60%"><input name="staticPriDns" class="mid"></td>
						</tr>
						<tr id="secDNSrow" style="display:none;" >
							<td class="head" id="wStaticSecDns" style="width: 40%">Secondary DNS Server</td>
							<td style="width: 60%"><input name="staticSecDns" class="mid"></td>
						</tr>
						<tr id="natRowDisplay">
							<td class="head" id="wNatEnabled" style="width: 40%">Enable NAT</td>
							<td style="width: 60%"><input name="natEnabled" type="checkbox"></td>
						</tr>
					</table>
					<br>
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="wMacAddress">MAC address</td>
						</tr>
						<tr id="MACrow">
							<td class="head" id="wMacAddr" style="width: 40%">WAN MAC address</td>
							<td style="width: 60%">
								<input name="wanMac" id="wanMac" class="mid">
								<input type="button" value="Restore Factory" id="WanMacRestore" name="restoremac" onClick="ajaxPostForm(_('wan reboot confirm'), document.rebootForm, 'rebootReloader', _('message config'), ajaxShowProgress);">
							</td>
						</tr>
					</table>
					<br>
					<table class="buttons">
						<tr>
							<td><input type="submit" class="normal" value="Apply" id="wApply">&nbsp;&nbsp;
								<input type="button" class="normal" value="Cancel" id="wCancel" onClick="window.location.reload();">&nbsp;&nbsp;
								<input type="button" class="normal" value="Reset" id="wReset" onClick="resetValues(this.form);">
								<input type="hidden" value="1" name="reboot">
								<input type="hidden" value="0" name="reset">
							</td>
						</tr>
					</table>
				</form>
			</tr>
		</table>
	</body>
</html>
