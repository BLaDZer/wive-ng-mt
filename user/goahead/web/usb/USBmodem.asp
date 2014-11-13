<!DOCTYPE html>
<html>
<head>
<title>3G modem setup</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="javascript">

function hideHint(ctl)
{
	var row = document.getElementById('modem_hint_row');
	row.innerHTML = '';
}

function showHint(key)
{
	var row = document.getElementById('modem_hint_row');
	var form = document.usbmodem;
	var text = '<div class="hint"><font color="#0000ff"><b>HINT:</b></font>&nbsp;';
	var show = true;
	
	if (key=='modem_enabled')
		text += 'Enable USB 3G/GPRS/CDMA modem support and USB modem connections control.';
	else if (key=='modem_status')
		text += 'This indicator shows current state of modem connection.';
	else if (form.modem_enabled.value == '1')
	{	
		if (key=='modem_type')
			text += 'Specify USB modem type.';
		else if (key=='modem_port')
			text += 'Manually specify USB modem port (ttyUSB0-ttyUSB9, ttyACM0) or set to AUTO for automatic port search.';	
		else if (key=='modem_speed')
			text += 'Manually specify USB modem port speed(57600-115200-230400) or set to AUTO for automatic select speed.';	
		else if (key=='modem_mtu')
			text += 'Specify Maximum Transfer Unit/Maximum Recieve Unit size in octets.';
		else if (key=='modem_user')
			text += 'Specify user name given by your mobile ISP.';
		else if (key=='modem_pass')
			text += 'Specify password given by your mobile ISP.';
		else if (key=='modem_dialn')
			text += 'Specify telephone number given by your mobile ISP.';	
		else if (key=='modem_apn')
			text += 'Specify Access Point Name (APN) for access to network given by your mobile ISP.';
		else if (key=='modem_atm')
			text += 'Enable for AT commands modem control.';
		else if (key=='modem_debug')
			text += 'Enable allow debug mode for modem connections.';
		else if (key=='modem_at')
			text += 'Specify AT commands for modem control.';
	}
	else
		show = false;
	
	if (show)
	{
		text += '</div>';
		row.innerHTML = text;
	}
}

function mtuChange(form)
{
	var modem_mtu_select = document.getElementById("modem_mtu_select");
	var modem_mtu_field  = document.getElementById("modem_mtu_field");

	if (form.modem_mtu_type.value == '1')
	{
		modem_mtu_field.style.display = '';
		modem_mtu_select.setAttribute("class", "half");
		modem_mtu_field.setAttribute("class", "half");
	}
	else
	{
		modem_mtu_select.setAttribute("class", "mid");
		modem_mtu_field.style.display = 'none';
		form.modem_mtu.value = form.modem_mtu_type.value;
	}
}


function CheckValue(form)
{
	if (document.usbmodem.modem_pass.value.match(/[\s\$]/))
	{
		alert("Password can not contain spaces or dollar ('$') sign!");
		document.usbmodem.modem_pass.focus();
		return false;
	}
		if (document.usbmodem.modem_user.value.match(/[\s\$]/))
		{
			alert("User name can not contain spaces or dollar ('$') sign!");
			document.usbmodem.modem_user.focus();
			return false;
		}

		return true;
}

function selectModemType(form)
{
	var gsm_on = form.modem_type.value == '0';
	var cdma_on = form.modem_type.value == '1';

	// Display mode-dependent elements
	displayElement( 'modem_dialn_row', gsm_on);
	displayElement( 'modem_apn_row', gsm_on);
}

function bodyOnLoad(form)
{
	initializeForm(form);

	/* Check if option was set */
	var modem_mtu_select = document.getElementById('modem_mtu_select');
	for (var i=0; i < modem_mtu_select.options.length; i++)
		if (form.modem_mtu_type.options[i].value == form.modem_mtu.value)
		{
			form.modem_mtu_type.value = form.modem_mtu_select.options[i].value;
			break;
		}

		modemSwitchClick(form);
		selectModemType(form);
		mtuChange(form);

		showModemStatus();

}

function modemSwitchClick(form)
{
	disableElement( [
		form.modem_type, form.modemstart, form.modemstop, form.modem_port, form.modem_speed, form.modem_mtu,
		form.modem_mtu_type, form.modem_user, form.modem_pass, form.modem_dialn, form.modem_apn, form.at_enabled,
		form.mdebug_enabled, form.modem_at1, form.modem_at2, form.modem_at3 ], form.modem_enabled.value != '1' );
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
		document.usbmodem.hiddenButton.value = parm;
		document.usbmodem.submit();
}


