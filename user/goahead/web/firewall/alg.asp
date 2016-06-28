<!DOCTYPE html>
<html>
<head>
<title>MAC Filtering Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("firewall");
Butterlate.setTextDomain("buttons");

function updateState()
{
	var st = {
		'ftp': '<% getCfgZero(1, "fwAlgFTP"); %>',
		'gre': '<% getCfgZero(1, "fwAlgGRE"); %>',
		'h323': '<% getCfgZero(1, "fwAlgH323"); %>',
		'ipsec': '<% getCfgZero(1, "fwAlgIPSec"); %>',
		'l2tp': '<% getCfgZero(1, "fwAlgL2TP"); %>',
		'pptp': '<% getCfgZero(1, "fwAlgPPTP"); %>',
		'sip': '<% getCfgZero(1, "fwAlgSIP"); %>',
		'rtsp': '<% getCfgZero(1, "fwAlgRTSP"); %>'
	};

  initTranslation();

	for (var field in st)
		setElementChecked('alg_' + field, st[field] == '1');
}

function initTranslation()
{
  _TR("algTitle", "alg title");
  _TR("algIntroduction", "alg introduction");
  _TR("algSetting", "alg setting");

  _TRV("algApply", "button apply");
}

</script>
</head>
<body bgcolor="#FFFFFF" onLoad="updateState();">
<table class="body">
  <tr>
    <td><h1 id="algTitle">ALG Settings</h1>
      <p id="algIntroduction">On this page you can enable/disable ALG services.</p>
      <hr>
      <form method="post" name="alg" action="/goform/setFirewallAlg" onSubmit="ajaxShowTimer(this, 'timerReloader', _('message apply'), 5);">
      <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="algSetting">ALG Settings</td>
          </tr>
          <tr>
            <td class="head">FTP</td>
            <td><input type="checkbox" id="alg_ftp" name="alg_ftp"></td>
          </tr>
          <tr>
            <td class="head">GRE</td>
            <td><input type="checkbox" id="alg_gre" name="alg_gre"></td>
          </tr>
          <tr>
            <td class="head">H.323</td>
            <td><input type="checkbox" id="alg_h323" name="alg_h323"></td>
          </tr>
          <tr>
            <td class="head">PPTP</td>
            <td><input type="checkbox" id="alg_pptp" name="alg_pptp"></td>
          </tr>
          <tr>
            <td class="head">SIP</td>
            <td><input type="checkbox" id="alg_sip" name="alg_sip"></td>
          </tr>
          <tr>
            <td class="head">RTSP</td>
            <td><input type="checkbox" id="alg_rtsp" name="alg_rtsp"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="submit-url" value="/firewall/alg.asp">
              <input type="submit" class="normal" id="algApply" value="Apply">
            </td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
