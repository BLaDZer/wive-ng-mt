<!DOCTYPE html>
<html>
	<head>
		<title>Wireless Distribution System</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_wireless.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			var wdsPhyMode		= NVRAM_WdsPhyMode.split(';');
			var wdsEncrypType	= NVRAM_WdsEncrypType.split(';');

			function initTranslation() {
				_TR("basicWDS",			"basic wds title");
				_TR("basicWDSIntroduction",	"basic wds introduction");
				_TR("basicWDSTitle",		"basic wds mode");
				_TR("basicWDSInterface",	"basic wds wifi mode")
				_TR("basicWDSLazyMode",		"basic wds lazy mode");
				_TR("basicWDSBridgeMode",	"basic wds bridge mode");
				_TR("basicWDSRepeaterMode",	"basic wds repeater mode");
				_TR("basicWDSPhyMode",		"basic wds phy mode");
				_TR("basicWDSDisable",		"button disable");
				_TR("basicWDSEncrypType",	"basic wds encryp type");
				_TR("basicWDSEncrypKey",	"basic wds encryp key");
				_TR("basicWDSAPMacAddr",	"basic wds ap macaddr");
				_TR("basicWDSAction",		"basic action");
				_TR("basicWDSApply",		"button apply");
				_TR("basicWDSCancel",		"button cancel");
				_TR("basicWDSReset",		"button reset");
				_TR("basicWDSAdd",		"button add");
				_TR("basicWDSDel1",		"button remove");
				_TR("basicWDSDel2",		"button remove");
				_TR("basicWDSDel3",		"button remove");
			}

			function initValues() {
				var wdsList	= NVRAM_WdsList.split(';');
				
				var wdslistArray;
				var wdsEncTypeArray;
				
				document.getElementById('wds_num').value = (NVRAM_WdsNum < 1) ? 1 : NVRAM_WdsNum;

				switch(NVRAM_WdsEnable) {
					case '0':	document.getElementById('wds_mode').options.selectedIndex = 0;	break;
					case '2':	document.getElementById('wds_mode').options.selectedIndex = 2;	break;
					case '3':	document.getElementById('wds_mode').options.selectedIndex = 3;	break;
					case '4':	document.getElementById('wds_mode').options.selectedIndex = 1;	break;
				}

				for (var i = 0; i < WDS_NUM_MAX; i++) {

					var phy_mode = document.getElementById('wds_phy_mode' + (i + 1));
				
					if (NVRAM_WdsIfName == 'wds')
						phy_mode.options[4].style.display = 'none';

					switch(wdsPhyMode[i].toUpperCase()) {
						case 'CCK':		phy_mode.options.selectedIndex = 0;	break;
						case 'OFDM':		phy_mode.options.selectedIndex = 1;	break;
						case 'HTMIX':		phy_mode.options.selectedIndex = 2;	break;
						case 'GREENFIELD':	phy_mode.options.selectedIndex = 3;	break;
						case 'VHT':		phy_mode.options.selectedIndex = 4;	break;
						default:		phy_mode.options.selectedIndex = 2;
					}
					
					switch(wdsEncrypType[i].toUpperCase()) {
						case 'NONE':	document.getElementById('wds_encryp_type' + i).options.selectedIndex = 0;	break;
						case 'WEP':	document.getElementById('wds_encryp_type' + i).options.selectedIndex = 1;	break;
						case 'TKIP':	document.getElementById('wds_encryp_type' + i).options.selectedIndex = 2;	break;
						default:	document.getElementById('wds_encryp_type' + i).options.selectedIndex = 3;
					}

					if (wdsList.length > i)
						document.getElementById('wds_' + (i + 1)).value = wdsList[i];

					document.getElementById('wds_encryp_key' + i).value = eval('NVRAM_Wds' + i + 'Key');
				}

				if (!BUILD_5GHZ_SUPPORT || (NVRAM_RadioOn == 0 && NVRAM_RadioOnINIC == 0)) {
					document.getElementById('wds_interface').value = "wds";
					displayElement('basicWdsInterfaceT', false);
				}
				else {
					document.getElementById('wds_interface').disabled = NVRAM_RadioOn == 0 || NVRAM_RadioOnINIC == 0;
					if (NVRAM_RadioOn == 0) 
						document.getElementById('wds_interface').value = "wdsi"
					else if (NVRAM_RadioOnINIC == 0)
						document.getElementById('wds_interface').value = "wds"
					else
						document.getElementById('wds_interface').value = NVRAM_WdsIfName;
				}

				var browser = getBrowser();
				for (var i = 0; i < WDS_NUM_MAX; i++) {
					document.getElementById('showPhassPhrase_' + i).title = _("secure wpa show pass phrase wds");
					if (browser.browser == 'ie' && browser.versionShort <= 8)
						displayElement( 'showPhassPhrase_' + i, false);
				}

				WdsModeOnChange();
				showWarning();
				initTranslation();
			}

			function checkValues() {
				var all_wds_list	= "";
				var all_wds_enc_type	= "";
				var all_wds_phy_mode	= "";
				var i;

				if (document.getElementById('wds_mode').options.selectedIndex != 0)
				{
					for (i = 0; i < NVRAM_WdsNum; i++)
						if (!CheckEncKey(i))
							return false;

					if (document.getElementById('wds_mode').options.selectedIndex >= 2) {
						var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
						for (i = 1; i <= WDS_NUM_MAX; i++) {
							if (document.getElementById('wds_' + i).value == "")
								continue;
							if (!re.test(document.getElementById('wds_' + i).value)) {
								alert(_("basic wds invalid mac"));
								document.getElementById('wds_' + i).focus();
								document.getElementById('wds_' + i).select();
								return false;
							}
							else {
								all_wds_list += document.getElementById('wds_' + i).value;
								all_wds_list += (i != WDS_NUM_MAX) ? ';' : '';
							}
						}
						if (all_wds_list == "") {
							alert(_("basic wds no mac"));
							document.getElementById('wds_1').focus();
							document.getElementById('wds_1').select();
							return false;
						}
						else {
							document.getElementById('wds_list').value = all_wds_list;
							document.getElementById('wds_1').disabled = true;
							document.getElementById('wds_2').disabled = true;
							document.getElementById('wds_3').disabled = true;
							document.getElementById('wds_4').disabled = true;
						}
					}
					for (i = 1; i <= WDS_NUM_MAX; i++) {
						all_wds_enc_type += document.getElementById('wds_encryp_type' + (i - 1)).value;
						all_wds_enc_type += (i != WDS_NUM_MAX) ? ';' : '';
						all_wds_phy_mode += document.getElementById('wds_phy_mode' + i).value;
						all_wds_phy_mode += (i != WDS_NUM_MAX) ? ';' : '';
					}
					document.getElementById('wds_encryp_type').value = all_wds_enc_type;
					document.getElementById('wds_phy_mode').value = all_wds_phy_mode;
				}
				ajaxShowTimer(document.wireless_wds, 'timerReloader', _('message apply'), 30);
				return true;
			}

			function wdsDisplay() {
				var count = +document.getElementById('wds_num').value;
				for (var i = 0; i < count; i++) {
					displayElement( 'div_wds' + i, document.getElementById('wds_mode').options.selectedIndex >= 1);
					enableElements( document.getElementById('wds_' + (i + 1)), document.getElementById('wds_mode').options.selectedIndex > 1);
					WdsSecurityOnChange(i);
				}
				displayElement('basicWdsInterfaceT', document.getElementById('wds_mode').options.selectedIndex >= 1 && (BUILD_5GHZ_SUPPORT && (NVRAM_RadioOn != 0 || NVRAM_RadioOnINIC != 0)));
				enableElements(document.getElementById('basicWDSAdd'), count < WDS_NUM_MAX);
				document.getElementById('wds_num').value = count;
			}

			function wdsAdd() {
				var count = +document.getElementById('wds_num').value;
				if (count < WDS_NUM_MAX) {
					showElement('div_wds' + count);
					enableElements(document.getElementById('basicWDSAdd'), ++count < WDS_NUM_MAX);

					document.getElementById('wds_' + count).value = '';
					document.getElementById('wds_encryp_type' + (count - 1)).options.selectedIndex = 0;
					document.getElementById('wds_encryp_key' + (count - 1)).value = '';
					document.getElementById('wds_num').value = count;
					NVRAM_WdsNum++;
				}
				wdsDisplay();
			}

			function wdsRemove(index) {
				var count = +document.getElementById('wds_num').value;
				if (index < --count) {
					for (var i = index; i < count; i++) {
						document.getElementById('wds_' + (i + 1)).value = document.getElementById('wds_' + (i + 2)).value;
						document.getElementById('wds_phy_mode' + (i + 1)).options.selectedIndex = document.getElementById('wds_phy_mode' + (i + 2)).options.selectedIndex;
						document.getElementById('wds_encryp_type' + i).options.selectedIndex = document.getElementById('wds_encryp_type' + (i + 1)).options.selectedIndex;
						document.getElementById('wds_encryp_key' + i).value = document.getElementById('wds_encryp_key' + (i + 1)).value;
					}
				}

				document.getElementById('wds_' + (count + 1)).value = '';
				document.getElementById('wds_encryp_type' + count).options.selectedIndex = 0;
				document.getElementById('wds_encryp_key' + count).value = '';

				hideElement('div_wds' + count);
				enableElements(document.getElementById('basicWDSAdd'), count < WDS_NUM_MAX);
				document.getElementById('wds_num').value = count;
				NVRAM_WdsNum--;
			}

			function WdsSecurityOnChange(i) {
				enableElements(document.getElementById('wds_encryp_key' + i), document.getElementById('wds_encryp_type' + i).options.selectedIndex >= 1);
			}

			function WdsModeOnChange() {
				enableElements( [	document.getElementById('wds_phy_mode'),
							document.getElementById('wds_encryp_type0'),
							document.getElementById('wds_encryp_type1'),
							document.getElementById('wds_encryp_type2'),
							document.getElementById('wds_encryp_type3'),
							document.getElementById('wds_encryp_key0'),
							document.getElementById('wds_encryp_key1'),
							document.getElementById('wds_encryp_key2'),
							document.getElementById('wds_encryp_key3') ], document.getElementById('wds_mode').options.selectedIndex >= 1);
				displayElement( 'dev_head', document.getElementById('wds_mode').options.selectedIndex >= 1);
				wdsDisplay();
			}

			function wdsInterfaceClick() {
				for (var i = 0; i < WDS_NUM_MAX; i++) {
					var phy_mode = document.getElementById('wds_phy_mode' + (i + 1));
					if (document.getElementById('wds_interface').value == 'wds') {
						phy_mode.options[4].style.display = 'none';
						if (phy_mode.options.selectedIndex == 4)
							phy_mode.options.selectedIndex = 2;
					}
					else
						phy_mode.options[4].style.display = 'block';
				}
			}

			function CheckEncKey(i) {
				var key = document.getElementById("wds_encryp_key" + i).value;

				if (document.getElementById('wds_encryp_type' + i).options.selectedIndex == 1) {
					if (key.length == 10 || key.length == 26) {
						var re = /[A-Fa-f0-9]{10,26}/;
						if (!re.test(key)) {
							alert("WDS" + i + " - " + _("basic wds key1"));
							document.getElementById('wds_encryp_key' + i).focus();
							document.getElementById('wds_encryp_key' + i).select();
							return false;
						}
						else
							return true;
					}
					else if (key.length == 5 || key.length == 13) {
						return true;
					}
					else {
						alert("WDS" + i + " - " + _("basic wds key1"));
						document.getElementById('wds_encryp_key' + i).focus();
						document.getElementById('wds_encryp_key' + i).select();
						return false;
					}
				} else if (document.getElementById('wds_encryp_type' + i).options.selectedIndex == 2 ||
						document.getElementById('wds_encryp_type' + i).options.selectedIndex == 3) {
					if (key.length < 8 || key.length > 64) {
						alert("WDS" + i + " - " + _("basic wds key2"));
						document.getElementById('wds_encryp_key' + i).focus();
						document.getElementById('wds_encryp_key' + i).select();
						return false;
					}
					if (key.length == 64) {
						var re = /[A-Fa-f0-9]{64}/;
						if (!re.test(key)) {
							alert("WDS" + i + " - " + _("basic wds key3"));
							document.getElementById('wds_encryp_key' + i).focus();
							document.getElementById('wds_encryp_key' + i).select();
							return false;
						} else
							return true;
					} else
						return true;
				}
				return true;
			}

			function showPassPhrase(id) {
				var element = document.getElementById('wds_encryp_key' + id);
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
				<td><h1 id="basicWDS">Wireless Distribution System</h1>
					<p id="basicWDSIntroduction">Wireless Distribution System Settings</p>
					<hr />
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="post" name="wireless_wds" action="/goform/wirelessWds">
						<table class="form">
							<tr>
								<td class="title" id="basicWDSTitle" colspan="4">Wireless Distribution System</td>
								<td class="title" style="text-align:right">
									<select name="wds_mode" id="wds_mode" style="width: 120px" onChange="WdsModeOnChange();">
										<option value=0 id="basicWDSDisable">Disable</option>
										<option value=4 id="basicWDSLazyMode">Lazy Mode</option>
										<option value=2 id="basicWDSBridgeMode">Bridge Mode</option>
										<option value=3 id="basicWDSRepeaterMode">Repeater Mode</option>
									</select>
								</td>
							</tr>
							<tr id="basicWdsInterfaceT" style="display:none;">
								<td class="head" id="basicWDSInterface" colspan="1">WDS Mode</td>
								<td colspan="5">
									<select name="wds_interface" id="wds_interface" size="1" class="normal" onChange="wdsInterfaceClick();">
										<option value="wds" selected id="1">2.4GHz</option>
										<option value="wdsi" id="2">5GHz</option>
									</select>
								</td>
							</tr>
							<tr id="dev_head" style="display:none;">
								<td class="head" style="text-align: center" id="basicWDSAPMacAddr">AP MAC Address</td>
								<td class="head" style="text-align: center" id="basicWDSPhyMode">Phy Mode</td>
								<td class="head" style="text-align: center" id="basicWDSEncrypType">EncrypType</td>
								<td class="head" style="text-align: center" id="basicWDSEncrypKey">EncrypKey</td>
								<td class="head" style="text-align: center" id="basicWDSAction">Action</td>
							</tr>
							<tr id="div_wds0" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_1" id="wds_1" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode1" id="wds_phy_mode1" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
										<option value="VHT">VHT</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type0" id="wds_encryp_type0" class="half" onChange="WdsSecurityOnChange(0);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key0" name="wds_encryp_key0" style="width: 85%;" value=""><span style="position: absolute; margin-top: 1px;"><input id="showPhassPhrase_0" type="checkbox" onChange="showPassPhrase(0);"></span></td>
								<td style="text-align: center"><input type="button" style="width: 120px" value="Add" id="basicWDSAdd" onClick="wdsAdd();"></td>
							</tr>
							<tr id="div_wds1" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_2" id="wds_2" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode2" id="wds_phy_mode2" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
										<option value="VHT">VHT</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type1" id="wds_encryp_type1" class="half" onChange="WdsSecurityOnChange(1);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key1" name="wds_encryp_key1" style="width: 85%;" value=""><span style="position: absolute; margin-top: 1px;"><input id="showPhassPhrase_1" type="checkbox" onChange="showPassPhrase(1);"></span></td>
								<td style="text-align: center"><input type="button" style="width: 120px" value="Delete" id="basicWDSDel1" onClick="wdsRemove(1);"></td>
							</tr>
							<tr id="div_wds2" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_3" id="wds_3" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode3" id="wds_phy_mode3" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
										<option value="VHT">VHT</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type2" id="wds_encryp_type2" class="half" onChange="WdsSecurityOnChange(2);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key2" name="wds_encryp_key2" style="width: 85%;" value=""><span style="position: absolute; margin-top: 1px;"><input id="showPhassPhrase_2" type="checkbox" onChange="showPassPhrase(2);"></span></td>
								<td style="text-align: center"><input type="button" style="width: 120px" value="Delete" id="basicWDSDel2" onClick="wdsRemove(2);"></td>
							</tr>
							<tr id="div_wds3" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_4" id="wds_4" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode4" id="wds_phy_mode4" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
										<option value="VHT">VHT</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type3" id="wds_encryp_type3" class="half" onChange="WdsSecurityOnChange(3);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key3" name="wds_encryp_key3" style="width: 85%;" value=""><span style="position: absolute; margin-top: 1px;"><input id="showPhassPhrase_3" type="checkbox" onChange="showPassPhrase(3);"></span></td>
								<td style="text-align: center"><input type="button" style="width: 120px" value="Delete" id="basicWDSDel3" onClick="wdsRemove(3);"></td>
							</tr>
						</table>
						<table class="buttons">
							<tr>
								<td>
									<input type="hidden" name="wds_list" id="wds_list">
									<input type="hidden" name="wds_phy_mode" id="wds_phy_mode">
									<input type="hidden" name="wds_encryp_type" id="wds_encryp_type">
									<input type="hidden" name="wds_num" id="wds_num">
									<input type="hidden" name="reset" value="0">
									<input type="submit" class="normal" value="Apply" id="basicWDSApply" onClick="return checkValues();"><input type="button" class="normal" value="Cancel" id="basicWDSCancel" onClick="window.location.reload()"><input type="button" class="normal" value="Reset" id="basicWDSReset" onClick="return resetValues(document.wireless_wds, 30);">
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