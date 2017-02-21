document.getElementById('statusSDKversion_value').innerHTML		= '<% getSdkVersion(); %>';
document.getElementById('statusSysDateTime_value').innerHTML	= '<% getSysDateTime(); %>';
document.getElementById('statusSysUpTime_value').innerHTML		= getUptime();
document.getElementById('statusSysPlatform_value').innerHTML	= '<% getPlatform(); %>';
document.getElementById('statusOpMode_value').innerHTML			= getOpMode();

document.getElementById('statusConnectedType_value').innerHTML	= '<% getCfgGeneral(1, "wanConnectionMode"); %>';
document.getElementById('statusINTIPAddr_value').innerHTML		= '<% getIntIp(); %>';
document.getElementById('statusWANIPAddr_value').innerHTML		= '<% getWanIp(); %>';
document.getElementById('statusSubnetMask_value').innerHTML		= '<% getWanNetmask(); %>';
document.getElementById('statusDefaultGW_value').innerHTML		= '<% getWanGateway(); %>';
document.getElementById('statusPrimaryDNS_value').innerHTML		= '<% getDns(1); %>';
document.getElementById('statusSecondaryDNS_value').innerHTML	= '<% getDns(2); %>';
document.getElementById('statusIPv6IntAddr_value').innerHTML	= '<% getIPv6IntAddr(); %>';
document.getElementById('statusIPv6ExtAddr_value').innerHTML	= '<% getIPv6ExtAddr(); %>';
document.getElementById('statusWANMAC_value').innerHTML			= '<% getWanMac(); %>';

document.getElementById('statusLANIPAddr_value').innerHTML		= '<% getLanIp(); %>';
document.getElementById('statusLANNetmask_value').innerHTML		= '<% getLanNetmask(); %>';
document.getElementById('statusLANMAC_value').innerHTML			= '<% getLanMac(); %>';

var mem_total	= '<% getMemTotalASP(); %>';
var mem_free	= '<% getMemLeftASP(); %>';

document.getElementById('statisticMMTotal_value').innerHTML		= +mem_total + _("statistic memory kib");
document.getElementById('statisticMMLeft_value').innerHTML		= +mem_free + _("statistic memory kib") + ' (' + (+mem_free / (+mem_total / 100)).toFixed(0) + '%)';
document.getElementById('statisticCpuUse_value').innerHTML		= '<% getCpuUsageASP(); %>';

displayElement('statusINTIPAddr_tr',	document.getElementById('statusINTIPAddr_value').innerHTML != document.getElementById('statusWANIPAddr_value').innerHTML);
if (NVRAM_OperationMode == 0) {
	showElement('statusINTIPAddr_tr');
	if (document.getElementById('statusINTIPAddr_value').innerHTML == document.getElementById('statusWANIPAddr_value').innerHTML)
		hideElement('statusWANIPAddr_tr');
}
displayElement('statusDefaultGW_tr',	'<% getWanGateway(); %>' != '');
displayElement('statusIPv6IntAddr_tr',	'<% getIPv6Built(); %>' == '1' && NVRAM_IPv6OpMode != '0');
displayElement('statusIPv6ExtAddr_tr',	'<% getIPv6Built(); %>' == '1' && NVRAM_IPv6OpMode != '0');
displayElement('statusPrimaryDNS_tr',	document.getElementById('statusPrimaryDNS_value').innerHTML != '');
displayElement('statusSecondaryDNS_tr',	document.getElementById('statusSecondaryDNS_value').innerHTML != '');
displayElement([ 'statusLocalNet_tr', 'statusLANIPAddr_tr', 'statusLANNetmask_tr', 'statusLANMAC_tr', 'statusConnectedType_tr' ],	NVRAM_OperationMode != '0');

showInterfaces();

ajaxLoadScript('/internet/ethernet-stat.js');

function getUptime() {
	function declOfNum(num, titl)
	{
		cases = [2, 0, 1, 1, 1, 2];
		return titl[ (num % 100 > 4 && num % 100 < 20) ? 2 : cases[ (num % 10 < 5) ? num % 10 : 5] ];
	}
	
	var uptime, text;
	try {
			uptime = JSON.parse('<% getSysUptime(); %>');
	} catch (e) { return; }
	text = uptime.day + " " + declOfNum(uptime.day, [ _("status system up time day1"), _("status system up time day2_4"), _("status system up time day5_0") ]);
	text += ", " + uptime.hour + " " + declOfNum(uptime.hour, [ _("status system up time hour1"), _("status system up time hour2_4"), _("status system up time hour5_0") ]);
	text += ", " + uptime.min + " " + declOfNum(uptime.min, [ _("status system up time min1"), _("status system up time min2_4"), _("status system up time min5_0") ]);
	text += ", " + uptime.sec + " " + declOfNum(uptime.sec, [ _("status system up time sec1"), _("status system up time sec2_4"), _("status system up time sec5_0") ]);
	return text;
}

function getOpMode() {
	switch ('<% getCfgZero(1, "OperationMode"); %>') {
		case '0':		return _("opmode mode bridge");			break;
		case '1':		return _("opmode mode gateway");		break;
		case '2':		return _("opmode mode e");				break;
		case '3':		return _("opmode mode a");				break;
		default:		return 'Unknown';						break;
	}
}

function getRXTX(rxtx) {
	if (rxtx > 1 * 1024 * 1024 * 1024)
		return (rxtx / (1024 * 1024 * 1024)).toFixed(1) + _("ethernet gibits");
	else if (rxtx > 1 * 1024 * 1024)
		return (rxtx / (1024 * 1024)).toFixed(1) + _("ethernet mibits");
	else if (rxtx > 1 * 1024)
		return (rxtx / 1024).toFixed(1) + _("ethernet kibits");
	else
		return rxtx;
}

function showInterfaces() {
	var html = "";
	
	try {
		var data = JSON.parse('<% getAllNICStatisticASP(); %>');
	} catch (e) { return; }

	html += '<table class="form">';
	html +=	'	<col style="width: 40%;" />';
	html += '	<col style="width: 15%;" />';
	html += '	<col style="width: 15%;" />';
	html += '	<col style="width: 15%;" />';
	html += '	<col style="width: 15%;" />';
	html += '	<tbody>';
	html += '		<tr>';
	html += '			<td class="title" colspan="5">' + _("statistic all interface") + '</td>';
	html += '		</tr>';
	html += '		<tr>';
	html += '			<th>' + _("statistic interface name") + '</th>';
	html += '			<th>' + _("statistic rx pkt") + '</th>';
	html += '			<th>' + _("statistic rx bytes") + '</th>';
	html += '			<th>' + _("statistic tx pkt") + '</th>';
	html += '			<th>' + _("statistic tx bytes") + '</th>';
	html += '		</tr>';

	for (var i = 0; i < data.iface.length; i++) {
		html += '<tr>';
		html += '	<td class="head">' + data.iface[i].name + '</td><td>' + data.iface[i].rx_packet + '</td><td>' + getRXTX(data.iface[i].rx_bytes) + '</td><td>' + data.iface[i].tx_packet + '</td><td>' + getRXTX(data.iface[i].tx_bytes) + '</td>';
		html += '</tr>';
	}

	html += '	</tbody>';
	html += '</table>';

	document.getElementById('statisticSWStats').innerHTML = html;
}

