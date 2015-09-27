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
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("buttons");
Butterlate.setTextDomain("network");

function initTranslation()
{
  _TR("QoSTitleStr", "qos title");
  _TR("QoSIntroStr", "qos introduction");
  _TR("QoSSetupStr", "qos setup");
  _TR("QoSStr", "qos type");
  _TR("QoSSetupBW", "qos bandwidth settings");

  _TR("QoSModeStr", "qos mode");
  _TR("QoSAll", "qos mode all");
  _TR("QoSWAN", "qos mode wan");
  _TR("QoSVPN", "qos mode vpn");

  _TR("QoSUpBWStr", "qos upload rate");
  _TR("QoSUpBWLStr", "qos upload limit");
  _TR("QoSDownBWStr", "qos download rate");
  _TR("QoSDownBWLStr", "qos download limit");
  _TR("QoSUPVPNBWStr", "qos upload vpn rate");
  _TR("QoSUPVPNBWLStr", "qos upload vpn limit");
  _TR("QoSSetupPPort", "qos priority port");
  _TR("QoSHPP", "qos high priority ports");
  _TR("QoSLPP", "qos low priority ports");
  _TR("QoSSetupPDSCP", "qos priority dscp");
  _TR("QoSHDSCP", "qos high priority dscps");
  _TR("QoSLDSCP", "qos low priority dscps");
  _TR("QoSDisable", "button disable");
  _TR("QoSSimple", "qos simple");
  _TR("QoSShaper", "qos shaper");
  _TR("QoSCODEL", "qos codel");
  _TR("fastpath_warning", "qos warning");

  _TRV("QoSApply", "button apply");
  _TRV("QoSReset", "button reset");
}

function QoSSelectChange(form)
{
	var complex_on = form.QoSSelect.value == '2';
  var nat_fp = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");

	// Display mode-dependent elements
	displayElement( [ 'BWSettings', 'QoSMode', 'PPortSettings', 'QoSUpBW', 'QoSUpBWL', 'QoSDownBW', 'QoSDownBWL', 'QoSUPVPNBW', 'QoSUPVPNBWL', 'user_Qos_HPP', 'user_Qos_LPP', 'user_Qos_HDSCP', 'user_Qos_LDSCP' ], complex_on );
  displayElement('fastpath_warning', (nat_fp == '2') || (nat_fp == '3'));
}

function initializeForm(form)
{
  form.QoSSelect.value    = '<% getCfgGeneral(1, "QoSEnable"); %>';
  form.QoSMode.value    = '<% getCfgGeneral(1, "QoSMode"); %>';
	form.UpBW.value    		= '<% getCfgGeneral(1, "QoS_rate_up"); %>';
	form.UpBWL.value		= '<% getCfgGeneral(1, "QoS_rate_limit_up"); %>';
	form.DownBW.value		= '<% getCfgGeneral(1, "QoS_rate_down"); %>';
	form.DownBWL.value		= '<% getCfgGeneral(1, "QoS_rate_limit_down"); %>';
	form.UpVPNBW.value		= '<% getCfgGeneral(1, "QoS_rate_vpn_up"); %>';
	form.UpVPNBWL.value		= '<% getCfgGeneral(1, "QoS_rate_vpn_limit_up"); %>';
	form.QoS_HPP.value		= '<% getCfgGeneral(1, "QoS_high_pp"); %>';
	form.QoS_LPP.value		= '<% getCfgGeneral(1, "QoS_low_pp"); %>';
	form.QoS_HDSCP.value		= '<% getCfgGeneral(1, "QoS_high_dscp"); %>';
	form.QoS_LDSCP.value		= '<% getCfgGeneral(1, "QoS_low_dscp"); %>';
}

function bodyOnLoad(form)
{
	initializeForm(form);
	QoSSelectChange(form);
  initTranslation();
}

</script>
</head>
<!--     body      -->
<body onLoad="bodyOnLoad(document.QoSSetup);">
<table class="body">
  <tr>
    <td><h1 id="QoSTitleStr">Quality of Service Settings </h1>
      <p id="QoSIntroStr"> Here you can setup rules to provide desired Quality of Service for specific applications.</p>
      <div style="display:none;" id="fastpath_warning">For correct operation of QoS (modes: Simple Priority-Based, Complex, Codel) need turning off <b>NAT offload mode</b>. Please note that turning off <b>NAT offload mode</b> will very increase CPU usage.</div>
      <hr>
      <form method="post" name="QoSSetup" action="/goform/QoSSetup">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="QoSSetupStr">QoS Setup</td>
          </tr>
          <tr>
            <td class="head" id="QoSStr">Type of QoS</td>
            <td><select name="QoSSelect" onChange="QoSSelectChange(this.form);" class="wide">
                <option value="0" id="QoSDisable">Disable</option>
                <option value="1" id="QoSSimple">Simple priority based QoS</option>
                <option value="2" id="QoSShaper">Complex with shaper (ports based mark + HTB limit)</option>
                <option value="3" id="QoSCODEL">Automatic Fair Queue Controlled Delay AQM (CODEL)</option>
              </select></td>
          </tr>
          <tr id="BWSettings">
            <td class="title" colspan="2" id="QoSSetupBW">Bandwidth settings</td>
          </tr>
          <tr id="QoSMode">
            <td class="head" id="QoSModeStr">Shape from:</td>
            <td><select name="QoSMode" class="wide">
                <option value="0" id="QoSAll">All</option>
                <option value="1" id="QoSWAN">Only WAN</option>
                <option value="2" id="QoSVPN">Only VPN</option>
              </select></td>
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
            <td class="head" id="QoSHPP">User High Priority Ports</td>
            <td><input name="QoS_HPP" class="superwide" size="60" maxlength="1024" type="text"></td>
          </tr>
          <tr id="user_Qos_LPP">
            <td class="head" id="QoSLPP">User Low Priority Ports</td>
            <td><input name="QoS_LPP" class="superwide" size="60" maxlength="1024" type="text"></td>
          </tr>
          <tr id="PDSCPSettings">
            <td class="title" colspan="2" id="QoSSetupPDSCP">Priority DSCP settings</td>
          </tr>
          <tr id="user_Qos_HDSCP">
            <td class="head" id="QoSHDSCP">User High Priority DSCPs</td>
            <td><input name="QoS_HDSCP" class="superwide" size="60" maxlength="1024" type="text"></td>
          </tr>
          <tr id="user_Qos_LDSCP">
            <td class="head" id="QoSLDSCP">User Low Priority DSCPs</td>
            <td><input name="QoS_LDSCP" class="superwide" size="60" maxlength="1024" type="text"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input value="/internet/qos.asp" name="submit-url" type="hidden">
              <input type=submit class="normal" value="Apply" id="QoSApply">
              <input type=reset  class="normal" value="Reset" id="QoSReset" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
