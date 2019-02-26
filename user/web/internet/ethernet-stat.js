function portName(num) {
	switch (ETHER_PORTS) {
		case 1:
					return "LAN";
					break;
		case 2:
					if (NVRAM_OperationMode == '1') {
						if (num == NVRAM_wan_port)
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
				if (num == NVRAM_wan_port && NVRAM_OperationMode == '1') return "WAN";
				if (num == NVRAM_sip_port) return "SIP";
				if (num == NVRAM_stb_port) return "TV";
				if (NVRAM_OperationMode != '1' && ETHER_PORTS == 3) return "LAN " + ((4 - num == 0) ? '(WAN)' : (4 - num));
				if (NVRAM_wan_port == '0') {
					var num_near = (num == 0 || num == 5) ? '(WAN)' : num;
					var num_dist = (ETHER_PORTS + ETHER_FIRST_PORT - num == 0 || ETHER_PORTS + ETHER_FIRST_PORT - num == 5) ? '(WAN)' : (ETHER_PORTS + ETHER_FIRST_PORT - num);
					return (NVRAM_lan_port == 'near') ? "LAN " + num_near : "LAN " + num_dist;
				}
				else {
					var num_near = (ETHER_PORTS + ETHER_FIRST_PORT - num - 1 == 0 || ETHER_PORTS + ETHER_FIRST_PORT - num - 1 == 5) ? '(WAN)' : (ETHER_PORTS + ETHER_FIRST_PORT - num - 1);
					var num_dist = (num + 1 == 0 || num + 1== 5) ? '(WAN)' : (num + 1);
					return (NVRAM_lan_port == 'near') ? "LAN " + num_near : "LAN " + num_dist;
				}
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
			'<table style="width: 100%;">' +
			'	<col style="width: 40%;" />';
for (i = ETHER_FIRST_PORT; i < ETHER_FIRST_PORT + ETHER_PORTS; i++)
	html +=	'	<col style="width: ' + ((100 - 40) / ETHER_PORTS).toFixed(0) + '%;  text-align: center;" />';

html	+=	'	<tbody>' +
			'		<tr>' +
			'			<td class="head">' + _("ethernet port status") + '</td>';
if (ETHER_PORTS == 2) {
		html +=	'<td style="text-align: center;">' + portName(0) + '<br>' + portImg(0) + '<br>' + portLink(0) + '</td>';
		html +=	'<td style="text-align: center;">' + portName(4) + '<br>' + portImg(4) + '<br>' + portLink(4) + '</td>';
}
else
	for (i = ETHER_FIRST_PORT; i < ETHER_FIRST_PORT + ETHER_PORTS; i++)
		html +=	'<td style="text-align: center;">' + portName(i) + '<br>' + portImg(i) + '<br>' + portLink(i) + '</td>';

html	+=	'		</tr>' +
			'		<tr>' +
			'			<td class="head">' + _("ethernet port rx bytes") + '</td>';

if (ETHER_PORTS == 2) {
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.rx_bytes[0]) + '</td>';
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.rx_bytes[4]) + '</td>';
}
else
	for (i = ETHER_FIRST_PORT; i < ETHER_FIRST_PORT + ETHER_PORTS; i++)
		html +=	'<td style="text-align: center;">' + portRXTX(portCount.rx_bytes[i]) + '</td>';
		             
html	+=	'		</tr>' +
			'		<tr>' +
			'			<td class="head">' + _("ethernet port tx bytes") + '</td>';
if (ETHER_PORTS == 2) {
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.tx_bytes[0]) + '</td>';
	html +=	'<td style="text-align: center;">' + portRXTX(portCount.tx_bytes[4]) + '</td>';
}
else
	for (i = ETHER_FIRST_PORT; i < ETHER_FIRST_PORT + ETHER_PORTS; i++)
		html +=	'<td  style="text-align: center;">' + portRXTX(portCount.tx_bytes[i]) + '</td>';

html	+=	'		</tr>' +
			'	</tbody>' +
			'</table>';

document.getElementById('ethernetStatus').innerHTML = html;
