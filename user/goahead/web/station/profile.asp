<!DOCTYPE html>
<html>
<head>
<title>Station Profile</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script language="JavaScript" type="text/javascript">

var profiles = [];
var bssid_list = [];
var profile_id = 0;

var auth_modes = { <% getStaAuthModes(); %> };
var encrypt_modes = { <% getStaEncryptModes(); %> };
var active_status = '<% getActiveProfileStatus(); %>';

var security = { // Auth mode :  [ Encryption modes ]
	'-': [ 1 ], // NONE
	'0': [ 1, 0 ], // OPEN: NONE, WEP
	'4': [ 6, 4 ], // WPA: AES, TKIP
	'7': [ 6, 4 ]  // WPA2: AES, TKIP
};

var colors = [
	'ff0000', '00ff00', '0000ff', 'ffff00', '00ffff', 'ff00ff',
	'c00000', '00c000', '0000c0', 'c0ff00', 'ffc000', 'c000ff',
	'ff00c0', '00c0ff', '00ffc0', 'c0c000', 'c000c0', '00c0c0',
	'800000', '008000', '000080', '808000', '800080', '008080',
	'80ff00', 'ff8000', '8000ff', 'ff8000', '0080ff', '00ff80',
	'80c000', 'c08000', '8000c0', 'c00080', '0080c0', '00c080'
];

function profileById(id)
{
	for (var i=0; i<profiles.length; i++)
		if (profiles[i]['html_id'] == id)
			return profiles[i];
	return null;
}

function parseProfiles(form)
{
	// Prepare hash map
	var map = [];

	for (var i=0; i<form.elements.length; i++)
	{
		var element = form.elements[i];
		var name = element.getAttribute('NAME');
		if ((name != null) && (name.indexOf('sta') == 0))
			map[name] = element.value.split(';');
	}

	// Create profiles
	var p_list = map['staProfile'];
	for (var i=0; i<p_list.length; i++)
	{
		if ((p_list[i] != null) && (p_list[i].length > 0))
		{
			var profile = [];
			profile['html_id'] = profile_id++;
			profile['detailed'] = false;
			for (var key in map)
			{
				var list = map[key];
				profile[key] = (i < list.length) ? list[i] : '';
			}

			profiles.push(profile);
		}
	}
}

