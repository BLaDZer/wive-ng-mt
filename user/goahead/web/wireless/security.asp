<!DOCTYPE html>
<html>
<head>
<title>Wireless Security Settings</title>
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

var MBSSID_MAX 			= 8;
var ACCESSPOLICYLIST_MAX	= 64;

var changed = 0;
var old_MBSSID;

var defaultShownMBSSID = 0;
var SSID = new Array();
var PreAuth = new Array();
var AuthMode = new Array();
var EncrypType = new Array();
var DefaultKeyID = new Array();
var Key1Type = new Array();
var Key1Str = new Array();
var Key2Type = new Array();
var Key2Str = new Array();
var Key3Type = new Array();
var Key3Str = new Array();
var Key4Type = new Array();
var Key4Str = new Array();
var WPAPSK = new Array();
var RekeyMethod = new Array();
var RekeyInterval = new Array();
var PMKCachePeriod = new Array();
var IEEE8021X = new Array();
var RADIUS_Server = new Array();
var RADIUS_Port = new Array();
var RADIUS_Key = new Array();
var session_timeout_interval = new Array();
var AccessPolicy = new Array();
var AccessControlList = new Array();

var security_modes_list =
[
	[ _("wireless disable"), "Disable", 0, 0 ],
	[ "OPEN", "OPEN", 0, 0 ],
	[ "SHARED", "SHARED", 0, 0 ],
	[ "WEP", "WEPAUTO", 0, 0 ],
	[ "WPA", "WPA", 1, 0 ],
	[ "WPA-PSK", "WPAPSK", 0, 0],
	[ "WPA2", "WPA2", 1, 0 ],
	[ "WPA2-PSK", "WPA2PSK", 0, 0 ],
	[ "WPA1/2-PSK", "WPAPSKWPA2PSK", 0, 0 ],
	[ "WPA1/2", "WPA1WPA2", 1, 0 ],
	[ "802.1X", "IEEE8021X", 1, 1 ]
];

function checkMac(str){
	var len = str.length;
	if(len!=17)
		return false;

	for (var i=0; i<str.length; i++) {
		if((i%3) == 2){
			if(str.charAt(i) == ':')
				continue;
		}else{
			if (    (str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
					(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
					(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') )
			continue;
		}
		return false;
	}
	return true;
}

function checkRange(str, num, min, max)
{
    d = atoi(str,num);
    if(d > max || d < min)
        return false;
    return true;
}

function checkIpAddr(field)
{
    if(field.value == "")
        return false;

    if ( checkAllNum(field.value) == 0)
        return false;

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        return false;
    }
   return true;
}

function atoi(str, num)
{
    i=1;
    if(num != 1 ){
        while (i != num && str.length != 0){
            if(str.charAt(0) == '.'){
                i++;
            }
            str = str.substring(1);
        }
        if(i != num )
            return -1;
    }

    for(i=0; i<str.length; i++){
        if(str.charAt(i) == '.'){
            str = str.substring(0, i);
            break;
        }
    }
    if(str.length == 0)
        return -1;
    return parseInt(str, 10);
}

function checkHex(str){
	var len = str.length;

	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') ||
			(str.charAt(i) >= 'a' && str.charAt(i) <= 'f') ||
			(str.charAt(i) >= 'A' && str.charAt(i) <= 'F') ){
				continue;
		}else
	        return false;
	}
    return true;
}

function checkInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if ( str.charAt(i) == '\r' || str.charAt(i) == '\n'){
				return false;
		}else
	        continue;
	}
    return true;
}

function checkStrictInjection(str)
{
	var len = str.length;
	for (var i=0; i<str.length; i++) {
		if ( str.charAt(i) == ';' || str.charAt(i) == ',' ||
			 str.charAt(i) == '\r' || str.charAt(i) == '\n'){
				return false;
		}else
	        continue;
	}
    return true;
}

