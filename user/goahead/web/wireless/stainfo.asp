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
		<script src="/lang/b28n.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("wireless");
			Butterlate.setTextDomain("buttons");

			function initTranslation()
			{
				_TR("stalistTitle",			"stalist title");
				_TR("stalistIntroduction",	"stalist introduction");
				_TR("stalistWirelessNet",	"stalist wireless network");
				_TR("stalistMacAddr",		"stalist mac address");
				_TR("stalistConnTime",		"stalist conn time");
				_TR("stalistAction",		"basic action");
				_TRV("disconnectAll",		"button disconnect all");
			}

			function initValues()
			{
				var elements = document.getElementsByTagName('input');
				for (var i = 0; i < elements.length; i++)
					if(elements[i].id == "disconnect")
						elements[i].value = _("button disconnect");
				ajaxLoadElement("stationListData", "/wireless/stainfo_clients.asp", showStationList);
				setInterval('ajaxLoadElement("stationListData", "/wireless/stainfo_clients.asp", showStationList);', 5000);
				showWarning();
				initTranslation();
			}

			function showStationList()
			{
				var data = JSON.parse(document.getElementById('stationListData').innerHTML);
				var html = "";
				
				html += '<table class="form" style="width: 1100px;">';
				html +=	'<tr>';
				html += '<td class="title" colspan="16" id="stalistWirelessNet">Wireless Network</td>';
				html +=	'</tr>';
				html +=	'<tr>';
				html +=	'<th>AID</th>';
				html +=	'<th id="stalistMacAddr">MAC ADDRESS</th>';
				html +=	'<th id="stalistConnTime">CONN TIME</th>';
				html +=	'<th id="stalistPSM">PSM</th>';
				html +=	'<th id="stalistMIMO">MIMO PS</th>';
				html +=	'<th id="stalistMCS">MCS</th>';
				html +=	'<th id="stalistBW">BW</th>';
				html +=	'<th id="stalistSGI">SGI</th>';
				html +=	'<th id="stalistSTBC">STBC</th>';
				html +=	'<th id="stalistLDPC">LDPC</th>';
				html +=	'<th id="stalistMODE">MODE</th>';
				html +=	'<th id="stalistTxRate">TX RATE</th>';
				html +=	'<th id="stalistRSSI">RSSI</th>';
				html +=	'<th id="stalistQuality">QUALITY</th>';
				html +=	'<th id="stalistRxTxCount">RX/TX COUNT</th>';
				html +=	'<th id="stalistAction">ACTIONS</th>';
				html +=	'</tr>';
				
				if (data.stationlist24 !== undefined) {
					for (var i = 0; i < data.stationlist24.length; i++) {
						html += '<tr>';
						html += '<td style="background-color: #C4D7FF; text-align: center">' + data.stationlist24[i].aid + '</td>';		//AID
						html += '<td style="background-color: #C4D7FF; text-align: center">' + data.stationlist24[i].mac + '</td>';		//MAC
						html += '<td style="text-align: center">' + data.stationlist24[i].conntime + '</td>';							//CONNTIME
						html += '<td style="text-align: center">' + data.stationlist24[i].psm + '</td>';								//PSM
						html += '<td style="text-align: center">' + data.stationlist24[i].mimo + '</td>';								//MIMO
						html += '<td style="text-align: center">' + data.stationlist24[i].mcs + '</td>';								//MCS
						html += '<td style="text-align: center">' + data.stationlist24[i].bw + '</td>';									//BW
						html += '<td style="text-align: center">' + data.stationlist24[i].sgi + '</td>';								//SGI
						html += '<td style="text-align: center">' + data.stationlist24[i].stbc + '</td>';								//STBC
						html += '<td style="text-align: center">' + data.stationlist24[i].ldpc + '</td>';								//LDPC
						html += '<td style="text-align: center">' + data.stationlist24[i].mode + '</td>';								//MODE
						html += '<td style="text-align: center">' + data.stationlist24[i].txrate +  'MBit/s' + '</td>';					//TX RATE
						html += '<td style="text-align: center">' + data.stationlist24[i].rssi0;										//RSSI
						if (data.stationlist24[i].rssi1 !== undefined)
							html += ', ' + data.stationlist24[i].rssi1;
						if (data.stationlist24[i].rssi2 !== undefined)
							html += ', ' + data.stationlist24[i].rssi2;
						html += '</td>';
						html += '<td style="text-align: center">' + data.stationlist24[i].quality0 + '%';								//QUALITY
						if (data.stationlist24[i].quality1 !== undefined)
							html += ', ' + data.stationlist24[i].quality1 + '%';
						if (data.stationlist24[i].quality2 !== undefined)
							html += ', ' + data.stationlist24[i].quality2 + '%';
						html += '</td>';
						html += '<td style="text-align: center">' + data.stationlist24[i].rxbytes +  '/ ' +  data.stationlist24[i].txbytes + '</td>';	//RX/TX BYTES
						html += '<td style="text-align: center"><input type="button" id="disconnect" value="disconnect" onclick="disconnectStation(this.form, ' + "'" + data.stationlist24[i].mac + "');" +'"></td>';	//DISCONNECT
						html += '</tr>';
					}
				}
				if (data.stationlist5 !== undefined) {				
					for (var i = 0; i < data.stationlist5.length; i++) {
						html += '<tr>';
						html += '<td style="background-color: #C4FFC4; text-align: center">' + data.stationlist5[i].aid + '</td>';		//AID
						html += '<td style="background-color: #C4FFC4; text-align: center">' + data.stationlist5[i].mac + '</td>';		//MAC
						html += '<td style="text-align: center">' + data.stationlist5[i].conntime + '</td>';							//CONNTIME
						html += '<td style="text-align: center">' + data.stationlist5[i].psm + '</td>';								//PSM
						html += '<td style="text-align: center">' + data.stationlist5[i].mimo + '</td>';								//MIMO
						html += '<td style="text-align: center">' + data.stationlist5[i].mcs + '</td>';								//MCS
						html += '<td style="text-align: center">' + data.stationlist5[i].bw + '</td>';									//BW
						html += '<td style="text-align: center">' + data.stationlist5[i].sgi + '</td>';								//SGI
						html += '<td style="text-align: center">' + data.stationlist5[i].stbc + '</td>';								//STBC
						html += '<td style="text-align: center">' + data.stationlist5[i].ldpc + '</td>';								//LDPC
						html += '<td style="text-align: center">' + data.stationlist5[i].mode + '</td>';								//MODE
						html += '<td style="text-align: center">' + data.stationlist5[i].txrate +  'MBit/s' + '</td>';					//TX RATE
						html += '<td style="text-align: center">' + data.stationlist5[i].rssi0;										//RSSI
						if (data.stationlist5[i].rssi1 !== undefined)
							html += ', ' + data.stationlist5[i].rssi1;
						if (data.stationlist5[i].rssi2 !== undefined)
							html += ', ' + data.stationlist5[i].rssi2;
						html += '</td>';
						html += '<td style="text-align: center">' + data.stationlist5[i].quality0 + '%';								//QUALITY
						if (data.stationlist5[i].quality1 !== undefined)
							html += ', ' + data.stationlist5[i].quality1 + '%';
						if (data.stationlist5[i].quality2 !== undefined)
							html += ', ' + data.stationlist5[i].quality2 + '%';
						html += '</td>';
						html += '<td style="text-align: center">' + data.stationlist5[i].rxbytes +  '/ ' +  data.stationlist5[i].txbytes + '</td>';	//RX/TX BYTES
						html += '<td style="text-align: center"><input type="button" id="disconnect" value="disconnect" onClick="disconnectStation(this.form, ' + "'" + data.stationlist5[i].mac + "');" +'"></td>';	//DISCONNECT
						html += '</tr>';
					}
				}				
				html +=	'</table>';
				document.getElementById('stationList').innerHTML = html; 
			}
			
			function disconnectStation(form, mac)
			{
				form.disconnectSta.value = mac;
				form.submit();
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
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form name="sta" action="/goform/disconnectSta" method="POST">
					<div id="stationListData" style="display: none;"></div>
					<div id="stationList"></div>
					<table class="button">
						<tr>
							<td><input type="hidden" name="disconnectSta" value="" />
								<input type="button" id="disconnectAll" value="Disconnect all" class="normal" onClick="disconnectStation(this.form, '*');">
								<input type="hidden" name="submit-url" value="/wireless/stainfo.asp" >
							</td>
						</tr>
					</table>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>
