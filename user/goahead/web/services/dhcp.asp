<!DOCTYPE html>
<html>
<head>
<title>Internet Services Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("internet");
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
	table += '<tr><td class="title" colspan="3">Static IP address assignment table:</td></tr>';
	table += '<tr><th style="text-align: left;">MAC address</th><th style="text-align: left;">IP address</th><th>Action</th></tr>';
	for (var i=0; i<dhcpList.length; i++)
	{
		var row = dhcpList[i];
		table += '<tr><td>' + row[0] + '</td>';
		table += '<td>' + row[1] + '</td>';
		table += '<td style="text-align: center;"><a style="color: #ff0000;" title="Delete record" href="javascript:deleteIPItem(' + i + ');"' + disabled + '><b>[x]</b></a></td></tr>';
	}
	table += '<tr><td><input class="mid" value="" name="dhcpStaticMAC"' + disabled + '></td>';
	table += '<td><input class="mid" value="" name="dhcpStaticIP"' + disabled + '></td>';
	table += '<td style="text-align: center;"><input type="button" class="normal" title="Add record" value="Add" onclick="addIPItem(this.form);"' + disabled + '></td></tr>';
	table += '</table>';
	
	var elem = document.getElementById("dhcpStaticIPList");
	if (elem!=null)
		elem.innerHTML = table;
}

function genIPTableData(form)
{
	var values = "";
	for (var i=0; i<dhcpList.length; i++) {
		values += dhcpList[i][0] + ' ' + dhcpList[i][1];
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
		Alert('You have entered invalid MAC address');
		form.dhcpStaticMAC.focus();
		return;
	}
	if (!validateIP(form.dhcpStaticIP, true))
	{
		Alert('You have entered invalid IP address');
		form.dhcpStaticMAC.focus();
		return;
	}
	
	addEntry(form.dhcpStaticMAC.value, form.dhcpStaticIP.value);
}

function addEntry(mac, ip)
{
	var index = findEntry(mac, null);
	if (index < 0)
	{
		dhcpList.push( [ mac, ip ] );
		genTable();
	}
	else
	{
		if (confirm('Do you want to overwrite existing record in static table for MAC address ' + mac + '?'))
		{
			dhcpList[index][1] = ip;
			genTable();
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
	var dhcp_on = form.lanDhcpType.options.selectedIndex == 1;
	
	enableElements( [ form.dhcpDomain, form.dhcpStart, form.dhcpEnd, form.dhcpMask, form.dhcpGateway, form.dhcpLease ], dhcp_on);
	displayElement( [ 'domain', 'start', 'end', 'mask', 'gateway', 'lease', 'dhcpClientsTable', 'dhcpStaticIPList' ], dhcp_on );
	enableElements( [ form.dhcpPriDns, form.dhcpSecDns ], dhcp_on && (!dnsproxy) );
	displayElement( [ 'pridns', 'secdns' ], !dnsproxy);
	
	genTable(!dhcp_on);
}

function initTranslation()
{
	_TR("lTitle", "services dhcp title");
	_TR("lIntroduction", "services dhcp introduction");
	_TR("lSetup", "services dhcp setup");

	_TR("lDhcpType", "service dhcp title");
	_TR("lDhcpTypeD", "button disable");
	_TR("lDhcpTypeS", "button enable");
	_TR("lDhcpStart", "lan dhcp start");
	_TR("lDhcpEnd", "lan dhcp end");
	_TR("lDhcpNetmask", "inet netmask");
	_TR("lDhcpPriDns", "inet pri dns");
	_TR("lDhcpSecDns", "inet sec dns");
	_TR("lDhcpGateway", "inet gateway");
	_TR("lDhcpLease", "lan dhcp lease");
	
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
			addEntry(row[0], row[1]);
		}
	}

	initTranslation();

	form.lanDhcpType.options.selectedIndex = 1*dhcp;
	dhcpTypeSwitch();
	
	loadDhcpClientsList();
}

function CheckValue(form)
{
	if (form.lanDhcpType.options.selectedIndex == 1)
	{
		if (!validateIP(form.dhcpStart, true))
		{
			form.dhcpStart.focus();
			return false;
		}
		if (!validateIP(form.dhcpEnd, true))
		{
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
				form.dhcpPriDns.focus();
				return false;
			}
		}
		if (form.dhcpSecDns.value != "")
		{
			if (!validateIP(form.dhcpSecDns, true))
			{
				form.dhcpSecDns.focus();
				return false;
			}
		}
		if (!validateIP(form.dhcpGateway, true))
		{
			form.dhcpGateway.focus();
			return false;
		}
		
		genIPTableData(form);
	}
	
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
		var index = findEntry(mac);
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

		addEntry(mac, ip);
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

</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="lTitle"></h1>
      <p id="lIntroduction"></p>
      <hr>
      <div id="dhcpClientsTable"> </div>
      <form method="POST" name="dhcpCfg" action="/goform/setDhcp" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="lSetup">DHCP Server Setup</td>
          </tr>
          <tr>
            <td class="head" id="lDhcpType">DHCP Server</td>
            <td><select name="lanDhcpType" class="mid" onChange="dhcpTypeSwitch();">
                <option value="DISABLE" id="lDhcpTypeD">Disabled</option>
                <option value="SERVER" id="lDhcpTypeS">Enabled</option>
              </select></td>
          </tr>
          <tr>
            <td class="head">DHCP Domain</td>
            <td><input name="dhcpDomain" class="mid" value="<% getCfgGeneral(1, "dhcpDomain"); %>"></td>
          </tr>
          <tr id="start">
            <td class="head" id="lDhcpStart">DHCP Pool Start IP</td>
            <td><input name="dhcpStart" class="mid" value="<% getCfgGeneral(1, "dhcpStart"); %>"></td>
          </tr>
          <tr id="end">
            <td class="head" id="lDhcpEnd">DHCP Pool End IP</td>
            <td><input name="dhcpEnd" class="mid" value="<% getCfgGeneral(1, "dhcpEnd"); %>"></td>
          </tr>
          <tr id="mask">
            <td class="head" id="lDhcpNetmask">DHCP Subnet Mask</td>
            <td><input name="dhcpMask" class="mid" value="<% getCfgGeneral(1, "dhcpMask"); %>"></td>
          </tr>
          <tr id="pridns">
            <td class="head" id="lDhcpPriDns">DHCP Primary DNS</td>
            <td><input name="dhcpPriDns" class="mid" value="<% getCfgGeneral(1, "dhcpPriDns"); %>"></td>
          </tr>
          <tr id="secdns">
            <td class="head" id="lDhcpSecDns">DHCP Secondary DNS</td>
            <td><input name="dhcpSecDns" class="mid" value="<% getCfgGeneral(1, "dhcpSecDns"); %>"></td>
          </tr>
          <tr id="gateway">
            <td class="head" id="lDhcpGateway">DHCP Default Gateway</td>
            <td><input name="dhcpGateway" class="mid" value="<% getCfgGeneral(1, "dhcpGateway"); %>"></td>
          </tr>
          <tr id="lease">
            <td class="head" id="lDhcpLease">DHCP Lease Time (in seconds)</td>
            <td><input name="dhcpLease" class="mid" value="<% getCfgGeneral(1, "dhcpLease"); %>"></td>
          </tr>
        </table>
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
