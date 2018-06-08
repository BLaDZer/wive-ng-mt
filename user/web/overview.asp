<!DOCTYPE html>
<html>
	<head>
		<title>Wive-NG-MT</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_overview.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			function initTranslation() {
				_TR("ovSelectLang",		"overview select language");
				_TR("ovLangApply",		"button apply");
				_TR("ovStatus",			"overview status");
				_TR("ovStatistic",		"overview statistics");
				_TR("ovManagement",		"overview management");
			}

			function initValues() {
				var lang = document.getElementById("langSelection");

				initTranslation();
				if (BUILD_LangEN)
					lang.options[lang.length] = new Option('English', 'en');
				if (BUILD_LangRU)
					lang.options[lang.length] = new Option('Russian', 'ru');

				lang.value = NVRAM_Language;

				showWarning();
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1>Wive-NG-MT - opensource firmware to make your life better.</h1>
					<p id="ovIntroduction" />
					<!-- ----------------- Langauge Settings ----------------- -->
					<form method="post" name="Lang" action="/goform/setSysLang">
						<blockquote>
							<fieldset>
								<legend id="ovSelectLang">Select Language</legend>
								<select class="half" name="langSelection" id="langSelection">
									<!-- added by initValue -->
								</select>
								&nbsp;&nbsp;
								<input type="hidden" name="submit-url" value="/overview.asp" >
								<input type="submit" class="half" value="Apply" id="ovLangApply" onClick="setTimeout(function () { parent.menu.location.reload(); }, 500)">
							</fieldset>
						</blockquote>
					</form>
					<blockquote>
						<fieldset>
							<p>
								<a href="/adm/status.asp" id="ovStatus">Status</a><br />
								<a href="/adm/management.asp" id="ovManagement">Management</a><br />
								<div id="ovAbout"><hr /><br />
								WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.
								<br />
								Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
								<br /><br />
								News and changes in Russian:<br />
								<a href="https://wi-cat.ru" target="_blank">wi-cat.ru - Wireless Comprehensive Advanced Technology.</a><br />
								<a href="http://wive-ng.sf.net" target="_blank">wive-ng.ru - development opensource routers firmware.</a><br />
								<a href="http://shop.nag.ru" target="_blank">shop.nag.ru - SNR-CPE official shop.</a><br />
								<br />Bug report please send:<br />
								    1) for SNR-CPE to wifi@nag.ru<br />
								    2) report of Wive-NG software problem please to <a href="https://wi-cat.ru/forums" target="_blank">forum</a><br />
								</div>
							</p>
						</fieldset>
					</blockquote>
				</td>
			</tr>
		</table>
	</body>
</html>