function appendProfile(p)
{
	var tbody = document.getElementById('profileListTable');
	var p_id = p['html_id'];
	var row = null, n=null;
	var tr = function(a, h) {
		var e = createNode('TR', a, h);
		tbody.appendChild(e);
		if (p['trows'] == null)
			p['trows'] = [];
		p['trows'].push(e);
		return row = e;
	};
	var td = function(a, h) { var e = createNode('TD', a, h); row.appendChild(e); return e; };
	
	tr({ 'id': 'rowHead' + p_id });
		var style = {};
		if ((p['staActive'] == '1') && (active_status != ''))
			style = { 'style' : 'background-color: #' + ((active_status) == '1' ? '3da42c' : 'dd3b3b' )};
		
		td(style, '<input type="radio" id="staActive' + p_id + '" name="staActive" value="' + p_id + '">');
		td(n, '<span style="cursor: pointer; color: #0000ff;" id="staMore' + p_id + '" onclick="triggerStaDetails(' + p_id + ');"></span>&nbsp;' +
			'<span style="cursor: pointer; color: #ff0000;" onclick="deleteProfile(' + p_id + ');">[&nbsp;x&nbsp;]</span>');
		td(n, '<input id="staProfile' + p_id + '" type="text">');
		td(n, '<input id="staSSID' + p_id + '" type="text">');
		td(n, '<select id="staChannel' + p_id + '">' +
			'<option value="0" selected>Auto</option>' +
			'</select>');
		td(n, '<td><select id="staAuth' + p_id + '" onchange="staAuthChange(' + p_id + ')">' +
			'<option value="0" selected>OPEN</option>' +
			'<option value="4">WPA</option>' +
			'<option value="7">WPA2</option>' +
			'</select>');
		td(n, '<select id="staEncrypt' + p_id + '" onchange="staEncryptChange(' + p_id + ');">&nbsp;</select>');
//		td(n, '<select id="staNetworkType' + p_id + '">' +
//			'<option value="0" selected>802.11 Ad Hoc</option>' +
//			'<option value="1">Infrastructure</option>' +
//			'</select>');
		td(n, 'Infrastructure');
		
	// Always turned on row
	tr( { 'id': 'rowDetails' + p_id });
		td({ 'id': 'colStaMoreLeft' + p_id, 'style': 'border-right: 1px dashed #9BABBD;'}, '&nbsp;' );
		td({ 'id': 'colStaMoreRight' + p_id, 'style': 'border-left: 1px dashed #9BABBD;'}, '&nbsp;' );
		td({ 'colspan': 6, 'class': 'title' }, 'Detailed configuration:');
		
	// Power saving
	tr( { 'id': 'rowPowerSaving' + p_id });
		td( { 'class': 'head', 'colspan': 1 }, 'Power Saving Mode');
		td( { 'colspan': 5 }, '<select id="staPSMode' + p_id + '">' +
			'<option value="0">CAM (Constantly Awake Mode)</option>' +
			'<option value="1">Power Saving Mode</option>' +
			'</select>');

	// RTS Threshold
	tr( { 'id': 'rowRTS' + p_id });
		td( { 'class': 'head', 'colspan': 1 }, 'RTS Threshold');
		td( { 'colspan': 5 }, '<input id="staRTS' + p_id + '" class="half">');

	// Fragment Threshold
	tr( { 'id': 'rowFragment' + p_id });
		td( { 'class': 'head', 'colspan': 1 }, 'Fragment Threshold');
		td( { 'colspan': 5 }, '<input id="staFragment' + p_id + '" class="half">');

	// WEP configuration
	tr( { 'id': 'rowWepHeader' + p_id });
		td( { 'class': 'title', 'colspan': 6 }, 'Wire Equivalence Protection (WEP)');
	
	tr( { 'id': 'rowWepDescription' + p_id });
		td( { 'class': 'head1', 'rowspan': 5 }, 'WEP Keys');
		td( { 'class': 'head' }, 'Key Representation');
		td( { 'class': 'head', 'colspan': 2}, 'Key Length');
		td( { 'class': 'head'}, 'Key value');
		td( { 'class': 'head'}, 'Default key');
	
	for (var key=1; key<=4; key++)
	{
		tr( { 'id': 'rowWepKey' + key + p_id });
		td(n, '<select id="staKey' + key + 'Type' + p_id + '">' +
			'<option value="0" selected>Hexadecimal</option>' +
			'<option value="1">Ascii Text</option>' +
			'</select>');
		td({ 'colspan' : 2 }, '<select id="staKey' + key + 'Length' + p_id + '">' +
			'<option value="0" selected>64 bit (10 hex digits / 5 ascii characters)</option>' +
			'<option value="1">128 bit (26 hex digits / 13 ascii characters)</option>' +
			'</select>');
		td(n, '<input type="password" id="staKey' + key + p_id + '">');
		td(n, '<input type="radio" id="staKey' + key + 'DefaultId' + p_id + '" name="staKeyDefaultId' + p_id + '">&nbsp;default');
	}
	
	// WPA/WPA2 configuration
	// WPA/WPA2 configuration
	tr( { 'id': 'rowWpaHeader' + p_id });
		td( { 'class': 'title', 'colspan': 6 }, 'Wireless Protected Access (WPA1, WPA2)');
	tr( { 'id': 'rowWpaPsk' + p_id });
		td( { 'class': 'head', 'colspan': 1 }, 'WPA Pass Phrase');
		td( { 'colspan': 5 }, '<input type="password" id="staWpaPsk' + p_id + '">');
}

function setupEncryption(p)
{
	var p_id = p['html_id'];
	var ex = function(id) { return document.getElementById(id + p_id); };
	
	// Get auth & encryption controls
	var enc = ex('staEncrypt');
	var auth = ex('staAuth');

	// Fill-up encryption control
	enc.options.length = 0;
	var sec = security[auth.value];
	if (sec == null)
		sec = security['-'];
	for (var i=0; i<sec.length; i++)
		enc.options.add(new Option(encrypt_modes[sec[i]], sec[i]));
}

function fillTable(p)
{
	// Fill table body
	var p_id = p['html_id'];
	var ex = function(id) { return document.getElementById(id + p_id); };
	
	// Main rows
	var visibleRows = [];
	
	ex('staActive').checked = p['staActive'] == '1';
	ex('staProfile').value = p['staProfile'];
	ex('staSSID').value = p['staSSID'];
	ex('staChannel').value = p['staChannel'];
	ex('staAuth').value = p['staAuth'];
	
	setupEncryption(p);
	ex('staEncrypt').value = p['staEncrypt'];
	
	ex('staPSMode').value = p['staPSMode'];
	ex('staRTS').value = (p['staRTS']*1 > 0) ? p['staRTS'] : 2347;
	ex('staFragment').value = (p['staFragment']*1 > 0) ? p['staFragment'] : 2346;

	// WEP
	ex('staKey1DefaultId').checked = true;

	for (var key=1; key<=4; key++)
	{
		ex('staKey' + key + 'Type').value = p['staKey' + key + 'Type'];
		ex('staKey' + key + 'Length').value = p['staKey' + key + 'Length'];
		ex('staKey' + key).value = p['staKey' + key];
		if (!ex('staKey' + key + 'DefaultId').checked)
			ex('staKey' + key + 'DefaultId').checked = (p['staKeyDefaultId'] * 1) == key;
	}
	
	// WPA
	ex('staWpaPsk').value = p['staWpaPsk'];
}

