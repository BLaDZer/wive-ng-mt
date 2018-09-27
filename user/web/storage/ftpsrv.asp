<!DOCTYPE html>
<html>
<head>
<title>FTP Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
<script src="/lang/<% getLangDictionary(); %>/dict_storage.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">
function initValue(form)
{
	var ftpenabled = defaultNumber("<% getCfgZero(1, "RemoteFTP"); %>", '1');
	form.ftp_enabled.value = ftpenabled;
	FtpEnableSwitch(form);
	init_translation_model();
}

function FtpEnableSwitch(form)
{
	disableElement( [ form.ftp_port, form.ftp_idle_timeout, form.ftp_abs_timeout ], form.ftp_enabled.value == '0');
}

function CheckValue(form)
{
	if ((form.ftp_enabled.value == '1') || (form.ftp_enabled.value == '2'))
	{
		if (!inRange(form.ftp_port.value, 1024, 65535) && form.ftp_port.value != '21')
		{
			alert(_("storage ftp invalid port"));
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

	ajaxShowTimer(form, 'timerReloader', _('message apply'), 3);
	return true;
}
</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue(document.storage_ftp);" >
<table class="body">
  <tr>
    <td><h1 data-tr="storage ftp title">FTP Settings </h1>
      <p data-tr="storage ftp introduction"></p>
      <hr />
      <iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
      <form method=post name=storage_ftp action="/goform/storageFtpSrv" onSubmit="return CheckValue(this)">
        <table class="form">
          <tr>
            <td class="title" colspan="2" data-tr="storage ftp server setup">FTP Server Setup</td>
          </tr>
          <tr>
            <td class="head" data-tr="storage ftp server access">FTP server access</td>
            <td><select name="ftp_enabled" class="half" onChange="FtpEnableSwitch(this.form);" >
                <option value="0" data-tr="button disable">Disable</option>
                <option value="1">LAN</option>
                <option value="2">LAN &amp; WAN</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" data-tr="storage ftp server port">FTP server port</td>
            <td><input type=text name="ftp_port" size=5 maxlength=5 value="<% getCfgGeneral(1, "FtpPort"); %>"></td>
          </tr>
          <tr>
            <td class="head" data-tr="storage ftp server idle timeout">FTP server idle timeout</td>
            <td><input type=text name="ftp_idle_timeout" size=4 maxlength=4 value="<% getCfgGeneral(1, "FtpIdleTime"); %>"></td>
          </tr>
        </table>
        <br />
        <table class="buttons">
          <tr>
            <td><input value="/storage/ftpsrv.asp" name="submit-url" type="hidden">
              <input type="submit" class="normal" value="Apply" id="ftpApply" data-tr="button apply">
              <input type="button" class="normal" value="Reset" id="ftpReset" data-tr="button cancel" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
