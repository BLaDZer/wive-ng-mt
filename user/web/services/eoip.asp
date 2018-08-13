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
				document.getElementById('eoip_enabled').value			= NVRAM_eoip_enabled;
				document.getElementById('eoip_local').value			= NVRAM_eoip_local;
				document.getElementById('eoip_remote').value			= NVRAM_eoip_remote;
				document.getElementById('eoip_tid').value			= NVRAM_eoip_tid;
				document.getElementById('eoip_brifs').value			= NVRAM_eoip_brifs;

				displayServiceStatus([[ NVRAM_eoip_enabled, 'ethtun' ]]);

				// Enable/disable
				showWarning();
				initTranslation();

				updateVisibility(document.eoipConfig);
			}

			// Set inintal values
			function updateVisibility(form) {
				eoip_enabled = form.eoip_enabled.value == "1";
				displayElement('eoip_local_tr', eoip_enabled);
				displayElement('eoip_remote_tr', eoip_enabled);
				displayElement('eoip_tid_tr', eoip_enabled);
				displayElement('eoip_brifs_tr', eoip_enabled);
				
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
				if (document.getElementById('eoip_enabled').value != "0") {
					// Check Remote IP
					if (!validateIP(document.getElementById('eoip_remote'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('eoip_remote').select();
						document.getElementById('eoip_remote').focus();
						return false;
					}

					// Check Local IP
					if (document.getElementById('eoip_local').value != "" && !validateIP(document.getElementById('eoip_local'))) {
						alert(_("services dhcp invalid ip"));
						document.getElementById('eoip_local').select();
						document.getElementById('eoip_local').focus();
						return false;
					}

					// Check Tunnel ID
					if(!validateNum(document.getElementById('eoip_tid').value)) {
						alert(_("services eoip invalid tid"));
						document.getElementById('eoip_tid').select();
						document.getElementById('eoip_tid').focus();
						return false;
					}

					// Check Interfaces
					if (document.getElementById('eoip_brifs').value == "") {
						alert(_("services eoip invalid brifs"));
						document.getElementById('eoip_brifs').select();
						document.getElementById('eoip_brifs').focus();
						return false;
					}

					// Check Interfaces
					l2tp_eth_brifs = NVRAM_l2tp_eth_brifs.toLowerCase().split(" ");
					brifs = document.getElementById('eoip_brifs').value.toLowerCase().split(" ");
					if (l2tp_eth_brifs.length == 1 && l2tp_eth_brifs[0] == "") l2tp_eth_brifs = [];
					if (brifs.length == 1 && brifs[0] == "") brifs = [];

					if (brifs.length == 0) {
						alert(_("services eoip invalid brifs"));
						document.getElementById('eoip_brifs').select();
						document.getElementById('eoip_brifs').focus();
						return false;
					}

					if (brifsListDupes(brifs))
					{
						alert(_("services eoip brifs duplicate"));
						document.getElementById('eoip_brifs').select();
						document.getElementById('eoip_brifs').focus();
						return false;
					}

					if (NVRAM_l2tp_eth_enabled == "1" && (brifsListDupes(brifs, l2tp_eth_brifs)))
					{
						alert(_("services eoip brifs in use"));
						document.getElementById('eoip_brifs').select();
						document.getElementById('eoip_brifs').focus();
						return false;
					}

				}

				ajaxShowTimer(document.eoipConfig, 'timerReloader', _('message apply'), 10);
				return true;
			}

			//
			function updateVisiblity() {
			}


			// Hide Hiht block
			function hideHint() {
				document.getElementById('eoip_hint_row').innerHTML = '';
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
				document.getElementById('eoip_hint_row').innerHTML = text;
			}

		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 id="eoipServerTitle" data-tr="services eoip title">EOIP Server Setup</h1>
					<p id="eoipServerIntroduction" data-tr="services eoip introduction">Here you can configure EOIP server settings.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="eoipConfig" action="/goform/eoipConfig">
						<table class="form">
							<col style="width: 40%;" />
							<col style="width: 50%;" />
							<col style="width: 10%;" />
							<tbody>
								<tr>
									<td class="title" colspan="3" id="eoipServerSetup" data-tr="services eoip config">EOIP Server Setup</td>
								</tr>
								<tr id="ethtun">
									<td class="head" id="eoipEnabled" data-tr="services eoip enable">EOIP server</td>
									<td>
										<select id="eoip_enabled" name="eoip_enabled" class="mid" onChange="updateVisibility(this.form)">
											<option value="0" data-tr="button disable">Disable</option>
											<option value="1" data-tr="button enable">Enable</option>
										</select>
									</td>
									<td style="text-align: center;">&nbsp;</td>
								</tr>
								<tr id="eoip_local_tr">
									<td class="head" data-tr="services eoip local">Local IP to use</td>
									<td colspan="2">
										<input id="eoip_local" name="eoip_local" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="eoip_remote_tr">
									<td class="head" data-tr="services eoip remote">Remote IP</td>
									<td colspan="2">
										<input id="eoip_remote" name="eoip_remote" class="mid" maxlength="15">
									</td>
								</tr>
								<tr id="eoip_tid_tr">
									<td class="head" data-tr="services eoip tid">Tunnel ID</td>
									<td colspan="2">
										<input id="eoip_tid" name="eoip_tid" class="mid" maxlength="15">
									</td>
								</tr>

								<tr id="eoip_brifs_tr">
									<td class="head" data-tr="services eoip brifs">Tunneling bridged interfaces</td>
									<td colspan="2">
										<input id="eoip_brifs" name="eoip_brifs" class="mid" maxlength="15">
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
					<div id="eoip_hint_row"></div>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>