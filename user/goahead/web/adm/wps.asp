<!DOCTYPE html>
<html>
<head>
<title>WPS</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script type="text/javascript" src="/adm/wps_timer.js"></script>
<script language="JavaScript" type="text/javascript">

function style_display_on(){
    if(window.ActiveXObject) { // IE
        return "block";
    } else if (window.XMLHttpRequest) { // Mozilla, Safari,...
        return "table-row";
    }
}

var http_request = false;
function makeRequest(url, content) {
	http_request = false;
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
		http_request = new XMLHttpRequest();
		if (http_request.overrideMimeType) {
			http_request.overrideMimeType('text/xml');
		}
	} else if (window.ActiveXObject) { // IE
		try {
			http_request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
			http_request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {}
		}
	}
	if (!http_request) {
		alert('Giving up :( Cannot create an XMLHTTP instance');
		return false;
	}
	http_request.onreadystatechange = alertContents;
	http_request.open('POST', url, true);
	http_request.send(content);
}

function alertContents() {
	if (http_request.readyState == 4) {
		if (http_request.status == 200) {
			WPSUpdateHTML( http_request.responseText);
		} else {
			alert('There was a problem with the request.');
		}
	}
}

function WPSUpdateHTML(str)
{
	var all_str = new Array();
	all_str = str.split(",");

	wpsconfigured = document.getElementById("WPSConfigured");
	if(all_str[0] == "1" || all_str[0] == "0")
		wpsconfigured.innerHTML = "No";
	else if(all_str[0] == "2")
		wpsconfigured.innerHTML = "Yes";
	else
		wpsconfigured.innerHTML = "Unknown";
	
	wpsssid = document.getElementById("WPSSSID");
	wpsssid.innerHTML = all_str[1];

	wpsauthmode = document.getElementById("WPSAuthMode");
	wpsauthmode.innerHTML = all_str[2];

	wpsencryptype = document.getElementById("WPSEncryptype");
	wpsencryptype.innerHTML = all_str[3];

	wpsdefaultkeyindex = document.getElementById("WPSDefaultKeyIndex");
	wpsdefaultkeyindex.innerHTML = all_str[4];

	wpswpakey = document.getElementById("WPSWPAKey");
	wpswpakey.innerHTML = all_str[5];

	wpsstatus = document.getElementById("WPSCurrentStatus");
	wpsstatus.innerHTML = all_str[6];
}

function updateWPS(){
	makeRequest("/goform/updateWPS", "something");
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}


function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function ValidateChecksum(PIN)
{
    var accum = 0;

    accum += 3 * (parseInt(PIN / 10000000) % 10);
    accum += 1 * (parseInt(PIN / 1000000) % 10);
    accum += 3 * (parseInt(PIN / 100000) % 10);
    accum += 1 * (parseInt(PIN / 10000) % 10);
    accum += 3 * (parseInt(PIN / 1000) % 10);
    accum += 1 * (parseInt(PIN / 100) % 10);
    accum += 3 * (parseInt(PIN / 10) % 10);
    accum += 1 * (parseInt(PIN / 1) % 10);

    return ((accum % 10) == 0);
}

var _pin_registrar_value = 1;
var _pin_value = 1;
function PINPBCFormCheck()
{
	if(_pin_value == 1 && _pin_registrar_value == 1){		/* PIN && internal registrar */
		if( document.WPSBegin.sta_pin.value == ""){
			alert("Please specify a PIN number.");
			return false;
		}
		if(! ValidateChecksum(document.WPSBegin.sta_pin.value)){
			alert("Wrong PIN number.");
			return false;
		}
	}
	return true;
}

function PINSelect()
{
	_pin_value = 1;
	document.getElementById("RegistrarSetting").style.visibility = "visible";
	document.getElementById("RegistrarSetting").style.display = style_display_on();

	if(document.WPSBegin.PINRegistrarRADIO.value == 0){
		document.getElementById("STAPIN").style.visibility = "hidden";
		document.getElementById("STAPIN").style.display = "none";
		document.getElementById("APPIN").style.visibility = "visible";
		document.getElementById("APPIN").style.display = style_display_on();;
		_pin_registrar_value = 0;
	}else if(document.WPSBegin.PINRegistrarRADIO.value == 1){
		document.getElementById("STAPIN").style.visibility = "visible";
		document.getElementById("STAPIN").style.display = style_display_on();;
		document.getElementById("APPIN").style.visibility = "hidden";
		document.getElementById("APPIN").style.display = "none";
		_pin_registrar_value = 1;
	}else{
		/* There is no value in document.WPSBegin.PINRegistrarRADIO when page init
		 *
		 */
		if(_pin_registrar_value){
			document.getElementById("STAPIN").style.visibility = "visible";
			document.getElementById("STAPIN").style.display = style_display_on();
			document.getElementById("APPIN").style.visibility = "hidden";
			document.getElementById("APPIN").style.display = "none";
		}else{
			document.getElementById("STAPIN").style.visibility = "hidden";
			document.getElementById("STAPIN").style.display = "none";
			document.getElementById("APPIN").style.visibility = "visible";
			document.getElementById("APPIN").style.display = style_display_on();
		}
	}
}

