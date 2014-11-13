<!DOCTYPE html>
<html>
<head>
<title>FTP Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");

function initValue(form)
{
	var ftpenabled = defaultNumber("<% getCfgZero(1, "RemoteFTP"); %>", '1');
	form.ftp_enabled.value = ftpenabled;
	FtpEnableSwitch(form);
}

function FtpEnableSwitch(form)
{
	disableElement( [ form.ftp_port, form.ftp_rootdir, form.ftp_idle_timeout, form.ftp_abs_timeout ], form.ftp_enabled.value == '0');
}

function CheckValue(form)
{
	if ((form.ftp_enabled.value == '1') || (form.ftp_enabled.value == '2'))
	{
		if (form.ftp_port.value == "")
		{
			alert('Please specify FTP Port');
			form.ftp_port.focus();
			form.ftp_port.select();
			return false;
		}
		else if (isNaN(form.ftp_port.value) ||
			 parseInt(form.ftp_port.value,10) > 65535 || (parseInt(form.ftp_port.value,10) < 1024 &&
			 parseInt(form.ftp_port.value,10) != 21))

		{
			alert('Please specify valid number');
			form.ftp_port.focus();
			form.ftp_port.select();
			return false;
		}
		
		if (isNaN(form.ftp_idle_timeout.value))
		{
			alert('Please specify valid number');
			form.ftp_idle_timeout.focus();
			form.ftp_idle_timeout.select();
			return false;
		}
	}

	return true;
}
</script>
</head>

<body onLoad="initValue(document.storage_ftp);" >
<table class="body">
  <tr>
    <td><h1 id="ftpTitle">FTP Settings </h1>
      <p id="ftpIntroduction"></p>
      <hr />
      <form method=post name=storage_ftp action="/goform/storageFtpSrv" onSubmit="return CheckValue()">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="ftpSrvSet">FTP Server Setup</td>
          </tr>
          <tr>
            <td class="head" id="ftpSrv">FTP server access</td>
            <td><select name="ftp_enabled" class="half" onChange="FtpEnableSwitch(this.form);" >
                <option value="0">Disable</option>
                <option value="1">LAN</option>
                <option value="2">LAN &amp; WAN</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="ftpSrvPort">FTP server port</td>
            <td><input type=text name="ftp_port" size=5 maxlength=5 value="<% getCfgGeneral(1, "FtpPort"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="ftpSrvRootDir">FTP server root directory</td>
            <td><input type=text name="ftp_rootdir" size=50 maxlength=50 value="<% getCfgGeneral(1, "FtpRootDir"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="ftpSrvLoginTimeout">FTP server idle timeout</td>
            <td><input type=text name="ftp_idle_timeout" size=4 maxlength=4 value="<% getCfgGeneral(1, "FtpIdleTime"); %>"></td>
          </tr>
        </table>
        <br />
        <table class="buttons">
          <tr>
            <td><input value="/storage/ftpsrv.asp" name="submit-url" type="hidden">
              <input type=submit class="normal" value="Apply" id="ftpApply" onClick="return CheckValue(this.form);" >
              &nbsp; &nbsp;
              <input type=reset  class="normal" value="Reset" id="ftpReset" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
