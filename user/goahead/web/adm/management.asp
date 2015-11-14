<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<title>System Management</title>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("admin");
Butterlate.setTextDomain("buttons");

function initTranslation()
{
  _TR("manTitle", "management title");
  _TR("manIntroduction", "management introduction");
  _TR("loading", "management uploading firmware");
  _TR("manDontRemoveUSB", "management dont remove usb");
  _TR("manLangSet", "management language settings");
  _TR("manSelectLang", "management language select");
  _TR("manAdmSet", "management administrator settings");
  _TR("manAdmLodin", "management administrator login");
  _TR("manAdmPasswd", "management administrator password");
  _TR("manAdmFirmware", "management firmware");
  _TR("uploadFWLocation", "management filename");
  _TR("manResetRWFS", "management reset rwfs");
  _TR("manRWFSUpload", "management upload rwfs");
  _TR("uploadRWFSLocation", "management filename");
  _TR("manSettingsManag", "management settings");
  _TR("setmanExpSetButton", "management backup file");
  _TR("setmanImpSetFileLocation", "management upload file");
  _TR("manResetToFactory", "management reset factory");

  _TRV("manLangApply", "button apply");
  _TRV("manAdmApply", "button apply");
  _TRV("uploadFWApply", "button update");
  _TRV("uploadRWFSApply", "button load");
  _TRV("setmanExpSetExport", "button backup");
  _TRV("setmanImpSetImport", "button load");
  _TRV("setmanLoadDefault", "button reset");
}

function SubmitForm(message, form)
{
	if (confirm(message))
		form.submit();
}