function checkAllNum(str)
{
    for (var i=0; i<str.length; i++){
        if((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
            continue;
        return false;
    }
    return true;
}

var http_request = false;
function makeRequest(url, content, handler) {
	http_request = false;
	if (window.XMLHttpRequest) { // Mozilla, Safari,...
		http_request = new XMLHttpRequest();
		if (http_request.overrideMimeType) {
			http_request.overrideMimeType('text/xml');
		}
	} else if (window.ActiveXObject) { // IE
		try {
			http_request = new ActiveXObject("Msxml2.XMLHTTP");
		} catch (e) {
			try {
			http_request = new ActiveXObject("Microsoft.XMLHTTP");
			} catch (e) {}
		}
	}
	if (!http_request) {
		alert('Giving up :( Cannot create an XMLHTTP instance');
		return false;
	}
	http_request.onreadystatechange = handler;
	http_request.open('POST', url, true);
	http_request.send(content);
}

function securityHandler()
{
	if (http_request.readyState == 4)
	{
		if (http_request.status == 200)
		{
			parseAllData(http_request.responseText);
			UpdateMBSSIDList();
			LoadFields(defaultShownMBSSID);

			// load Access Policy for MBSSID[selected]
			LoadAP();
			ShowAP(defaultShownMBSSID);
		} else {
			alert('There was a problem with the request.');
		}
	}
}

function deleteAccessPolicyListHandler()
{
	window.location.reload(false);
}


function parseAllData(str)
{
	var all_str = new Array();
	all_str = str.split("\n");

	defaultShownMBSSID = parseInt(all_str[0]);

	for (var i=0; i<all_str.length-2; i++) {
		var fields_str = new Array();
		fields_str = all_str[i+1].split("\r");

		SSID[i] = fields_str[0];
		PreAuth[i] = fields_str[1];
		AuthMode[i] = fields_str[2];
		EncrypType[i] = fields_str[3];
		DefaultKeyID[i] = fields_str[4];
		Key1Type[i] = fields_str[5];
		Key1Str[i] = fields_str[6];
		Key2Type[i] = fields_str[7];
		Key2Str[i] = fields_str[8];
		Key3Type[i] = fields_str[9];
		Key3Str[i] = fields_str[10];
		Key4Type[i] = fields_str[11];
		Key4Str[i] = fields_str[12];
		WPAPSK[i] = fields_str[13];
		RekeyMethod[i] = fields_str[14];
		RekeyInterval[i] = (fields_str[15] == "") ? 3600 : fields_str[15];
		PMKCachePeriod[i] = fields_str[16];
		IEEE8021X[i] = fields_str[17];
		RADIUS_Server[i] = fields_str[18];
		RADIUS_Port[i] = fields_str[19];
		RADIUS_Key[i] = fields_str[20];
		session_timeout_interval[i] = fields_str[21];
		AccessPolicy[i] = fields_str[22];
		AccessControlList[i] = fields_str[23];

		/* !!!! IMPORTANT !!!!*/
		if(IEEE8021X[i] == "1")
			AuthMode[i] = "IEEE8021X";

		if(AuthMode[i] == "OPEN" && EncrypType[i] == "NONE" && IEEE8021X[i] == "0")
			AuthMode[i] = "Disable";
	}
}

function checkData()
{
	var securitymode;
//	var ssid = document.security_form.Ssid.value;
	
	securitymode = document.security_form.security_mode.value;
	if (securitymode == "OPEN" || securitymode == "SHARED" ||securitymode == "WEPAUTO")
	{
		if(! check_Wep(securitymode) )
			return false;
	}else if (securitymode == "WPAPSK" || securitymode == "WPA2PSK" || securitymode == "WPAPSKWPA2PSK"){
		var keyvalue = document.security_form.passphrase.value;

		if (keyvalue.length == 0){
			alert(_("secure no key"));
			return false;
		}

		if (keyvalue.length < 8){
			alert(_("apcli short phrase"));
			return false;
		}
		
		if(checkInjection(document.security_form.passphrase.value) == false){
			alert(_("apcli chars not allowed"));
			return false;
		}

		if(document.security_form.cipher[0].checked != true && 
		   document.security_form.cipher[1].checked != true &&
   		   document.security_form.cipher[2].checked != true){
   		   alert(_("secure choose algo"));
   		   return false;
		}

		if(checkAllNum(document.security_form.keyRenewalInterval.value) == false){
			alert(_("secure renewal"));
			return false;
		}
		if(document.security_form.keyRenewalInterval.value < 60){
			alert(_("secure renewal short"));
			// return false;
		}
		if(check_wpa() == false)
			return false;
	}
	//802.1x
	else if (securitymode == "IEEE8021X") // 802.1x
	{
		if( document.security_form.ieee8021x_wep[0].checked == false &&
			document.security_form.ieee8021x_wep[1].checked == false){
			alert(_("secure choose wep"));
			return false;
		}
		if(check_radius() == false)
			return false;
	}else if (securitymode == "WPA" || securitymode == "WPA1WPA2") //     WPA or WPA1WP2 mixed mode
	{
		if(check_wpa() == false)
			return false;
		if(check_radius() == false)
			return false;
	}else if (securitymode == "WPA2") //         WPA2
	{
		if(check_wpa() == false)
			return false;
		if( document.security_form.PreAuthentication[0].checked == false &&
			document.security_form.PreAuthentication[1].checked == false){
			alert(_("secure choose preauth"));
			return false;
		}

		if(!document.security_form.PMKCachePeriod.value.length){
			alert(_("secure no pmk"));
			return false;
		}
		if(check_radius() == false)
			return false;
	}

	// check Access Policy
	for(i=0; i<MBSSID_MAX; i++)
	{
		if( document.getElementById("newap_text_" + i).value != ""){
			if(!checkMac(document.getElementById("newap_text_" + i).value)){
				alert(_("secure invalid mac"));
				return false;
			}
		}
	}

	return true;
}

function check_wpa()
{
	if(document.security_form.cipher[0].checked != true && 
		   document.security_form.cipher[1].checked != true &&
		   document.security_form.cipher[2].checked != true){
		   alert(_("secure choose algo"));
		   return false;
	}

	if(checkAllNum(document.security_form.keyRenewalInterval.value) == false){
		alert(_("secure renewal"));
		return false;
	}
	if(document.security_form.keyRenewalInterval.value < 60){
		alert(_("secure renewal short"));
		// return false;
	}
	return true;
}

function check_radius()
{
	if(!document.security_form.RadiusServerIP.value.length){
		alert(_("secure no radius ip"));
		return false;
	}
	if(!document.security_form.RadiusServerPort.value.length){
		alert(_("secure no radius port"));
		return false;
	}
	if(!document.security_form.RadiusServerSecret.value.length){
		alert(_("secure no radius secret"));
		return false;
	}

	if(checkIpAddr(document.security_form.RadiusServerIP) == false){
		alert(_("secure invalid radius ip"));
		return false;
	}
	if( (checkRange(document.security_form.RadiusServerPort.value, 1, 1, 65535)==false) ||
		(checkAllNum(document.security_form.RadiusServerPort.value)==false)){
		alert(_("secure invalid radius port"));
		return false;
	}
	if(checkStrictInjection(document.security_form.RadiusServerSecret.value)==false){
		alert(_("secure invalid radius secret"));
		return false;
	}

	if(document.security_form.RadiusServerSessionTimeout.value.length){
		if(checkAllNum(document.security_form.RadiusServerSessionTimeout.value)==false){
			alert(_("secure invalid timeout"));
			return false;
		}	
	}

	return true;
}

function securityMode(c_f)
{
	var security_mode;

	changed = c_f;

	hideWep();

	displayElement( [
		'div_security_shared_mode', 'div_wpa', 'div_wpa_algorithms', 'wpa_passphrase',
		'wpa_key_renewal_interval', 'wpa_PMK_Cache_Period', 'wpa_preAuthentication',
		'div_radius_server', 'div_8021x_wep' // 802.1x
		], false);

	document.security_form.cipher[0].disabled = true;
	document.security_form.cipher[1].disabled = true;
	document.security_form.cipher[2].disabled = true;
	document.security_form.passphrase.disabled = true;
	document.security_form.keyRenewalInterval.disabled = true;
	document.security_form.PMKCachePeriod.disabled = true;
	document.security_form.PreAuthentication.disabled = true;

	// 802.1x
	document.security_form.ieee8021x_wep.disable = true;
	document.security_form.RadiusServerIP.disable = true;
	document.security_form.RadiusServerPort.disable = true;
	document.security_form.RadiusServerSecret.disable = true;
	document.security_form.RadiusServerSessionTimeout.disable = true;
	document.security_form.RadiusServerIdleTimeout.disable = true;

	security_mode = document.security_form.security_mode.value;

	if (security_mode == "OPEN" || security_mode == "SHARED" ||security_mode == "WEPAUTO"){
		showWep(security_mode);
	}else if (security_mode == "WPAPSK" || security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK"){
		// WPA
		document.getElementById("div_wpa").style.visibility = "visible";
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wpa").style.display = "block";
		}
		else if (window.XMLHttpRequest) { // Mozilla, Safari,...
			document.getElementById("div_wpa").style.display = "table";
		}

		document.getElementById("div_wpa_algorithms").style.visibility = "visible";
		document.getElementById("div_wpa_algorithms").style.display = '';
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;

		// deal with TKIP-AES mixed mode
		// WTF?
		// if(security_mode == "WPAPSK" || security_mode == "WPA2PSK" || security_mode == "WPAPSKWPA2PSK")
			document.security_form.cipher[2].disabled = false;

		document.getElementById("wpa_passphrase").style.visibility = "visible";
		document.getElementById("wpa_passphrase").style.display = '';
		document.security_form.passphrase.disabled = false;

		document.getElementById("wpa_key_renewal_interval").style.visibility = "visible";
		document.getElementById("wpa_key_renewal_interval").style.display = '';
		document.security_form.keyRenewalInterval.disabled = false;
		if (document.security_form.cipher[0].checked == document.security_form.cipher[1].checked == document.security_form.cipher[2].checked) {
			document.security_form.cipher[1].checked = true;
		}
	}else if (security_mode == "WPA" || security_mode == "WPA2" || security_mode == "WPA1WPA2") //wpa enterprise
	{
		document.getElementById("div_wpa").style.visibility = "visible";
		if (window.ActiveXObject) { // IE
			document.getElementById("div_wpa").style.display = "block";
		}else if (window.XMLHttpRequest) { // Mozilla, Safari,...
			document.getElementById("div_wpa").style.display = "table";
		}

		document.getElementById("div_wpa_algorithms").style.visibility = "visible";
		document.getElementById("div_wpa_algorithms").style.display = '';
		document.security_form.cipher[0].disabled = false;
		document.security_form.cipher[1].disabled = false;
		document.getElementById("wpa_key_renewal_interval").style.visibility = "visible";
		document.getElementById("wpa_key_renewal_interval").style.display = '';
		document.security_form.keyRenewalInterval.disabled = false;
	
		// 802.1x
		document.getElementById("div_radius_server").style.visibility = "visible";
		document.getElementById("div_radius_server").style.display = '';
		document.security_form.RadiusServerIP.disable = false;
		document.security_form.RadiusServerPort.disable = false;
		document.security_form.RadiusServerSecret.disable = false;	
		document.security_form.RadiusServerSessionTimeout.disable = false;
		document.security_form.RadiusServerIdleTimeout.disable = false;	

		if(security_mode == "WPA")
			document.security_form.cipher[2].disabled = false;

		if(security_mode == "WPA2"){
			document.security_form.cipher[2].disabled = false;
			document.getElementById("wpa_preAuthentication").style.visibility = "visible";
			document.getElementById("wpa_preAuthentication").style.display = '';
			document.security_form.PreAuthentication.disabled = false;
			document.getElementById("wpa_PMK_Cache_Period").style.visibility = "visible";
			document.getElementById("wpa_PMK_Cache_Period").style.display = '';
			document.security_form.PMKCachePeriod.disabled = false;
		}

		if(security_mode == "WPA1WPA2"){
			document.security_form.cipher[2].disabled = false;
		}
		if (document.security_form.cipher[0].checked == document.security_form.cipher[1].checked == document.security_form.cipher[2].checked) {
			document.security_form.cipher[1].checked = true;
		}
	}else if (security_mode == "IEEE8021X"){ // 802.1X-WEP
		document.getElementById("div_8021x_wep").style.visibility = "visible";
		document.getElementById("div_8021x_wep").style.display = '';

		document.getElementById("div_radius_server").style.visibility = "visible";
		document.getElementById("div_radius_server").style.display = '';
		document.security_form.ieee8021x_wep.disable = false;
		document.security_form.RadiusServerIP.disable = false;
		document.security_form.RadiusServerPort.disable = false;
		document.security_form.RadiusServerSecret.disable = false;	
		document.security_form.RadiusServerSessionTimeout.disable = false;
		//document.security_form.RadiusServerIdleTimeout.disable = false;
	}
}


function hideWep()
{
	document.getElementById("div_wep").style.visibility = "hidden";
	document.getElementById("div_wep").style.display = "none";
}
function showWep(mode)
{
	<!-- WEP -->
	document.getElementById("div_wep").style.visibility = "visible";

	if (window.ActiveXObject) { // IE 
		document.getElementById("div_wep").style.display = "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari...
		document.getElementById("div_wep").style.display = "table";
	}

	if(mode == "SHARED"){
		document.getElementById("div_security_shared_mode").style.visibility = "visible";
		document.getElementById("div_security_shared_mode").style.display = '';
	}
	//document.security_form.wep_auth_type.disabled = false;
}


function check_Wep(securitymode)
{
	var defaultid = document.security_form.wep_default_key.value;
	var key_input;

	if ( defaultid == 1 )
		var keyvalue = document.security_form.wep_key_1.value;
	else if (defaultid == 2)
		var keyvalue = document.security_form.wep_key_2.value;
	else if (defaultid == 3)
		var keyvalue = document.security_form.wep_key_3.value;
	else if (defaultid == 4)
		var keyvalue = document.security_form.wep_key_4.value;

	if (keyvalue.length == 0 &&  (securitymode == "SHARED" || securitymode == "OPEN" || securitymode == "WEPAUTO")){ // shared wep  || md5
		alert(_("secure no wep key")+defaultid+' !');
		return false;
	}

	var keylength = document.security_form.wep_key_1.value.length;
	if (keylength != 0){
		if (document.security_form.WEP1Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert(_("secure short wep key"));
				document.security_form.wep_key_1.focus();
				return false;
			}
			if(checkInjection(document.security_form.wep_key_1.value)== false){
				alert(_("secure invalid wep key"));
				document.security_form.wep_key_1.focus();
				return false;
			}
		}
		if (document.security_form.WEP1Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert(_("secure long wep key"));
				document.security_form.wep_key_1.focus();
				return false;
			}
			if(checkHex(document.security_form.wep_key_1.value) == false){
				alert(_("secure invalid key"));
				document.security_form.wep_key_1.focus();
				return false;
			}
		}
	}

	keylength = document.security_form.wep_key_2.value.length;
	if (keylength != 0){
		if (document.security_form.WEP2Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert(_("secure short wep key"));
				document.security_form.wep_key_2.focus();
				return false;
			}
			if(checkInjection(document.security_form.wep_key_2.value)== false){
				alert(_("secure invalid wep key"));
				document.security_form.wep_key_2.focus();
				return false;
			}			
		}
		if (document.security_form.WEP2Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert(_("secure long wep key"));
				document.security_form.wep_key_2.focus();
				return false;
			}
			if(checkHex(document.security_form.wep_key_2.value) == false){
				alert(_("secure invalid key"));
				document.security_form.wep_key_2.focus();
				return false;
			}
		}
	}

	keylength = document.security_form.wep_key_3.value.length;
	if (keylength != 0){
		if (document.security_form.WEP3Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert(_("secure short wep key"));
				document.security_form.wep_key_3.focus();
				return false;
			}
			if(checkInjection(document.security_form.wep_key_3.value)== false){
				alert(_("secure invalid wep key"));
				document.security_form.wep_key_3.focus();
				return false;
			}
		}
		if (document.security_form.WEP3Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert(_("secure long wep key"));
				document.security_form.wep_key_3.focus();
				return false;
			}
			if(checkHex(document.security_form.wep_key_3.value) == false){
				alert(_("secure invalid key"));
				document.security_form.wep_key_3.focus();
				return false;
			}			
		}
	}

	keylength = document.security_form.wep_key_4.value.length;
	if (keylength != 0){
		if (document.security_form.WEP4Select.options.selectedIndex == 0){
			if(keylength != 5 && keylength != 13) {
				alert(_("secure short wep key"));
				document.security_form.wep_key_4.focus();
				return false;
			}
			if(checkInjection(document.security_form.wep_key_4.value)== false){
				alert(_("secure invalid wep key"));
				document.security_form.wep_key_4.focus();
				return false;
			}			
		}
		if (document.security_form.WEP4Select.options.selectedIndex == 1){
			if(keylength != 10 && keylength != 26) {
				alert(_("secure long wep key"));
				document.security_form.wep_key_4.focus();
				return false;
			}

			if(checkHex(document.security_form.wep_key_4.value) == false){
				alert(_("secure invalid key"));
				document.security_form.wep_key_4.focus();
				return false;
			}			
		}
	}
	return true;
}
	
