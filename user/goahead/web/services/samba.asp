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
				document.formSamba.SmbEnabled.value		= NVRAM_SmbEnabled;
				document.formSamba.SmbTimeserver.value	= NVRAM_SmbTimeserver;
				document.formSamba.WorkGroup.value		= NVRAM_WorkGroup;
				document.formSamba.SmbNetBIOS.value		= NVRAM_SmbNetBIOS;
				document.formSamba.SmbString.value		= NVRAM_SmbString;
				document.formSamba.SmbOsLevel.value		= NVRAM_SmbOsLevel;

				smbEnabledSwitch(document.formSamba);
				initTranslation();
				showWarning();
			}

			function smbEnabledSwitch(form) {
				enableElements( [ form.WorkGroup, form.SmbNetBIOS, form.SmbString, form.SmbOsLevel, form.SmbTimeserver  ] , form.SmbEnabled.value == '1');
				displayElement( [ 'div_workgroup', 'div_netbios', 'div_desc', 'div_os_level', 'div_time_server' ], form.SmbEnabled.value == '1');
				displayServiceStatus();
			}

			function checkForm(form) {
				if (form.SmbEnabled.value == '1') {
					if (form.WorkGroup.value == '')	{
						alert(_("services samba no workgroup"));
						form.WorkGroup.focus();
						return false;
					}
					
					if (form.SmbNetBIOS.value == '') {
						alert(_("services samba no netbios"));
						form.SmbNetBIOS.focus();
						return false;
					}
					
					// Check OS level
					var os_level = -1;
					if (validateNum(form.SmbOsLevel.value, false))
						os_level = 1*form.SmbOsLevel.value;
					
					if ((os_level < 0) || (os_level > 255)) {
						alert(_("services samba invalid os level"));
						form.SmbOsLevel.focus();
						return false;
					}
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function displayServiceHandler(response) {
				var form = document.l2tpConfig;
				var services = [
					// turned_on, row_id, daemon_id
					[ NVRAM_SmbEnabled, 'samba', 'nmbd' ],
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
		<table class="body">
			<tr id="warning"></tr>
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
								<select name="SmbEnabled" class="mid" onChange="smbEnabledSwitch(this.form);">
									<option value="0" id="sambaDisable">Disable</option>
									<option value="1" id="sambaEnable">Enable</option>
								</select>
							</td>
							<td style="width: 56px;">&nbsp;</td>
						</tr>
						<tr id="div_workgroup">
							<td class="head" id="sambaWorkgroup">Workgroup</td>
							<td colspan="2"><input name="WorkGroup" class="mid"></td>
						</tr>
						<tr id="div_netbios">
							<td class="head" id="sambaNetbios">Netbios name</td>
							<td colspan="2"><input name="SmbNetBIOS" class="mid"></td>
						</tr>
						<tr id="div_desc">
							<td class="head" id="sambaString">Server string</td>
							<td colspan="2"><input name="SmbString" class="mid"></td>
						</tr>
						<tr id="div_os_level">
							<td class="head" id="sambaOSLevel">OS level</td>
							<td colspan="2"><input name="SmbOsLevel" class="mid"></td>
						</tr>
						<tr id="div_time_server">
							<td class="head" id="sambaTimeServer">Enable time server</td>
							<td colspan="2">
								<select name="SmbTimeserver" class="mid">
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
				</td>
			</tr>
		</table>
	</body>
</html>
