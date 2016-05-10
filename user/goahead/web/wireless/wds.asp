<!DOCTYPE html>
<html>
<head>
<title>Wireless Distribution System</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("wireless");
Butterlate.setTextDomain("buttons");

var wdsNum  = '<% getCfgZero(1, "WdsNum"); %>';
var wdsMode  = '<% getCfgZero(1, "WdsEnable"); %>';
var wdsList  = '<% getCfgGeneral(1, "WdsList"); %>'.split(';');
var wdsPhyMode  = '<% getCfgZero(1, "WdsPhyMode"); %>'.split(';');
var wdsEncrypType  = '<% getCfgGeneral(1, "WdsEncrypType"); %>'.split(';');
var wdsEncrypKey0  = '<% getCfgGeneral(1, "Wds0Key"); %>';
var wdsEncrypKey1  = '<% getCfgGeneral(1, "Wds1Key"); %>';
var wdsEncrypKey2  = '<% getCfgGeneral(1, "Wds2Key"); %>';
var wdsEncrypKey3  = '<% getCfgGeneral(1, "Wds3Key"); %>';
var is5gh = '<% getCfgGeneral(1, "WdsIfName"); %>' == 'wdsi';
var is5gh_1t1r = '<% is5gh_1t1r(); %>' == '1';

var WDS_NUM_MAX = 4;

function wdsDisplay(form) {
	var count = form.wds_num.value * 1;

	for (var i=0; i < count; i++) {
		displayElement( 'div_wds' + i, (form.wds_mode.options.selectedIndex >= 1));
		enableElements( [ eval("form.wds_"+(i+1)) ], (form.wds_mode.options.selectedIndex > 1));
		WdsSecurityOnChange(form, i);
	}

	enableElements(form.basicWDSAdd, (count < WDS_NUM_MAX));
}

function wdsAdd(form) {
	var count = form.wds_num.value * 1;
	if (count < WDS_NUM_MAX) {
		showElement('div_wds' + count);
		enableElements(form.basicWDSAdd, ((++count) < WDS_NUM_MAX));
		form.wds_num.value = count;
		// Clear values
		form.elements['wds_' + count].value = '';
		form.elements['wds_encryp_type' + (count-1)].options.selectedIndex = 0;
		form.elements['wds_encryp_key' + (count-1)].value = '';
                // Increase counter of WDS rules
		wdsNum++;
	}
}

function wdsRemove(form, index) {
	var count = form.wds_num.value * 1;
	if ((index < 1) || (index >= count))
		return;
	if (index < (--count)) {
		// Move values
		for (var i=index; i < count; i++) {
			form.elements['wds_' + (i+1)].value = form.elements['wds_' + (i+2)].value;
			form.elements['wds_phy_mode' + (i+1)].options.selectedIndex = form.elements['wds_phy_mode' + (i+2)].options.selectedIndex;
			form.elements['wds_encryp_type' + i].options.selectedIndex = form.elements['wds_encryp_type' + (i+1)].options.selectedIndex;
			form.elements['wds_encryp_key' + i].value = form.elements['wds_encryp_key' + (i+1)].value;
		}
	}

	// Clear values
	form.elements['wds_' + (count+1)].value = '';
	form.elements['wds_encryp_type' + count].options.selectedIndex = 0;
	form.elements['wds_encryp_key' + count].value = '';

	hideElement('div_wds' + count);
	form.wds_num.value = count;

	// Enable controls
	enableElements(form.basicWDSAdd, (count < WDS_NUM_MAX));

        // Decrease counter of WDS rules
	wdsNum--;
}

function WdsSecurityOnChange(form, i) {
	enableElements( [ eval("form.wds_encryp_key"+i) ], (eval("form.wds_encryp_type"+i).options.selectedIndex >= 1));
}

function WdsModeOnChange(form) {
	enableElements( [ form.wds_phy_mode, form.wds_encryp_type0, form.wds_encryp_type1, form.wds_encryp_type2, form.wds_encryp_type3, form.wds_encryp_key0, form.wds_encryp_key1, form.wds_encryp_key2, form.wds_encryp_key3 ], (form.wds_mode.options.selectedIndex >= 1));
	displayElement( 'dev_head', (form.wds_mode.options.selectedIndex >= 1));

	wdsDisplay(form);
}