function submit_apply()
{
	if (checkData() == true)
	{
		changed = 0;
		return true;
	}
	return false;
}

function LoadFields(MBSSID)
{
	var result;

	// Security Policy
	var b8021x = '<% get802_1XBuilt(); %>';
	var sp_select = document.getElementById("security_mode");
	sp_select.options.length = 0;

	for (var i=0; i<security_modes_list.length; i++)
	{
		var mode = security_modes_list[i];
		var setup = true;
		if ((mode[3] > 0) && (MBSSID == 0)) // 802.1x support only for MBSSID 0
			setup = false;
		if ((mode[2] > 0) && (b8021x == '0'))
			setup = false;
		
		if (setup)
			sp_select.options.add(new Option(mode[0], mode[1]));
	}

	for (var i=0; i<sp_select.options.length; i++)
		if (sp_select.options[i].value == AuthMode[MBSSID])
		{
			sp_select.options[i].selected = true;
			break;
		}

	// WEP
	document.getElementById("WEP1").value = Key1Str[MBSSID];
	document.getElementById("WEP2").value = Key2Str[MBSSID];
	document.getElementById("WEP3").value = Key3Str[MBSSID];
	document.getElementById("WEP4").value = Key4Str[MBSSID];

	document.getElementById("WEP1Select").selectedIndex = (Key1Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP2Select").selectedIndex = (Key2Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP3Select").selectedIndex = (Key3Type[MBSSID] == "0" ? 1 : 0);
	document.getElementById("WEP4Select").selectedIndex = (Key4Type[MBSSID] == "0" ? 1 : 0);

	document.getElementById("wep_default_key").selectedIndex = parseInt(DefaultKeyID[MBSSID]) - 1 ;

	// SHARED && NONE
	if(AuthMode[MBSSID] == "SHARED" && EncrypType[MBSSID] == "NONE")
		document.getElementById("security_shared_mode").selectedIndex = 1;
	else
		document.getElementById("security_shared_mode").selectedIndex = 0;

	// WPA
	if(EncrypType[MBSSID] == "TKIP")
		document.security_form.cipher[0].checked = true;
	else if(EncrypType[MBSSID] == "AES")
		document.security_form.cipher[1].checked = true;
	else if(EncrypType[MBSSID] == "TKIPAES")
		document.security_form.cipher[2].checked = true;

	document.getElementById("passphrase").value = WPAPSK[MBSSID];
	document.getElementById("keyRenewalInterval").value = RekeyInterval[MBSSID];
	document.getElementById("PMKCachePeriod").value = PMKCachePeriod[MBSSID];
	//document.getElementById("PreAuthentication").value = PreAuth[MBSSID];
	if(PreAuth[MBSSID] == "0")
		document.security_form.PreAuthentication[0].checked = true;
	else
		document.security_form.PreAuthentication[1].checked = true;

	//802.1x wep
	if(IEEE8021X[MBSSID] == "1"){
		if(EncrypType[MBSSID] == "WEP")
			document.security_form.ieee8021x_wep[1].checked = true;
		else
			document.security_form.ieee8021x_wep[0].checked = true;
	}
	
	document.getElementById("RadiusServerIP").value = RADIUS_Server[MBSSID];
	document.getElementById("RadiusServerPort").value = RADIUS_Port[MBSSID];
	document.getElementById("RadiusServerSecret").value = RADIUS_Key[MBSSID];
	document.getElementById("RadiusServerSessionTimeout").value = session_timeout_interval[MBSSID];
	
	securityMode(0);
}


