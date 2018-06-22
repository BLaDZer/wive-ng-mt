<!DOCTYPE html>
<html>
	<head>
		<title>System Log</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_admin.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script>
			function initTranslation() {
				_TR("syslogTitle",			"syslog title");
				_TR("syslogIntroduction",	"syslog introduction");
				_TR("syslogSetup",			"syslog setup");
				_TR("syslogEnabled",		"syslog enabled");
				_TR("syslogKernel",			"syslog kernel");
				_TR("syslogRemoteIP",		"syslog remote ip");
				_TR("syslogSysLog",			"syslog system log");
				_TR("syslogLogLevel",			"syslog level");

				_TR("syslogApply",			"button apply");
				_TR("syslogClear",			"button clear");
				_TR("syslogRefresh",		"button refresh");
				_TR("syslogClear2",		"button clear");
				_TR("syslogRefresh2",		"button refresh");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
				if (elements[i].id == "disable")
					elements[i].innerHTML = _("button disable");
				else if (elements[i].id == "enable")
					elements[i].innerHTML = _("button enable");
			}

			function initValue() {
				document.getElementById('KLogd').value			= NVRAM_KLogd;
				document.getElementById('SysLogd').value		= NVRAM_SysLogd;
				document.getElementById('SysLogLevel').value		= NVRAM_SysLogLevel;
				document.getElementById('RemoteSysLogIP').value	= NVRAM_RemoteSysLogIP;

				syslogdSelect();
				ajaxPerformRequest("/goform/getsyslog", uploadLogField);
				showWarning();
				initTranslation();
			}

			function uploadLogField(str) {
				if (str == "-1") {
					document.getElementById("syslog").value = _("syslog not support");
				}
				else {
					document.getElementById("syslog").value = str;
					document.getElementById("syslog").scrollTop = document.getElementById("syslog").scrollHeight;
				}
			}

			function checkSetupForm(form){
				if (form.RemoteSysLogIP.value != '') {
					if (!validateDNS(form.RemoteSysLogIP) && !validateIP(form.RemoteSysLogIP)) {
						alert(_("syslog invalid ip dns"));
						form.RemoteSysLogIP.select();
						form.RemoteSysLogIP.focus();
						return false;
					}
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
				return true;
			}

			function syslogdSelect() {
				displayElement([ 'rmtSysLogIP', 'klogdRow', 'syslog_view' ], document.getElementById('SysLogd').value == '1');
			}
			
			function refreshSysLog() {
				ajaxPerformRequest('/goform/getsyslog', uploadLogField);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValue()">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="syslogTitle">System Log</h1>
					<p id="syslogIntroduction">Here you can configure system logging</p>
					<hr>
					<!-- ================= System log setup ================= -->
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="post" name="LogdSetup" action="/goform/setuplog" onSubmit="return checkSetupForm(this);">
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="syslogSetup">System Log Setup:</td>
						</tr>
						<tr>
							<td class="head" id="syslogEnabled">System logging daemon:</td>
							<td>
								<select name="SysLogd" id="SysLogd" onChange="syslogdSelect();" class="normal">
									<option value="0" id="disable">Disable</option>
									<option value="1" id="enable">Enable</option>
								</select>
							</td>
						</tr>

						<tr id="logLevelRow">
							<td class="head" id="syslogLogLevel">System log level:</td>
							<td>
								<select name="SysLogLevel" id="SysLogLevel" class="normal">
									<option value="1" id="sllEmergency">Emergency</option>
									<option value="2" id="sllAlert">Alert</option>
									<option value="3" id="sllCritical">Critical</option>
									<option value="4" id="sllError">Error</option>
									<option value="5" id="sllWarning">Warning</option>
									<option value="6" id="sllNotice">Notice</option>
									<option value="7" id="sllInfo">Info</option>
									<option value="8" id="sllDebug">Debug</option>
								</select>
							</td>
						</tr>

						<tr id="klogdRow">
							<td class="head" id="syslogKernel">Kernel logging daemon:</td>
							<td>
								<select name="KLogd" id="KLogd" class="normal">
									<option value="0" id="disable">Disable</option>
									<option value="1" id="enable">Enable</option>
								</select>
							</td>
						</tr>
						<tr id="rmtSysLogIP" style="display: none;">
							<td class="head" id="syslogRemoteIP">Remote system log IP:</td>
							<td><input name="RemoteSysLogIP" id="RemoteSysLogIP" class="normal"></td>
						</tr>

					</table>
					<table class="buttons">
						<tr>
							<td>
								<input type="submit" value="Apply" id="syslogApply" class="normal">
								<input type="hidden" name="submit-url" value="/adm/syslog.asp" >
							</td>
						</tr>
					</table>
					</form>
					<br>
					<!-- ================= System log ================= -->
					<table class="form" id="syslog_view" style="display: none;">
						<tr>
							<td class="title" colspan="2" id="syslogSysLog">System Log: </td>
						</tr>
						<tr>
							<td colspan="2">
								<form method="post" name="SubmitClearLog1" action="/goform/clearlog">
									<input class="normal" style="margin-right: 10px" type="button" value="Refresh" id="syslogRefresh" name="refreshlog" onClick="ajaxPerformRequest('/goform/getsyslog', uploadLogField);"><input class="normal" type="button" value="Clear" id="syslogClear" name="clearlog" onClick="ajaxPostRequest('/goform/clearlog', 'stub=value', false, refreshSysLog);">
								</form>
							</td>
						</tr>
						<tr>
							<td><textarea style="font-size: 8pt; width: 99%; height: 480px; border: 1px solid;" name="syslog" id="syslog" wrap="off" readonly="1"></textarea></td>
						</tr>
						<tr>
							<td colspan="2">
								<form method="post" name="SubmitClearLog2" action="/goform/clearlog">
									<input class="normal" style="margin-right: 10px" type="button" value="Refresh" id="syslogRefresh2" name="refreshlog" onClick="ajaxPerformRequest('/goform/getsyslog', uploadLogField);"><input class="normal" type="button" value="Clear" id="syslogClear2" name="clearlog" onClick="ajaxPostRequest('/goform/clearlog', 'stub=value', false, refreshSysLog);">
								</form>
							</td>
						</tr>
					</table>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>
