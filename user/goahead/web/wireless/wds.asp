<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<title>Wireless Distribution System</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var wdsMode  = '<% getCfgZero(1, "WdsEnable"); %>';
var wdsList  = '<% getCfgGeneral(1, "WdsList"); %>';
var wdsPhyMode  = '<% getCfgZero(1, "WdsPhyMode"); %>';
var wdsEncrypType  = '<% getCfgGeneral(1, "WdsEncrypType"); %>';
var wdsEncrypKey0  = '<% getCfgGeneral(1, "Wds0Key"); %>';
var wdsEncrypKey1  = '<% getCfgGeneral(1, "Wds1Key"); %>';
var wdsEncrypKey2  = '<% getCfgGeneral(1, "Wds2Key"); %>';
var wdsEncrypKey3  = '<% getCfgGeneral(1, "Wds3Key"); %>';

function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}

function WdsSecurityOnChange(i)
{
	if (eval("document.wireless_wds.wds_encryp_type"+i).options.selectedIndex >= 1)
		eval("document.wireless_wds.wds_encryp_key"+i).disabled = false;
	else
		eval("document.wireless_wds.wds_encryp_key"+i).disabled = true;
}

function WdsModeOnChange()
{
	document.getElementById("div_wds_phy_mode").style.visibility = "hidden";
	document.getElementById("div_wds_phy_mode").style.display = "none";
	document.wireless_wds.wds_phy_mode.disabled = true;
	document.getElementById("div_wds_encryp_type0").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_type0").style.display = "none";
	document.wireless_wds.wds_encryp_type0.disabled = true;
	document.getElementById("div_wds_encryp_type1").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_type1").style.display = "none";
	document.wireless_wds.wds_encryp_type1.disabled = true;
	document.getElementById("div_wds_encryp_type2").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_type2").style.display = "none";
	document.wireless_wds.wds_encryp_type2.disabled = true;
	document.getElementById("div_wds_encryp_type3").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_type3").style.display = "none";
	document.wireless_wds.wds_encryp_type3.disabled = true;
	document.getElementById("div_wds_encryp_key0").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_key0").style.display = "none";
	document.wireless_wds.wds_encryp_key0.disabled = true;
	document.getElementById("div_wds_encryp_key1").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_key1").style.display = "none";
	document.wireless_wds.wds_encryp_key1.disabled = true;
	document.getElementById("div_wds_encryp_key2").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_key2").style.display = "none";
	document.wireless_wds.wds_encryp_key2.disabled = true;
	document.getElementById("div_wds_encryp_key3").style.visibility = "hidden";
	document.getElementById("div_wds_encryp_key3").style.display = "none";
	document.wireless_wds.wds_encryp_key3.disabled = true;
	document.getElementById("wds_mac_list_1").style.visibility = "hidden";
	document.getElementById("wds_mac_list_1").style.display = "none";
	document.wireless_wds.wds_1.disabled = true;
	document.getElementById("wds_mac_list_2").style.visibility = "hidden";
	document.getElementById("wds_mac_list_2").style.display = "none";
	document.wireless_wds.wds_2.disabled = true;
	document.getElementById("wds_mac_list_3").style.visibility = "hidden";
	document.getElementById("wds_mac_list_3").style.display = "none";
	document.wireless_wds.wds_3.disabled = true;
	document.getElementById("wds_mac_list_4").style.visibility = "hidden";
	document.getElementById("wds_mac_list_4").style.display = "none";
	document.wireless_wds.wds_4.disabled = true;

	if (document.wireless_wds.wds_mode.options.selectedIndex >= 1) {
		document.getElementById("div_wds_phy_mode").style.visibility = "visible";
		document.getElementById("div_wds_phy_mode").style.display = style_display_on();
		document.wireless_wds.wds_phy_mode.disabled = false;
		document.getElementById("div_wds_encryp_type0").style.visibility = "visible";
		document.getElementById("div_wds_encryp_type0").style.display = style_display_on();
		document.wireless_wds.wds_encryp_type0.disabled = false;
		document.getElementById("div_wds_encryp_type1").style.visibility = "visible";
		document.getElementById("div_wds_encryp_type1").style.display = style_display_on();
		document.wireless_wds.wds_encryp_type1.disabled = false;
		document.getElementById("div_wds_encryp_type2").style.visibility = "visible";
		document.getElementById("div_wds_encryp_type2").style.display = style_display_on();
		document.wireless_wds.wds_encryp_type2.disabled = false;
		document.getElementById("div_wds_encryp_type3").style.visibility = "visible";
		document.getElementById("div_wds_encryp_type3").style.display = style_display_on();
		document.wireless_wds.wds_encryp_type3.disabled = false;
		document.getElementById("div_wds_encryp_key0").style.visibility = "visible";
		document.getElementById("div_wds_encryp_key0").style.display = style_display_on();
		document.wireless_wds.wds_encryp_key0.disabled = false;
		document.getElementById("div_wds_encryp_key1").style.visibility = "visible";
		document.getElementById("div_wds_encryp_key1").style.display = style_display_on();
		document.wireless_wds.wds_encryp_key1.disabled = false;
		document.getElementById("div_wds_encryp_key2").style.visibility = "visible";
		document.getElementById("div_wds_encryp_key2").style.display = style_display_on();
		document.wireless_wds.wds_encryp_key2.disabled = false;
		document.getElementById("div_wds_encryp_key3").style.visibility = "visible";
		document.getElementById("div_wds_encryp_key3").style.display = style_display_on();
		document.wireless_wds.wds_encryp_key3.disabled = false;
	}

	WdsSecurityOnChange(0);
	WdsSecurityOnChange(1);
	WdsSecurityOnChange(2);
	WdsSecurityOnChange(3);

	if (document.wireless_wds.wds_mode.options.selectedIndex >= 2) {
		document.getElementById("wds_mac_list_1").style.visibility = "visible";
		document.getElementById("wds_mac_list_1").style.display = style_display_on();
		document.wireless_wds.wds_1.disabled = false;
		document.getElementById("wds_mac_list_2").style.visibility = "visible";
		document.getElementById("wds_mac_list_2").style.display = style_display_on();
		document.wireless_wds.wds_2.disabled = false;
		document.getElementById("wds_mac_list_3").style.visibility = "visible";
		document.getElementById("wds_mac_list_3").style.display = style_display_on();
		document.wireless_wds.wds_3.disabled = false;
		document.getElementById("wds_mac_list_4").style.visibility = "visible";
		document.getElementById("wds_mac_list_4").style.display = style_display_on();
		document.wireless_wds.wds_4.disabled = false;
	}
}