function ShowAP(MBSSID)
{
	var i;
	for(i=0; i<MBSSID_MAX; i++){
		document.getElementById("apselect_"+i).selectedIndex	= AccessPolicy[i];
		document.getElementById("AccessPolicy_"+i).style.visibility = "hidden";
		document.getElementById("AccessPolicy_"+i).style.display = "none";
	}

	document.getElementById("AccessPolicy_"+MBSSID).style.visibility = "visible";
	if (window.ActiveXObject) {			// IE
		document.getElementById("AccessPolicy_"+MBSSID).style.display = "block";
	}else if (window.XMLHttpRequest) {	// Mozilla, Safari,...
		document.getElementById("AccessPolicy_"+MBSSID).style.display = "table";
	}
}

function LoadAP()
{
	for(var i=0; i<SSID.length; i++){
		var j=0;
		var aplist = new Array;

		if(AccessControlList[i].length != 0){
			aplist = AccessControlList[i].split(";");
			for(j=0; j<aplist.length; j++){
				document.getElementById("newap_"+i+"_"+j).value = aplist[j];
			}

			// hide the lastest <td>
			if(j%2){
				document.getElementById("newap_td_"+i+"_"+j).style.visibility = "hidden";
				document.getElementById("newap_td_"+i+"_"+j).style.display = "none";
				j++;
			}
		}

		// hide <tr> left
		for(; j<ACCESSPOLICYLIST_MAX; j+=2){
			document.getElementById("id_"+i+"_"+j).style.visibility = "hidden";
			document.getElementById("id_"+i+"_"+j).style.display = "none";
		}
	}
}

