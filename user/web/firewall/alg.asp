<!DOCTYPE html>
<html>
	<head>
		<title>MAC Filtering Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_firewall.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			function initTranslation() {
				_TR("algTitle",			"alg title");
				_TR("algIntroduction",	"alg introduction");
				_TR("algSetting",		"alg setting");
				_TR("algApply",		"button apply");
			}

			function initValues() {
				var st = {
					'ftp':   NVRAM_fwAlgFTP,
					'gre':   NVRAM_fwAlgGRE,
					'h323':  NVRAM_fwAlgH323,
					'ipsec': NVRAM_fwAlgIPSec,
					'l2tp':  NVRAM_fwAlgL2TP,
					'pptp':  NVRAM_fwAlgPPTP,
					'sip':   NVRAM_fwAlgSIP,
					'rtsp':  NVRAM_fwAlgRTSP
				};
				
				for (var field in st)
					setElementChecked('alg_' + field, st[field] == '1');
					
				showWarning();
				initTranslation();
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 id="algTitle">ALG Settings</h1>
					<p id="algIntroduction">On this page you can enable/disable ALG services.</p>
					<hr>
					<iframe id="timerReloader" name="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="alg" action="/goform/setFirewallALG" onSubmit="ajaxShowTimer(this, 'timerReloader', _('message apply'), 15);">
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
								<td><input type="checkbox" id="alg_pptp" name="alg_pptp" onClick="if (this.checked) this.form.alg_gre.checked = true;"></td>
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
								<td><input type="submit" class="normal" id="algApply" value="Apply"></td>
							</tr>
						</table>
					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>