function initTranslation()
{
	_TR("basicWDS", "basic wds title");
	_TR("basicWDSIntroduction", "basic wds introduction");
	_TR("basicWDSTitle", "basic wds title");
	_TR("basicWDSMode", "basic wds mode");
	_TR("basicWDSLazyMode", "basic wds lazy mode");
	_TR("basicWDSBridgeMode", "basic wds bridge mode");
	_TR("basicWDSRepeaterMode", "basic wds repeater mode");
	_TR("basicWDSPhyMode", "basic wds phy mode");
	_TR("basicWDSDisable", "button disable");
	_TR("basicWDSEncrypType", "basic wds encryp type");
	_TR("basicWDSEncrypKey", "basic wds encryp key");
	_TR("basicWDSAPMacAddr", "basic wds ap macaddr");
	_TR("basicWDSAction", "basic action");

	_TRV("basicWDSApply", "button apply");
	_TRV("basicWDSCancel", "button cancel");
	_TRV("basicWDSAdd", "button add");

	var elements = document.getElementsByTagName('input');
  	for (i = 0; i < elements.length; i++)
    	if (elements[i].id == "basicWDSDel")
			elements[i].value = _("button remove");
}

function initValue()
{
	var wdslistArray;
	var wdsEncTypeArray;
	var form = document.wireless_wds;
	var count = form.wds_num.value * 1;

	count = (count < 1) ? 1 : count;
	form.wds_num.value = count;

	initTranslation();

	wdsMode = 1*wdsMode;
	if (wdsMode == 0)
		form.wds_mode.options.selectedIndex = 0;
	else if (wdsMode == 4)
		form.wds_mode.options.selectedIndex = 1;
	else if (wdsMode == 2)
		form.wds_mode.options.selectedIndex = 2;
	else if (wdsMode == 3)
		form.wds_mode.options.selectedIndex = 3;

	for (var i=0; i < WDS_NUM_MAX; i++) {
		//select phy_mode
		var phy_mode = form.elements['wds_phy_mode' + (i+1)];

		if (!is5gh_1t1r && is5gh)
			addOption(phy_mode, 'VHT', 'VHT');

		if (wdsPhyMode[i] == "CCK" || wdsPhyMode[i] == "cck")
			phy_mode.options.selectedIndex = 0;
		else if (wdsPhyMode[i] == "OFDM" || wdsPhyMode[i] == "ofdm")
			phy_mode.options.selectedIndex = 1;
		else if (wdsPhyMode[i] == "HTMIX" || wdsPhyMode[i] == "htmix")
			phy_mode.options.selectedIndex = 2;
		else if (wdsPhyMode[i] == "GREENFIELD" || wdsPhyMode[i] == "greenfield")
			phy_mode.options.selectedIndex = 3;
		else if (wdsPhyMode[i] == "VHT" || wdsPhyMode[i] == "vht")
				phy_mode.options.selectedIndex = 4;
		else
			phy_mode.options.selectedIndex = 2;

		//select encryp_type
		var encryp_type = form.elements['wds_encryp_type' + i];
		if (wdsEncrypType[i] == "NONE" || wdsEncrypType[i] == "none")
			encryp_type.options.selectedIndex = 0;
		else if (wdsEncrypType[i] == "WEP" || wdsEncrypType[i] == "wep")
			encryp_type.options.selectedIndex = 1;
		else if (wdsEncrypType[i] == "TKIP" || wdsEncrypType[i] == "tkip")
			encryp_type.options.selectedIndex = 2;
		else
			encryp_type.options.selectedIndex = 3;

		//select wds_list
		if (wdsList.length > i)
			form.elements['wds_' + (i+1)].value = wdsList[i];

		form.elements['wds_encryp_key' + i].value = eval('wdsEncrypKey' + i);
	}

	WdsModeOnChange(form);
}