function AdmFormCheck(form)
{
	var re_login = /^[a-zA-Z0-9_]+$/;
	if (!re_login.test(form.admuser.value))
	{
		alert(_("management uncorrect login"));
		form.admuser.focus();
		return false;
	}

	var re_pass = /^[a-zA-Z0-9_\{\}\[\];:\'\"\,\.\/\?<>\-\=\+\\\!\~\`\|\@\#\%^\&\*\(\~`)]+$/;
	if (!re_pass.test(form.admpass.value))
	{
		alert(_("management uncorrect password"));
		form.admpass.focus();
		return false;
	}
	return true;
}

function initValue()
{
	var lang_element = document.getElementById("langSelection");
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_ru = "<% getLangBuilt("ru"); %>";

	lang_element.options.length = 0;
	if (lang_en == "1")
		lang_element.options[lang_element.length] = new Option('English', 'en');
	if (lang_ru == "1")
		lang_element.options[lang_element.length] = new Option('Russian', 'ru');

	if (document.cookie.length > 0)
	{
		var s = document.cookie.indexOf("language=");
		var e = document.cookie.indexOf(";", s);
		var lang = "en";
		var i;

		if (s != -1)
		{
			if (e == -1)
				lang = document.cookie.substring(s+9);
			else
				lang = document.cookie.substring(s+9, e);
		}
		for (i=0; i<lang_element.options.length; i++) {
			if (lang == lang_element.options[i].value) {
				lang_element.options.selectedIndex = i;
				break;
			}
		}
	}
  initTranslation();
	// Firmware
	document.getElementById("loading").style.display="none";
}

function setLanguage()
{
	document.cookie="language="+document.Lang.langSelection.value+"; path=/";
	parent.menu.location.reload();
	return true;
}

function onUploadFirmwareSubmit(form)
{
	if (checkFilePresent(form.filename))
		ajaxPostForm(
			_("management dont power off"),
			form,
			'firmwareReloader',
			_("message upgrade"),
			ajaxShowProgress);
}

function onImportSettings(form)
{
	if (checkFilePresent(form.filename))
		ajaxPostForm(
			_("management ask upload settings"),
			form,
			'setmanReloader',
			_("message config"),
			ajaxShowProgress);
}

function onUploadRWFSSubmit(form)
{
	if (checkFilePresent(form.filename))
		ajaxPostForm(
			_("management ask upload rwfs"),
			form,
			'RWFSReloader',
			_("message rwfs"),
			ajaxShowProgress);
}

function onReset2DefaultsSubmit(form)
{
    ajaxPostForm(
      _("management ask reset factory"),
      form,
      'defaultsReloader',
      _("message config"),
      ajaxShowProgress);
}
</script>
</head>
<body onLoad="initValue();">
<table class="body" style="width:600px;">
  <tr>
    <td><h1 id="manTitle">System Management</h1>
      <div id="manIntroduction">
        <p>You can select language and set administrator login and password here.</p>
        <p>You can also upgrade the Wive-NG-MT firmware to obtain new functionality.
          It takes about 2 minute to upload firmware &amp; upgrade flash. Please be patient.</p>
        <p style="color: #ff0000;">Caution! A corrupted image will hang up the system.</p>
      </div>
      <hr>
      <p id="loading" style="display: none; color: #ff0000; font-size: 16px;"> Uploading firmware <br>
        <br id="manDontRemoveUSB">Please be patient and don't remove USB device if present... </p>
      <div id="staticControls">
        <!-- ================= Langauge Settings ================= -->
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="manLangSet">Language Settings</td>
          </tr>
          <tr>
            <td class="head" id="manSelectLang">Select Language</td>
            <td><form method="POST" name="Lang" action="/goform/setSysLang">
                <select name="langSelection" id="langSelection">
                  <!-- added by initValue -->
                </select>
                <input type="hidden" name="submit-url" value="/adm/management.asp" >
                <input type="submit" class="half" value="Apply" id="manLangApply" onClick="return setLanguage();">
                &nbsp; &nbsp;
              </form></td>
          </tr>
        </table>
        <!-- ================= Adm Settings ================= -->
        <form method="POST" name="Adm" action="/goform/setSysAdm" onSubmit="return AdmFormCheck(this);">
          <table class="form">
            <tr>
              <td class="title" colspan="2" id="manAdmSet">Administrator Settings</td>
            </tr>
            <tr>
              <td class="head" id="manAdmLodin">Login</td>
              <td><input type="text" name="admuser" size="16" maxlength="16" value='<% getCfgGeneral(1, "Login"); %>'></td>
            </tr>
            <tr>
              <td class="head" id="manAdmPasswd">Password</td>
              <td><input type="password" name="admpass" size="16" maxlength="32" value='<% getCfgGeneral(1, "Password"); %>' onblur="this.select(); this.setAttribute('type','password');" onfocus="this.select(); this.setAttribute('type','text');"></td>
            </tr>
            <tr>
              <td class="head" ></td>
              <td><input type="hidden" name="submit-url" value="/adm/management.asp" >
                <input type="submit" class="half" value="Apply" id="manAdmApply"></td>
            </tr>
          </table>
        </form>
        <!-- ================= Firmware ================= -->
        <table class="form">
          <tr>
            <td colspan="2" class="title" id="manAdmFirmware">Firmware update</td>
          </tr>
          <tr>
            <td class="head" id="uploadFWLocation">Filename:</td>
            <td class="value"><form method="POST" name="UploadFirmware" action="/cgi-bin/upload.cgi" enctype="multipart/form-data">
                <input type="checkbox" name="reset_rwfs" checked="checked"><span id="manResetRWFS">Reset RWFS on update</span><br>
                <input name="filename" size="20" maxlength="256" type="file">
                <input type="button" value="Update" id="uploadFWApply" class="half" name="UploadFirmwareSubmit" onClick="onUploadFirmwareSubmit(this.form);">
                <br>
                <iframe id="firmwareReloader" name="firmwareReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
              </form></td>
          </tr>
        </table>
        <!-- ================= RwFs ================= -->
        <table class="form">
          <tr>
            <td colspan="2" class="title" id="manRWFSUpload">RW-FS Upload</td>
          </tr>
          <tr>
            <td class="head" id="uploadRWFSLocation">Filename:</td>
            <td class="value"><form method="POST" name="UploadRWFS" action="/cgi-bin/upload_rwfs.cgi" enctype="multipart/form-data">
                <input type="file" name="filename" maxlength="256">
                <input type="button" value="Load" id="uploadRWFSApply" class="half" onClick="onUploadRWFSSubmit(this.form);">
                <iframe id="RWFSReloader" name="RWFSReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
              </form></td>
          </tr>
        </table>
        <!-- ================= Settings management ================= -->
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="manSettingsManag">Router Settings Management</td>
          </tr>
          <tr>
            <td class="head" id="setmanExpSetButton">Backup Settings to file</td>
            <td><form method="GET" name="ExportSettings" action="/cgi-bin/ExportSettings.sh"
			onsubmit="return confirm(_('management export settings'));" >
                <input type="submit" value="Backup" id="setmanExpSetExport" name="Export" class="half">
              </form></td>
          </tr>
          <tr>
            <td class="head" id="setmanImpSetFileLocation">Load settings from file</td>
            <td><form method="POST" name="ImportSettings" action="/cgi-bin/upload_settings.cgi" enctype="multipart/form-data">
                <input type="file" name="filename" maxlength="256">
                <input type="button" value="Load" id="setmanImpSetImport" class="half" onClick="onImportSettings(this.form);">
                <iframe id="setmanReloader" name="setmanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
              </form></td>
          </tr>
          <tr>
            <td class="head" id="manResetToFactory">Reset to factory defaults</td>
            <td><form method="GET" name="LoadDefaultSettings" action="/goform/LoadDefaultSettings">
                <input type="button" value="Reset" id="setmanLoadDefault" name="LoadDefault" class="half" onClick="onReset2DefaultsSubmit(this.form);">
                <iframe name="defaultsReloader" id="defaultsReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
              </form></td>
          </tr>
        </table>
      </div>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
