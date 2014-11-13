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
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("internet");

var secs;
var timerID = null;
var timerRunning = false;

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

function display_on()
{
	if (window.ActiveXObject) // IE
		return "block";
	else if (window.XMLHttpRequest) // Mozilla, Firefox, Safari,...
		return "table-row";
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
	_TR("lLan2Enable", "inet enable");
	_TR("lLan2Disable", "inet disable");
	_TR("lLan2Ip", "inet lan2 ip");
	_TR("lLan2Netmask", "inet lan2 netmask");
	_TR("lGateway", "inet gateway");
	_TR("lPriDns", "inet pri dns");
	_TR("lSecDns", "inet sec dns");
	_TR("lMac", "inet mac");

	_TRV("lApply", "inet apply");
	_TRV("lCancel", "inet cancel");
}

function initValue()
{
	var form = document.spotCfg;
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	document.spotCfg.sRadCoaNoIpCheck.checked = ("<% getCfgZero(1, "chilli_coanoipcheck"); %>" == "on");
	document.spotCfg.sUamAnyDns.checked = ("<% getCfgZero(1, "chilli_uamanydns"); %>" == "on");

	initTranslation();

}

function CheckValue()
{
	var form = document.spotCfg;
	
	var chkIp = [form.sIp, form.sNetmask, form.sPriDns, form.sSecDns];
	var chkSpace = [form.sDomain, form.sRadServer1, form.sRadServer2, 
	    form.sRadSecret, form.sNasId, form.sRadLocationId, form.sRadLocationName,
	    form.sUamServer, form.sUamHomepage, form.sUamSecret, form.sUamAllowed, form.sMacAllowed];
	var chkInt = [form.sLease, form.sRadCoaPort];
	var obj;
	
	for(var i=0; i<chkIp.length; i++) {
		obj = chkIp[i];
		if (!validateIP(obj, true))
		{
			obj.focus();
			obj.select();
			return false;
		}
	}
	for(var i=0; i<chkSpace.length; i++) {
		obj = chkIp[i];
		if (obj.value.indexOf(" ") >= 0)
		{
			alert('Space in field is not allowed!');
			obj.focus();
			obj.select();
			return false;
		}
	}
	for(var i=0; i<chkInt.length; i++) {
		obj = chkIp[i];
		if (!/^\d+$/.test(obj.value))
		{
			alert('This field expects number!');
			obj.focus();
			obj.select();
			return false;
		}
	}
	return true;
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="lTitle"></h1>
      <p id="lIntroduction"></p>
      <hr />
      <form method="POST" name="spotCfg" action="/goform/setHotspot" onSubmit="return CheckValue();">
        <table width="95%" border="1" cellpadding="2" cellspacing="1">
          <tr>
            <td class="title" colspan="2" id="spotSetup">Hotspot Setup</td>
          </tr>
          <!================ Hotspot variables ==========================>
          <tr>
            <td class="head" id="sIp">Subnet Address</td>
            <td><input name="sIp" maxlength="15" value="<% getSpotIp(); %>" ></td>
          </tr>
          <tr>
            <td class="head" id="sNetmask">Subnet Mask</td>
            <td><input name="sNetmask" maxlength="15" value="<% getSpotNetmask(); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sPriDns">Primary DNS Server</td>
            <td><input name="sPriDns" maxlength="15" value="<% getCfgGeneral(1, "chilli_dns1"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sSecDns">Secondary DNS Server</td>
            <td><input name="sSecDns" maxlength="15" value="<% getCfgGeneral(1, "chilli_dns2"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sDomain">Domain name</td>
            <td><input name="sDomain" maxlength="31" value="<% getCfgGeneral(1, "chilli_domain"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sLease">Lease time (sec)</td>
            <td><input name="sLease" maxlength="8" value="<% getCfgGeneral(1, "chilli_lease"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadServer1">Radius server auth address</td>
            <td><input name="sRadServer1" maxlength="15" value="<% getCfgGeneral(1, "chilli_radiusserver1"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadServer2">Radius server acct address</td>
            <td><input name="sRadServer2" maxlength="15" value="<% getCfgGeneral(1, "chilli_radiusserver2"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadSecret">Radius server secret</td>
            <td><input name="sRadSecret" maxlength="31" value="<% getCfgGeneral(1, "chilli_radiussecret"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sNasId">Radius NAS ID</td>
            <td><input name="sNasId" maxlength="31" value="<% getCfgGeneral(1, "chilli_radiusnasid"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadLocId">Radius Location ID</td>
            <td><input name="sRadLocationId" maxlength="15" value="<% getCfgGeneral(1, "chilli_radiuslocationid"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadLocName">Radius Location Name</td>
            <td><input name="sRadLocationName" maxlength="15" value="<% getCfgGeneral(1, "chilli_radiuslocationname"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadCoaPort">Radius CoA/DM port</td>
            <td><input name="sRadCoaPort" maxlength="15" value="<% getCfgGeneral(1, "chilli_coaport"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadCoaNoIpCheck">No IP check for CoA/DM request</td>
            <td><input name="sRadCoaNoIpCheck" type="checkbox"></td>
          </tr>
          <tr>
            <td class="head" id="sUamServer">UAM server URL</td>
            <td><input name="sUamServer" maxlength="63" value="<% getCfgGeneral(1, "chilli_uamserver"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamHomepage">UAM homepage URL</td>
            <td><input name="sUamHomepage" maxlength="63" value="<% getCfgGeneral(1, "chilli_uamhomepage"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamSecret">UAM secret</td>
            <td><input name="sUamSecret" maxlength="31" value="<% getCfgGeneral(1, "chilli_uamsecret"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamAllowed">UAM allowed hosts</td>
            <td><input name="sUamAllowed" maxlength="127" value="<% getCfgGeneral(1, "chilli_uamallowed"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamAnyDNS">Allow all DNS requests</td>
            <td><input name="sUamAnyDns" type="checkbox"></td>
          </tr>
          <tr>
            <td class="head" id="sMacAllowed">Allowed MAC</td>
            <td><input name="sMacAllowed" maxlength="127" value="<% getCfgGeneral(1, "chilli_macallowed"); %>"></td>
          </tr>
        </table>
        <table width="95%" cellpadding="2" cellspacing="1">
          <tr align="center">
            <td><input type="submit" class="normal" value="Apply" id="lApply" onClick="TimeoutReload(20);">
              &nbsp;&nbsp;
              <input type="reset"  class="normal" value="Cancel" id="lCancel" onClick="window.location.reload();"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
