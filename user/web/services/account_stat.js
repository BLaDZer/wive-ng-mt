var stat	= JSON.parse('<% iptStatList(); %>');
var html =  '<br>' +
			'<table class="form">';


if (stat.data.length > 0) {
	if (stat.data[0].proto.length == 0)
		html +=	'	<col style="width: 30%" />' +
				'	<col style="width: 15%" />' +
				'	<col style="width: 15%" />' +
				'	<col style="width: 15%" />' +
				'	<col style="width: 15%" />' +
				'	<col style="width: 10%" />';
	else
		html +=	'	<col style="width: 30%" />' +
				'	<col style="width: 10%" />' +
				'	<col style="width: 13%" />' +
				'	<col style="width: 12%" />' +
				'	<col style="width: 13%" />' +
				'	<col style="width: 12%" />' +
				'	<col style="width: 10%" />';

	html +=	'	<tbody>' +
			'		<tr>' +
			'			<td colspan="' + ((stat.data[0].proto.length == 0) ? 6 : 7) + '" class="title">' + _("services account table") + '</td>' +
			'		</tr>' +
			'		<tr>' +
			'			<td class="head" style="text-align: left;">' + _('services account ip addr') + '</td>' +
			((stat.data[0].proto.length == 0) ? '' : '			<td class="head" style="text-align: left;">' + _('services account proto') + '</td>') +
			'			<td class="head" style="text-align: left;">' + _('services account txbytes') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('services account txpackets') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('services account rxbytes') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('services account rxpackets') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('services account time') + '</td>' +
			'		</tr>';

	if (stat.data[0].proto.length == 0) {
		for (var i = 0; i < stat.data.length; i++)
			html += '		<tr>' +
					'			<td style="text-align: left;">' + stat.data[i].ip + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].tx_bytes + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].tx_packets + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].rx_bytes + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].rx_packets + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].time + '</td>' +
					'		</tr>';
	}
	else {
			html += '		<tr>' +
					'			<td style="text-align: left;">' + stat.data[i].ip + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].proto + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].tx_bytes + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].tx_packets + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].rx_bytes + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].rx_packets + '</td>' +
					'			<td style="text-align: left;">' + stat.data[i].time + '</td>' +
					'		</tr>';
	}
}
else {
	html += '	<tbody>' +
			'		<tr>' +
			'			<td class="title">' + _("services account table") + '</td>' +
			'		</tr>' +
			'		<tr>' +
			'			<td style="text-align: left;">' + _("services account no statistic") + '</td>' +
			'		</tr>';
}

html +=	'	</tbody>' +
	'</table>';

document.getElementById('accountStatTable').innerHTML = html;