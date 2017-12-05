<!DOCTYPE html>
<html>
	<head>
		<title>Samba/WINS setup</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_services.js"></script>
		<script type="text/javascript" src="/js/nvram.js"></script>
		<script type="text/javascript" src="/js/ajax.js"></script>
		<script type="text/javascript" src="/js/controls.js"></script>
		<script type="text/javascript" src="/js/validation.js"></script>
		<script>
			function initTranslation() {
				_TR("sambaTitle",		"services samba title");
				_TR("sambaIntroduction",	"services samba introduction");
				_TR("sambaSettings",		"services samba settings");
				_TR("sambaEnabled",		"services samba enabled");
				_TR("sambaDisable",		"button disable");
				_TR("sambaEnable",		"button enable");
				_TR("sambaWorkgroup",		"services samba workgroup");
				_TR("sambaNetbios",		"services samba netbios name");
				_TR("sambaString",		"services samba server string");
				_TR("sambaOSLevel",		"services samba os level");
				_TR("sambaTimeServer",		"services samba time server");
				_TR("sambaDisable2",		"button disable");
				_TR("sambaEnable2",		"button enable");
				_TR("sambaApply",		"button apply");
			}

			function initValues() {
				document.getElementById('SmbEnabled').value		= NVRAM_SmbEnabled;
				document.getElementById('SmbTimeserver').value		= NVRAM_SmbTimeserver;
				document.getElementById('WorkGroup').value		= NVRAM_WorkGroup;
				document.getElementById('SmbNetBIOS').value		= NVRAM_SmbNetBIOS;
				document.getElementById('SmbString').value		= NVRAM_SmbString;
				document.getElementById('SmbOsLevel').value		= NVRAM_SmbOsLevel;

				smbEnabledSwitch(document.formSamba);
				initTranslation();
				showWarning();
				displayServiceStatus([[ NVRAM_SmbEnabled, 'samba', 'nmbd' ]]);
			}

			function smbEnabledSwitch() {
				enableElements([	document.getElementById('WorkGroup'),
							document.getElementById('SmbNetBIOS'),
							document.getElementById('SmbString'),
							document.getElementById('SmbOsLevel'),
							document.getElementById('SmbTimeserver') ], document.getElementById('SmbEnabled').value == '1');
				displayElement([	'div_workgroup',
							'div_netbios',
							'div_desc',
							'div_os_level',
							'div_time_server' ], document.getElementById('SmbEnabled').value == '1');
			}

			function checkForm() {
				if (document.getElementById('SmbEnabled').value == '1') {
					if (document.getElementById('WorkGroup').value == '') {
						alert(_("services samba no workgroup"));
						document.getElementById('WorkGroup').focus();
						return false;
					}

					if (document.getElementById('SmbNetBIOS').value == '') {
						alert(_("services samba no netbios"));
						document.getElementById('SmbNetBIOS').focus();
						return false;
					}

					// Check OS level
					var os_level = -1;
					if (validateNum(document.getElementById('SmbOsLevel').value, false))
						os_level = +document.getElementById('SmbOsLevel').value;

					if (os_level < 0 || os_level > 255) {
						alert(_("services samba invalid os level"));
						document.getElementById('SmbOsLevel').focus();
						return false;
					}
				}
				ajaxShowTimer(document.formSamba, 'timerReloader', _('message apply'), 5);
				return true;
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="sambaTitle">Samba/WINS Setup</h1>
					<p id="sambaIntroduction">Here you can configure Samba/WINS service.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form action="/goform/formSamba" method="POST" name="formSamba" onSubmit="return checkForm(this);" >
					<table class="form">
						<col style="width: 40%"/>
						<col style="width: 50%"/>
						<col style="width: 10%"/>
						<tbody>
							<tr>
								<td class="title" colspan="3" id="sambaSettings">Samba/WINS Settings</td>
							</tr>
							<tr id="samba">
								<td class="head" id="sambaEnabled">Enable Samba</td>
								<td>
									<select name="SmbEnabled" id="SmbEnabled" class="mid" onChange="smbEnabledSwitch(this.form);">
										<option value="0" id="sambaDisable">Disable</option>
										<option value="1" id="sambaEnable">Enable</option>
									</select>
								</td>
								<td style="text-align: center"></td>
							</tr>
							<tr id="div_workgroup">
								<td class="head" id="sambaWorkgroup">Workgroup</td>
								<td colspan="2"><input name="WorkGroup" id="WorkGroup" class="mid"></td>
							</tr>
							<tr id="div_netbios">
								<td class="head" id="sambaNetbios">Netbios name</td>
								<td colspan="2"><input name="SmbNetBIOS" id="SmbNetBIOS" class="mid"></td>
							</tr>
							<tr id="div_desc">
								<td class="head" id="sambaString">Server string</td>
								<td colspan="2"><input name="SmbString" id="SmbString" class="mid"></td>
							</tr>
							<tr id="div_os_level">
								<td class="head" id="sambaOSLevel">OS level</td>
								<td colspan="2"><input name="SmbOsLevel" id="SmbOsLevel" class="mid"></td>
							</tr>
							<tr id="div_time_server">
								<td class="head" id="sambaTimeServer">Enable time server</td>
								<td colspan="2">
									<select name="SmbTimeserver" id="SmbTimeserver" class="mid">
										<option value="0" id="sambaDisable2">Disable</option>
										<option value="1" id="sambaEnable2">Enable</option>
									</select>
								</td>
							</tr>
						</tbody>
					</table>
					<table class="buttons">
						<tr>
							<td>
								<input type="hidden" value="/services/samba.asp" name="submit-url">
								<input type="submit" class="normal" id="sambaApply" value="Apply">
							</td>
						</tr>
					</table>
					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>