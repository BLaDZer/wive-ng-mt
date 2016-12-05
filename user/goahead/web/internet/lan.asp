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
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("buttons");

			var opMode		= '<% getCfgZero(1, "OperationMode"); %>';
			var hostname		= '<% getCfgGeneral(1, "HostName"); %>';
			var wanIp		= '<% getCfgGeneral(1, "wan_ipaddr"); %>';
			var gateway		= '<% getCfgGeneral(1, "wan_gateway"); %>';
			var dnsPrimary		= '<% getDns(1); %>';
			var dnsSecondary	= '<% getDns(2); %>';
			var lanIp		= '<% getLanIp(); %>';
			var lanNetmask		= '<% getLanNetmask(); %>';
			var dhcp 		= '<% getCfgZero(1, "dhcpEnabled"); %>';
			var dhcpStart		= '<% getCfgGeneral(1, "dhcpStart"); %>';
			var dhcpEnd		= '<% getCfgGeneral(1, "dhcpEnd"); %>';
			var dhcpGateway		= '<% getCfgGeneral(1, "dhcpGateway"); %>';

			function initTranslation()
			{
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
				_TRV("lApply",			"button apply");
				_TRV("lCancel",			"button cancel");
				_TRV("lReset",			"button reset");
			}

			function initValues()
			{
				var form = document.lanCfg;

				form.hostname.value	= hostname;
				form.lanIp.value	= lanIp;
				form.lanNetmask.value	= lanNetmask;
				form.lanGateway.value	= gateway;
				form.lanPriDns.value	= dnsPrimary;
				form.lanSecDns.value	= dnsSecondary;
				form.dhcpStart.value	= dhcpStart;
				form.dhcpEnd.value	= dhcpEnd;
				form.dhcpGateway.value	= dhcpGateway;

				displayElement( [ 'brGateway', 'brPriDns', 'brSecDns' ], opMode == "0" );

				showWarning();
				initTranslation();
			}

			function checkValues(form) 
			{
				var dhcp_edit = 0;
				
				if ((form.hostname.value == "") || (form.hostname.value.match(/[^A-Z0-9_-]/gi))) {
					alert(_("inet hostname wrong"));
					form.hostname.focus();
					return false;
				}
				if (!validateIP(form.lanIp, true))
				{
					form.lanIp.focus();
					return false;
				}
				if (!validateIPMask(form.lanNetmask, true))
				{
					form.lanNetmask.focus();
					return false;
				}
				if ((opMode != "0") && (form.lanIp.value == wanIp)) {
					alert(_("inet lan wan same"));
					form.lanIp.focus();
					return false;
				}
				if (opMode == "0") {
					if (!validateIP(form.lanGateway, true))	{
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

				if (((form.lanIp.value != lanIp) || (form.lanNetmask.value != lanNetmask) && (dhcp == "1")) && opMode != '0') {
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
						if (form.dhcpGateway.value == lanIp)
							form.dhcpGateway.value = form.lanIp.value;
					}
				}

				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return false;
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
			<td><h1 id="lTitle"></h1>
				<p id="lIntroduction"></p>
				<hr>
				<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
				<form method="POST" name="lanCfg" action="/goform/setLan" onSubmit="return checkValues(this);">
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
							<td><input type="submit" class="normal" value="Apply"  id="lApply">&nbsp;&nbsp;
								<input type="button" class="normal" value="Cancel" id="lCancel" onClick="window.location.reload();">&nbsp;&nbsp;
								<input type="button" class="normal" value="Reset"  id="lReset"  onClick="resetValues(this.form);">
								<input type="hidden" name="reset" value="0">
								<input name="dhcpStart" type="hidden">
								<input name="dhcpEnd" type="hidden">
								<input name="dhcpGateway" type="hidden">
							</td>
						</tr>
					</table>
				</form>
			</tr>
		</table>
	</body>
</html>
