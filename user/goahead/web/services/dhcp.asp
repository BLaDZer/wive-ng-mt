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
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("services");
Butterlate.setTextDomain("buttons");

var secs;
var timerID = null;
var timerRunning = false;

var dhcpList = [];

function genTable(disabled)
{
	disabled = (disabled) ? ' disabled="disabled"' : '';

	var table = '<table class="form" style="width: 100%">';
	table += '<tr><td class="title" colspan="4">' + _("services dhcp static title") + '</td></tr>';
	table += '<tr><th style="text-align: left;">' + _("inet mac") + '</th>';
	table += '<th style="text-align: left;">' + _("inet ip") + '</th>';
	table += '<th style="text-align: left;">' + _("services dhcp table desc") + '</th>';
	table += '<th>' + _("routing action") + '</th></tr>';
	for (var i=0; i<dhcpList.length; i++)
	{
		var row = dhcpList[i];
		table += '<tr><td>' + row[0] + '</td>';
		table += '<td>' + row[1] + '</td>';
		table += '<td>' + row[2] + '</td>';
		table += '<td style="text-align: center;">';
		table += '<a style="color: #ff0000;" title="';
		table += _("services dhcp edit record");
		table += '" href="javascript:editIPItem(' + i + ');"' + disabled + '>';
		table += '<img src="/graphics/edit.png" alt="[+]"></a>';
		table += '<a style="color: #ff0000;" title="';
		table += _("services dhcp delete record");
		table += '" href="javascript:deleteIPItem(' + i + ');"' + disabled + '>';
		table += '<img src="/graphics/cross.png" alt="[x]"></a></td></tr>';
	}
	table += '<tr><td><input class="mid" value="" name="dhcpStaticMAC"' + disabled + '></td>';
	table += '<td><input class="mid" value="" name="dhcpStaticIP"' + disabled + '></td>';
	table += '<td><input class="half" value="" name="dhcpStaticDesc"' + disabled + '></td>';
	table += '<td style="text-align: center;"><input type="button" class="normal" title="';
	table += _("services dhcp add record");
	table += '" value="';
	table += _("button add");
	table += '" onclick="addIPItem(this.form);"' + disabled + '></td></tr>';
	table += '</table>';
	
	var elem = document.getElementById("dhcpStaticIPList");
	if (elem!=null)
		elem.innerHTML = table;
}

function genIPTableData(form)
{
	var values = "";
	for (var i=0; i<dhcpList.length; i++) {
		values += dhcpList[i][0] + ' ' + dhcpList[i][1] + ' ' + dhcpList[i][2];
		if (dhcpList.length > (i+1)) {
			values += ";";
		}
	}
	form.dhcpAssignIP.value = values;
}

function addIPItem(form)
{
	if (!validateMAC(form.dhcpStaticMAC.value, true))
	{
		alert(_("services dhcp invalid mac"));
		form.dhcpStaticMAC.focus();
		return;
	}
	if (!validateIP(form.dhcpStaticIP, true))
	{
		alert(_("services dhcp invalid ip"));
		form.dhcpStaticIP.focus();
		return;
	}

	var re_desc = /^[a-zA-Z0-9_]+$/;
	if (!re_desc.test(form.dhcpStaticDesc.value))
	{
		alert(_("services dhcp invalid desc"));
		form.dhcpStaticDesc.focus();
		return;
	}
	
	addEntry(form.dhcpStaticMAC.value, form.dhcpStaticIP.value, form.dhcpStaticDesc.value);
}

function addEntry(mac, ip, desc)
{
	var index = findEntry(mac, ip);
	if (index < 0)
	{
		dhcpList.push( [ mac, ip, desc ] );
		genTable();
	}
	else
	{
		if (dhcpList[index][0] == mac){
			if (confirm(_("services dhcp ask overwrite mac") + mac + '?'))
			{
				dhcpList[index][1] = ip;
				dhcpList[index][2] = desc;
				genTable();
			}
		}
		else if (dhcpList[index][1] == ip)
		{
			if (confirm(_("services dhcp ask overwrite ip") + ip + '?'))
			{
				dhcpList[index][0] = mac;
				dhcpList[index][2] = desc;
				genTable();
			}
		}
	}
}