function selectMBSSIDChanged()
{
	// check if any security settings changed
	if(changed){
		ret = confirm(_("secure confirm"));
		if(!ret){
			document.security_form.ssidIndex.options.selectedIndex = old_MBSSID;
			return false;
		}
		else
			changed = 0;
	}

	var selected = document.security_form.ssidIndex.options.selectedIndex;

	// backup for user cancel action
	old_MBSSID = selected;

	MBSSIDChange(selected);
}

/*
 * When user select the different SSID, this function would be called.
 */
function MBSSIDChange(selected)
{
	// load wep/wpa/802.1x table for MBSSID[selected]
	LoadFields(selected);

	// update Access Policy for MBSSID[selected]
	ShowAP(selected);

	// radio button special case
	WPAAlgorithms = EncrypType[selected];
	IEEE8021XWEP = IEEE8021X[selected];
	PreAuthentication = PreAuth[selected];

	changeSecurityPolicyTableTitle(SSID[selected]);

	// clear all new access policy list field
	for(i=0; i<MBSSID_MAX; i++)
		document.getElementById("newap_text_"+i).value = "";

	return true;
}

function changeSecurityPolicyTableTitle(t)
{
	var title = document.getElementById("sp_title");
	title.innerHTML = "\"" + t + "\"";
}