function CheckEncKey(form, i)
{
	var key;
	key = eval("form.wds_encryp_key"+i).value;

	if (eval("form.wds_encryp_type"+i).options.selectedIndex == 1) {
		if (key.length == 10 || key.length == 26) {
			var re = /[A-Fa-f0-9]{10,26}/;
			if (!re.test(key)) {
				alert("WDS"+i+" - "+_("basic wds key1"));
				eval("form.wds_encryp_key"+i).focus();
				eval("form.wds_encryp_key"+i).select();
				return false;
			}
			else
				return true;
		}
		else if (key.length == 5 || key.length == 13) {
			return true;
		}
		else {
			alert("WDS"+i+" - "+_("basic wds key1"));
			eval("form.wds_encryp_key"+i).focus();
			eval("form.wds_encryp_key"+i).select();
			return false;
		}
	} else if (eval("form.wds_encryp_type"+i).options.selectedIndex == 2 ||
			eval("form.wds_encryp_type"+i).options.selectedIndex == 3) {
		if (key.length < 8 || key.length > 64) {
			alert("WDS"+i+" - "+_("basic wds key2"));
			eval("form.wds_encryp_key"+i).focus();
			eval("form.wds_encryp_key"+i).select();
			return false;
		}
		if (key.length == 64) {
			var re = /[A-Fa-f0-9]{64}/;
			if (!re.test(key)) {
				alert("WDS"+i+" - "+_("basic wds key3"));
				eval("form.wds_encryp_key"+i).focus();
				eval("form.wds_encryp_key"+i).select();
				return false;
			} else
				return true;
		} else
			return true;
	}
	return true;
}