function initializeForm(form)
{
var wmenabled     	     = defaultNumber("<% getCfgZero(1, "MODEMENABLED"); %>", '0');
var watmenabled			 = '<% getCfgGeneral(1, "MODEMATENABLED"); %>';
var wmdebug				 = '<% getCfgGeneral(1, "MODEMDEBUG"); %>';
form.modem_type.value    = '<% getCfgGeneral(1, "MODEMTYPE"); %>';
form.modem_port.value    = '<% getCfgGeneral(1, "WMODEMPORT"); %>';
form.modem_speed.value   = '<% getCfgGeneral(1, "MODEMSPEED"); %>';
form.modem_mtu.value     = '<% getCfgGeneral(1, "MODEMMTU"); %>';
form.modem_user.value    = '<% getCfgGeneral(1, "MODEMUSERNAME"); %>';
form.modem_pass.value    = '<% getCfgGeneral(1, "MODEMPASSWORD"); %>';
form.modem_dialn.value	 = '<% getCfgGeneral(1, "MODEMDIALNUMBER"); %>';
form.modem_apn.value     = '<% getCfgGeneral(1, "APN"); %>';
form.modem_at1.value     = '<% getCfgGeneral(1, "MODEMAT1"); %>';
form.modem_at2.value     = '<% getCfgGeneral(1, "MODEMAT2"); %>';
form.modem_at3.value     = '<% getCfgGeneral(1, "MODEMAT3"); %>';

	form.modem_enabled.value = (wmenabled != '1') ? '0' : '1';
	form.at_enabled.checked = (watmenabled == '1');
	form.mdebug_enabled.checked = (wmdebug == '1');
	
	selectModemType(form);
}

function showModemStatus()
{
	var reset_timer = function() { self.setTimeout('showModemStatus();', 5000); };

	ajaxLoadElement(
		'modem_status_col',
		'/usb/modem_status.asp',
		reset_timer
	);
}
</script>
</head>