function delap(mbssid, num)
{
	makeRequest("/goform/APDeleteAccessPolicyList", mbssid+ "," +num, deleteAccessPolicyListHandler);
}

function initTranslation()
{
	_TR("secureSelectSSID", "secure select ssid");
	_TR("secureSSIDChoice", "secure ssid choice");

	_TR("securityTitle", "secure ssid title");
	_TR("securityIntroduction", "secure ssid introduction");
	_TR("sp_title", "secure security policy");
	_TR("secureSecureMode", "secure security mode");
	_TR("secureEncrypType", "secure encryp type");
	_TR("secureEncrypTypeNone", "wireless none");

	_TR("secureWEP", "secure wep");
	_TR("secureWEPDefaultKey", "secure wep default key");
	_TR("secureWEPDefaultKey1", "secure wep default key1");
	_TR("secureWEPDefaultKey2", "secure wep default key2");
	_TR("secureWEPDefaultKey3", "secure wep default key3");
	_TR("secureWEPDefaultKey4", "secure wep default key4");
	_TR("secureWEPKey", "secure wep key");
	_TR("secureWEPKey1", "secure wep key1");
	_TR("secureWEPKey2", "secure wep key2");
	_TR("secureWEPKey3", "secure wep key3");
	_TR("secureWEPKey4", "secure wep key4");

	_TR("secreWPA", "secure wpa");
	_TR("secureWPAAlgorithm", "secure wpa algorithm");
	_TR("secureWPAPassPhrase", "secure wpa pass phrase");
	_TR("secureWPAKeyRenewInterval", "secure wpa key renew interval");
	_TR("secureWPAPMKCachePeriod", "secure wpa pmk cache period");
	_TR("secureWPAPreAuth", "secure wpa preauth");
	_TR("secureWPAPreAuthDisable", "wireless disable");
	_TR("secureWPAPreAuthEnable", "wireless enable");

	_TR("secure8021XWEP", "secure 8021x wep");
	_TR("secure1XWEP", "secure 1x wep");
	_TR("secure1XWEPDisable", "wireless disable");
	_TR("secure1XWEPEnable", "wireless enable");

	_TR("secureRadius", "secure radius");
	_TR("secureRadiusIPAddr", "secure radius ipaddr");
	_TR("secureRadiusPort", "secure radius port");
	_TR("secureRadiusSharedSecret", "secure radius shared secret");
	_TR("secureRadiusSessionTimeout", "secure radius session timeout");
	_TR("secureRadiusIdleTimeout", "secure radius idle timeout");

	_TRV("secureApply", "wireless apply");
	_TRV("secureCancel", "wireless cancel");
}

function initAll()
{
	initTranslation();
	makeRequest("/goform/wirelessGetSecurity", "n/a", securityHandler);
}

function UpdateMBSSIDList()
{
	document.security_form.ssidIndex.length = 0;

	for(var i=0; i<SSID.length; i++){
		var j = document.security_form.ssidIndex.options.length;
		document.security_form.ssidIndex.options[j] = new Option(SSID[i], i, false, false);
	}
	
	document.security_form.ssidIndex.options.selectedIndex = defaultShownMBSSID;
	old_MBSSID = defaultShownMBSSID;
	changeSecurityPolicyTableTitle(SSID[defaultShownMBSSID]);
}

function setChange(c){
	changed = c;
}

var WPAAlgorithms;
function onWPAAlgorithmsClick(type)
{
	if(type == 0 && WPAAlgorithms == "TKIP") return;
	if(type == 1 && WPAAlgorithms == "AES") return;
	if(type == 2 && WPAAlgorithms == "TKIPAES") return;
	setChange(1);
}

var IEEE8021XWEP;
function onIEEE8021XWEPClick(type)
{
	if(type == 0 && IEEE8021XWEP == false) return;
	if(type == 1 && IEEE8021XWEP == true) return;
	setChange(1);
}

var PreAuthentication;
function onPreAuthenticationClick(type)
{
	if(type == 0 && PreAuthentication == false) return;
	if(type == 1 && PreAuthentication == true) return;
	setChange(1);
}

</script>
</head>