function initTranslation()
{
	var e = document.getElementById("basicWDSTitle");
	e.innerHTML = _("basic wds title");
	e = document.getElementById("basicWDSMode");
	e.innerHTML = _("basic wds mode");
	e = document.getElementById("basicWDSDisable");
	e.innerHTML = _("wireless disable");
	e = document.getElementById("basicWDSPhyMode");
	e.innerHTML = _("basic wds phy mode");
	e = document.getElementById("basicWDSEncrypType");
	e.innerHTML = _("basic wds encryp type");
	e = document.getElementById("basicWDSEncrypKey");
	e.innerHTML = _("basic wds encryp key");
	e = document.getElementById("basicWDSAPMacAddr");
	e.innerHTML = _("basic wds ap macaddr");
}

function initValue()
{
	var wdslistArray;
	var wdsEncTypeArray;

	initTranslation();

	wdsMode = 1*wdsMode;
	if (wdsMode == 0)
		document.wireless_wds.wds_mode.options.selectedIndex = 0;
	else if (wdsMode == 4)
		document.wireless_wds.wds_mode.options.selectedIndex = 1;
	else if (wdsMode == 2)
		document.wireless_wds.wds_mode.options.selectedIndex = 2;
	else if (wdsMode == 3)
		document.wireless_wds.wds_mode.options.selectedIndex = 3;

	if (wdsPhyMode.indexOf("CCK") >= 0 || wdsPhyMode.indexOf("cck") >= 0)
		document.wireless_wds.wds_phy_mode.options.selectedIndex = 0;
	else if (wdsPhyMode.indexOf("OFDM") >= 0 || wdsPhyMode.indexOf("ofdm") >= 0)
		document.wireless_wds.wds_phy_mode.options.selectedIndex = 1;
	else if (wdsPhyMode.indexOf("HTMIX") >= 0 || wdsPhyMode.indexOf("htmix") >= 0)
		document.wireless_wds.wds_phy_mode.options.selectedIndex = 2;
	else
		document.wireless_wds.wds_phy_mode.options.selectedIndex = 2; // Default as HTMIX

	if (wdsEncrypType != "") {
		wdsEncTypeArray = wdsEncrypType.split(";");
		for (i = 1; i <= wdsEncTypeArray.length; i++) {
			k = i - 1;
			if (wdsEncTypeArray[k] == "NONE" || wdsEncTypeArray[k] == "none")
				eval("document.wireless_wds.wds_encryp_type"+k).options.selectedIndex = 0;
			else if (wdsEncTypeArray[k] == "WEP" || wdsEncTypeArray[k] == "wep")
				eval("document.wireless_wds.wds_encryp_type"+k).options.selectedIndex = 1;
			else if (wdsEncTypeArray[k] == "TKIP" || wdsEncTypeArray[k] == "tkip")
				eval("document.wireless_wds.wds_encryp_type"+k).options.selectedIndex = 2;
			else if (wdsEncTypeArray[k] == "AES" || wdsEncTypeArray[k] == "aes")
				eval("document.wireless_wds.wds_encryp_type"+k).options.selectedIndex = 3;
		}
	}

	WdsModeOnChange();

	document.wireless_wds.wds_encryp_key0.value = wdsEncrypKey0;
	document.wireless_wds.wds_encryp_key1.value = wdsEncrypKey1;
	document.wireless_wds.wds_encryp_key2.value = wdsEncrypKey2;
	document.wireless_wds.wds_encryp_key3.value = wdsEncrypKey3;

	if (wdsList != "") {
		wdslistArray = wdsList.split(";");
		for (i = 1; i <= wdslistArray.length; i++)
			eval("document.wireless_wds.wds_"+i).value = wdslistArray[i - 1];
	}
}

