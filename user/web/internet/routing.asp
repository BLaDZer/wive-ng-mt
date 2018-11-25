<!DOCTYPE html>
<html>
	<head>
		<title>Static Routing Settings</title>
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
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script>
			var ROUTING_TABLE = [ <% getRoutingTable(); %> ];
			var ROUTING_TABLE_IPV6 = getRoutingTableIPv6();

			function getRoutingTableIPv6() {
				var routing_tbl = [ <% getRoutingTableIPv6(); %> ];
				var res = [];
				for (var i=0;i<routing_tbl.length;i++) // >
				{
					var hasOne = false;
					for (var j=0;j<i;j++) { // >
						if (routing_tbl[j].toString() == routing_tbl[i].toString()) {
							hasOne = true;
							break;
						}
					}
					if (hasOne == false) {
						res.push(routing_tbl[i]);
					}
				}
				return res;
			}

			function initTranslation() {
				init_translation_model();
			}

			function initValues() {
				var form	= document.editRouting;
				var ifc		= form.interface.options;

				ifc.add(new Option("LAN", "LAN"));
				if (NVRAM_OperationMode != '0' && NVRAM_OperationMode != '3') {
					ifc.add(new Option("WAN", "WAN"));
				}
				ifc.add(new Option("VPN", "VPN"));

				ifc.add(new Option(_("routing custom"), "Custom"));

				form.hostnet.value		= 'host';
				form.interface.value	= 'LAN';
				hideElement('routingNetmaskRow');

				// Dynamic routing
				form.RIPSelect.selectedIndex = NVRAM_RIPEnable;
				displayElement('dynamicRoutingDiv', BUILD_DYNAMIC_ROUTING);

				genRoutingTable();
				showWarning();
				initTranslation();
			}

			function checkValues(form) {
				var trans = [];
				for (var i=0; i < ROUTING_TABLE.length; i++)
				{
					if (ROUTING_TABLE[i][12] != 0)
						trans.push(ROUTING_TABLE[i]);
				}
				form.routingTableDiff.value = trans.join(';');
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);
				return false;
			}

			function hostnetChange(form) {
				displayElement('routingNetmaskRow', form.hostnet.value == 'net');
			}

			function interfaceChange(form) {
				displayElement('customInterfaceRow', form.interface.value == 'Custom');
			}

			function wrapDel(str, idle) {
				return (idle == 1) ? "<del>" + str + "</del>" : str;
			}

			function genRoutingTableIPv6() {
				function chunkSubstr(str, size) {
					const numChunks = Math.ceil(str.length / size)
					const chunks = new Array(numChunks)

					for (var i = 0, o = 0; i < numChunks; ++i, o += size) {
						chunks[i] = str.substr(o, size)
					}

					return chunks
				}

				if (ROUTING_TABLE_IPV6.length == 0) {
					setInnerHTML('ajxCtxRoutingTableIPv6', '');
					return;
				}

				var html = '<table class="form">';

				html += '<tr><td class="title" colspan="11" data-tr="routing del title ipv6">Current IPv6 Routing table:</td></tr>'; // Header
				html += '<tr><th data-tr="routing number">ID</th>' +
					'<th data-tr="routing dest" align="center">Destination</th>' +
					'<th data-tr="routing next hop" align="center">Next Hop</th>' +
					'<th data-tr="routing del flags" align="center">Flags</th>' +
					'<th data-tr="routing del metric" align="center">Metric</th>' +
					'<th data-tr="routing del ref" align="center">Ref</th>' +
					'<th data-tr="routing del use" align="center">Use</th>' +
					'<th data-tr="routing interface" align="center">Interface</th>' +
					'</tr>';

				for (var i = 0; i < ROUTING_TABLE_IPV6.length; i++) {
					var row = ROUTING_TABLE_IPV6[i];

					var d		= row[10];
					var iface	= (row[9] == 'Custom') ? (row[9] + ' (' + row[0] + ')') : row[9];

					iface = ROUTING_TABLE_IPV6[i][0];
					dest = ipaddr.parseCIDR(chunkSubstr(ROUTING_TABLE_IPV6[i][1], 4).join(":")+"/"+ROUTING_TABLE_IPV6[0][2]);
					src = ipaddr.parseCIDR(chunkSubstr(ROUTING_TABLE_IPV6[i][3], 4).join(":")+"/"+ROUTING_TABLE_IPV6[0][4]);
					next_hop = ipaddr.parse(chunkSubstr(ROUTING_TABLE_IPV6[i][5], 4).join(":"));
					metric = ROUTING_TABLE_IPV6[i][6];
					ref = ROUTING_TABLE_IPV6[i][7];
					use = ROUTING_TABLE_IPV6[i][8];
					flags = ROUTING_TABLE_IPV6[i][9];

					html += '<tr>' +
						'<td style="text-align: center;">' + (i+1) + '</td>' + // rownum
						'<td>' + dest.toString().replace(',','/') + '</td>' + // destination
						'<td>' + next_hop + '</td>' + // next hop
						'<td style="text-align: center;">' + flags + '</td>' + // flags
						'<td style="text-align: center;">' + metric + '</td>' + // metric
						'<td style="text-align: center;">' + ref + '</td>' + // ref
						'<td style="text-align: center;">' + use + '</td>' + // use
						'<td style="text-align: center;">' + iface + '</td>' + // interface
						'</tr>';
				}

				html += '</table><br>';
				setInnerHTML('ajxCtxRoutingTableIPv6', html);
			}

			function genRoutingTable() {
				var html = '<table class="form">';
				
				html += '<tr><td class="title" colspan="11" data-tr="routing del title">Current Routing table in the system:</td></tr>'; // Header
				html += '<tr><th data-tr="routing number">ID</th>' +
					'<th data-tr="routing dest" align="center">Destination</th>' +
					'<th data-tr="routing netmask" align="center">Netmask</th>' +
					'<th data-tr="routing gateway" align="center">Gateway</th>' +
					'<th data-tr="routing del flags" align="center">Flags</th>' +
					'<th data-tr="routing del metric" align="center">Metric</th>' +
					'<th data-tr="routing del ref" align="center">Ref</th>' +
					'<th data-tr="routing del use" align="center">Use</th>' +
					'<th data-tr="routing interface" align="center">Interface</th>' +
					'<th data-tr="routing comment" align="center">Comment</th>' +
					'<th data-tr="routing action">Actions</th></tr>';
				
				for (var i = 0; i < ROUTING_TABLE.length; i++) {
					var row = ROUTING_TABLE[i];
					if (row[12] == 2) // Record was deleted?
						continue;
					
					var style	= (row[8] > -1) ? ' style="background-color: #cccccc;"' : ''; // Category
					var d		= row[10];
					var iface	= (row[9] == 'Custom') ? (row[9] + ' (' + row[0] + ')') : row[9];
					
					html += '<tr' + style + '>';
					html += '<td style="text-align: center;">' + (i+1) + '</td>' + // rownum
						'<td>' + wrapDel(row[1], d) + '</td>' + // destination
						'<td>' + wrapDel(row[3], d) + '</td>' + // netmask
						'<td>' + wrapDel(row[2], d) + '</td>' + // gateway
						'<td style="text-align: center;">' + wrapDel(row[4], d) + '</td>' + // flags
						'<td style="text-align: center;">' + wrapDel(row[7], d) + '</td>' + // metric
						'<td style="text-align: center;">' + wrapDel(row[5], d) + '</td>' + // ref
						'<td style="text-align: center;">' + wrapDel(row[6], d) + '</td>' + // use
						'<td style="text-align: center;">' + wrapDel(iface, d) + '</td>' + // interface
						'<td>' + wrapDel(row[11], d) + '&nbsp;</td>'; // comment
					
					html += (row[8] > -1) ?
						'<td style="text-align: center;"><a style="color: #ff0000; cursor: pointer;" href="javascript:removeRoutingItem(' + i + ');"><img src="/graphics/cross.png" alt="[x]"></a></td>' : '<td>&nbsp;</td>';
				}

				html += '</table><br>';

				setInnerHTML('ajxCtxRoutingTable', html);
				genRoutingTableIPv6();
				initTranslation();

			}

			function removeRoutingItem(index) {
				if ((index<0) || (index >= ROUTING_TABLE.length))
					return;
				var row = ROUTING_TABLE[index];
				if (row[8] > -1)
				{
					if (row[12] == 1)
						ROUTING_TABLE.splice(index, 1); // Remove rule if was added, then deleted in same transaction
					else
						row[12] = 2; // Mark rule as deleted if existed
					genRoutingTable();
				}
			}

			function addRoutingRule(form) {
				var row = [ '', '0.0.0.0', '0.0.0.0', '0.0.0.0',  1, 0, 0, 0,  0, 'LAN', 0, '', 1 ]; // New rule

				// Destination
				if (!validateIP(form.dest, true)) {
					form.dest.focus();
					return false;
				}
				row[1] = form.dest.value;

				// Netmask
				if (form.hostnet.value == 'net') {
					if (!validateIPMask(form.netmask, true))
					{
						form.netmask.focus();
						return false;
					}
					row[3] = form.netmask.value;
				}
				else
					row[3] = '255.255.255.255';

				// Gateway
				if (!validateIP(form.gateway, true)) {
					form.gateway.focus();
					return false;
				}
				row[2] = form.gateway.value;

				// Interface
				row[9] = form.interface.value;
				if (row[9] == 'Custom')
					row[0] = form.custom_interface.value;
				
				// Comment
				row[11] = form.comment.value;
				if (row[11].match(/[^A-ZА-Я0-9\s.]/gi)) {
					alert(_("routing unsup chars"));
					form.comment.focus();
					return;
				}

				// Clear values
				form.dest.value = '';
				form.hostnet.value = 'host';
				form.gateway.value = '';
				form.comment.value = '';

				hostnetChange(form);

				// Add row & rebuild table
				ROUTING_TABLE.push(row);
				genRoutingTable();
			}

		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 data-tr="routing title">Static Routing Settings</h1>
					<p id="routingIntroduction" data-tr="routing introduction"> You may add or remote Internet routing rules here.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" action="/goform/editRouting" name="editRouting">
						<div id="ajxCtxRoutingTable"></div>
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 60%;" />
							<tbody>
								<tr>
									<td class="title" colspan="2" data-tr="routing add rule">Add a routing rule</td>
								</tr>
								<tr>
									<td class="head" data-tr="routing dest">Destination</td>
									<td><input class="mid" name="dest" type="text" maxlength="15"></td>
								</tr>
								<tr>
									<td class="head" data-tr="routing range">Host/Net</td>
									<td>
										<select class="mid" name="hostnet" onChange="hostnetChange(this.form);">
											<option selected="selected" value="host" data-tr="routing host">Host</option>
											<option value="net" data-tr="routing net">Net</option>
										</select>
									</td>
								</tr>
								<tr id="routingNetmaskRow">
									<td class="head" data-tr="routing netmask">Subnet Mask</td>
									<td><input class="mid" name="netmask" type="text" maxlength="15"></td>
								</tr>
								<tr>
									<td class="head" data-tr="routing gateway">Gateway</td>
									<td><input class="mid" name="gateway" type="text" maxlength="15"></td>
								</tr>
								<tr>
									<td class="head" data-tr="routing interface">Interface</td>
									<td>
										<select class="mid" name="interface" onChange="interfaceChange(this.form);"></select>
									</td>
								</tr>
								<tr id="customInterfaceRow" style="display: none;">
									<td class="head" data-tr="routing interface name">Interface Name</td>
									<td><input alias="right" class="mid" name="custom_interface" type="text" maxlength="8"></td>
								</tr>
								<tr>
									<td class="head" data-tr="routing comment">Comment</td>
									<td><input name="comment" class="mid" type="text" maxlength="15"></td>
								</tr>
								<tr>
							</tbody>
						</table>
						<input type="button" class="normal" value="Add" data-tr="button add rule" onClick="addRoutingRule(this.form);">
						<br>
						<br>
						<div id="dynamicRoutingDiv" style="display:none;">
							<table class="form">
								<col style="width: 40%;" />
								<col style="width: 60%;" />
								<tbody>
									<tr>
										<td class="title" colspan="2" data-tr="routing dynamic title">Dynamic routing</td>
									</tr>
									<tr>
										<td class="head" data-tr="routing rip">RIP</td>
										<td>
											<select name="RIPSelect" class="mid">
												<option value="0" data-tr="button disable">Disable</option>
												<option value="1" data-tr="button enable">Enable</option>
											</select>
										</td>
									</tr>
								</tbody>
							</table>
							<br>
						</div>
						<div id="ajxCtxRoutingTableIPv6"></div>
						<table class="buttons">
							<tr>
								<td>
									<input type="hidden" name="routingTableDiff" >
									<input type="submit" class="normal" value="Apply" data-tr="button apply" onClick="return checkValues(this.form);">
									<input type="button" class="normal" value="Cancel" data-tr="button cancel" name="routingCancel" onClick="window.location.reload();">
									<input type="reset"  class="normal" value="Reset"  data-tr="button reset"  name="routingReset"  onClick="resetValues(this.form, 30);">
									<input type="hidden" name="reset" value="0">
								</td>
							</tr>
						</table>
						<div class="whitespace"></div>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>