<body onLoad="initAll();">
<table class="body">
  <tbody>
    <tr>
      <td><h1 id="securityTitle">Wireless Security/Encryption Settings </h1>
        <p id="securityIntroduction">Here you can configure wireless security and encryption to prevent unauthorized access to the router.</p>
        <hr />
        <form method="post" name="security_form" action="/goform/APSecurity" onSubmit="return submit_apply();">
          
          <!-- ---------------------  MBSSID Selection  --------------------- -->
          <table class="form">
            <tr>
              <td class="title" colspan="2" id="secureSelectSSID">Select SSID</td>
            </tr>
            <tr>
              <td class="head" id="secureSSIDChoice">SSID choice</td>
              <td><select name="ssidIndex" class="mid" onChange="selectMBSSIDChanged();">
                  <!-- ....Javascript will update options.... -->
                </select></td>
            </tr>
          </table>
          <table class="form">
            <tr>
              <td class="title" colspan="2"><span id="sp_title">Security Policy </span></td>
            </tr>
            <tr id="div_security_infra_mode" name="div_security_infra_mode">
              <td class="head" id="secureSecureMode">Security Mode</td>
              <td><select name="security_mode" id="security_mode" class="mid" onChange="securityMode(1)">
                  <!-- ....Javascript will update options.... -->
                </select></td>
            </tr>
            <tr id="div_security_shared_mode" name="div_security_shared_mode" style="visibility: hidden;">
              <td class="head" id="secureEncrypType">Encrypt Type</td>
              <td><select name="security_shared_mode" id="security_shared_mode" class="mid" onChange="securityMode(1)">
                  <option value=WEP>WEP</option>
                  <option value=None id="secureEncrypTypeNone">None</option>
                </select></td>
            </tr>
          </table>
          <!-- WEP -->
          <table id="div_wep" name="div_wep" class="form" style="visibility: hidden;">
            <tr>
              <td class="title" colspan="4" id="secureWEP">Wired Equivalent Privacy (WEP)</td>
            </tr>
            <tr>
              <td class="head" colspan="2" id="secureWEPDefaultKey">Default Key</td>
              <td colspan="2"><select name="wep_default_key" id="wep_default_key" class="half" onChange="setChange(1)">
                  <option value="1" id="secureWEPDefaultKey1">Key 1</option>
                  <option value="2" id="secureWEPDefaultKey2">Key 2</option>
                  <option value="3" id="secureWEPDefaultKey3">Key 3</option>
                  <option value="4" id="secureWEPDefaultKey4">Key 4</option>
                </select></td>
            </tr>
            <tr>
              <td class="head1" rowspan="4" id="secureWEPKey">WEP Keys</td>
              <td class="head2" id="secureWEPKey1">WEP Key 1 :</td>
              <td><input name="wep_key_1" id="WEP1" maxlength="26" value="" onKeyUp="setChange(1)"></td>
              <td><select id="WEP1Select" class="half" name="WEP1Select" onChange="setChange(1)">
                  <option value="1">ASCII</option>
                  <option value="0">Hex</option>
                </select></td>
            </tr>
            <tr>
              <td class="head2" id="secureWEPKey2">WEP Key 2 : </td>
              <td><input name="wep_key_2" id="WEP2" maxlength="26" value="" onKeyUp="setChange(1)"></td>
              <td><select id="WEP2Select" name="WEP2Select" class="half" onChange="setChange(1)">
                  <option value="1">ASCII</option>
                  <option value="0">Hex</option>
                </select></td>
            </tr>
            <tr>
              <td class="head2" id="secureWEPKey3">WEP Key 3 : </td>
              <td><input name="wep_key_3" id="WEP3" maxlength="26" value="" onKeyUp="setChange(1)"></td>
              <td><select id="WEP3Select" name="WEP3Select" class="half" onChange="setChange(1)">
                  <option value="1">ASCII</option>
                  <option value="0">Hex</option>
                </select></td>
            </tr>
            <tr>
              <td class="head2" id="secureWEPKey4">WEP Key 4 : </td>
              <td><input name="wep_key_4" id="WEP4" maxlength="26" value="" onKeyUp="setChange(1)"></td>
              <td><select id="WEP4Select" name="WEP4Select" class="half" onChange="setChange(1)">
                  <option value="1">ASCII</option>
                  <option value="0">Hex</option>
                </select></td>
            </tr>
          </table>
          
          <!-- <br> --> 
          
          <!-- WPA -->
          <table id="div_wpa" name="div_wpa" class="form" style="visibility: hidden; display: none;">
            <tr>
              <td class="title" colspan="2" id="secreWPA">WPA</td>
            </tr>
            <tr id="div_wpa_algorithms" name="div_wpa_algorithms" style="visibility: hidden;">
              <td class="head" id="secureWPAAlgorithm">WPA Algorithms</td>
              <td><input name="cipher" id="cipher" value="0" type="radio" onClick="onWPAAlgorithmsClick(0)">
                TKIP &nbsp;
                <input name="cipher" id="cipher" value="1" type="radio" onClick="onWPAAlgorithmsClick(1)">
                AES &nbsp;
                <input name="cipher" id="cipher" value="2" type="radio" onClick="onWPAAlgorithmsClick(2)">
                TKIP/AES &nbsp; </td>
            </tr>
            <tr id="wpa_passphrase" name="wpa_passphrase" style="visibility: hidden;">
              <td class="head" id="secureWPAPassPhrase">Pass Phrase</td>
              <td><input name="passphrase" id="passphrase" size="28" maxlength="64" value="" onKeyUp="setChange(1)"></td>
            </tr>
            <tr id="wpa_key_renewal_interval" name="wpa_key_renewal_interval" style="visibility: hidden;">
              <td class="head" id="secureWPAKeyRenewInterval">Key Renewal Interval</td>
              <td><input name="keyRenewalInterval" id="keyRenewalInterval" size="4" maxlength="4" value="3600" onKeyUp="setChange(1)">
                seconds </td>
            </tr>
            <tr id="wpa_PMK_Cache_Period" name="wpa_PMK_Cache_Period" style="visibility: hidden;">
              <td class="head" id="secureWPAPMKCachePeriod">PMK Cache Period</td>
              <td><input name="PMKCachePeriod" id="PMKCachePeriod" size="4" maxlength="4" value="" onKeyUp="setChange(1)">
                minute </td>
            </tr>
            <tr id="wpa_preAuthentication" name="wpa_preAuthentication" style="visibility: hidden;">
              <td class="head" id="secureWPAPreAuth">Pre-Authentication</td>
              <td><input name="PreAuthentication" id="PreAuthentication" value="0" type="radio" onClick="onPreAuthenticationClick(0)">
                <font id="secureWPAPreAuthDisable">Disable &nbsp;</font>
                <input name="PreAuthentication" id="PreAuthentication" value="1" type="radio" onClick="onPreAuthenticationClick(1)">
                <font id="secureWPAPreAuthEnable">Enable &nbsp;</font></td>
            </tr>
          </table>
          
          <!-- 802.1x --> 
          <!-- WEP  -->
          <table id="div_8021x_wep" name="div_8021x_wep" class="form" style="visibility: hidden; display: none;">
            <tr>
              <td class="title" colspan="2" id="secure8021XWEP">802.1x WEP</td>
            </tr>
            <tr>
              <td class="head" id="secure1XWEP">WEP</td>
              <td><input name="ieee8021x_wep" id="ieee8021x_wep" value="0" type="radio" onClick="onIEEE8021XWEPClick(0)">
                <span id="secure1XWEPDisable">Disable &nbsp;</span>
                <input name="ieee8021x_wep" id="ieee8021x_wep" value="1" type="radio" onClick="onIEEE8021XWEPClick(1)">
                <span id="secure1XWEPEnable">Enable</span></td>
            </tr>
          </table>
          <table id="div_radius_server" name="div_radius_server" class="form" style="visibility: hidden; display: none;">
            <tr>
              <td class="title" colspan="2" id="secureRadius">Radius Server</td>
            </tr>
            <tr>
              <td class="head" id="secureRadiusIPAddr"> IP Address </td>
              <td><input name="RadiusServerIP" id="RadiusServerIP" size="16" maxlength="32" value="" onKeyUp="setChange(1)"></td>
            </tr>
            <tr>
              <td class="head" id="secureRadiusPort"> Port </td>
              <td><input name="RadiusServerPort" id="RadiusServerPort" size="5" maxlength="5" value="" onKeyUp="setChange(1)"></td>
            </tr>
            <tr>
              <td class="head" id="secureRadiusSharedSecret"> Shared Secret </td>
              <td><input name="RadiusServerSecret" id="RadiusServerSecret" size="16" maxlength="64" value="" onKeyUp="setChange(1)"></td>
            </tr>
            <tr>
              <td class="head" id="secureRadiusSessionTimeout"> Session Timeout </td>
              <td><input name="RadiusServerSessionTimeout" id="RadiusServerSessionTimeout" size="3" maxlength="4" value="0" onKeyUp="setChange(1)"></td>
            </tr>
            <tr>
              <td class="head" id="secureRadiusIdleTimeout"> Idle Timeout </td>
              <td><input name="RadiusServerIdleTimeout" id="RadiusServerIdleTimeout" size="3" maxlength="4" value="" onKeyUp="setChange(1)" readonly></td>
            </tr>
          </table>
          
          <!--									--> 
          <!--	AccessPolicy for mbssid 		--> 
          <!--									--> 
          
          <script language="JavaScript" type="text/javascript">
