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

				document.getElementById('l2tp_eth_enabled').value		= NVRAM_l2tp_eth_enabled;
				document.getElementById('l2tp_eth_tid').value			= NVRAM_l2tp_eth_tid;
				document.getElementById('l2tp_eth_ptid').value			= NVRAM_l2tp_eth_ptid;
				document.getElementById('l2tp_eth_sid').value			= NVRAM_l2tp_eth_sid;
				document.getElementById('l2tp_eth_psid').value			= NVRAM_l2tp_eth_psid;
				document.getElementById('l2tp_eth_sport').value			= NVRAM_l2tp_eth_sport;
				document.getElementById('l2tp_eth_dport').value			= NVRAM_l2tp_eth_dport;
				document.getElementById('l2tp_eth_brifs').value			= NVRAM_l2tp_eth_brifs;

				displayServiceStatus([[ NVRAM_l2tp_eth_enabled, 'ethtun' ]]);

				// Enable/disable
				showWarning();
				initTranslation();

				updateVisibility(document.l2tpv3Config);
			}

			// Set inintal values
			function updateVisibility(form) {
				l2tp_eth_enabled = form.l2tp_eth_enabled.value == "1";
				displayElement('l2tp_eth_tid_tr',   l2tp_eth_enabled);
				displayElement('l2tp_eth_ptid_tr',  l2tp_eth_enabled);
				displayElement('l2tp_eth_sid_tr',   l2tp_eth_enabled);
				displayElement('l2tp_eth_psid_tr',  l2tp_eth_enabled);
				displayElement('l2tp_eth_sport_tr', l2tp_eth_enabled);
				displayElement('l2tp_eth_dport_tr', l2tp_eth_enabled);
				displayElement('l2tp_eth_brifs_tr', l2tp_eth_enabled);
				
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
			function CheckValues() {
				if (document.getElementById('l2tp_eth_enabled').value != "0") {
					// Check Tunnel ID
					if(!validateNum(document.getElementById('l2tp_eth_tid').value)) {
						alert(_("services l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_tid').select();
						document.getElementById('l2tp_eth_tid').focus();
						return false;
					}

					// Check Peer Tunnel ID
					if(!validateNum(document.getElementById('l2tp_eth_ptid').value)) {
						alert(_("services l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_ptid').select();
						document.getElementById('l2tp_eth_ptid').focus();
						return false;
					}

					// Check Session ID
					if(!validateNum(document.getElementById('l2tp_eth_sid').value)) {
						alert(_("services l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_sid').select();
						document.getElementById('l2tp_eth_sid').focus();
						return false;
					}

					// Check Peer Session ID
					if(!validateNum(document.getElementById('l2tp_eth_psid').value)) {
						alert(_("services l2tpv3 invalid id"));
						document.getElementById('l2tp_eth_psid').select();
						document.getElementById('l2tp_eth_psid').focus();
						return false;
					}

					// Check Source Port
					if(!validateNum(document.getElementById('l2tp_eth_sport').value)) {
						alert(_("services l2tpv3 invalid port"));
						document.getElementById('l2tp_eth_sport').select();
						document.getElementById('l2tp_eth_sport').focus();
						return false;
					}

					// Check Destination Port
					if(!validateNum(document.getElementById('l2tp_eth_dport').value)) {
						alert(_("services l2tpv3 invalid port"));
						document.getElementById('l2tp_eth_dport').select();
						document.getElementById('l2tp_eth_dport').focus();
						return false;
					}

					// Check Interfaces
					eoip_brifs = NVRAM_eoip_brifs.toLowerCase().split(" ");
					brifs = document.getElementById('l2tp_eth_brifs').value.toLowerCase().split(" ");
					if (eoip_brifs.length == 1 && eoip_brifs[0] == "") eoip_brifs = [];
					if (brifs.length == 1 && brifs[0] == "") brifs = [];

					if (brifs.length == 0) {
						alert(_("services l2tpv3 invalid brifs"));
						document.getElementById('l2tp_eth_brifs').select();
						document.getElementById('l2tp_eth_brifs').focus();
						return false;
					}

					if (brifsListDupes(brifs))
					{
						alert(_("services l2tpv3 brifs duplicate"));
						document.getElementById('l2tp_eth_brifs').select();
						document.getElementById('l2tp_eth_brifs').focus();
						return false;
					}

					if (NVRAM_eoip_enabled == "1" && brifsListDupes(brifs, eoip_brifs))
					{
						alert(_("services l2tpv3 brifs in use"));
						document.getElementById('l2tp_eth_brifs').select();
						document.getElementById('l2tp_eth_brifs').focus();
						return false;
					}
				}

				ajaxShowTimer(document.l2tpv3Config, 'timerReloader', _('message apply'), 10);
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
				<td><h1 id="l2tpv3ServerTitle" data-tr="services l2tpv3 title">L2TPv3 Server Setup</h1>
					<p id="l2tpv3ServerIntroduction" data-tr="services l2tpv3 introduction" >Here you can configure L2TPv3 server settings.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="l2tpv3Config" action="/goform/l2tpv3Config">
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" id="l2tpv3ServerSetup" data-tr="services l2tpv3 config">L2TPv3 Server Setup</td>
								</tr>
								<tr id="ethtun">
									<td class="head" id="l2tpv3Enabled" data-tr="services l2tpv3 enable">L2TPv3 server</td>
									<td>
										<select id="l2tp_eth_enabled" name="l2tp_eth_enabled" class="mid" onChange="updateVisibility(this.form)">
											<option value="0" data-tr="button disable">Disable</option>
											<option value="1" data-tr="button enable">Enable</option>
										</select>
									</td>
									<td style="text-align: center;">&nbsp;</td>
								</tr>
								<tr id="l2tp_eth_tid_tr">
									<td class="head" data-tr="services l2tpv3 tid">Tunnel ID</td>
									<td colspan="2">
										<input id="l2tp_eth_tid" name="l2tp_eth_tid" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="l2tp_eth_ptid_tr">
									<td class="head" data-tr="services l2tpv3 ptid">Peer Tunnel ID</td>
									<td colspan="2">
										<input id="l2tp_eth_ptid" name="l2tp_eth_ptid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_sid_tr">
									<td class="head" data-tr="services l2tpv3 sid">Session ID</td>
									<td colspan="2">
										<input id="l2tp_eth_sid" name="l2tp_eth_sid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_psid_tr">
									<td class="head" data-tr="services l2tpv3 psid">Peer Session ID</td>
									<td colspan="2">
										<input id="l2tp_eth_psid" name="l2tp_eth_psid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_sport_tr">
									<td class="head" data-tr="services l2tpv3 source port">Source Port</td>
									<td colspan="2">
										<input id="l2tp_eth_sport" name="l2tp_eth_sport" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_dport_tr">
									<td class="head" data-tr="services l2tpv3 dest port">Destination Port</td>
									<td colspan="2">
										<input id="l2tp_eth_dport" name="l2tp_eth_dport" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="l2tp_eth_brifs_tr">
									<td class="head"data-tr="services l2tpv3 brifs">Tunneling bridged interfaces</td>
									<td colspan="2">
										<input id="l2tp_eth_brifs" name="l2tp_eth_brifs" class="mid" maxlength="15">
									</td>
								</tr>

							</tbody>
						</table>
						<table class="buttons">
							<tr>
								<td>
									<input type="submit" class="normal" data-tr="button apply"  value="Apply"  onClick="return CheckValues();">
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