function CheckEncKey(i)
{
	var key;
	key = eval("document.wireless_wds.wds_encryp_key"+i).value;

	if (eval("document.wireless_wds.wds_encryp_type"+i).options.selectedIndex == 1) {
		if (key.length == 10 || key.length == 26) {
			var re = /[A-Fa-f0-9]{10,26}/;
			if (!re.test(key)) {
				alert("WDS"+i+"Key should be a 10/26 hexdecimal or a 5/13 ascii");
				eval("document.wireless_wds.wds_encryp_key"+i).focus();
				eval("document.wireless_wds.wds_encryp_key"+i).select();
				return false;
			}
			else
				return true;
		}
		else if (key.length == 5 || key.length == 13) {
			return true;
		}
		else {
			alert("WDS"+i+"Key should be a 10/26 hexdecimal or a 5/13 ascii");
			eval("document.wireless_wds.wds_encryp_key"+i).focus();
			eval("document.wireless_wds.wds_encryp_key"+i).select();
			return false;
		}
	}
	else if (eval("document.wireless_wds.wds_encryp_type"+i).options.selectedIndex == 2 ||
			eval("document.wireless_wds.wds_encryp_type"+i).options.selectedIndex == 3)
	{
		if (key.length < 8 || key.length > 64) {
			alert("WDS"+i+"Key should be with length 8~64");
			eval("document.wireless_wds.wds_encryp_key"+i).focus();
			eval("document.wireless_wds.wds_encryp_key"+i).select();
			return false;
		}
		if (key.length == 64) {
			var re = /[A-Fa-f0-9]{64}/;
			if (!re.test(key)) {
				alert("WDS"+i+"Key should be a 64 hexdecimal");
				eval("document.wireless_wds.wds_encryp_key"+i).focus();
				eval("document.wireless_wds.wds_encryp_key"+i).select();
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

function CheckValue()
{
	var all_wds_list;
	var all_wds_enc_type;

	all_wds_enc_type = document.wireless_wds.wds_encryp_type0.value+";"+
		document.wireless_wds.wds_encryp_type1.value+";"+
		document.wireless_wds.wds_encryp_type2.value+";"+
		document.wireless_wds.wds_encryp_type3.value;
	document.wireless_wds.wds_encryp_type.value = all_wds_enc_type;

	if (!CheckEncKey(0) || !CheckEncKey(1) || !CheckEncKey(2) || !CheckEncKey(3))
		return false;

	all_wds_list = '';
	if (document.wireless_wds.wds_mode.options.selectedIndex >= 2)
	{
		var re = /[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}:[A-Fa-f0-9]{2}/;
		for (i = 1; i <= 4; i++)
		{
			if (eval("document.wireless_wds.wds_"+i).value == "")
				continue;
			if (!re.test(eval("document.wireless_wds.wds_"+i).value)) {
				alert("Please fill WDS remote AP MAC Address in correct format! (XX:XX:XX:XX:XX:XX)");
				eval("document.wireless_wds.wds_"+i).focus();
				eval("document.wireless_wds.wds_"+i).select();
				return false;
			}
			else {
				all_wds_list += eval("document.wireless_wds.wds_"+i).value;
				all_wds_list += ';';
			}
		}
		if (all_wds_list == "")
		{
			alert("No remote AP MAC addresses specified!");
			document.wireless_wds.wds_1.focus();
			document.wireless_wds.wds_1.select(); 
			return false;
		}
		else
		{
			document.wireless_wds.wds_list.value = all_wds_list;
			document.wireless_wds.wds_1.disabled = true;
			document.wireless_wds.wds_2.disabled = true;
			document.wireless_wds.wds_3.disabled = true;
			document.wireless_wds.wds_4.disabled = true;
		}
	}

	return true;
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1>Wireless Distribution System (WDS)</h1>
      <p>Wireless Distribution System Settings</p>
      <hr />
      <form method="post" name="wireless_wds" action="/goform/wirelessWds" onSubmit="return CheckValue()">
        <table class="form">
          <tr>
            <td class="title" id="basicWDSTitle" colspan="2">Wireless Distribution System (WDS)</td>
          </tr>
          <tr>
            <td class="head" id="basicWDSMode">WDS Mode</td>
            <td><select name="wds_mode" id="wds_mode" class="mid" onChange="WdsModeOnChange()">
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
            <td><select name="wds_encryp_type0" id="wds_encryp_type0" class="half" onChange="WdsSecurityOnChange(0)">
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
            <td class="head" id="basicWDSEncrypType">EncrypType</td>
            <td><select name="wds_encryp_type1" id="wds_encryp_type1" class="half" onChange="WdsSecurityOnChange(1)">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select>
            <td>
          </tr>
          <tr id="div_wds_encryp_key1" name="div_wds_encryp_key1">
            <td class="head" id="basicWDSEncrypKey">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key1" class="wide" value=""></td>
          </tr>
          <tr id="div_wds_encryp_type2" name="div_wds_encryp_type2">
            <td class="head" id="basicWDSEncrypType">EncrypType</td>
            <td><select name="wds_encryp_type2" id="wds_encryp_type2" class="half" onChange="WdsSecurityOnChange(2)">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select>
            <td>
          </tr>
          <tr id="div_wds_encryp_key2" name="div_wds_encryp_key2">
            <td class="head" id="basicWDSEncrypKey">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key2" class="wide" value=""></td>
          </tr>
          <tr id="div_wds_encryp_type3" name="div_wds_encryp_type3">
            <td class="head" id="basicWDSEncrypType">EncrypType</td>
            <td><select name="wds_encryp_type3" id="wds_encryp_type3" class="half" onChange="WdsSecurityOnChange(3)">
                <option value="NONE">NONE</option>
                <option value="WEP">WEP</option>
                <option value="TKIP">TKIP</option>
                <option value="AES">AES</option>
              </select>
            <td>
          </tr>
          <tr id="div_wds_encryp_key3" name="div_wds_encryp_key3">
            <td class="head" id="basicWDSEncrypKey">EncrypKey</td>
            <td><input type="text" name="wds_encryp_key3" class="wide" value=""></td>
          </tr>
          <tr id="wds_mac_list_1" name="wds_mac_list_1">
            <td class="head" id="basicWDSAPMacAddr">AP MAC Address</td>
            <td><input type="text" name="wds_1" class="mid" value=""></td>
          </tr>
          <tr id="wds_mac_list_2" name="wds_mac_list_2">
            <td class="head" id="basicWDSAPMacAddr">AP MAC Address</td>
            <td><input type="text" name="wds_2" class="mid" value=""></td>
          </tr>
          <tr id="wds_mac_list_3" name="wds_mac_list_3">
            <td class="head" id="basicWDSAPMacAddr">AP MAC Address</td>
            <td><input type="text" name="wds_3" class="mid" value=""></td>
          </tr>
          <tr id="wds_mac_list_4" name="wds_mac_list_4">
            <td class="head" id="basicWDSAPMacAddr">AP MAC Address</td>
            <td><input type="text" name="wds_4" class="mid" value=""></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="hidden" name="wds_list" value="">
              <input type="hidden" name="wds_encryp_type" value="">
              <input type="submit" class="normal" value="Apply">
              &nbsp; &nbsp;
              <input type="reset"  class="normal" value="Cancel" onClick="window.location.reload()">
              <input type="hidden" name="submit-url" value="/wireless/wds.asp"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
