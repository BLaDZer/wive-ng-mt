<!DOCTYPE html>
<html>
<head>
<title>System Log</title>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("admin");
Butterlate.setTextDomain("buttons");

function uploadLogField(str)
{
	if (str == "-1")
	{
		document.getElementById("syslog").value = _("syslog not support");
	}
	else
	{
		document.getElementById("syslog").value = str;
		document.getElementById("syslog").scrollTop = document.getElementById("syslog").scrollHeight;
	}
}

function updateLog()
{
	ajaxPerformRequest("/goform/getsyslog", uploadLogField);
}

function initTranslation()
{
    _TR("syslogTitle", "syslog title");
    _TR("syslogIntroduction", "syslog introduction");
    _TR("syslogSetup", "syslog setup");
    _TR("syslogEnabled", "syslog enabled");
    _TR("syslogKernel", "syslog kernel");
    _TR("syslogRemoteIP", "syslog remote ip");
    _TR("syslogSysLog", "syslog system log");

    _TRV("syslogApply", "button apply");
    _TRV("syslogClear", "button clear");
    _TRV("syslogRefresh", "button refresh");
    _TRV("syslogClear2", "button clear");
    _TRV("syslogRefresh2", "button refresh");

    var elements = document.getElementsByTagName('option');
    for (var i = 0; i < elements.length; i++)
    if (elements[i].id == "disable")
	elements[i].innerHTML = _("button disable");
    else if (elements[i].id == "enable")
	elements[i].innerHTML = _("button enable");
}

function pageInit()
{
	initTranslation();

	var form = document.LogdSetup;
	var klogd = '<% getCfgZero(1, "KLogd");%>';
	var syslogd = '<% getCfgZero(1, "SysLogd");%>';

	form.KLogd.value = (klogd == '1') ? '1' : '0';
	form.SysLogd.value = (syslogd == '1') ? '1' : '0';

	syslogdSelect(form);
	updateLog();
}

function clearlogclick()
{
	ajaxPostRequest("/goform/clearlog", 'stub=value', false, refreshlogclick);
}

function refreshlogclick()
{
	updateLog();
	return true;
}

function checkSetupForm(form)
{
	if (form.RemoteSysLogIP.value != '')
	{
		if (!validateIP(form.RemoteSysLogIP))
		{
			alert(_("syslog invalid ip"));
			form.RemoteSysLogIP.focus();
		}
	}
	ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
	return true;
}

function syslogdSelect(form)
{
	displayElement([ 'rmtSysLogIP', 'klogdRow', 'syslog_view' ], form.SysLogd.value == '1');
}

</script>
</head>
<body bgcolor="#FFFFFF" onLoad="pageInit()">
<table class="body">
  <tr>
    <td><h1 id="syslogTitle">System Log</h1>
      <p id="syslogIntroduction">Here you can configure system logging</p>
      <hr>
      <!-- ================= System log setup ================= -->
      <form method="post" name="LogdSetup" action="/goform/setuplog" onSubmit="checkSetupForm(this);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="syslogSetup">System Log Setup:</td>
          </tr>
          <tr>
            <td class="head" id="syslogEnabled">System logging daemon:</td>
            <td><select name="SysLogd" onChange="syslogdSelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="klogdRow">
            <td class="head" id="syslogKernel">Kernel logging daemon:</td>
            <td><select name="KLogd">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="rmtSysLogIP" style="display: none;">
            <td class="head" id="syslogRemoteIP">Remote system log IP:</td>
            <td><input name="RemoteSysLogIP" value='<% getCfgGeneral(1, "RemoteSysLogIP"); %>'></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="submit" value="Apply" id="syslogApply">
              <input type="hidden" name="submit-url" value="/adm/syslog.asp" ></td>
          </tr>
        </table>
      </form>
      <!-- ================= System log ================= -->
      <table class="form" id="syslog_view" style="display: none;">
        <tr>
          <td class="title" colspan="2" id="syslogSysLog">System Log: </td>
        </tr>
        <tr>
          <td colspan="2"><form method="post" name="SubmitClearLog1" action="/goform/clearlog">
              <input type="button" value="Refresh" id="syslogRefresh" name="refreshlog" onClick="refreshlogclick();">
              <input type="button" value="Clear" id="syslogClear" name="clearlog" onClick="clearlogclick();">
            </form></td>
        </tr>
        <tr>
          <td><textarea style="font-size: 8pt; width: 680px; height: 480px; border: 1px solid;" name="syslog" id="syslog" wrap="off" readonly="1">
		</textarea></td>
        </tr>
        <tr>
          <td colspan="2"><form method="post" name="SubmitClearLog2" action="/goform/clearlog">
              <input type="button" value="Refresh" id="syslogRefresh2" name="refreshlog" onClick="refreshlogclick();">
              <input type="button" value="Clear" id="syslogClear2" name="clearlog" onClick="clearlogclick();">
            </form></td>
        </tr>
      </table>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
