var scanApHTTPrequest;
var scanApFreq;
var scanApCli = 0;
var scanApData;
var scanApColors = [
	'ff0000', '00ff00', '0000ff', 'ffff00', '00ffff', 'ff00ff',
	'c00000', '00c000', '0000c0', 'c0ff00', 'ffc000', 'c000ff',
	'ff00c0', '00c0ff', '00ffc0', 'c0c000', 'c000c0', '00c0c0',
	'800000', '008000', '000080', '808000', '800080', '008080',
	'80ff00', 'ff8000', '8000ff', 'ff8000', '0080ff', '00ff80',
	'80c000', 'c08000', '8000c0', 'c00080', '0080c0', '00c080'
];

function scanApHTTPmakeRequest(url, content, handler)
{
	scanApHTTPrequest = false;
	if (window.XMLHttpRequest) {
		scanApHTTPrequest = new XMLHttpRequest();
		if (scanApHTTPrequest.overrideMimeType) {
			scanApHTTPrequest.overrideMimeType('text/xml');
		}
	} else if (window.ActiveXObject) {
		try {
			scanApHTTPrequest = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
			scanApHTTPrequest = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {}
		}
	}
	if (!scanApHTTPrequest) {
		console.error('Giving up :( Cannot create an XMLHTTP instance');
		return false;
	}
	scanApHTTPrequest.onreadystatechange = handler;
	scanApHTTPrequest.open('POST', url, true);
	scanApHTTPrequest.send(content);
}

function convertHexToString(input) {

    // split input into groups of two
    var hex = input.match(/[\s\S]{2}/g) || [];
    var output = '';

    // build a hex-encoded representation of your string
    for (var i = 0, j = hex.length; i < j; i++) {
        output += '%' + ('0' + hex[i]).slice(-2);
    }

    // decode it using this trick
    output = decodeURIComponent(output);

    return output;
}