function deleteProfile(id)
{
	var p = profileById(id);
	if (p == null)
		return;
	
	var ex = function(id) { return document.getElementById(id + p['html_id']); };
	var p_name = ex('staProfile').value;

	if (confirm('Do you really want to delete profile "' + p_name + '"?'))
	{
		for (var i=0; i<p['trows'].length; i++)
		{
			var node = p['trows'][i];
			if (node != null)
				node.parentNode.removeChild(node);
		}
		
		// Remove profile from list
		for (var i=0; i<profiles.length; i++)
			if (profiles[i]['html_id'] == id)
			{
				profiles.splice(i, 1);
				return;
			}
	}
}

function staAuthChange(id)
{
	var p = profileById(id);
	if (p == null)
		return;
	setupEncryption(p);
	showStaDetails(p);
}

function staEncryptChange(id)
{
	var p = profileById(id);
	if (p == null)
		return;
	showStaDetails(p);
}

function triggerStaDetails(id, show)
{
	var p = profileById(id);
	if (p == null)
		return;
	p['detailed'] = (show != null) ? show : (! p['detailed']);
	selectTab(0);
	showStaDetails(p);
}

function showStaDetails(p)
{
	var ei = function(xid) { return xid + p['html_id']; };
	var ex = function(id) { return document.getElementById(ei(id)); };
	var vl = function(id) { return document.getElementById(ei(id)).value; };
	var det  = p['detailed'];
	var rows = [];
	
	ex('staMore').innerHTML = '[&nbsp;' + (p['detailed'] ? '&minus;' : '+') + '&nbsp;]';
	
	rows[ei('rowDetails')] = det;
	rows[ei('rowPowerSaving')] = det;
	rows[ei('rowRTS')] = det;
	rows[ei('rowFragment')] = det;
	
	var wep_on = ((vl('staAuth') == '0') && (vl('staEncrypt') == '0'));
	var wpa_on = ((vl('staAuth') == '4') || (vl('staAuth') == '7'));
	
	rows[ei('rowWepHeader')] = det && wep_on;
	rows[ei('rowWepDescription')] = det && wep_on;
	for (var key=1; key<=4; key++)
		rows[ei('rowWepKey' + key)] = det && wep_on;

	rows[ei('rowWpaHeader')] = det && wpa_on;
	rows[ei('rowWpaPsk')] = det && wpa_on;

	// Encryption
//	ex('staEncrypt').innerHTML = encrypt_modes[enc_mode];

	// Show/hide elements
	var visible = 0;
	for (var key in rows)
	{
		displayElement(key, rows[key]);
		if (rows[key])
			visible++;
	}
	
	if (visible > 0)
	{
		ex('colStaMoreLeft').setAttribute('rowspan', visible);
		ex('colStaMoreRight').setAttribute('rowspan', visible);
	}
}

function PageInit()
{
	parseProfiles(document.sta_profile);

	// Generate table body
	for (var i=0; i<profiles.length; i++)
		appendProfile(profiles[i]);

	for (var i=0; i<profiles.length; i++)
		fillTable(profiles[i]);
	
	// Display all profiles
	for (var i=0; i<profiles.length; i++)
		showStaDetails(profiles[i]);
	
	// Draw empty graph
	genStaDiagram();
}

function createProfile()
{
	var p = [];
	
	p['html_id'] = profile_id++;
	p['detailed'] = true;
	p['staProfile'] = 'Profile ' + (profiles.length + 1);
	p['staSSID'] = '';
	p['staNetworkType'] = 1;
	p['staPSMode'] = 0;
	p['staAdhocMode'] = 0;
	p['staChannel'] = 0;
	p['staPramType'] = 0;
	p['staRTSCheck'] = 1;
	p['staFragmentCheck'] = 1;
	p['staRTS'] = 2347;
	p['staFragment'] = 2346;
	p['staAuth'] = 0;
	p['staEncrypt'] = 1;
	p['staKeyDefaultId'] = 1;
	p['staKey1Type'] = 0;
	p['staKey2Type'] = 0;
	p['staKey3Type'] = 0;
	p['staKey4Type'] = 0;
	p['staKey1Length'] = 0;
	p['staKey2Length'] = 0;
	p['staKey3Length'] = 0;
	p['staKey4Length'] = 0;
	p['staKey1'] = '';
	p['staKey2'] = '';
	p['staKey3'] = '';
	p['staKey4'] = '';
	p['staWpaPsk'] = '';
	p['sta8021xKeyMgmt'] = 3;
	p['sta8021xEAP'] = 7;
	p['sta8021xTunnel'] = 3;
	p['sta8021xIdentity'] = 0;
	p['sta8021xPassword'] = 0;
	p['sta8021xClientCert'] = 0;
	p['sta8021xPrivateKey'] = 0;
	p['sta8021xPrivateKeyPassword'] = 0;
	p['sta8021xCACert'] = 0;
	p['staActive'] = 0;
	
	return p;
}

