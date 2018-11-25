<!DOCTYPE html>
<html>
	<head>
		<title>Internet Services Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_services.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_hint.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>

			var WLAN_IF_INIC = '<% getWlanIf(2); %>';

			function initTranslation()
			{
				var trlist = document.getElementsByAttribute("data-tr");
				for (var i=0; i<trlist.length; i++) {
					var elem = trlist[i];
					_TR(elem, elem.getAttribute("data-tr"));
				}
			}

			// Set inintal values
			function initValues() {

				form = document.getElementById('l2tunnelsConfig');

				form.l2tp_eth_enabled.value		= NVRAM_l2tp_eth_enabled;
				form.l2tp_eth_local.value		= NVRAM_l2tp_eth_local;
				form.l2tp_eth_remote.value		= NVRAM_l2tp_eth_remote;
				form.l2tp_eth_tid.value			= NVRAM_l2tp_eth_tid;
				form.l2tp_eth_ptid.value		= NVRAM_l2tp_eth_ptid;
				form.l2tp_eth_sid.value			= NVRAM_l2tp_eth_sid;
				form.l2tp_eth_psid.value		= NVRAM_l2tp_eth_psid;
				form.l2tp_eth_sport.value		= NVRAM_l2tp_eth_sport;
				form.l2tp_eth_dport.value		= NVRAM_l2tp_eth_dport;
				form.l2tp_eth_brifs.value		= NVRAM_l2tp_eth_brifs;

				form.eoip_enabled.value			= NVRAM_eoip_enabled;
				form.eoip_local.value			= NVRAM_eoip_local;
				form.eoip_remote.value			= NVRAM_eoip_remote;
				form.eoip_tid.value			= NVRAM_eoip_tid;
				form.eoip_brifs.value			= NVRAM_eoip_brifs;

				function populateBrif(brifs, custom, elem) {
					elem.checked = ( brifs.indexOf(elem.value) != -1 );
					var ind = custom.indexOf(elem.value)
					if (ind != -1) custom.splice( ind, 1 );
				}

				if (NVRAM_l2tp_eth_enabled != "0") {
					var l2tp_eth_brifs_list = NVRAM_l2tp_eth_brifs.split(" ");
					var l2tp_eth_brifs_custom = l2tp_eth_brifs_list.slice();
					if (NVRAM_l2tp_eth_brifs == "all") {
						form.l2tp_eth_brifs_all.checked = true;
					}
					else
					{
						populateBrif(l2tp_eth_brifs_list, l2tp_eth_brifs_custom, form.l2tp_eth_brifs_wan);
						populateBrif(l2tp_eth_brifs_list, l2tp_eth_brifs_custom, form.l2tp_eth_brifs_vpn);
						populateBrif(l2tp_eth_brifs_list, l2tp_eth_brifs_custom, form.l2tp_eth_brifs_wlan);
						populateBrif(l2tp_eth_brifs_list, l2tp_eth_brifs_custom, form.l2tp_eth_brifs_wlan_inic);

						form.l2tp_eth_brifs_custom.value = l2tp_eth_brifs_custom.join(" ");
					}
				}

				if (NVRAM_eoip_enabled != "0") {
					var eoip_brifs_list = NVRAM_eoip_brifs.split(" ");
					var eoip_brifs_custom = eoip_brifs_list.slice();

					if (NVRAM_eoip_brifs == "all") {
						form.eoip_brifs_all.checked = true;
					}
					else
					{
						populateBrif(eoip_brifs_list, eoip_brifs_custom, form.eoip_brifs_wan);
						populateBrif(eoip_brifs_list, eoip_brifs_custom, form.eoip_brifs_vpn);
						populateBrif(eoip_brifs_list, eoip_brifs_custom, form.eoip_brifs_wlan);
						populateBrif(eoip_brifs_list, eoip_brifs_custom, form.eoip_brifs_wlan_inic);

						form.eoip_brifs_custom.value = eoip_brifs_custom.join(" ");
					}
				}

				displayServiceStatus([[ NVRAM_l2tp_eth_enabled, 'ethtun_l2tpv3', 'ethtun' ]]);
				displayServiceStatus([[ NVRAM_eoip_enabled,     'ethtun_eoip', 'ethtun' ]]);

				// Enable/disable
				showWarning();
				initTranslation();

				updateVisibility(document.l2tunnelsConfig);
			}

			// Set inintal values
			function updateVisibility(form) {
				l2tp_eth_enabled = form.l2tp_eth_enabled.value == "1";
				displayElement('l2tp_eth_local_tr', l2tp_eth_enabled);
				displayElement('l2tp_eth_remote_tr', l2tp_eth_enabled);
				displayElement('l2tp_eth_tid_tr',   l2tp_eth_enabled);
				displayElement('l2tp_eth_ptid_tr',  l2tp_eth_enabled);
				displayElement('l2tp_eth_sid_tr',   l2tp_eth_enabled);
				displayElement('l2tp_eth_psid_tr',  l2tp_eth_enabled);
				displayElement('l2tp_eth_sport_tr', l2tp_eth_enabled);
				displayElement('l2tp_eth_dport_tr', l2tp_eth_enabled);
				displayElement('l2tp_eth_brifs_tr', l2tp_eth_enabled);

				displayElement(['l2tp_eth_brifs_wlan_inic_div', 'eoip_brifs_wlan_inic_div'], BUILD_5GHZ_SUPPORT && (WLAN_IF_INIC != ''));

				eoip_enabled = form.eoip_enabled.value == "1";
				displayElement('eoip_local_tr', eoip_enabled);
				displayElement('eoip_remote_tr', eoip_enabled);
				displayElement('eoip_tid_tr', eoip_enabled);
				displayElement('eoip_brifs_tr', eoip_enabled);

				displayElement('l2tp_eth_brifs_inner', form.l2tp_eth_brifs_all.checked == false);
				displayElement('eoip_brifs_inner', form.eoip_brifs_all.checked == false);
			}

			function brifsListDupes(brifs1, brifs2) {
				single_brif = false;
				if (brifs2 === undefined)
				{
					brifs2 = brifs1
					single_brif = true;
				}

				for (i in brifs1)
				for (j in brifs2)
				{
					b1 = brifs1[i];
					b2 = brifs2[j]

					if ((!single_brif || i != j) && b1 == b2) return true;

					if (!single_brif && b1 == "all" && brifs2.length > 0) return true;
					if (!single_brif && b2 == "all" && brifs1.length > 0) return true;
				}

				return false;
			}

			// Check values
			function CheckValues(form) {
				l2tp_eth_brifs = form.l2tp_eth_brifs_custom.value.toLowerCase().split(" ");
				eoip_brifs = form.eoip_brifs_custom.value.toLowerCase().split(" ");

				if (l2tp_eth_brifs.length == 1 && l2tp_eth_brifs[0] == "") l2tp_eth_brifs = [];
				if (eoip_brifs.length == 1 && eoip_brifs[0] == "") eoip_brifs = [];

				if (form.l2tp_eth_brifs_all.checked) {
					l2tp_eth_brifs = [ "all" ];
				}
				else
				{
					if (form.l2tp_eth_brifs_wan.checked) l2tp_eth_brifs.push(form.l2tp_eth_brifs_wan.value);
					if (form.l2tp_eth_brifs_vpn.checked) l2tp_eth_brifs.push(form.l2tp_eth_brifs_vpn.value);
					if (form.l2tp_eth_brifs_wlan.checked) l2tp_eth_brifs.push(form.l2tp_eth_brifs_wlan.value);
					if (form.l2tp_eth_brifs_wlan_inic.checked) l2tp_eth_brifs.push(form.l2tp_eth_brifs_wlan_inic.value);
				}

				if (form.eoip_brifs_all.checked) {
					eoip_brifs = [ "all" ];
				}
				else
				{
					if (form.eoip_brifs_wan.checked) eoip_brifs.push(form.eoip_brifs_wan.value);
					if (form.eoip_brifs_vpn.checked) eoip_brifs.push(form.eoip_brifs_vpn.value);
					if (form.eoip_brifs_wlan.checked) eoip_brifs.push(form.eoip_brifs_wlan.value);
					if (form.eoip_brifs_wlan_inic.checked) eoip_brifs.push(form.eoip_brifs_wlan_inic.value);
				}

				form.l2tp_eth_brifs.value = l2tp_eth_brifs.join(' ');
				form.eoip_brifs.value = eoip_brifs.join(' ');

				if (document.getElementById('l2tp_eth_enabled').value != "0") {

					// Check Local IP
					if (document.getElementById('l2tp_eth_local').value != "" && !validateIP(document.getElementById('l2tp_eth_local'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('l2tp_eth_local').select();
						document.getElementById('l2tp_eth_local').focus();
						return false;
					}

					// Check Remote IP
					if (!validateIP(document.getElementById('l2tp_eth_remote'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('l2tp_eth_remote').select();
						document.getElementById('l2tp_eth_remote').focus();
						return false;
					}

					// Check Tunnel ID
					if(!validateNum(document.getElementById('l2tp_eth_tid').value)) {
						alert(_("l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_tid').select();
						document.getElementById('l2tp_eth_tid').focus();
						return false;
					}

					// Check Peer Tunnel ID
					if(!validateNum(document.getElementById('l2tp_eth_ptid').value)) {
						alert(_("l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_ptid').select();
						document.getElementById('l2tp_eth_ptid').focus();
						return false;
					}

					// Check Session ID
					if(!validateNum(document.getElementById('l2tp_eth_sid').value)) {
						alert(_("l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_sid').select();
						document.getElementById('l2tp_eth_sid').focus();
						return false;
					}

					// Check Peer Session ID
					if(!validateNum(document.getElementById('l2tp_eth_psid').value)) {
						alert(_("l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_psid').select();
						document.getElementById('l2tp_eth_psid').focus();
						return false;
					}

					// Check Source Port
					if(!validateNum(document.getElementById('l2tp_eth_sport').value)) {
						alert(_("l2tpv3 invalid port"));
						document.getElementById('l2tp_eth_sport').select();
						document.getElementById('l2tp_eth_sport').focus();
						return false;
					}

					// Check Destination Port
					if(!validateNum(document.getElementById('l2tp_eth_dport').value)) {
						alert(_("l2tpv3 invalid port"));
						document.getElementById('l2tp_eth_dport').select();
						document.getElementById('l2tp_eth_dport').focus();
						return false;
					}

					// Check Interfaces
					if (l2tp_eth_brifs.length == 0) {
						alert(_("l2tpv3 invalid brifs"));
						document.getElementById('l2tp_eth_brifs').select();
						document.getElementById('l2tp_eth_brifs').focus();
						return false;
					}

					if (brifsListDupes(l2tp_eth_brifs))
					{
						alert(_("l2tpv3 brifs duplicate"));
						document.getElementById('l2tp_eth_brifs').select();
						document.getElementById('l2tp_eth_brifs').focus();
						return false;
					}

					if (form.eoip_enabled.value != "0" && brifsListDupes(l2tp_eth_brifs, eoip_brifs))
					{
						alert(_("l2tpv3 brifs in use"));
						document.getElementById('l2tp_eth_brifs').select();
						document.getElementById('l2tp_eth_brifs').focus();
						return false;
					}
				}

				if (form.eoip_enabled.value != "0") {

					// Check Local IP
					if (document.getElementById('eoip_local').value != "" && !validateIP(document.getElementById('eoip_local'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('eoip_local').select();
						document.getElementById('eoip_local').focus();
						return false;
					}

					// Check Remote IP
					if (!validateIP(document.getElementById('eoip_remote'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('eoip_remote').select();
						document.getElementById('eoip_remote').focus();
						return false;
					}

					// Check Tunnel ID
					if(!validateNum(document.getElementById('eoip_tid').value)) {
						alert(_("eoip invalid tid"));
						document.getElementById('eoip_tid').select();
						document.getElementById('eoip_tid').focus();
						return false;
					}

					// Check Interfaces
					if (document.getElementById('eoip_brifs').value == "") {
						alert(_("eoip invalid brifs"));
						document.getElementById('eoip_brifs_custom').select();
						document.getElementById('eoip_brifs_custom').focus();
						return false;
					}

					// Check Interfaces
					if (eoip_brifs.length == 0) {
						alert(_("eoip invalid brifs"));
						document.getElementById('eoip_brifs_custom').select();
						document.getElementById('eoip_brifs_custom').focus();
						return false;
					}

					if (brifsListDupes(eoip_brifs))
					{
						alert(_("eoip brifs duplicate"));
						document.getElementById('eoip_brifs_custom').select();
						document.getElementById('eoip_brifs_custom').focus();
						return false;
					}

					if (form.l2tp_eth_enabled.value != "0" && (brifsListDupes(eoip_brifs, l2tp_eth_brifs)))
					{
						alert(_("eoip brifs in use"));
						document.getElementById('eoip_brifs_custom').select();
						document.getElementById('eoip_brifs_custom').focus();
						return false;
					}
				}

				ajaxShowTimer(document.l2tunnelsConfig, 'timerReloader', _('message apply'), 10);
				return true;
			}

			//
			function updateVisiblity() {
			}


			// Hide Hiht block
			function hideHint() {
				document.getElementById('l2tp_eth_hint_row').innerHTML = '';
			}

			// Show Hint block
			function showHint(key) {
				var text = '<div class="hint"><font color="#0000ff"><b>' + _('hint title') + ':</b></font>&nbsp;';
				var show = true;

				switch (key) {
//					case 'l2tp_enable':	text += _("hint l2tp server");		break;
					default:		return;
				}

				text += '</div>';
				document.getElementById('l2tp_eth_hint_row').innerHTML = text;
			}

		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 data-tr="l2 tunnels title">L2 Tunnels Setup</h1>
					<p data-tr="l2 tunnels introduction" >Here you can configure L2 tunnels settings.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="l2tunnelsConfig" id="l2tunnelsConfig" action="/goform/l2tunnelsConfig">

						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" data-tr="l2tpv3 config">L2TPv3 Setup</td>
								</tr>
								<tr id="ethtun_l2tpv3">
									<td class="head" id="l2tpv3Enabled" data-tr="l2tpv3 enable">L2TPv3</td>
									<td>
										<select id="l2tp_eth_enabled" name="l2tp_eth_enabled" class="mid" onChange="updateVisibility(this.form)">
											<option value="0" data-tr="button disable">Disable</option>
											<option value="1" data-tr="button enable">Enable</option>
										</select>
									</td>
									<td style="text-align: center;">&nbsp;</td>
								</tr>
								<tr id="l2tp_eth_local_tr">
									<td class="head" data-tr="eoip local">Local IP to use</td>
									<td colspan="2">
										<input id="l2tp_eth_local" name="l2tp_eth_local" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="l2tp_eth_remote_tr">
									<td class="head" data-tr="eoip remote">Remote IP</td>
									<td colspan="2">
										<input id="l2tp_eth_remote" name="l2tp_eth_remote" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="l2tp_eth_tid_tr">
									<td class="head" data-tr="l2tpv3 tid">Tunnel ID</td>
									<td colspan="2">
										<input id="l2tp_eth_tid" name="l2tp_eth_tid" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="l2tp_eth_ptid_tr">
									<td class="head" data-tr="l2tpv3 ptid">Peer Tunnel ID</td>
									<td colspan="2">
										<input id="l2tp_eth_ptid" name="l2tp_eth_ptid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_sid_tr">
									<td class="head" data-tr="l2tpv3 sid">Session ID</td>
									<td colspan="2">
										<input id="l2tp_eth_sid" name="l2tp_eth_sid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_psid_tr">
									<td class="head" data-tr="l2tpv3 psid">Peer Session ID</td>
									<td colspan="2">
										<input id="l2tp_eth_psid" name="l2tp_eth_psid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_sport_tr">
									<td class="head" data-tr="l2tpv3 source port">Source Port</td>
									<td colspan="2">
										<input id="l2tp_eth_sport" name="l2tp_eth_sport" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_dport_tr">
									<td class="head" data-tr="l2tpv3 dest port">Destination Port</td>
									<td colspan="2">
										<input id="l2tp_eth_dport" name="l2tp_eth_dport" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_brifs_tr">
									<td class="head"data-tr="l2tpv3 brifs">Tunneling bridged interfaces</td>
									<td colspan="2">
										<div>
											<input name="l2tp_eth_brifs_all"      type="checkbox" value="all" onClick="updateVisibility(this.form)"><span data-tr="l2 tunnels all ifaces">All interfaces</span>
										</div>
										<div id="l2tp_eth_brifs_inner">
										<div>
											<input name="l2tp_eth_brifs_wan"       type="checkbox" value="<% getWanIf(); %>"><span>WAN (<% getWanIf(); %>)</span>
											<input name="l2tp_eth_brifs_vpn"       type="checkbox" value="<% getVpnIf(); %>"><span>VPN (<% getVpnIf(); %>)</span>
										</div>
										<div>
											<input name="l2tp_eth_brifs_wlan"      type="checkbox" value="<% getWlanIf(1); %>"><span>WLAN 2.4 GHz (<% getWlanIf(1); %>)</span>
										</div>
										<div id="l2tp_eth_brifs_wlan_inic_div">
											<input name="l2tp_eth_brifs_wlan_inic" type="checkbox" value="<% getWlanIf(2); %>"><span>WLAN 5 GHz (<% getWlanIf(2); %>)</span>
										</div>
										<div data-tr="l2 tunnels custom brifs">Custom: </div>
										<div><input name="l2tp_eth_brifs_custom"></div>
										</div>
										<div><input id="l2tp_eth_brifs" name="l2tp_eth_brifs" type="hidden"></div>
									</td>
								</tr>

							</tbody>
						</table>

						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" data-tr="eoip config">EOIP Setup</td>
								</tr>
								<tr id="ethtun_eoip">
									<td class="head" id="eoipEnabled" data-tr="eoip enable">EOIP</td>
									<td>
										<select id="eoip_enabled" name="eoip_enabled" class="mid" onChange="updateVisibility(this.form)">
											<option value="0" data-tr="button disable">Disable</option>
											<option value="1" data-tr="button enable">Enable</option>
										</select>
									</td>
									<td style="text-align: center;">&nbsp;</td>
								</tr>
								<tr id="eoip_local_tr">
									<td class="head" data-tr="eoip local">Local IP to use</td>
									<td colspan="2">
										<input id="eoip_local" name="eoip_local" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="eoip_remote_tr">
									<td class="head" data-tr="eoip remote">Remote IP</td>
									<td colspan="2">
										<input id="eoip_remote" name="eoip_remote" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="eoip_tid_tr">
									<td class="head" data-tr="eoip tid">Tunnel ID</td>
									<td colspan="2">
										<input id="eoip_tid" name="eoip_tid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="eoip_brifs_tr">
									<td class="head" data-tr="eoip brifs">Tunneling bridged interfaces</td>
									<td colspan="2">
										<div>
											<input name="eoip_brifs_all"      type="checkbox" value="all" onClick="updateVisibility(this.form)"><span data-tr="l2 tunnels all ifaces">All interfaces</span>
										</div>
										<div id="eoip_brifs_inner">
										<div>
											<input name="eoip_brifs_wan"       type="checkbox" value="<% getWanIf(); %>"><span>WAN (<% getWanIf(); %>)</span>
											<input name="eoip_brifs_vpn"       type="checkbox" value="<% getVpnIf(); %>"><span>VPN (<% getVpnIf(); %>)</span>
										</div>
										<div>
											<input name="eoip_brifs_wlan"      type="checkbox" value="<% getWlanIf(1); %>"><span>WLAN 2.4 GHz (<% getWlanIf(1); %>)</span>
										</div>
										<div id="eoip_brifs_wlan_inic_div">
											<input name="eoip_brifs_wlan_inic" type="checkbox" value="<% getWlanIf(2); %>"><span>WLAN 5 GHz (<% getWlanIf(2); %>)</span>
										</div>
										<div data-tr="l2 tunnels custom brifs">Custom: </div>
										<div><input name="eoip_brifs_custom"></div>
										</div>
										<div><input id="eoip_brifs" name="eoip_brifs" type="hidden"></div>
									</td>
								</tr>

							</tbody>
						</table>

						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" data-tr="button apply"  value="Apply"  onClick="return CheckValues(this.form);">
									<input type="button" class="normal" data-tr="button cancel" value="Cancel" onClick="window.location.reload();">
									<input type="button" class="normal" data-tr="button reset"  value="Reset"  onClick="resetValues(this.form);">
									<input value="0" name="reset" type="hidden">
								</td>
							</tr>
						</table>

					</form>
					<div id="l2tp_eth_hint_row"></div>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>