function scanApHTTPrequestHandler()
{
	if (scanApHTTPrequest.readyState == 4)
	{
		if (scanApHTTPrequest.status == 200)
		{
			scanApData = JSON.parse(scanApHTTPrequest.responseText);
			if (scanApFreq == "2.4") {
				var div = document.getElementById('scanApPlot');
				showElement('scanApPlot');
				showElement('scanApButtons');
				hideElement('scanApPreloader');
			}
			else {
				var div = document.getElementById('scanApPlotINIC');
				showElement('scanApPlotINIC');
				showElement('scanApButtonsINIC');
				hideElement('scanApPreloaderINIC');
			}
			div.removeChild(div.firstChild);
			var el = drawSVG(div, 'svg', { 'width': 785, 'height': 300 });
			showGraph(el, scanApFreq);
			showLegend();
		} else {
			console.error('There was a problem with the request.');
		}
	}
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

// Show Graph
function showGraph(svg, scanApFreq)
{
	var w 			= svg.getAttribute('width');
	var h			= svg.getAttribute('height');
	var pad			= [60, 10, 30, 50 ];			// Left, right, top, bottom
	var s_width		= 5;
	var r_power		= [0, 100, 10];					// Min, Max, Step
	var chan_sh		= [ 10, 10 ];					// Left, right

	if (scanApFreq == "2.4") {
		var z		= 10;
		var chans	= [ 2412, 2417, 2422, 2427, 2432, 2437, 2442, 2447, 2452, 2457, 2462, 2467, 2472, 2484 ];	// 2.4GHz channels freqency
	}
	else {
		var z		= 3;
		var chans	= [ [5160, 32], [5180, 36], [5200, 40], [5220, 44], [5240, 48], [5260, 52], [5280, 56], [5300, 60], [5320, 64],
						[5480, 96], [5500, 100], [5520, 104], [5540, 108], [5560, 112], [5580, 116], [5600, 120], [5620, 124], [5640, 128], [5660, 132], [5680, 136], [5700, 140], [5720, 144],
						[5725, 145], [5745, 149], [5765, 153], [5785, 157], [5805, 161], [5825, 165]
					  ];							// 5GHz channels freqency
	}
	// Background
	drawSVG(svg, 'rect',
		{
			'fill':				'#363636',
			'fill-opacity':		'1',
			'fill-rule':		'evenodd',
			'stroke':			'none',
			'x':				'0',
			'y':				'0',
			'width':			w,
			'height':			h
		}
	);

	// Show channels
	for (var chan = 0; chan < chans.length; chan++)
	{
		if (scanApFreq == "2.4") {
			var dist = chans[chans.length-1] - chans[0] + chan_sh[0] + chan_sh[1];
			var x = Math.round(pad[0] + (w - pad[0] - pad[1]) * ((chans[chan] - chans[0] + chan_sh[0])/dist));
		}
		else {
			var shift = (chans[chan][1] <= 64) ? 0 : (chans[chan][1] <= 144) ? 20 : 40;
			var x = Math.round(pad[0] + chan_sh[0] + shift + ((w - pad[0] - 40) / (chans.length + 2)) * (chan + 1));
		}

		drawSVG(svg, 'line',
			{
				'x1':				x,
				'y1':				pad[2],
				'x2':				x,
				'y2':				h - pad[3] + s_width + (chan % 2) * 15,
				'style':			'stroke:#c0c0c0;',
				'fill':				'none',
				'stroke-dasharray': '2,2'
			}
		);

		// Channel
		drawSVG(svg, 'text',
			{
				'fill':					'#FFFFFF',
				'x':					x,
				'y':					pad[2] - 5,
				'text-anchor':			'middle',
				'dominant-baseline':	'text-after-edge',
				'text-rendering':		'optimizeLegibility',
				'font-size':			'8pt',
				'font-family':			'Tahoma',
				'fill-rule':			'evenodd'
			},
			(scanApFreq == "2.4") ? chan + 1 : chans[chan][1]
		);

		// Frequence
		drawSVG(svg, 'text',
			{
				'fill':					'#FFFFFF',
				'x':					x,
				'y':					h - pad[3] + s_width + 5 + (chan % 2)*15,
				'dominant-baseline':	'text-before-edge',
				'text-anchor':			'middle',
				'text-rendering':		'optimizeLegibility',
				'font-size': 			'8pt',
				'font-family':			'Tahoma',
				'fill-rule': 			'evenodd'
			},
			(scanApFreq == "2.4") ? chans[chan] : chans[chan][0]
		);
	}

	// Draw horizontal grid
	for (var power = r_power[0]; power <= r_power[1]; power += r_power[2])
	{
		var dist	= r_power[0] + r_power[1];
		var red		= Math.round(255 - 255 * (power / dist));
		var green	= Math.round(255 - 255 * ((dist - power) / dist));
		var y		= Math.round(h - 20 - (pad[2] + (h - pad[2] - pad[3]) * (power / 110)));

		drawSVG(svg, 'line',
			{
				'x1': 					pad[0] - s_width,
				'y1': 					y,
				'x2': 					w - pad[1],
				'y2': 					y,
				'style': 				'stroke:rgb(' + red + ',' + green +',0);',
				'stroke-dasharray': 	'2,2'
			}
		);

		// Signal
		drawSVG(svg, 'text',
			{
				'fill':					'rgb(' + red + ', ' + green +', 0)',
				'x':					pad[0] - s_width - 5,
				'y':					y,
				'text-anchor':			'end',
				'dominant-baseline':	'central',
				'text-rendering':		'optimizeLegibility',
				'font-size':			'8pt',
				'font-family':			'Tahoma',
				'fill-rule':			'evenodd'
			},
			power
		);
	}

	// Draw stations
	for (var i = 0; i < scanApData.wireless.length; i++)
	{
		var dist	= chans[chans.length-1] - chans[0] + chan_sh[0] + chan_sh[1];
		var dx		= w - pad[0] - pad[1];
		var dy		= h - pad[2] - pad[3];
		var thick	= (scanApData.wireless[i].signal >= 66) ? 3 : (scanApData.wireless[i].signal > 33) ? 2 : 1;

		if (scanApFreq == "2.4") {
			switch (scanApData.wireless[i].extch) {
				case 'NONE':
					var x1 = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 1] - chans[0] + chan_sh[0] - 10)/dist));
					var x2 = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 1] - chans[0] + chan_sh[0] + 10)/dist));
					break;
				case 'ABOVE':
					var x1 = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 1] - chans[0] + chan_sh[0] - 10) / dist));
					var x2 = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel + 4 - 1] - chans[0] + chan_sh[0] + 10) / dist));
					break;
				case 'BELOW':
					var x1 = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 4 - 1] - chans[0] + chan_sh[0] - 10) / dist));
					var x2 = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 1] - chans[0] + chan_sh[0] + 10) / dist));
					break;
			}
		}
		else {
			for (var j = 0; j < chans.length; j++)
				if (chans[j][1] == scanApData.wireless[i].channel) {
					var shift = (chans[j][1] <= 64) ? 0 : (chans[j][1] <= 144) ? 20 : 40;
					switch (scanApData.wireless[i].extch) {
						case 'NONE':
							var x1 = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * j + (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							var x2 = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * (j + 2) - (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							break;
						case 'ABOVE':
							var x1 = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * j + ((w - pad[0] - 40) / (chans.length + 2)) * 0.5);
							var x2 = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * (j + 3) - (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							break;
						case 'BELOW':
							var x1 = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * (j - 1) + (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							var x2 = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * (j + 2) - (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							break;
					}
				}
		}
		var y		= Math.round(pad[2] + dy * ((110 - scanApData.wireless[i].signal) / (r_power[2] - r_power[0] + r_power[1])));

		var col		= scanApColors[i % scanApData.wireless.length];

		drawSVG(svg, 'line',
			{
				'x1':		x1 + z,
				'y1':		y,
				'x2':		x1,
				'y2':		h - pad[3],
				'style':	'stroke-width: ' + thick + '; stroke: #' + col + '; stroke-opacity: 0.65;'
			}
		);
		drawSVG(svg, 'line',
			{
				'x1':		x1 + z,
				'y1':		y,
				'x2':		x2 - z,
				'y2':		y,
				'style':	'stroke-width: ' + thick + '; stroke: #' + col + '; stroke-opacity: 0.65;'
			}
		);
		drawSVG(svg, 'line',
			{
				'x1':		x2 - z,
				'y1':		y,
				'x2':		x2,
				'y2':		h - pad[3],
				'style':	'stroke-width: ' + thick + '; stroke: #' + col + '; stroke-opacity: 0.65;'
			}
		);
	}

	// Draw text for stations
	for (var i = 0; i < scanApData.wireless.length; i++)
	{
		if (scanApFreq == "2.4") {
			switch (scanApData.wireless[i].extch) {
				case 'NONE':
					var x = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 1] - chans[0] + chan_sh[0]) / dist));
					break;
				case 'ABOVE':
					var x = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel + 2 - 1] - chans[0] + chan_sh[0]) / dist));
					break;
				case 'BELOW':
					var x = Math.round(pad[0] + dx * ((chans[+scanApData.wireless[i].channel - 2 - 1] - chans[0] + chan_sh[0]) / dist));
					break;
			}
		}
		else {
			for (var j = 0; j < chans.length; j++)
				if (chans[j][1] == scanApData.wireless[i].channel) {
					var shift = (chans[j][1] <= 64) ? 0 : (chans[j][1] <= 144) ? 20 : 40;
					switch (scanApData.wireless[i].extch) {
						case 'NONE':
							var x = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * (j + 1));
							break;
						case 'ABOVE':
							var x = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * (j + 2) - (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							break;
						case 'BELOW':
							var x = Math.round(pad[0] + chan_sh[0] + shift + (w - pad[0] - 40) / (chans.length + 2) * j + (w - pad[0] - 40) / (chans.length + 2) * 0.5);
							break;
					}
				}
		}
		var y = Math.round(pad[2] + dy * ((110 - scanApData.wireless[i].signal) / (r_power[2] - r_power[0] + r_power[1])));
		var col= scanApColors[i % scanApData.wireless.length];

		if (scanApData.wireless[i].ssid[0] == "0" && scanApData.wireless[i].ssid[1] == "x")
			scanApData.wireless[i].ssid = convertHexToString(scanApData.wireless[i].ssid.substr(2, scanApData.wireless[i].ssid.length - 2));

		drawSVG(svg, 'text',
			{
				'fill':					'#' + col,
				'x':					x,
				'y':					y - 5,
				'text-anchor':			'middle',
				'dominant-baseline':	'text-after-edge',
				'text-rendering':		'optimizeLegibility',
				'font-size':			'8pt',
				'font-family':			'Tahoma',
				'fill-rule':			'evenodd',
				'font-weight':			'bold'
			},
			scanApData.wireless[i].ssid
		);
	}

	// Vertical line
	drawSVG(svg, 'line', { 'x1': pad[0], 'y1': pad[2], 'x2': pad[0], 'y2': h - pad[3] + s_width, 'style': 'stroke: #ffffff; stroke-width: 2px;', 'fill': 'none' } );
	// Horizontal line
	drawSVG(svg, 'line', { 'x1': pad[0] - s_width, 'y1': h - pad[3], 'x2': w - pad[1], 'y2': h - pad[3], 'style': 'stroke: #ffffff; stroke-width: 2px;', 'fill': 'none' } );

	// Draw text
	drawSVG(svg, 'text',
		{
			'fill':					'rgb(' + red + ',' + green +',0)',
			'x':					0,
			'y':					0,
			'text-anchor':			'middle',
			'dominant-baseline':	'central',
			'transform':			'rotate(-90), translate(' + (-h/2) + ', 15)',
			'text-rendering':		'optimizeLegibility',
			'font-size':			'10pt',
			'font-family':			'Tahoma',
			'fill-rule':			'evenodd'
		},
		'Signal [%]'
	);
}

