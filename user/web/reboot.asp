<!DOCTYPE html>
<html>
	<head>
		<title>Wive-NG-MT</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_tree.js"></script>
		<script type="text/javascript" src="/js/ajax.js"></script>
		<script>
			function initValues() {
				if (!ajaxPostForm(_('treeapp reboot confirm'), document.rebootForm, 'rebootReloader', _("message config"), ajaxShowProgress))
					top.view.location = "overview.asp";
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<table class="body">
			<form name="rebootForm" style="display: none;" method="GET" action="/goform/reboot">
				<iframe id="rebootReloader" name="rebootReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
			</form>
		</table>
	</body>
</html>
