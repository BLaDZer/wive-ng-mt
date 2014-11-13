<!DOCTYPE html>
<html>
<head>
<title>QoS/Bandwidth Management Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">



function QoSSelectChange(form)
{
	var complex_on = form.QoSSelect.value == '2';

	// Display mode-dependent elements
	displayElement( [ 'BWSettings', 'PPortSettings', 'QoSUpBW', 'QoSUpBWL', 'QoSDownBW', 'QoSDownBWL', 'QoSUPVPNBW',
	'QoSUPVPNBWL', 'user_Qos_HPP', 'user_Qos_LPP' ], complex_on );
}
function initializeForm(form)
{
form.QoSSelect.value	= '<% getCfgGeneral(1, "QoSEnable"); %>';
var simpleqos		= '<% getCfgGeneral(1, "simple_qos"); %>';
form.simple_qos.checked = (simpleqos == '1');

form.UpBW.value    		= '<% getCfgGeneral(1, "QoS_rate_up"); %>';
form.UpBWL.value		= '<% getCfgGeneral(1, "QoS_rate_limit_up"); %>';
form.DownBW.value		= '<% getCfgGeneral(1, "QoS_rate_down"); %>';
form.DownBWL.value		= '<% getCfgGeneral(1, "QoS_rate_limit_down"); %>';
form.UpVPNBW.value		= '<% getCfgGeneral(1, "QoS_rate_vpn_up"); %>';
form.UpVPNBWL.value		= '<% getCfgGeneral(1, "QoS_rate_vpn_limit_up"); %>';
form.QoS_HPP.value		= '<% getCfgGeneral(1, "QoS_high_pp"); %>';
form.QoS_LPP.value		= '<% getCfgGeneral(1, "QoS_low_pp"); %>';

}
function bodyOnLoad(form)
{
		initializeForm (form);
		QoSSelectChange(form);
}
</script>
</head>
<!--     body      -->
<body onLoad="bodyOnLoad(document.QoSSetup);">
<table class="body">
  <tr>
    <td><h1 id="QoSTitleStr">Quality of Service Settings </h1>
      <p id="QoSIntroStr"> Here you can setup rules to provide desired Quality of Service for specific applications.</p>
      <p>For correct operation of QoS (mode Ralink, Priority-Based and Complex) automatically turning off <b>NAT offload mode</b>.
        Please note that turning off <b>NAT offload mode</b> will increase CPU usage up to 50%.</p>
      <hr>
      <form method="post" name="QoSSetup" action="/goform/QoSSetup">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="QoSSetupStr">QoS Setup </td>
          </tr>
          <tr>
            <td class="head" id="QoSStr">Type of QoS</td>
            <td><select name="QoSSelect" onChange="QoSSelectChange(this.form);" class="mid">
                <option value="0">Disable</option>
                <option value="1">Simple priority based</option>
                <option value="2">Complex with shaper</option>
              </select></td>
          </tr>
          <tr>
            <td class="head">Simple QoS</td>
            <td><input type="checkbox" name="simple_qos"></td>
          </tr>
          <tr id="BWSettings">
            <td class="title" colspan="2" id="QoSSetupBW">Bandwidth settings</td>
          </tr>
          <tr id="QoSUpBW">
            <td class="head" id="QoSUpBWStr">Upload rate:</td>
            <td><input name="UpBW" class="short_qos" size="7" maxlength="7" type="text">
              &nbsp;&nbsp;KBits/sec</td>
          </tr>
          <tr id="QoSUpBWL">
            <td class="head" id="QoSUpBWLStr">Upload rate limit:</td>
            <td><input name="UpBWL" class="short_qos" size="7" maxlength="7" type="text">
              &nbsp;&nbsp;KBits/sec</td>
          </tr>
          <tr id="QoSDownBW">
            <td class="head" id="QoSDownBWStr">Download rate:</td>
            <td><input name="DownBW" class="short_qos" size="7" maxlength="7" type="text">
              &nbsp;&nbsp;KBits/sec</td>
          </tr>
          <tr id="QoSDownBWL">
            <td class="head" id="QoSDownBWLStr">Download rate limit:</td>
            <td><input name="DownBWL" class="short_qos" size="7" maxlength="7" type="text">
              &nbsp;&nbsp;KBits/sec</td>
          </tr>
          <tr id="QoSUPVPNBW">
            <td class="head" id="QoSUPVPNBWStr">Upload VPN rate:</td>
            <td><input name="UpVPNBW" class="short_qos" size="7" maxlength="7" type="text">
              &nbsp;&nbsp;KBits/sec</td>
          </tr>
          <tr id="QoSUPVPNBWL">
            <td class="head" id="QoSUPVPNBWLStr">Upload VPN rate limit:</td>
            <td><input name="UpVPNBWL" class="short_qos" size="7" maxlength="7" type="text">
              &nbsp;&nbsp;KBits/sec</td>
          </tr>
          <tr id="PPortSettings">
            <td class="title" colspan="2" id="QoSSetupPPort">Priority port settings</td>
          </tr>
          <tr id="user_Qos_HPP">
            <td class="head">User High Priority Ports</td>
            <td><input name="QoS_HPP" class="superwide" size="60" maxlength="60" type="text"></td>
          </tr>
          <tr id="user_Qos_LPP">
            <td class="head">User Low Priority Ports</td>
            <td><input name="QoS_LPP" class="superwide" size="60" maxlength="60" type="text"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input value="/internet/qos.asp" name="submit-url" type="hidden">
              <input type=submit class="normal" value="Apply" id="QoSApply">
              &nbsp; &nbsp;
              <input type=reset  class="normal" value="Reset" id="QoSReset" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