function deleteIPItem(index)
{
	if ((index>=0) && (index < dhcpList.length))
	{
		var row = dhcpList[index];
		dhcpList.splice(index, 1);
		
		// Update DHCP table
		var tbl = document.getElementById('dhcpClientsTable');
		if (tbl != null)
			updateDhcpClientsList(tbl);
		
		genTable();
	}
}

function editIPItem(index)
{
	var form = document.dhcpCfg;
	if ((index>=0) && (index < dhcpList.length))
	{
		var row = dhcpList[index];
		form.dhcpStaticMAC.value = row[0];
		form.dhcpStaticIP.value = row[1];
		form.dhcpStaticDesc.value = row[2];
		form.dhcpStaticDesc.focus();
	}
}

function StartTheTimer()
{
	if (secs==0)
	{
		TimeoutReload(15);
		//window.location.reload();
		window.location.href=window.location.href;	//reload page
	}
	else
	{
		self.status = secs;
		secs = secs - 1;
		timerRunning = true;
		timerID = self.setTimeout("StartTheTimer()", 1000);
	}
}

function TimeoutReload(timeout)
{
	secs = timeout;
	if (timerRunning)
		clearTimeout(timerID);
	timerRunning = false;
	StartTheTimer();
}

function dhcpTypeSwitch()
{
	var form = document.dhcpCfg;
	var dnsproxy = <% getCfgZero(1, "dnsPEnabled"); %> * 1;
	var dhcp_on = form.dhcpEnabled.options.selectedIndex == 1;
	
	enableElements( [ form.dhcpDomain, form.dhcpStart, form.dhcpEnd, form.dhcpMask, form.dhcpGateway, form.dhcpLease, form.dhcpARPPTimeout ], dhcp_on);
	displayElement( [ 'domain', 'start', 'end', 'mask', 'gateway', 'lease', 'dhcpClientsTable', 'dhcpStaticIPList', 'arpping' ], dhcp_on );
	enableElements( [ form.dhcpPriDns, form.dhcpSecDns ], dhcp_on && (!dnsproxy) );
	displayElement( [ 'pridns', 'secdns' ], !dnsproxy && dhcp_on);
	
	genTable(!dhcp_on);
	displayServiceStatus();
}

function initTranslation()
{
	_TR("lTitle", "services dhcp title");
	_TR("lIntroduction", "services dhcp introduction");
	_TR("lSetup", "services dhcp setup");

	_TR("lDhcpType", "services dhcp title");
	_TR("lDhcpTypeD", "button disable");
	_TR("lDhcpTypeS", "button enable");
	_TR("dhcpDomain", "services dhcp domain");
	_TR("lDhcpStart", "services dhcp start");
	_TR("lDhcpEnd", "services dhcp end");
	_TR("lDhcpNetmask", "inet netmask");
	_TR("lDhcpPriDns", "inet pri dns");
	_TR("lDhcpSecDns", "inet sec dns");
	_TR("lDhcpGateway", "inet gateway");
	_TR("lDhcpLease", "services dhcp lease");
	_TR("dClients", "services dhcp clients");
	_TR("dHostname", "inet hostname");
	_TR("dMAC", "inet mac");
	_TR("dIP", "inet ip");
	_TR("dExpires", "services dhcp expires");
	_TR("dStatic", "services dhcp static");
	_TR("ldhcpARPPTimeout", "services dhcp arpping timeout");
	
	_TRV("lApply", "button apply");
	_TRV("lCancel", "button cancel");
}

function initValue()
{
	var form = document.dhcpCfg;
	var dhcp = <% getCfgZero(1, "dhcpEnabled"); %>;
	var dhcp_static = "<% getCfgZero(1, "dhcpStatic"); %>";

	if (dhcp_static.length > 1) {
		dhcp_static = dhcp_static.split(";");
		for (var i=0; i<dhcp_static.length; i++) {
			var row = dhcp_static[i].split(" ");
			if (row.length < 3)
				row[2] = "";
			addEntry(row[0], row[1], row[2]);
		}
	}

	initTranslation();

	form.dhcpEnabled.options.selectedIndex = 1*dhcp;
	dhcpTypeSwitch();
	
	loadDhcpClientsList();
}

