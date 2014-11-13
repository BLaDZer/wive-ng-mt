<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<title>AP Client Feature</title>
<script type="text/javascript">

function SecurityModeSwitch(form)
{
	var wpa_on = (form.apcli_mode.value == 'WPAPSK') || (form.apcli_mode.value == 'WPA2PSK');
	displayElement( [ 'div_apcli_enc', 'div_apcli_wpapsk' ], wpa_on);
}

function initValue()
{
	var form = document.wireless_apcli;

	form.apcli_ssid.value   = '<% getCfgGeneral(1, "ApCliSsid"); %>';
	form.apcli_bssid.value  = '<% getCfgGeneral(1, "ApCliBssid"); %>';
	form.apcli_mode.value   = '<% getCfgGeneral(1, "ApCliAuthMode"); %>';
	form.apcli_enc.value    = '<% getCfgGeneral(1, "ApCliEncrypType"); %>';
	form.apcli_apiface.checked = '<% getCfgGeneral(1, "ApCliClientOnly"); %>' == '1';
	form.apcli_bridge.checked = '<% getCfgGeneral(1, "ApCliBridgeOnly"); %>' == '1';

	SecurityModeSwitch(form);
}

function CheckValue(form)
{
	var wpa_on = (form.apcli_mode.value == 'WPAPSK') || (form.apcli_mode.value == 'WPA2PSK');

	if (form.apcli_ssid.value.length <= 0)
	{
		alert("Please specify SSID");
		form.apcli_ssid.focus();
		return false;
	}

	if (form.apcli_bssid.value.length > 0)
	{
		if (!validateMAC(form.apcli_bssid.value, false))
		{
			alert("Please specify correct BSSID");
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
			alert("Password phrase length should be larger than 8 characters");
			form.apcli_wpapsk.focus();
			return false;
		}
		else if (wpapsk.length > 64)
		{
			alert('Password phrase is too long');
			form.apcli_wpapsk.focus();
			return false;
		}
		else if (wpapsk.match(/[\n\r\$]/))
		{
			alert('Some of characters in password phrase are not allowed');
			form.apcli_wpapsk.focus();
			return false;
		}
	}
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1>AP Client Feature</h1>
      <p>Here you can configure AP Client parameters.</p>
      <p>For correct work need set wireless channel (in Wireless Settings->Basiс) as channel used by AP to connect.</p>
      <hr />
      <form method="POST" name="wireless_apcli" action="/goform/wirelessApcli" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="3">AP Client Parameters</td>
          </tr>
          <tr>
            <td class="head">SSID</td>
            <td><input name="apcli_ssid"></td>
          </tr>
          <tr>
            <td class="head">MAC Address (Optional)</td>
            <td><input name="apcli_bssid"></td>
          </tr>
          <tr>
            <td class="head">Security Mode</td>
            <td><select name="apcli_mode" id="apcli_mode" size="1" onChange="SecurityModeSwitch(this.form);">
                <option value="OPEN">Open</option>
                <option value="WPAPSK">WPAPSK</option>
                <option value="WPA2PSK">WPA2PSK</option>
              </select></td>
          </tr>
          <tr id="div_apcli_enc">
            <td class="head">Encryption Type</td>
            <td><select name="apcli_enc" id="apcli_enc">
                <option value="AES" selected>AES</option>
                <option value="TKIP">TKIP</option>
              </select></td>
          </tr>
          <tr id="div_apcli_wpapsk">
            <td class="head">Pass Phrase</td>
            <td><input type="password" name="apcli_wpapsk" value="<% getCfgGeneralHTML(1, "ApCliWPAPSK"); %>"></td>
          </tr>
          <tr>
            <td class="head">Disable AP Interface</td>
            <td><input type="checkbox" name="apcli_apiface"></td>
          </tr>
          <tr>
            <td class="head">Enable Bridge Mode</td>
            <td><input type="checkbox" name="apcli_bridge"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="submit" class="normal" value="Apply">
              &nbsp; &nbsp;
              <input type="reset"  class="normal" value="Cancel" onClick="window.location.reload();">
              <input type="hidden" name="submit-url" value="/wireless/apcli.asp"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
