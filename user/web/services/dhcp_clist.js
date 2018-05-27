var clist = JSON.parse('<% getDhcpCliList(); %>');

if (clist.data.length > 0) {
	var html =	'<br>' +
			'<table class="form">' +
			'	<col style="width: 22%" />' +
			'	<col style="width: 22%" />' +
			'	<col style="width: 22%" />' +
			'	<col style="width: 22%" />' +
			'	<col style="width: 12%" />' +
			'	<tbody>' +
			'		<tr><td colspan="5" class="title">' + _('services dhcp clients') + '</td></tr>' +
			'		<tr>' +
			'			<td class="head" style="text-align: left;">' + _('inet hostname') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('inet mac') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('inet ip') + '</td>' +
			'			<td class="head" style="text-align: left;">' + _('services dhcp expires') + '</td>' +
			'			<td class="head" align="center">' + _('services dhcp static') + '</td>' +
			'		</tr>';
	for (var i = 0; i < clist.data.length; i++)
		html +=	'		<tr>' +
			'			<td id="dhclient_row' + i + '_host" style="text-align: left;">' + clist.data[i].hostname + '</td>' +
			'			<td id="dhclient_row' + i + '_mac" style="text-align: left;">' + clist.data[i].mac + '</td>' +
			'			<td id="dhclient_row' + i + '_ip" style="text-align: left;">' + clist.data[i].ip + '</td>' +
			'			<td style="text-align: left;">' + clist.data[i].expire + '</td>' +
			'			<td id="dhclient_row' + i + '_status" style="text-align: center;"><input id="dhclient_row' + i + '" type="checkbox" onchange="toggleDhcpTable(this);"></td>' +
			'		</tr>';
	html +=		'	</tbody>' +
			'</table>';
	document.getElementById('dhcpClientsTable').innerHTML = html;
}
else
	document.getElementById('dhcpClientsTable').innerHTML = '';

updateDhcpClientsList(document.getElementById('dhcpClientsTable'));