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
		<script type="text/javascript" src="/lang/b28n.js"></script>
		<script type="text/javascript" src="/js/nvram.js"></script>
		<script type="text/javascript" src="/js/ajax.js"></script>
		<script type="text/javascript" src="/js/controls.js"></script>
		<script type="text/javascript" src="/js/validation.js"></script>
		<script>
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			function initTranslation() {
			  _TR("sambaTitle",			"services samba title");
			  _TR("sambaIntroduction",	"services samba introduction");
			  _TR("sambaSettings",		"services samba settings");
			  _TR("sambaEnabled",		"services samba enabled");
			  _TR("sambaDisable",		"button disable");
			  _TR("sambaEnable",		"button enable");
			  _TR("sambaWorkgroup",		"services samba workgroup");
			  _TR("sambaNetbios",		"services samba netbios name");
			  _TR("sambaString",		"services samba server string");
			  _TR("sambaOSLevel",		"services samba os level");
			  _TR("sambaTimeServer",	"services samba time server");
			  _TR("sambaDisable2",		"button disable");
			  _TR("sambaEnable2",		"button enable");
			  _TRV("sambaApply",		"button apply");
			}

			function initValues() {
				document.getElementById('SmbEnabled').value		= NVRAM_SmbEnabled;
				document.getElementById('SmbTimeserver').value	= NVRAM_SmbTimeserver;
				document.getElementById('WorkGroup').value		= NVRAM_WorkGroup;
				document.getElementById('SmbNetBIOS').value		= NVRAM_SmbNetBIOS;
				document.getElementById('SmbString').value		= NVRAM_SmbString;
				document.getElementById('SmbOsLevel').value		= NVRAM_SmbOsLevel;

				smbEnabledSwitch(document.formSamba);
				initTranslation();
				showWarning();
			}

			function smbEnabledSwitch() {
				enableElements([document.getElementById('WorkGroup'), document.getElementById('SmbNetBIOS'), document.getElementById('SmbString'), document.getElementById('SmbOsLevel'), document.getElementById('SmbTimeserver')], document.getElementById('SmbEnabled').value == '1');
				displayElement(['div_workgroup', 'div_netbios', 'div_desc', 'div_os_level', 'div_time_server'], document.getElementById('SmbEnabled').value == '1');
				displayServiceStatus();
			}

			function checkForm() {
				if (document.getElementById('SmbEnabled').value == '1') {
					if (document.getElementById('WorkGroup').value == '')	{
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
				ajaxShowTimer(document.formSamba, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function displayServiceHandler(response) {
				var services = [
					// turned_on, row_id, daemon_id
					[ NVRAM_SmbEnabled, 'samba', 'nmbd' ]
				];

			  // Create associative array
			  var tmp		= response.split(',');
			  var daemons	= [];
			  for (var i = 0; i < tmp.length; i++)
				daemons[tmp[i]] = 1;

			  // Now display all services
			  for (var i=0; i<services.length; i++) {
				var service = services[i];
				var row		= document.getElementById(service[1]);
				var tds		= [];
				for (var j = 0; j < row.childNodes.length; j++)
				  if (row.childNodes[j].nodeName == 'TD')
					tds.push(row.childNodes[j]);

				if (row != null) {
					// Fill-up status
					if (service[0]*1 == '0')
						tds[2].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
					else
						tds[2].innerHTML = (daemons[service[2]] == 1) ?
							'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
							'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
					}
				}
				setTimeout('displayServiceStatus();', 5000);
			}

			function displayServiceStatus() {
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
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
						<tr>
							<td class="title" colspan="3" id="sambaSettings">Samba/WINS Settings</td>
						</tr>
						<tr id="samba">
							<td class="head" style="width: 40%" id="sambaEnabled">Enable Samba</td>
							<td>
								<select name="SmbEnabled" id="SmbEnabled" class="mid" onChange="smbEnabledSwitch(this.form);">
									<option value="0" id="sambaDisable">Disable</option>
									<option value="1" id="sambaEnable">Enable</option>
								</select>
							</td>
							<td style="width: 56px;">&nbsp;</td>
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
					<div class="whitespace">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
