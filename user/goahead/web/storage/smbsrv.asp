<!DOCTYPE html>
<html>
<head>
<title>SAMBA Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("storage");
var smbenabled = '<% getCfgZero(1, "SmbEnabled"); %>';
var smbwg = '<% getCfgGeneral(1, "HostName"); %>';
var smbnetbios = '<% getCfgGeneral(1, "SmbNetBIOS"); %>';
var smbstring = '<% getCfgGeneral(1, "SmbString"); %>';

function initTranslation()
{
	var e = document.getElementById("smbTitle");
	e.innerHTML = _("smb title");
	e = document.getElementById("smbIntroduction");
	e.innerHTML = _("smb introduction");

	e = document.getElementById("smbSrvSet");
	e.innerHTML = _("smb server setup");
	e = document.getElementById("smbSrv");
	e.innerHTML = _("smb server enable");
	e = document.getElementById("smbSrvEnable");
	e.innerHTML = _("storage enable");
	e = document.getElementById("smbSrvDisable");
	e.innerHTML = _("storage disable");
	e = document.getElementById("smbSrvWG");
	e.innerHTML = _("smb server workgroup");
	e = document.getElementById("smbSrvNetBIOS");
	e.innerHTML = _("smb server netbios");
	e = document.getElementById("smbSrvString");
	e.innerHTML = _("smb server string");

	e = document.getElementById("smbApply");
	e.value = _("storage apply");
	e = document.getElementById("smbReset");
	e.value = _("storage reset");
}

function initValue()
{
	initTranslation();

	/*
	alert(smbenabled);
	alert(anonymous);
	alert(port);
	alert(maxusers);
	alert(logintimeout);
	alert(staytimeout);
	*/

	document.storage_smb.smb_workgroup.disabled = true;
	document.storage_smb.smb_netbios.disabled = true;
	document.storage_smb.smb_string.disabled = true;

	if (smbenabled == 1)
	{
		// alert("SAMBA E");
		document.storage_smb.smb_enabled[0].checked = true;
	}
	else
	{
		// alert("SAMBA D");
		document.storage_smb.smb_enabled[1].checked = true;
	}

	if (smbenabled == 1)
	{
		document.storage_smb.smb_workgroup.disabled = false;
		document.storage_smb.smb_workgroup.value = smbwg;

		document.storage_smb.smb_netbios.disabled = false;
		document.storage_smb.smb_netbios.value = smbnetbios;

		document.storage_smb.smb_string.disabled = false;
		document.storage_smb.smb_string.value = smbstring;
	}
}

function CheckValue()
{
	if (document.storage_smb.smb_enabled[0].checked == true)
	{
		if (document.storage_smb.smb_workgroup.value == "")
		{
			alert('Please specify SAMBA Workgroup');
			document.storage_smb.smb_workgroup.focus();
			document.storage_smb.smb_workgroup.select();
			return false;
		}
		else if (document.storage_smb.smb_workgroup.value.indexOf(" ") >= 0)
		{
			alert('Blank spaces are not allowed in this field');
			document.storage_smb.smb_workgroup.focus();
			document.storage_smb.smb_workgroup.select();
			return false;
		}

		if (document.storage_smb.smb_netbios.value == "")
		{
			alert('Please specify SAMBA NetBIOS Name');
			document.storage_smb.smb_netbios.focus();
			document.storage_smb.smb_netbios.select();
			return false;
		}
		else if (document.storage_smb.smb_netbios.value.indexOf(" ") >= 0)
		{
			alert('Blank spaces are not allowed in this field');
			document.storage_smb.smb_netbios.focus();
			document.storage_smb.smb_netbios.select();
			return false;
		}

		if (document.storage_smb.smb_string.value == "")
		{
			alert('Please specify SAMBA Server String');
			document.storage_smb.smb_string.focus();
			document.storage_smb.smb_string.select();
			return false;
		}
	}

	return true;
}

function smb_enable_switch()
{
	if (document.storage_smb.smb_enabled[1].checked == true)
	{
		document.storage_smb.smb_workgroup.disabled = true;
		document.storage_smb.smb_netbios.disabled = true;
		document.storage_smb.smb_string.disabled = true;
	}
	else
	{
		document.storage_smb.smb_workgroup.disabled = false;
		document.storage_smb.smb_netbios.disabled = false;
		document.storage_smb.smb_string.disabled = false;
	}
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="smbTitle">SAMBA Settings </h1>
      <p id="smbIntroduction"></p>
      <hr />
      <form method=post name=storage_smb action="/goform/storageSmbSrv" onSubmit="return CheckValue()">
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="2" id="smbSrvSet">SAMBA Server Setup</td>
          </tr>
          <tr>
            <td class="head" id="smbSrv">SAMBA Server</td>
            <td><input type=radio name=smb_enabled value="1" onClick="smb_enable_switch()">
              <font id="smbSrvEnable">Enable</font>&nbsp;
              <input type=radio name=smb_enabled value="0" onClick="smb_enable_switch()" checked>
              <font id="smbSrvDisable">Disable</font></td>
          </tr>
          <tr>
            <td class="head" id="smbSrvWG">Workgroup</td>
            <td><input type=text name=smb_workgroup size=16 maxlength=16 value="LOCALNET"></td>
          </tr>
          <tr>
            <td class="head" id="smbSrvNetBIOS">NetBIOS Name</td>
            <td><input type=text name=smb_netbios size=16 maxlength=16 value="Wive-NG-MT"></td>
          </tr>
          <tr>
            <td class="head" id="smbSrvString">Server String</td>
            <td><input type=text name=smb_string size=16 maxlength=16 value="SERVER"></td>
          </tr>
        </table>
        <hr />
        <br />
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="center">
            <td><input type=submit style="{width:120px;}" value="Apply" id="smbApply">
              &nbsp; &nbsp;
              <input type=reset  style="{width:120px;}" value="Reset" id="smbReset" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