function addProfile(p)
{
	if (p == null)
		p = createProfile();
	
	profiles.push(p);
	appendProfile(p);
	fillTable(p);
	showStaDetails(p);
	
	// Switch to profile tab
	selectTab(0);
}

function updateProfiles(form)
{
	for (var i=0; i<profiles.length; i++)
	{
		var p = profiles[i];
		var p_id = p['html_id'];
		var ei = function(xid) { return xid + p_id; };
		var ex = function(id) { return document.getElementById(ei(id)); };
		var vl = function(id) { return document.getElementById(ei(id)).value; };
		
		// Profile name
		var p_name = vl('staProfile');
		if (p_name.match(/;|^\s*$/))
		{
			alert('Profile name for profile #' + (i+1) + ' is not valid');
			ex('staProfile').focus();
			return false;
		}
		if (p_name.length > 32)
		{
			alert('Profile name for profile ' + p_name + ' is too long');
			ex('staProfile').focus();
			return false;
		}
		
		p['staProfile'] = p_name;
		
		// SSID
		var p_ssid = vl('staSSID');
		if (p_ssid.match(/;|^\s*$/))
		{
			alert('SSID for profile ' + p['staProfile'] + ' is not valid');
			ex('staSSID').focus();
			return false;
		}
		if (p_ssid.length > 24)
		{
			alert('SSID for profile ' + p['staProfile'] + ' is too long');
			ex('staSSID').focus();
			return false;
		}
		
		p['staSSID'] = p_ssid;
		
		p['staNetworkType'] = 1; // Network type is always infrastructure now
		p['staPSMode'] = vl('staPSMode');
		p['staAdhocMode'] = 0; // AdHoc mode is OPEN always
		p['staChannel'] = 0; // Channel is automatic
		p['staPreamType'] = 0; // Auto
		p['staRTSCheck'] = 1; // Always check RTS
		p['staFragmentCheck'] = 1; // Always check fragment
		
		// RTS threshold
		if (!vl('staRTS').match(/^\d{1,4}$/))
		{
			alert ('Invalit RTS threshold for profile ' + p['staProfile']);
			triggerStaDetails(p_id, true);
			ex('staRTS').focus();
			return false;
		}
		p['staRTS'] = vl('staRTS');
		
		// Fragment threshold
		if (!vl('staFragment').match(/^\d{1,4}$/))
		{
			alert ('Invalit Fragment threshold for profile ' + p['staProfile']);
			triggerStaDetails(p_id, true);
			ex('staFragment').focus();
			return false;
		}
		p['staFragment'] = vl('staFragment');
		
		var wep_on = ((vl('staAuth') == '0') && (vl('staEncrypt') == '0'));
		var wpa_on = ((vl('staAuth') == '4') || (vl('staAuth') == '7'));
		
		p['staAuth'] = vl('staAuth');
		p['staEncrypt'] = vl('staEncrypt');
		p['staKeyDefaultId'] = 1;
		
		// Keys
		for (var key=1; key<=4; key++)
		{
			var k_type = p['staKey' + key + 'Type'] = ex('staKey' + key + 'Type').value;
			var k_length = p['staKey' + key + 'Length'] = ex('staKey' + key + 'Length').value;
			var k = p['staKey' + key] = ex('staKey' + key).value;
			
			// Check key
			if ((wep_on) && (k.length != 0))
			{
				if (k_type == '0') // HEX
				{
					if ((k_length == '1') && (!k.match(/^[a-zA-Z0-9]{26}$/))) // 26
					{
						alert('Key must contain 26 HEX digits');
						triggerStaDetails(p_id, true);
						ex('staKey' + key).focus();
						return false;
					}
					else if ((k.length != 13) || (k.match(/;/)))
					{
						alert('Key must contain 13 ASCII characters except ";" symbol');
						triggerStaDetails(p_id, true);
						ex('staKey' + key).focus();
						return false;
					}
				}
				else // ASCII
				{
					if ((k_length == '1') && (!k.match(/^[a-zA-Z0-9]{10}$/))) // 10
					{
						alert('Key must contain 10 HEX digits');
						triggerStaDetails(p_id, true);
						ex('staKey' + key).focus();
						return false;
					}
					else if ((k.length != 5) || (k.match(/;/)))
					{
						alert('Key must contain 5 ASCII characters except ";" symbol');
						triggerStaDetails(p_id, true);
						ex('staKey' + key).focus();
						return false;
					}
				}
			}
			
			if (ex('staKey' + key + 'DefaultId').checked)
				p['staKeyDefaultId'] = key;
		}
		
		// WPA PSK
		var p_psk = vl('staWpaPsk');
		if (p_psk.match(/;/))
		{
			alert('Password phrase for profile ' + p['staProfile'] + ' can not contain ";" character');
			triggerStaDetails(p_id, true);
			ex('staWpaPsk').focus();
			return false;
		}
		if (p_psk.length > 64)
		{
			alert('Password phrase for profile ' + p['staProfile'] + ' is too long');
			triggerStaDetails(p_id, true);
			ex('staSSID').focus();
			return false;
		}
		
		p['staWpaPsk'] = p_psk;
		
		// Make some magix
		p['sta8021xKeyMgmt'] = 3;
		p['sta8021xEAP'] = 7;
		p['sta8021xTunnel'] = 3;
		p['sta8021xIdentity'] = 0;
		p['sta8021xPassword'] = 0;
		p['sta8021xClientCert'] = 0;
		p['sta8021xPrivateKey'] = 0;
		p['sta8021xPrivateKeyPassword'] = 0;
		p['sta8021xCACert'] = 0;
		p['staActive'] = (ex('staActive').checked) ? 1 : 0;
	}
	
	return true;
}

