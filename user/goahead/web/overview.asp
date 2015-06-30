<!DOCTYPE html>
<html>
<head>
<title>Wive-NG-MT</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("view");
Butterlate.setTextDomain("buttons");

function initTranslation()
{
	_TR("ovSelectLang", "overview select language");
	_TRV("ovLangApply", "button apply");

	_TR("ovStatus", "overview status");
	_TR("ovStatistic", "overview statistics");
	_TR("ovManagement", "overview management");
}

function initValue()
{
	var lang_element = document.getElementById("langSelection");
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_ru = "<% getLangBuilt("ru"); %>";

	initTranslation();
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

		if (s != -1) {
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
}

function setLanguage()
{
	document.cookie="language="+document.Lang.langSelection.value+"; path=/";
	parent.menu.location.reload();
	return true;
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1>Wive-NG-MT - firmware for Mediatek based Wi-Fi CPE productions by NAG</h1>
      <p id="ovIntroduction" />

      <!-- ----------------- Langauge Settings ----------------- -->

      <form method="post" name="Lang" action="/goform/setSysLang">
        <blockquote>
          <fieldset>
            <legend id="ovSelectLang">Select Language</legend>
            <select name="langSelection" id="langSelection">
              <!-- added by initValue -->
            </select>
            &nbsp;&nbsp;
	    <input type="hidden" name="submit-url" value="/overview.asp" >
            <input type="submit" style="{width:50px;}" value="Apply" id="ovLangApply" onClick="return setLanguage()">
          </fieldset>
        </blockquote>
      </form>
      <blockquote>
        <fieldset>
          <p> <a href="/adm/status.asp" id="ovStatus">Status</a><br />
            <a href="/adm/statistic.asp" id="ovStatistic">Statistic</a><br />
            <a href="/adm/management.asp" id="ovManagement">Management</a><br />
            <div id="ovAbout"><hr /><br />
            WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.
            <br />
	    Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
            <br /><br />
            News and changes in Russian:
            <a href="http://wive-ng.sf.net">wive-ng.sf.net - development opensource routers firmware</a><br />
            <br />Bug report please send support[at]nag.ru or sfstudio[at]mail.ru<br /></div>
          </p>
        </fieldset>
      </blockquote></td>
  </tr>
</table>
</body>
</html>
