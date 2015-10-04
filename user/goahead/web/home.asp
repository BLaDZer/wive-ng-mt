<!DOCTYPE html>
<html>
<!-- Copyright (c) Go Ahead Software Inc., 1994-2000. All Rights Reserved. -->
<!-- Copyright (c) Ralink (2004-2010) Tech Inc. All Rights Reserved. -->
<!-- Copyright (c) Wive-NG project (2010-2015) http://wive-ng.sf.net. All Rights Reserved. -->
<head>
<title>Wive-NG-MT router firmware - http://wive-ng.sf.net</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

function getCookie(name) {
  var cookie = " " + document.cookie;
  var search = " " + name + "=";
  var setStr = null;
  var offset = 0;
  var end = 0;

  if (cookie.length > 0) {
	offset = cookie.indexOf(search);
	if (offset != -1) {
    	    offset += search.length;
    	    end = cookie.indexOf(";", offset)
    	    if (end == -1)
    		end = cookie.length;
    	    setStr = unescape(cookie.substring(offset, end));
	}
  }
  return(setStr);
}

function initLanguage()
{
	var lang_en = "<% getLangBuilt("en"); %>";
	var lang_ru = "<% getLangBuilt("ru"); %>";
	var lang = "<% getCfgGeneral(1, "Language"); %>";
	var cook = "<% getCfgGeneral(1, "Language"); %>";

	if (lang == "")
		lang = "en";

	if (cook == "")
		cook = "en";

	if (document.cookie.length > 0)
		cook = getCookie("language");

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
	} else if (lang == "ru") {
		document.cookie="language=ru; path=/";
		if (cook != lang)
			window.location.reload();
		if (lang_ru != "1") {
			if (lang_en == "1") {
				document.cookie="language=en; path=/";
				window.location.reload();
			}
		}
	} else {
		document.cookie="language=en; path=/";
		cook = "en";
		lang = "en"
		lang_en = "1";
		lang_ru = "0";
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
