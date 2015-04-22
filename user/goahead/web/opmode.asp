<!DOCTYPE html>
<html>
<head>
<title>Operation Mode</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="style/windows.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("mode");
Butterlate.setTextDomain("buttons");

var opmode;
var old_mode;

function changeMode()
{
	var form = document.opmode;

	if (document.opmode.opMode[0].checked)
		opmode = 0;
	else if (form.opMode[1].checked || form.opMode[3].checked)
		opmode = 1;
	else if (form.opMode[2].checked)
		opmode = 2;
}

function initTranslation()
{
	_TR("oTitle", "opmode title");
	_TR("oFirmVer", "opmode firmware version");
	_TR("oIntroduction", "opmode introduction");

	_TR("oModeB", "opmode mode bridge");
	_TR("oModeBIntro", "opmode mode bridge intro");
	_TR("oModeG", "opmode mode gateway");
	_TR("oModeGIntro", "opmode mode gateway intro");
	_TR("oModeE", "opmode mode e");
	_TR("stadd", "opmode mode e intro");
	_TR("oModeA", "opmode mode a");
	_TR("apclidd", "opmode mode a intro");
	_TR("oModeS", "opmode mode s");
	_TR("spotdd", "opmode mode s intro");

	_TRV("oApply", "button apply");
	_TRV("oCancel", "button cancel");
}

function initValue()
{
	opmode = "<% getCfgZero(1, "OperationMode"); %>";
	old_mode = opmode;

	var gwb = "<% getGWBuilt(); %>";
	var apcli = "<% getWlanApcliBuilt(); %>";
	var sta = "<% getStationBuilt(); %>";
	var spot = "<% getSpotBuilt(); %>";
	var mem_size = <% getMemAmount(); %>;
	var form = document.opmode;

	initTranslation();
	
	displayElement("ram_caution", mem_size < 30000);

	if (gwb == "0")
	{
		hideElement("gwdt");
		hideElement("oModeGIntro");
	}
	if (apcli == "0")
	{
		hideElement("apclidt");
		hideElement("apclidd");
	}
	if (sta == "0")
	{
		hideElement("stadt");
		hideElement("stadd");
	}
	if (spot == "0")
	{
		hideElement("spotdt");
		hideElement("spotdd");
	}

	if (opmode == "1")
		form.opMode[1].checked = true;
	else if (opmode == "2")
		form.opMode[2].checked = true;
	else if (opmode == "3")
		form.opMode[3].checked = true;
	else if (opmode == "4")
		form.opMode[4].checked = true;
	else
		form.opMode[0].checked = true;
	changeMode();
}

function msg()
{
}

</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="oTitle">Operation Mode Configuration</h1>
      <p><div id="oFirmVer">Current Firmware Version:</div> <% getSdkVersion(); %></p>
      <hr>
      <p id="oIntroduction">You can configure the operation mode suitable for your environment.</p>
      <hr>
      <form method="POST" name="opmode" action="/goform/setOpMode">
        <dl>
          <dt>
            <input type="radio" name="opMode" id="opMode" value="0" onClick="changeMode()">
            <b id="oModeB">Bridge:</b> </dt>
          <dd id="oModeBIntro"></dd>
          <dt id="gwdt">
            <input type="radio" name="opMode" id="opMode" value="1" onClick="changeMode()">
            <b id="oModeG">Gateway:</b> </dt>
          <dd id="oModeGIntro"></dd>
          <dt id="stadt">
            <input type="radio" name="opMode" id="opMode" value="2" onClick="changeMode()">
            <b id="oModeE">Ethernet Converter:</b> </dt>
          <dd id="stadd"></dd>
          <dt id="apclidt">
            <input type="radio" name="opMode" id="opMode" value="3" onClick="changeMode()">
            <b id="oModeA">AP Client:</b> </dt>
          <dd id="apclidd"></dd>
          <dt id="spotdt">
            <input type="radio" name="opMode" id="opMode" value="4" onClick="changeMode()">
            <b id="oModeS">Hotspot:</b> </dt>
          <dd id="spotdd"></dd>
        </dl>
        <p></p>
        <center>
          <input type="button" style="{width:120px;}" value="Apply" id="oApply" onClick="msg(); ajaxPostForm(_('opmode confirm'), this.form, 'setmodeReloader', '/messages/rebooting.asp', ajaxShowProgress);">
          &nbsp;&nbsp;
          <input type="reset" style="{width:120px;}" value="Reset" id="oCancel" onClick="window.location.reload()">
          <iframe id="setmodeReloader" name="setmodeReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        </center>
      </form></td>
  </tr>
</table>
</body>
</html>