var aptable;

for (aptable = 0; aptable < MBSSID_MAX; aptable++)
{
	document.write(" <table id=\"AccessPolicy_"+ aptable +"\" border=\"1\" class=\"form\">");
	document.write(" <tbody> <tr> <td class=title colspan=2 >"+_("secure access policy")+"</td></tr>");
	document.write(" <tr> <td bgcolor=#E8F8FF class=head >"+_("secure access policy capable")+"</td>");
	document.write(" <td> <select name=apselect_"+ aptable + " id=apselect_"+aptable+" class=\"mid\" onchange=\"setChange(1)\">");
	document.write(" 			<option value=0 >"+_("wireless disable")+"</option> <option value=1 >"+_("wireless allow")+"</option><option value=2 >"+_("wireless reject")+"</option></select> </td></tr>");

	for(i=0; i< ACCESSPOLICYLIST_MAX/2; i++){
		input_name = "newap_"+ aptable +"_" + (2*i);
		td_name = "newap_td_"+ aptable +"_" + (2*i);

		document.write(" <tr id=id_"+aptable+"_");
		document.write(i*2);
		document.write("> <td id=");
		document.write(td_name);
		document.write("> <input style=\"width: 30px;\" value=Del onclick=\"delap("+aptable+", ");
		document.write(2*i);
		document.write(")\" type=button > <input id=");
		document.write(input_name);
		document.write(" size=16 maxlength=20 readonly></td>");

		input_name = "newap_" + aptable + "_" + (2*i+1);
		td_name = "newap_td_" + aptable + "_" + (2*i+1);
		document.write("      <td id=");
		document.write(td_name);
		document.write("> <input style=\"width: 30px;\" value=Del onclick=\"delap("+aptable+", ");
		document.write(2*i+1);
		document.write(")\" type=button> <input id=");
		document.write(input_name);
		document.write(" size=16 maxlength=20 readonly></td> </tr>");
	}

	document.write("<tr><td class=\"head\">"+_("secure access policy new")+"</td>");
	document.write("	<td><input name=\"newap_text_"+aptable+"\" id=\"newap_text_"+aptable+"\" size=\"16\" maxlength=\"20\"></td></tr></tbody></table>");
}
</script> 
          
          <!-- <br> -->
          <table border="0" cellpadding="2" cellspacing="1" width="90%">
            <tr align="center">
              <td><input style="width: 120px;" value="Apply" id="secureApply" type="submit">
                &nbsp; &nbsp;
                <input style="width: 120px;" value="Cancel" id="secureCancel" type="reset" onClick="window.location.reload();" >
                <input type="hidden" name="submit-url" value="/wireless/security.asp" ></td>
            </tr>
          </table>
        </form>
        <div class="whitespace">&nbsp;</div></td>
    </tr>
  </tbody>
</table>
</body>
</html>
