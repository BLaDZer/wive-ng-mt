<!DOCTYPE html>
<html>
<head>
<title>Content Filter Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("firewall");

var filteringRules = [];

function genFilteringTable()
{
	var table = '<table class="small" style="width: 100%"><tr>' +
		'<th>Type</th>' +
		'<th>Value</th>' +
		'<th>Action</th></tr>';

	for (var i=0; i<filteringRules.length; i++)
	{
		var row = filteringRules[i];
		var type = 'Unknown';

		if (row[0] == 'url')
			type = 'Block URL';
		else if (row[0] == 'host')
			type = 'Block Host';

		table +=
			'<tr>' +
			'<td style="text-align: left;">' + type + '</td>' + // Type
			'<td style="text-align: left;">' + row[1] + '</td>' + // Protocol
			'<td style="text-align: center;"><a style="color: #ff0000;" title="Delete record" onclick="deleteRule(' + i + ');"><b>[x]</b></a></td>' +
			'</tr>';
	}

	if (filteringRules.length <= 0)
		table += '<tr><td colspan="3" style="text-align: left;">No content filtering rules yet</td></tr>';

	// Controls
	table +=
		'<tr>'+
		'<td style="text-align: left;"><select name="filterType" tabindex="1"><option value="url">Block URL</option><option value="host">Block Host</option></select></td>' +
		'<td style="text-align: left;"><input type="text" class="normal" name="filterValue" tabindex="2"></td>' +
		'<td style="text-align: center;"><input type="button" class="short" title="Add record" value="Add" tabindex="3" onclick="addRule(this.form);"></td>' +
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
		alert('Filtering value not specified');
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

	addAllRules(urls, 'url');
	addAllRules(hosts, 'host');

	for (var field in st)
		setElementChecked('websFilter_' + field, st[field] == '1');
	
	genFilteringTable();
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
	
	return true;
}

</script>
</head>

<!--     body      -->
<body onLoad="initValues();">
<table class="body">
  <tr>
    <td><h1 id="ContentFilterTitle">Content Filter Settings </h1>
      <% checkIfUnderBridgeModeASP(); %>
      <p id="ContentFilterIntrodution">Here you can setup Content Filter to restrict access to unwanted content.</p>
      <hr>
      <form action="/goform/webContentFilterSetup" method="POST" name="websContentFilterSetup" onSubmit="return submitForm(this);">
        
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
            <td class="title" colspan="2" id="WebsContentFilter">Web URL / Host blocking rules</td>
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
              <input type="submit" value="Apply"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
