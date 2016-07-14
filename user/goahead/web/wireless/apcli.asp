<!DOCTYPE html>
<html>
<head>
<title>AP Client Feature</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("wireless");
Butterlate.setTextDomain("buttons");

function initTranslation() {
	_TR("apcliTitle", "apcli title");
	_TR("apcliParameters", "apcli param");
	_TR("apcliAbout", "apcli about");
	_TR("apcliClientParam", "apcli client param");
	_TR("apcliSSID", "station ssid");
	_TR("apcliMAC", "apcli mac");
	_TR("apcliSecurityMode", "secure security mode");
	_TR("apcliEncryptionType", "secure encryp type");
	_TR("apcliPass", "secure wpa pass phrase");
	_TR("apcliShowPass", "secure wpa show pass phrase");
	_TR("apcliAutoscan", "apcli enable autoscan");
	_TR("apcliDisableIface", "apcli disable iface");
	_TR("apcliEnableBridge", "apcli enable bridge");
	_TR("basicAPCLIMode", "basic apcli mode");  
	_TRV("apcliApply", "button apply");
	_TRV("apcliCancel", "button cancel");
}

function SecurityModeSwitch(form)
{
	var enc_type = '<% getCfgGeneral(1, "ApCliEncrypType"); %>';
	var wpa_on = (form.apcli_mode.value == 'WPAPSK') || (form.apcli_mode.value == 'WPA2PSK');
	form.apcli_enc.value = (enc_type != "") ? "AES" : enc_type;
	displayElement( [ 'div_apcli_enc', 'div_apcli_wpapsk' ], wpa_on);
}

function initValue()
{
	var form = document.wireless_apcli;

	form.apcli_ssid.value   = '<% getCfgGeneral(1, "ApCliSsid"); %>';
	form.apcli_bssid.value  = '<% getCfgGeneral(1, "ApCliBssid"); %>';
	form.apcli_mode.value   = '<% getCfgGeneral(1, "ApCliAuthMode"); %>';
	form.apcli_autoscan.checked = '<% getCfgGeneral(1, "ApCliAutoConnect"); %>' == '1';
	form.apcli_apiface.checked = '<% getCfgGeneral(1, "ApCliClientOnly"); %>' == '1';
	form.apcli_bridge.checked = '<% getCfgGeneral(1, "ApCliBridgeOnly"); %>' == '1';
	
	var radio_on = "<% getCfgZero(1, "RadioOn"); %>";
	var radio_on_ac = "<% getCfgZero(1, "RadioOnINIC"); %>";
	var is5gh_support = '<% is5gh_support(); %>';
	var apcli_mode = '<% getCfgGeneral(1, "ApCliIfName"); %>';

	if ((is5gh_support == 0) || ((radio_on == 0) && (radio_on_ac == 0))) {
		form.apcli_interface.value = "apcli0";
		displayElement('apcliWiFiMode_tr', false);
	} 
	else {
		form.apcli_interface.disabled = (radio_on == 0) || (radio_on_ac == 0);
		if (radio_on == 0) 
			form.apcli_interface.value = "apclii0"
		else if (radio_on_ac == 0)
			form.apcli_interface.value = "apcli0"
		else
			form.apcli_interface.value = apcli_mode;
	}
	SecurityModeSwitch(form);
	initTranslation();
	showWarning();
}

function CheckValue(form)
{
	var wpa_on = (form.apcli_mode.value == 'WPAPSK') || (form.apcli_mode.value == 'WPA2PSK');

	if (form.apcli_ssid.value.length <= 0)
	{
		alert(_("apcli no ssid"));
		form.apcli_ssid.focus();
		return false;
	}

	if (form.apcli_bssid.value.length > 0)
	{
		if (!validateMAC(form.apcli_bssid.value, false))
		{
			alert(_("apcli no correct bssid"));
			form.apcli_bssid.focus;
			return false;
		}
	}

	if (wpa_on)
	{
		// Check password phrase
		var wpapsk = form.apcli_wpapsk.value;
		if (wpapsk.length < 8)
		{
			alert(_("apcli short phrase"));
			form.apcli_wpapsk.focus();
			return false;
		}
		else if (wpapsk.length > 64)
		{
			alert(_("apcli long phrase"));
			form.apcli_wpapsk.focus();
			return false;
		}
		else if (wpapsk.match(/[\n\r\$]/))
		{
			alert(_("apcli chars not allowed"));
			form.apcli_wpapsk.focus();
			return false;
		}
	}
	
	form.apcli_interface.disabled = false; 
	
	if (!ajaxPostForm(_('apcli reboot confirm'), form, 'rebootReloader', _("message config"), ajaxShowProgress)) {
		form.reboot.value = "0";
		form.submit();
	}
	return true;
}

