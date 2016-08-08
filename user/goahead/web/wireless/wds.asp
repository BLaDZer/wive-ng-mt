<!DOCTYPE html>
<html>
	<head>
		<title>Wireless Distribution System</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("wireless");
			Butterlate.setTextDomain("buttons");

			var wdsNum			= '<% getCfgZero(1, "WdsNum"); %>';
			var wdsPhyMode		= '<% getCfgZero(1, "WdsPhyMode"); %>'.split(';');
			var wdsEncrypType	= '<% getCfgGeneral(1, "WdsEncrypType"); %>'.split(';');
			var wdsEncrypKey0	= '<% getCfgGeneral(1, "Wds0Key"); %>';
			var wdsEncrypKey1	= '<% getCfgGeneral(1, "Wds1Key"); %>';
			var wdsEncrypKey2	= '<% getCfgGeneral(1, "Wds2Key"); %>';
			var wdsEncrypKey3	= '<% getCfgGeneral(1, "Wds3Key"); %>';
			var is5gh			= '<% getCfgGeneral(1, "WdsIfName"); %>' == 'wdsi';
			var is5gh_1t1r		= '<% is5gh_1t1r(); %>' == '1';
			var is5gh_support	= '<% is5gh_support(); %>';
			var wds_interface	= '<% getCfgGeneral(1, "WdsIfName"); %>';
			var radio_on		= "<% getCfgZero(1, "RadioOn"); %>";
			var radio_on_ac		= "<% getCfgZero(1, "RadioOnINIC"); %>";
			var WDS_NUM_MAX		= 4;

			function initTranslation()
			{
				_TR("basicWDS",				"basic wds title");
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
				_TRV("basicWDSApply",		"button apply");
				_TRV("basicWDSCancel",		"button cancel");
				_TRV("basicWDSReset",		"button reset");
				_TRV("basicWDSAdd",			"button add");

				var elements = document.getElementsByTagName('input');
				for (var i = 0; i < elements.length; i++)
					if (elements[i].id == "basicWDSDel")
						elements[i].value = _("button remove");
			}

			function initValues()
			{
				var form	= document.wireless_wds;
				var wdsMode	= '<% getCfgZero(1, "WdsEnable"); %>';
				var wdsList	= '<% getCfgGeneral(1, "WdsList"); %>'.split(';');
				
				var wdslistArray;
				var wdsEncTypeArray;
				
				form.wds_num.value = ('<% getCfgZero(1, "WdsNum"); %>' < 1) ? 1 : '<% getCfgZero(1, "WdsNum"); %>';

				switch(wdsMode) {
					case '0':	form.wds_mode.options.selectedIndex = 0;	break;
					case '2':	form.wds_mode.options.selectedIndex = 2;	break;
					case '3':	form.wds_mode.options.selectedIndex = 3;	break;
					case '4':	form.wds_mode.options.selectedIndex = 1;	break;
				}

				for (var i=0; i < WDS_NUM_MAX; i++) {

					var phy_mode = form.elements['wds_phy_mode' + (i + 1)];

					if (!is5gh_1t1r && is5gh)
						addOption(phy_mode, 'VHT', 'VHT');

					switch(wdsPhyMode[i].toUpperCase()) {
						case 'CCK':			phy_mode.options.selectedIndex = 0;		break;
						case 'OFDM':		phy_mode.options.selectedIndex = 1;		break;
						case 'HTMIX':		phy_mode.options.selectedIndex = 2;		break;
						case 'GREENFIELD':	phy_mode.options.selectedIndex = 3;		break;
						case 'VHT':			phy_mode.options.selectedIndex = 4;		break;
						default:			phy_mode.options.selectedIndex = 2;
					}
					
					switch(wdsEncrypType[i].toUpperCase()) {
						case 'NONE':	form.elements['wds_encryp_type' + i].options.selectedIndex = 0;		break;
						case 'WEP':		form.elements['wds_encryp_type' + i].options.selectedIndex = 1;		break;
						case 'TKIP':	form.elements['wds_encryp_type' + i].options.selectedIndex = 2;		break;
						default:		form.elements['wds_encryp_type' + i].options.selectedIndex = 3;
					}
					
					if (wdsList.length > i)
						form.elements['wds_' + (i+1)].value = wdsList[i];

					form.elements['wds_encryp_key' + i].value = eval('wdsEncrypKey' + i);
				}

				if ((is5gh_support == 0) || ((radio_on == 0) && (radio_on_ac == 0))) {
					form.wds_interface.value = "wds";
					displayElement('basicWdsInterfaceT', false);
				} 
				else {
					form.wds_interface.disabled = (radio_on == 0) || (radio_on_ac == 0);
					if (radio_on == 0) 
						form.wds_interface.value = "wdsi"
					else if (radio_on_ac == 0)
						form.wds_interface.value = "wds"
					else
						form.wds_interface.value = wds_interface;
				}	

				WdsModeOnChange(form);
				showWarning();
				initTranslation();
			}

			function checkValues(form) {
				var all_wds_list		= "";
				var all_wds_enc_type	= "";
				var all_wds_phy_mode	= "";
				var i;

				if (form.wds_mode.options.selectedIndex != 0)
				{
					for (i = 0; i < wdsNum; i++)
						if (!CheckEncKey(form, i))
							return false;

					if (form.wds_mode.options.selectedIndex >= 2) {
						var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
						for (i = 1; i <= WDS_NUM_MAX; i++) {
							if (eval("form.wds_" + i).value == "")
								continue;
							if (!re.test(eval("form.wds_" + i).value)) {
								alert(_("basic wds invalid mac"));
								eval("form.wds_" + i).focus();
								eval("form.wds_" + i).select();
								return false;
							} else {
								all_wds_list += eval("form.wds_" + i).value;
								all_wds_list += (i != WDS_NUM_MAX) ? ';' : '';
							}
						}
						if (all_wds_list == "")	{
							alert(_("basic wds no mac"));
							form.wds_1.focus();
							form.wds_1.select(); 
							return false;
						} else {
							form.wds_list.value = all_wds_list;
							form.wds_1.disabled = true;
							form.wds_2.disabled = true;
							form.wds_3.disabled = true;
							form.wds_4.disabled = true;
						}
					}
					for (i = 1; i <= WDS_NUM_MAX; i++) {
						all_wds_enc_type += eval("form.wds_encryp_type" + (i - 1)).value;
						all_wds_enc_type += (i != WDS_NUM_MAX) ? ';' : '';
						all_wds_phy_mode += eval("form.wds_phy_mode" + i).value;
						all_wds_phy_mode += (i != WDS_NUM_MAX) ? ';' : '';
					}
					form.wds_encryp_type.value = all_wds_enc_type;
					form.wds_phy_mode.value = all_wds_phy_mode;
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function wdsDisplay(form) {
				var count = +form.wds_num.value;
				for (var i = 0; i < count; i++) {
					displayElement( 'div_wds' + i, form.wds_mode.options.selectedIndex >= 1);
					enableElements( [ eval("form.wds_" + (i + 1)) ], form.wds_mode.options.selectedIndex > 1);
					WdsSecurityOnChange(form, i);
				}
				displayElement('basicWdsInterfaceT', (form.wds_mode.options.selectedIndex >= 1) && ((is5gh_support != 0) && ((radio_on != 0) || (radio_on_ac != 0))))
				enableElements(form.basicWDSAdd, count < WDS_NUM_MAX);
				form.wds_num.value = count;
			}

			function wdsAdd(form) {
				var count = +form.wds_num.value;
				if (count < WDS_NUM_MAX) {
					showElement('div_wds' + count);
					enableElements(form.basicWDSAdd, (++count < WDS_NUM_MAX));

					form.elements['wds_' + count].value = '';
					form.elements['wds_encryp_type' + (count - 1)].options.selectedIndex = 0;
					form.elements['wds_encryp_key' + (count - 1)].value = '';
					form.wds_num.value = count;
					wdsNum++;
				}
				wdsDisplay(form);
			}

			function wdsRemove(form, index) {
				var count = +form.wds_num.value;
				if (index < --count) {
					for (var i = index; i < count; i++) {
						form.elements['wds_' + (i + 1)].value = form.elements['wds_' + (i + 2)].value;
						form.elements['wds_phy_mode' + (i + 1)].options.selectedIndex = form.elements['wds_phy_mode' + (i + 2)].options.selectedIndex;
						form.elements['wds_encryp_type' + i].options.selectedIndex = form.elements['wds_encryp_type' + (i + 1)].options.selectedIndex;
						form.elements['wds_encryp_key' + i].value = form.elements['wds_encryp_key' + (i + 1)].value;
					}
				}

				form.elements['wds_' + (count + 1)].value = '';
				form.elements['wds_encryp_type' + count].options.selectedIndex = 0;
				form.elements['wds_encryp_key' + count].value = '';

				hideElement('div_wds' + count);
				enableElements(form.basicWDSAdd, (count < WDS_NUM_MAX));
				form.wds_num.value = count;
				wdsNum--;
			}

			function WdsSecurityOnChange(form, i) {
				enableElements( [ eval("form.wds_encryp_key" + i) ], (eval("form.wds_encryp_type" + i).options.selectedIndex >= 1));
			}

			function WdsModeOnChange(form) {
				enableElements( [ form.wds_phy_mode, form.wds_encryp_type0, form.wds_encryp_type1, form.wds_encryp_type2, form.wds_encryp_type3, 
								  form.wds_encryp_key0, form.wds_encryp_key1, form.wds_encryp_key2, form.wds_encryp_key3 ], form.wds_mode.options.selectedIndex >= 1);
				displayElement( 'dev_head', form.wds_mode.options.selectedIndex >= 1);
				wdsDisplay(form);
			}

			function CheckEncKey(form, i)
			{
				var key = document.getElementById("wds_encryp_key" + i).value;

				if (eval("form.wds_encryp_type" + i).options.selectedIndex == 1) {
					if (key.length == 10 || key.length == 26) {
						var re = /[A-Fa-f0-9]{10,26}/;
						if (!re.test(key)) {
							alert("WDS" + i + " - " + _("basic wds key1"));
							eval("form.wds_encryp_key" + i).focus();
							eval("form.wds_encryp_key" + i).select();
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
						eval("form.wds_encryp_key" + i).focus();
						eval("form.wds_encryp_key" + i).select();
						return false;
					}
				} else if (eval("form.wds_encryp_type" + i).options.selectedIndex == 2 ||
						eval("form.wds_encryp_type" + i).options.selectedIndex == 3) {
					if (key.length < 8 || key.length > 64) {
						alert("WDS" + i + " - " + _("basic wds key2"));
						eval("form.wds_encryp_key" + i).focus();
						eval("form.wds_encryp_key" + i).select();
						return false;
					}
					if (key.length == 64) {
						var re = /[A-Fa-f0-9]{64}/;
						if (!re.test(key)) {
							alert("WDS" + i + " - " + _("basic wds key3"));
							eval("form.wds_encryp_key" + i).focus();
							eval("form.wds_encryp_key" + i).select();
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
		<table class="body">
			<tr id="warning"><tr>
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
									<select name="wds_mode" id="wds_mode" class="normal" onChange="WdsModeOnChange(this.form);">
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
									<select name="wds_interface" size="1" class="normal">
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
								<td style="text-align: center"><input type="text" name="wds_1" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode1" id="wds_phy_mode1" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type0" id="wds_encryp_type0" class="half" onChange="WdsSecurityOnChange(this.form, 0);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key0" name="wds_encryp_key0" class="normal" value=""><input type="checkbox" onChange="showPassPhrase(0);"></td>
								<td style="text-align: center"><input type="button" class="normal" value="Add" id="basicWDSAdd" onClick="wdsAdd(this.form);"></td>
							</tr>
							<tr id="div_wds1" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_2" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode2" id="wds_phy_mode2" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type1" id="wds_encryp_type1" class="half" onChange="WdsSecurityOnChange(this.form, 1);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key1" name="wds_encryp_key1" class="normal" value=""><input type="checkbox" onChange="showPassPhrase(1);"></td>
								<td style="text-align: center"><input type="button" class="normal" value="Delete" id="basicWDSDel" onClick="wdsRemove(this.form, 1);"></td>
							</tr>
							<tr id="div_wds2" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_3" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode3" id="wds_phy_mode3" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type2" id="wds_encryp_type2" class="half" onChange="WdsSecurityOnChange(this.form, 2);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key2" name="wds_encryp_key2" class="normal" value=""><input type="checkbox" onChange="showPassPhrase(2);"></td>
								<td style="text-align: center"><input type="button" class="normal" value="Delete" id="basicWDSDel" onClick="wdsRemove(this.form, 2);"></td>
							</tr>
							<tr id="div_wds3" style="display:none;">
								<td style="text-align: center"><input type="text" name="wds_4" style="width: 98%"; value="" maxlength="17"></td>
								<td style="text-align: center">
									<select name="wds_phy_mode4" id="wds_phy_mode4" class="half">
										<option value="CCK">CCK</option>
										<option value="OFDM">OFDM</option>
										<option value="HTMIX">HTMIX</option>
										<option value="GREENFIELD">GREENFIELD</option>
									</select>
								</td>
								<td style="text-align: center">
									<select name="wds_encryp_type3" id="wds_encryp_type3" class="half" onChange="WdsSecurityOnChange(this.form, 3);">
										<option value="NONE">NONE</option>
										<option value="WEP">WEP</option>
										<option value="TKIP">TKIP</option>
										<option value="AES">AES</option>
									</select>
								</td>
								<td><input type="password" type="text" id="wds_encryp_key3" name="wds_encryp_key3" class="normal" value=""><input type="checkbox" onChange="showPassPhrase(3);"></td>
								<td style="text-align: center"><input type="button" class="normal" value="Delete" id="basicWDSDel" onClick="wdsRemove(this.form, 3);"></td>
							</tr>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="hidden" name="wds_list">
									<input type="hidden" name="wds_phy_mode">
									<input type="hidden" name="wds_encryp_type">
									<input type="hidden" name="wds_num">
									<input type="hidden" name="reset" value="0">
									<input type="submit" class="normal" value="Apply" id="basicWDSApply" onClick="return checkValues(this.form);">&nbsp; &nbsp;
									<input type="button" class="normal" value="Cancel" id="basicWDSCancel" onClick="window.location.reload()">&nbsp; &nbsp;
									<input type="button" class="normal" value="Reset" id="basicWDSReset" onClick="return resetValues(this.form, 30);">
								</td>
							</tr>
						</table>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>
