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

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("buttons");

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

function initTranslation()
{
	_TR("sTitle", "hotspot title");
	_TR("sIntroduction", "hotspot introduction");
	_TR("spotSetup", "hotspot setup");
	_TR("spotEnable", "hotspot enable");
	_TR("sIp", "inet ip");
	_TR("sNetwork", "inet netmask");
	_TR("sPriDns", "inet pri dns");
	_TR("sSecDns", "inet sec dns");
	_TR("sDomain", "hotspot domain");
	_TR("sLease", "hotspot lease");
	_TR("sRadServer1", "hotspot radius auth");
	_TR("sRadServer2", "hotspot radius acct");
	_TR("sRadSecret", "hotspot radius secret");
	_TR("sNasId", "hotspot radius nas");
	_TR("sRadLocId", "hotspot radius location id");
	_TR("sRadLocName", "hotspot radius location name");
	_TR("sRadCoaPort", "hotspot radius coa port");
	_TR("sRadCoaNoIpCheck", "hotspot radius no check ip");
	_TR("sUamServer", "hotspot uam server");
	_TR("sUamHomepage", "hotspot uam homepage");
	_TR("sUamSecret", "hotspot uam secret");
	_TR("sUamAllowed", "hotspot uam allowed");
	_TR("ssUamDomain", "hotspot uam domain");
	_TR("sUamAnyDNS", "hotspot uam any dns");
	_TR("sMacAllowed", "hotspot uam mac");

	_TRV("sApply", "button apply");
	_TRV("sCancel", "button cancel");
}

function initValue()
{
	var form = document.spotCfg;
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	document.spotCfg.sRadCoaNoIpCheck.checked = ("<% getCfgZero(1, "chilli_coanoipcheck"); %>" == "on");
	document.spotCfg.spotEnable.checked = ("<% getCfgZero(1, "hotspot"); %>" == "on");
	document.spotCfg.sUamAnyDns.checked = ("<% getCfgZero(1, "chilli_uamanydns"); %>" == "on");

	initTranslation();

}

function CheckValue()
{
	var form = document.spotCfg;

	var chkIp = [form.sIp, form.sNetmask, form.sPriDns, form.sSecDns];
	var chkSpace = [form.sDomain, form.sRadServer1, form.sRadServer2, 
	    form.sRadSecret, form.sNasId, form.sRadLocationId, form.sRadLocationName,
	    form.sUamServer, form.sUamHomepage, form.sUamSecret, form.sUamAllowed, form.sUamDomain, form.sMacAllowed];
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
			alert(_("hotspot dont space"));
			obj.focus();
			obj.select();
			return false;
		}
	}
	for(var i=0; i<chkInt.length; i++) {
		obj = chkIp[i];
		if (!/^\d+$/.test(obj.value))
		{
			alert(_("hotspot expects number"));
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
    <td><h1 id="sTitle"></h1>
      <p id="sIntroduction"></p>
      <hr />
      <form method="POST" name="spotCfg" action="/goform/setHotspot" onSubmit="return CheckValue();">
        <table width="95%" border="1" cellpadding="2" cellspacing="1">
          <tr>
            <td class="title" colspan="2" id="spotSetup">Hotspot Setup</td>
          </tr>
          <!================ Hotspot variables ==========================>
          <tr>
            <td class="head" id="spotEnable">Enable hotspot</td>
            <td><input name="spotEnable" type="checkbox"></td>
          </tr>
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
            <td><input name="sDomain" maxlength="64" value="<% getCfgGeneral(1, "chilli_domain"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sLease">Lease time (sec)</td>
            <td><input name="sLease" maxlength="8" value="<% getCfgGeneral(1, "chilli_lease"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadServer1">Radius server auth address</td>
            <td><input name="sRadServer1" maxlength="256" value="<% getCfgGeneral(1, "chilli_radiusserver1"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadServer2">Radius server acct address</td>
            <td><input name="sRadServer2" maxlength="256" value="<% getCfgGeneral(1, "chilli_radiusserver2"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sRadSecret">Radius server secret</td>
            <td><input name="sRadSecret" maxlength="256" value="<% getCfgGeneral(1, "chilli_radiussecret"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sNasId">Radius NAS ID</td>
            <td><input name="sNasId" maxlength="256" value="<% getCfgGeneral(1, "chilli_radiusnasid"); %>"></td>
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
            <td><input name="sUamServer" maxlength="256" value="<% getCfgGeneral(1, "chilli_uamserver"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamHomepage">UAM homepage URL</td>
            <td><input name="sUamHomepage" maxlength="256" value="<% getCfgGeneral(1, "chilli_uamhomepage"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamSecret">UAM secret</td>
            <td><input name="sUamSecret" maxlength="256" value="<% getCfgGeneral(1, "chilli_uamsecret"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamAllowed">UAM allowed hosts</td>
            <td><input name="sUamAllowed" maxlength="1024" value="<% getCfgGeneral(1, "chilli_uamallowed"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamDomain">UAM allowed domains</td>
            <td><input name="sUamDomain" maxlength="1024" value="<% getCfgGeneral(1, "chilli_uamdomain"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="sUamAnyDNS">Allow all DNS requests</td>
            <td><input name="sUamAnyDns" type="checkbox"></td>
          </tr>
          <tr>
            <td class="head" id="sMacAllowed">Allowed MAC</td>
            <td><input name="sMacAllowed" maxlength="512" value="<% getCfgGeneral(1, "chilli_macallowed"); %>"></td>
          </tr>
        </table>
        <table width="95%" cellpadding="2" cellspacing="1">
          <tr align="center">
            <td><input type="submit" class="normal" value="Apply" id="sApply" onClick="TimeoutReload(20);">
              &nbsp;&nbsp;
              <input type="reset"  class="normal" value="Cancel" id="sCancel" onClick="window.location.reload();"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
