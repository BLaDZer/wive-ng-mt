<!DOCTYPE html>
<html>
	<head>
		<title>System Management</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_admin.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			function initTranslation() {
			  _TR("manTitle",				"management title");
			  _TR("manIntroduction",			"management introduction");
			  _TR("loading",				"management uploading firmware");
			  _TR("manDontRemoveUSB",			"management dont remove usb");
			  _TR("manLangSet",				"management language settings");
			  _TR("manSelectLang",				"management language select");

			  _TR("manUserSet",				"management user settings");
			  _TR("manUserLogin",				"management user login");
			  _TR("manUserPassword",			"management user password");
			  _TR("manUserConfirmation",			"management user confirmation");
			  _TR("manUserEmptyText",			"management user empty password text");


			  _TR("manAdmLogin",				"management administrator login");
			  _TR("manOrdLogin",				"management ordinary user login");
			  _TR("manMgmtLogin",				"management management user login");
			  _TR("manAdmPasswd",				"management administrator password");


			  _TR("manAdmFirmware",				"management firmware");
			  _TR("uploadFWLocation",			"management filename");
			  _TR("uploadFWtext",				"management firmware text");
			  _TR("uploadFWnote",				"management firmware note");
			  _TR("manResetRWFS",				"management reset rwfs");
			  _TR("manRWFSUpload",				"management upload rwfs");
			  _TR("uploadRWFSLocation",			"management filename");
			  _TR("manSettingsManag",			"management settings");
			  _TR("setmanExpSetButton",			"management backup file");
			  _TR("setmanImpSetFileLocation",	"management upload file");
			  _TR("manResetToFactory", 			"management reset factory");

			  _TR("manLangApply", 				"button apply");
			  _TR("manAdmApply", 				"button apply");
			  _TR("uploadFWApply", 			"button update");
			  _TR("uploadRWFSApply", 			"button load");
			  _TR("setmanExpSetExport", 		"button backup");
			  _TR("setmanImpSetImport", 		"button load");
			  _TR("setmanLoadDefault", 		"button reset");
			}

			function initValue() {
				var lang_element = document.getElementById("langSelection");

				lang_element.options.length = 0;
				if (BUILD_LangEN)
					lang_element.options[lang_element.length] = new Option('English', 'en');
				if (BUILD_LangRU)
					lang_element.options[lang_element.length] = new Option('Russian', 'ru');
				lang_element.value = NVRAM_Language;
				
				document.getElementById('admuser').value			= NVRAM_Login;
				document.getElementById('admpass').value			= NVRAM_Password;
				document.getElementById('admpassconf').value			= NVRAM_Password;

				document.getElementById('orduser').value			= NVRAM_UserLogin;
				document.getElementById('ordpass').value			= NVRAM_UserPassword;
				document.getElementById('ordpassconf').value			= NVRAM_UserPassword;

				document.getElementById('mgmtuser').value			= NVRAM_MngmtLogin;
				document.getElementById('mgmtpass').value			= NVRAM_MngmtPassword;
				document.getElementById('mgmtpassconf').value			= NVRAM_MngmtPassword;


				document.getElementById("loading").style.display	= "none";

				showWarning();
				initTranslation();

				disableControlsByAuth();
			}

			function AdmFormCheck(form) {
				var re_login = /^[a-zA-Z0-9_]+$/;
				if (!re_login.test(form.admuser.value)) {
					alert(_("management uncorrect login"));
					form.admuser.select();
					form.admuser.focus();
					return false;
				}

				var re_login_user = /^[a-zA-Z0-9_]*$/;
				if (!re_login_user.test(form.orduser.value)) {
					alert(_("management uncorrect login"));
					form.orduser.select();
					form.orduser.focus();
					return false;
				}

				if (!re_login_user.test(form.mgmtuser.value)) {
					alert(_("management uncorrect login"));
					form.mgmtuser.select();
					form.mgmtuser.focus();
					return false;
				}

				if (form.orduser.value == form.admuser.value) {
					alert(_("management login overlap"));
					form.orduser.select();
					form.orduser.focus();
					return false;
				}

				if (form.mgmtuser.value == form.admuser.value) {
					alert(_("management login overlap"));
					form.mgmtuser.select();
					form.mgmtuser.focus();
					return false;
				}

				if (form.mgmtuser.value == form.orduser.value && form.mgmtuser.value != '') {
					alert(_("management login overlap"));
					form.mgmtuser.select();
					form.mgmtuser.focus();
					return false;
				}


				var re_pass = /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
				if (!re_pass.test(form.admpass.value)) {
					alert(_("management uncorrect password"));
					form.admpass.select();
					form.admpass.focus();
					return false;
				}

				var re_pass_user = /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]*$/;
				if (!re_pass_user.test(form.ordpass.value)) {
					alert(_("management uncorrect password"));
					form.ordpass.select();
					form.ordpass.focus();
					return false;
				}

				if (!re_pass_user.test(form.mgmtpass.value)) {
					alert(_("management uncorrect password"));
					form.mgmtpass.select();
					form.mgmtpass.focus();
					return false;
				}

				
				if (form.admpass.value != form.admpassconf.value) {
					alert(_("management password confirmation")); 
					form.admpass.value = "";
					form.admpassconf.value = "";
					form.admpass.focus();
					return false;
				}

				if (form.ordpass.value != form.ordpassconf.value) {
					alert(_("management password confirmation")); 
					form.admpass.value = "";
					form.admpassconf.value = "";
					form.admpass.focus();
					return false;
				}

				if (form.mgmtpass.value != form.mgmtpassconf.value) {
					alert(_("management password confirmation")); 
					form.mgmtpass.value = "";
					form.mgmtpassconf.value = "";
					form.mgmtpass.focus();
					return false;
				}


				return true;
			}


			function setLanguage() {
				parent.menu.location.reload();
				return true;
			}

			function onUploadFirmwareSubmit(form) {
				if (checkFilePresent(form.filename))
				{
					ajaxReloadDelayedPage(150000);
					ajaxPostForm(
						_("management dont power off"),
						form,
						'firmwareReloader',
						_("message upgrade"),
						ajaxShowProgress);
				}
			}

			function onImportSettings(form) {
				if (checkFilePresent(form.filename))
					ajaxPostForm(
						_("management ask upload settings"),
						form,
						'setmanReloader',
						_("message config"),
						ajaxShowProgress);
			}

			function onUploadRWFSSubmit(form) {
				
				if (checkFilePresent(form.filename))
					ajaxPostForm(
						_("management ask upload rwfs"),
						form,
						'RWFSReloader',
						_("message rwfs"),
						ajaxShowProgress);
			}

			function onReset2DefaultsSubmit(form) {
				ajaxPostForm(
				  _("management ask reset factory"),
				  form,
				  'defaultsReloader',
				  _("message config"),
				  ajaxShowProgress);
			}

			function showWarning() {
				var warning_access_password		= NVRAM_Password == "Admin";
				var warning_wireless_security		= NVRAM_AuthMode == "OPEN";
				var warning_wireless_key		= NVRAM_WPAPSK1 == "1234567890";
				var warningHTML					= "";

				if (warning_access_password || warning_wireless_security || warning_wireless_key) {
					warningHTML += '<br>';
					warningHTML += '<table class="warning">';
					warningHTML += '<tr><th class="warning" align="center" colspan="2">' + _("warning header") + '</th></tr>';
					if  (warning_access_password) {
						warningHTML += '<tr>';
						warningHTML += '<td class="warning" colspan="2">' + _("warning access password") + '</td>';
						warningHTML += '</tr>';
					}
					if (warning_access_password && (warning_wireless_security || warning_wireless_key)) {
						warningHTML += '<tr><td colspan="2"><hr class="warning"></td></tr>';
					}
					if  (warning_wireless_security || warning_wireless_key) {
						warningHTML += '<tr>';
						warningHTML += '<td class="warning">' + _("warning wireless security") + '</td>';
						warningHTML += '<td align="right" class="warning"><input align="right" type="button" style="{width:120px;}" value="' + _("button warning") + '" onClick=\'window.location.assign("/wireless/main.asp");\'></td>';
						warningHTML += '</tr>';
					}
					warningHTML += '</table>';
					warningHTML += '<br>';
					ajaxModifyElementHTML('warning', warningHTML);
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValue();">
		<div id="warning"></div>
		<table class="body" style="width:600px;">
			<tr>
				<td>
					<h1 id="manTitle">System Management</h1>
					<div id="manIntroduction">
						<p>You can select language and set administrator login and password here.</p>
						<p>You can also upgrade the Wive-NG-MT firmware to obtain new functionality.</p>
						<p>It takes about 5 minutes to upload firmware &amp; upgrade flash. Please be patient.</p>
						<p style="color: #ff0000;">Caution! A corrupted image will hang up the system.</p>
					</div>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<p id="loading" style="display: none; color: #ff0000; font-size: 16px;"> Uploading firmware <br>
						<br id="manDontRemoveUSB">Please be patient and don't remove USB device if present... 
					</p>
					<div id="staticControls">
						<!-- ================= Langauge Settings ================= -->
						<table class="form">
							<tr>
								<td class="title" colspan="2" id="manLangSet">Language Settings</td>
							</tr>
							<tr>
								<td class="head" id="manSelectLang" style="width: 192px">Select Language</td>
								<td>
									<form method="POST" name="Lang" action="/goform/setSysLang">
										<select name="langSelection" id="langSelection" class="half"></select>
										<input type="hidden" name="submit-url" value="/adm/management.asp">&nbsp;
										<input type="submit" class="half" value="Apply" id="manLangApply" onClick="setTimeout(function () { parent.menu.location.reload(); }, 500);">&nbsp;&nbsp;
									</form>
								</td>
							</tr>
						</table>
						<!-- ================= Adm Settings ================= -->
						<form method="POST" name="Adm" action="/goform/setSysAdm" onSubmit="return AdmFormCheck(this);">
							<table class="form">
								<tr>
									<td class="title" colspan="4" id="manUserSet">User Settings</td>
								</tr>

								<tr>
									<th></th>
									<th id="manUserLogin">Login</th>
									<th id="manUserPassword">Password</th>
									<th id="manUserConfirmation">Confirmation</th>
								</tr>
								<tr class="auth-hide-user">
									<td class="head" id="manAdmLogin" style="width: 192px">Administrator</td>
									<td colspan="1"><input type="text" class="normal" id="admuser" name="admuser" size="16" maxlength="16"></td>
									<td style="width: 162px"><input type="password" class="normal" id="admpass" name="admpass" size="16" maxlength="32"></td>
									<td><input type="password" class="normal" id="admpassconf" name="admpassconf" size="16" maxlength="32"></td>
								</tr>
								<tr>
									<td class="head" id="manOrdLogin" style="width: 192px">Ordinary user</td>
									<td colspan="1"><input type="text" class="normal" id="orduser" name="orduser" size="16" maxlength="16"></td>
									<td style="width: 162px"><input type="password" class="normal" id="ordpass" name="ordpass" size="16" maxlength="32"></td>
									<td><input type="password" class="normal" id="ordpassconf" name="ordpassconf" size="16" maxlength="32"></td>
								</tr>

								<tr class="auth-hide-user">
									<td class="head" id="manMgmtLogin" style="width: 192px">Management user</td>
									<td colspan="1"><input type="text" class="normal" id="mgmtuser" name="mgmtuser" size="16" maxlength="16"></td>
									<td style="width: 162px"><input type="password" class="normal" id="mgmtpass" name="mgmtpass" size="16" maxlength="32"></td>
									<td><input type="password" class="normal" id="mgmtpassconf" name="mgmtpassconf" size="16" maxlength="32"></td>
								</tr>
								<tr>
									<td class="head" ></td>
									<td colspan="1">
										<input type="hidden" name="submit-url" value="/adm/management.asp">
										<input type="submit" class="half" value="Apply" id="manAdmApply">
									</td>

									<td colspan="2" style="text-align: center;">
										<span id="manUserEmptyText">Set empty to prevent user access</span>
									</td>
								</tr>
							</table>

						</form>
						<!-- ================= Firmware ================= -->
						<table class="form auth-hide-user">
							<tr>
								<td colspan="2" class="title auth-hide-user" id="manAdmFirmware">Firmware update</td>
							</tr>
							<tr>
								<td class="head" id="uploadFWLocation">Filename:</td>
								<td class="value">
									<form method="POST" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
										<input type="checkbox" name="reset_rwfs" checked="checked"><span id="manResetRWFS">Replace(update) RWFS</span><br>
										<input name="filename" size="20" maxlength="256" type="file" style="max-width: 420px">
										<input type="button" value="Update" id="uploadFWApply" class="half" name="UploadFirmwareSubmit" onClick="onUploadFirmwareSubmit(this.form);">
										<br>
										<iframe id="firmwareReloader" name="firmwareReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
									</form>
								</td>
							</tr>
							<tr>
								<td colspan="2">
									<span id="uploadFWtext"></span><a href="https://sourceforge.net/projects/wive-ng/files/wive-ng-mt/" target="_blank">https://sourceforge.net/projects/wive-ng/files/wive-ng-mt/</a><br>
									<span id="uploadFWnote"></span>
								</td>
							</tr>
						</table>
						<!-- ================= RwFs ================= -->
						<table class="form auth-hide-user">
							<tr>
								<td colspan="2" class="title" id="manRWFSUpload">RW-FS Upload</td>
							</tr>
							<tr>
								<td class="head" id="uploadRWFSLocation">Filename:</td>
								<td class="value">
									<form method="POST" name="UploadRWFS" action="/cgi-bin/upload_rwfs.cgi" enctype="multipart/form-data">
										<input type="file" name="filename" maxlength="256" style="max-width: 420px">
										<input type="button" value="Load" id="uploadRWFSApply" class="half" onClick="onUploadRWFSSubmit(this.form);">
										<iframe id="RWFSReloader" name="RWFSReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
									</form>
								</td>
							</tr>
						</table>
						<!-- ================= Settings management ================= -->
						<table class="form auth-hide-user">
							<tr>
								<td class="title" colspan="2" id="manSettingsManag">Router Settings Management</td>
							</tr>
							<tr>
								<td class="head" id="setmanExpSetButton">Backup Settings to file</td>
								<td>
									<form method="GET" name="ExportSettings" action="/goform/getDeviceConfig" onsubmit="return confirm(_('management export settings'));">
										<input type="submit" value="Backup" id="setmanExpSetExport" name="Export" class="half">
									</form>
								</td>
							</tr>
							<tr>
								<td class="head" id="setmanImpSetFileLocation">Load settings from file</td>
								<td>
									<form method="POST" name="ImportSettings" action="/cgi-bin/upload_settings.cgi" enctype="multipart/form-data">
										<input type="file" name="filename" maxlength="256" style="max-width: 420px">
										<input type="button" value="Load" id="setmanImpSetImport" class="half" onClick="onImportSettings(this.form);">
										<iframe id="setmanReloader" name="setmanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
									</form>
								</td>
							</tr>
							<tr>
								<td class="head" id="manResetToFactory">Reset to factory defaults</td>
								<td>
									<form method="GET" name="LoadDefaultSettings" action="/goform/LoadDefaultSettings">
										<input type="button" value="Reset" id="setmanLoadDefault" name="LoadDefault" class="half" onClick="onReset2DefaultsSubmit(this.form);">
										<iframe name="defaultsReloader" id="defaultsReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
									</form>
								</td>
							</tr>
						</table>
					</div>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>
