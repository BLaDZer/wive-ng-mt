<!DOCTYPE html>
<html>
<head>
<title>Port Forwarding Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("firewall");
Butterlate.setTextDomain("buttons");

var MAX_RULES = 32;

var portForwardingRules =
[
	<% getPortForwardRules(); %>
];

var portFilteringRules =
[
	<% getPortFilteringRules(); %>
];

var defaultFilterPolicy = '<% getCfgZero(1, "DefaultFirewallPolicy"); %>';

function showPortRange(from, to)
{
	if (from == '')
		return '*';
	return (to != '') ? from + '-' + to : from;
}

function showProtocol(proto)
{
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

function showValue(value)
{
	return (value != '') ? value : '*';
}

function showPolicy(policy)
{
	var color = (policy == '1') ? '#3da42c' : '#dd3b3b';
	var text  = (policy == '1') ? 'accept' : 'drop';

	return '<td rowspan="2" style="color: ' + color + ';">' + text + '</td>';
}

function genRulesTable()
{
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

	for (var i=0; i<portForwardingRules.length; i++)
	{
		var row = portForwardingRules[i];
		table += '<tr>' + '<td>' + row[0] + '</td>' + // Interface
			'<td>' + showProtocol(row[1]) + '</td>' + // Protocol
			'<td>' + showPortRange(row[2], row[3]) + '</td>' + // Source port range
			'<td>' + row[4] + '</td>' + // Destination IP
			'<td>' + showPortRange(row[5], row[6]) + '</td>' + // Destination port range
			'<td><input type="checkbox"' + ((row[7] != 0) ? ' checked="true">' : '>') + // NAT Loopback
			'<td>' + row[8] + '&nbsp;</td>' + // Comment
			'<td style="text-align: center;">' +
			'<a style="color: #ff0000;" title="' + _("forward delete record") + '" href="javascript:deleteRuleItem(' + i + ');"' + disabled + '><img src="/graphics/cross.png" alt="[x]"></a></td>' +
			'</tr>';
	}

	if (portForwardingRules.length <= 0)
		table += '<tr><td colspan="8" style="text-align: left;">' + _("forward no rules") + '</td></tr>';

	// Controls
	table +=
		'<tr>'+
		'<td rowspan="2"><select name="fwdIface" tabindex="1"><option value="WAN" selected="selected">WAN</option><option value="VPN">VPN</option></select></td>' +
		'<td rowspan="2"><select name="protocol" tabindex="2"><option value="3" selected="selected">TCP&amp;UDP</option><option value="1">TCP</option><option value="2">UDP</option></select></td>' +
		'<td><input type="text" class="short" name="fromPort" tabindex="3"></td>' + 
		'<td rowspan="2"><input type="text" class="normal" name="ip_address" tabindex="5"></td>' +
		'<td><input type="text" class="short" name="redirectFromPort" tabindex="6"></td>' + 
		'<td rowspan="2"><input type="checkbox" name="natLoopback" tabindex="7"></td>' + 
		'<td rowspan="2"><input type="text" class="normal" name="comment" tabindex="8"></td>' +
		'<td rowspan="2" style="text-align: center;"><input type="button" class="half" title="' + _("forward add record") + '" value="' + _("button add") + '" tabindex="10" onclick="addRuleItem(this.form);"' + disabled + '></td>' +
		'</tr>' +
		'<tr>' +
		'<td><input type="text" class="short" name="toPort" tabindex="4"></td>' + 
		'<td><input type="text" class="short" name="redirectToPort" tabindex="8"></td>' + 
		'</tr>';

	// Close manager
	table += '</table>';

	var elem = document.getElementById("portForwardingTable");
	if (elem!=null)
		elem.innerHTML = table;
}

function genFilteringTable()
{
	var disabled = (portFilteringRules.length >= MAX_RULES) ? ' disabled="disabled"' : '';

	var table = '<table class="small" style="width: 100%"><tr>';
	table += '<th rowspan="2">' + _("forward interface") + '</th>';
	table += '<th rowspan="2">' + _("forward protocol") + '</th>';
	table += '<th rowspan="2">' + _("forward mac") + '</th>';
	table += '<th>' + _("forward src ip") + '</th>';
	table += '<th rowspan="2">' + _("forward src ports") + '</th>';
	table += '<th>' + _("forward dst ip") + '</th>';
	table += '<th rowspan="2">' + _("forward dst ports") + '</th>';
	table += '<th rowspan="2">' + _("forward policy") + '</th>';
	table += '<th rowspan="2">' + _("forward comment") + '</th>';
	table += '<th rowspan="2">' + _("forward action") + '</th>';
	table += '</tr><tr>';
	table += '<th>' + _("forward mask") + '</th>';
	table += '<th>' + _("forward mask") + '</th>';
	table += '</tr>';

	for (var i=0; i<portFilteringRules.length; i++)
	{
		var row = portFilteringRules[i];

		table +=
			'<tr>' +
			'<td rowspan="2">' + row[0] + '</td>' + // Interface
			'<td rowspan="2">' + showProtocol(row[1]) + '</td>' + // Protocol
			'<td rowspan="2">' + showValue(row[2]) + '</td>' + // MAC
			'<td>' + showValue(row[3]) + '</td>' + // Source IP
			'<td rowspan="2">' + showPortRange(row[5], row[6]) + '</td>' + // Source port range
			'<td>' + showValue(row[7]) + '</td>' + // Destination IP
			'<td rowspan="2">' + showPortRange(row[9], row[10]) + '</td>' + // Destination port range
			showPolicy(row[11]) + // Policy
			'<td rowspan="2">' + row[12] + '&nbsp;</td>' + // Comment
			'<td rowspan="2" style="text-align: center;"><a style="color: #ff0000;" title="' + _("forward delete record") + '" href="javascript:deleteForwardingItem(' + i + ');"' + disabled + '><img src="/graphics/cross.png" alt="[x]"></a></td>' +
			'</tr><tr>' +
			'<td>' + showValue(row[4]) + '</td>' + // Source IP Mask
			'<td>' + showValue(row[8]) + '</td>' + // Destination IP Mask
			'</tr>';
	}

	if (portFilteringRules.length <= 0)
		table += '<tr><td colspan="10" style="text-align: left;">' + _("forward no filter rules") + '</td></tr>';

	var accept_sel = (defaultFilterPolicy == '0') ? ' selected="selected"' : '';
	var drop_sel = (defaultFilterPolicy != '0') ? ' selected="selected"' : '';
	table +=
		'<tr><td colspan="8" style="text-align:left;">' + _("port basic default policy") + '</td>' +
		'<td colspan="2" style="text-align: right;"><select name="defaultFilteringPolicy" onchange="javascript:changedefaultFilterPolicy(this.form);"><option value="0"' + accept_sel +'>' + _("port basic default policy accepted") + '</option><option value="1"' + drop_sel +'>' + _("port basic default policy dropped") + '</option></select></td></tr>';

	// Controls
	table +=
		'<tr>'+
		'<td rowspan="2"><select name="interface" tabindex="10"><option value="LAN">LAN</option><option value="WAN" selected="selected">WAN</option><option value="VPN">VPN</option></select></td>' +
		'<td rowspan="2"><select name="protocol" tabindex="11" onchange="javascript:protocolChange(this.form);"><option value="5">None</option><option value="1">TCP</option><option value="2">UDP</option><option value="4">ICMP</option></select></td>' +
		'<td rowspan="2"><input type="text" tabindex="12" class="normal" name="mac_address"></td>' +
		'<td><input type="text" tabindex="13" class="normal" name="sip_address"></td>' +
		'<td><input type="text" tabindex="15" class="short" name="sFromPort" disabled="disabled"></td>' +
		'<td><input type="text" tabindex="17" class="normal" name="dip_address"></td>' +
		'<td><input type="text" tabindex="19" class="short" name="dFromPort" disabled="disabled"></td>' +
		'<td rowspan="2"><select tabindex="21" name="policy"><option value="0"' + accept_sel + '>' + _("port filter action drop") + '</option><option value="1"' + drop_sel + '>' + _("port filter action accept") + '</option></select></td>' +
		'<td rowspan="2"><input tabindex="22" type="text" class="normal" name="comment"></td>' +
		'<td rowspan="2" style="text-align: center;"><input type="button" tabindex="23" class="half" title="' + _("forward add record") + '" value="' + _("button add") + '" onclick="addFilteringItem(this.form);"' + disabled + '></td>' +
		'</tr>' +
		'<tr>' +
		'<td><input type="text" tabindex="14" class="normal" name="sip_mask"></td>' +
		'<td><input type="text" tabindex="16" class="short" name="sToPort" disabled="disabled"></td>' +
		'<td><input type="text" tabindex="18" class="normal" name="dip_mask"></td>' +
		'<td><input type="text" tabindex="20" class="short" name="dToPort" disabled="disabled"></td>' +
		'</tr>';

	// Close manager
	table += '</table>';

	var elem = document.getElementById("portFilteringTable");
	if (elem!=null)
		elem.innerHTML = table;
}

function changedefaultFilterPolicy(form)
{
	defaultFilterPolicy = form.defaultFilteringPolicy.value;
}

function protocolChange(form)
{
	var dis = !((form.protocol.value == '1') || (form.protocol.value == '2'));
	form.sFromPort.disabled = dis;
	form.sToPort.disabled   = dis;
	form.dFromPort.disabled = dis;
	form.dToPort.disabled   = dis;
}

function deleteRuleItem(index)
{
	if ((index>=0) && (index < portForwardingRules.length))
	{
		portForwardingRules.splice(index, 1);
		genRulesTable();
	}
}

function deleteForwardingItem(index)
{
	if ((index>=0) && (index < portFilteringRules.length))
	{
		portFilteringRules.splice(index, 1);
		genFilteringTable();
	}
}

function getPortDistance(fromPort, toPort)
{
	var result = -1;

	if (fromPort.value != '')
	{
		if (!validatePort(fromPort, true))
			return;

		result = 0;

		if (toPort.value != '')
		{
			if (!validatePort(toPort, true))
				return;

			// Calculate port distance
			result = toPort.value - fromPort.value;

			if (result < 0)
			{
				alert(_("forward alert port"));
				fromPort.focus();
				return null;
			}
		}
	}
	else if (toPort.value != '')
	{
		alert(_("forward port specified"));
		fromPort.focus();
		return null;
	}

	return result;
}

function checkComment(com)
{
	var comment = com.value;
	if (comment.length > 32)
	{
		alert(_("forward comment too long") + comment);
		com.focus();
		return null;
	}

	return comment.replace(/[,;]/g, ' ');
}

function addRuleItem(form)
{
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
	if (srcDistance >= 0)
	{
		if (dstDistance >= 0) // Destination ports are defined
		{
			if (srcDistance != dstDistance)
			{
				alert(_("forward port not match src") + srcDistance + _("forward port not match dst") + dstDistance);
				return;
			}
		}
	}
	else if (dstDistance >= 0)
	{
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

function checkIPMask(ip, mask)
{
	if (ip.value != '')
	{
		// First validate IP
		if (!validateIP(ip, true))
			return false;

		// Now validate mask if present
		if (mask.value != '')
			if (!validateIPMask(mask, true))
				return false;
	}
	else if (mask.value != '')
	{
		alert(_("forward ip not mask") + mask.value);
		ip.focus();
		return false;
	}

	return true;
}

function addFilteringItem(form)
{
	if (form.mac_address.value != '')
		if (!validateMAC(form.mac_address.value))
		{
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
	if ((form.protocol.value == '1') || (form.protocol.value == '2'))
	{
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

function submitForwardForm(form)
{
	form.portForwardRules.value = genTableData(portForwardingRules, form);
	ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
	return true;
}

function submitFilterForm(form)
{
	form.portFilteringRules.value = genTableData(portFilteringRules, form);
	form.defaultFirewallPolicy.value = defaultFilterPolicy;
	ajaxShowTimer(form, 'timerReloader2', _('message apply'), 15);
	return true;
}

function submitFirewallForm(form)
{
	form.ForwardSesLimit.value = defaultNumber(form.ForwardSesLimit.value, "0");
	ajaxShowTimer(form, 'timerReloader3', _('message apply'), 15);
	return true;
}

function initState()
{
	var opmode = '<% getCfgZero(1, "OperationMode"); %>';
	var pfwEnabled = '<% getCfgZero(1, "PortForwardEnable"); %>';
	var filteringEnabled = '<% getCfgZero(1, "IPPortFilterEnable"); %>';

	displayElement('bridge_warning', opmode == '0'); // bridge mode

	document.portForward.portForwardEnabled.value = (pfwEnabled == '1') ? '1' : '0';
	document.portFiltering.portFilterEnabled.value = (filteringEnabled == '1') ? '1' : '0';

	updateForwardingState(document.portForward);
	updateFilteringState(document.portFiltering);
	initTranslation();
	showWarning();
}

function updateForwardingState(form)
{
	var ena = form.portForwardEnabled.value == '1';
	displayElement('portForwardingRow', ena);
	if (ena)
		genRulesTable();
}

function updateFilteringState(form)
{
	var ena = form.portFilterEnabled.value == '1';
	displayElement('portFilteringRow', ena);
	if (ena)
		genFilteringTable();
}

function genTableData(rules, form)
{
	var values = "";
	for (var i=0; i<rules.length; i++)
	{
		var rule = rules[i];
		var row = '';

		for (var j=0; j<rule.length; j++)
		{
			if (row != '')
				row += ',';
			row += rule[j];
		}
		values += row + ';'
	}
	return values;
}

function initTranslation()
{
  _TR("FirewallTitle", "firewall title");
  _TR("bridge_warning", "firewall bridge warning");
  _TR("FirewallSet", "firewall title");
  _TR("defSesLimit", "firewall defailt session limit");
  _TR("forwardTitle", "forward title");
  _TR("forwardIntroduction", "forward introduction");
  _TR("forwardVirtualSrv", "forward virtual server");
  _TR("ForwardSesLimit", "forward session limit");
  _TR("forwardVirtualSrvSet", "forward virtual server setting");
  _TR("forwardVirtualSrvDisable", "button disable");
  _TR("forwardVirtualSrvEnable", "button enable");
  _TR("portTitle", "port title");
  _TR("portIntroduction", "port introduction");
  _TR("portBasicSet", "port basic setting");
  _TR("portBasicFilter", "port basic filter");
  _TR("portDisable", "button disable");
  _TR("portEnable", "button enable");

  var elements = document.getElementsByTagName('input');
  for (i = 0; i < elements.length; i++)
  	if (elements[i].id == "apply")
  		elements[i].value = _("button apply");
}

</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initState();">
<table class="body">
  <tr id="warning"><tr>
  <tr>
    <td><!-- Port forwarding -->
      <h1 id="forwardTitle">Port Forwarding Settings</h1>
      <p id="forwardIntroduction">Here you can setup port forwarding to provide services to the Internet.</p>
      <hr>
      <form method="POST" name="portForward" action="/goform/portForward" onSubmit="return submitForwardForm(this);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="forwardVirtualSrv">Port Forwarding Settings</td>
          </tr>
          <tr>
            <td class="head" id="forwardVirtualSrvSet"> Port Forwarding Settings </td>
            <td><select onChange="updateForwardingState(this.form);" name="portForwardEnabled" class="half">
                <option value="0" id="forwardVirtualSrvDisable" selected="selected">Disable</option>
                <option value="1" id="forwardVirtualSrvEnable">Enable</option>
              </select></td>
          </tr>
          <tr id="portForwardingRow">
            <td colspan="2" id="portForwardingTable"></td>
          </tr>
        </table>
        <!-- Port forwarding rules -->
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="portForwardRules" value="">
              <input type="submit" class="half" id="apply" value="Apply">
              <input type="hidden" name="submit-url" value="/firewall/firewall.asp" ></td>
          </tr>
        </table>
      </form>
      <!-- MAC / IP / Port Filtering -->
      <h1 id="portTitle">MAC/IP/Port Filtering Settings</h1>
      <p id="portIntroduction">Here you can setup firewall rules to protect your network from malware and other security threats from the Internet.</p>
      <hr>
      <form method="POST" name="portFiltering" action="/goform/portFiltering" onSubmit="return submitFilterForm(this);">
        <iframe name="timerReloader2" id="timerReloader2" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="portBasicSet">Basic Settings</td>
          </tr>
          <tr>
            <td class="head" id="portBasicFilter"> MAC/IP/Port Filtering </td>
            <td><select name="portFilterEnabled" class="half" onChange="updateFilteringState(this.form);">
                <option value="0" id="portDisable" selected="selected">Disable</option>
                <option value="1" id="portEnable">Enable</option>
              </select></td>
          </tr>
          <tr id="portFilteringRow">
            <td id="portFilteringTable" colspan="2"></td>
          </tr>
        </table>
        <!-- MAC / IP / Port filtering rules -->
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="portFilteringRules" value="">
              <input type="hidden" name="defaultFirewallPolicy" value="">
              <input type="submit" class="normal" id="apply" value="Apply">
              <input type="hidden" name="submit-url" value="/firewall/firewall.asp" ></td>
          </tr>
        </table>
      </form>
      <!-- Other netfilter settings -->
      <h1 id="FirewallTitle">Other Settings</h1>
      <div style="display:none;" id="bridge_warning">
      	<p><b>Warning:</b> The current operation mode is "Bridge mode" and these settings may not be functional.</p>
      </div>
      <hr>
      <form method="POST" name="Firewall" action="/goform/setFirewall" onSubmit="return submitFirewallForm(this);">
        <iframe name="timerReloader3" id="timerReloader3" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="FirewallSet">Firewall Settings</td>
          </tr>
          <tr>
            <td class="head" id="ForwardSesLimit">Limit TCP session per ip</td>
            <td><input type="text" class="short" name="ForwardSesLimit" value="<% getCfgZero(1, "ForwardSesLimit"); %>">
            <font color="#808080" id="defSesLimit">(default 0 - disabled)</font></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td>
              <input type="submit" class="normal" id="apply" value="Apply">
              <input type="hidden" name="submit-url" value="/firewall/firewall.asp" >
            </td>
          </tr>
        </table>
      </form>
    </td>
  </tr>
</table>
</body>
</html>
