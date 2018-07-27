<!DOCTYPE html>
<html>
	<head>
		<title>AP Client Feature</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_wireless.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/scanap.js"></script>
		<script>
			var interval;

			function initTranslation() {
				_TR("apcliTitle",				"apcli title");
				_TR("apcliParameters",				"apcli param");
				_TR("apcliAbout",				"apcli about");
				_TR("apcliEnable",				"apcli client enable");
				_TR("apcli_enable_enable",			"button enable");
				_TR("apcli_enable_disable",			"button disable");
				_TR("apcliOpen",				"apcli open");
				_TR("apcliWiFiMode",				"apcli client wifi mode")
				_TR("apcliSSID",				"station ssid");
				_TR("apcliMAC",					"apcli mac");
				_TR("apcliSecurityMode",			"secure security mode");
				_TR("apcliEncryptionType",			"secure encryp type");
				_TR("apcliPass",				"secure wpa pass phrase");
				_TR("apcliShowPass",				"secure wpa show pass phrase");
				_TR("apcliAutoscan",				"apcli enable autoscan");
				_TR("apcliDisableIface",			"apcli disable iface");
				_TR("apcliEnableBridge",			"apcli enable bridge");
				_TR("basicAPCLIMode",				"basic apcli mode");
				_TR("scanapLegendButtonScan",			"scanap legend button scan");
				_TR("apcliApply",				"button apply");
				_TR("apcliCancel",				"button cancel");
				_TR("apcliReset",				"button reset");
			}

			function initValues()
			{
				var form						= document.wireless_apcli;
				document.getElementById('apcli_enable').checked		= NVRAM_ApCliEnable == '1';
				document.getElementById('apcli_ssid').value		= NVRAM_ApCliSsid;
				document.getElementById('apcli_bssid').value		= NVRAM_ApCliBssid;
				document.getElementById('apcli_mode').value		= NVRAM_ApCliAuthMode;
				document.getElementById('apcli_autoscan').checked	= NVRAM_ApCliAutoConnect == '1';
				document.getElementById('apcli_apiface').checked	= NVRAM_ApCliClientOnly == '1';
				document.getElementById('apcli_bridge').checked		= NVRAM_ApCliBridgeOnly == '1';
				document.getElementById('apcli_wpapsk').value		= NVRAM_ApCliWPAPSK;

				var apcli_mode						= NVRAM_ApCliIfName;

				if (!BUILD_5GHZ_SUPPORT || (NVRAM_RadioOn == 0 && NVRAM_RadioOnINIC == 0)) {
					document.getElementById('apcli_interface').value = "apcli0";
					displayElement('apcliWiFiMode_tr', false);
				}
				else {
					document.getElementById('apcli_interface').disabled = (NVRAM_RadioOn == 0) || (NVRAM_RadioOnINIC == 0);
					if (NVRAM_RadioOn == 0) 
						document.getElementById('apcli_interface').value = "apclii0"
					else if (NVRAM_RadioOnINIC == 0)
						document.getElementById('apcli_interface').value = "apcli0"
					else
						document.getElementById('apcli_interface').value = apcli_mode;
				}

				ajaxLoadElement("apcliStatusData", "/wireless/apcli-status.asp", showAPCliStatus);
				interval = setInterval('ajaxLoadElement("apcliStatusData", "/wireless/apcli-status.asp", showAPCliStatus);', 5000);

				// Hide 'SCAN' buttons if IE8 (no SVG support)
				var browser = getBrowser();
				if (browser.browser == 'ie' && browser.versionShort <= 8)
					displayElement( ['scanapLegendButtonScan', 'scanapLegendButtonScanINIC' ], false);

				securityModeSwitch(form);
				apcliEnableSwitch(form);
				showWarning();
				initTranslation();
			}

			function checkValues(form)
			{
				if (form.apcli_enable.checked) {
					if (form.apcli_bssid.value.length > 0) {
						if (!validateMAC(form.apcli_bssid.value, false)) {
							alert(_("apcli no correct bssid"));
							form.apcli_bssid.focus();
							form.apcli_bssid.select();
							return false;
						}
					}

					if ((form.apcli_mode.value == 'WPAPSK') || (form.apcli_mode.value == 'WPA2PSK'))
						if (form.apcli_wpapsk.value.length < 8) {
							alert(_("apcli short phrase"));
							form.apcli_wpapsk.focus();
							form.apcli_wpapsk.select();
							return false;
						} else if (form.apcli_wpapsk.value.length > 64) {
							alert(_("apcli long phrase"));
							form.apcli_wpapsk.focus();
							form.apcli_wpapsk.select();
							return false;
						}
					else
						for (var i = 0; i < form.apcli_wpapsk.value.length; i++)
							if ((form.apcli_wpapsk.value.charCodeAt(i) < 32) || (form.apcli_wpapsk.value.charCodeAt(i) > 126)) {
								alert(_("apcli chars not allowed"));
								form.apcli_wpapsk.focus();
								form.apcli_wpapsk.select();
								return false;
							}
				}

				form.apcli_interface.disabled = false;
				clearInterval(interval);
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 30);
				return true;
			}

			function showAPCliStatus()
			{
					var text;
					var apcliStatus = document.getElementById('apcliStatusData').innerHTML;
					var apcliEnable = '<% getCfgGeneral(1, "ApCliEnable"); %>';

					if (apcliEnable == "0")
						text = "<b><font color=\"#808080\">" + _('apcli status disabled') + "</font></b>";
					else if (parseInt(apcliStatus) < 0)
						text = "<b><font color=\"#FF0000\">" + _('apcli status error') + " (" + apcliStatus + ")</font></b>";
					else if (parseInt(apcliStatus) == 0)
						text = "<b><font color=\"#808080\">" + _('apcli status disconnected') + "</font></b>";
					else
						text = "<b><font color=\"#008000\">" + _('apcli status connected') + " ( " + apcliStatus.toUpperCase() + ")</font></b> ";

					ajaxModifyElementHTML('apcli_status', text);	
			}

			function securityModeSwitch(form)
			{
				var enc_type = '<% getCfgGeneral(1, "ApCliEncrypType"); %>';
				document.getElementById('apcli_enc').value = (enc_type != "") ? "AES" : enc_type;
				displayElement( [ 'div_apcli_enc', 'div_apcli_wpapsk' ], (document.getElementById('apcli_mode').value == 'WPAPSK' || document.getElementById('apcli_mode').value == 'WPA2PSK') && document.getElementById('apcli_enable').checked == 1);
			}

			function apcliEnableSwitch(form)
			{
				displayElement( [ 'apcliWiFiMode_tr', 'apcliSSID_tr', 'apcliMAC_tr',
								  'apcliSecurityMode_tr', 'div_apcli_enc', 'div_apcli_wpapsk',
								  'apcliAutoscan_tr', 'apcliDisableIface_tr', 'apcliEnableBridge_tr' ], document.getElementById('apcli_enable').checked == 1);
				displayElement('apcliWiFiMode_tr', BUILD_5GHZ_SUPPORT && NVRAM_RadioOn == 1 && NVRAM_RadioOnINIC == 1 && document.getElementById('apcli_enable').checked == 1);
				securityModeSwitch(form);
			}

			function showPassPhrase() {
				var element = document.getElementById('apcli_wpapsk');
				if (element.type == 'password')
					element.type = 'text'
				else
					element.type = 'password';
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 id="apcliTitle">AP Client Feature</h1>
					<p id="apcliParameters">Here you can configure AP Client parameters.</p>
					<p id="apcliAbout">For correct work need set wireless channel (in Wireless Settings->Basiс) as channel used by AP to connect.</p>
					<hr />
					<form method="POST" name="wireless_apcli" action="/goform/wirelessApcli">
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<div id="apcliStatusData" style="display: none;"></div>
					<table class="form">
						<tr>
							<td id="apcliEnableTitle" class="title" colspan="2">AP Client Parameters</td>
						</tr>
						<tr> 
							<td class="head" style="width: 40%"><input type="checkbox" name="apcli_enable" id="apcli_enable" onClick="apcliEnableSwitch(this.form);">
								<b id="apcliEnable">Enable AP Client</b></td>
							<td id="apcli_status" style="width: 60%"></td>
						</tr>
						<tr id="apcliWiFiMode_tr">
							<td id="apcliWiFiMode" class="head">APCLI Mode</td>
							<td><select id="apcli_interface" name="apcli_interface" size="1" class="normal">
								<option value="apcli0" selected id="1">2.4GHz</option>
								<option value="apclii0" id="2">5GHz</option>
							</select></td>
						</tr>
						<tr id="apcliSSID_tr">
							<td id="apcliSSID" class="head">SSID</td>
							<td>
								<input id="apcli_ssid" name="apcli_ssid" class="normal" maxlength="32">
								<input id="scanapLegendButtonScan" type="button" class="short" value="Scan" onClick="scanAp('apcli', 1);">
							</td>
						</tr>
						<tr id="scanAp" style="display: none;">
							<td colspan="2">
								<div style="width: 785; height: 300px;">
									<div id="scanApPlot" style="width: 100%; height: 300px; margin: 0 auto;">
									</div>
									<div id="scanApPreloader" style="display: none; width:100%; height: 100%">
										<img style="position:relative; left: 50%; top: 50%; margin-top: -32px; margin-left: -32px;" src="/graphics/preloader.gif">
									</div>
								</div>
							</td>
						</tr>
						<tr id="scanApButtons" style="display: none;">
							<td colspan="2" id="scanApButtons_td"></td>
						</tr>
						<tr id="scanApINIC" style="display: none;">
							<td colspan="2">
								<div style="width: 785; height: 300px;">
									<div id="scanApPlotINIC" style="width: 100%; height: 300px; margin: 0 auto;">
									</div>
									<div id="scanApPreloaderINIC" style="display: none; width:100%; height: 100%">
										<img style="position:relative; left: 50%; top: 50%; margin-top: -32px; margin-left: -32px;" src="/graphics/preloader.gif">
									</div>
								</div>
							</td>
						</tr>
						<tr id="scanApButtonsINIC" style="display: none;">
							<td colspan="2" id="scanApButtonsINIC_td"></td>
						</tr>
						<tr id="apcliMAC_tr">
							<td id="apcliMAC" class="head">MAC Address (Optional)</td>
							<td><input id="apcli_bssid" name="apcli_bssid" class="normal"></td>
						</tr>
						<tr id="apcliSecurityMode_tr">
							<td id="apcliSecurityMode" class="head">Security Mode</td>
							<td><select name="apcli_mode" id="apcli_mode" size="1" class="normal" onChange="securityModeSwitch(this.form);">
								<option value="OPEN" id="apcliOpen">Open</option>
								<option value="WPAPSK" id="apcliWPA">WPA-PSK</option>
								<option value="WPA2PSK" id="apcliWPA2">WPA2-PSK</option>
							</select></td>
						</tr>
						<tr id="div_apcli_enc">
							<td id="apcliEncryptionType" class="head">Encryption Type</td>
							<td><select name="apcli_enc" id="apcli_enc" class="normal">
								<option value="AES" selected id="apcliAES">AES</option>
								<option value="TKIP" id="apcliTKIP">TKIP</option>
							</select></td>
						</tr>
						<tr id="div_apcli_wpapsk">
							<td id="apcliPass" class="head">Pass Phrase</td>
							<td class="value"><div style="float: left"><input type="password" id="apcli_wpapsk" name="apcli_wpapsk" class="normal"></div>
									  <div style="float: left; display: flex; height: 1.75em; align-items: center;"><input type="checkbox" onChange="showPassPhrase();"></div>
									  <div id="apcliShowPass" style="display: flex; height: 1.75em; align-items: center;">(show)</div>
							</td>
						</tr> 
						<tr id="apcliAutoscan_tr">
							<td id="apcliAutoscan" class="head">Auto channel select</td>
							<td><input type="checkbox" name="apcli_autoscan" id="apcli_autoscan"></td>
						</tr>
						<tr id="apcliDisableIface_tr">
							<td id="apcliDisableIface" class="head">Disable AP Interface</td>
							<td><input type="checkbox" name="apcli_apiface" id="apcli_apiface"></td>
						</tr>
						<tr id="apcliEnableBridge_tr">
							<td class="head" id="apcliEnableBridge">Enable Bridge Mode</td>
							<td><input type="checkbox" name="apcli_bridge" id="apcli_bridge"></td>
						</tr>
					</table>
					<table class="buttons">
						<tr>
							<td><input type="submit" class="normal" value="Apply"  id="apcliApply"  onClick="return checkValues(this.form);"><input type="button" class="normal" value="Cancel" id="apcliCancel" onClick="window.location.reload();"><input type="button" class="normal" value="Reset"  id="apcliReset"  onClick="resetValues(this.form, 15);">
								<input type="hidden" value="0" name="reset">
							</td>
						</tr>
					</table>
					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>