function CheckValue(form)
{
	if (form.dhcpEnabled.options.selectedIndex == 1)
	{
		if (!validateIP(form.dhcpStart, true))
		{
			alert(_("services dhcp invalid ip"));
			form.dhcpStart.focus();
			return false;
		}
		if (!validateIP(form.dhcpEnd, true))
		{
			alert(_("services dhcp invalid ip"));
			form.dhcpEnd.focus();
			return false;
		}
		if (!validateIP(form.dhcpMask, true))
		{
			form.dhcpMask.focus();
			return false;
		}
		if (form.dhcpPriDns.value != "")
		{
			if (!validateIP(form.dhcpPriDns, true))
			{
				alert(_("services dhcp invalid ip"));
				form.dhcpPriDns.focus();
				return false;
			}
		}
		if (form.dhcpSecDns.value != "")
		{
			if (!validateIP(form.dhcpSecDns, true))
			{
				alert(_("services dhcp invalid ip"));
				form.dhcpSecDns.focus();
				return false;
			}
		}
		if (!validateIP(form.dhcpGateway, true))
		{
			alert(_("services dhcp invalid ip"));
			form.dhcpGateway.focus();
			return false;
		}
		
		genIPTableData(form);
	}
	ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
	return true;
}

function updateDhcpClientsList(element)
{
	// Update all checkboxes
	var rows = element.getElementsByTagName('INPUT');
	for (var i=0; i<rows.length; i++)
	{
		// Get check ID
		var id = rows[i].id;
		if (id == null)
			next;
		// Get values
		var ip = document.getElementById(id + '_ip');
		if (ip == null)
			next;
		ip = ip.innerHTML;
		var mac = document.getElementById(id + '_mac');
		if (mac == null)
			next;
		mac = mac.innerHTML;
		
		// Set-up checked value
		var index = findEntry(mac, null);
		rows[i].checked = (index >= 0);
		
		var status = document.getElementById(id + '_status');
		if (status != null)
		{
			if (index >= 0)
				status.style.backgroundColor = (ip == dhcpList[index][1]) ? '#3da42c' : '#dd3b3b';
			else
				status.style.backgroundColor = 'inherit';
		}
	}
}

function loadDhcpClientsList()
{
	var reloader = function(element)
	{
		initTranslation();
		updateDhcpClientsList(element);
		self.setTimeout('loadDhcpClientsList();', 5000);
	}

	ajaxLoadElement("dhcpClientsTable", "/services/dhcp_clist.asp", reloader);
}

function findEntry(mac, ip)
{
	// Check if item not exists
	for (var i=0; i<dhcpList.length; i++)
	{
		var row = dhcpList[i];
		if (row[0] == mac)
			return i;
		if (row[1] == ip)
			return i;
	}
	
	return -1;
}

function toggleDhcpTable(check)
{
	// Get check ID
	var id = check.id;
	if (id == null)
		return;
	// Get values
	var ip = document.getElementById(id + '_ip');
	if (ip == null)
		return;
	ip = ip.innerHTML;
	var mac = document.getElementById(id + '_mac');
	if (mac == null)
		return;
	mac = mac.innerHTML;
	
	// Check action
	if (check.checked) // Add item to list
	{
		if (!validateMAC(mac, true))
			return;
		if (!validateIP(ip, true))
			return;

		addEntry(mac, ip, "");
	}
	else // Remove item from list
	{
		for (var i=0; i<dhcpList.length; i++)
		{
			var row = dhcpList[i];
			if ((row[0] == mac) && (row[1] == ip))
			{
				dhcpList.splice(i, 1);
				genTable();
				return;
			}
		}
	}
}

function displayServiceHandler(response)
{
  var form = document.l2tpConfig;

  var services = [
    // turned_on, row_id, daemon_id
    [ '<% getCfgGeneral(1, "dhcpEnabled"); %>', 'udhcpd', 'udhcpd' ],
  ];

  // Create associative array
  var tmp = response.split(',');
  var daemons = [];
  for (var i=0; i<tmp.length; i++)
    daemons[tmp[i]] = 1;

  // Now display all services
  for (var i=0; i<services.length; i++)
  {
    var service = services[i];
    var row = document.getElementById(service[1]);
    var tds = [];
    for (var j=0; j<row.childNodes.length; j++)
      if (row.childNodes[j].nodeName == 'TD')
        tds.push(row.childNodes[j]);

    if (row != null)
    {
      // Fill-up status
      if (service[0]*1 == 0)
        tds[2].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
      else
        tds[2].innerHTML = (daemons[service[2]] == 1) ?
          '<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
          '<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
    }
  }

  serviceStatusTimer = setTimeout('displayServiceStatus();', 5000);
}