function PBCSelect()
{
	_pin_value = 0;
	document.getElementById("RegistrarSetting").style.visibility = "hidden";
	document.getElementById("RegistrarSetting").style.display = "none";
	document.getElementById("STAPIN").style.visibility = "hidden";
	document.getElementById("STAPIN").style.display = "none";
	document.getElementById("APPIN").style.visibility = "hidden";
	document.getElementById("APPIN").style.display = "none";	
}

function PINInternalRADIO()
{
	_pin_value = 1;
	_pin_registrar_value = 1;
	document.getElementById("STAPIN").style.visibility = "visible";
	document.getElementById("STAPIN").style.display = style_display_on();
	document.getElementById("APPIN").style.visibility = "hidden";
	document.getElementById("APPIN").style.display = "none";
}

function PINExternalRADIO()
{
	_pin_value = 1;
	_pin_registrar_value = 0;
	document.getElementById("STAPIN").style.visibility = "hidden";
	document.getElementById("STAPIN").style.display = "none";
	document.getElementById("APPIN").style.visibility = "visible";
	document.getElementById("APPIN").style.display = style_display_on();

}

function updateState(){
}

function pageInit(){
	PINSelect();
	updateState();
	updateWPS();
	InitializeTimer(3);
}

</script>
</head>
<body onLoad="pageInit()">
<table class="body">
  <tr>
    <td><h1>Wi-Fi Protected Setup</h1>
      <p> Here you can setup security easily by choosing PIN or PBC method to have Wi-Fi Protected Setup.</p>
      <table class="form">
        
        <!-- =================  WPS Status  ================= -->
        <tr>
          <td class="title" colspan="2">WPS Status</td>
        </tr>
        <tr>
          <td class="head" >WPS Current Status: </td>
          <td><span id="WPSCurrentStatus"> </span></td>
        </tr>
        <form method="post" name ="SubmitOOB" action="/goform/OOB">
          <tr>
            <td class="head" >WPS Configured: </td>
            <td><span id="WPSConfigured"> </span>
              <input type="submit" value="Reset OOB" name="submitResetOOB" align="left"></td>
          </tr>
        </form>
        <tr>
          <td class="head" >WPS SSID: </td>
          <td><span id="WPSSSID"> </span></td>
        </tr>
        <tr>
          <td class="head" >WPS Auth Mode: </td>
          <td><span id="WPSAuthMode"> </span></td>
        </tr>
        <tr>
          <td class="head" >WPS Encryption Type: </td>
          <td><span id="WPSEncryptype"> </span></td>
        </tr>
        <tr>
          <td class="head" >WPS Default Key Index: </td>
          <td><span id="WPSDefaultKeyIndex"> </span></td>
        </tr>
        <tr>
          <td class="head" >WPS WPA Key: </td>
          <td><span id="WPSWPAKey"> </span></td>
        </tr>
        <tr>
          <td class="head" >AP PIN: </td>
          <td><% getPINASP(); %></td>
        </tr>
      </table>
      <hr />
      <form method="post" name="WPSBegin" action="/goform/WPSBegin">
        <table class="form">
          <!-- =================  PIN & PBC  ================= -->
          <tr>
            <td class="title" colspan="2">WPS Setup</td>
          </tr>
          <tr>
            <td class="head" >Method</td>
            <td><input type=radio name=PINPBCRADIO value="1" checked onClick="PINSelect()" >
              PIN
              <input type=radio name=PINPBCRADIO value="0" onClick="PBCSelect()">
              PBC </td>
          </tr>
          <tr id="RegistrarSetting" >
            <td class="head" >Registrar Setting:</td>
            <td><input type=radio name=PINRegistrarRADIO value="1" checked onClick="PINInternalRADIO()" >
              Internal
              <input type=radio name=PINRegistrarRADIO value="0" onClick="PINExternalRADIO()">
              External </td>
          </tr>
          <tr id="STAPIN">
            <td class="head">STA PIN: </td>
            <td><input type="text" name="sta_pin" size="8" maxlength="24">
              <input type="checkbox" name="force_rekey">
              force to generate new security settings.(Previous enrollees would be disconnected.)</td>
          </tr>
          <tr id="APPIN">
            <td class="head">AP PIN: </td>
            <td><input type="text" name="ap_pin" size="8" maxlength="24" value='<% getPINASP(); %>' readonly></td>
          </tr>
          <tr>
            <td class="head">Press </td>
            <td><input value="Begin WPS" name="WPSBeginSubmit" onClick="return PINPBCFormCheck()" type="submit"></td>
          </tr>
        </table>
      </form>
      <br></td>
  </tr>
</table>
</body>
</html>
