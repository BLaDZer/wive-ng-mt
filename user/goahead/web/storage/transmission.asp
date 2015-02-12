<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<title>Transmission Settings</title>
<script language="Javascript" type="text/javascript">
function initValue(form)
{
	form.TransEnabled.value = defaultNumber("<% getCfgZero(1, "TransmissionEnabled"); %>", '0');
	form.transAccess.value = '<% getCfgGeneral(1, "TransAccess"); %>'
	form.transAuthor.value = '<% getCfgGeneral(1, "TransAuthor"); %>'
	TransEnabledSwitch(form);
}

function TransEnabledSwitch(form)
{
	disableElement( [ form.transRPCPort, form.transstart, form.transstop, form.transreload, form.transAuthor,
	form.transAccess, form.WebGUI, form.transLog, form.transPass, form.transInPort] , form.TransEnabled.value != '1');
}

function CheckValue(form)
{
	if ((document.formTrans.TransEnabled.value == '1'))
	{
		if (document.formTrans.transRPCPort.value == "")
		{
			alert('Please specify Transmission RPC port');
			document.formTrans.transRPCPort.focus();
			document.formTrans.transRPCPort.select();
			return false;
		}
		else if (isNaN(document.formTrans.transRPCPort.value) ||
			 parseInt(document.formTrans.transRPCPort.value,10) > 65535 ||
			 parseInt(document.formTrans.transRPCPort.value,10) < 1024)

		{
			alert('Please specify valid Transmission RPC port number');
			document.formTrans.transRPCPort.focus();
			document.formTrans.transRPCPort.select();
			return false;
		}
		
		if (document.formTrans.transInPort.value == "")
		{
			alert('Please specify Transmission incoming peer port');
			document.formTrans.transInPort.focus();
			document.formTrans.transInPort.select();
			return false;
		}
		else if (isNaN(document.formTrans.transInPort.value) ||
			 parseInt(document.formTrans.transInPort.value,10) > 65535 ||
			 parseInt(document.formTrans.transInPort.value,10) < 1024)

		{
			alert('Please specify valid Transmission incoming peer port number');
			document.formTrans.transInPort.focus();
			document.formTrans.transInPort.select();
			return false;
		}
		
		if (document.formTrans.transLog.value.match(/[\s\$]/))
		{
			alert("User name can not contain spaces or dollar ('$') sign!");
			document.formTrans.transLog.focus();
			return false;
		}
		
		if (document.formTrans.transPass.value.match(/[\s\$]/))
		{
			alert("Password can not contain spaces or dollar ('$') sign!");
			document.formTrans.transPass.focus();
			return false;
		}
	}
	return true;
}

function submit_apply(parm)
{		
		if (parm == "apply")
			{
				if (CheckValue(this.form) != true)
				{
					return;
				}
			}
		document.formTrans.hiddenButton.value = parm;
		document.formTrans.submit();	
}

</script>
</head>

<body onLoad="initValue(document.formTrans)">
<table class="body">
  <tr>
    <td><h1>Transmission settings</h1>
      <p>Here you can configure transmission-daemon service.</p>
      <hr>
      <form action="/goform/formTrans" method="POST" name="formTrans">
        <input type=hidden name="hiddenButton" value="">
        <table class="form">
          <tr>
            <td class="title" colspan="2">Transsmission Settings</td>
          </tr>
          <tr>
            <td class="head">Enable Transmission</td>
            <td><select name="TransEnabled" class="half" onChange="TransEnabledSwitch(this.form);">
                <option value="0">Disable</option>
                <option value="1">Enable</option>
              </select>
              &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
              <input type="button" style="{width:80px;}" value="Start" name="transstart" onClick="submit_apply('start')">
              &nbsp;
              <input type="button" style="{width:80px;}" value="Stop" name="transstop" onClick="submit_apply('stop')">
              &nbsp;
              <input type="button" style="{width:80px;}" value="Reload" name="transreload" onClick="submit_apply('reload')"></td>
          </tr>
          <tr>
            <td class="head">RPC port</td>
            <td><input type=text name="transRPCPort" size=5 maxlength=5 value="<% getCfgGeneral(1, "TransRPCPort"); %>" ></td>
          </tr>
          <tr>
            <td class="head">RPC port access</td>
            <td><select name="transAccess" class="half">
                <option value="0">Disable</option>
                <option value="1">LAN</option>
                <option value="2">LAN &amp; WAN</option>
              </select>
              &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; &nbsp;
              <input type="button" style="{width:80px;}" onClick="location.href='http://<% getLanIp(); %>:<% getCfgGeneral(1, "TransRPCPort"); %>'" name="WebGUI" value="WebGUI"></td>
          </tr>
          <tr>
            <td class="head">Authorisation access</td>
            <td><select name="transAuthor" class="half">
                <option value="0">Disable</option>
                <option value="1">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head">Login</td>
            <td><input type=text name="transLog" size=16 maxlength=16 value="<% getCfgGeneral(1, "TransLogin"); %>" ></td>
          </tr>
          <tr>
            <td class="head">Password</td>
            <td><input type=password name="transPass" size=16 maxlength=16 value="<% getCfgGeneral(1, "TransPass"); %>" ></td>
          </tr>
          <tr>
            <td class="head">Incoming peer port</td>
            <td><input type=text name="transInPort" size=5 maxlength=5 value="<% getCfgGeneral(1, "TransInPort"); %>" ></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input value="/storage/transmission.asp" name="submit-url" type="hidden">
              <input type="button" class="normal" value="Apply" id="TransApply" onClick="submit_apply('apply')">
              &nbsp; &nbsp;
              <input type="button"  class="normal" value="Reset" id="TransReset" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