function showPassPhrase() {
	var element = document.getElementById('apcli_wpapsk');
	if (element.type == 'password')
		element.type = 'text'
	else
		element.type = 'password';
}
</script>

</head>
<body bgcolor="#FFFFFF" onLoad="initValue();">
<table class="body">
	<tr id="warning"><tr>
	<tr>
		<td><h1 id="apcliTitle">AP Client Feature</h1>
			<p id="apcliParameters">Here you can configure AP Client parameters.</p>
			<p id="apcliAbout">For correct work need set wireless channel (in Wireless Settings->Basiс) as channel used by AP to connect.</p>
			<hr />
			<form method="POST" name="wireless_apcli" action="/goform/wirelessApcli">
			<iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
			<table class="form">
				<tr>
					<td id="apcliClientParam" class="title" colspan="3">AP Client Parameters</td>
				</tr>
				<tr id="apcliWiFiMode_tr">
					<td id="apcliWiFiMode" class="head" colspan="1">APCLI Mode</td>
					<td colspan="2"><select name="apcli_interface" size="1" class="normal">
						<option value="apcli0" selected id="1">2.4GHz</option>
						<option value="apclii0" id="2">5GHz</option>
					</select></td>
				</tr>				
				<tr>
					<td id="apcliSSID" class="head">SSID</td>
					<td><input name="apcli_ssid" class="normal"></td>
				</tr>
				<tr>
					<td id="apcliMAC" class="head">MAC Address (Optional)</td>
					<td><input name="apcli_bssid" class="normal"></td>
				</tr>
				<tr>
					<td id="apcliSecurityMode" class="head">Security Mode</td>
					<td><select name="apcli_mode" id="apcli_mode" size="1" class="normal" onChange="SecurityModeSwitch(this.form);">
						<option value="OPEN" id="apcliOpen">Open</option>
						<option value="WPAPSK" id="apcliWPA">WPAPSK</option>
						<option value="WPA2PSK" id="apcliWPA2">WPA2PSK</option>
					</select></td>
				</tr>
				<tr id="div_apcli_enc">
					<td id="apcliEncryptionType" class="head">Encryption Type</td>
					<td><select name="apcli_enc" id="apcli_enc" class="normal">
						<option value="AES" selected id="apcliAES">AES</option>
						<option value="TKIP" id="apcliTKIP">TKIP</option>
					</select></td>
				</tr>
				<tr id="div_apcli_wpapsk">
					<td id="apcliPass" class="head">Pass Phrase</td>
					<td class="value"><input type="password" id="apcli_wpapsk" name="apcli_wpapsk" class="normal" value="<% getCfgGeneralHTML(1, "ApCliWPAPSK"); %>"><input type="checkbox" onChange="showPassPhrase();"><font id="apcliShowPass">(show)</font></td>
				</tr> 
				<tr>
					<td id="apcliAutoscan" class="head">Auto channel select</td>
					<td><input type="checkbox" name="apcli_autoscan"></td>
				</tr>
				<tr>
					<td id="apcliDisableIface" class="head">Disable AP Interface</td>
					<td><input type="checkbox" name="apcli_apiface"></td>
				</tr>
				<tr>
					<td class="head" id="apcliEnableBridge">Enable Bridge Mode</td>
					<td><input type="checkbox" name="apcli_bridge"></td>
				</tr>
			</table>
			<table class="buttons">
				<tr>
					<td><input type="button" class="normal" value="Apply" id="apcliApply" onClick="return CheckValue(this.form);">
						&nbsp; &nbsp;
						<input type="reset"  class="normal" value="Cancel" id="apcliCancel" onClick="window.location.reload();">
						<input type="hidden" name="submit-url" value="/wireless/apcli.asp">
						<input type="hidden" value="1" name="reboot">
						<iframe id="rebootReloader" name="rebootReloader" src="" style="width:0;height:0;border:0px solid #fff;">
					</td>
				</tr>
			</table>
		</form>
		<div class="whitespace">&nbsp;</div></td>
	</tr>
</table>
</body>
</html>
