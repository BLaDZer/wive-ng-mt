<!DOCTYPE html>
<html>
	<head>
		<title>Internet Services Settings</title>
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
			Butterlate.setTextDomain("hint");
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			var users = [ <% getL2TPUserList(); %> ];		// Login/password list

			// Set translation
			function initTranslation()
			{
				_TR("l2tpServerTitle",			"services l2tp title");
				_TR("l2tpServerIntroduction",	"services l2tp introduction");
				_TR("l2tpServerSetup",			"services l2tp config");
				_TR("l2tpEnabled",				"services l2tp enable");
				_TR("l2tpDisable",				"button disable");
				_TR("l2tpEnable",				"button enable");
				_TR("l2tpLocalIP",				"services l2tp ip");
				_TR("l2tpListIP",				"services l2tp range ip");
				_TR("l2tpMTU",					"services l2tp mtu");
				_TR("l2tpMTUauto",				"services l2tp mtu auto");
				_TR("l2tpMTUcustom",			"services l2tp mtu custom");
				_TR("l2tpMRU",					"services l2tp mru");
				_TR("l2tpMRUauto",				"services l2tp mru auto");
				_TR("l2tpMRUcustom",			"services l2tp mru custom");
				_TR("l2tpAdditional",			"wan additional options");
				_TR("l2tpLCP",					"vpn adaptive lcp");
				_TR("l2tpDebug",				"vpn allow debug");
				_TR("l2tpNAT",					"vpn enable nat");
				_TR("l2tpProxyARP",				"services l2tp proxy arp");
				_TR("l2tpMPPE",					"vpn allow mppe");
				_TRV("l2tpApply",				"button apply");
				_TRV("l2tpCancel",				"button cancel");
				_TRV("l2tpReset",				"button reset");
			}

			// Set inintal values
			function initValues()
			{
				var i;

				if (users.length > 0)
					for (i = 0; i < users.length; i++)
						if (users[i][0] == "" || users[i][1] == "") {
							users.splice(i, 1);
							i--;
						}

				// Set variables
				document.getElementById('l2tp_srv_enabled').options.selectedIndex	= <% getCfgZero(1, "l2tp_srv_enabled"); %>;
				document.getElementById('l2tp_srv_ip_local').value					= '<% getCfgGeneral(1, "l2tp_srv_ip_local"); %>';
				document.getElementById('l2tp_srv_ip_range').value					= '<% getCfgGeneral(1, "l2tp_srv_ip_range"); %>';
				document.getElementById('l2tp_srv_mtu_size').value					= '<% getCfgGeneral(1, "l2tp_srv_mtu_size"); %>';
				document.getElementById('l2tp_srv_mru_size').value					= '<% getCfgGeneral(1, "l2tp_srv_mru_size"); %>';
				document.getElementById('l2tp_srv_lcp_adapt').checked				= <% getCfgZero(1, "l2tp_srv_lcp_adapt"); %>	== 1;
				document.getElementById('l2tp_srv_debug').checked					= <% getCfgZero(1, "l2tp_srv_debug"); %>		== 1;
				document.getElementById('l2tp_srv_nat_enabled').checked				= <% getCfgZero(1, "l2tp_srv_nat_enabled"); %>	== 1;
				document.getElementById('l2tp_srv_mppe_enabled').checked			= <% getCfgZero(1, "l2tp_srv_mppe_enabled"); %>	== 1;
				document.getElementById('l2tp_srv_proxyarp').checked				= <% getCfgZero(1, "l2tp_srv_proxyarp"); %>		== 1;

				// Set-up MTU & MRU
				for (i = 0; i < document.getElementById('l2tp_srv_mtu_sel').options.length; i++)
					if (document.getElementById('l2tp_srv_mtu_sel').options[i].value == document.getElementById('l2tp_srv_mtu_size').value)	{
						document.getElementById('l2tp_srv_mtu_sel').value = document.getElementById('l2tp_srv_mtu_sel').options[i].value;
						break;
					}
				
				for (i = 0; i < document.getElementById('l2tp_srv_mru_sel').options.length; i++)
					if (document.getElementById('l2tp_srv_mru_sel').options[i].value == document.getElementById('l2tp_srv_mru_size').value) {
						document.getElementById('l2tp_srv_mru_sel').value = document.getElementById('l2tp_srv_mru_sel').options[i].value;
						break;
					}

				// Enable/disable
				l2tpEnableSwitch();
				mtuChange();
				mruChange();
				displayServiceStatus();
				showWarning();
				initTranslation();
			}

			// Check values
			function CheckValues()
			{
				if (document.getElementById('l2tp_srv_enabled').options.selectedIndex != 0) {
					// Check Server IP
					if (!validateIP(document.getElementById('l2tp_srv_ip_local'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('l2tp_srv_ip_local').select();
						document.getElementById('l2tp_srv_ip_local').focus();
						return false;
					}
					
					// Check Client IP range
					var range	= document.getElementById('l2tp_srv_ip_range').value.replace(/ /g, '').split('-');
					var startIp	= range[0].split('.');
					var stopIp	= range[1].split('.');
					if (!validateIP(range[0]) || !validateIP(range[1]) ||
						!(startIp[0] <= stopIp[0] && startIp[1] <= stopIp[1] && 
						  startIp[2] <= stopIp[2] && startIp[3] <= stopIp[3] &&
						  range[0] != range[1])) {
						alert(_("services l2tp invalid ip range"));
						document.getElementById('l2tp_srv_ip_range').select();
						document.getElementById('l2tp_srv_ip_range').focus();
						return false;
					}

					// Check MTU
					if (document.getElementById('l2tp_srv_mtu_sel').value == '1') {
						if(!validateNum(document.getElementById('l2tp_srv_mtu_size').value)) {
							alert(_("services l2tp invalid mtu"));
							document.getElementById('l2tp_srv_mtu_size').select();
							document.getElementById('l2tp_srv_mtu_size').focus();
							return false;
						}
						document.getElementById('l2tp_srv_mtu_size').value = +document.getElementById('l2tp_srv_mtu_size').value;
					}

					// Check MRU
					if (document.getElementById('l2tp_srv_mru_sel').value == '1') {
						if(!validateNum(document.getElementById('l2tp_srv_mru_size').value)) {
							alert(_("services l2tp invalid mru"));
							document.getElementById('l2tp_srv_mru_size').select();
							document.getElementById('l2tp_srv_mru_size').focus();
							return false;
						}
						document.getElementById('l2tp_srv_mru_size').value = +document.getElementById('l2tp_srv_mru_size').value;
					}
				}
				ajaxShowTimer(document.l2tpConfig, 'timerReloader', _('message apply'), 15);
				return true;
			}

			// Hide Hiht block
			function hideHint()
			{
				document.getElementById('l2tp_hint_row').innerHTML = '';
			}

			// Show Hint block
			function showHint(key)
			{
				var text = '<div class="hint"><font color="#0000ff"><b>HINT:</b></font>&nbsp;';
				var show = true;
				
				switch (key) {
					case 'l2tp_enable': 	text += _("hint l2tp server");		break;
					case 'l2tp_ip':			text += _("hint vpn l2tp server");	break;
					case 'l2tp_ip_list':	text += _("hint vpn range");		break;
					case 'l2tp_mtu':		text += _("hint vpn mtu");			break;
					case 'l2tp_mru':		text += _("hint vpn mtu");			break;
					case 'l2tp_nat':		text += _("hint vpn nat");			break;
					case 'l2tp_mppe':		text += _("hint vpn mppe");			break;
					case 'l2tp_debug':		text += _("hint vpn debug");		break;
					case 'l2tp_lcp':		text += _("hint vpn lcp");			break;
					case 'l2tp_proxyarp':	text += _("hint l2tp proxyarp");	break;
					default:				return;
				}

				text += '</div>';
				document.getElementById('l2tp_hint_row').innerHTML = text;
			}

			function genTable()
			{
				var table, i;
				
				table  = '<table class="form">';
				table += '<tr><td class="title" colspan="3">' + _("services l2tp users") + '</td></tr>';
				table += '<tr><th style="width: 40%;">' + _("services l2tp login") + '</th><th style="width: 40%;">' + _("services l2tp password") + '</th><th>' + _("routing action") + '</th></tr>';

				for (i = 0; i < users.length; i++) {
					table += '<tr><td style="padding-left: 20px;">' + users[i][0] + '<input name="l2tp_srv_user' + i + '" type="hidden" value="' + users[i][0] + '"></td>';
					table += '<td style="padding-left: 20px;">xxxx<input name="l2tp_srv_pass' + i + '" type="hidden" value="' + users[i][1] + '"></td>';
					table += '<td style="text-align: center;">';
					table += '<img src="/graphics/edit.png" title="' + _("services dhcp edit record") + '" onClick="editUser(' + i + ');"> ';
					table += '<img src="/graphics/cross.png" title="' + _("services dhcp delete record") + '" onClick="deleteUser(' + i + ');">';
					table += '</td></tr>';
				}

				table += '<tr><td><input type="text" id="l2tpLogin" maxlength="32" value="" style="width: 99%"></td>';
				table += '<td><input type="password" id="l2tpPassword" maxlength="32" value="" style="width: 99%"></td>';
				table += '<td style="text-align: center;">';
				table += '<input type="button" class="normal" title="' + _("services dhcp add record") + '" value="' + _("button add edit") + '" onclick="addUser(this.form);">';
				table += '<input type="hidden" id="l2tpEdit" value="-1">';
				table += '<input type="hidden" name="l2tp_srv_user_num" value="' + users.length + '">';
				table += '</td></tr>';
				table += '</table>';
				
				document.getElementById("l2tpUserList").innerHTML = table;
			}

			// Add user/password
			function addUser()
			{
				var index	= document.getElementById('l2tpEdit').value;
				var login	= document.getElementById('l2tpLogin').value;
				var pass	= document.getElementById('l2tpPassword').value;

				// Check login
				var re_login = /^[a-zA-Z0-9_]+$/;
				if (!re_login.test(login)) {
					alert(_("services l2tp uncorrect login"));
					document.getElementById('l2tpLogin').select();
					document.getElementById('l2tpLogin').focus();
					return false;
				}

				// Check login exist
				for (var i = 0; i < users.length; i++) {
					if (login == users[i][0] && index != i) {
						alert(_("services l2tp exist login"));
						document.getElementById('l2tpLogin').select();
						document.getElementById('l2tpLogin').focus();
						return false;
					}
				}

				// Check pass
				var re_pass = /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
				if (!re_pass.test(pass)) {
					alert(_("services l2tp uncorrect password"));
					document.getElementById('l2tpPassword').select();
					document.getElementById('l2tpPassword').focus();
					return false;
				}	
				
				if (index == -1)
					users.push([ login, pass ]);
				else {
					var index = document.getElementById('l2tpEdit').value;
					users[index][0] = document.getElementById('l2tpLogin').value;
					users[index][1] = document.getElementById('l2tpPassword').value;
				}
				genTable();
			}

			// Delete user/password
			function deleteUser(index)
			{
				users.splice(index, 1);
				genTable();
			}

			// Edit user/password
			function editUser(index)
			{
				document.getElementById('l2tpLogin').value		= users[index][0];
				document.getElementById('l2tpPassword').value	= users[index][1];
				document.getElementById('l2tpEdit').value		= index;
			}

			// Show/hide L2TP settings
			function l2tpEnableSwitch()
			{
				displayElement([ 'l2tp_ip',
								 'l2tp_ip_list',
								 'l2tp_mtu',
								 'l2tp_mru',
								 'l2tp_additional',
								 'l2tpUserList' ],		document.getElementById('l2tp_srv_enabled').options.selectedIndex != 0);
				genTable();
			}

			// Change MTU
			function mtuChange()
			{
				if (document.getElementById('l2tp_srv_mtu_sel').value == '1') {
					document.getElementById('l2tp_srv_mtu_size').style.display = '';
					document.getElementById('l2tp_srv_mtu_sel').setAttribute("class", "half");
					if (document.getElementById('l2tp_srv_mtu_size').value == 'AUTO')
						document.getElementById('l2tp_srv_mtu_size').value = '1460';
					document.getElementById('l2tp_srv_mtu_size').focus();
				}
				else {
					document.getElementById('l2tp_srv_mtu_size').style.display = 'none';
					document.getElementById('l2tp_srv_mtu_sel').setAttribute("class", "mid");
					document.getElementById('l2tp_srv_mtu_size').value = document.getElementById('l2tp_srv_mtu_sel').value;
				}
			}

			// Change MRU
			function mruChange()
			{
				if (document.getElementById('l2tp_srv_mru_sel').value == '1') {
					document.getElementById('l2tp_srv_mru_size').style.display = '';
					document.getElementById('l2tp_srv_mru_sel').setAttribute("class", "half");
					if (document.getElementById('l2tp_srv_mru_size').value == 'AUTO')
						document.getElementById('l2tp_srv_mru_size').value = '1460';
					document.getElementById('l2tp_srv_mru_size').focus();
				}
				else {
					document.getElementById('l2tp_srv_mru_size').style.display = 'none';
					document.getElementById('l2tp_srv_mru_sel').setAttribute("class", "mid");
					document.getElementById('l2tp_srv_mru_size').value = document.getElementById('l2tp_srv_mru_sel').value;
				}
			}

			// Display server status
			function displayServiceHandler(response)
			{
				var i, j;
				var services = [
					// turned_on, row_id, daemon_id
					[ '<% getCfgGeneral(1, "l2tp_srv_enabled"); %>', 'xl2tpd', 'xl2tpd-srv' ]
				];

				// Create associative array
				var tmp = response.split(',');
				var daemons = [];
				for (i = 0; i < tmp.length; i++)
					daemons[tmp[i]] = 1;

				// Now display all services
				for (i = 0; i < services.length; i++) {
					var service = services[i];
					var row = document.getElementById(service[1]);
					var tds = [];
					for (j = 0; j < row.childNodes.length; j++)
						if (row.childNodes[j].nodeName == 'TD')
							tds.push(row.childNodes[j]);

					if (row != null) {
						// Fill-up status
						if (service[0] == '0')
							tds[2].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
						else
							tds[2].innerHTML = (daemons[service[2]] == 1) ?
								'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
								'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
					}
				}

				serviceStatusTimer = setTimeout('displayServiceStatus();', 5000);
			}

			// Get server status
			function displayServiceStatus()
			{
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"></tr>
			<tr>
				<td><h1 id="l2tpServerTitle">L2TP Server Setup</h1>
					<p id="l2tpServerIntroduction">Here you can configure L2TP server settings.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="l2tpConfig" action="/goform/l2tpConfig">
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" id="l2tpServerSetup">L2TP Server Setup</td>
								</tr>
								<tr id="xl2tpd" onMouseOver="showHint('l2tp_enable')" onMouseOut="hideHint('l2tp_enable')">
									<td class="head" id="l2tpEnabled">L2TP server</td>
									<td>
										<select id="l2tp_srv_enabled" name="l2tp_srv_enabled" class="mid" onChange="l2tpEnableSwitch();">
											<option value="0" id="l2tpDisable">Disable</option>
											<option value="1" id="l2tpEnable">Enable</option>
										</select>
									</td>
									<td style="text-align: center;">&nbsp;</td>
								</tr>
								<tr onMouseOver="showHint('l2tp_ip')" onMouseOut="hideHint('l2tp_ip')" id="l2tp_ip">
									<td class="head" id="l2tpLocalIP">Our local IP to use</td>
									<td colspan="2">
										<input id="l2tp_srv_ip_local" name="l2tp_srv_ip_local" class="mid" maxlength="15">
									</td>
								</tr>
								<tr onMouseOver="showHint('l2tp_ip_list')" onMouseOut="hideHint('l2tp_ip_list')" id="l2tp_ip_list">
									<td class="head" id="l2tpListIP">Allocate from this IP range</td>
									<td colspan="2">
										<input id="l2tp_srv_ip_range" name="l2tp_srv_ip_range" class="mid" maxlength="33">
									</td>
								</tr>
								<tr onMouseOver="showHint('l2tp_mtu')" onMouseOut="hideHint('l2tp_mtu')" id="l2tp_mtu">
									<td class="head" id="l2tpMTU">Tunnel <acronym title="Maximum Transfer Unit">MTU</acronym>:</td>
									<td colspan="2">
										<input id="l2tp_srv_mtu_size" name="l2tp_srv_mtu_size" type="text" style="display:none; width: 89px;" maxlength="4">
										<select id="l2tp_srv_mtu_sel" onChange="mtuChange(this.form);" class="mid" >
											<option value="AUTO" id="l2tpMTUauto">AUTO</option>
											<option value="1" id="l2tpMTUcustom" selected="selected">Custom</option>
											<option value="1460">1460</option>
											<option value="1440">1440</option>
											<option value="1400">1400</option>
											<option value="1300">1300</option>
											<option value="1200">1200</option>
											<option value="1100">1100</option>
											<option value="1000">1000</option>
										</select>
									</td>
								</tr>
								<tr onMouseOver="showHint('l2tp_mru')" onMouseOut="hideHint('l2tp_mru')" id="l2tp_mru">
									<td class="head" id="l2tpMRU">Tunnel <acronym title="Maximum Receive Unit">MRU</acronym>:</td>
									<td colspan="2">
										<input id="l2tp_srv_mru_size" name="l2tp_srv_mru_size" type="text" style="display:none; width: 89px;" maxlength="4">
										<select id="l2tp_srv_mru_sel" onChange="mruChange(this.form);" class="mid" >
											<option value="AUTO" id="l2tpMRUauto">AUTO</option>
											<option value="1" id="l2tpMRUcustom" selected="selected">Custom</option>
											<option value="1460">1460</option>
											<option value="1440">1440</option>
											<option value="1400">1400</option>
											<option value="1300">1300</option>
											<option value="1200">1200</option>
											<option value="1100">1100</option>
											<option value="1000">1000</option>
										</select>
									</td>
								</tr>
							</tbody>
						</table>
						<table class="form" id="l2tp_additional">
							<col style="width: 50%;" />
							<col style="width: 50%;" />
							<tbody>
								<tr>
									<td class="title" colspan="2" id="l2tpAdditional">Additional options</td>
								</tr>
								<tr>
									<td onMouseOver="showHint('l2tp_lcp')" onMouseOut="hideHint()">
										<input id="l2tp_srv_lcp_adapt" name="l2tp_srv_lcp_adapt" type="checkbox">&nbsp;<b id="l2tpLCP">Adaptive LCP</b>
									</td>
									<td onMouseOver="showHint('l2tp_debug')" onMouseOut="hideHint()">
										<input id="l2tp_srv_debug" name="l2tp_srv_debug" type="checkbox">&nbsp;<b id="l2tpDebug">L2TP debugging</b>
									</td>
								</tr>
								<tr>
									<td onMouseOver="showHint('l2tp_nat')" onMouseOut="hideHint()">
										<input id="l2tp_srv_nat_enabled" name="l2tp_srv_nat_enabled" type="checkbox">&nbsp;<b id="l2tpNAT">Enable NAT</b>
									</td>
									<td onMouseOver="showHint('l2tp_proxyarp')" onMouseOut="hideHint()">
										<input id="l2tp_srv_proxyarp" name="l2tp_srv_proxyarp" type="checkbox">&nbsp;<b id="l2tpProxyARP">Proxyarp</b>
									</td>
								</tr>
								<tr>
									<td onmouseover="showHint('l2tp_mppe')" onmouseout="hideHint()">
										<input id="l2tp_srv_mppe_enabled" name="l2tp_srv_mppe_enabled" type="checkbox">&nbsp;<b id="l2tpMPPE">Require MPPE</b>
									</td>
									<td>
									</td>
								</tr>
							</tbody>
						</table>
						<div id="l2tpUserList"> </div>
						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" id="l2tpApply"  value="Apply"  onClick="return CheckValues();">&nbsp;&nbsp;
									<input type="button" class="normal" id="l2tpCancel" value="Cancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="normal" id="l2tpReset"  value="Reset"  onClick="resetValues(this.form);">
									<input value="0" name="reset" type="hidden">
								</td>
							</tr>
						</table>
					</form>
					<div id="l2tp_hint_row">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
