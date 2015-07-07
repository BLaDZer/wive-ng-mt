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

var wdsMode  = '<% getCfgZero(1, "WdsEnable"); %>';
var wdsList  = '<% getCfgGeneral(1, "WdsList"); %>';
var wdsPhyMode  = '<% getCfgZero(1, "WdsPhyMode"); %>';
var wdsEncrypType  = '<% getCfgGeneral(1, "WdsEncrypType"); %>';
var wdsEncrypKey0  = '<% getCfgGeneral(1, "Wds0Key"); %>';
var wdsEncrypKey1  = '<% getCfgGeneral(1, "Wds1Key"); %>';
var wdsEncrypKey2  = '<% getCfgGeneral(1, "Wds2Key"); %>';
var wdsEncrypKey3  = '<% getCfgGeneral(1, "Wds3Key"); %>';

function WdsSecurityOnChange(form, i)
{
	enableElements( [ eval("form.wds_encryp_key"+i) ], (eval("form.wds_encryp_type"+i).options.selectedIndex >= 1));
}

function WdsModeOnChange(form)
{
	enableElements( [ form.wds_phy_mode, form.wds_encryp_type0, form.wds_encryp_type1, form.wds_encryp_type2, form.wds_encryp_type3, form.wds_encryp_key0, form.wds_encryp_key1, form.wds_encryp_key2, form.wds_encryp_key3 ], (form.wds_mode.options.selectedIndex >= 1));
	displayElement( [ 'div_wds_phy_mode', 'div_wds_encryp_type0', 'div_wds_encryp_type1', 'div_wds_encryp_type2', 'div_wds_encryp_type3', 'div_wds_encryp_key0', 'div_wds_encryp_key1', 'div_wds_encryp_key2', 'div_wds_encryp_key3' ], (form.wds_mode.options.selectedIndex >= 1));

	WdsSecurityOnChange(form, 0);
	WdsSecurityOnChange(form, 1);
	WdsSecurityOnChange(form, 2);
	WdsSecurityOnChange(form, 3);

	enableElements( [ form.wds_1, form.wds_2, form.wds_3, form.wds_4 ], (form.wds_mode.options.selectedIndex >= 2));
	displayElement( [ 'wds_mac_list_1', 'wds_mac_list_2', 'wds_mac_list_3', 'wds_mac_list_4' ], (form.wds_mode.options.selectedIndex >= 2));
}

function initTranslation()
{
	_TR("basicWDSTitle", "basic wds title");
	_TR("basicWDSMode", "basic wds mode");
	_TR("basicWDSPhyMode", "basic wds phy mode");
	_TR("basicWDSDisable", "button disable");
	_TR("basicWDSEncrypType", "basic wds encryp type");
	_TR("basicWDSEncrypKey", "basic wds encryp key");
	_TR("basicWDSAPMacAddr", "basic wds ap macaddr");
	_TR("basicWDSEncrypType1", "basic wds encryp type");
	_TR("basicWDSEncrypKey1", "basic wds encryp key");
	_TR("basicWDSAPMacAddr2", "basic wds ap macaddr");
	_TR("basicWDSEncrypType2", "basic wds encryp type");
	_TR("basicWDSEncrypKey2", "basic wds encryp key");
	_TR("basicWDSAPMacAddr3", "basic wds ap macaddr");
	_TR("basicWDSEncrypType3", "basic wds encryp type");
	_TR("basicWDSEncrypKey3", "basic wds encryp key");
	_TR("basicWDSAPMacAddr4", "basic wds ap macaddr");

	_TRV("basicWDSApply", "button apply");
	_TRV("basicWDSCancel", "button cancel");
}

