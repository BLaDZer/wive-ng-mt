<!DOCTYPE html>
<html>
<head>
<title>Print Server Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script language="JavaScript" type="text/javascript">

function initValue()
{
	var printersrvebl = '<% getCfgZero(1, "PrinterSrvEnabled"); %>';

	if (printersrvebl == "1")
	{
		document.printer.enabled[0].checked = true;
	}
	else
	{
		document.printer.enabled[1].checked = true;
	}
	var printersrvbd = '<% getCfgZero(1, "PrinterSrvBidir"); %>';

	if (printersrvbd == "1")
	{
		document.printer.bdenabled[0].checked = true;
	}
	else
	{
		document.printer.bdenabled[1].checked = true;
	}
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="printerTitle">Printer Server Settings</h1>
      <p id="printerIntroduction">Here you can configure printer server settings.
        If you printer need firmware - upload firmware in /etc/prnfw.dl on rwfs or /opt/prnfw.dl on optware dir in external drive.</p>
      <hr>
      <form method=POST name=printer action="/goform/printersrv">
        <table class="form">
          <tr>
            <td id="printerSettings" class="title" colspan="2">Printer Server Setup</td>
          </tr>
          <tr>
            <td id="printerCapability" class="head">Print Server</td>
            <td><input type="radio" name="enabled" value="1">
              <font id="printerEnable">Enable</font>
              <input type="radio" name="enabled" value="0">
              <font id="printerDisable">Disable</font></td>
          </tr>
          <tr>
            <td class="head" id="printerBidirectional">Bidirectional exchange</td>
            <td><input type="radio" name="bdenabled" value="1">
              <font id="bidirectEnable">Enable</font>
              <input type="radio" name="bdenabled" value="0">
              <font id="bidirectDisable">Disable</font></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type=submit class="normal" value="Apply" id="printerApply">
              &nbsp; &nbsp;
              <input type=button class="normal" value="Cancel" id="printerCancel" onClick="window.location.reload()">
              <input type="hidden" name="submit-url" value="/usb/P910NDprintersrv.asp"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