function displayServiceStatus()
{
  ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
}

</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="lTitle"></h1>
      <p id="lIntroduction"></p>
      <hr>
      <form method="POST" name="dhcpCfg" action="/goform/setDhcp" onSubmit="return CheckValue(this);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="3" id="lSetup">DHCP Server Setup</td>
          </tr>
          <tr id="udhcpd">
            <td class="head" id="lDhcpType">DHCP Server</td>
            <td><select name="dhcpEnabled" class="mid" onChange="dhcpTypeSwitch();">
                <option value="0" id="lDhcpTypeD">Disabled</option>
                <option value="1" id="lDhcpTypeS">Enabled</option>
              </select></td>
            <td style="width: 56px;">&nbsp;</td>
          </tr>
          <tr id="domain">
            <td class="head" id="dhcpDomain">DHCP Domain</td>
            <td colspan="2"><input name="dhcpDomain" class="mid" value="<% getCfgGeneral(1, "dhcpDomain"); %>"></td>
          </tr>
          <tr id="start">
            <td class="head" id="lDhcpStart">DHCP Pool Start IP</td>
            <td colspan="2"><input name="dhcpStart" class="mid" value="<% getCfgGeneral(1, "dhcpStart"); %>"></td>
          </tr>
          <tr id="end">
            <td class="head" id="lDhcpEnd">DHCP Pool End IP</td>
            <td colspan="2"><input name="dhcpEnd" class="mid" value="<% getCfgGeneral(1, "dhcpEnd"); %>"></td>
          </tr>
          <tr id="mask">
            <td class="head" id="lDhcpNetmask">DHCP Subnet Mask</td>
            <td colspan="2"><input name="dhcpMask" class="mid" value="<% getCfgGeneral(1, "dhcpMask"); %>"></td>
          </tr>
          <tr id="pridns">
            <td class="head" id="lDhcpPriDns">DHCP Primary DNS</td>
            <td colspan="2"><input name="dhcpPriDns" class="mid" value="<% getCfgGeneral(1, "dhcpPriDns"); %>"></td>
          </tr>
          <tr id="secdns">
            <td class="head" id="lDhcpSecDns">DHCP Secondary DNS</td>
            <td colspan="2"><input name="dhcpSecDns" class="mid" value="<% getCfgGeneral(1, "dhcpSecDns"); %>"></td>
          </tr>
          <tr id="gateway">
            <td class="head" id="lDhcpGateway">DHCP Default Gateway</td>
            <td colspan="2"><input name="dhcpGateway" class="mid" value="<% getCfgGeneral(1, "dhcpGateway"); %>"></td>
          </tr>
          <tr id="lease">
            <td class="head" id="lDhcpLease">DHCP Lease Time (in seconds)</td>
            <td colspan="2"><input name="dhcpLease" class="mid" value="<% getCfgGeneral(1, "dhcpLease"); %>"></td>
          </tr>
          <tr id="arpping">
            <td class="head" id="ldhcpARPPTimeout">DHCP ARP ping timeout (in ms)</td>
            <td colspan="2"><input name="dhcpARPPTimeout" class="mid" value="<% getCfgGeneral(1, "dhcpARPPTimeout"); %>"></td>
          </tr>
        </table>
        <div id="dhcpClientsTable"> </div>
        <div id="dhcpStaticIPList"> </div>
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="dhcpAssignIP" value="">
              <input type="submit" class="normal" value="Apply" id="lApply" onClick="TimeoutReload(20);">
              &nbsp;&nbsp;
              <input type="reset"  class="normal" value="Cancel" id="lCancel" onClick="window.location.reload();">
              <input type="hidden" value="/services/dhcp.asp" name="submit-url"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