function initValue()
{
	var wdslistArray;
	var wdsEncTypeArray;
	var form = document.wireless_wds;
	
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

	if (wdsPhyMode.indexOf("CCK") >= 0 || wdsPhyMode.indexOf("cck") >= 0)
		form.wds_phy_mode.options.selectedIndex = 0;
	else if (wdsPhyMode.indexOf("OFDM") >= 0 || wdsPhyMode.indexOf("ofdm") >= 0)
		form.wds_phy_mode.options.selectedIndex = 1;
	else if (wdsPhyMode.indexOf("HTMIX") >= 0 || wdsPhyMode.indexOf("htmix") >= 0)
		form.wds_phy_mode.options.selectedIndex = 2;
	else
		form.wds_phy_mode.options.selectedIndex = 2; // Default as HTMIX

	if (wdsEncrypType != "") {
		wdsEncTypeArray = wdsEncrypType.split(";");
		for (i = 1; i <= wdsEncTypeArray.length; i++) {
			k = i - 1;
			if (wdsEncTypeArray[k] == "NONE" || wdsEncTypeArray[k] == "none")
				eval("form.wds_encryp_type"+k).options.selectedIndex = 0;
			else if (wdsEncTypeArray[k] == "WEP" || wdsEncTypeArray[k] == "wep")
				eval("form.wds_encryp_type"+k).options.selectedIndex = 1;
			else if (wdsEncTypeArray[k] == "TKIP" || wdsEncTypeArray[k] == "tkip")
				eval("form.wds_encryp_type"+k).options.selectedIndex = 2;
			else if (wdsEncTypeArray[k] == "AES" || wdsEncTypeArray[k] == "aes")
				eval("form.wds_encryp_type"+k).options.selectedIndex = 3;
		}
	}

	WdsModeOnChange(form);

	form.wds_encryp_key0.value = wdsEncrypKey0;
	form.wds_encryp_key1.value = wdsEncrypKey1;
	form.wds_encryp_key2.value = wdsEncrypKey2;
	form.wds_encryp_key3.value = wdsEncrypKey3;

	if (wdsList != "") {
		wdslistArray = wdsList.split(";");
		for (i = 1; i <= wdslistArray.length; i++)
			eval("form.wds_"+i).value = wdslistArray[i - 1];
	}
}