function submitForm(form)
{
	if (!updateProfiles(form))
		return false;
	
	// Serialize profiles
	var config = [];
	for (var i=0; i<profiles.length; i++)
	{
		var p = profiles[i];
		for (var key in p)
		{
			var l = config[key];
			if (!l)
				l = config[key] = [];
			l.push(p[key]);
		}
	}
	
	// Store config in from
	for (var key in config)
	{
		if (form.elements[key] != null)
			form.elements[key].value = config[key].join(';');
	}
	
	return true;
}

function selectTab(tab)
{
	for (var i=0; i<2; i++)
	{
		document.getElementById('profilesTab' + i).className = (i == tab) ? 'tab_sel' : 'tab';
		displayElement('profilesTabPage' + i, tab == i);
	}
}

function genStaTable()
{
	var html = '<table class="form" style="min-width: 600px; width: 600px;">';
	
	html += '<tr><td class="title" colspan="8" id="scanSiteSurvey">Site Survey</td></tr>'; // Header
	html += '<tr><th>Sel</th>' +
		'<th>SSID</th>' +
		'<th>BSSID</th>' +
		'<th>RSSI</th>' +
		'<th>Channel</th>' +
		'<th>Encryption</th>' +
		'<th>Authentication</th>' +
		'<th>Network Type</th></td>';
	
	for (var i=0; i<bssid_list.length; i++)
	{
		var row = bssid_list[i];
		var color = colors[i % colors.length];
		var style = (row[0] == 1) ? ' style="color: #ffffff; background-color: #3da42c;">' : '>'; // Category
		var sel = (row[0] == 1) ? ' checked' : '';

		html += '<tr' + style;
		html += '<td style="background-color: #' + color + '">' +
				'<input name="bssid_sel" type="radio"' + sel + ' value="' + (i+1) + '"></td>' + // rownum
			'<td' + style + row[1] + '</td>' + // SSID
			'<td' + style + row[2] + '</td>' + // BSSID
			'<td' + style + row[4] + '</td>' + // RSSI
			'<td' + style + row[5] + '</td>' + // Channel
			'<td' + style + row[6] + '</td>' + // Encryption
			'<td' + style + row[7] + '</td>' + // Authentication
			'<td' + style + row[8] + '</td>'; // Network Type
		
		html += '</tr>';
	}
	
	html += '</table>';
	
	setInnerHTML('ajxCtxStaTable', html);
}

function current_bssid()
{
	var form = document.sta_site_survey;
	for (var i=0; i<form.bssid_sel.length; i++)
		if (form.bssid_sel[i].checked)
			return bssid_list[i];
	return null;
}

function drawSVG(target, name, attributes, html)
{
	var item = document.createElementNS('http://www.w3.org/2000/svg', name);
	for (var key in attributes)
		item.setAttribute(key, attributes[key]);
	if (html != null)
	{
		if (name == 'text')
			item.textContent = html;
		else
			item.innerHTML = html;
	}
	if (target != null)
		target.appendChild(item);
	return item;
}

