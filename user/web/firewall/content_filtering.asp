<!DOCTYPE html>
<html>
	<head>
		<title>Content Filter Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_firewall.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			var filteringRules = [];

			function initTranslation()
			{
				_TR("ContentFilterTitle",		"content filter title");
				_TR("ContentFilterIntrodution", "content filter introduction");
				_TR("WebsContentFilter",		"content filter webs content filter");
				_TR("WebsContentFilterFilter",	"content filter webs content filter filter");
				_TR("websFilterProxy",			"content filter webs content filter proxy");
				_TR("websFilterJava",			"content filter webs content filter java");
				_TR("websFilterActivex",		"content filter webs content filter activex");
				_TR("websFilterCookies",		"content filter webs content filter cookies");
				_TR("websBlockingRules",		"content filter webs blocking rules");
				_TR("ContentFilterApply",		"button apply");
			}

			function initValues()
			{
				var st = {
					'proxy'   : NVRAM_websFilterProxy,
					'java'    : NVRAM_websFilterJava,
					'activex' : NVRAM_websFilterActivex,
					'cookies' : NVRAM_websFilterCookies
				};

				addAllRules(NVRAM_websURLFilters, 'url');
				addAllRules(NVRAM_websHostFilters, 'host');

				for (var field in st)
					document.getElementById('websFilter_' + field).checked = st[field] == '1';

				genFilteringTable();
				showWarning();
				initTranslation();
			}

			function checkValues(form) {
				var ents = { 'url' : [], 'host': [] };

				// Separate into 2 arrays
				for (var i = 0; i < filteringRules.length; i++)
					ents[filteringRules[i][0]].push(filteringRules[i][1]);

				form.urlFiltering.value  = ents['url'].join(';');
				form.hostFiltering.value = ents['host'].join(';');

				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}
			
			function genFilteringTable() {
				var table = '<table class="small" style="width: 100%"><tr>' +
					'<th>' + _("content filter table type") + '</th>' +
					'<th>' + _("content filter table value") + '</th>' +
					'<th>' + _("content filter table action") + '</th></tr>';

				for (var i = 0; i < filteringRules.length; i++) {
					var row  = filteringRules[i];
					var type = _("content filter unknown");

					if (row[0] == 'url')
						type = _("content filter block url");
					else if (row[0] == 'host')
						type = _("content filter block host");

					table +=
						'<tr>' +
						'<td style="text-align: left;">' + type + '</td>' + // Type
						'<td style="text-align: left;">' + row[1] + '</td>' + // Protocol
						'<td style="text-align: center;"><a style="color: #ff0000;" title="' + _("forward delete record") + '" onclick="deleteRule(' + i + ');"><img src="/graphics/cross.png" alt="[x]"></a></td>' +
						'</tr>';
				}

				if (filteringRules.length <= 0)
					table += '<tr><td colspan="3" style="text-align: left;">' + _("content filter no rules") + '</td></tr>';

				// Controls
				table +=
					'<tr>'+
					'<td style="text-align: left; width: 25%;"><select name="filterType" tabindex="1" class="normal"><option value="url">' + _("content filter block url") + '</option><option value="host">' + _("content filter block host") + '</option></select></td>' +
					'<td style="text-align: left; width: 50%;"><input type="text" style="width: 99%" name="filterValue" tabindex="2" size="100"></td>' +
					'<td style="text-align: center; width: 25%;"><input type="button" class="normal" title="' + _("forward add record") + '" value="' + _("button add") + '" tabindex="3" onclick="addRule(this.form);"></td>' +
					'</tr>';

				// Close manager
				table += '</table>';

				var elem = document.getElementById("websFilteringRules");
				if (elem!=null)
					elem.innerHTML = table;
			}

			function addRule(form) {
				if (form.filterType.value == "url") {
					if ((form.filterValue.value == "") || (form.filterValue.value.match(/[^A-ZА-Я0-9:\/._-]/gi))) {
						alert(_("content filter wrong url"));
						form.filterValue.focus();
						return;
					}
				}
				else if (form.filterType.value == "host") {
					if ((form.filterValue.value == "") || (form.filterValue.value.match(/[^A-ZА-Я0-9._-]/gi))) {
						alert(_("content filter wrong host"));
						form.filterValue.focus();
						return;
					}
				}
					
				var row = [ form.filterType.value, form.filterValue.value ];

				// Add a rule
				filteringRules.push(row);

				// Regenerate table
				genFilteringTable();
			}

			function deleteRule(index) {
				if ((index>=0) && (index < filteringRules.length)) {
					filteringRules.splice(index, 1);
					genFilteringTable();
				}
			}

			function addAllRules(list, type) {
				if (list.length > 0) {
					entries = list.split(';');
					for (var i=0; i<entries.length; i++)
						filteringRules.push( [ type, entries[i] ] );
				}
			}
		</script>
	</head>
	<body onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 id="ContentFilterTitle">Content Filter Settings </h1>
					<p id="ContentFilterIntrodution">Here you can setup Content Filter to restrict access to unwanted content.</p>
					<hr>
					<iframe id="timerReloader" name="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" action="/goform/webContentFilterSetup" name="websContentFilterSetup" onSubmit="return checkValues(this);">
						<table class="form">
							<tr>
								<td id="WebsContentFilter" class="title" colspan="2">Web Content Filter Setup</td>
							</tr>
							<tr>
								<td id="WebsContentFilterFilter" class="head"> Filter: </td>
								<td><input name="websFilterProxy"   id="websFilter_proxy"   type="checkbox"><span id="websFilterProxy">Proxy</span>
									<input name="websFilterJava"    id="websFilter_java"    type="checkbox"><span id="websFilterJava">Java</span>
									<input name="websFilterActivex" id="websFilter_activex" type="checkbox"><span id="websFilterActivex">ActiveX</span>
									<input name="websFilterCookies" id="websFilter_cookies" type="checkbox"><span id="websFilterCookies">Cookies</span></td>
							</tr>
							<tr>
								<td id="websBlockingRules" class="title" colspan="2">Web URL / Host blocking rules</td>
							</tr>
							<tr>
								<td id="websFilteringRules" colspan="2"></td>
							</tr>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="hidden" name="urlFiltering" />
									<input type="hidden" name="hostFiltering" />
									<input type="submit" class="normal" value="Apply" id="ContentFilterApply"></td>
							</tr>
						</table>
					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>
