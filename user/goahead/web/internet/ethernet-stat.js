function portName(num) {
	if (num == wan_port) return "WAN";
	if (num == sip_port) return "SIP";
	if (num == stb_port) return "TV";
	return (lan_port == 'near') ? "LAN " + (4 - num) : "LAN " + (num + 1);
}

function portImg(num) {
	if (portStatus.ethernet[num].link == '1')
		return '<img src="/graphics/' + portStatus.ethernet[num].speed + '_' + portStatus.ethernet[num].duplex.toLowerCase() + '.gif">';
	else
		return '<img width="31px" height="31px" src="/graphics/empty.gif">';
}

function portLink(num) {
	return (portStatus.ethernet[num].link == '1') ? (portStatus.ethernet[num].duplex == 'F') ? portStatus.ethernet[num].speed + ' FD' : portStatus.ethernet[num].speed + ' HD' : '-';
}

function portRXTX(rxtx) {
	if (rxtx > 1 * 1024 * 1024 * 1024)
		return (rxtx / (1024 * 1024 * 1024)).toFixed(1) + _("ethernet gibits");
	else if (rxtx > 1 * 1024 * 1024)
		return (rxtx / (1024 * 1024)).toFixed(1) + _("ethernet mibits");
	else if (rxtx > 1 * 1024)
		return (rxtx / 1024).toFixed(1) + _("ethernet kibits");
	else 
		return rxtx;
}

var portStatus	= JSON.parse('<% getPortStatus(); %>');
var portCount	= JSON.parse('<% getHWStatistic(); %>');
var i;
var html		= "";

html	+=	'<table class="form">';
html	+=	'	<col style="width: 40%;" />';
html	+=	'	<col style="width: 12%;  text-align: center;" />';
html	+=	'	<col style="width: 12%;  text-align: center;" />';
html	+=	'	<col style="width: 12%;  text-align: center;" />';
html	+=	'	<col style="width: 12%;  text-align: center;" />';
html	+=	'	<col style="width: 12%;  text-align: center;" />';
html	+=	'	<tbody>';
html	+=	'		<tr>';
html	+=	'			<td class="title" colspan="6">' + _("ethernet port status head") + '</td>';
html	+=	'		</tr>';

if (gwb == '1') {
	html	+=	'		<tr>';
	html	+=	'			<td class="head">' + _("ethernet port status") + '</td><td style="text-align: center;">' + portName(0) + '<br>' + portImg(0) + '<br>' + portLink(0) + '</td>' +
																				  '<td style="text-align: center;">' + portName(1) + '<br>' + portImg(1) + '<br>' + portLink(1) + '</td>' +
																				  '<td style="text-align: center;">' + portName(2) + '<br>' + portImg(2) + '<br>' + portLink(2) + '</td>' +
																				  '<td style="text-align: center;">' + portName(3) + '<br>' + portImg(3) + '<br>' + portLink(3) + '</td>' +
																				  '<td style="text-align: center;">' + portName(4) + '<br>' + portImg(4) + '<br>' + portLink(4) + '</td>';
	html	+=	'		</tr>';
	html	+=	'		<tr>';
	html	+=	'			<td class="head">' + _("ethernet port rx bytes") + '</td><td style="text-align: center;">' + portRXTX(portCount.rx_bytes[0]) + '</td><td style="text-align: center;">' + portRXTX(portCount.rx_bytes[1]) + '</td><td style="text-align: center;">' + portRXTX(portCount.rx_bytes[2]) + '</td><td style="text-align: center;">' + portRXTX(portCount.rx_bytes[3]) + '</td><td style="text-align: center;">' + portRXTX(portCount.rx_bytes[4]) + '</td>';
	html	+=	'		</tr>';
	html	+=	'		<tr>';
	html	+=	'			<td class="head">' + _("ethernet port tx bytes") + '</td><td style="text-align: center;">' + portRXTX(portCount.tx_bytes[0]) + '</td><td style="text-align: center;">' + portRXTX(portCount.tx_bytes[1]) + '</td><td style="text-align: center;">' + portRXTX(portCount.tx_bytes[2]) + '</td><td style="text-align: center;">' + portRXTX(portCount.tx_bytes[3]) + '</td><td style="text-align: center;">' + portRXTX(portCount.tx_bytes[4]) + '</td>';
	html	+=	'		</tr>';
}
else {
	html	+=	'		<tr>';
	html	+=	'			<td class="head">' + _("ethernet port status") + '</td><td colspan="5" style="text-align: center;">' + portName(0) + '<br>' + portImg(0) + '<br>' + portLink(0) + '</td>';
	html	+=	'		</tr>';
	html	+=	'		<tr>';
	html	+=	'			<td class="head">' + _("ethernet port rx bytes") + '</td><td style="text-align: center;">' + portRXTX(portCount.rx_bytes[0]) + '</td>';
	html	+=	'		</tr>';
	html	+=	'		<tr>';
	html	+=	'			<td class="head">' + _("ethernet port tx bytes") + '</td><td style="text-align: center;">' + portRXTX(portCount.tx_bytes[0]) + '</td>';
	html	+=	'		</tr>';
}

html	+=	'	</tbody>';
html	+=	'</table>';

document.getElementById('ethernetStatus').innerHTML = html;
