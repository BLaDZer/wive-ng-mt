<!DOCTYPE html>
<html>
	<head>
		<title>Internet Services Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_services.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/json2.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			var dhcpList = [];
			var dhcp_interval;

			function initTranslation() {
				_TR("lTitle",			"services dhcp title");
				_TR("lIntroduction",		"services dhcp introduction");
				_TR("lSetup",			"services dhcp setup");
				_TR("lDhcpType",		"services dhcp title");
				_TR("lDhcpTypeD",		"button disable");
				_TR("lDhcpTypeS",		"button enable");
				_TR("dhcpDomain",		"services dhcp domain");
				_TR("lDhcpStart",		"services dhcp start");
				_TR("lDhcpEnd",			"services dhcp end");
				_TR("lDhcpNetmask",		"inet netmask");
				_TR("lDhcpPriDns",		"inet pri dns");
				_TR("lDhcpSecDns",		"inet sec dns");
				_TR("lDhcpGateway",		"inet gateway");
				_TR("lDhcpLease",		"services dhcp lease");
				_TR("lDhcpLeaseNote",		"services dhcp lease note");
				_TR("dClients",			"services dhcp clients");
				_TR("dHostname",		"inet hostname");
				_TR("dMAC",			"inet mac");
				_TR("dIP",			"inet ip");
				_TR("dExpires",			"services dhcp expires");
				_TR("dStatic",			"services dhcp static");
				_TR("lDhcpARPPTimeout",		"services dhcp arpping timeout");
				_TR("lDhcpARPPTimeoutNote",	"services dhcp arpping timeout note");
				_TR("lApply",			"button apply");
				_TR("lCancel",			"button cancel");
				_TR("lReset",			"button reset");
			}

			function initValues() {
				if (NVRAM_dhcpStatic.length > 1) {
					NVRAM_dhcpStatic = NVRAM_dhcpStatic.split(";");
					for (var i = 0; i < NVRAM_dhcpStatic.length; i++) {
						var row = NVRAM_dhcpStatic[i].split(" ");
						if (row.length < 3)
							row[2] = "";
						addEntry(row[0], row[1], row[2]);
					}
				}

				document.dhcpCfg.dhcpEnabled.options.selectedIndex	= +NVRAM_dhcpEnabled;
				document.dhcpCfg.dhcpDomain.value			= NVRAM_dhcpDomain;
				document.dhcpCfg.dhcpStart.value			= NVRAM_dhcpStart;
				document.dhcpCfg.dhcpEnd.value				= NVRAM_dhcpEnd;
				document.dhcpCfg.dhcpMask.value				= NVRAM_dhcpMask;
				document.dhcpCfg.dhcpPriDns.value			= NVRAM_dhcpPriDns;
				document.dhcpCfg.dhcpSecDns.value			= NVRAM_dhcpSecDns;
				document.dhcpCfg.dhcpGateway.value			= NVRAM_dhcpGateway;
				document.dhcpCfg.dhcpLease.value			= NVRAM_dhcpLease;
				document.dhcpCfg.dhcpARPPTimeout.value			= NVRAM_dhcpARPPTimeout;

				dhcpTypeSwitch();
				showWarning();
				initTranslation();
				loadDhcpClientsList();
				displayServiceStatus([[ NVRAM_dhcpEnabled, 'udhcpd', 'udhcpd' ]]);
				disableControlsByAuth();
			}

			function CheckValue() {
				var form = document.dhcpCfg;
				if (form.dhcpEnabled.options.selectedIndex == 1) {
					if ( /^[A-Za-z0-9_.@#=+ -]+$/.test(form.dhcpDomain.value) == false){
						alert(_("services dhcp invalid domain"));
						form.dhcpDomain.focus();
						form.dhcpDomain.select();
						return false;
					}

					if (!validateIP(form.dhcpStart)) {
						alert(_("services dhcp invalid ip"));
						form.dhcpStart.focus();
						form.dhcpStart.select();
						return false;
					}
					if (!validateIP(form.dhcpEnd)) {
						alert(_("services dhcp invalid ip"));
						form.dhcpEnd.focus();
						form.dhcpEnd.select();
						return false;
					}

					var startIp = ipaddr.IPv4.parse(form.dhcpStart.value);
					var endIp = ipaddr.IPv4.parse(form.dhcpEnd.value);
					if (startIp === null || endIp === null || startIp.toNumber() > endIp.toNumber()) {
						alert(_("services dhcp end ip less than start"));
						form.dhcpEnd.focus();
						form.dhcpEnd.select();
						return false;
					}

					if (!validateIP(form.dhcpMask)) {
						form.dhcpMask.focus();
						form.dhcpMask.select();
						return false;
					}
					if (form.dhcpPriDns.value != "") {
						if (!validateIP(form.dhcpPriDns)) {
							alert(_("services dhcp invalid ip"));
							form.dhcpPriDns.focus();
							form.dhcpPriDns.select();
							return false;
						}
					}
					if (form.dhcpSecDns.value != "") {
						if (!validateIP(form.dhcpSecDns)) {
							alert(_("services dhcp invalid ip"));
							form.dhcpSecDns.focus();
							form.dhcpSecDns.select();
							return false;
						}
					}
					if (!validateIP(form.dhcpGateway)) {
						alert(_("services dhcp invalid ip"));
						form.dhcpGateway.focus();
						form.dhcpGateway.select();
						return false;
					}
					if (!validateNum(form.dhcpLease.value) || +form.dhcpLease.value < 0) {
						alert(_("services dhcp invalid lease"));
						form.dhcpLease.focus();
						form.dhcpLease.select();
						return false;
					}
					if (!validateNum(form.dhcpARPPTimeout.value) || +form.dhcpARPPTimeout.value < 0) {
						alert(_("services dhcp invalid arptimeout"));
						form.dhcpARPPTimeout.focus();
						form.dhcpARPPTimeout.select();
						return false;
					}
					genIPTableData();
				}
				ajaxShowTimer(document.dhcpCfg, 'timerReloader', _('message apply'), 10);
				clearInterval(dhcp_interval);
				return true;
			}

			function loadDhcpClientsList() {
				ajaxLoadScript('/services/dhcp_clist.js');
				dhcp_interval = setTimeout('loadDhcpClientsList();', 5000);
			}

			function genTable(disabled) {
				disabled = (disabled) ? ' disabled="disabled"' : '';

				var table =	'<br>' +
						'<table class="form" style="width: 100%">' +
						'	<tr>' +
						'		<td class="title" colspan="4">' + _("services dhcp static title") + '</td>' +
						'	</tr>' +
						'	<tr>' +
						'		<th style="text-align: left;">' + _("inet mac") + '</th>' +
						'		<th style="text-align: left;">' + _("inet ip") + '</th>' +
						'		<th style="text-align: left;">' + _("services dhcp table desc") + '</th>' +
						'		<th>' + _("routing action") + '</th>' +
						'	</tr>';
				for (var i = 0; i < dhcpList.length; i++)
					table +=	'	<tr>' +
							'		<td>' + dhcpList[i][0] + '</td>' +
							'		<td>' + dhcpList[i][1] + '</td>' +
							'		<td>' + dhcpList[i][2] + '</td>' +
							'		<td style="text-align: center;">' +
							'			<a style="color: #ff0000;" title="' +	_("services dhcp edit record") + '" href="javascript:editIPItem(' + i + ');"' + disabled + '><img src="/graphics/edit.png" alt="[+]"></a>&nbsp;&nbsp;' +
							'			<a style="color: #ff0000;" title="' + _("services dhcp delete record") + '" href="javascript:deleteIPItem(' + i + ');"' + disabled + '><img src="/graphics/cross.png" alt="[x]"></a>' +
							'		</td>' +
							'	</tr>';
				table +=	'	<tr>' +
						'		<td><input style="width: 98%" value="" maxlength="17" name="dhcpStaticMAC"' + disabled + '></td>' +
						'		<td><input style="width: 98%" value="" maxlength="15" name="dhcpStaticIP"' + disabled + '></td>' +
						'		<td><input style="width: 98%" value="" maxlength="32" name="dhcpStaticDesc"' + disabled + '></td>' +
						'		<td style="text-align: center;"><input type="button" class="normal" title="' + _("services dhcp add record") + '" value="' + _("button add edit") + '" onclick="addIPItem();"' + disabled + '></td>' +
						'	</tr>' +
						'</table>';
				document.getElementById("dhcpStaticIPList").innerHTML = table;
			}

			function genIPTableData() {
				var values = '';
				for (var i = 0; i < dhcpList.length; i++) {
					values += dhcpList[i][0] + ' ' + dhcpList[i][1] + ' ' + dhcpList[i][2];
					if (dhcpList.length > (i + 1))
						values += ';';
				}
				document.dhcpCfg.dhcpAssignIP.value = values;
			}

			function addIPItem() {
				if (!validateMAC(document.dhcpCfg.dhcpStaticMAC.value)) {
					alert(_('services dhcp invalid mac'));
					document.dhcpCfg.dhcpStaticMAC.focus();
					document.dhcpCfg.dhcpStaticMAC.select();
					return;
				}
				if (!validateIP(document.dhcpCfg.dhcpStaticIP)) {
					alert(_('services dhcp invalid ip'));
					document.dhcpCfg.dhcpStaticIP.focus();
					document.dhcpCfg.dhcpStaticIP.select();
					return;
				}

				var re_desc = /^[a-zA-Z0-9_]+$/;
				if (!re_desc.test(document.dhcpCfg.dhcpStaticDesc.value)) {
					alert(_('services dhcp invalid desc'));
					document.dhcpCfg.dhcpStaticDesc.focus();
					document.dhcpCfg.dhcpStaticDesc.select();
					return;
				}
				addEntry(document.dhcpCfg.dhcpStaticMAC.value, document.dhcpCfg.dhcpStaticIP.value, document.dhcpCfg.dhcpStaticDesc.value);
			}

			function addEntry(mac, ip, desc) {
				var index = findEntry(mac, ip);
				if (index < 0) {
					dhcpList.push( [ mac, ip, desc ] );
					genTable();
				}
				else {
					if (dhcpList[index][0] == mac){
						if (confirm(_('services dhcp ask overwrite mac') + mac + '?')) {
							dhcpList[index][1] = ip;
							dhcpList[index][2] = desc;
							genTable();
						}
					}
					else if (dhcpList[index][1] == ip) {
						if (confirm(_('services dhcp ask overwrite ip') + ip + '?')) {
							dhcpList[index][0] = mac;
							dhcpList[index][2] = desc;
							genTable();
						}
					}
				}
			}

			function deleteIPItem(index) {
				if (index >= 0 && index < dhcpList.length) {
					var row = dhcpList[index];
					dhcpList.splice(index, 1);

					// Update DHCP table
					var tbl = document.getElementById('dhcpClientsTable');
					if (tbl != null)
						updateDhcpClientsList(tbl);
					genTable();
				}
			}

			function editIPItem(index) {
				if (index >= 0 && index < dhcpList.length) {
					var row = dhcpList[index];
					document.dhcpCfg.dhcpStaticMAC.value		= row[0];
					document.dhcpCfg.dhcpStaticIP.value		= row[1];
					document.dhcpCfg.dhcpStaticDesc.value		= row[2];
					document.dhcpCfg.dhcpStaticDesc.focus();
				}
			}

			function dhcpTypeSwitch() {
				var dhcp_on = document.dhcpCfg.dhcpEnabled.options.selectedIndex == 1;
				
				enableElements( [	document.dhcpCfg.dhcpDomain,
							document.dhcpCfg.dhcpStart,
							document.dhcpCfg.dhcpEnd,
							document.dhcpCfg.dhcpMask,
							document.dhcpCfg.dhcpGateway,
							document.dhcpCfg.dhcpLease,
							document.dhcpCfg.dhcpARPPTimeout ], dhcp_on);
				displayElement( [	'domain',
							'start',
							'end',
							'mask',
							'gateway',
							'lease',
							'dhcpClientsTable',
							'dhcpStaticIPList',
							'arpping' ], dhcp_on );
				enableElements( [	document.dhcpCfg.dhcpPriDns,
							document.dhcpCfg.dhcpSecDns ], dhcp_on && NVRAM_dnsPEnabled != '1');
				displayElement( [	'pridns',
							'secdns' ], dhcp_on && NVRAM_dnsPEnabled != '1');

				genTable(!dhcp_on);
			}

			// Update all checkboxes
			function updateDhcpClientsList(element) {
				var rows = element.getElementsByTagName('INPUT');
				for (var i = 0; i < rows.length; i++) {
					// Check elements
					if (rows[i].id == null || document.getElementById(rows[i].id + '_ip') == null || document.getElementById(rows[i].id + '_mac') == null)
						continue;
					// Set-up checked value
					var index = findEntry(document.getElementById(rows[i].id + '_mac').innerHTML, null);
					rows[i].checked = index >= 0;

					var status = document.getElementById(rows[i].id + '_status');
					if (status != null) {
						if (index >= 0)
							status.style.backgroundColor = (document.getElementById(rows[i].id + '_ip').innerHTML == dhcpList[index][1]) ? '#3da42c' : '#dd3b3b';
						else
							status.style.backgroundColor = 'inherit';
					}
				}
			}

			// Check if item not exists
			function findEntry(mac, ip) {
				for (var i = 0; i < dhcpList.length; i++)
					if (dhcpList[i][0] == mac || dhcpList[i][1] == ip)
						return i;
				return -1;
			}

			function toggleDhcpTable(check) {
				// Check values
				if (check.id == null || document.getElementById(check.id + '_ip') == null || document.getElementById(check.id + '_mac') == null)
					return;
				var ip		= document.getElementById(check.id + '_ip').innerHTML;
				var mac		= document.getElementById(check.id + '_mac').innerHTML;
				var host	= document.getElementById(check.id + '_host').innerHTML;
				
				// Check action
				if (check.checked) {								// Add item to list
					if (!validateMAC(mac)) {
						alert(_("services dhcp invalid mac"));
						return;
					}
					if (!validateIP(ip)) {
						alert(_("services dhcp invalid ip"));
						return;
					}
					addEntry(mac, ip, host + '_Static');
					updateDhcpClientsList(document.getElementById('dhcpClientsTable'));
				}
				else										// Remove item from list
					for (var i = 0; i < dhcpList.length; i++)
						if (dhcpList[i][0] == mac && dhcpList[i][1] == ip) {
							dhcpList.splice(i, 1);
							updateDhcpClientsList(document.getElementById('dhcpClientsTable'));
							genTable();
							return;
						}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="lTitle"></h1>
					<p id="lIntroduction"></p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="dhcpCfg" action="/goform/setDhcp" onSubmit="return CheckValue(this);">
					<table class="form auth-hide-user">
						<col style="width: 40%"/>
						<col style="width: 50%"/>
						<col style="width: 10%"/>
						<tbody>
							<tr>
								<td class="title" colspan="3" id="lSetup">DHCP Server Setup</td>
							</tr>
							<tr id="udhcpd">
								<td class="head" id="lDhcpType">DHCP Server</td>
								<td>
									<select name="dhcpEnabled" class="mid" onChange="dhcpTypeSwitch();">
										<option value="0" id="lDhcpTypeD">Disabled</option>
										<option value="1" id="lDhcpTypeS">Enabled</option>
									</select>
								</td>
								<td style="text-align: center">&nbsp;</td>
							</tr>
							<tr id="domain">
								<td class="head" id="dhcpDomain">DHCP Domain</td>
								<td colspan="2"><input name="dhcpDomain" class="mid"></td>
							</tr>
							<tr id="start">
								<td class="head" id="lDhcpStart">DHCP Pool Start IP</td>
								<td colspan="2"><input name="dhcpStart" class="mid"></td>
							</tr>
							<tr id="end">
								<td class="head" id="lDhcpEnd">DHCP Pool End IP</td>
								<td colspan="2"><input name="dhcpEnd" class="mid"></td>
							</tr>
							<tr id="mask">
								<td class="head" id="lDhcpNetmask">DHCP Subnet Mask</td>
								<td colspan="2"><input name="dhcpMask" class="mid"></td>
							</tr>
							<tr id="pridns">
								<td class="head" id="lDhcpPriDns">DHCP Primary DNS</td>
								<td colspan="2"><input name="dhcpPriDns" class="mid"></td>
							</tr>
							<tr id="secdns">
								<td class="head" id="lDhcpSecDns">DHCP Secondary DNS</td>
								<td colspan="2"><input name="dhcpSecDns" class="mid"></td>
							</tr>
							<tr id="gateway">
								<td class="head" id="lDhcpGateway">DHCP Default Gateway</td>
								<td colspan="2"><input name="dhcpGateway" class="mid"></td>
							</tr>
							<tr id="lease">
								<td class="head" id="lDhcpLease">DHCP Lease Time (in seconds)</td>
								<td colspan="2"><input name="dhcpLease" class="mid"><font color="#808080" id="lDhcpLeaseNote">(seconds, default: 86400)</font></td>
							</tr>
							<tr id="arpping">
								<td class="head" id="lDhcpARPPTimeout">DHCP ARP ping timeout (in ms)</td>
								<td colspan="2"><input name="dhcpARPPTimeout" class="mid"><font color="#808080" id="lDhcpARPPTimeoutNote">(seconds, default: 86400)</font></td>
							</tr>
						</tbody>
					</table>
					<div id="dhcpClientsTable"> </div>
					<div id="dhcpStaticIPList"> </div>
					<table class="buttons">
						<tr>
							<td>
								<input type="hidden" name="dhcpAssignIP" value="">
								<input type="hidden" name="reset" value="0">
								<input type="submit" class="normal" value="Apply" id="lApply"><input type="button" class="normal" value="Cancel" id="lCancel" onClick="window.location.reload();"><input type="button" class="normal" value="Reset"  id="lReset"  onClick="resetValues(this.form, 10);">
							</td>
						</tr>
					</table>
					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>