function CheckEncKey(form, i)
{
	var key;
	key = eval("form.wds_encryp_key"+i).value;

	if (eval("form.wds_encryp_type"+i).options.selectedIndex == 1) {
		if (key.length == 10 || key.length == 26) {
			var re = /[A-Fa-f0-9]{10,26}/;
			if (!re.test(key)) {
				alert("WDS"+i+_("basic wds key1"));
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
			alert("WDS"+i+_("basic wds key1"));
			eval("form.wds_encryp_key"+i).focus();
			eval("form.wds_encryp_key"+i).select();
			return false;
		}
	}
	else if (eval("form.wds_encryp_type"+i).options.selectedIndex == 2 ||
			eval("form.wds_encryp_type"+i).options.selectedIndex == 3)
	{
		if (key.length < 8 || key.length > 64) {
			alert("WDS"+i+_("basic wds key2"));
			eval("form.wds_encryp_key"+i).focus();
			eval("form.wds_encryp_key"+i).select();
			return false;
		}
		if (key.length == 64) {
			var re = /[A-Fa-f0-9]{64}/;
			if (!re.test(key)) {
				alert("WDS"+i+_("basic wds key3"));
				eval("form.wds_encryp_key"+i).focus();
				eval("form.wds_encryp_key"+i).select();
				return false;
			}
			else
				return true;
		}
		else
			return true;
	}
	return true;
}

function CheckValue(form)
{
	var all_wds_list;
	var all_wds_enc_type;

	all_wds_enc_type = form.wds_encryp_type0.value+";"+
		form.wds_encryp_type1.value+";"+
		form.wds_encryp_type2.value+";"+
		form.wds_encryp_type3.value;
	form.wds_encryp_type.value = all_wds_enc_type;

	if (!CheckEncKey(form, 0) || !CheckEncKey(form, 1) || !CheckEncKey(form, 2) || !CheckEncKey(form, 3))
		return false;

	all_wds_list = '';
	if (form.wds_mode.options.selectedIndex >= 2)
	{
		var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		for (i = 1; i <= 4; i++)
		{
			if (eval("form.wds_"+i).value == "")
				continue;
			if (!re.test(eval("form.wds_"+i).value)) {
				alert(_("basic wds invalid mac"));
				eval("form.wds_"+i).focus();
				eval("form.wds_"+i).select();
				return false;
			}
			else {
				all_wds_list += eval("form.wds_"+i).value;
				all_wds_list += ';';
			}
		}
		if (all_wds_list == "")
		{
			alert(_("basic wds no mac"));
			form.wds_1.focus();
			form.wds_1.select(); 
			return false;
		}
		else
		{
			form.wds_list.value = all_wds_list;
			form.wds_1.disabled = true;
			form.wds_2.disabled = true;
			form.wds_3.disabled = true;
			form.wds_4.disabled = true;
		}
	}

	return true;
}
</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1>Wireless Distribution System</h1>
      <p>Wireless Distribution System Settings</p>
      <hr />
      <form method="post" name="wireless_wds" action="/goform/wirelessWds" onSubmit="return CheckValue(this.form);">
        <table class="form">
          <tr>
            <td class="title" id="basicWDSTitle" colspan="2">Wireless Distribution System</td>
          </tr>
          <tr>
            <td class="head" id="basicWDSMode">WDS Mode</td>
            <td><select name="wds_mode" id="wds_mode" class="mid" onChange="WdsModeOnChange(this.form);">
                <option value=0 id="basicWDSDisable">Disable</option>
                <option value=4>Lazy Mode</option>
                <option value=2>Bridge Mode</option>
                <option value=3>Repeater Mode</option>
              </select></td>
          </tr>
          <tr id="div_wds_phy_mode" name="div_wds_phy_mode">
            <td class="head" id="basicWDSPhyMode">Phy Mode</td>
            <td><select name="wds_phy_mode" id="wds_phy_mode" class="half">
                <option value="CCK;CCK;CCK;CCK">CCK</option>
                <option value="OFDM;OFDM;OFDM;OFDM">OFDM</option>
                <option value="HTMIX;HTMIX;HTMIX;HTMIX">HTMIX</option>
                <!--
	<option value="GREENFIELD;GREENFIELD;GREENFIELD;GREENFIELD">GREENFIELD</option>
	-->
              </select></td>
          </tr>
          <tr id="div_wds_encryp_type0" name="div_wds_encryp_type0">
            <td class="head" id="basicWDSEncrypType">EncrypType</td>
            <td><select name="wds_encryp_type0" id="wds_encryp_type0" class="half" onChange="WdsSecurityOnChange(this.form, 0);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select></td>
          </tr>
          <tr id="div_wds_encryp_key0" name="div_wds_encryp_key0">
            <td class="head" id="basicWDSEncrypKey">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key0" class="wide" value=""></td>
          </tr>
          <tr id="div_wds_encryp_type1" name="div_wds_encryp_type1">
            <td class="head" id="basicWDSEncrypType1">EncrypType</td>
            <td><select name="wds_encryp_type1" id="wds_encryp_type1" class="half" onChange="WdsSecurityOnChange(this.form, 1);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select></td>
          </tr>
          <tr id="div_wds_encryp_key1" name="div_wds_encryp_key1">
            <td class="head" id="basicWDSEncrypKey1">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key1" class="wide" value=""></td>
          </tr>
          <tr id="div_wds_encryp_type2" name="div_wds_encryp_type2">
            <td class="head" id="basicWDSEncrypType2">EncrypType</td>
            <td><select name="wds_encryp_type2" id="wds_encryp_type2" class="half" onChange="WdsSecurityOnChange(this.form, 2);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select></td>
          </tr>
          <tr id="div_wds_encryp_key2" name="div_wds_encryp_key2">
            <td class="head" id="basicWDSEncrypKey2">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key2" class="wide" value=""></td>
          </tr>
          <tr id="div_wds_encryp_type3" name="div_wds_encryp_type3">
            <td class="head" id="basicWDSEncrypType3">EncrypType</td>
            <td><select name="wds_encryp_type3" id="wds_encryp_type3" class="half" onChange="WdsSecurityOnChange(this.form, 3);">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select></td>
          </tr>
          <tr id="div_wds_encryp_key3" name="div_wds_encryp_key3">
            <td class="head" id="basicWDSEncrypKey3">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key3" class="wide" value=""></td>
          </tr>
          <tr id="wds_mac_list_1" name="wds_mac_list_1">
            <td class="head" id="basicWDSAPMacAddr">AP MAC Address</td>
            <td><input type="text" name="wds_1" class="mid" value=""></td>
          </tr>
          <tr id="wds_mac_list_2" name="wds_mac_list_2">
            <td class="head" id="basicWDSAPMacAddr2">AP MAC Address</td>
            <td><input type="text" name="wds_2" class="mid" value=""></td>
          </tr>
          <tr id="wds_mac_list_3" name="wds_mac_list_3">
            <td class="head" id="basicWDSAPMacAddr3">AP MAC Address</td>
            <td><input type="text" name="wds_3" class="mid" value=""></td>
          </tr>
          <tr id="wds_mac_list_4" name="wds_mac_list_4">
            <td class="head" id="basicWDSAPMacAddr4">AP MAC Address</td>
            <td><input type="text" name="wds_4" class="mid" value=""></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="wds_list" value="">
              <input type="hidden" name="wds_encryp_type" value="">
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
