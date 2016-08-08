<!DOCTYPE html>
<html>
	<head>
		<title>Station List</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">

		<!--[if IE]><script src="/js/excanvas.min.js"></script><![endif]-->
		<script src="/js/jquery.min.js"></script>
		<script src="/js/jquery.flot.min.js"></script>
		<script src="/js/jquery.flot.time.min.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/lang/b28n.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("wireless");
			Butterlate.setTextDomain("buttons");

			var updateTime		= 5000;
			var wirelessMode	= "Basic";
			var wirelessTabeWidth	= "800px";
			var wirelessTableColumn = 9;
			var wirelessAvgRxLast	= 0;
			var wirelessAvgTxLast	= 0;
			var wirelessAvgRxLast24	= 0;
			var wirelessAvgTxLast24	= 0;
			var wirelessAvgRxLast5	= 0;
			var wirelessAvgTxLast5	= 0;

			var lastRxTxCount	= [];		// Last RX/TX count for all clients
			var lastAllRxTx		= [];		// Summary of last RX/TX count 
			var plotData 		= [];		// Statistic of all clients
			var plotMACs 		= [];		// List MACs for Ploting
			var plotMACsAll		= 0;		// Plot All MAC's

			function initTranslation()
			{
				_TR("stalistTitle",					"stalist title");
				_TR("stalistIntroduction",			"stalist introduction");
				_TR("stalistWirelessSummary",		"stalist wireless summary");
				_TR("stalistWirelessStations",		"stalist wireless network");
				_TR("stalistWirelessPlot",			"stalist wireless plot");
				_TR("stalistMacAddr",				"stalist mac address");
				_TR("stalistConnTime",				"stalist conn time");
				_TR("stalistMODE",					"stalist mode");
				_TR("stalistTxRate",				"stalist txrate");
				_TR("stalistRSSI",					"stalist rssi");
				_TR("stalistQuality",				"stalist quality");
				_TR("stalistRxTxCount",				"stalist rxtx");
				_TR("stalistPlot",					"stalist plot");
				_TR("stalistAction",				"stalist action");
				_TR("stalistWirelessNameSum",		"stalist wireless sum total");
				_TR("stalistWirelessNameSum24",		"stalist wireless sum total 24");
				_TR("stalistWirelessNameSum5",		"stalist wireless sum total 5");
				_TR("stalistWirelessSumAID",		"stalist wireless sum aid");
				_TR("stalistWirelessSumTXRate", 	"stalist wireless sum txrate");
				_TR("stalistWirelessSumTxRx",		"stalist wireless sum rxtx");
				_TR("stalistWirelessSumRSSI",		"stalist wireless sum rssi");
				_TR("stalistWirelessSumQuality",	"stalist wireless sum quality");
				_TR("stalistWirelessMode",			"stalist wireless mode");
				_TR("stalistWirelessModeBasic",		"stalist wireless mode basic");
				_TR("stalistWirelessModeAdvanced",	"stalist wireless mode advanced");
				_TR("wirelessPlotTypeName",					"stalist wireless plot type");
				_TR("wirelessPlotTimeName",					"stalist wireless plot time");
				_TR("time1M",						"stalist wireless plot time 1m");
				_TR("time2M",						"stalist wireless plot time 2m");
				_TR("time3M",						"stalist wireless plot time 3m");
				_TR("time4M",						"stalist wireless plot time 4m");
				_TR("time5M",						"stalist wireless plot time 5m");
				_TR("time10M",						"stalist wireless plot time 10m");
				_TR("time15M",						"stalist wireless plot time 15m");
				_TR("time20M",						"stalist wireless plot time 20m");
				_TR("time30M",						"stalist wireless plot time 30m");
				_TR("time1H",						"stalist wireless plot time 1h");
				_TR("time3H",						"stalist wireless plot time 3h");
				_TR("time6H",						"stalist wireless plot time 6h");
				_TR("timeAll",						"stalist wireless plot time all");
				_TR("typeTxRate",					"stalist wireless plot type txrate");
				_TR("typeRSSI",						"stalist wireless plot type rssi");
				_TR("typeQuality",					"stalist wireless plot type quality");
				_TR("typeRxTx",						"stalist wireless plot type rxtx");
				_TR("typeRxTxSum",						"stalist wireless plot type rxtx sum");
				_TR("typeTx",						"stalist wireless plot type tx");
				_TR("typeRx",						"stalist wireless plot type rx");
				_TRV("disconnectAll",				"button disconnect all");
				var elements = document.getElementsByTagName('input');
				for (var i = 0; i < elements.length; i++)
					if(elements[i].id == "disconnect")
						elements[i].value = _("button disconnect");
			}

			function initValues()
			{
				var is5gh_support	= '<% is5gh_support(); %>';
				var radio_on		= "<% getCfgZero(1, "RadioOn"); %>";
				var radio_on_ac		= "<% getCfgZero(1, "RadioOnINIC"); %>";				
				
				if ((radio_on == 0) && (radio_on_ac == 0)) {
					hideElement('tableWirelessSummary');
				}

				if (radio_on == 0) {
					hideElement('stalistWirelessNameSum24');
					hideElement('stalistWirelessSumAIDData24');
					hideElement('stalistWirelessSumTXRateData24');
					hideElement('stalistWirelessSumTxRxData24');
					hideElement('stalistWirelessSumRSSIData24');
					hideElement('stalistWirelessSumQualityData24');
					document.getElementById('stalistWirelessNameSum').style.width = "30%";
					document.getElementById('stalistWirelessNameSum5').style.width = "30%";
				}
				
				if (radio_on_ac == 0) {
					hideElement('stalistWirelessNameSum5');
					hideElement('stalistWirelessSumAIDData5');
					hideElement('stalistWirelessSumTXRateData5');
					hideElement('stalistWirelessSumTxRxData5');
					hideElement('stalistWirelessSumRSSIData5');
					hideElement('stalistWirelessSumQualityData5');
					document.getElementById('stalistWirelessNameSum').style.width = "30%";
					document.getElementById('stalistWirelessNameSum24').style.width = "30%";
				}

				if (is5gh_support == 0) {
					hideElement('stalistWirelessNameSum5');
					hideElement('stalistWirelessSumAIDData5');
					hideElement('stalistWirelessSumTXRateData5');
					hideElement('stalistWirelessSumTxRxData5');
					hideElement('stalistWirelessSumRSSIData5');
					hideElement('stalistWirelessSumQualityData5');				
					hideElement('stalistWirelessNameSum24');
					hideElement('stalistWirelessSumAIDData24');
					hideElement('stalistWirelessSumTXRateData24');
					hideElement('stalistWirelessSumTxRxData24');
					hideElement('stalistWirelessSumRSSIData24');
					hideElement('stalistWirelessSumQualityData24');
					document.getElementById('stalistWirelessNameSum').style.width = "60%";
				}

				if (getCookie('wirelessMode') !== undefined) {
					wirelessMode = getCookie('wirelessMode');
					if (wirelessMode == "Basic") {
						wirelessTabeWidth		= "800px";
						wirelessTableColumn		= 9;
						document.getElementById('tableWirelessSummary').style.width = "800px";
						document.getElementById('tableWirelessPlot').style.width = "800px";
					}
					else {
						wirelessTabeWidth	= "1100px";
						wirelessTableColumn	= 17;
						document.getElementById('tableWirelessSummary').style.width = "1100px";
						document.getElementById('tableWirelessPlot').style.width = "1100px";
					}					
				}
				if (getCookie('plotMACs') !== undefined) {
					plotMACs = JSON.parse(getCookie('plotMACs'));
				}
				if (getCookie('plotMACsAll') !== undefined) {
					plotMACsAll = getCookie('plotMACsAll');
				}
				
				ajaxLoadElement("stationListData", "/wireless/stainfo_clients.asp", startShowStationList);
				showWarning();
			}

			function setCookie(name, value, options) {
				options = options || {};

				var expires = options.expires;

				if (typeof expires == "number" && expires) {
					var d = new Date();
					d.setTime(d.getTime() + expires * 1000);
					expires = options.expires = d;
				}
				if (expires && expires.toUTCString) {
					options.expires = expires.toUTCString();
				}

				value = encodeURIComponent(value);

				var updatedCookie = name + "=" + value;

				for (var propName in options) {
					updatedCookie += "; " + propName;
					var propValue = options[propName];
					if (propValue !== true) {
						updatedCookie += "=" + propValue;
					}
				}

				document.cookie = updatedCookie;
			}			
			
			function getCookie(name) {
				var matches = document.cookie.match(new RegExp(
					"(?:^|; )" + name.replace(/([\.$?*|{}\(\)\[\]\\\/\+^])/g, '\\$1') + "=([^;]*)"
				));
				return matches ? decodeURIComponent(matches[1]) : undefined;
			}
			
			function startShowStationList() {
				getStationsCounters();
				showStationList();
				setInterval('ajaxLoadElement("stationListData", "/wireless/stainfo_clients.asp", getStationsCounters);', 5000);
				setInterval('showStationList();', 5000);
			}
			
			function wirelessModeChange()
			{
				if (document.sta.stalistWirelessModeSelect.selectedIndex == 0) {
					wirelessMode			= "Basic";
					wirelessTabeWidth		= "800px";
					wirelessTableColumn		= 9;
					document.getElementById('tableWirelessSummary').style.width = "800px";
					document.getElementById('tableWirelessPlot').style.width = "800px";
				}
				else {
					wirelessMode 		= "Advanced";
					wirelessTabeWidth	= "1100px";
					wirelessTableColumn	= 17;
					document.getElementById('tableWirelessSummary').style.width = "1100px";
					document.getElementById('tableWirelessPlot').style.width = "1100px";
				}
				setCookie('wirelessMode', wirelessMode);
				showStationList();
			}

			function disconnectStation(form, mac)
			{
				form.disconnectSta.value = mac;
				if (mac == "*") {
					lastRxTxCount = [];
					plotData = [];
					plotMACs = [];
				} else if (lastRxTxCount.indexOf(mac.replace(/:/g, '')) != "-1") {
					lastRxTxCount.splice(lastRxTxCount.indexOf(mac.replace(/:/g, '')), 3);
					if (plotMACs.indexOf(mac.replace(/:/g, '')) != "-1")
						plotMACs.splice(plotMACs.indexOf(mac.replace(/:/g, '')), 1);
					for (var j = 0; j < plotData.length; j++) {
						if (plotData[j][0] == mac.replace(/:/g, '')) {
							plotData[j][5] = null;
							plotData[j][6] = null;
						}
					}
				}
				setCookie('plotMACs', JSON.stringify(plotMACs));

				var m_method	= $(form).attr('method');
				var m_action	= $(form).attr('action');
				var m_data		= $(form).serialize();
				$.ajax({
					type:		m_method,
					url:		m_action,
					data:		m_data,
					success: 	function(data) {$('#results').html(data); },
					error:  	function(xhr, str){	alert('Error: ' + xhr.responseCode); }
				});
				ajaxLoadElement("stationListData", "/wireless/stainfo_clients.asp", getStationsCounters);
				ajaxLoadElement("stationListData", "/wireless/stainfo_clients.asp", showStationList);
			}

			function addremoveplotMACs(mac_id)
			{
				if (plotMACsAll == 1)
					plotMACsAll = 0;
				if (document.getElementById('checkbox_' + mac_id).checked == false) {
					if (plotMACs.indexOf(mac_id) != "-1") {
						plotMACs.splice(plotMACs.indexOf(mac_id), 1);
					}
				}
				else {
					plotMACs.push(mac_id);
				}
				setCookie('plotMACs', JSON.stringify(plotMACs));				
				setCookie('plotMACsAll', plotMACsAll);
				showStationList();
			}

			function addremoveplotMACsAll()
			{
				if (plotMACsAll == 0)
					plotMACsAll = 1;
				else {
					plotMACsAll = 0;
					plotMACs = [];
				}
				setCookie('plotMACsAll', plotMACsAll);
				showStationList();
			}
			
			function showStationList()
			{
				var i, mac_id, checked, rx, tx, bytes;
				var data	                = JSON.parse(document.getElementById('stationListData').innerHTML);
				var html	                = "";
				var time	                = new Date().getTime();
				var MACs                	= plotMACs.slice(0);
				var plotAll					= "";
				var quality					= 0;
				var rssi					= 0;

				var wirelessAIDs	    	= 0;
				var wirelessAIDs24	    	= 0;
				var wirelessAIDs5		    = 0;
				var wirelessAvgRSSI	    	= 0;
				var wirelessAvgRSSI24    	= 0;
				var wirelessAvgRSSI5    	= 0;
				var wirelessAvgQuality		= 0;
				var wirelessAvgQuality24	= 0;
				var wirelessAvgQuality5		= 0;
				var wirelessAvgTxRate		= 0;
				var wirelessAvgTxRate24		= 0;
				var wirelessAvgTxRate5		= 0;
				var wirelessAvgRx	    	= 0;
				var wirelessAvgTx	    	= 0;
				var wirelessAvgRx24	    	= 0;
				var wirelessAvgTx24	    	= 0;
				var wirelessAvgRx5	    	= 0;
				var wirelessAvgTx5	    	= 0;

				if (wirelessMode == "Basic") {
					i = "selected";
					j = "";
				}
				else {
					i = "";
					j = "selected";
				}
				
				if (plotMACsAll == 1) 
					plotAll = "checked";
				
				html += '<table class="form" style="width: ' + wirelessTabeWidth + ';">';
				html +=	'<tr>';
				html += '<td class="title" colspan="' + (wirelessTableColumn / 2 | 0) + '" id="stalistWirelessStations">Wireless Station List</td>';
				html += '<td class="title" colspan="' + (wirelessTableColumn - (wirelessTableColumn /2 | 0)) + '" style="text-align: right;">';
				html += '<select name="stalistWirelessModeSelect" class="half" onChange="wirelessModeChange();">';
				html += '<option value="Basic" id="stalistWirelessModeBasic" ' + i + '>Basic</option>';
				html += '<option value="Advanced" id="stalistWirelessModeAdvanced" ' + j + '>Advanced</option>';
				html += '</select>';
				html += '</td>';
				html +=	'</tr>';
				html +=	'<tr>';
				html +=	'<th>AID</th>';
				html +=	'<th id="stalistMacAddr">MAC ADDRESS</th>';
				html +=	'<th id="stalistConnTime">CONN TIME</th>';
				if (wirelessMode == "Advanced") {
					html +=	'<th id="stalistPSM">PSM</th>';
					html +=	'<th id="stalistMIMO">MIMO PS</th>';
					html +=	'<th id="stalistMCS">MCS</th>';
					html +=	'<th id="stalistBW">BW</th>';
					html +=	'<th id="stalistSGI">SGI</th>';
					html +=	'<th id="stalistSTBC">STBC</th>';
					html +=	'<th id="stalistLDPC">LDPC</th>';
					html +=	'<th id="stalistMODE">MODE</th>';
				}
				html +=	'<th id="stalistTxRate">TX RATE</th>';
				html +=	'<th id="stalistRSSI">RSSI</th>';
				html +=	'<th id="stalistQuality">QUALITY</th>';
				html +=	'<th id="stalistRxTxCount">RX/TX COUNT</th>';
				html += '<th><div style="display: flex; height: 1.75em; align-items: center; width: 5em; margin: 0 auto;"><div style="margin: 0 auto;" id="stalistPlot">PLOT</div>';
				html += '<div><input type="checkbox" id="checkbox_all" ' + plotAll + ' onClick="addremoveplotMACsAll();"></div></div></th>';
				html +=	'<th id="stalistAction">ACTIONS</th>';
				html +=	'</tr>';

				if (data.stationlist24 !== undefined) {
					for (i = 0; i < data.stationlist24.length; i++) {

						mac_id = data.stationlist24[i].mac.replace(/:/g, '');

						if (plotMACs.indexOf(mac_id) != "-1")
							checked = "checked";
						else {
							if (plotMACsAll == 1) {
								checked = "checked";
								plotMACs.push(mac_id);
							}
							else {
								checked = "";
							}
						}

						html += '<tr>';
						html += '<td style="background-color: #C4D7FF; text-align: center">' + data.stationlist24[i].aid + '</td>';		//AID
						html += '<td style="background-color: #C4D7FF; text-align: center">' + data.stationlist24[i].mac + '</td>';		//MAC
						html += '<td style="text-align: center">' + data.stationlist24[i].conntime + '</td>';					//CONNTIME
						if (wirelessMode == "Advanced") {
							html += '<td style="text-align: center">' + data.stationlist24[i].psm + '</td>';				//PSM
							html += '<td style="text-align: center">' + data.stationlist24[i].mimo + '</td>';				//MIMO
							html += '<td style="text-align: center">' + data.stationlist24[i].mcs + '</td>';				//MCS
							html += '<td style="text-align: center">' + data.stationlist24[i].bw + '</td>';					//BW
							html += '<td style="text-align: center">' + data.stationlist24[i].sgi + '</td>';				//SGI
							html += '<td style="text-align: center">' + data.stationlist24[i].stbc + '</td>';				//STBC
							html += '<td style="text-align: center">' + data.stationlist24[i].ldpc + '</td>';				//LDPC
							html += '<td style="text-align: center">' + data.stationlist24[i].mode + '</td>';				//MODE
						}
						html += '<td style="text-align: center">' + data.stationlist24[i].txrate +  'MBit/s' + '</td>';		//TX RATE
						html += '<td style="text-align: center">' + data.stationlist24[i].rssi0;							//RSSI
						if (data.stationlist24[i].rssi1 !== undefined)
							html += ', ' + data.stationlist24[i].rssi1;
						if (data.stationlist24[i].rssi2 !== undefined)
							html += ', ' + data.stationlist24[i].rssi2;
						html += '</td>';
						html += '<td style="text-align: center">' + data.stationlist24[i].quality0 + '%';					//QUALITY
						if (data.stationlist24[i].quality1 !== undefined) 
							html += ', ' + data.stationlist24[i].quality1 + '%';
						if (data.stationlist24[i].quality2 !== undefined)
							html += ', ' + data.stationlist24[i].quality2 + '%';
						html += '</td>';

						if (data.stationlist24[i].quality2 != undefined) {
							quality = (+data.stationlist24[i].quality2 + +data.stationlist24[i].quality1 + +data.stationlist24[i].quality0) / 3 | 0;
						} 
						else if (data.stationlist24[i].quality1 != undefined) {
							quality = (+data.stationlist24[i].quality0 + +data.stationlist24[i].quality1) / 2 | 0;
						} 
						else {
							quality = +data.stationlist24[i].quality0;
						}

						if (data.stationlist24[i].rssi != undefined) {
							rssi = (+data.stationlist24[i].rssi2 + +data.stationlist24[i].rssi1 + +data.stationlist24[i].rssi0) / 3 | 0;
						} 
						else if (data.stationlist24[i].quality1 != undefined) {
							rssi = (+data.stationlist24[i].rssi1 + +data.stationlist24[i].rssi0) / 2 | 0;
						} 
						else {
							rssi = +data.stationlist24[i].rssi0;
						}
						
						rx = +data.stationlist24[i].rxbytes.replace(/ /g, '');
						if (rx >= (1024 * 1024 * 1024))
							rx = (rx / 1024 / 1024 / 1024).toFixed(2) + " GiB";
						else if (rx >= (1024 * 1024))
							rx = (rx / 1024 / 1024).toFixed(2) + " MiB";
						else if (rx >= (1024))
							rx = (rx / 1024).toFixed(0) + " KiB";

						tx = +data.stationlist24[i].txbytes.replace(/ /g, '');
						if (tx >= (1024 * 1024 * 1024))
							tx = (tx / 1024 / 1024 / 1024).toFixed(2) + " GiB";
						else if (tx >= (1024 * 1024))
							tx = (tx / 1024 / 1024).toFixed(2) + " MiB";
						else if (tx >= (1024))
							tx = (tx / 1024).toFixed(0) + " KiB";

						html += '<td style="text-align: center">' + rx +  ' / ' +  tx + '</td>';						//RX/TX BYTES
						html += '<td style="text-align: center"><input type="checkbox" id="checkbox_' + mac_id + '" ' + checked + ' onClick="addremoveplotMACs(' + "'" + mac_id + "'" + ')"></td>';
						html += '<td style="text-align: center"><input type="button" id="disconnect" value="disconnect" onClick="disconnectStation(this.form, ' + "'" + data.stationlist24[i].mac + "');" +'"></td>';	//DISCONNECT
						html += '</tr>';

						wirelessAIDs++;
						wirelessAIDs24++;
						wirelessAvgRSSI24		+= +rssi;
						wirelessAvgQuality24	+= +quality;
						wirelessAvgTxRate24		+= +data.stationlist24[i].txrate;
						wirelessAvgRx24			+= +data.stationlist24[i].rxbytes.replace(/ /g, '');
						wirelessAvgTx24			+= +data.stationlist24[i].txbytes.replace(/ /g, '');
					}
				}

				if (data.stationlist5 !== undefined) {
					for (i = 0; i < data.stationlist5.length; i++) {

						mac_id = data.stationlist5[i].mac.replace(/:/g, '');

						if (plotMACs.indexOf(mac_id) != "-1")
								checked = "checked";
						else {
							if (plotMACsAll == 1) {
								checked = "checked";
								plotMACs.push(mac_id);
							}
							else {
								checked = "";
							}
						}

						if (plotMACs.indexOf(mac_id) != "-1")							// Flag of checked client
							checked = "checked";
						else
							checked = "";

						html += '<tr>';
						html += '<td style="background-color: #C4FFC4; text-align: center">' + data.stationlist5[i].aid + '</td>';		//AID
						html += '<td style="background-color: #C4FFC4; text-align: center">' + data.stationlist5[i].mac + '</td>';		//MAC
						html += '<td style="text-align: center">' + data.stationlist5[i].conntime + '</td>';					//CONNTIME
						if (wirelessMode == "Advanced") {
							html += '<td style="text-align: center">' + data.stationlist5[i].psm + '</td>';					//PSM
							html += '<td style="text-align: center">' + data.stationlist5[i].mimo + '</td>';				//MIMO
							html += '<td style="text-align: center">' + data.stationlist5[i].mcs + '</td>';					//MCS
							html += '<td style="text-align: center">' + data.stationlist5[i].bw + '</td>';					//BW
							html += '<td style="text-align: center">' + data.stationlist5[i].sgi + '</td>';					//SGI
							html += '<td style="text-align: center">' + data.stationlist5[i].stbc + '</td>';				//STBC
							html += '<td style="text-align: center">' + data.stationlist5[i].ldpc + '</td>';				//LDPC
							html += '<td style="text-align: center">' + data.stationlist5[i].mode + '</td>';				//MODE
						}
						html += '<td style="text-align: center">' + data.stationlist5[i].txrate +  'MBit/s' + '</td>';				//TX RATE
						html += '<td style="text-align: center">' + data.stationlist5[i].rssi0;							//RSSI
						if (data.stationlist5[i].rssi1 !== undefined)
							html += ', ' + data.stationlist5[i].rssi1;
						if (data.stationlist5[i].rssi2 !== undefined)
							html += ', ' + data.stationlist5[i].rssi2;
						html += '</td>';
						html += '<td style="text-align: center">' + data.stationlist5[i].quality0 + '%';					//QUALITY
						if (data.stationlist5[i].quality1 !== undefined)
							html += ', ' + data.stationlist5[i].quality1 + '%';
						if (data.stationlist5[i].quality2 !== undefined)
							html += ', ' + data.stationlist5[i].quality2 + '%';
						html += '</td>';

						if (data.stationlist5[i].quality2 != undefined) {
							quality = (+data.stationlist5[i].quality2 + +data.stationlist5[i].quality1 + +data.stationlist5[i].quality0) / 3 | 0;
						} 
						else if (data.stationlist5[i].quality1 != undefined) {
							quality = (+data.stationlist5[i].quality0 + +data.stationlist5[i].quality1) / 2 | 0;
						} 
						else {
							quality = +data.stationlist5[i].quality0;
						}

						if (data.stationlist5[i].rssi != undefined) {
							rssi = (+data.stationlist5[i].rssi2 + +data.stationlist5[i].rssi1 + +data.stationlist5[i].rssi0) / 3 | 0;
						} 
						else if (data.stationlist5[i].quality1 != undefined) {
							rssi = (+data.stationlist5[i].rssi1 + +data.stationlist5[i].rssi0) / 2 | 0;
						} 
						else {
							rssi = +data.stationlist5[i].rssi0;
						}
						
						rx = +data.stationlist5[i].rxbytes.replace(/ /g, '');
						if (rx >= (1024 * 1024 * 1024))
							rx = (rx / 1024 / 1024 / 1024).toFixed(2) + " GiB";
						else if (rx >= (1024 * 1024))
							rx = (rx / 1024 / 1024).toFixed(2) + " MiB";
						else if (rx >= (1024))
							rx = (rx / 1024).toFixed(0) + " KiB";

						tx = +data.stationlist5[i].txbytes.replace(/ /g, '');
						if (tx >= (1024 * 1024 * 1024))
							tx = (tx / 1024 / 1024 / 1024).toFixed(2) + " GiB";
						else if (tx >= (1024 * 1024))
							tx = (tx / 1024 / 1024).toFixed(2) + "MiB";
						else if (tx >= (1024))
							tx = (tx / 1024).toFixed(0) + " KiB";

						html += '<td style="text-align: center">' + rx +  ' / ' +  tx + '</td>';	//RX/TX BYTES
						html += '<td style="text-align: center"><input type="checkbox" id="checkbox_' + mac_id + '" ' + checked + ' onClick="addremoveplotMACs(' + "'" + mac_id + "'" + ')"></td>';
						html += '<td style="text-align: center"><input type="button" id="disconnect" value="disconnect" onClick="disconnectStation(this.form, ' + "'" + data.stationlist5[i].mac + "');" +'"></td>';	//DISCONNECT
						html += '</tr>';

						wirelessAIDs++;
						wirelessAIDs5++;
						wirelessAvgRSSI5		+= +rssi;
						wirelessAvgQuality5		+= +quality;
						wirelessAvgTxRate5		+= +data.stationlist5[i].txrate;
						wirelessAvgRx5			+= +data.stationlist5[i].rxbytes.replace(/ /g, '');
						wirelessAvgTx5			+= +data.stationlist5[i].txbytes.replace(/ /g, '');
					}
				}

				html +=	'</table>';
				document.getElementById('stationList').innerHTML = html;

				displayElement('stationWirelessButtons', (wirelessAIDs > 0));

				wirelessAvgRSSI		= ((wirelessAvgRSSI24 + wirelessAvgRSSI5) / wirelessAIDs | 0);
				wirelessAvgQuality	= ((wirelessAvgQuality24 + wirelessAvgQuality5) / wirelessAIDs | 0) + "%";
				wirelessAvgTxRate	= ((wirelessAvgTxRate24 + wirelessAvgTxRate5) / wirelessAIDs | 0);
				wirelessAvgRx		= wirelessAvgRx24 + wirelessAvgRx5;
				wirelessAvgTx		= wirelessAvgTx24 + wirelessAvgTx5;
				
				if (wirelessAIDs24 > 0) {
					wirelessAvgRSSI24		= (wirelessAvgRSSI24 / wirelessAIDs24 | 0);
					wirelessAvgQuality24	= (wirelessAvgQuality24 / wirelessAIDs24 | 0) + "%";
					wirelessAvgTxRate24		= (wirelessAvgTxRate24 / wirelessAIDs24 | 0);
					wirelessAvgTx24			= (wirelessAvgTx24 / wirelessAIDs24 | 0);
					wirelessAvgRx24			= (wirelessAvgRx24 / wirelessAIDs24 | 0);
					
					// Average RX 2.4GHz
					if (wirelessAvgRxLast24 == 0)
						wirelessAvgRxLast24 = wirelessAvgRx24;
					bytes = ((wirelessAvgRx24 - wirelessAvgRxLast24) / (updateTime / 1000) | 0);
					wirelessAvgRxLast24 = wirelessAvgRx24;

					if (bytes >= (1024 * 1024))
						wirelessAvgRx24 = (bytes / 1024 / 1024).toFixed(2) + " MiB";
					else if (bytes >= 1024)
						wirelessAvgRx24 = (bytes / 1024).toFixed(0) + " KiB";
					else if (bytes > 0)
						wirelessAvgRx24 = bytes + " B";
					else
						wirelessAvgRx24 = "-";

					// Average TX 2.4GHz
					if (wirelessAvgTxLast24 == 0)
						wirelessAvgTxLast24 = +wirelessAvgTx24;
					bytes = ((+wirelessAvgTx24 - wirelessAvgTxLast24) / (updateTime / 1000) | 0);
					wirelessAvgTxLast24 = wirelessAvgTx24;

					if (bytes >= (1024 * 1024))
						wirelessAvgTx24 = (bytes / 1024 / 1024).toFixed(2) + " MiB";
					else if (bytes >= 1024)
						wirelessAvgTx24 = (bytes / 1024).toFixed(0) + " KiB";
					else if (bytes > 0)
						wirelessAvgTx24 = bytes + " B";
					else
						wirelessAvgTx24 = "-";
				}
				else {
					wirelessAvgRSSI24		= "-";
					wirelessAvgQuality24	= "-";
					wirelessAvgRx24			= "-";
					wirelessAvgTx24			= "-";
					wirelessAvgTxRate24		= "-";
				}

				if (wirelessAIDs5 > 0) {
					wirelessAvgRSSI5		= (wirelessAvgRSSI5 / wirelessAIDs5 | 0);
					wirelessAvgQuality5		= (wirelessAvgQuality5 / wirelessAIDs5 | 0) + "%";
					wirelessAvgTxRate5		= (wirelessAvgTxRate5 / wirelessAIDs5 | 0);
					wirelessAvgTx5			= (wirelessAvgTx5 / wirelessAIDs5 | 0);
					wirelessAvgRx5			= (wirelessAvgRx5 / wirelessAIDs5 | 0);
					
					// Average RX 5GHz
					if (wirelessAvgRxLast5 == 0)
						wirelessAvgRxLast5 = +wirelessAvgRx5;
					bytes = ((+wirelessAvgRx5 - wirelessAvgRxLast5) / (updateTime / 1000) | 0);
					wirelessAvgRxLast5 = wirelessAvgRx5;

					if (bytes >= (1024 * 1024))
						wirelessAvgRx5 = (bytes / 1024 / 1024).toFixed(2) + " MiB";
					else if (bytes >= 1024)
						wirelessAvgRx5 = (bytes / 1024).toFixed(0) + " KiB";
					else if (bytes > 0)
						wirelessAvgRx5 = bytes + " B";
					else
						wirelessAvgRx5 = "-";

					// Average TX 2.4GHz
					if (wirelessAvgTxLast5 == 0)
						wirelessAvgTxLast5 = +wirelessAvgTx5;
					bytes = ((+wirelessAvgTx5 - wirelessAvgTxLast5) / (updateTime / 1000) | 0);
					wirelessAvgTxLast5 = wirelessAvgTx5;

					if (bytes >= (1024 * 1024))
						wirelessAvgTx5 = (bytes / 1024 / 1024).toFixed(2) + " MiB";
					else if (bytes >= 1024)
						wirelessAvgTx5 = (bytes / 1024).toFixed(0) + " KiB";
					else if (bytes > 0)
						wirelessAvgTx5 = bytes + " B";
					else
						wirelessAvgTx5 = "-";
				}
				else {
					wirelessAvgRSSI5		= "-";
					wirelessAvgQuality5		= "-";
					wirelessAvgRx5			= "-";
					wirelessAvgTx5			= "-";
					wirelessAvgTxRate5		= "-";
				}				
				
				if (wirelessAIDs > 0) {
					// Average RX
					if (wirelessAvgRxLast == 0)
						wirelessAvgRxLast = +wirelessAvgRx;
					bytes = ((+wirelessAvgRx - wirelessAvgRxLast) / (updateTime / 1000) | 0);
					wirelessAvgRxLast = wirelessAvgRx;

					if (bytes >= (1024 * 1024))
						wirelessAvgRx = (bytes / 1024 / 1024).toFixed(2) + " MiB";
					else if (bytes >= 1024)
						wirelessAvgRx = (bytes / 1024).toFixed(0) + " KiB";
					else if (bytes > 0)
						wirelessAvgRx = bytes + " B";
					else
						wirelessAvgRx = "-";

					// Average TX
					if (wirelessAvgTxLast == 0)
						wirelessAvgTxLast = +wirelessAvgTx;
					bytes = ((+wirelessAvgTx - wirelessAvgTxLast) / (updateTime / 1000) | 0);
					wirelessAvgTxLast = wirelessAvgTx;

					if (bytes >= (1024 * 1024))
						wirelessAvgTx = (bytes / 1024 / 1024).toFixed(2) + " MiB";
					else if (bytes >= 1024)
						wirelessAvgTx = (bytes / 1024).toFixed(0) + " KiB";
					else if (bytes > 0)
						wirelessAvgTx = bytes + " B";
					else
						wirelessAvgTx = "-";
				}
				else {
					wirelessAvgTxRate	= "-";
					wirelessAvgRSSI		= "-";
					wirelessAvgQuality	= "-";
					wirelessAvgRx		= "-";
					wirelessAvgTx		= "-";
				}

				document.getElementById('stalistWirelessSumAIDData').innerHTML			= wirelessAIDs;
				document.getElementById('stalistWirelessSumAIDData24').innerHTML		= wirelessAIDs24;
				document.getElementById('stalistWirelessSumAIDData5').innerHTML			= wirelessAIDs5;
				
				document.getElementById('stalistWirelessSumTXRateData').innerHTML		= wirelessAvgTxRate;
				document.getElementById('stalistWirelessSumTXRateData24').innerHTML 	= wirelessAvgTxRate24;
				document.getElementById('stalistWirelessSumTXRateData5').innerHTML		= wirelessAvgTxRate5;
				
				document.getElementById('stalistWirelessSumRSSIData').innerHTML			= wirelessAvgRSSI;
				document.getElementById('stalistWirelessSumRSSIData24').innerHTML		= wirelessAvgRSSI24;
				document.getElementById('stalistWirelessSumRSSIData5').innerHTML		= wirelessAvgRSSI5;
				
				document.getElementById('stalistWirelessSumQualityData').innerHTML		= wirelessAvgQuality;
				document.getElementById('stalistWirelessSumQualityData24').innerHTML	= wirelessAvgQuality24;
				document.getElementById('stalistWirelessSumQualityData5').innerHTML		= wirelessAvgQuality5;
				
				document.getElementById('stalistWirelessSumTxRxData').innerHTML			= wirelessAvgRx + " / " + wirelessAvgTx;
				document.getElementById('stalistWirelessSumTxRxData24').innerHTML		= wirelessAvgRx24 + " / " + wirelessAvgTx24;
				document.getElementById('stalistWirelessSumTxRxData5').innerHTML		= wirelessAvgRx5 + " / " + wirelessAvgTx5;

				initTranslation();
				showPlot();
			}

			function getStationsCounters()
			{
				var i, mac_id;
				var data	                = JSON.parse(document.getElementById('stationListData').innerHTML);
				var time	                = new Date().getTime();
				var MACs                	= plotMACs.slice(0);
				var rate;           												// Tx Rate for ploting
				var quality;        												// Quality for ploting
				var rssi;					            							// RSSI for ploting;
				var rxbytes, txbytes;       										// Rx/Tx Bytes for ploting;

				if (data.stationlist24 !== undefined) {
					for (i = 0; i < data.stationlist24.length; i++) {

						mac_id = data.stationlist24[i].mac.replace(/:/g, '');

						rate		= null;
						quality		= null;
						rssi		= null;
						rxbytes		= txbytes = null;

						if (MACs.indexOf(mac_id) != "-1")
							MACs.splice(MACs.indexOf(mac_id), 1);

						rate = data.stationlist24[i].txrate;

						if (data.stationlist24[i].quality2 !== undefined)
							quality = ((+data.stationlist24[i].quality2 + +data.stationlist24[i].quality1 + +data.stationlist24[i].quality0) / 3 | 0);
						else if (data.stationlist24[i].quality1 !== undefined)
							quality = ((+data.stationlist24[i].quality1 + +data.stationlist24[i].quality0) / 2 | 0);
						else
							quality = +data.stationlist24[i].quality0;

						if (data.stationlist24[i].rssi2 !== undefined)
							rssi = -((-data.stationlist24[i].rssi2 + -data.stationlist24[i].rssi1 + -data.stationlist24[i].rssi0) / 3 | 0);
						else if (data.stationlist24[i].rssi1 !== undefined)
							rssi = -((-data.stationlist24[i].rssi1 + -data.stationlist24[i].rssi0) / 2 | 0);
						else if (data.stationlist24[i].rssi0 !== undefined)
							rssi = data.stationlist24[i].rssi0;

						rxbytes = +data.stationlist24[i].rxbytes.replace(/ /g, '');
						txbytes = +data.stationlist24[i].txbytes.replace(/ /g, '');

						plotData.push([ mac_id, time, rate, rssi, quality, rxbytes, txbytes ]);
					}
				}

				if (data.stationlist5 !== undefined) {
					for (i = 0; i < data.stationlist5.length; i++) {

						mac_id = data.stationlist5[i].mac.replace(/:/g, '');

						rate		= null;
						quality		= null;
						rssi		= null;
						rxbytes		= txbytes = null;

						if (MACs.indexOf(mac_id) != "-1")
							MACs.splice(MACs.indexOf(mac_id), 1);

						rate = data.stationlist5[i].txrate;

						if (data.stationlist5[i].quality2 !== undefined)
							quality = ((+data.stationlist5[i].quality2 + +data.stationlist5[i].quality1 + +data.stationlist5[i].quality0) / 3 | 0);
						else if (data.stationlist5[i].quality1 !== undefined)
							quality = ((+data.stationlist5[i].quality1 + +data.stationlist5[i].quality0) / 2 | 0);
						else
							quality = +data.stationlist5[i].quality0;

						if (data.stationlist5[i].rssi2 !== undefined)
							rssi = -((-data.stationlist5[i].rssi2 + -data.stationlist5[i].rssi1 + -data.stationlist5[i].rssi0) / 3 | 0);
						else if (data.stationlist5[i].rssi1 !== undefined)
							rssi = -((-data.stationlist5[i].rssi1 + -data.stationlist5[i].rssi0) / 2 | 0);
						else if (data.stationlist5[i].rssi0 !== undefined)
							rssi = data.stationlist5[i].rssi0;

						rxbytes = +data.stationlist5[i].rxbytes.replace(/ /g, '');
						txbytes = +data.stationlist5[i].txbytes.replace(/ /g, '');

						plotData.push([ mac_id, time, rate, rssi, quality, rxbytes, txbytes ]);
					}
				}

				for (i = 0; i < MACs.length; i++) {
					plotData.push([ MACs[i], time, null, null, null, null, null ]);

					for (j = 0; j < plotData.length; j++) {
						if (plotData[j][0] == MACs[i]) {
							plotData[j][5] = null;
							plotData[j][6] = null;
						}
					}
					if (lastRxTxCount.indexOf(MACs[i]) != "-1") {
						lastRxTxCount.splice(lastRxTxCount.indexOf(MACs[i].replace(/:/g, '')), 3);
					}
				}
				setCookie('plotData', JSON.stringify(plotData));
			}
			
			
			function showPlot()
			{
				var time			= new Date().getTime();
				var plotType		= document.getElementById('wirelessPlotType').selectedIndex;
				var plotTime		= document.getElementById('wirelessPlotTime').selectedIndex;
				var plotGraphData	= [];
				var allRxTxTmp		= [];
				var plotOptions;
				var startTime;
				var allRx, allTX;
				var i = j = k = tmp = label = data = RxTxCount = labelRxTx = "";

				switch(plotTime) {
					case 0:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 1 * 60 * 1000).getTime() };		break;	// 1M
					case 1:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 2 * 60 * 1000).getTime() };		break;	// 2M
					case 2:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 3 * 60 * 1000).getTime() };		break;	// 3M
					case 3:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 4 * 60 * 1000).getTime() };		break;	// 4M
					case 4:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 5 * 60 * 1000).getTime() };		break;	// 5M
					case 5:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 10 * 60 * 1000).getTime() };		break;	// 10M
					case 6:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 15 * 60 * 1000).getTime() };		break;	// 15M
					case 7:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 20 * 60 * 1000).getTime() };		break;	// 20M
					case 8:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 30 * 60 * 1000).getTime() };		break;	// 30M
					case 9:		startTime = { mode: "time", timezone: "browser", min: new Date(time - 1 * 3600 * 1000).getTime() };		break;	// 1H
					case 10:	startTime = { mode: "time", timezone: "browser", min: new Date(time - 3 * 3600 * 1000).getTime() };		break;	// 3H
					case 11:	startTime = { mode: "time", timezone: "browser", min: new Date(time - 6 * 3600 * 1000).getTime() };		break;	// 6H
					case 12:	startTime = { mode: "time", timezone: "browser" };														break;	// All time
				}
				switch(plotType) {
					case 0:		plotOptions = { legend: { position: "nw" }, xaxis: startTime, yaxis: { min: 0 } }; 				        break;	// TX RATE
					case 1:		plotOptions = { legend: { position: "nw" }, xaxis: startTime, yaxis: { min: -100, max: 0 } };			break;	// RSSI
					case 2:		plotOptions = { legend: { position: "nw" }, xaxis: startTime, yaxis: { min: 0, max: 100 } };			break;	// QUALITY
					default:	plotOptions = { legend: { position: "nw" }, xaxis: startTime, yaxis: {
						tickFormatter: function (v, axis) {
							if (v > 1000000) {
								v = v / 1000000;
								return v.toFixed(2) + "M"
							}
							if (v > 1000) {
								v = v / 1000;
								return v.toFixed(2) + "K";
							}
							return v;
						},
						min: 0 } };								                														break;	// RX/TX COUNT
				}

				displayElement("tableWirelessPlot", plotMACs.length > 0)
				
				if (plotMACs.length > 0) {
					for (i = 0; i < plotMACs.length; i++) {
						data = "";

						// Converting Lable format
						label = "";
						tmp = plotMACs[i].match(/(.{1,2})/gim);
						for (j = 0; j < tmp.length; j++)
							if ((j + 1) < tmp.length)
								label = label + tmp[j] + ":";
							else
								label = label + tmp[j];

						// Filling Data
						if (plotType == 3)
							labelRxTx = ' (Rx Bytes)';
						if (plotType != 4) {
							data += '{ "label":"' + label + labelRxTx + '", ';
							data += ' "data": [ ';
						}
						for (j = 0; j < plotData.length; j++) {
							if (plotData[j][0] == plotMACs[i]) {
								if (plotType == 4) {
									allRxTxTmp.push( [ plotData[j][1], plotData[j][5], plotData[j][6] ] );
								} 
								else {
									if (data[data.length - 1] == "]")
										data += ", ";
									switch(plotType) {
										case 0:		data += '[ ' + plotData[j][1] + ', ' + plotData[j][2] + ' ]';	    break;	// TX RATE
										case 1:		data += '[ ' + plotData[j][1] + ', ' + plotData[j][3] + ' ]';	    break;	// RSSI
										case 2:		data += '[ ' + plotData[j][1] + ', ' + plotData[j][4] + ' ]';	    break;	// QUALITY
										case 3:		lastCount = "";
											if (lastRxTxCount.indexOf(plotData[j][0]) == "-1") {
												lastRxTxCount.push(plotData[j][0]);
												lastRxTxCount.push(plotData[j][5]);
												lastRxTxCount.push(plotData[j][6]);
												lastCount = plotData[j][5];
											}
											else {
												lastCount = lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 1];
												lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 1] = plotData[j][5];
											}
											if (lastCount > +plotData[j][5])
												lastCount = +plotData[j][5];
											RxTxCount = ((+plotData[j][5] - lastCount) / (updateTime / 1000) | 0);
											if (RxTxCount < 0)
												RxTxCount = 0;
											data += '[ ' + plotData[j][1] + ', ' + RxTxCount + ' ]';                    break;	// RX/TX COUNT
										case 5: lastCount = "";
											if (lastRxTxCount.indexOf(plotData[j][0]) == "-1") {
												lastRxTxCount.push(plotData[j][0]);
												lastRxTxCount.push(plotData[j][5]);
												lastRxTxCount.push(plotData[j][6]);
												lastCount = plotData[j][5];
											}
											else {
												lastCount = lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 1];
												lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 1] = plotData[j][5];
											}
											if (lastCount > +plotData[j][5])
												lastCount = +plotData[j][5];
											RxTxCount = ((+plotData[j][5] - lastCount) / (updateTime / 1000) | 0);
											if (RxTxCount < 0)
												RxTxCount = 0;
											data += '[ ' + +plotData[j][1] + ', ' + RxTxCount + ' ]';                    break;	// RX COUNT
										case 6: lastCount = "";
											if (lastRxTxCount.indexOf(plotData[j][0]) == "-1") {
												lastRxTxCount.push(plotData[j][0]);
												lastRxTxCount.push(plotData[j][5]);
												lastRxTxCount.push(plotData[j][6]);
												lastCount = plotData[j][6];
											}
											else {
												lastCount = lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 2];
												lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 2] = plotData[j][6];
											}
											RxTxCount = ((+plotData[j][6] - lastCount) / (updateTime / 1000) | 0);
											if (RxTxCount < 0)
												RxTxCount = 0;
											data += '[ ' + +plotData[j][1] + ', ' + RxTxCount + ' ]';					break;	// TX COUNT
									}
								}
							}
						}
						
						if (plotType == 4) {
							var allTime;
							var lastAllRx;
							var lastAllTx;
							data += '{ "label":"Summary RX", "data": [';
							for (i = 0; i < allRxTxTmp.length; i++) {
								if (data[data.length - 1] == "]")
									data += ", ";
								allTime = allRxTxTmp[i][0];
								allRx	= 0;
								for (j = 0; j < allRxTxTmp.length; j++) {
									if ((allRxTxTmp[j][0] == allTime) && (allRxTxTmp[j][1] != "-1")) {
										allRx += allRxTxTmp[j][1];
										allRxTxTmp[j][1] = "-1";
									}
								}
								if (lastAllRx == 0)
									lastAllRx = allRx;
								data += ' [ ' + allTime + ', ' + ((allRx - lastAllRx) / (updateTime / 1000) | 0) + ' ]';
								lastAllRx = allRx;
							}
							data += ']}';
							plotGraphData.push(JSON.parse(data));
							data = '{ "label":"Summary TX", "data": [';
							for (i = 0; i < allRxTxTmp.length; i++) {
								if (data[data.length - 1] == "]")
									data += ", ";
								allTime = allRxTxTmp[i][0];
								allTx	= 0;
								for (j = 0; j < allRxTxTmp.length; j++) {
									if ((allRxTxTmp[j][0] == allTime) && (allRxTxTmp[j][2] != "-1")) {
										allTx += allRxTxTmp[j][2];
										allRxTxTmp[j][2] = "-1";
									}
								}
								if (lastAllTx == 0)
									lastAllTx = allTx;
								data += '[ ' + allTime + ', ' + ((allTx - lastAllTx) / (updateTime / 1000) | 0) + ' ]';
								lastAllTx = allTx;
							}
						}
						
						data += ' ] }';
						plotGraphData.push(JSON.parse(data));

						if (plotType == 3) {
							data = "";
							labelRxTx = ' (Tx Bytes) ';
							data += '{ "label":"' + label + labelRxTx + '", ';
							data += ' "data": [ ';
							for (j = 0; j < plotData.length; j++) {
								if (plotData[j][0] == plotMACs[i]) {
									if (data[data.length - 1] == "]")
										data += ", ";
									lastCount = "";
									if (lastRxTxCount.indexOf(plotData[j][0]) == "-1") {
										lastRxTxCount.push(plotData[j][0]);
										lastRxTxCount.push(plotData[j][5]);
										lastRxTxCount.push(plotData[j][6]);
										lastCount = plotData[j][6];
									}
									else {
										lastCount = lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 2];
										lastRxTxCount[lastRxTxCount.indexOf(plotData[j][0]) + 2] = plotData[j][6];
									}
									RxTxCount = ((+plotData[j][6] - lastCount) / (updateTime / 1000) | 0);
									if (RxTxCount < 0)
										RxTxCount = 0;
									data += '[ ' + plotData[j][1] + ', ' + RxTxCount + ' ]';
								}
							}
							data += ' ] }';
							plotGraphData.push(JSON.parse(data));
						}
					}

					$.plot($("#placeholder"), plotGraphData, plotOptions);
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"></tr>
			<tr>
				<td><h1 id="stalistTitle">Station List</h1>
					<p id="stalistIntroduction"> Here you can monitor stations associated with this AP. </p>
					<hr />
					<form name="sta" action="/goform/disconnectSta" method="POST">
						<div id="stationListData" style="display: none;"></div>
						<div id="stationList"></div>
						<table id="stationWirelessButtons" class="button" style="display: none;">
							<tr>
								<td><input type="hidden" name="disconnectSta" value="" />
									<input type="button" id="disconnectAll" value="Disconnect all" class="normal" onClick="disconnectStation(this.form, '*');">
									<input type="hidden" name="submit-url" value="/wireless/stainfo.asp" >
								</td>
							</tr>
						</table>
						<br>
						<table id="tableWirelessSummary" class="form">
							<tr>
								<td class="title" colspan="4" id="stalistWirelessSummary">Wireless Network Summary</td>
							</tr>
							<tr id="stalistWirelessSummHead">
								<th style="width: 40%;" id="stalistWirelessName"></th>
								<th style="width: 20%;" id="stalistWirelessNameSum">Total</th>
								<th style="width: 20%;" id="stalistWirelessNameSum24">2.4GHz</th>
								<th style="width: 20%;" id="stalistWirelessNameSum5">5GHz</th>
							</tr>
							<tr>
								<td id="stalistWirelessSumAID" class="head">Summary AIDs:</td>
								<td style="text-align: center;" id="stalistWirelessSumAIDData"></td>
								<td style="text-align: center;" id="stalistWirelessSumAIDData24"></td>
								<td style="text-align: center;" id="stalistWirelessSumAIDData5"></td>
							</tr>
							<tr>
								<td id="stalistWirelessSumTXRate" class="head">Average TX Rate 2.4GHz:</td>
								<td style="text-align: center;" id="stalistWirelessSumTXRateData"></td>
								<td style="text-align: center;" id="stalistWirelessSumTXRateData24"></td>
								<td style="text-align: center;" id="stalistWirelessSumTXRateData5"></td>
							</tr>
							<tr>
								<td id="stalistWirelessSumTxRx" class="head">Average Bitrate:</td>
								<td style="text-align: center;" id="stalistWirelessSumTxRxData"></td>
								<td style="text-align: center;" id="stalistWirelessSumTxRxData24"></td>
								<td style="text-align: center;" id="stalistWirelessSumTxRxData5"></td>
							</tr>
							<tr>
								<td id="stalistWirelessSumRSSI" class="head">Average RSSI:</td>
								<td style="text-align: center;" id="stalistWirelessSumRSSIData"></td>
								<td style="text-align: center;" id="stalistWirelessSumRSSIData24"></td>
								<td style="text-align: center;" id="stalistWirelessSumRSSIData5"></td>
							</tr>
							<tr>
								<td id="stalistWirelessSumQuality" class="head">Average Quality:</td>
								<td style="text-align: center;" id="stalistWirelessSumQualityData"></td>
								<td style="text-align: center;" id="stalistWirelessSumQualityData24"></td>
								<td style="text-align: center;" id="stalistWirelessSumQualityData5"></td>
							</tr>
						</table>
						<br>
						<table id="tableWirelessPlot" class="form" style="display: none;">
							<tr>
								<td class="title" colspan="2" id="stalistWirelessPlot">Wireless Network Plot</td>
							</tr>
							<tr>
								<td class=head><label for="wirelessPlotType" id="wirelessPlotTypeName">Graphic Type: </label>
									<select id="wirelessPlotType" name="wirelessPlotType" class="normal" onChange="showPlot();">
										<option value="0" id="typeTxRate" selected>TX Rate</option>
										<option value="1" id="typeRSSI">RSSI</option>
										<option value="2" id="typeQuality">Quality</option>
										<option value="3" id="typeRxTx">Rx/Tx Bandwidth</option>
										<option value="4" id="typeRxTxSum" selected>Rx/Tx Bandwidth Summary</option>
										<option value="5" id="typeRx">Rx Bandwidth</option>
										<option value="6" id="typeTx">Tx Bandwidth</option>
									</select>
								</td>
								<td class=head><label for="wirelessPlotTime" id="wirelessPlotTimeName">Graphic Time: </label>
									<select id="wirelessPlotTime" name="wirelessPlotTime" class="normal" onChange="showPlot();">
										<option value="0" id="time1M" selected>1 Minute</option>
										<option value="1" id="time2M">2 Minutes</option>
										<option value="2" id="time3M">3 Minutes</option>
										<option value="3" id="time4M">4 Minutes</option>
										<option value="4" id="time5M">5 Minutes</option>
										<option value="5" id="time10M">10 Minutes</option>
										<option value="6" id="time15M">15 Minutes</option>
										<option value="7" id="time20M">20 Minutes</option>
										<option value="8" id="time30M">30 Minutes</option>
										<option value="9" id="time1H">1H</option>
										<option value="10" id="time3H">3H</option>
										<option value="11" id="time6H">6H</option>
										<option value="12" id="timeAll">All time</option>
									</select>
								</td>
							</tr>
							<tr>
								<td colspan="2"><div id="placeholder" style="width: full; height: 300px;"></div></td>
							</tr>
						</table>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>
