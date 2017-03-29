<!DOCTYPE html>
<html>
	<head>
		<title>Ethernet Port Settings</title>
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
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("buttons");

			NVRAM_stb_port	= (NVRAM_stb_port == '1') ? (NVRAM_wan_port == '0') ? 1 : +NVRAM_wan_port - 1 : -1;
			NVRAM_sip_port	= (NVRAM_sip_port == '1') ? (NVRAM_wan_port == '0') ? 2 : +NVRAM_wan_port - 2 : -1;

			var port_swmode	= [ NVRAM_port1_swmode, NVRAM_port2_swmode, NVRAM_port3_swmode, NVRAM_port4_swmode, NVRAM_port5_swmode ];
			
			function initTranslation() {
				_TR("ethernetTitle",			"ethernet title");
				_TR("ethernetIntroduction",		"ethernet introduction");
				_TR("ethernetPortManagement",	"ethernet port management");
				_TR("ethernetWANport",			"ethernet wan port");
				_TR("ethernetFirstLANport",		"ethernet lan port");
				_TR("ethernetNearToWAN", 		"ethernet lan port near");
				_TR("ethernetDistantFromWAN",	"ethernet lan port distant");
				if (ETHER_PORTS == 3) {
					_TR("ethernetPort1Mode",		"ethernet port 1 mode");
					_TR("ethernetPort2Mode",		"ethernet port 2 mode");
					_TR("ethernetPort3Mode",		"ethernet port 1 mode");
					_TR("ethernetPort4Mode",		"ethernet port 2 mode");
					_TR("ethernetPort5Mode",		"ethernet port 3 mode");
				}
				else if (ETHER_PORTS == 2) {
					if (NVRAM_OperationMode == 1) {
						_TR("ethernetPort1Mode",		"ethernet port 1 mode");
						_TR("ethernetPort5Mode",		"ethernet port 5 mode");
					}
					else {
						_TR("ethernetPort1Mode",		"ethernet port 1 mode");
						_TR("ethernetPort5Mode",		"ethernet port 2 mode");
					}
				}
				else {
					_TR("ethernetPort1Mode",		"ethernet port 1 mode");
					_TR("ethernetPort2Mode",		"ethernet port 2 mode");
					_TR("ethernetPort3Mode",		"ethernet port 3 mode");
					_TR("ethernetPort4Mode",		"ethernet port 4 mode");
					_TR("ethernetPort5Mode",		"ethernet port 5 mode");
				}

				_TRV("ethernetApply",			"button apply");
				_TRV("ethernetCancel",			"button cancel");
				_TRV("ethernetReset",			"button reset");
			}

			function initValues() {
				// hide unused elements in pure AP mode
				switch(ETHER_PORTS) {
					case 1:
								hideElement(ethernetWANport_tr);
								hideElement(ethernetFirstLANport_tr);
								hideElement(ethernetPort2Mode_tr);
								hideElement(ethernetPort3Mode_tr);
								hideElement(ethernetPort4Mode_tr);
								hideElement(ethernetPort5Mode_tr);
								break;
					case 2:
								if (NVRAM_OperationMode != 1)
									hideElement(ethernetWANport_tr);
								hideElement(ethernetFirstLANport_tr);
								hideElement(ethernetPort2Mode_tr);
								hideElement(ethernetPort3Mode_tr);
								hideElement(ethernetPort4Mode_tr);
								break;
					case 3:
								hideElement(ethernetWANport_tr);
								hideElement(ethernetFirstLANport_tr);
								hideElement(ethernetPort1Mode_tr);
								hideElement(ethernetPort2Mode_tr);
								break;
				}

				// Set WAN port number
				if (ETHER_PORTS == 5 && ((NVRAM_wan_port > 0 && NVRAM_wan_port < (ETHER_PORTS - 1)) || NVRAM_wan_port > (ETHER_PORTS - 1)))
					NVRAM_wan_port = ETHER_PORTS - 1;
					
				// Set LAN port number
				if (NVRAM_lan_port != 'near' && NVRAM_lan_port != 'distant')
					NVRAM_lan_port = 'near';

				if (NVRAM_OperationMode != 1) {
					hideElement(ethernetWANport_tr);
					hideElement(ethernetFirstLANport_tr);
				}
					
				document.getElementById('wan_port').value = NVRAM_wan_port;
				document.getElementById('lan_port').value = NVRAM_lan_port;

				// Add Port speeds options & Show
				var j = (ETHER_PORTS == 2) ? 5 : ETHER_PORTS;
				for (i = ETHER_FIRST_PORT + 1; i <= ETHER_FIRST_PORT + j; i++) {
					addOption(document.getElementById('port' + i + '_swmode'), _("ethernet port mode auto"), 'auto');
					addOption(document.getElementById('port' + i + '_swmode'), _("ethernet port mode 10h"),  '10h');
					addOption(document.getElementById('port' + i + '_swmode'), _("ethernet port mode 10f"),  '10f');
					addOption(document.getElementById('port' + i + '_swmode'), _("ethernet port mode 100h"), '100h');
					addOption(document.getElementById('port' + i + '_swmode'), _("ethernet port mode 100f"), '100f');
					if (BUILD_GIGAPHY == '1')
						addOption(document.getElementById('port' + i + '_swmode'), _("ethernet port mode 1000f"), '1000f');
					document.getElementById('port' + i + '_swmode').value = port_swmode[i - 1];
				}

				function reloadStat() {
					ajaxLoadScript('/internet/ethernet-stat.js');
					setTimeout(reloadStat, 10000);
				}

				showWarning();
				initTranslation();
				reloadStat();
			}
			
			function checkValues(form) {
				if (!ajaxPostForm(_('ethernet reboot confirm'), form, 'timerReloader', _("message config"), ajaxShowProgress)) {
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
					form.submit();
				}
				else 
					form.reboot.value = "1";
			}
			
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td><h1 id="ethernetTitle">Ethernet Port Settings</h1>
					<p id="ethernetIntroduction">You can set up ethernet port</p>
					<hr>
					<form name="setEthernetPort" method="POST" action="/goform/setEthernetPort">
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
						<div id="ethernetStatus"></div>
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 60%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" id="ethernetPortManagement">Port Management</td>
								</tr>
								<tr id="ethernetWANport_tr">
									<td class="head" id="ethernetWANport">WAN port</td>
									<td colspan="2" id="ethernetWANportSel">
										<select id="wan_port" name="wan_port" class="short">
											<option value="0">1</option>
											<option value="4">5</option>
										</select>
									</td>
								</tr>
								<tr id="ethernetFirstLANport_tr">
									<td class="head" id="ethernetFirstLANport">First LAN port</td>
									<td colspan="2" id="ethernetFirstLANportSel">
										<select id="lan_port" name="lan_port" class="mid">
											<option value="near" id="ethernetNearToWAN">Near to the WAN port</option>
											<option value="distant" id="ethernetDistantFromWAN">Distant from the WAN port</option>
										</select>
									</td>
								</tr>
								<tr id="ethernetPort1Mode_tr">
									<td class="head" id="ethernetPort1Mode">Port 1 mode</td>
									<td colspan="2">
										<select id="port1_swmode" name="port1_swmode" class="mid"></select>
									</td>
								</tr>
								<tr id="ethernetPort2Mode_tr">
									<td class="head" id="ethernetPort2Mode">Port 2 mode</td>
									<td colspan="2">
										<select id="port2_swmode" name="port2_swmode" class="mid"></select>
									</td>
								</tr>
								<tr id="ethernetPort3Mode_tr">
									<td class="head" id="ethernetPort3Mode">Port 3 mode</td>
									<td colspan="2">
										<select id="port3_swmode" name="port3_swmode" class="mid"></select>
									</td>
								</tr>
								<tr id="ethernetPort4Mode_tr">
									<td class="head" id="ethernetPort4Mode">Port 4 mode</td>
									<td colspan="2">
										<select id="port4_swmode" name="port4_swmode" class="mid"></select>
									</td>
								</tr>
								<tr id="ethernetPort5Mode_tr">
									<td class="head" id="ethernetPort5Mode">Port 5 mode</td>
									<td colspan="2">
										<select id="port5_swmode" name="port5_swmode" class="mid"></select>
									</td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="submit" class="mid" value="Apply"  id="ethernetApply"  onClick="checkValues(this.form);">&nbsp;&nbsp;
									<input type="button" class="mid" value="Cancel" id="ethernetCancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="mid" value="Reset"  id="ethernetReset"  onClick="resetValues(this.form);">
									<input type="hidden" name="reset" value="0">
									<input type="hidden" name="reboot" value="0">
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