<body onLoad="bodyOnLoad(document.usbmodem);">
<table class="body">
  <tr>
    <td><h1>USB Modem setup</h1>
      <p>This page is used to configure the USB Modem setting, connections control and display of current state
        your modem connections.</p>
      <hr>
      <form action="/goform/usbmodem" method="POST" name="usbmodem">
        <input type=hidden name="hiddenButton" value="">
        <table class="form">
          <tr>
            <td class="title" colspan="2">USB Modem configuration</td>
          </tr>
          <tr id="modem_staus_row" onMouseOver="showHint('modem_status')" onMouseOut="hideHint('modem_status')">
            <td class="head" height="30">USB modem status</td>
            <td id="modem_status_col">&nbsp;</td>
          </tr>
          <tr id="modem_enable_row" >
            <td class="head" onMouseOver="showHint('modem_enabled')" onMouseOut="hideHint('modem_enabled')">USB Modem control</td>
            <td onMouseOver="showHint('modem_enabled')" onMouseOut="hideHint('modem_enabled')"><select name="modem_enabled" class="half" onChange="modemSwitchClick(this.form);">
                <option value="0">Disable</option>
                <option value="1">Enable</option>
              </select>
              &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
              <input type="button" style="{width:100px;}" value="Connect" name="modemstart" onClick="submit_apply('connect')">
              <input type="button" style="{width:100px;}" value="Disconnect" name="modemstop" onClick="submit_apply('disconnect')"></td>
          </tr>
          <tr id="modem_type_row" onMouseOver="showHint('modem_type')" onMouseOut="hideHint('modem_type')">
            <td class="head">Modem type:</td>
            <td><select name="modem_type" onChange="selectModemType(this.form);" class="mid">
                <option value="0">WCDMA/UMTS/GPRS</option>
                <option value="1">CDMA/EVDO</option>
              </select></td>
          </tr>
          <tr id="modem_port_row" onMouseOver="showHint('modem_port')" onMouseOut="hideHint('modem_port')">
            <td class="head">Modem port:</td>
            <td><select name="modem_port" class="half">
                <option value="AUTO">AUTO</option>
                <option value="ttyUSB0">ttyUSB0</option>
                <option value="ttyUSB1">ttyUSB1</option>
                <option value="ttyUSB2">ttyUSB2</option>
                <option value="ttyUSB3">ttyUSB3</option>
                <option value="ttyUSB4">ttyUSB4</option>
                <option value="ttyUSB5">ttyUSB5</option>
                <option value="ttyUSB6">ttyUSB6</option>
                <option value="ttyUSB7">ttyUSB7</option>
                <option value="ttyUSB8">ttyUSB8</option>
                <option value="ttyUSB9">ttyUSB9</option>
                <option value="ttyACM0">ttyACM0</option>
              </select></td>
          </tr>
          <tr id ="modem_speed_row" onMouseOver="showHint('modem_speed')" onMouseOut="hideHint('modem_speed')">
            <td class="head">Modem port speed:</td>
            <td><select name="modem_speed" class="half">
                <option value="AUTO">AUTO</option>
                <option value="57600">57600</option>
                <option value="115200">115200</option>
                <option value="230400">230400</option>
              </select></td>
          </tr>
          <tr id="modem_mtu_row" onMouseOver="showHint('modem_mtu')" onMouseOut="hideHint('modem_mtu')">
            <td class="head">Modem MTU/MRU:</td>
            <td><input id="modem_mtu_field" name="modem_mtu" maxlength="4" type="text" class="half" style="display:none; ">
              <select id="modem_mtu_select" name="modem_mtu_type" onChange="mtuChange(this.form);" class="mid" >
                <option value="AUTO">AUTO</option>
                <option value="1" selected="selected" >Custom</option>
                <option value="1500">1500</option>
                <option value="1492">1492</option>
                <option value="1440">1440</option>
                <option value="1400">1400</option>
                <option value="1300">1300</option>
                <option value="1200">1200</option>
                <option value="1100">1100</option>
                <option value="1000">1000</option>
              </select></td>
          </tr>
          <tr id="modem_user_row" onMouseOver="showHint('modem_user')" onMouseOut="hideHint('modem_user')">
            <td class="head" >User name:</td>
            <td><input name="modem_user" class="mid" size="25" maxlength="60" type="text"></td>
          </tr>
          <tr id="modem_pass_row" onMouseOver="showHint('modem_pass')" onMouseOut="hideHint('modem_pass')">
            <td class="head">Password:</td>
            <td><input name="modem_pass" class="mid" size="25" maxlength="60" type="password"></td>
          </tr>
          <tr id="modem_dialn_row" style="display: none;" onMouseOver="showHint('modem_dialn')" onMouseOut="hideHint('modem_dialn')">
            <td class="head">Dial number:</td>
            <td><input name="modem_dialn" class="mid" size="25" maxlength="60" type="text"></td>
          </tr>
          <tr id="modem_apn_row" onMouseOver="showHint('modem_apn')" onMouseOut="hideHint('modem_apn')">
            <td class="head">Access Point Name (APN):</td>
            <td><input name="modem_apn" class="mid" size="25" maxlength="60" type="text"></td>
          </tr>
        </table>
        <table class="form">
          <tr>
            <td class="title" colspan="2">Additional options
              </th>
          </tr>
          <tr id="at_anable_row">
            <td class="head" width="50%" onMouseOver="showHint('modem_atm')" onMouseOut="hideHint('modem_atm')"><input name="at_enabled" type="checkbox">
              Enable AT commands</td>
            <td class="head" width="50%" onMouseOver="showHint('modem_debug')" onMouseOut="hideHint('modem_debug')"><input name="mdebug_enabled" type="checkbox">
              Allow debug</td>
          </tr>
          <tr id="modem AT1_row">
            <td onMouseOver="showHint('modem_at')" onMouseOut="hideHint('modem_at')"><input name="modem_at1" size="40" maxlength="40" type="text"></td>
            <td rowspan=3 width="50%"></td>
          </tr>
          <tr id="modem AT2_row">
            <td width="50%" onMouseOver="showHint('modem_at')" onMouseOut="hideHint('modem_at')"><input name="modem_at2" size="40" maxlength="40" type="text"></td>
          </tr>
          <tr id="modem AT3_row">
            <td width="50%" onMouseOver="showHint('modem_at')" onMouseOut="hideHint('modem_at')"><input name="modem_at3" size="40" maxlength="40" type="text"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input value="/usb/USBmodem.asp" name="submit-url" type="hidden">
              <input class="normal" value="Apply" name="Apply" type="button" onClick="submit_apply('apply')" >
              &nbsp;&nbsp;
              <input class="normal" value="Cancel" name="Cancel" onClick="window.location.reload()" type="button">
              &nbsp;&nbsp; </td>
          </tr>
        </table>
        <div id="modem_hint_row">&nbsp;</div>
        <div class="whitespace">&nbsp;</div>
      </form></td>
  </tr>
</table>
</body>
</html>
