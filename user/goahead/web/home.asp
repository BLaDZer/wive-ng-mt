<!DOCTYPE html>
<html>
<!-- Copyright (c) Go Ahead Software Inc., 1994-2000. All Rights Reserved. -->
<!-- Copyright (c) Ralink (2004-2010) Tech Inc. All Rights Reserved. -->
<!-- Copyright (c) Wive-NG project (2010-2014) http://wive-ng.sf.net. All Rights Reserved. -->
<head>
<title>Wive-NG-MT router firmware - http://wive-ng.sf.net</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">
function initLanguage()
{

	var lang = "<% getCfgGeneral(1, "Language"); %>";
	if (lang=="")
		lang = "en";
	var cook = "en";
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_ru = "<% getLangBuilt("ru"); %>";

	if (document.cookie.length > 0) {
		var s = document.cookie.indexOf("language=");
		var e = document.cookie.indexOf(";", s);
		if (s != -1) {
			if (e == -1)
				cook = document.cookie.substring(s+9);
			else
				cook = document.cookie.substring(s+9, e);
		}
	}

	if (lang == "en") {
		document.cookie="language=en; path=/";
		if (cook != lang)
			window.location.reload();
		if (lang_en != "1") {
			if (lang_ru == "1") {
				document.cookie="language=ru; path=/";
				window.location.reload();
			}
		}
	}
	else if (lang == "ru") {
		document.cookie="language=ru; path=/";
		if (cook != lang)
			window.location.reload();
		if (lang_ru != "1") {
			if (lang_en == "1") {
				document.cookie="language=en; path=/";
				window.location.reload();
			}
		}
	}
	else {
		document.cookie="language=en; path=/";
		lang_en == "1";
		window.location.reload();
	}
}

function onInit()
{
	initLanguage();
}

</script>
</head>
<frameset id="homeFrameset" rows="70,1*" cols="*" border="0" framespacing="0" frameborder="NO" onLoad="onInit()">
  <frame src="title.htm" name="title" frameborder="NO" scrolling="NO" marginwidth="0" marginheight="0">
  <frameset id="homeMenuFrameset" cols="200,1*">
    <frame src="treeapp.asp" name="menu" marginwidth="0" marginheight="0" scrolling="AUTO" frameborder="NO" >
    <frame src="overview.asp" name="view" scrolling="AUTO" marginwidth="0" topmargin="0" marginheight="0" frameborder="NO">
  </frameset>
</frameset>
<noframes>
<body bgcolor="#FFFFFF">
</body>
</noframes>
</html>
