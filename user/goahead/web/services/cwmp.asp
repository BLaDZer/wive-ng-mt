<!DOCTYPE html>
<html>
	<head>
		<title>CWMP Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			var statusAdvancedMenu	= 1;

			// Set translation
			function initTranslation()
			{
				_TR("cwmpTitle",			"services cwmp title");
				_TR("cwmpIntroduction",		"services cwmp introduction");
				_TR("cwmpSettings",			"services cwmp settings");
				_TR("cwmpdEnabled",			"services cwmp enabled");
				_TR("cwmpACStype",			"services cwmp acs type");
				_TR("cwmpACSurl",			"services cwmp acs url");
				_TR("cwmpACSauth",			"services cwmp acs auth");
				_TR("cwmpACSusername",		"services cwmp acs user");
				_TR("cwmpACSpassword",		"services cwmp acs pass");
				_TR("cwmpCPEauth",			"services cwmp cpe auth");
				_TR("cwmpCPEusername",		"services cwmp cpe user");
				_TR("cwmpCPEpassword",		"services cwmp cpe pass");
				_TR("cwmpCAfile",			"services cwmp ca file");
				_TR("cwmpCApassword",		"services cwmp ca pass");
				_TR("cwmpLogLevel",			"services cwmp adv log level");
				_TR("cwmpHTTPport",			"services cwmp adv http port");
				_TR("cwmpHTTPtimeout",		"services cwmp adv http timeout");
				_TR("cwmpSesConnTimeout",	"services cwmp adv sesconn timeout");
				_TR("cwmpSesRespTimeout",	"services cwmp adv sesresp timeout");
				_TR("cwmpCPEmanifactire",	"services cwmp adv cpe manufacture");
				_TR("cwmpCPEoui",			"services cwmp adv cpe oui");
				_TR("cwmpCPEpc",			"services cwmp adv cpe pc");
				_TR("cwmpLogCrit",			"services cwmp log crit");
				_TR("cwmpLogError",			"services cwmp log error");
				_TR("cwmpLogWarn",			"services cwmp log warn");
				_TR("cwmpLogInfo",			"services cwmp log info");
				_TR("cwmpLogDebug",			"services cwmp log debug");
				_TR("cwmpLogTrace",			"services cwmp log trace");
				_TR("cwmpInformPeriodic",	"services cwmp inform periodic");
				_TR("cwmpInformInterval",	"services cwmp inform interval");

				_TRV("cwmpApply",			"button apply");
				_TRV("cwmpCancel",			"button cancel");
				_TRV("cwmpReset",			"button reset");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					if (elements[i].id == "Disable")
						elements[i].innerHTML = _("button disable");
					else if (elements[i].id == "Enable")
						elements[i].innerHTML = _("button enable");
			}

			// Set inintal values
			function initValues()
			{
				var i, opt, tmp;

				document.getElementById('cwmp_enabled').options.selectedIndex = '<% getCfgZero(1, "cwmpdEnabled"); %>'

				opt = document.getElementById('cwmp_acs_type').options;
				for (var i = 0; i < opt.length;i++) {
					if (opt[i].value == '<% getCfgGeneral(1, "cwmp_acs_type"); %>')
						opt[i].selected = true;
				}

				document.getElementById('cwmp_acs_url').value = '<% getCfgGeneral(1, "cwmp_acs_url"); %>';

				document.getElementById('cwmp_cpe_auth').options.selectedIndex = '<% getCfgZero(1, "cwmp_cpe_auth"); %>'
				document.getElementById('cwmp_cpe_username').value = ('<% getCfgGeneral(1, "cwmp_cpe_username"); %>' != '') ? '<% getCfgGeneral(1, "cwmp_cpe_username"); %>' : 'cwmp';
				document.getElementById('cwmp_cpe_password').value = ('<% getCfgGeneral(1, "cwmp_cpe_password"); %>' != '') ? '<% getCfgGeneral(1, "cwmp_cpe_password"); %>' : 'cwmp';

				document.getElementById('cwmp_acs_auth').options.selectedIndex = '<% getCfgZero(1, "cwmp_acs_auth"); %>'
				document.getElementById('cwmp_acs_username').value = ('<% getCfgGeneral(1, "cwmp_acs_username"); %>' != '') ? '<% getCfgGeneral(1, "cwmp_acs_username"); %>' : 'cwmp';
				document.getElementById('cwmp_acs_password').value = ('<% getCfgGeneral(1, "cwmp_acs_password"); %>' != '') ? '<% getCfgGeneral(1, "cwmp_acs_password"); %>' : 'cwmp';

				document.getElementById('cwmpd_inform_periodic_enable').options.selectedIndex = '<% getCfgZero(1, "cwmpd_inform_periodic_enable"); %>';
				document.getElementById('cwmpd_inform_periodic_interval').value = ('<% getCfgGeneral(1, "cwmpd_inform_periodic_interval"); %>' == '') ? 3600 : '<% getCfgGeneral(1, "cwmpd_inform_periodic_interval"); %>';

				opt = document.getElementById('cwmpd_log_level').options;
				tmp = '<% getCfgGeneral(1, "cwmpd_log_level"); %>';
				if (tmp.length > 0) {
					for (var i = 0; i < opt.length;i++)
						if (opt[i].value == tmp) 
							opt[i].selected = true;
				}
				else
					opt[3].selected = true;	// Info

				document.getElementById('cwmpd_httpd_port').value = ('<% getCfgGeneral(1, "cwmpd_httpd_port"); %>' != '') ? '<% getCfgGeneral(1, "cwmpd_httpd_port"); %>' : '1008';
				document.getElementById('cwmpd_httpd_timeout').value = ('<% getCfgGeneral(1, "cwmpd_httpd_timeout"); %>' != '') ? '<% getCfgGeneral(1, "cwmpd_httpd_timeout"); %>' : '5000';
				document.getElementById('cwmpd_session_connect_timeout').value = ('<% getCfgGeneral(1, "cwmpd_session_connect_timeout"); %>' != '') ? '<% getCfgGeneral(1, "cwmpd_session_connect_timeout"); %>' : '15';
				document.getElementById('cwmpd_session_response_timeout').value = ('<% getCfgGeneral(1, "cwmpd_session_response_timeout"); %>' != '') ? '<% getCfgGeneral(1, "cwmpd_session_response_timeout"); %>' : '30';

				document.getElementById('cwmp_cpe_manufacture').value = '<% getCfgGeneral(1, "cwmp_cpe_manufacture"); %>';
				document.getElementById('cwmp_cpe_oui').value = '<% getCfgGeneral(1, "cwmp_cpe_oui"); %>';
				document.getElementById('cwmp_cpe_pc').value = '<% getCfgGeneral(1, "cwmp_cpe_pc"); %>';

				document.getElementById('cwmp_ca_file').value = ('<% getCfgGeneral(1, "cwmp_ca_file"); %>' != '') ? '<% getCfgGeneral(1, "cwmp_ca_file"); %>' : '/etc/raddb/certs/ca.pem';
				document.getElementById('cwmp_ca_password').value = ('<% getCfgGeneral(1, "cwmp_ca_password"); %>' != '') ? '<% getCfgGeneral(1, "cwmp_ca_password"); %>' : '';
				displayElement([ 'cwmpCAfile_tr', 'cwmpCApassword_tr' ], '<% getOpenSSLBuilt(); %>' == '1');

				cwmpEnableSwitch();
				showAdvancedMenu();

				displayServiceStatus();
				showWarning();
				initTranslation();
			}

			// Check values
			function CheckValues()
			{
				if (document.getElementById('cwmp_enabled').value == 1) {
					var re_url		= /^[а-яА-Яa-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
					var re_name		= /^[a-zA-Z0-9_-]+$/;
					var re_pass	= /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
					var re_num		= /^[0-9]+$/;
					var re_file		= /^[a-zA-Z0-9\_\.\-\/]+$/;

					// Check ACS URL
					if (!re_url.test(document.getElementById('cwmp_acs_url').value) || document.getElementById('cwmp_acs_url').value.length == 0) {
						alert(_("services cwmp uncorrect url"));
						document.getElementById('cwmp_acs_url').select();
						document.getElementById('cwmp_acs_url').focus();
						return false;
					}

					// Check ACS username & password
					if (document.getElementById('cwmp_acs_auth').value == 1) {
						if (!re_name.test(document.getElementById('cwmp_acs_username').value) || document.getElementById('cwmp_acs_username').value.length == 0) {
							alert(_("services cwmp uncorrect name"));
							document.getElementById('cwmp_acs_username').select();
							document.getElementById('cwmp_acs_username').focus();
							return false;
						}
						if (!re_pass.test(document.getElementById('cwmp_acs_password').value) || document.getElementById('cwmp_acs_password').value.length == 0) {
							alert(_("services cwmp uncorrect password"));
							document.getElementById('cwmp_acs_password').select();
							document.getElementById('cwmp_acs_password').focus();
							return false;
						}
					}

					// Check CPE username & password
					if (document.getElementById('cwmp_cpe_auth').value == 1) {
						if (!re_name.test(document.getElementById('cwmp_cpe_username').value) || document.getElementById('cwmp_cpe_username').value.length == 0) {
							alert(_("services cwmp uncorrect name"));
							document.getElementById('cwmp_cpe_username').select();
							document.getElementById('cwmp_cpe_username').focus();
							return false;
						}
						if (!re_pass.test(document.getElementById('cwmp_cpe_password').value) || document.getElementById('cwmp_cpe_password').value.length == 0) {
							alert(_("services cwmp uncorrect password"));
							document.getElementById('cwmp_cpe_password').select();
							document.getElementById('cwmp_cpe_password').focus();
							return false;
						}
					}

					// Check CA file & password
					if ('<% getOpenSSLBuilt(); %>' == '1') {
						if (!re_file.test(document.getElementById('cwmp_ca_file').value) || document.getElementById('cwmp_ca_file').value.length == 0) {
							alert(_("services cwmp uncorrect ca file"));
							document.getElementById('cwmp_ca_file').select();
							document.getElementById('cwmp_ca_file').focus();
							return false;
						}
						if (!re_pass.test(document.getElementById('cwmp_ca_password').value) && document.getElementById('cwmp_ca_password').value.length > 0) {
							alert(_("services cwmp uncorrect password"));
							document.getElementById('cwmp_ca_password').select();
							document.getElementById('cwmp_ca_password').focus();
							return false;
						}
					}

					// Check HTTP port
					if (!re_num.test(document.getElementById('cwmpd_httpd_port').value) || document.getElementById('cwmpd_httpd_port').value.length == 0 ||
					   (+document.getElementById('cwmpd_httpd_port').value < 1 || +document.getElementById('cwmpd_httpd_port').value > 65535)) {
						if (statusAdvancedMenu == 0)
							showAdvancedMenu();
						alert(_("services cwmp uncorrect port"));
						document.getElementById('cwmpd_httpd_port').select();
						document.getElementById('cwmpd_httpd_port').focus();
						return false;
					}
					else
						document.getElementById('cwmpd_httpd_port').value = +document.getElementById('cwmpd_httpd_port').value;

					// Check HTTP timeout
					if (!re_num.test(document.getElementById('cwmpd_httpd_timeout').value) || document.getElementById('cwmpd_httpd_timeout').value.length == 0) {
						if (statusAdvancedMenu == 0)
							showAdvancedMenu();
						alert(_("services cwmp uncorrect timeout"));
						document.getElementById('cwmpd_httpd_timeout').select();
						document.getElementById('cwmpd_httpd_timeout').focus();
						return false;
					}
					else
						document.getElementById('cwmpd_httpd_timeout').value = +document.getElementById('cwmpd_httpd_timeout').value;

					// Check Session Connect Timeout
					if (!re_num.test(document.getElementById('cwmpd_session_connect_timeout').value) || document.getElementById('cwmpd_session_connect_timeout').value.length == 0) {
						if (statusAdvancedMenu == 0)
							showAdvancedMenu();
						alert(_("services cwmp uncorrect timeout"));
						document.getElementById('cwmpd_session_connect_timeout').select();
						document.getElementById('cwmpd_session_connect_timeout').focus();
						return false;
					}
					else
						document.getElementById('cwmpd_session_connect_timeout').value = +document.getElementById('cwmpd_session_connect_timeout').value;

					// Check Session Response Timeout
					if (!re_num.test(document.getElementById('cwmpd_session_response_timeout').value) || document.getElementById('cwmpd_session_response_timeout').value.length == 0) {
						if (statusAdvancedMenu == 0)
							showAdvancedMenu();
						alert(_("services cwmp uncorrect timeout"));
						document.getElementById('cwmpd_session_response_timeout').select();
						document.getElementById('cwmpd_session_response_timeout').focus();
						return false;
					}
					else
						document.getElementById('cwmpd_session_response_timeout').value = +document.getElementById('cwmpd_session_response_timeout').value;

					// Check Inform Periodic Interval
					if (document.getElementById('cwmpd_inform_periodic_enable').value == 1 && (!re_num.test(document.getElementById('cwmpd_inform_periodic_interval').value) || document.getElementById('cwmpd_inform_periodic_interval').value <= 0)) {
						if (statusAdvancedMenu == 0)
							showAdvancedMenu();
						alert(_("services cwmp uncorrect timeout"));
						document.getElementById('cwmpd_inform_periodic_interval').select();
						document.getElementById('cwmpd_inform_periodic_interval').focus();
						return false;
					}
					else
						document.getElementById('cwmpd_inform_periodic_interval').value = +document.getElementById('cwmpd_inform_periodic_interval').value;
				}
				ajaxShowTimer(document.cwmpConfig, 'timerReloader', _('message apply'), 15);
				return true;
			}

			// Show/hide CWMP settings
			function cwmpEnableSwitch()
			{
				displayElement([ 'cwmpACStype_tr', "cwmpACSurl_tr", 'cwmpCPEauth_tr', 'cwmpACSauth_tr', 'cwmpCAfile_tr', 'cwmpCApassword_tr', 'cwmpAdvanced_tr' ], document.getElementById('cwmp_enabled').value == 1);
				displayElement([ 'cwmpCPEusername_tr', 'cwmpCPEpassword_tr' ], document.getElementById('cwmp_enabled').value == 1 && document.getElementById('cwmp_cpe_auth').value == 1);
				displayElement([ 'cwmpACSusername_tr', 'cwmpACSpassword_tr' ], document.getElementById('cwmp_enabled').value == 1 && document.getElementById('cwmp_acs_auth').value == 1);
				displayElement([ 'cwmpLogLevel_tr', 'cwmpHTTPport_tr', 'cwmpHTTPtimeout_tr', 'cwmpSesConnTimeout_tr', 'cwmpSesRespTimeout_tr',
								 'cwmpCPEmanifactire_tr', 'cwmpCPEoui_tr', 'cwmpCPEpc_tr' ], document.getElementById('cwmp_enabled').value == 1 && statusAdvancedMenu == 1);
			}

			// Show/hide CWMP Inform Periodic Interval
			function cwmpPeriodicSwitch()
			{
				displayElement([ 'cwmpInterval_tr' ], statusAdvancedMenu == 1 && document.getElementById('cwmpd_inform_periodic_enable').value == 1);
			}

			function showAdvancedMenu() {
				var elements = [ 'cwmpLogLevel_tr', 'cwmpHTTPport_tr', 'cwmpHTTPtimeout_tr', 'cwmpSesConnTimeout_tr', 'cwmpSesRespTimeout_tr',
								 'cwmpCPEmanifactire_tr', 'cwmpCPEoui_tr', 'cwmpCPEpc_tr', 'cwmpPeriodic_tr' ];
				if (statusAdvancedMenu == 0) {
					ajaxModifyElementHTML('cwmpAdvanced', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services cwmp advanced"));
					statusAdvancedMenu = 1;
					displayElement(elements, true);
				} else {
					ajaxModifyElementHTML('cwmpAdvanced', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services cwmp advanced"));
					statusAdvancedMenu = 0;
					displayElement(elements, false);
				}
				cwmpPeriodicSwitch();
			}

			// Display server status
			function displayServiceHandler(response)
			{
				var daemons = response.split(',');

				if ('<% getCfgGeneral(1, "cwmpdEnabled"); %>' == '0' || '<% getCfgGeneral(1, "cwmpdEnabled"); %>' == '')
					document.getElementById('cwmp_status').innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
				else
					document.getElementById('cwmp_status').innerHTML = (daemons.indexOf('cwmpd') >= 0) ?
						'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
						'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';

				setTimeout('displayServiceStatus();', 5000);
			}

			// Get server status
			function displayServiceStatus()
			{
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"></tr>
			<tr>
				<td><h1 id="cwmpTitle">CWMP (TR-069) Setup</h1>
					<p id="cwmpIntroduction">Here you can configure CWMP (TR-069) settings.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="cwmpConfig" action="/goform/cwmpConfig">
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" id="cwmpSettings">CWMP (TR-069) Settings</td>
								</tr>
								<tr>
									<td class="head" id="cwmpdEnabled">CWMP (TR-069)</td>
									<td>
										<select id="cwmp_enabled" name="cwmp_enabled" class="mid" onChange="cwmpEnableSwitch();">
											<option value="0" id="Disable">Disable</option>
											<option value="1" id="Enable">Enable</option>
										</select>
									</td>
									<td id="cwmp_status" style="text-align: center;">&nbsp;</td>
								</tr>
								<tr id="cwmpACStype_tr">
									<td class="head" id="cwmpACStype">ACS Type</td>
									<td colspan="2">
										<select id="cwmp_acs_type" name="cwmp_acs_type" class="mid">
											<option value="tr-098-1-8-0" id="cwmpTypeErtelecom">TR-098 1.8.0</option>
											<option value="dlink_101" id="cwmpTypeDlink">D-Link ACS 1.0.1</option>
											<option value="dlink_115" id="cwmpTypeDlink">D-Link ACS 1.1.5 (modified TR-098 1.8.0)</option>
										</select>
									</td>
								</tr>
								<tr id="cwmpACSurl_tr">
									<td class="head" id="cwmpACSurl">ACS Address</td>
									<td colspan="2">
										<input id="cwmp_acs_url" name="cwmp_acs_url" style="width: 99%" />
									</td>
								</tr>
								<tr id="cwmpACSauth_tr">
									<td class="head" id="cwmpACSauth">ACS Authentification</td>
									<td colspan="2">
										<select id="cwmp_acs_auth" name="cwmp_acs_auth" class="mid" onChange="cwmpEnableSwitch();">
											<option value="0" id="Disable">Disable</option>
											<option value="1" id="Enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="cwmpACSusername_tr">
									<td class="head" id="cwmpACSusername">ACS Username</td>
									<td colspan="2">
										<input type="text" id="cwmp_acs_username" name="cwmp_acs_username" class="mid" />
									</td>
								</tr>
								<tr id="cwmpACSpassword_tr">
									<td class="head" id="cwmpACSpassword">ACS Password</td>
									<td colspan="2">
										<input type="password" id="cwmp_acs_password" name="cwmp_acs_password" class="mid" />
									</td>
								</tr>
								<tr id="cwmpCPEauth_tr">
									<td class="head" id="cwmpCPEauth">CPE Authentification</td>
									<td colspan="2">
										<select id="cwmp_cpe_auth" name="cwmp_cpe_auth" class="mid" onChange="cwmpEnableSwitch();">
											<option value="0" id="Disable">Disable</option>
											<option value="1" id="Enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="cwmpCPEusername_tr">
									<td class="head" id="cwmpCPEusername">CPE Username</td>
									<td colspan="2">
										<input type="text" id="cwmp_cpe_username" name="cwmp_cpe_username" class="mid" />
									</td>
								</tr>
								<tr id="cwmpCPEpassword_tr">
									<td class="head" id="cwmpCPEpassword">CPE Password</td>
									<td colspan="2">
										<input type="password" id="cwmp_cpe_password" name="cwmp_cpe_password" class="mid" />
									</td>
								</tr>
								<tr id="cwmpCAfile_tr">
									<td class="head" id="cwmpCAfile">SSL CA File</td>
									<td colspan="2">
										<input type="text" id="cwmp_ca_file" name="cwmp_ca_file" style="width: 99%" />
									</td>
								</tr>
								<tr id="cwmpCApassword_tr">
									<td class="head" id="cwmpCApassword">SSL CA Password</td>
									<td colspan="2">
										<input type="password" id="cwmp_ca_password" name="cwmp_ca_password" class="mid" />
									</td>
								</tr>
								<tr id="cwmpAdvanced_tr">
									<td class="title" colspan="3" id="cwmpAdvanced" onClick="showAdvancedMenu();">Advanced settings</td>
								</tr>
								<tr id="cwmpPeriodic_tr">
									<td class="head" id="cwmpInformPeriodic">CPE Inform periodic</td>
									<td colspan="2">
										<select name="cwmpd_inform_periodic_enable" id="cwmpd_inform_periodic_enable" class="mid" onChange="cwmpPeriodicSwitch();">
											<option value="0" id="Disable">Disable</option>
											<option value="1" id="Enable">Enable</option>
										</select>
									</td>
								</tr>
								<tr id="cwmpInterval_tr">
									<td class="head" id="cwmpInformInterval">CPE Inform interval</td>
									<td colspan="2">
										<input id="cwmpd_inform_periodic_interval" name="cwmpd_inform_periodic_interval" class="mid" />
									</td>
								</tr>
								<tr id="cwmpLogLevel_tr">
									<td class="head" id="cwmpLogLevel">CPE Log Level</td>
									<td colspan="2">
										<select id="cwmpd_log_level" name="cwmpd_log_level" class="mid" onChange="cwmpEnableSwitch();">
											<option value="crit"  id="cwmpLogCrit">Critical</option>
											<option value="error" id="cwmpLogError">Error</option>
											<option value="warn"  id="cwmpLogWarn">Warning</option>
											<option value="info"  id="cwmpLogInfo">Info</option>
											<option value="debug" id="cwmpLogDebug">Debug</option>
											<option value="trace" id="cwmpLogTrace">Trace</option>
										</select>
									</td>
								</tr>
								<tr id="cwmpHTTPport_tr">
									<td class="head" id="cwmpHTTPport">HTTP Port</td>
									<td colspan="2">
										<input type="text" id="cwmpd_httpd_port" name="cwmpd_httpd_port" class="mid" />
									</td>
								</tr>
								<tr id="cwmpHTTPtimeout_tr">
									<td class="head" id="cwmpHTTPtimeout">HTTP Timeout</td>
									<td colspan="2">
										<input id="cwmpd_httpd_timeout" name="cwmpd_httpd_timeout" class="mid" />
									</td>
								</tr>
								<tr id="cwmpSesConnTimeout_tr">
									<td class="head" id="cwmpSesConnTimeout">Session Connect Timeout</td>
									<td colspan="2">
										<input id="cwmpd_session_connect_timeout" name="cwmpd_session_connect_timeout" class="mid" />
									</td>
								</tr>
								<tr id="cwmpSesRespTimeout_tr">
									<td class="head" id="cwmpSesRespTimeout">Session Response Timeout</td>
									<td colspan="2">
										<input id="cwmpd_session_response_timeout" name="cwmpd_session_response_timeout" class="mid" />
									</td>
								</tr>
								<tr id="cwmpCPEmanifactire_tr">
									<td class="head" id="cwmpCPEmanifactire">CPE Manufacture</td>
									<td colspan="2">
										<input id="cwmp_cpe_manufacture" name="cwmp_cpe_manufacture" class="mid" />
									</td>
								</tr>
								<tr id="cwmpCPEoui_tr">
									<td class="head" id="cwmpCPEoui">CPE OUI</td>
									<td colspan="2">
										<input id="cwmp_cpe_oui" name="cwmp_cpe_oui" class="mid" />
									</td>
								</tr>
								<tr id="cwmpCPEpc_tr">
									<td class="head" id="cwmpCPEpc">CPE Product Class</td>
									<td colspan="2">
										<input id="cwmp_cpe_pc" name="cwmp_cpe_pc" class="mid" />
									</td>
								</tr>
							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" id="cwmpApply"  value="Apply"  onClick="return CheckValues();">&nbsp;&nbsp;
									<input type="button" class="normal" id="cwmpCancel" value="Cancel" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="normal" id="cwmpReset"  value="Reset"  onClick="resetValues(this.form);">
									<input value="0" name="reset" type="hidden">
								</td>
							</tr>
						</table>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>