<!DOCTYPE html>
<html>
	<head>
		<title>Static Routing Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("buttons");

			var opmode			= "<% getCfgZero(1, "OperationMode"); %>";
			var routingRules	= [ <% getRoutingTable(); %> ];

			function initTranslation()
			{
				_TR("routingTitle", 					"routing title");
				_TR("routingIntroduction", 				"routing introduction");
				_TR("routingAddRule", 					"routing add rule");
				_TR("routingDest", 						"routing dest");
				_TR("routingTableDest", 				"routing dest");
				_TR("routingRange", 					"routing range");
				_TR("routingNetmask", 					"routing netmask");
				_TR("routingTableNetmask", 				"routing netmask");
				_TR("routingGateway",					"routing gateway");
				_TR("routingTableGateway", 				"routing gateway");
				_TR("routingInterface",					"routing interface");
				_TR("routingTableInterface",			"routing interface");
				_TR("routingIfaceName",					"routing interface name");
				_TR("routingComment",					"routing comment");
				_TR("routingTableComment",				"routing comment");
				_TR("routingHost",						"routing host");
				_TR("routingNet",						"routing net");
				_TR("dynamicRoutingTitle",				"routing dynamic title");
				_TR("RIP",								"routing rip")
				_TR("RIPDisable",						"button disable");
				_TR("RIPEnable",						"button enable");
				_TR("routingCurrentRoutingTableRules",	"routing del title");
				_TR("routingTableNo", 					"routing number");
				_TR("routingDelFlags",					"routing del flags");
				_TR("routingDelMetric",					"routing del metric");
				_TR("routingDelRef",					"routing del ref");
				_TR("routingDelUse",					"routing del use");
				_TR("routingAction",					"routing action");

				_TRV("buttonAdd",						"button add");
				_TRV("routingApply",					"button apply");
				_TRV("routingApply2",					"button apply");
				_TRV("routingCancel",					"button cancel");
				_TRV("routingCancel2",					"button cancel");
				_TRV("routingReset",					"button reset");
				_TRV("routingReset2",					"button reset");
			}

			function initValues()
			{
				var form		= document.editRouting;
				var dform		= document.dynamicRouting;
				var rip_ena		= '<% getCfgZero(1, "RIPEnable"); %>';
				var dr_built	= '<% getDynamicRoutingBuilt(); %>';

				var ifc = form.interface.options;
				ifc.add(new Option("LAN", "LAN"));
				if (opmode != '0')
				{
					ifc.add(new Option("WAN", "WAN"));
					ifc.add(new Option("VPN", "VPN"));
				}
				ifc.add(new Option(_("routing custom"), "Custom"));

				form.hostnet.value		= 'host';
				form.interface.value	= 'LAN';
				hideElement('routingNetmaskRow');

				// Dynamic routing
				dform.RIPSelect.selectedIndex = (rip_ena == '1') ? 1 : 0;
				displayElement('dynamicRoutingDiv', dr_built == '1');

				genRoutingTable();
				showWarning();
				initTranslation();
			}

			function checkValues(form) {
				var trans = [];
				for (var i=0; i < routingRules.length; i++)
				{
					if (routingRules[i][12] != 0)
						trans.push(routingRules[i]);
				}
				form.routingTableDiff.value = trans.join(';');
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function hostnetChange(form)
			{
				displayElement('routingNetmaskRow', form.hostnet.value == 'net');
			}

			function interfaceChange(form)
			{
				displayElement('customInterfaceRow', form.interface.value == 'Custom');
			}

			function wrapDel(str, idle)
			{
				return (idle == 1) ? "<del>" + str + "</del>" : str;
			}

			function genRoutingTable()
			{
				var html = '<table class="form">';
				
				html += '<tr><td class="title" colspan="11" id="routingCurrentRoutingTableRules">Current Routing table in the system:</td></tr>'; // Header
				html += '<tr><th id="routingTableNo">ID</th>' +
					'<th id="routingTableDest" align="center">Destination</th>' + 
					'<th id="routingTableNetmask" align="center">Netmask</th>' +
					'<th id="routingTableGateway" align="center">Gateway</th>' +
					'<th id="routingDelFlags" align="center">Flags</th>' +
					'<th id="routingDelMetric" align="center">Metric</th>' +
					'<th id="routingDelRef" align="center">Ref</th>' +
					'<th id="routingDelUse" align="center">Use</th>' +
					'<th id="routingTableInterface" align="center">Interface</th>'+
					'<th id="routingTableComment" align="center">Comment</th>'+
					'<th id="routingAction">Actions</th></tr>';
				
				for (var i=0; i<routingRules.length; i++)
				{
					var row = routingRules[i];
					if (row[12] == 2) // Record was deleted?
						continue;
					
					var style = (row[8] > -1) ? ' style="background-color: #cccccc;"' : ''; // Category
					var d = row[10];
					var iface = (row[9] == 'Custom') ? (row[9] + ' (' + row[0] + ')') : row[9];
					
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

				html += '</table>';

				setInnerHTML('ajxCtxRoutingTable', html);
			}

			function removeRoutingItem(index)
			{
				if ((index<0) || (index >= routingRules.length))
					return;
				var row = routingRules[index];
				if (row[8] > -1)
				{
					if (row[12] == 1)
						routingRules.splice(index, 1); // Remove rule if was added, then deleted in same transaction
					else
						row[12] = 2; // Mark rule as deleted if existed
					genRoutingTable();
				}
			}

			function addRoutingRule(form)
			{
				var row = [ '', '0.0.0.0', '0.0.0.0', '0.0.0.0',  1, 0, 0, 0,  0, 'LAN', 0, '', 1 ]; // New rule

				// Destination
				if (!validateIP(form.dest, true))
				{
					form.dest.focus();
					return false;
				}
				row[1] = form.dest.value;

				// Netmask
				if (form.hostnet.value == 'net')
				{
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
				if (!validateIP(form.gateway, true))
				{
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
				if (row[11].match(/[^A-ZА-Я0-9\s.]/gi))
				{
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
				routingRules.push(row);
				genRoutingTable();
			}

		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td><h1 id="routingTitle">Static Routing Settings</h1>
					<p id="routingIntroduction"> You may add or remote Internet routing rules here.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" action="/goform/editRouting" name="editRouting" onSubmit="return checkValues(this);">
						<!-- Rule adding -->
						<table class="form">
							<tr>
								<td class="title" colspan="2" id="routingAddRule">Add a routing rule</td>
							</tr>
							<tr>
								<td class="head" style="width: 40%;" id="routingDest">Destination</td>
								<td style="width: 60%;"><input class="mid" name="dest" type="text" maxlength="15"></td>
							</tr>
							<tr>
								<td class="head" style="width: 40%;" id="routingRange">Host/Net</td>
								<td style="width: 60%;"><select class="mid" name="hostnet" onChange="hostnetChange(this.form);">
									<option selected="selected" value="host" id="routingHost">Host</option>
									<option value="net" id="routingNet">Net</option>
								</select></td>
							</tr>
							<tr id="routingNetmaskRow">
								<td class="head" style="width: 40%;" id="routingNetmask">Subnet Mask</td>
								<td style="width: 60%;"><input class="mid" name="netmask" type="text" maxlength="15"></td>
							</tr>
							<tr>
								<td class="head" style="width: 40%;" id="routingGateway">Gateway</td>
								<td style="width: 60%;"><input class="mid" name="gateway" type="text" maxlength="15"></td>
							</tr>
							<tr>
								<td class="head" style="width: 40%;" id="routingInterface">Interface</td>
								<td style="width: 60%;"><select class="mid" name="interface" onChange="interfaceChange(this.form);">
								</select></td>
							</tr>
							<tr id="customInterfaceRow" style="display: none;">
								<td class="head" style="width: 40%;" id="routingIfaceName">Interface Name</td>
								<td style="width: 60%;"><input alias="right" class="mid" name="custom_interface" type="text" maxlength="8"></td>
							</tr>
							<tr>
								<td class="head" style="width: 40%;" id="routingComment">Comment</td>
								<td style="width: 60%;"><input name="comment" class="mid" type="text" maxlength="32"></td>
							</tr>
							<tr>
						</table>
						<table>
							<tr>
								<td><input value="Add" id="buttonAdd" class="normal" onClick="addRoutingRule(this.form);" type="button"></td>
							</tr>
						</table>
						<br>
						<!--  delete rules -->
						<div id="ajxCtxRoutingTable"></div>
						<table class="buttons">
							<tr>
								<td><input type="hidden" name="routingTableDiff" >
									<input value="Apply" id="routingApply" class="normal" type="submit">&nbsp;&nbsp;
									<input type="button" class="normal" value="Cancel" id="routingCancel" name="routingCancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="reset"  class="normal" value="Reset"  id="routingReset"  name="routingReset"  onClick="resetValues(this.form);">
									<input type="hidden" name="reset" value="0">
								</td>
							</tr>
						</table>
						<br>
					</form>
					<div id="dynamicRoutingDiv" style="display:none;">
						<form method="POST" name="dynamicRouting" action="/goform/dynamicRouting">
							<table class="form">
								<tr>
									<td class="title" colspan="2" id="dynamicRoutingTitle">Dynamic routing</td>
								</tr>
								<tr>
									<td class="head" style="width: 40%;" id="RIP">RIP</td>
									<td style="width: 60%;"><select name="RIPSelect" size="1">
										<option value="0" id="RIPDisable">Disable</option>
										<option value="1" id="RIPEnable">Enable</option>
									</select></td>
								</tr>
							</table>
							<table class="buttons">
								<tr>
									<td><input type="submit" class="normal" value="Apply"  id="routingApply2"  name="dynamicRoutingApply" onClick="ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);">&nbsp;&nbsp;
										<input type="button" class="normal" value="Cancel" id="routingCancel2" name="dynamicRoutingCancel" onClick="window.location.reload();">&nbsp;&nbsp;
										<input type="reset"  class="normal" value="Reset"  id="routingReset2"  name="dynamicRoutingReset"  onClick="resetValues(this.form);">
										<input type="hidden" name="reset" value="0">
									</td>
								</tr>
							</table>
						</form>
					</div>
				</td>
			</tr>
		</table>
	</body>
</html>