// Scan AP
function scanAp(f, cli)
{
	if (f == "apcli") {
		hideElement('scanAp');
		hideElement('scanApPlot');
		hideElement('scanApButtons');
		hideElement('scanApPreloader');
		hideElement('scanApINIC');
		hideElement('scanApPlotINIC');
		hideElement('scanApButtonsINIC');
		hideElement('scanApPreloaderINIC');
		
		if (document.getElementById('apcli_interface').selectedIndex == 0)
			scanApFreq = "2.4";
		else
			scanApFreq = "5";
		
		scanApCli	= 1;
	}
	else 	
		scanApFreq	= f;
	if (scanApFreq == "2.4") {
		showElement('scanAp');
		hideElement('scanApPlot');
		hideElement('scanApButtons');
		showElement('scanApPreloader');
		scanApHTTPmakeRequest("/goform/getScanAp", "0", scanApHTTPrequestHandler);
	}
	else {
		showElement('scanApINIC');
		hideElement('scanApPlotINIC');
		hideElement('scanApButtonsINIC');
		showElement('scanApPreloaderINIC');
		scanApHTTPmakeRequest("/goform/getScanAp", "1", scanApHTTPrequestHandler);
	}
}

// Show Graph legend
function showLegend()
{
	var i, j, k;
	var security;
	var html = "";

	if (scanApData.wireless.length > 0) {
		html += '<table width="100%">';
		html += '<tr>'
		if (scanApCli == 1) 
			html += '<th id="scanapLegendSelect">Select</th>'
		html += '<th id="scanapLegendColor" style="width: 3%"></th>'
		html += '<th id="scanapLegendChan" style="width: 10%">Channel</th>';
		html += '<th id="scanapLegendSSID">SSID</th>';
		html += '<th id="scanapLegendBSSID">BSSID</th>';
		html += '<th id="scanapLegendSecurity">Security</th>';
		html += '<th id="scanapLegendSignal" style="width: 7%">Signal</th>';
		html += '<th id="scanapLegend80211">802.11</th>';
		html += '<th id="scanapLegendType">Type</th>';
		html += '</tr>';
	
		for (i = 0; i < scanApData.wireless.length; i++) {
			if (scanApData.wireless[i].ssid[0] == "0" && scanApData.wireless[i].ssid[1] == "x")
				scanApData.wireless[i].ssid = convertHexToString(scanApData.wireless[i].ssid.substr(2, scanApData.wireless[i].ssid.length - 2));
			security = "";

			html += '<tr>';
			if (scanApCli == 1) {
				if (i == 0)
					var chk = "checked";
				else
					var chk = "";
				html += '<td style="text-align: center"><input type="radio" name="scanapSelectStation" value="' + i + '" ' + chk + '></td>';
			}
			html += '<td style="background-color: #' + scanApColors[i % scanApData.wireless.length] + '"></td>';
			html += '<td style="text-align: center">' + scanApData.wireless[i].channel;
			switch (scanApData.wireless[i].extch) {
				case 'NONE':	html += '</td>';													break;
				case 'ABOVE':	html += ' + ' + (+scanApData.wireless[i].channel + 4) + '</td>';	break;
				case 'BELOW':	html += ' + ' + (+scanApData.wireless[i].channel - 4) + '</td>'; 	break;
			}
			html += '</td>';
			html += '<td style="text-align: center">' + scanApData.wireless[i].ssid + '</td>';
			html += '<td style="text-align: center">' + scanApData.wireless[i].bssid.toUpperCase() + '</td>';
			if (scanApData.wireless[i].security.indexOf("WPAPSK") != -1 || 
				scanApData.wireless[i].security.indexOf("WPA1PSK") != -1)
					security = "WPA-PSK"
			else if (scanApData.wireless[i].security.indexOf("WPA2PSK") != -1)
					if (security.length > 0) 
						security += " / WPA2-PSK"
					else 
						security = "WPA2-PSK"
			else if (scanApData.wireless[i].security.indexOf("WPA1WPA2") != -1)
					security = "WPA / WPA2";
			else if (scanApData.wireless[i].security.indexOf("WPA2") != -1)
					security = "WPA2";
			else if (scanApData.wireless[i].security.indexOf("WPA") != -1)
					security = "WPA";
			else if (scanApData.wireless[i].security.indexOf("WEP") != -1)
					security = "WEP";
			else if (scanApData.wireless[i].security.indexOf("NONE") != -1) 
					security = "OPEN";

			html += '<td style="text-align: center">' + security + '</td>';
			html += '<td style="text-align: center">' + scanApData.wireless[i].signal + '%</td>';
			// replace("ABOVE", "").replace("BELOW", "").replace("NONE", "") - workaround
			html += '<td style="text-align: center">' + scanApData.wireless[i].wmode.replace("11", "").replace("ABOVE", "").replace("BELOW", "").replace("NONE", "").toUpperCase() + '</td>';
			html += '<td style="text-align: center">' + scanApData.wireless[i].nt + '</td>';
			html += '</tr>';
		}
	}
	else 
		html += '<table width="100%">';

	html += '<tr>';
	if (scanApCli == 1) {
		html += '<td colspan="9" align="center">';
		html +=	'<input id="scanapLegendButtonSelect" type="button" class="normal" value="Select" onClick="selectScanAp();">&nbsp;&nbsp;';
		html +=	'<input id="scanapLegendButtonRefresh" type="button" class="normal" value="Refresh" onClick="refreshScanAp(' + "'apcli'" + ');">&nbsp;&nbsp;';
		html += '<input id="scanapLegendButtonClose" type="button" class="normal" value="Close" onClick="closeScanAp(' + "'cli'" + ');">';
	}
	else if (scanApFreq == "2.4") {
		html += '<td colspan="8" align="center">';
		html +=	'<input id="scanapLegendButtonRefresh" type="button" class="normal" value="Refresh" onClick="refreshScanAp(' + "'2.4'" + ');">&nbsp;&nbsp;';
		html += '<input id="scanapLegendButtonClose" type="button" class="normal" value="Close" onClick="closeScanAp(' + "'2.4'" + ');">';
	}
	else {
		html += '<td colspan="8" align="center">';
		html +=	'<input id="scanapLegendButtonRefreshINIC" type="button" class="normal" value="Refresh" onClick="refreshScanAp(' + "'5'" + ');">&nbsp;&nbsp;';
		html += '<input id="scanapLegendButtonCloseINIC" type="button" class="normal" value="Close" onClick="closeScanAp(' + "'5'" + ');">';
	}
	html += '</td>';
	html += '</tr>';
	html += '</table>';
	if (scanApFreq == "2.4") {
		document.getElementById('scanApButtons_td').innerHTML = html;
		document.getElementById('scanApButtonsINIC_td').innerHTML = "";
	}
	else {
		document.getElementById('scanApButtonsINIC_td').innerHTML = html;
		document.getElementById('scanApButtons_td').innerHTML = "";
	}
	initTranslation();
}