function CheckValue(form) {
	var all_wds_list = "";
	var all_wds_enc_type = "";
	var all_wds_phy_mode = "";

	// if disabled - not need check
	if (form.wds_mode.options.selectedIndex == 0)
		return true;

	// check real used rules only
	for (i = 0; i < wdsNum; i++) {
	    if (!CheckEncKey(form, i))
		return false;
	}

	if (form.wds_mode.options.selectedIndex >= 2) {
		var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		for (i = 1; i <= WDS_NUM_MAX; i++) {
			if (eval("form.wds_"+i).value == "")
				continue;
			if (!re.test(eval("form.wds_"+i).value)) {
				alert(_("basic wds invalid mac"));
				eval("form.wds_"+i).focus();
				eval("form.wds_"+i).select();
				return false;
			} else {
				all_wds_list += eval("form.wds_"+i).value;
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
		all_wds_enc_type += eval("form.wds_encryp_type"+(i-1)).value;
		all_wds_enc_type += (i != WDS_NUM_MAX) ? ';' : '';
		all_wds_phy_mode += eval("form.wds_phy_mode"+i).value;
		all_wds_phy_mode += (i != WDS_NUM_MAX) ? ';' : '';
	}
	form.wds_encryp_type.value = all_wds_enc_type;
	form.wds_phy_mode.value = all_wds_phy_mode;
	return true;
}
</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="basicWDS">Wireless Distribution System</h1>
      <p id="basicWDSIntroduction">Wireless Distribution System Settings</p>
      <hr />
      <form method="post" name="wireless_wds" action="/goform/wirelessWds" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" id="basicWDSTitle" colspan="4">Wireless Distribution System</td>
            <td class="title" style="text-align:right"><select name="wds_mode" id="wds_mode" class="normal" onChange="WdsModeOnChange(this.form);">
                <option value=0 id="basicWDSDisable">Disable</option>
                <option value=4 id="basicWDSLazyMode">Lazy Mode</option>
                <option value=2 id="basicWDSBridgeMode">Bridge Mode</option>
                <option value=3 id="basicWDSRepeaterMode">Repeater Mode</option>
              </select></td>
          </tr>
          <tr id="dev_head" style="display:none;">
            <td class="head" id="basicWDSAPMacAddr">AP MAC Address</td>
            <td class="head" id="basicWDSPhyMode">Phy Mode</td>
            <td class="head" id="basicWDSEncrypType">EncrypType</td>
            <td class="head" id="basicWDSEncrypKey">EncrypKey</td>
            <td class="head" id="basicWDSAction">Action</td>
          </tr>
          <tr id="div_wds0" style="display:none;">
          	<td><input type="text" name="wds_1" class="normal" value=""></td>
          	<td><select name="wds_phy_mode1" id="wds_phy_mode1" class="half">
                <option value="CCK">CCK</option>
                <option value="OFDM">OFDM</option>
                <option value="HTMIX">HTMIX</option>
                <option value="GREENFIELD">GREENFIELD</option>
              </select></td>
            <td><select name="wds_encryp_type0" id="wds_encryp_type0" class="half" onChange="WdsSecurityOnChange(this.form, 0);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
            </select></td>
            <td><input type="password" type="text" name="wds_encryp_key0" class="normal" value=""></td>
            <td><input type="button" class="normal" value="Add" id="basicWDSAdd" onClick="wdsAdd(this.form);"></td>
          </tr>
          <tr id="div_wds1" style="display:none;">
          	<td><input type="text" name="wds_2" class="normal" value=""></td>
          	<td><select name="wds_phy_mode2" id="wds_phy_mode2" class="half">
                <option value="CCK">CCK</option>
                <option value="OFDM">OFDM</option>
                <option value="HTMIX">HTMIX</option>
                <option value="GREENFIELD">GREENFIELD</option>
              </select></td>
            <td><select name="wds_encryp_type1" id="wds_encryp_type1" class="half" onChange="WdsSecurityOnChange(this.form, 1);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
            </select></td>
            <td><input type="password" type="text" name="wds_encryp_key1" class="normal" value=""></td>
            <td><input type="button" class="normal" value="Delete" id="basicWDSDel" onClick="wdsRemove(this.form, 1);"></td>
          </tr>
          <tr id="div_wds2" style="display:none;">
          	<td><input type="text" name="wds_3" class="normal" value=""></td>
          	<td><select name="wds_phy_mode3" id="wds_phy_mode3" class="half">
                <option value="CCK">CCK</option>
                <option value="OFDM">OFDM</option>
                <option value="HTMIX">HTMIX</option>
                <option value="GREENFIELD">GREENFIELD</option>
              </select></td>
            <td><select name="wds_encryp_type2" id="wds_encryp_type2" class="half" onChange="WdsSecurityOnChange(this.form, 2);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
            </select></td>
            <td><input type="password" type="text" name="wds_encryp_key2" class="normal" value=""></td>
            <td><input type="button" class="normal" value="Delete" id="basicWDSDel" onClick="wdsRemove(this.form, 2);"></td>
          </tr>
          <tr id="div_wds3" style="display:none;">
          	<td><input type="text" name="wds_4" class="normal" value=""></td>
          	<td><select name="wds_phy_mode4" id="wds_phy_mode4" class="half">
                <option value="CCK">CCK</option>
                <option value="OFDM">OFDM</option>
                <option value="HTMIX">HTMIX</option>
                <option value="GREENFIELD">GREENFIELD</option>
              </select></td>
            <td><select name="wds_encryp_type3" id="wds_encryp_type3" class="half" onChange="WdsSecurityOnChange(this.form, 3);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
            </select></td>
            <td><input type="password" type="text" name="wds_encryp_key3" class="normal" value=""></td>
            <td><input type="button" class="normal" value="Delete" id="basicWDSDel" onClick="wdsRemove(this.form, 3);"></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="wds_list" value="">
              <input type="hidden" name="wds_phy_mode" value="">
              <input type="hidden" name="wds_encryp_type" value="">
              <input type="hidden" name="wds_num" value="<% getCfgZero(1, "WdsNum"); %>">
              <input type="submit" class="normal" value="Apply" id="basicWDSApply">
              <input type="reset"  class="normal" value="Cancel" id="basicWDSCancel" onClick="window.location.reload()">
              <input type="hidden" name="submit-url" value="/wireless/wds.asp"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
