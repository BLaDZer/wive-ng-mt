<!DOCTYPE html>
<html>
<head>
<title>Local Area Network (LAN) Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("buttons");

var secs;
var timerID = null;
var timerRunning = false;
var opmode = "<% getCfgZero(1, "OperationMode"); %>";
var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
var lanip = "<% getLanIp(); %>";
var lanmask = "<% getLanNetmask(); %>";
var hostname = "<% getCfgGeneral(1, "HostName"); %>";
var lan2 = "<% getCfgZero(1, "Lan2Enabled"); %>";
var dhcp = "<% getCfgZero(1, "dhcpEnabled"); %>";

function StartTheTimer()
{
	if (secs==0)
	{
		TimeoutReload(10);
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

function initTranslation()
{
	_TR("lTitle", "lan title");
	_TR("lIntroduction", "lan introduction");
	_TR("lSetup", "lan setup");

	_TR("lHostname", "inet hostname");
	_TR("lIp", "inet ip");
	_TR("lNetmask", "inet netmask");
	_TR("lLan2", "inet lan2");
	_TR("lLan2Ip", "inet lan2 ip");
	_TR("lLan2Netmask", "inet lan2 netmask");
	_TR("lGateway", "inet gateway");
	_TR("lPriDns", "inet pri dns");
	_TR("lSecDns", "inet sec dns");
	_TR("lMac", "inet mac");
	_TR("lEnable", "button enable");
	_TR("lDisable", "button disable");

	_TRV("lApply", "button apply");
	_TRV("lCancel", "button cancel");
}

function initValue()
{
	var form = document.lanCfg;

	initTranslation();

	form.lanIp.value = lanip;
	form.lanNetmask.value = lanmask;
	form.hostname.value = hostname;

	// Lan2
	form.lan2Ip.value = '<% getCfgGeneral(1, "lan2_ipaddr"); %>';
	form.lan2Netmask.value = '<% getCfgGeneral(1, "lan2_netmask"); %>';
	form.lan2enabled.value = (lan2 == "1") ? "1" : "0";
	lan2_enable_switch(form);

	//gateway, dns only allow to configure at bridge mode
	displayElement( [ 'brGateway', 'brPriDns', 'brSecDns' ], opmode == "0" );
}

function CheckValue()
{
	var form = document.lanCfg;
	var dhcp_edit = 0;
	if (!validateBlanksList( [ form.hostname ] ))
		return false;

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
	
	if (form.lan2enabled.value == "1")
	{
		if (!validateIP(form.lan2Ip, true))
		{
			form.lan2Ip.focus();
			return false;
		}
		if (!validateIPMask(form.lan2Netmask, true))
		{
			form.lan2Netmask.focus();
			return false;
		}
	}
	if ((form.lanIp.value != lanip) && (dhcp == "1"))
		dhcp_edit = 1;
	if ((form.lanNetmask.value != lanmask) && (dhcp == "1"))
		dhcp_edit = 1;
	if (dhcp_edit == 1)
		dhcp(form);
	return true;
}

function lan2_enable_switch(form)
{
	enableElements( [ form.lan2Ip, form.lan2Netmask ], (form.lan2enabled.value == "1"));
	displayElement( [ 'lan2ip', 'lan2mask' ], (form.lan2enabled.value == "1"));
}

function dhcp(form) {
	if (confirm(_("lan accept dhcp opts"))) {
		var ip = form.lanIp.value.split(".");
		var mask = form.lanNetmask.value.split(".");
		var ip_start = [];
		var ip_end = [];
		for (var i = 0; i < 4; i++) {
			ip_start[i] = ip[i] & mask[i];
			ip_end[i] = ip[i] | (~ mask[i] & 0xff);
		}
		form.dhcpStart.value = ip_start[0] + "." + ip_start[1] + "." + ip_start[2] + "." + ip_start[3];
		form.dhcpEnd.value = ip_end[0] + "." + ip_end[1] + "." + ip_end[2] + "." + ip_end[3];
		form.dhcpGateway.value = form.lanIp.value;
	}
}

</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="lTitle"></h1>
      <p id="lIntroduction"></p>
      <hr>
      <form method="POST" name="lanCfg" action="/goform/setLan" onSubmit="return CheckValue();">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="lSetup">LAN Interface Setup</td>
          </tr>
          <tr <% var hashost = getHostSupp();
	if (hashost != "1") write("style=\"visibility:hidden;display:none\""); %>>
            <td class="head" id="lHostname">Hostname</td>
            <td><input name="hostname" class="mid" value=""></td>
          </tr>
          <tr>
            <td class="head" id="lIp">IP Address</td>
            <td><input name="lanIp" class="mid" value=""></td>
          </tr>
          <tr>
            <td class="head" id="lNetmask">Subnet Mask</td>
            <td><input name="lanNetmask" class="mid" value=""></td>
          </tr>
          </tr>
          <tr>
            <td class="head" id="lLan2">LAN2</td>
            <td><select name="lan2enabled" onChange="lan2_enable_switch(this.form);" class="half">
                <option value="1" id="lEnable">Enabled</option>
                <option value="0" id="lDisable">Disabled</option>
              </select></td>
          </tr>
          <tr id="lan2ip">
            <td class="head" id="lLan2Ip">LAN2 IP Address</td>
            <td><input name="lan2Ip" class="mid" value=""></td>
          </tr>
          <tr id="lan2mask">
            <td class="head" id="lLan2Netmask">LAN2 Subnet Mask</td>
            <td><input name="lan2Netmask" class="mid" value=""></td>
          </tr>
          <tr id="brGateway">
            <td class="head" id="lGateway">Default Gateway</td>
            <td><input name="lanGateway" class="mid" value="<% getCfgGeneral(1, "wan_gateway"); %>"></td>
          </tr>
          <tr id="brPriDns">
            <td class="head" id="lPriDns">Primary DNS Server</td>
            <td><input name="lanPriDns" class="mid" value="<% getDns(1); %>"></td>
          </tr>
          <tr id="brSecDns">
            <td class="head" id="lSecDns">Secondary DNS Server</td>
            <td><input name="lanSecDns" class="mid" value="<% getDns(2); %>"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="submit" class="normal" value="Apply" id="lApply" onClick="TimeoutReload(20);">
              &nbsp;&nbsp;
              <input type="reset"  class="normal" value="Cancel" id="lCancel" onClick="window.location.reload();">
              <input type="hidden" value="/internet/lan.asp" name="submit-url" >
              <input type="hidden" value="<% getCfgGeneral(1, "dhcpStart"); %>" name="dhcpStart">
              <input type="hidden" value="<% getCfgGeneral(1, "dhcpEnd"); %>" name="dhcpEnd">
              <input type="hidden" value="<% getCfgGeneral(1, "dhcpGateway"); %>" name="dhcpGateway">
              </td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
