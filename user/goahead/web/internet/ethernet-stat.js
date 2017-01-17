function portName(num) {
	switch (ports) {
		case 1:
					return "LAN";
					break;
		case 2:
					if (opmode == 1) {
						if (num == wan_port)
							return "WAN";
						else
							return "LAN";
					}
					else {
						if (num == 4) num = 1;
						return "LAN " + (num + 1);
					}
					break;
		default:
				if (num == wan_port && opmode == 1) return "WAN";
				if (num == sip_port) return "SIP";
				if (num == stb_port) return "TV";
				return (lan_port == 'near') ? "LAN " + (ports + first_port - 1 - num) : "LAN " + (num + 1);
	}
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
var html =	'' +
			'<table class="form">' +
			'	<col style="width: 40%;" />';
for (i = first_port; i < first_port + ports; i++)
	html +=	'	<col style="width: ' + ((100 - 40) / ports).toFixed(0) + '%;  text-align: center;" />';

html	+=	'	<tbody>' +
			'		<tr>' +
			'			<td class="title" colspan="' + 6 + '">' + _("ethernet port status head") + '</td>' +
			'		</tr>' +
			'		<tr>' +
			'			<td class="head">' + _("ethernet port status") + '</td>';
if (ports == 2) {
		html +=	'<td style="text-align: center;">' + portName(0) + '<br>' + portImg(0) + '<br>' + portLink(0) + '</td>';
		html +=	'<td style="text-align: center;">' + portName(4) + '<br>' + portImg(4) + '<br>' + portLink(4) + '</td>';
}
else
	for (i = first_port; i < first_port + ports; i++)
		html +=	'<td style="text-align: center;">' + portName(i) + '<br>' + portImg(i) + '<br>' + portLink(i) + '</td>';

html	+=	'		</tr>' +
			'		<tr>' +
			'			<td class="head">' + _("ethernet port rx bytes") + '</td>';

if (ports == 2) {
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.rx_bytes[0]) + '</td>';
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.rx_bytes[4]) + '</td>';
}
else
	for (i = first_port; i < first_port + ports; i++)
		html +=	'<td style="text-align: center;">' + portRXTX(portCount.rx_bytes[i]) + '</td>';
		             
html	+=	'		</tr>' +
			'		<tr>' +
			'			<td class="head">' + _("ethernet port tx bytes") + '</td>';
if (ports == 2) {
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.tx_bytes[0]) + '</td>';
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.tx_bytes[4]) + '</td>';
}
else
	for (i = first_port; i < first_port + ports; i++)
		html +=	'<td  style="text-align: center;">' + portRXTX(portCount.tx_bytes[i]) + '</td>';

html	+=	'		</tr>' +
			'	</tbody>' +
			'</table>';

document.getElementById('ethernetStatus').innerHTML = html;