// Refresh scan AP
function selectScanAp()
{
	hideElement('scanAp');
	hideElement('scanApPlot');
	hideElement('scanApButtons');
	hideElement('scanApPreloader');
	hideElement('scanApINIC');
	hideElement('scanApPlotINIC');
	hideElement('scanApButtonsINIC');
	hideElement('scanApPreloaderINIC');
	
	var i;
	var rad = document.getElementsByName('scanapSelectStation');
	for (i = 0; i < rad.length; i++) {
		if (rad[i].checked) {
			// Selecting SSID & BSSID
			document.getElementById('apcli_ssid').value = scanApData.wireless[i].ssid;
			document.getElementById('apcli_bssid').value = scanApData.wireless[i].bssid.toUpperCase();
			
			// Selecting security mode
			if (scanApData.wireless[i].security.indexOf("WPA2PSK") != -1)
				document.getElementById('apcli_mode').selectedIndex = 2;
			else if (scanApData.wireless[i].security.indexOf("WPAPSK") != -1 || scanApData.wireless[i].security.indexOf("WPA1PSK") != -1)
				document.getElementById('apcli_mode').selectedIndex = 1;
			else if (scanApData.wireless[i].security.indexOf("NONE") != -1)
				document.getElementById('apcli_mode').selectedIndex = 0;
			securityModeSwitch(document.wireless_apcli);
			
			if (scanApData.wireless[i].security.indexOf("TKIP") != -1)
				document.getElementById('apcliEncryptionType').selectedIndex = 0;
			else if (scanApData.wireless[i].security.indexOf("AES") != -1)
				document.getElementById('apcliEncryptionType').selectedIndex = 1;

			// Focusing on Password
			if (document.getElementById('apcli_mode').selectedIndex != 0) {
				document.getElementById('apcli_wpapsk').value = "";
				document.getElementById('apcli_wpapsk').focus();
			}
		}
	}
}

// Refresh scan AP
function refreshScanAp(f)
{
	if (f == "apcli") {
		hideElement('scanApPlot');
		hideElement('scanApButtons');
		hideElement('scanApPreloader');
		hideElement('scanApPlot');
		hideElement('scanApButtons');
		hideElement('scanApPreloader');
		scanAp("apcli", 1);
	}
	else {
		if (f == "2.4") {
			hideElement('scanApPlot');
			hideElement('scanApButtons');
			showElement('scanApPreloader');
		}
		else {
			hideElement('scanApPlotINIC');
			hideElement('scanApButtonsINIC');
			showElement('scanApPreloaderINIC');
		}
		scanAp(f);
	}
}

// Close Scan AP window
function closeScanAp(f)
{
	if (f == "2.4") {
		hideElement('scanAp');
		hideElement('scanApPlot');
		hideElement('scanApButtons');
		hideElement('scanApPreloader');
	}
	else {
		hideElement('scanApINIC');
		hideElement('scanApPlotINIC');
		hideElement('scanApButtonsINIC');
		hideElement('scanApPreloaderINIC');
	}
}
