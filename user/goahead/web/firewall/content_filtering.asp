<!DOCTYPE html>
<html>
<head>
<title>Content Filter Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("firewall");
Butterlate.setTextDomain("buttons");

var filteringRules = [];

function genFilteringTable()
{
	var table = '<table class="small" style="width: 100%"><tr>' +
		'<th>' + _("content filter table type") + '</th>' +
		'<th>' + _("content filter table value") + '</th>' +
		'<th>' + _("content filter table action") + '</th></tr>';

	for (var i=0; i<filteringRules.length; i++)
	{
		var row = filteringRules[i];
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
		'<td style="text-align: left;"><select name="filterType" tabindex="1"><option value="url">' + _("content filter block url") + '</option><option value="host">' + _("content filter block host") + '</option></select></td>' +
		'<td style="text-align: left;"><input type="text" class="normal" name="filterValue" tabindex="2"></td>' +
		'<td style="text-align: center;"><input type="button" class="mid" title="' + _("forward add record") + '" value="' + _("button add") + '" tabindex="3" onclick="addRule(this.form);"></td>' +
		'</tr>';

	// Close manager
	table += '</table>';

	var elem = document.getElementById("filteringRules");
	if (elem!=null)
		elem.innerHTML = table;
}

function addRule(form)
{
	if (form.filterValue.value.match(/^\s*$/))
	{
		alert(_("content filter not def"));
		form.filterValue.focus();
		return;
	}

	var row = [ form.filterType.value, form.filterValue.value ];

	// Add a rule
	filteringRules.push(row);

	// Regenerate table
	genFilteringTable();
}

function deleteRule(index)
{
	if ((index>=0) && (index < filteringRules.length))
	{
		filteringRules.splice(index, 1);
		genFilteringTable();
	}
}

function initValues()
{
	var st = {
		'proxy'   : '<% getCfgZero(1, "websFilterProxy"); %>',
		'java'    : '<% getCfgZero(1, "websFilterJava"); %>',
		'activex' : '<% getCfgZero(1, "websFilterActivex"); %>',
		'cookies' : '<% getCfgZero(1, "websFilterCookies"); %>'
	};

	var urls = '<% getCfgGeneral(1, "websURLFilters"); %>';
	var hosts = '<% getCfgGeneral(1, "websHostFilters"); %>';
	var opmode = '<% getCfgZero(1, "OperationMode"); %>';

	displayElement('bridge_warning', opmode == '0'); // bridge mode

	addAllRules(urls, 'url');
	addAllRules(hosts, 'host');

	for (var field in st)
		setElementChecked('websFilter_' + field, st[field] == '1');

	genFilteringTable();
	initTranslation();
	showWarning();
}

function addAllRules(list, type)
{
	if (list.length > 0)
	{
		entries = list.split(';');
		for (var i=0; i<entries.length; i++)
		{
			if (!entries[i].match(/^\s*$/))
				filteringRules.push( [ type, entries[i] ] );
		}
	}
}

function submitForm(form)
{
	var ents = { 'url' : [], 'host': [] };

	// Separate into 2 arrays
	for (var i=0; i<filteringRules.length; i++)
		ents[filteringRules[i][0]].push(filteringRules[i][1]);

	form.urlFiltering.value  = ents['url'].join(';');
	form.hostFiltering.value = ents['host'].join(';');

	ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
	return true;
}

function initTranslation()
{
	_TR("ContentFilterTitle", "content filter title");
	_TR("ContentFilterIntrodution", "content filter introduction");
	_TR("WebsContentFilter", "content filter webs content filter");
	_TR("WebsContentFilterFilter", "content filter webs content filter filter");
  	_TR("websFilter_proxy", "content filter webs content filter proxy");
  	_TR("websFilter_java", "content filter webs content filter java");
  	_TR("websFilter_activex", "content filter webs content filter activex");
  	_TR("WebsBlockingRules", "content filter webs blocking rules");
  	_TR("bridge_warning", "firewall bridge warning");

	_TRV("ContentFilterApply", "button apply");
}

</script>
</head>
<body onLoad="initValues();">
<table class="body">
  <tr id="warning"><tr>
  <tr>
    <td><h1 id="ContentFilterTitle">Content Filter Settings </h1>
      <div style="display:none;" id="bridge_warning">
      	<p><b>Warning:</b> The current operation mode is "Bridge mode" and these settings may not be functional.</p>
      </div>
      <p id="ContentFilterIntrodution">Here you can setup Content Filter to restrict access to unwanted content.</p>
      <hr>
      <form action="/goform/webContentFilterSetup" method="POST" name="websContentFilterSetup" onSubmit="return submitForm(this);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <!-- New content filtering -->
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="WebsContentFilter">Web Content Filter Setup</td>
          </tr>
          <tr>
            <td class="head" id="WebsContentFilterFilter"> Filter: </td>
            <td><input type="checkbox" name="websFilterProxy" id="websFilter_proxy">
              &nbsp;Proxy&nbsp;
              <input type="checkbox" name="websFilterJava" id="websFilter_java">
              &nbsp;Java&nbsp;
              <input type="checkbox" name="websFilterActivex" id="websFilter_activex">
              &nbsp;ActiveX </td>
          </tr>
          <tr>
            <td class="title" colspan="2" id="WebsBlockingRules">Web URL / Host blocking rules</td>
          </tr>
          <tr>
            <td colspan="2" id="filteringRules"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="urlFiltering" />
              <input type="hidden" name="hostFiltering" />
              <input type="hidden" name="submit-url" value="/firewall/content_filtering.asp" />
              <input type="submit" value="Apply" id="ContentFilterApply"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