function showGraph(svg)
{
	var w = svg.getAttribute('width');
	var h = svg.getAttribute('height');
	var pad = [60, 10, 30, 50 ]; // Left, right, top, bottom
	var s_width = 5;
	var r_power = [-90, 0, 10]; // Min, Max, Step
	var chans = [ 2412, 2417, 2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462, 2467, 2472, 2484 ];
	var chan_sh = [ 10, 10 ]; // Left, right
	
	// Background
	drawSVG(svg, 'rect',
		{
			'fill': '#000000',
			'fill-opacity:': '1',
			'fill-rule': 'evenodd',
			'stroke': 'none',
			'x': '0',
			'y': '0',
			'width': w,
			'height': h
		}
	);
	
	// Show channels
	for (var chan = 0; chan<chans.length; chan++)
	{
		var dist = chans[chans.length-1] - chans[0] + chan_sh[0] + chan_sh[1];
		var x = Math.round(pad[0] + (w - pad[0] - pad[1]) * ((chans[chan] - chans[0] + chan_sh[0])/dist));
		drawSVG(svg, 'line',
			{
				'x1': x,
				'y1': pad[2],
				'x2': x,
				'y2': h - pad[3] + s_width + (chan % 2)*15,
				'style': 'stroke:#c0c0c0;',
				'fill': 'none',
				'stroke-dasharray': '2,2'
			}
		);
		
		// Channel
		drawSVG(svg, 'text',
			{
				'fill': "#ffffff",
				'x': x,
				'y': pad[2]-5,
				'text-anchor': 'middle',
				'dominant-baseline' : 'text-after-edge',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			chan + 1
		);
		
		// Frequence
		drawSVG(svg, 'text',
			{
				'fill': "#ffffff",
				'x': x,
				'y': h - pad[3] + s_width + 5 + (chan % 2)*15,
				'dominant-baseline' : 'text-before-edge',
				'text-anchor': 'middle',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			chans[chan]
		);
	}
	
	// Draw horizontal grid
	for (var power = r_power[0]; power <= r_power[1]; power += r_power[2])
	{
		var dist = r_power[0]-r_power[1];
		var red = Math.round(255*(power/dist));
		var green = Math.round(255*((dist - power)/dist));
		var y = Math.round(pad[2] + (h - pad[2] - pad[3]) * ((-power)/(r_power[2]-dist)));
		drawSVG(svg, 'line',
			{
				'x1': pad[0] - s_width,
				'y1': y,
				'x2': w - pad[1],
				'y2': y,
				'style': 'stroke:rgb(' + red + ',' + green +',0);',
				'stroke-dasharray': '2,2'
			}
		);
		
		// DBm
		drawSVG(svg, 'text',
			{
				'fill': 'rgb(' + red + ',' + green +',0)',
				'x': pad[0] - s_width - 5,
				'y': y,
				'text-anchor': 'end',
				'dominant-baseline' : 'central',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			power
		);
	}

	// Now draw stations
	for (var i=0; i<bssid_list.length; i++)
	{
		var sta = bssid_list[i];
		var dist = chans[chans.length-1] - chans[0] + chan_sh[0] + chan_sh[1];
		var dx = w - pad[0] - pad[1];
		var dy = h - pad[2] - pad[3];
		var x1 = Math.round(pad[0] + dx * ((chans[sta[5]-1] - chans[0] + chan_sh[0] - 10)/dist));
		var x2 = Math.round(pad[0] + dx * ((chans[sta[5]-1] - chans[0] + chan_sh[0] + 10)/dist));
		var y  = Math.round(pad[2] + dy * ((-sta[3])/(r_power[2]-r_power[0]+r_power[1])));
		var col= colors[i % bssid_list.length];
		
		drawSVG(svg, 'rect',
			{
				'fill': '#' + col,
				'stroke': '#' + col,
				'x': x1,
				'y': y,
				'width': x2-x1,
				'height': h - pad[3] - y,
				'stroke-dasharray': '2,2',
				'style': 'fill-opacity: 0.45; stroke-opacity: 1;'
			}
		);
	}
	
	// Now draw text for stations
	for (var i=0; i<bssid_list.length; i++)
	{
		var sta = bssid_list[i];
		var x = Math.round(pad[0] + dx * ((chans[sta[5]-1] - chans[0] + chan_sh[0])/dist));
		var y = Math.round(pad[2] + dy * ((-sta[3])/(r_power[2]-r_power[0]+r_power[1])));
		var col= colors[i % bssid_list.length];

		drawSVG(svg, 'text',
			{
				'fill': '#' + col,
				'x': x,
				'y': y-5,
				'text-anchor': 'middle',
				'dominant-baseline' : 'text-after-edge',
				'text-rendering': "optimizeLegibility",
				'font-size': "8pt", 'font-family': "Tahoma",
				'fill-rule': "evenodd"
			},
			sta[1]
		);
	}
	
	// Vertical line
	drawSVG(svg, 'line', { 'x1': pad[0], 'y1': pad[2], 'x2': pad[0], 'y2': h - pad[3] + s_width, 'style': 'stroke:#ffffff;stroke-width:2px;', 'fill': 'none' } );
	// Horizontal line
	drawSVG(svg, 'line', { 'x1': pad[0] - s_width, 'y1': h - pad[3], 'x2': w - pad[1], 'y2': h - pad[3], 'style': 'stroke:#ffffff;stroke-width:2px;', 'fill': 'none' } );
	
	// Draw text
	drawSVG(svg, 'text',
		{
			'fill': 'rgb(' + red + ',' + green +',0)',
			'x': 0,
			'y': 0,
			'text-anchor': 'middle',
			'dominant-baseline' : 'central',
			'transform' : 'rotate(-90),translate(' + (-h/2) + ',15)',
			'text-rendering': "optimizeLegibility",
			'font-size': "10pt", 'font-family': "Tahoma",
			'fill-rule': "evenodd"
		},
		'Amplitude [dB]'
	);
}

function performStaScan()
{
	var handler = function(response)
	{
		eval('bssid_list = [' + response + '];');
		genStaTable();
		genStaDiagram();
	};

	ajaxPerformRequest('/station/site_survey.asp', handler);
}

function genStaDiagram()
{
	var div = document.getElementById('ajxCtxStaGraph');
	while (div.hasChildNodes())
		div.removeChild(div.firstChild);
	var el = drawSVG(div, 'svg', { 'width': 600, 'height': 300});
	showGraph(el);
}

function createSurveyProfile()
{
	var bssid = current_bssid();
	if (bssid == null)
		return;
	
	var p = createProfile();
	var g_auth = bssid[7];
	var g_encry = bssid[6];
	
	// SSID
	p['staSSID'] = bssid[1];

	// Authorization
	if (bssid[9] == 1)
	{
		if (g_auth.indexOf("WPA2-PSK") >= 0)
			p['staAuth'] = 7;
		else if (g_auth.indexOf("WPA-PSK") >= 0)
			p['staAuth'] = 4;
	}

	// Encryption
	if (p['staAuth'] == '0') // OPEN
		p['staEncrypt'] = (g_encry.indexOf("WEP") >= 0) ? 0 : 1;
	else
		p['staEncrypt'] = (g_encry.indexOf("TKIP") >= 0) ? 4 : 6;
	
	// Add profile to list
	addProfile(p);
}

</script>
</head>

<body onLoad="PageInit();">
<table class="body">
  <tr>
    <td><h1 id="profTitle">Station Profile</h1>
      <p id="profIntroduction">The Status page shows the settings and current operation status of the Station.</p>
      <hr>
      <div id="staTabControl" class="tab_control">
        <div id="profilesTab0" class="tab_sel" onClick="selectTab(0);">Profiles</div>
        <div id="profilesTab1" class="tab" onClick="selectTab(1);">Site Survey</div>
      </div>
      
      <!-- Profiles -->
      
      <div id="profilesTabPage0" style="float:left;">
        <table class="form">
          <thead>
            <tr>
              <td class="title" colspan="8" id="profList">Profile List</td>
            </tr>
            <tr>
              <th style="width: 15px;"><input type="radio" name="staActive" checked>
                &nbsp;</th>
              <th>More</th>
              <th>Profile</th>
              <th>SSID</th>
              <th>Channel</th>
              <th>Authentication</th>
              <th>Encryption</th>
              <th>Network Type</th>
            </tr>
          </thead>
          <tbody id="profileListTable">
          </tbody>
        </table>
        <form method="POST" name="sta_profile" action="/goform/setStaProfile" onSubmit="return submitForm(this);">
          <table class="buttons">
            <tr>
              <td><input type="button" value="Add profile" onClick="addProfile();" >
                <input type="submit" value="Save &amp; Activate Profile" ></td>
            </tr>
          </table>
          <input type="hidden" name="staProfile" value="<% getCfgGeneralHTML(1, "staProfile"); %>" >
          <!-- Profile names -->
          <input type="hidden" name="staSSID" value="<% getCfgGeneralHTML(1, "staSSID"); %>" >
          <!-- SSID -->
          <input type="hidden" name="staNetworkType" value="<% getCfgGeneralHTML(1, "staNetworkType"); %>" >
          <!-- Network type -->
          <input type="hidden" name="staPSMode" value="<% getCfgGeneralHTML(1, "staPSMode"); %>" >
          <!-- PSMode -->
          <input type="hidden" name="staAdhocMode" value="<% getCfgGeneralHTML(1, "staAdhocMode"); %>" >
          <!-- AdHoc mode -->
          <input type="hidden" name="staChannel" value="<% getCfgGeneralHTML(1, "staChannel"); %>" >
          <!-- Channel -->
          <input type="hidden" name="staPreamType" value="<% getCfgGeneralHTML(1, "staPreamType"); %>" >
          <!-- PreamType -->
          <input type="hidden" name="staRTSCheck" value="<% getCfgGeneralHTML(1, "staRTSCheck"); %>" >
          <!-- RTS Check -->
          <input type="hidden" name="staFragmentCheck" value="<% getCfgGeneralHTML(1, "staFragmentCheck"); %>" >
          <!-- Fragment check -->
          <input type="hidden" name="staRTS" value="<% getCfgGeneralHTML(1, "staRTS"); %>" >
          <!-- RTS -->
          <input type="hidden" name="staFragment" value="<% getCfgGeneralHTML(1, "staFragment"); %>" >
          <!-- Fragment -->
          <input type="hidden" name="staAuth" value="<% getCfgGeneralHTML(1, "staAuth"); %>" >
          <!-- Authentication -->
          <input type="hidden" name="staEncrypt" value="<% getCfgGeneralHTML(1, "staEncrypt"); %>" >
          <!-- Encryption -->
          <input type="hidden" name="staKeyDefaultId" value="<% getCfgGeneralHTML(1, "staKeyDefaultId"); %>" >
          <!-- Key default ID -->
          <input type="hidden" name="staKey1Type" value="<% getCfgGeneralHTML(1, "staKey1Type"); %>" >
          <!-- Key 1 type -->
          <input type="hidden" name="staKey2Type" value="<% getCfgGeneralHTML(1, "staKey2Type"); %>" >
          <!-- Key 2 type -->
          <input type="hidden" name="staKey3Type" value="<% getCfgGeneralHTML(1, "staKey3Type"); %>" >
          <!-- Key 3 type -->
          <input type="hidden" name="staKey4Type" value="<% getCfgGeneralHTML(1, "staKey4Type"); %>" >
          <!-- Key 4 type -->
          <input type="hidden" name="staKey1Length" value="<% getCfgGeneralHTML(1, "staKey1Length"); %>" >
          <!-- Key 1 length -->
          <input type="hidden" name="staKey2Length" value="<% getCfgGeneralHTML(1, "staKey2Length"); %>" >
          <!-- Key 2 length -->
          <input type="hidden" name="staKey3Length" value="<% getCfgGeneralHTML(1, "staKey3Length"); %>" >
          <!-- Key 3 length -->
          <input type="hidden" name="staKey4Length" value="<% getCfgGeneralHTML(1, "staKey4Length"); %>" >
          <!-- Key 4 length -->
          <input type="hidden" name="staKey1" value="<% getCfgGeneralHTML(1, "staKey1"); %>" >
          <!-- Key 1 -->
          <input type="hidden" name="staKey2" value="<% getCfgGeneralHTML(1, "staKey2"); %>" >
          <!-- Key 2 -->
          <input type="hidden" name="staKey3" value="<% getCfgGeneralHTML(1, "staKey3"); %>" >
          <!-- Key 3 -->
          <input type="hidden" name="staKey4" value="<% getCfgGeneralHTML(1, "staKey4"); %>" >
          <!-- Key 4 -->
          <input type="hidden" name="staWpaPsk" value="<% getCfgGeneralHTML(1, "staWpaPsk"); %>" >
          <!-- WPA PSK -->
          <input type="hidden" name="sta8021xKeyMgmt" value="<% getCfgGeneralHTML(1, "sta8021xKeyMgmt"); %>" >
          <!-- 802.1 key management -->
          <input type="hidden" name="sta8021xEAP" value="<% getCfgGeneralHTML(1, "sta8021xEAP"); %>" >
          <!-- 802.1 EAP -->
          <input type="hidden" name="sta8021xTunnel" value="<% getCfgGeneralHTML(1, "sta8021xTunnel"); %>" >
          <!-- 802.1 tunnel -->
          <input type="hidden" name="sta8021xIdentity" value="<% getCfgGeneralHTML(1, "sta8021xIdentity"); %>" >
          <!-- 802.1 identity -->
          <input type="hidden" name="sta8021xPassword" value="<% getCfgGeneralHTML(1, "sta8021xPassword"); %>" >
          <!-- 802.1 password -->
          <input type="hidden" name="sta8021xClientCert" value="<% getCfgGeneralHTML(1, "sta8021xClientCert"); %>" >
          <!-- 802.1 Client cert path -->
          <input type="hidden" name="sta8021xPrivateKey" value="<% getCfgGeneralHTML(1, "sta8021xPrivateKey"); %>" >
          <!-- 802.1 Private key -->
          <input type="hidden" name="sta8021xPrivateKeyPassword" value="<% getCfgGeneralHTML(1, "sta8021xPrivateKeyPassword"); %>" >
          <!-- 802.1 Private key password -->
          <input type="hidden" name="sta8021xCACert" value="<% getCfgGeneralHTML(1, "sta8021xCACert"); %>" >
          <!-- 802.1 CA Cert Path -->
          <input type="hidden" name="staActive" value="<% getCfgGeneralHTML(1, "staActive"); %>" >
          <!-- STA active -->
        </form>
      </div>
      
      <!-- Site survey -->
      
      <div id="profilesTabPage1" style="float:left; display: none;">
        <form method="POST" name="sta_site_survey">
          <div id="ajxCtxStaTable"></div>
          <div id="ajxCtxStaGraph"></div>
          <table class="buttons">
            <tr>
              <td><input type="button" class="normal" value="Scan" id="scanRescan" onClick="performStaScan();">
                &nbsp;
                <input type="button" class="normal" name="addProfileButton" value="Create Profile" id="scanAddProfile" onClick="createSurveyProfile();"></td>
            </tr>
          </table>
        </form>
      </div>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
