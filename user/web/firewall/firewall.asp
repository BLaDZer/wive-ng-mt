<!DOCTYPE html>
<html>
	<head>
		<title>Port Forwarding Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_firewall.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script>
			var MAX_RULES					= 32;
			var portForwardingRules			= [ <% getPortForwardRules(); %> ];
			var portFilteringRules			= [ <% getPortFilteringRules(); %> ];
			var portFilteringInputRules		= [ <% getPortFilteringInputRules(); %> ];
			var defaultFilterPolicy			= '<% getCfgZero(1, "DefaultFirewallPolicy"); %>';

			function initValues() {
				document.getElementById('portForwardEnabled').value			= NVRAM_PortForwardEnable;
				document.getElementById('portFilterEnabled').value			= NVRAM_IPPortFilterEnable;
				document.getElementById('portFilterInputEnabled').value		= NVRAM_IPPortFilterInputEnable;
				document.getElementById('ForwardSesLimit').value			= NVRAM_ForwardSesLimit;

				updateForwardingState(document.portForward);
				updateFilteringState(document.portFiltering);
				updateFilteringInputState(document.portFilteringInput);

				// DMZ
				document.getElementById('DMZEnabled').options.selectedIndex = +NVRAM_DMZEnable;
				document.getElementById('dmzLoopback').value = NVRAM_DMZNATLoopback;
				document.getElementById('DMZIPAddress').value = NVRAM_DMZIPAddress;

				dmzEnableSwitch();

				showWarning();
				init_translation_model();
			}

			function checkValuesDMZ(form) {
				if (document.getElementById('DMZEnabled').options.selectedIndex && !validateIP(document.getElementById('DMZIPAddress'), true)) {
					document.getElementById('DMZIPAddress').focus();
					return false;
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function dmzEnableSwitch() {
				enableElements([document.getElementById('DMZIPAddress'), document.getElementById('dmzLoopback')], document.getElementById('DMZEnabled').value == '1');
				displayElement([ 'dmzAdress', 'dmzLoopback_tr' ], document.getElementById('DMZEnabled').value == '1');
			}

			function dmzLoopbackWarning() {
				if (document.getElementById('dmzLoopback').value == '1') {
					if (!confirm(_("dmz confirm")))
						document.getElementById('dmzLoopback').value = '0';
				}
			}

			function rulesHaveValueRow(rules, value, row_num) {
				for (var i=0;i<rules.length;i++) {
					var row = rules[i];
					if (row.length > row_num && row[row_num] == value) return true;
				}

				return false;
			}

			function submitForwardForm(form) {
				form.portForwardRules.value = genTableData(portForwardingRules, form);
				if (form.portForwardEnabled.value != "0" && NVRAM_vpnDGW != "0" && NVRAM_vpnEnabled != "off" && rulesHaveValueRow(portForwardingRules, "VPN", 0) && confirm(_("firewall restart vpn"))) {
                                    form.vpn_restart.value = "1";
				}

				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function submitFilterForm(form) {
				form.portFilteringRules.value = genTableData(portFilteringRules, form);
				form.defaultFirewallPolicy.value = defaultFilterPolicy;

				if (form.portFilterEnabled.value != "0" && NVRAM_vpnDGW != "0" && NVRAM_vpnEnabled != "off" && rulesHaveValueRow(portFilteringRules, "VPN", 0) && confirm(_("firewall restart vpn"))) {
                                    form.vpn_restart.value = "1";
				}

				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function submitFilterInputForm(form) {
				form.portFilteringInputRules.value = genTableData(portFilteringInputRules, form);

				if (form.portFilterInputEnabled != "0" && NVRAM_vpnDGW != "0" && NVRAM_vpnEnabled != "off" && rulesHaveValueRow(portFilteringInputRules, "VPN", 0) && confirm(_("firewall restart vpn"))) {
                                    form.vpn_restart.value = "1";
				}

				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function submitFirewallForm(form) {
				form.ForwardSesLimit.value = defaultNumber(form.ForwardSesLimit.value, "0");
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}
			
			function showPortRange(from, to) {
				if (from == '')
					return '*';
				return (to != '') ? from + '<br>-<br>' + to : from;
			}

			function showProtocol(proto) {
				if (proto == '1')
					return 'TCP';
				if (proto == '2')
					return 'UDP';
				if (proto == '3')
					return 'TCP&amp;UDP';
				if (proto == '4')
					return 'ICMP';
				if (proto == '5')
					return '*';
				return 'unknown';
			}

			function showValue(value) {
				return (value != '') ? value : '*';
			}

			function showPolicy(policy) {
				var color = (policy == '1') ? '#3da42c' : '#dd3b3b';
				var text  = (policy == '1') ? _("port filter action accept") : _("port filter action drop");
				return '<td rowspan="2" style="color: ' + color + ';">' + text + '</td>';
			}

			function checkComment(element) {
				var comment = element.value;
				return comment.replace(/[,;'"]/g, ' '); //'// mc highlighting fix
			}

			function checkIPMask(ip, mask) {
				if (ip.value != '') {
					// First validate IP
					if (!validateIP(ip, true))
						return false;

					// Now validate mask if present
					if (mask.value != '')
						if (!validateIPMask(mask, true))
							return false;
				}
				else if (mask.value != '') {
					alert(_("forward ip not mask") + mask.value);
					ip.focus();
					return false;
				}

				return true;
			}
			
			function genRulesTable() {
				var disabled = (portForwardingRules.length >= MAX_RULES) ? ' disabled="disabled"' : '';

				var table = '<table class="small" style="width: 100%"><tr>';
				table += '<th>' + _("forward interface") + '</th>';
				table += '<th>' + _("forward protocol") + '</th>';
				table += '<th>' + _("forward src ports") + '</th>';
				table += '<th>' + _("forward dst ip") + '</th>';
				table += '<th>' + _("forward dst ports") + '</th>';
				table += '<th>' + _("forward nat") + '</th>';
				table += '<th>' + _("forward comment") + '</th>';
				table += '<th>' + _("forward action") + '</th></tr>';

				for (var i = 0; i < portForwardingRules.length; i++) {
					var row = portForwardingRules[i];
					table += '<tr>' + '<td>' + row[0] + '</td>' + // Interface
						'<td>' + showProtocol(row[1]) + '</td>' + // Protocol
						'<td>' + showPortRange(row[2], row[3]) + '</td>' + // Source port range
						'<td>' + row[4] + '</td>' + // Destination IP
						'<td>' + showPortRange(row[5], row[6]) + '</td>' + // Destination port range
						'<td><input type="checkbox"' + ((row[7] != 0) ? ' checked="true">' : '>') + // NAT Loopback
						'<td style="max-width: 120px;white-space: wrap; overflow: hidden;">' + row[8] + '&nbsp;</td>' + // Comment
						'<td style="text-align: center;">' +
						'<a style="color: #ff0000;" title="' + _("forward delete record") + '" href="javascript:deleteRuleItem(' + i + ');"' + disabled + '><img src="/graphics/cross.png" alt="[x]"></a></td>' +
						'</tr>';
				}

				if (portForwardingRules.length <= 0)
					table += '<tr><td colspan="8" style="text-align: left;">' + _("forward no rules") + '</td></tr>';

				// Controls
				table +=
					'<tr>'+
					'<td rowspan="2"><select class="pfSmall" name="fwdIface" tabindex="1"><option value="WAN" selected="selected">WAN</option><option value="VPN">VPN</option></select></td>' +
					'<td rowspan="2"><select class="pfSmall" name="protocol" tabindex="2"><option value="3" selected="selected">TCP&amp;UDP</option><option value="1">TCP</option><option value="2">UDP</option></select></td>' +
					'<td><input type="text" class="pfShort" maxlength="5" name="fromPort" tabindex="3" ></td>' + 
					'<td rowspan="2"><input class="pfSmall" type="text" class="pfNormal" maxlength="15" name="ip_address" tabindex="5"></td>' +
					'<td><input type="text" class="pfShort" maxlength="5" name="redirectFromPort" tabindex="6"></td>' + 
					'<td rowspan="2"><input type="checkbox" name="natLoopback" tabindex="8"></td>' + 
					'<td rowspan="2"><input class="pfSmall" type="text" style="width: 95%;" maxlength="25" name="comment" tabindex="9"></td>' +
					'<td rowspan="2" style="text-align: center;"><input class="pfSmall" type="button" title="' + _("forward add record") + '" value="' + _("button add") + '" tabindex="10" onclick="addRuleItem(this.form);"' + disabled + '></td>' +
					'</tr>' +
					'<tr>' +
					'<td><input type="text" class="pfShort" maxlength="5" name="toPort" tabindex="4"></td>' + 
					'<td><input type="text" class="pfShort" maxlength="5" name="redirectToPort" tabindex="7"></td>' + 
					'</tr>';

				// Close manager
				table += '</table>';

				var elem = document.getElementById("portForwardingTable");
				if (elem != null)
					elem.innerHTML = table;
			}

			function genFilteringTable() {
				var disabled = (portFilteringRules.length >= MAX_RULES) ? ' disabled="disabled"' : '';

				var table = '<table class="small" style="width: 100%;"><tr>';
				table += '<th>' + _("forward interface") + '</th>';
				table += '<th>' + _("forward protocol") + '</th>';
				table += '<th>' + _("forward src ip") + '</th>';
				table += '<th rowspan="2" style="white-space: normal;">' + _("forward src ports") + '</th>';
				table += '<th>' + _("forward dst ip") + '</th>';
				table += '<th rowspan="2" style="white-space: normal;">' + _("forward dst ports") + '</th>';
				table += '<th rowspan="2">' + _("forward policy") + '</th>';
				table += '<th rowspan="2">' + _("forward comment") + '</th>';
				table += '<th rowspan="2">' + _("forward action") + '</th>';
				table += '</tr><tr>';
				table += '<th colspan="2">' + _("forward mac") + '</th>';
				table += '<th>' + _("forward mask") + '</th>';
				table += '<th>' + _("forward mask") + '</th>';
				table += '</tr>';

				for (var i = 0; i < portFilteringRules.length; i++) {
					var row = portFilteringRules[i];

					table +=
						'<tr>' +
						'<td>' + row[0] + '</td>' + // Interface
						'<td>' + showProtocol(row[1]) + '</td>' + // Protocol
						'<td>' + showValue(row[3]) + '</td>' + // Source IP
						'<td rowspan="2" style="white-space: normal;">' + showPortRange(row[5], row[6]) + '</td>' + // Source port range
						'<td>' + showValue(row[7]) + '</td>' + // Destination IP
						'<td rowspan="2" style="white-space: normal;">' + showPortRange(row[9], row[10]) + '</td>' + // Destination port range
						showPolicy(row[11]) + // Policy
						'<td rowspan="2" style="max-width: 90px; white-space: wrap; overflow: hidden;">' + row[12] + '&nbsp;</td>' + // Comment
						'<td rowspan="2" style="text-align: center;"><a style="color: #ff0000;" title="' + _("forward delete record") + '" href="javascript:deleteForwardingItem(' + i + ');"' + disabled + '><img src="/graphics/cross.png" alt="[x]"></a></td>' +
						'</tr><tr>' +
						'<td colspan="2">' + showValue(row[2]) + '</td>' + // MAC
						'<td>' + showValue(row[4]) + '</td>' + // Source IP Mask
						'<td>' + showValue(row[8]) + '</td>' + // Destination IP Mask
						'</tr>';
				}

				if (portFilteringRules.length <= 0)
					table += '<tr><td colspan="9" style="text-align: left;">' + _("forward no filter rules") + '</td></tr>';

				var accept_sel = (defaultFilterPolicy == '0') ? ' selected="selected"' : '';
				var drop_sel = (defaultFilterPolicy != '0') ? ' selected="selected"' : '';
				table +=
					'<tr><td colspan="7" style="text-align:left;">' + _("port basic default policy") + '</td>' +
					'<td colspan="2" style="text-align: right;"><select name="defaultFilteringPolicy" onchange="javascript:changedefaultFilterPolicy(this.form);"><option value="0"' + accept_sel +'>' + _("port basic default policy accepted") + '</option><option value="1"' + drop_sel +'>' + _("port basic default policy dropped") + '</option></select></td></tr>';

				// Controls
				table +=
					'<tr>'+
					'<td><select class="pfSmall" name="interface" tabindex="10"><option value="LAN">LAN</option><option value="WAN" selected="selected">WAN</option><option value="VPN">VPN</option></select></td>' +
					'<td><select class="pfSmall" name="protocol" tabindex="11" onchange="javascript:protocolChange(this.form);"><option value="5">None</option><option value="1">TCP</option><option value="2">UDP</option><option value="4">ICMP</option></select></td>' +
					'<td><input type="text" tabindex="13" class="pfNormal" maxlength="15" name="sip_address"></td>' +
					'<td><input type="text" tabindex="15" class="pfShort" maxlength="5" name="sFromPort" disabled="disabled"></td>' +
					'<td><input type="text" tabindex="17" class="pfNormal" maxlength="15" name="dip_address"></td>' +
					'<td><input type="text" tabindex="19" class="pfShort" maxlength="5" name="dFromPort" disabled="disabled"></td>' +
					'<td rowspan="2"><select class="pfSmall" style="width: 80px;" tabindex="21" name="policy"><option value="0"' + accept_sel + '>' + _("port filter action drop") + '</option><option value="1"' + drop_sel + '>' + _("port filter action accept") + '</option></select></td>' +
					'<td rowspan="2"><input class="pfSmall" tabindex="22" type="text" style="width: 95%;" maxlength="18" name="comment"></td>' +
					'<td rowspan="2" style="text-align: center;"><input class="pfSmall" type="button" tabindex="23" title="' + _("forward add record") + '" value="' + _("button add") + '" onclick="addFilteringItem(this.form);"' + disabled + '></td>' +
					'</tr>' +
					'<tr>' +
					'<td colspan="2"><input class="pfSmall" type="text" tabindex="12" style="width: 95%" maxlength="17" name="mac_address"></td>' +
					'<td><input type="text" tabindex="14" class="pfNormal" maxlength="15" name="sip_mask"></td>' +
					'<td><input type="text" tabindex="16" class="pfShort" maxlength="5" name="sToPort" disabled="disabled"></td>' +
					'<td><input type="text" tabindex="18" class="pfNormal" maxlength="15" name="dip_mask"></td>' +
					'<td><input type="text" tabindex="20" class="pfShort" maxlength="5" name="dToPort" disabled="disabled"></td>' +
					'</tr>';

				// Close manager
				table += '</table>';

				var elem = document.getElementById("portFilteringTable");
				if (elem!=null)
					elem.innerHTML = table;
			}

			function genFilteringInputTable() {
				var disabled = (portFilteringInputRules.length >= MAX_RULES) ? ' disabled="disabled"' : '';

				var table = '<table class="small" style="width: 100%;"><tr>';
				table += '<th>' + _("forward interface") + '</th>';
				table += '<th>' + _("forward protocol") + '</th>';
				table += '<th>' + _("forward src ip") + '</th>';
				table += '<th rowspan="2" style="white-space: normal;">' + _("forward src ports") + '</th>';
				table += '<th rowspan="2" style="white-space: normal;">' + _("forward dst ports") + '</th>';
				table += '<th rowspan="2">' + _("forward comment") + '</th>';
				table += '<th rowspan="2">' + _("forward action") + '</th>';
				table += '</tr><tr>';
				table += '<th colspan="2">' + _("forward mac") + '</th>';
				table += '<th>' + _("forward mask") + '</th>';
				table += '</tr>';

				for (var i = 0; i < portFilteringInputRules.length; i++) {
					var row = portFilteringInputRules[i];

					table +=
						'<tr>' +
						'<td>' + row[0] + '</td>' + // Interface
						'<td>' + showProtocol(row[1]) + '</td>' + // Protocol
						'<td>' + showValue(row[3]) + '</td>' + // Source IP
						'<td rowspan="2" style="white-space: normal;">' + showPortRange(row[5], row[6]) + '</td>' + // Source port range
						'<td rowspan="2" style="white-space: normal;">' + showPortRange(row[9], row[10]) + '</td>' + // Destination port range
						'<td rowspan="2" style="max-width: 90px; white-space: wrap; overflow: hidden;">' + row[12] + '&nbsp;</td>' + // Comment
						'<td rowspan="2" style="text-align: center;"><a style="color: #ff0000;" title="' + _("forward delete record") + '" href="javascript:deleteForwardingInputItem(' + i + ');"' + disabled + '><img src="/graphics/cross.png" alt="[x]"></a></td>' +
						'</tr><tr>' +
						'<td colspan="2">' + showValue(row[2]) + '</td>' + // MAC
						'<td>' + showValue(row[4]) + '</td>' + // Source IP Mask
						'</tr>';
				}

				if (portFilteringInputRules.length <= 0)
					table += '<tr><td colspan="7" style="text-align: left;">' + _("forward no filter rules") + '</td></tr>';

				// Controls
				table +=
					'<tr>'+
					'<td><select name="interface" tabindex="30" style="width: 95%"><option value="LAN">LAN</option><option value="WAN" selected="selected">WAN</option><option value="VPN">VPN</option></select></td>' +
					'<td><select name="protocol" tabindex="31" style="width: 95%" onchange="protocolChange(this.form);"><option value="5">None</option><option value="1">TCP</option><option value="2">UDP</option><option value="4">ICMP</option></select></td>' +
					'<td><input type="text" tabindex="33" maxlength="15" name="sip_address" class="normal"></td>' +
					'<td><input type="text" tabindex="35" maxlength="5" name="sFromPort" disabled="disabled" class="short"></td>' +
					'<td><input type="text" tabindex="37" maxlength="5" name="dFromPort" disabled="disabled" class="short"></td>' +
					'<td rowspan="2"><input tabindex="39" type="text" style="width: 95%;" maxlength="18" name="comment" style="width: 95%"></td>' +
					'<td rowspan="2" style="text-align: center;"><input type="button" tabindex="40" title="' + _("forward add record") + '" value="' + _("button add") + '" onclick="addFilteringInputItem(this.form);"' + disabled + '></td>' +
					'</tr>' +
					'<tr>' +
					'<td colspan="2"><input type="text" tabindex="32" style="width: 95%" maxlength="17" name="mac_address"></td>' +
					'<td><input type="text" tabindex="34" maxlength="15" name="sip_mask" class="normal"></td>' +
					'<td><input type="text" tabindex="36" maxlength="5" name="sToPort" disabled="disabled" class="short"></td>' +
					'<td><input type="text" tabindex="38" maxlength="5" name="dToPort" disabled="disabled" class="short"></td>' +
					'</tr>';

				// Close manager
				table += '</table>';

				var elem = document.getElementById("portFilteringInputTable");
				if (elem!=null)
					elem.innerHTML = table;
			}

			function changedefaultFilterPolicy(form) {
				defaultFilterPolicy = form.defaultFilteringPolicy.value;
			}

			function protocolChange(form) {
				var dis = !((form.protocol.value == '1') || (form.protocol.value == '2'));
				form.sFromPort.disabled = dis;
				form.sToPort.disabled   = dis;
				form.dFromPort.disabled = dis;
				form.dToPort.disabled   = dis;
			}

			function deleteRuleItem(index) {
				if (index >= 0 && index < portForwardingRules.length) {
					portForwardingRules.splice(index, 1);
					genRulesTable();
				}
			}

			function deleteForwardingItem(index) {
				if (index >= 0 && index < portFilteringRules.length) {
					portFilteringRules.splice(index, 1);
					genFilteringTable();
				}
			}

			function deleteForwardingInputItem(index) {
				if (index >= 0 && index < portFilteringInputRules.length) {
					portFilteringInputRules.splice(index, 1);
					genFilteringInputTable();
				}
			}

			function getPortDistance(fromPort, toPort) {
				var result = -1;

				if (fromPort.value != '') {
					if (!validatePort(fromPort, true))
						return;

					result = 0;

					if (toPort.value != '') {
						if (!validatePort(toPort, true))
							return;

						// Calculate port distance
						result = toPort.value - fromPort.value;

						if (result < 0) {
							alert(_("forward alert port"));
							fromPort.focus();
							return null;
						}
					}
				}
				else if (toPort.value != '') {
					alert(_("forward port specified"));
					fromPort.focus();
					return null;
				}

				return result;
			}

			function addRuleItem(form) {
				if (!validateIP(form.ip_address, true))
					return;

				// Get source port distance
				var srcDistance = getPortDistance(form.fromPort, form.toPort); // Source port distance
				if (srcDistance == null)
					return;

				// Get destination port distance
				var dstDistance = getPortDistance(form.redirectFromPort, form.redirectToPort); // Destination port distance
				if (dstDistance == null)
					return;

				// Check distances
				if (srcDistance >= 0) {
					if (dstDistance >= 0) { // Destination ports are defined
						if (srcDistance != dstDistance) {
							alert(_("forward port not match src") + srcDistance + _("forward port not match dst") + dstDistance);
							return;
						}
					}
				}
				else if (dstDistance >= 0) {
					alert(_("forward src port not def"));
					form.fromPort.focus();
					return;
				}

				// Check comment
				var comment = checkComment(form.comment);
				if (comment == null)
					return;

				// Generate row data
				var row =
					[
						form.fwdIface.value,
						form.protocol.value,
						(srcDistance >=0) ? form.fromPort.value : '',
						(srcDistance > 0) ? form.toPort.value : '',
						form.ip_address.value,
						(dstDistance >= 0) ? form.redirectFromPort.value : '',
						(dstDistance > 0) ? form.redirectToPort.value : '',
						(form.natLoopback.checked) ? 1 : 0,
						comment
					];

				// Add a rule
				portForwardingRules[portForwardingRules.length] = row;

				// Regenerate table
				genRulesTable();
			}

			function addFilteringItem(form) {
				if (form.mac_address.value != '')
					if (!validateMAC(form.mac_address.value)) {
						alert(_("inet invalid mac"));
						form.mac_address.focus();
						return;
					}

				// Check IP & mask
				if (!checkIPMask(form.sip_address, form.sip_mask))
					return;
				if (!checkIPMask(form.dip_address, form.dip_mask))
					return;

				var srcDistance = -1;
				var dstDistance = -1;

				// If protocol is UDP or TCP, check port range
				if ((form.protocol.value == '1') || (form.protocol.value == '2')) {
					// Check source ports
					srcDistance = getPortDistance(form.sFromPort, form.sToPort);
					if (srcDistance == null)
						return;
					// Check destination ports
					dstDistance = getPortDistance(form.dFromPort, form.dToPort);
					if (dstDistance == null)
						return;
				}

				// Check comment
				var comment = checkComment(form.comment);
				if (comment == null)
					return;

				if ((form.sip_address.value != '') && (form.sip_mask.value == ''))
					form.sip_mask.value = '255.255.255.255';
				if ((form.dip_address.value != '') && (form.dip_mask.value == ''))
					form.dip_mask.value = '255.255.255.255';

				// Generate row data
				var row =
					[
						form.interface.value,
						form.protocol.value,
						form.mac_address.value,
						(form.sip_address.value != '') ? form.sip_address.value : '',
						(form.sip_address.value != '') ? form.sip_mask.value : '',
						(srcDistance >=0) ? form.sFromPort.value : '',
						(srcDistance > 0) ? form.sToPort.value : '',
						(form.dip_address.value != '') ? form.dip_address.value : '',
						(form.dip_address.value != '') ? form.dip_mask.value : '',
						(dstDistance >= 0) ? form.dFromPort.value : '',
						(dstDistance > 0) ? form.dToPort.value : '',
						form.policy.value,
						comment
					];

				// Add a rule
				portFilteringRules[portFilteringRules.length] = row;

				// Regenerate table
				genFilteringTable();
			}

			function addFilteringInputItem(form) {
				if (form.mac_address.value != '')
					if (!validateMAC(form.mac_address.value)) {
						alert(_("inet invalid mac"));
						form.mac_address.focus();
						return;
					}

				// Check IP & mask
				if (!checkIPMask(form.sip_address, form.sip_mask))
					return;

				var srcDistance = -1;
				var dstDistance = -1;

				// If protocol is UDP or TCP, check port range
				if ((form.protocol.value == '1') || (form.protocol.value == '2')) {
					// Check source ports
					srcDistance = getPortDistance(form.sFromPort, form.sToPort);
					if (srcDistance == null)
						return;
					// Check destination ports
					dstDistance = getPortDistance(form.dFromPort, form.dToPort);
					if (dstDistance == null)
						return;
				}

				// Check comment
				var comment = checkComment(form.comment);
				if (comment == null)
					return;

				if ((form.sip_address.value != '') && (form.sip_mask.value == ''))
					form.sip_mask.value = '255.255.255.255';

				// Generate row data
				var row =
					[
						form.interface.value,
						form.protocol.value,
						form.mac_address.value,
						(form.sip_address.value != '') ? form.sip_address.value : '',
						(form.sip_address.value != '') ? form.sip_mask.value : '',
		 				(srcDistance >=0) ? form.sFromPort.value : '',
						(srcDistance > 0) ? form.sToPort.value : '',
						'',
						'',
						(dstDistance >= 0) ? form.dFromPort.value : '',
						(dstDistance > 0) ? form.dToPort.value : '',
						1,
						comment
					];

				// Add a rule
				portFilteringInputRules[portFilteringInputRules.length] = row;

				// Regenerate table
				genFilteringInputTable();
			}

			function updateForwardingState(form) {
				var ena = document.getElementById('portForwardEnabled').value == '1';
				displayElement('portForwardingRow', ena);
				if (ena)
					genRulesTable();
			}

			function updateFilteringState(form) {
				var ena = document.getElementById('portFilterEnabled').value == '1';
				displayElement('portFilteringRow', ena);
				if (ena)
					genFilteringTable();
			}

			function updateFilteringInputState(form) {
				var ena = document.getElementById('portFilterInputEnabled').value == '1';
				displayElement('portFilteringInputRow', ena);
				if (ena)
					genFilteringInputTable();
			}

			function genTableData(rules, form) {
				var values = "";
				for (var i = 0; i < rules.length; i++) {
					var rule = rules[i];
					var row = '';

					for (var j = 0; j < rule.length; j++) {
						if (row != '')
							row += ',';
						row += rule[j];
					}
					values += row + ';'
				}
				return values;
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<!-- Port forwarding -->
					<h1 data-tr="forward title">Port Forwarding Settings</h1>
					<p data-tr="forward introduction">Here you can setup port forwarding to provide services to the Internet.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="portForward" action="/goform/portForward" onSubmit="return submitForwardForm(this);">
						<table class="form">
							<col style="width: 40%" />
							<col style="width: 60%" />
							<tbody>
								<tr>
									<td class="title" colspan="2" data-tr="forward virtual server">Port Forwarding Settings</td>
								</tr>
								<tr>
									<td class="head" data-tr="forward virtual server setting"> Port Forwarding Settings </td>
									<td><select onChange="updateForwardingState(this.form);" id="portForwardEnabled" name="portForwardEnabled" class="normal">
										<option value="0" data-tr="button disable" selected="selected">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select></td>
								</tr>
								<tr id="portForwardingRow">
									<td colspan="2" id="portForwardingTable"></td>
								</tr>
							</tbody>
						</table>
						<!-- Port forwarding rules -->
						<table class="buttons">
							<tr>
								<td><input type="hidden" name="portForwardRules" value="">
								<input type="hidden" name="vpn_restart" value="0">
								<input type="submit" class="normal" data-tr="button apply" value="Apply">
								<input type="hidden" name="submit-url" value="/firewall/firewall.asp" ></td>
							</tr>
						</table>
					</form>
					<br>
					<!-- MAC / IP / Port Filtering Forward-->
					<h1 data-tr="port title">MAC/IP/Port Filtering Settings</h1>
					<p data-tr="port introduction">Here you can setup firewall rules to protect your network from malware and other security threats from the Internet.</p>
					<hr>
					<form method="POST" name="portFiltering" action="/goform/portFiltering" onSubmit="return submitFilterForm(this);">
						<table class="form">
							<col style="width: 40%" />
							<col style="width: 60%" />
							<tbody>
								<tr>
									<td class="title" colspan="2" data-tr="port basic setting">Basic Settings</td>
								</tr>
								<tr>
									<td class="head" data-tr="port basic filter"> MAC/IP/Port Filtering </td>
									<td><select id="portFilterEnabled" name="portFilterEnabled" class="normal" onChange="updateFilteringState(this.form);">
										<option value="0" data-tr="button disable" selected="selected">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select></td>
								</tr>
									<tr id="portFilteringRow">
									<td id="portFilteringTable" colspan="2"></td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="hidden" name="portFilteringRules" value="">
									<input type="hidden" name="defaultFirewallPolicy" value="">
									<input type="hidden" name="vpn_restart" value="0">
									<input type="submit" class="normal" data-tr="button apply" value="Apply">
									<input type="hidden" name="submit-url" value="/firewall/firewall.asp" ></td>
							</tr>
						</table>
					</form>
					<br>
					<!-- MAC / IP / Port Filtering INPUT -->
					<form method="POST" name="portFilteringInput" action="/goform/portFilteringInput" onSubmit="return submitFilterInputForm(this);">
						<table class="form">
							<col style="width: 40%" />
							<col style="width: 60%" />
							<tbody>
								<tr>
									<td class="title" colspan="2" data-tr="port basic setting input">Basic Settings</td>
								</tr>
								<tr>
									<td class="head" data-tr="port basic filter input">MAC/IP/Port Filtering</td>
									<td><select id="portFilterInputEnabled" name="portFilterInputEnabled" class="normal" onChange="updateFilteringInputState(this.form);">
										<option value="0" data-tr="button disable" selected="selected">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select></td>
								</tr>
									<tr id="portFilteringInputRow">
									<td id="portFilteringInputTable" colspan="2"></td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="hidden" name="portFilteringInputRules" value="">
									<input type="hidden" name="defaultFirewallInputPolicy" value="">
									<input type="hidden" name="vpn_restart" value="0">
									<input type="submit" class="normal" data-tr="button apply" value="Apply">
									<input type="hidden" name="submit-url" value="/firewall/firewall.asp" ></td>
							</tr>
						</table>
					</form>
					<br>

					<h1 data-tr="dmz title"> DMZ Settings </h1>
					<p data-tr="dmz introduction"> Here you can setup the De-Militarized Zone (DMZ) to separate your external services from the rest of LAN.</p>
					<hr>
					<form method="POST" name="DMZ" action="/goform/setFirewallDMZ">
						<table class="form">
							<col style="width: 40%" />
							<col style="width: 60%" />
							<tbody>
								<tr>
									<td class="title" colspan="2" data-tr="dmz title">DMZ Settings</td>
								</tr>
								<tr>
									<td class="head" data-tr="dmz setting"> DMZ Settings </td>
									<td><select onChange="dmzEnableSwitch();" id="DMZEnabled" name="DMZEnabled" class="normal">
										<option data-tr="button disable" value="0">Disable</option>
										<option data-tr="button enable"  value="1">Enable</option>
									</select></td>
								</tr>
								<tr id="dmzAdress">
									<td class="head" data-tr="dmz ipaddr"> DMZ IP Address </td>
									<td><input type="text" id="DMZIPAddress" name="DMZIPAddress" class="normal"></td>
								</tr>
								<tr id="dmzLoopback_tr">
									<td class="head" id="ldmzLoopback">DMZ NAT loopback</td>
									<td><select id="dmzLoopback" name="dmzLoopback" class="normal" onChange="dmzLoopbackWarning();">
										<option value="0" data-tr="button disable">Disable</option>
										<option value="1" data-tr="button enable">Enable</option>
									</select></td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" value="Apply"  data-tr="button apply"  onClick="return checkValuesDMZ(this.form);">
									<input type="button" class="normal" value="Cancel" data-tr="button cancel" onClick="window.location.reload();">
									<input type="button" class="normal" value="Reset"  data-tr="button reset"  onClick="resetValues(this.form);">
									<input type="hidden" name="reset" value="0">
								</td>
							</tr>
						</table>
					</form>

					<!-- Other netfilter settings -->
					<h1 data-tr="firewall title">Other Settings</h1>
					<hr>
					<form method="POST" name="Firewall" action="/goform/setFirewall" onSubmit="return submitFirewallForm(this);">
						<table class="form">
							<col style="width: 40%" />
							<col style="width: 60%" />
							<tbody>
								<tr>
									<td class="title" colspan="2" data-tr="firewall title">Firewall Settings</td>
								</tr>
								<tr>
									<td class="head" data-tr="forward session limit">Limit TCP session per ip</td>
									<td><input type="text" id="ForwardSesLimit" name="ForwardSesLimit" class="half" maxlength="5">
										<font color="#808080" data-tr="firewall default session limit">(default 0 - disabled)</font></td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" data-tr="button apply" value="Apply">
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
