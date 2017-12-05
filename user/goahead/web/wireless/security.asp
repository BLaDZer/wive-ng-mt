<!DOCTYPE html>
<html>
	<head>
		<title>Wireless Security Settings</title>
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
		<script src="/js/validation.js"></script>
		<script>
			var old_MBSSID;
			var WPAAlgorithms;
			var PreAuthentication;
			var network				=	{};
			var securityChanged		=	false;
			var http_request		=	false;
			var security_modes_list	=	[
											[ _("wireless disable"),	"Disable",			0, 0 ],
											[ "WEP",					"WEPAUTO",			0, 0 ],
											[ "WPA (Enterprise)",		"WPA",				1, 1 ],
											[ "WPA-PSK (Personal)",		"WPAPSK",			0, 0 ],
											[ "WPA2 (Enterprise)",		"WPA2",				1, 1 ],
											[ "WPA2-PSK (Personal)",	"WPA2PSK",			0, 0 ],
											[ "WPA1/2-PSK (Personal)",	"WPAPSKWPA2PSK",	0, 0 ],
											[ "WPA1/2 (Enterprise)",	"WPA1WPA2",			1, 1 ]
										];

			// Language translation
			function initTranslation() {
				_TR("secureSelectSSID",				"secure select ssid");
				_TR("secureSSIDChoice",				"secure ssid choice");
				_TR("securityTitle",				"secure ssid title");
				_TR("securityIntroduction",			"secure ssid introduction");
				_TR("sp_title",						"secure security policy");
				_TR("secureSecureMode",				"secure security mode");

				_TR("secureWEP",					"secure wep");
				_TR("secureWEPDefaultKey",			"secure wep default key");
				_TR("secureWEPDefaultKey1",			"secure wep default key1");
				_TR("secureWEPDefaultKey2",			"secure wep default key2");
				_TR("secureWEPDefaultKey3",			"secure wep default key3");
				_TR("secureWEPDefaultKey4",			"secure wep default key4");
				_TR("secureWEPKey",					"secure wep key");
				_TR("secureWEPKey1",				"secure wep key1");
				_TR("secureWEPKey2",				"secure wep key2");
				_TR("secureWEPKey3",				"secure wep key3");
				_TR("secureWEPKey4",				"secure wep key4");

				_TR("secureWPA",					"secure wpa");
				_TR("secureWPAAlgorithm",			"secure wpa algorithm");
				
				if (old_MBSSID == 0 && BUILD_5GHZ_SUPPORT && NVRAM_SSID1 != NVRAM_SSID1INIC)
					_TR("secureWPAPassPhrase",		"secure wpa pass phrase nic");
				else
					_TR("secureWPAPassPhrase",		"secure wpa pass phrase");


				_TR("secureWPAPassPhraseInic",		"secure wpa pass phrase inic");
				_TR("secureWPAKeyRenewInterval",	"secure wpa key renew interval");
				_TR("secureWPAPMKCachePeriod",		"secure wpa pmk cache period");
				_TR("secureWPAPreAuth",				"secure wpa preauth");
				_TR("secureWPAPreAuthDisable",		"wireless disable");
				_TR("secureWPAPreAuthEnable",		"wireless enable");

				_TR("secureRadius",					"secure radius");
				_TR("secureRadiusIPAddr",			"secure radius ipaddr");
				_TR("secureRadiusPort",				"secure radius port");
				_TR("secureRadiusSharedSecret",		"secure radius shared secret");
				_TR("secureRadiusSessionTimeout",	"secure radius session timeout");
				_TR("secureShowPass",				"secure wpa show pass phrase");
				_TR("secureShowPass1",				"secure wpa show pass phrase");
				_TR("secureShowPass2",				"secure wpa show pass phrase");
				_TR("secureShowPass3",				"secure wpa show pass phrase");
				_TR("secureShowPass4",				"secure wpa show pass phrase");
				_TR("secureShowPass5",				"secure wpa show pass phrase");
				_TR("secureShowPass6",				"secure wpa show pass phrase");
				
				_TR("PreAuthenticationDisable",		"button disable");
				_TR("PreAuthenticationEnable",		"button enable");
				_TR("secureKeySeconds",				"secure key seconds");
				_TR("secureKeyMinutes",				"secure key minutes");
				
				_TR("secureApply",					"button apply");
				_TR("secureCancel",				"button cancel");
				_TR("secureReset",					"button reset");
			}

			// Init values on page load
			function initValues() {
				showWarning();
				initTranslation();

				var html = '';
				for (var i = 0; i < MBSSID_MAX; i++)
					html += '<input type="hidden" name="AccessControlList' + i + '" id="AccessControlList' + i + '" value="">';
				document.getElementById('accessPolicyInput').innerHTML = html;	

				createAccessPolicyTable();
				makeRequest("/goform/wirelessGetSecurity", "n/a", securityHandler);
				var br = getBrowser();
				if (br.browser == 'ie' && br.versionShort <= 8)
					displayElement([document.getElementById('passphrase_type'), document.getElementById('passphrase_inic_type'),
									'secureWEPKey1_show', 'secureWEPKey2_show', 'secureWEPKey3_show', 'secureWEPKey4_show',
									'secureRadiusSharedSecret_show'], false);
			}

			// Check values on form submit
			function checkValues() {
				switch (document.getElementById('security_mode').value) {
					case 'WEPAUTO':		
											if (!checkWEP())
												return false;
											break;
					case 'WPAPSK':
					case 'WPA2PSK':
					case 'WPAPSKWPA2PSK':
											if (document.getElementById('passphrase').value.length == 0) {
												document.getElementById('passphrase').focus();
												alert(_("secure no key"));
												return false;
											}

											if (document.getElementById('passphrase').value.length < 8) {
												document.getElementById('passphrase').focus();
												document.getElementById('passphrase').select();
												alert(_("apcli short phrase"));
												return false;
											}

											if (!checkInjection(document.getElementById('passphrase').value)) {
												document.getElementById('passphrase').focus();
												document.getElementById('passphrase').select();
												alert(_("apcli chars not allowed"));
												return false;
											}

											if (BUILD_5GHZ_SUPPORT == '1' && NVRAM_RadioOnINIC == '1')
												if (!checkInjection(document.getElementById('passphraseinic').value)) {
													document.getElementById('passphraseinic').focus();
													document.getElementById('passphraseinic').select();
													alert(_("apcli chars not allowed"));
													return false;
												}

											if(!checkWPA())
												return false;
											break;
					case 'WPA':
					case 'WPA1WPA2':
											if(!checkWPA())
												return false;
											if(!checkRadius())
												return false;
											break;
					case 'WPA2':
											if(!checkWPA())
												return false;

											if(!document.getElementById('PMKCachePeriod').value.length){
												alert(_("secure no pmk"));
												return false;
											}
											if(checkRadius() == false)
												return false;
				}

				// check Access Policy
				for(i = 0; i < MBSSID_MAX; i++) {
					if (document.getElementById("newap_text_" + i).value != '')
						if (!validateMAC(document.getElementById("newap_text_" + i).value)) {
							alert(_("secure invalid mac"));
							return false;
						}
					if (i < network.data.length)
						document.getElementById('AccessControlList' + i).value = network.data[i].AccessControlList;
					else 
						document.getElementById('AccessControlList' + i).value = '';
				}

				if (document.getElementById('AccessPolicy' + old_MBSSID).selectedIndex > 0 && document.getElementById('AccessControlList' + old_MBSSID).value.length == 0) {
					document.getElementById('newap_text_' + old_MBSSID).focus();
					alert(_("secure empty accesspolicy"));
					return false;
				}

				if (NVRAM_SSID1 == NVRAM_SSID1INIC)
					document.getElementById("passphraseinic").value = document.getElementById("passphrase").value;

				ajaxShowTimer(document.security_form, 'timerReloader', _('message apply'), (NVRAM_AutoChannelSelect > 0 || NVRAM_AutoChannelSelectINIC > 0) ? 40 : 30);
				securityChanged = false;
				return true;
			}

			// Check WEP keys
			function checkWEP() {
				// validate HEX string
				function validateHex(str) {
					var re = /^[0-9a-fA-F]+$/;
					if (!re.test(str))
						return false;
					else
						return true;
				}

				var keyValue;
				var keyLength;

				switch (document.getElementById('wep_default_key').value) {
					case '1':	keyValue = document.getElementById('wep_key_1').value;		break;
					case '2':	keyValue = document.getElementById('wep_key_2').value;		break;
					case '3':	keyValue = document.getElementById('wep_key_3').value;		break;
					case '4':	keyValue = document.getElementById('wep_key_4').value;		break;
				}
				
				if (keyValue.length == 0) {
					document.getElementById('wep_key_' + document.getElementById('wep_default_key').value).focus();
					document.getElementById('wep_key_' + document.getElementById('wep_default_key').value).select();
					alert(_("secure no wep key") + document.getElementById('wep_default_key').value + ' !');
					return false;
				}

				for (var i = 1; i <= 4; i++) {
					// validate WEP Key
					keyLength = document.getElementById('wep_key_' + i).value.length;
					if (keyLength > 0) {
						// validate ASCII value
						if (document.getElementById('WEP' + i + 'Select').options.selectedIndex == 0) {
							if (keyLength != 5 && keyLength != 13) {
								document.getElementById('wep_key_' + i).focus();
								document.getElementById('wep_key_' + i).select();
								alert(_("secure short wep key"));
								return false;
							}
							if(!checkInjection(document.getElementById('wep_key_' + i).value)) {
								document.getElementById('wep_key_' + i).focus();
								document.getElementById('wep_key_' + i).select();
								alert(_("secure invalid wep key"));
								return false;
							}
						}
						// validate HEX value
						if (document.getElementById('WEP' + i + 'Select').options.selectedIndex == 1) {
							if (keyLength != 10 && keyLength != 26) {
								document.getElementById('wep_key_' + i).focus();
								document.getElementById('wep_key_' + i).select();
								alert(_("secure long wep key"));
								return false;
							}
							if (!validateHex(document.getElementById('wep_key_' + i).value)){
								document.getElementById('wep_key_' + i).focus();
								document.getElementById('wep_key_' + i).select();
								alert(_("secure invalid key"));
								return false;
							}
						}
					}
				}
				return true;
			}

			// Check WPA variables
			function checkWPA() {
				// validate Key renewal interval
				if (!validateNum(document.getElementById('keyRenewalInterval').value)) {
					document.getElementById('keyRenewalInterval').focus();
					document.getElementById('keyRenewalInterval').select();
					alert(_("secure renewal"));
					return false;
				}
				else if (document.getElementById('keyRenewalInterval').value < 10 ||
						 document.getElementById('keyRenewalInterval').value > 86400) {
					document.getElementById('keyRenewalInterval').focus();
					document.getElementById('keyRenewalInterval').select();
					alert(_("secure renewal wrong"));
					return false;
				}
				return true;
			}

			// Check Radius variables
			function checkRadius() {
				// validate RADIUS IP address
				if(!validateIP(document.getElementById('RadiusServerIP'))) {
					document.getElementById('RadiusServerIP').focus();
					document.getElementById('RadiusServerIP').select();
					alert(_("secure invalid radius ip"));
					return false;
				}
				// validate RADIUS port
				if(!validatePort(document.getElementById('RadiusServerPort'))) {
					document.getElementById('RadiusServerPort').focus();
					document.getElementById('RadiusServerPort').select();
					alert(_("secure invalid radius port"));
					return false;
				}
				// validate RADIUS secret
				if( !checkStrictInjection(document.getElementById('RadiusServerSecret').value) || 
					!document.getElementById('RadiusServerSecret').value.length) {
					document.getElementById('RadiusServerSecret').focus();
					document.getElementById('RadiusServerSecret').select();
					alert(_("secure invalid radius secret"));
					return false;
				}
				// validate RADIUS timeout
				if (document.getElementById('RadiusServerSessionTimeout').value.length > 0)
					if(!validateNum(document.getElementById('RadiusServerSessionTimeout').value)){
						document.getElementById('RadiusServerSessionTimeout').focus();
						document.getElementById('RadiusServerSessionTimeout').select();
						alert(_("secure invalid timeout"));
						return false;
					}
				return true;
			}

			// Check ASCII 10 & 13 on string
			function checkInjection(str) {
				var re = /[\r\n]/;
				if (re.test(str))
					return false;
				else
					return true;
			}

			// Check ASCII 10, 13 and ; , on string
			function checkStrictInjection(str) {
				var re = /^[\r\n;,]+$/;
				if (re.test(str))
					return false;
				else
					return true;
			}

			// mage AJAX request 
			function makeRequest(url, content, handler) {
				http_request = false;
				if (window.XMLHttpRequest) {							// Mozilla, Safari,...
					http_request = new XMLHttpRequest();
					if (http_request.overrideMimeType)
						http_request.overrideMimeType('text/xml');
				}
				else if (window.ActiveXObject) {						// IE
					try {
						http_request = new ActiveXObject("Msxml2.XMLHTTP");
					}
					catch (e) {
						try {
							http_request = new ActiveXObject("Microsoft.XMLHTTP");
						}
						catch (e) {}
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

			// processing request data
			function securityHandler() {
				if (http_request.readyState == 4) 
					if (http_request.status == 200) {
						var i, j, aplist;

						network = JSON.parse(http_request.responseText);

						if (BUILD_5GHZ_SUPPORT && NVRAM_RadioOnINIC == '1' && NVRAM_SSID1INIC.length > 0 && NVRAM_SSID1 != NVRAM_SSID1INIC)
							network.data[0].SSID = network.data[0].SSID + ' / ' + NVRAM_SSID1INIC;

						for (i = 0; i < network.data.length; i++) {
							if ((network.data[i].AuthMode == "OPEN" && network.data[i].EncrypType == "NONE") || network.data[i].AuthMode.length == 0)
								network.data[i].AuthMode = "Disable";
							network.data[i].AccessPolicyOrig		= network.data[i].AccessPolicy;
							network.data[i].AccessControlListOrig	= network.data[i].AccessControlList;
							network.data[i].DefaultKeyID			= (network.data[i].DefaultKeyID.length == 0) 	? '1' 		: network.data[i].DefaultKeyID;
							network.data[i].EncrypType				= (network.data[i].EncrypType.length == 0) 		? 'NONE' 	: network.data[i].EncrypType;
							network.data[i].Key1Type				= (network.data[i].Key1Type.length == 0) 		? '0' 		: network.data[i].Key1Type;
							network.data[i].Key2Type				= (network.data[i].Key2Type.length == 0) 		? '0' 		: network.data[i].Key2Type;
							network.data[i].Key3Type				= (network.data[i].Key3Type.length == 0) 		? '0' 		: network.data[i].Key3Type;
							network.data[i].Key4Type				= (network.data[i].Key4Type.length == 0) 		? '0' 		: network.data[i].Key4Type;
							network.data[i].PreAuth					= (network.data[i].PreAuth.length == 0) 		? '0' 		: network.data[i].PreAuth;
							network.data[i].RekeyMethod				= (network.data[i].RekeyMethod.length == 0) 	? 'TIME'	: network.data[i].RekeyMethod;
						}

						document.getElementById('ssidIndex').length = 0;

						for(i = 0; i < network.data.length; i++) 
							document.getElementById('ssidIndex').options[document.getElementById('ssidIndex').options.length] = new Option(network.data[i].SSID, i, false, false);

						document.getElementById('ssidIndex').options.selectedIndex = network.default_mbssid;
						old_MBSSID = network.default_mbssid;
						document.getElementById('sp_title').innerHTML = _("secure security policy") + ' "' + network.data[network.default_mbssid].SSID + '"';

						LoadFields(network.default_mbssid);
						prepareAccessPolicy();
					}
					else
						alert('There was a problem with the request.');
			}

			// Show only needed fields
			function securityMode(check) {
				displayElement(['div_wep', 'div_wpa', 'div_wpa_algorithms', 'wpa_passphrase', 'wpa_passphrase5', 
								'wpa_key_renewal_interval', 'wpa_PMK_Cache_Period', 'wpa_preAuthentication', 'div_radius_server'], false);

				switch (document.getElementById('security_mode').value) {
					case 'WEPAUTO':			displayElement('div_wep', true);
											break;
					case 'WPAPSK':
					case 'WPA2PSK':
					case 'WPAPSKWPA2PSK':	displayElement(['div_wpa', 'div_wpa_algorithms', 'wpa_passphrase', 'wpa_key_renewal_interval'], true);
											displayElement('wpa_passphrase5', BUILD_5GHZ_SUPPORT && NVRAM_SSID1 != NVRAM_SSID1INIC && old_MBSSID == 0);
											break;
					case 'WPA2':			displayElement(['wpa_preAuthentication', 'wpa_PMK_Cache_Period'], true);
					case 'WPA':
					case 'WPA1WPA2':		displayElement(['div_wpa', 'div_wpa_algorithms', 'wpa_key_renewal_interval', 'div_radius_server', ], true);
											break;
				}

				if (check && (document.getElementById('security_mode').value != 'WPA2PSK' && document.getElementById('security_mode').value != 'WPA2'))
					setTimeout(function () { showWarningEncriptionMode() }, 100);
			}

			// Load fields data
			function LoadFields(MBSSID) {
				var i;

				// Security Mode
				document.getElementById('security_mode').options.length = 0;

				for (i = 0; i < security_modes_list.length; i++)
					document.getElementById('security_mode').options.add(new Option(security_modes_list[i][0], security_modes_list[i][1]));

				for (i = 0; i < document.getElementById('security_mode').options.length; i++)
					if (document.getElementById('security_mode').options[i].value == network.data[MBSSID].AuthMode) {
						document.getElementById('security_mode').options[i].selected = true;
						break;
					}

				// WEP
				document.getElementById('wep_key_1').value = network.data[MBSSID].Key1Str;
				document.getElementById('wep_key_2').value = network.data[MBSSID].Key2Str;
				document.getElementById('wep_key_3').value = network.data[MBSSID].Key3Str;
				document.getElementById('wep_key_4').value = network.data[MBSSID].Key4Str;

				document.getElementById('WEP1Select').selectedIndex = (network.data[MBSSID].Key1Type == '0' ? 1 : 0);
				document.getElementById('WEP2Select').selectedIndex = (network.data[MBSSID].Key2Type == '0' ? 1 : 0);
				document.getElementById('WEP3Select').selectedIndex = (network.data[MBSSID].Key3Type == '0' ? 1 : 0);
				document.getElementById('WEP4Select').selectedIndex = (network.data[MBSSID].Key4Type == '0' ? 1 : 0);

				document.getElementById('wep_default_key').selectedIndex = parseInt(network.data[MBSSID].DefaultKeyID) - 1 ;

				// WPA
				if(network.data[MBSSID].EncrypType == 'TKIP')
					document.getElementById('cipher')[0].selected = true;
				else if(network.data[MBSSID].EncrypType == 'TKIPAES')
					document.getElementById('cipher')[2].selected = true;
				else
					document.getElementById('cipher')[1].selected = true;

				document.getElementById('passphrase').value			= network.data[MBSSID].WPAPSK;
				document.getElementById('passphraseinic').value		= NVRAM_WPAPSK1INIC;
				document.getElementById('keyRenewalInterval').value	= network.data[MBSSID].RekeyInterval;
				document.getElementById('PMKCachePeriod').value		= network.data[MBSSID].PMKCachePeriod;
				if (network.data[MBSSID].PreAuth == '0')
					document.getElementById('PreAuthentication')[0].selected = true;
				else
					document.getElementById('PreAuthentication')[1].selected = true;

				document.getElementById("RadiusServerIP").value				= network.data[MBSSID].RADIUS_Server;
				document.getElementById("RadiusServerPort").value			= (network.data[MBSSID].RADIUS_Port.length == 0) ? 1812 : network.data[MBSSID].RADIUS_Port;
				document.getElementById("RadiusServerSecret").value			= (network.data[MBSSID].RADIUS_Key.length == 0) ? 'wive-ng-mt' : network.data[MBSSID].RADIUS_Key;
				document.getElementById("RadiusServerSessionTimeout").value	= (network.data[MBSSID].session_timeout_interval.length == 0) ? 0 : network.data[MBSSID].session_timeout_interval;

				securityChanged = false;
				securityMode(true);
				if (document.getElementById('cipher').value != 1)
					setTimeout(function () { showWarningEncriptionAlgo() }, 100);
			}

			// change MBSSID
			function MBSSIDChange() {
				if (securityChanged) 
					if (!confirm(_("secure confirm"))) {
						document.getElementById('ssidIndex').options.selectedIndex = old_MBSSID;
						return false;
					}
					else {
						network.data[old_MBSSID].AccessPolicy = network.data[old_MBSSID].AccessPolicyOrig;
						network.data[old_MBSSID].AccessControlList = network.data[old_MBSSID].AccessControlListOrig;
						securityChanged = false;
					}

				old_MBSSID	= document.getElementById('ssidIndex').options.selectedIndex;
				LoadFields(document.getElementById('ssidIndex').options.selectedIndex);
				showAccessPolicy(document.getElementById('ssidIndex').options.selectedIndex);		// update Access Policy

				WPAAlgorithms		= network.data[old_MBSSID].EncrypType;
				PreAuthentication	= (network.data[old_MBSSID].PreAuth == '1') ? true : false;

				document.getElementById('sp_title').innerHTML = _("secure security policy") + ' "' + network.data[old_MBSSID].SSID + '"';

				createAccessPolicyTable();
				prepareAccessPolicy();
				
				displayElement(['newap_text_' + old_MBSSID + '_tr', 'newap_macs_' + old_MBSSID + '_tr'], document.getElementById('AccessPolicy' + old_MBSSID).value != 0);
				
				displayElement('wpa_passphrase5', old_MBSSID == 0 && BUILD_5GHZ_SUPPORT && NVRAM_SSID1 != NVRAM_SSID1INIC);
				if (old_MBSSID == 0 && BUILD_5GHZ_SUPPORT && NVRAM_SSID1 != NVRAM_SSID1INIC)
					_TR("secureWPAPassPhrase", "secure wpa pass phrase nic");
				else
					_TR("secureWPAPassPhrase", "secure wpa pass phrase");
			}

			// WPA algorithm change
			function onWPAAlgorithmsClick() {
				if (document.getElementById('cipher').value != 1)
					setTimeout(function () { showWarningEncriptionAlgo(); }, 100);

				if (document.getElementById('cipher').value == 0 && WPAAlgorithms == "TKIP")	return;
				if (document.getElementById('cipher').value == 1 && WPAAlgorithms == "AES")		return;
				if (document.getElementById('cipher').value == 2 && WPAAlgorithms == "TKIPAES")	return;
				securityChanged = true;
			}

			// Pre-Authentication change
			function onPreAuthenticationClick() {
				if(document.getElementById('PreAuthentication').value == 0 && PreAuthentication == false) return;
				if(document.getElementById('PreAuthentication').value == 1 && PreAuthentication == true)  return;
				securityChanged = true;
			}

			// AccessPolicy change
			function accessPolicyClick(table) {
				network.data[table].AccessPolicy = document.getElementById('AccessPolicy' + table).value;
				displayElement(['newap_text_' + table + '_tr', 'newap_macs_' + table + '_tr'], document.getElementById('AccessPolicy' + table).value != 0);
			}

			// Show/Hide passphrase
			function showPassPhrase(id) {
				document.getElementById(id).type = (document.getElementById(id).type == 'password') ? 'text' : 'password';
			}

			// Create access policy table
			function createAccessPolicyTable() {
				var html = '';
				for (var aptable = 0; aptable < MBSSID_MAX; aptable++) {
					html += '<table id="AccessPolicy_' + aptable + '_table" border="1" class="form">' +
							'	<tbody>' + 
							'		<tr>' +
							'			<td class="title" colspan="2">' + _('secure access policy') + '</td>' +
							'		</tr>' +
							'		<tr>' +
							'			<td bgcolor="#E8F8FF" class="head" >' + _('secure access policy capable') + '</td>' +
							'			<td>' +
							'				<select name="AccessPolicy' + aptable + '" id="AccessPolicy' + aptable + '" class="mid" onChange="securityChanged = true; accessPolicyClick(' + aptable + ');">' +
							'					<option value="0">' + _('wireless disable') + '</option>' +
							'					<option value="1">' + _('wireless allow') + '</option>' +
							'					<option value="2">' + _('wireless reject') + '</option>' +
							'				</select>' +
							'			</td>' +
							'		</tr>' +
							'		<tr  id="newap_text_' + aptable + '_tr">' +
							'			<td class="head">' + _('secure access policy new') + '</td>' +
							'			<td>' +
							'				<input class="mid" id="newap_text_' + aptable + '" maxlength="17">' +
							'				<input class="half" id="newap_text_' + aptable + '_add" type="button" value="' + _('station add') + '" onClick="addMAC(' + aptable +')">' +
							'			</td>' +
							'		</tr>' +
							'		<tr id="ap_' + aptable + '_title"><td class="title" colspan="2">' + _('secure access policy mac list') + '</td></tr>' +

							'		<tr id="newap_macs_' + aptable + '_tr">' +
							'			<td colspan="2" id="ap_' + aptable + '_td">' +
							'				<table>';

					for (var i = 0; i < ACCESSPOLICYLIST_MAX / 4; i++) {
						input_name	=   'newap_' + aptable + '_' + (4 * i);
						td_name		=   'newap_td_' + aptable + '_' + (4 * i);
						html 		+=	'				<tr id="id_' + aptable + '_' + (4 * i) + '">' +
										'					<td id="' + td_name + '" style="width: 173px;">' +
										'						<div style="width: 25%; float: left; margin-top: 3px;"><img src="/graphics/cross.png" alt="[-]" title="' + _('station del') + '" onClick="delMAC(' + aptable + ', ' + (4 * i) + ')"></div>' +
										'						<div id="' + input_name + '" style="width: 75%; float: left; margin-top: 4px;"></div>' +
										'					</td>';

						input_name	=	'newap_' + aptable + '_' + (4 * i + 1);
						td_name		=	'newap_td_' + aptable + '_' + (4 * i + 1);
						html 		+=	'					<td id="' + td_name + '" style="width: 173px;">' +
										'						<div style="width: 25%; float: left; margin-top: 3px;"><img src="/graphics/cross.png" alt="[-]" title="' + _('station del') + '" onClick="delMAC(' + aptable + ', ' + (4 * i + 1) + ')"></div>' +
										'						<div id="' + input_name + '" style="width: 75%; float: left; margin-top: 4px;"></div>' +
										'					</td>';

						input_name	=	'newap_' + aptable + '_' + (4 * i + 2);
						td_name		=	'newap_td_' + aptable + '_' + (4 * i + 2);
						html		+=	'					<td id="' + td_name + '" style="width: 173px;">' +
										'						<div style="width: 25%; float: left; margin-top: 3px;"><img src="/graphics/cross.png" alt="[-]" title="' + _('station del') + '" onClick="delMAC(' + aptable + ', ' + (4 * i + 2) + ')"></div>' +
										'						<div id="' + input_name + '" style="width: 75%; float: left; margin-top: 4px;"></div>' +
										'					</td>';

						input_name	=	'newap_' + aptable + '_' + (4 * i + 3);
						td_name		=	'newap_td_' + aptable + '_' + (4 * i + 3);
						html 		+=	'					<td id="' + td_name + '" style="width: 173px;">' +
										'						<div style="width: 25%; float: left; margin-top: 3px;"><img src="/graphics/cross.png" alt="[-]" title="' + _('station del') + '" onClick="delMAC(' + aptable + ', ' + (4 * i + 3) + ')"></div>' +
										'						<div id="' + input_name + '" style="width: 75%; float: left; margin-top: 4px;"></div>' +
										'					</td>' +
										'				</tr>';
					}

					html +=	'				</table>' +
							'			</td>' +
							'		</tr>' +
							'	</tbody>' +
							'</table>';
				}
				document.getElementById('accessPolicyDiv').innerHTML = html;
			}

			// Show access policy
			function showAccessPolicy(MBSSID) {
				for (var i = 0; i < MBSSID_MAX; i++) {
					if (i < network.data.length)
						document.getElementById('AccessPolicy' + i).selectedIndex = network.data[i].AccessPolicy;
					else
						document.getElementById('AccessPolicy' + i).selectedIndex = 0;
					displayElement('AccessPolicy_' + i + '_table', false);
				}
				displayElement('AccessPolicy_' + MBSSID + '_table', true);
			}

			// prepare access policy list
			function prepareAccessPolicy() {
				// load Access Policy for MBSSID[selected]
				for (i = 0; i < network.data.length; i++) {
					j		= 0;
					aplist	= [];
					if (network.data[i].AccessControlList.length != 0) {
						aplist = network.data[i].AccessControlList.split(";");
						for (j = 0; j < aplist.length; j++) 
							document.getElementById("newap_" + i + "_" + j).innerHTML = aplist[j];
					}
					else
						displayElement(['ap_' + i + '_td', 'ap_' + i + '_title', 'newap_text_' + i + '_tr'], false);

					for(; j < ACCESSPOLICYLIST_MAX; j++) {
						displayElement('newap_td_' + i + '_' + j, false);
						if (!(j % 4))
							displayElement('id_' + i + '_' + j, false);
					}
					displayElement(['newap_text_' + i + '_tr', 'newap_macs_' + i + '_tr'], network.data[i].AccessPolicy != 0);
				}
				showAccessPolicy(document.getElementById('ssidIndex').value);
			}

			// Add access policy MAC
			function addMAC(table) {
				var mac = document.getElementById('newap_text_' + table).value.toUpperCase();
				if (!validateMAC(mac)) {
					alert(_("inet invalid mac"));
					document.getElementById('newap_text_' + table).focus();
					document.getElementById('newap_text_' + table).select();
					return false;
				}
				if (network.data[table].AccessControlList.toUpperCase().indexOf(mac.toUpperCase()) >= 0) {
					alert(_("secure mac exist"));
					document.getElementById('newap_text_' + table).focus();
					document.getElementById('newap_text_' + table).select();
					return false;
				}

				createAccessPolicyTable();
				
				if (network.data[table].AccessControlList.length > 0)
					network.data[table].AccessControlList += ';';
				network.data[table].AccessControlList += mac;

				prepareAccessPolicy();
			}

			// Delete access policy MAC
			function delMAC(mbssid, num) {
				var new_aplist = '';
				var aplist = network.data[mbssid].AccessControlList.split(";");
				aplist.splice(num, 1);
				for (var i = 0; i < aplist.length; i++) {
					new_aplist += aplist[i];
					if (i + 1 < aplist.length)
						new_aplist += ';';
				}
				network.data[mbssid].AccessControlList = new_aplist;
				createAccessPolicyTable();
				prepareAccessPolicy();
			}

			// Show warning message
			function showWarning() {
				var warning_access_password		= NVRAM_Password == "Admin";
				var warning_wireless_security	= NVRAM_AuthMode == "OPEN";
				var warning_wireless_key		= NVRAM_WPAPSK1  == "1234567890";

				var warningHTML = "";

				if (warning_access_password || warning_wireless_security || warning_wireless_key) {
					warningHTML += '<br>';
					warningHTML += '<table class="warning">';
					warningHTML += '<tr><th class="warning" align="center" colspan="2">' + _("warning header") + '</th></tr>';
					if  (warning_wireless_security || warning_wireless_key) {
						warningHTML += '<tr>';
						warningHTML += '<td class="warning" colspan="2">' + _("warning wireless security") + '</td>';
						warningHTML += '</tr>';
					}
					if (warning_access_password && (warning_wireless_security || warning_wireless_key)) {
						warningHTML += '<tr><td colspan="2"><hr class="warning"></td></tr>';
					}
					if  (warning_access_password) {
						warningHTML += '<tr>';
						warningHTML += '<td class="warning">' + _("warning access password") + '</td>';
						warningHTML += '<td align="right" class="warning"><input align="right" type="button" style="{width:120px;}" value="' + _("button warning") + '" onClick=\'window.location.assign("/adm/management.asp");\'></td>';
						warningHTML += '</tr>';
					}
					warningHTML += '</table>';
					warningHTML += '<br>';
					ajaxModifyElementHTML('warning', warningHTML);
				}
			}

			function showWarningEncriptionMode() {
				var setPass = false;
				var br = getBrowser();
				var message = (br.browser == 'firefox') ? _('secure lowsecure mode').replace(/_/g, '\r\n') : _('secure lowsecure mode').replace(/_/g, '\r');
				if (confirm(message)) {
					if (network.data[old_MBSSID].AuthMode != 'WPAPSK' && network.data[old_MBSSID].AuthMode != 'WPAPSKWPA2PSK') {
						setPass = true;
						setTimeout(function () { alert(_('secure lowsecure mode apply')) }, 100);
					}
					network.data[old_MBSSID].AuthMode	= 'WPA2PSK';
					network.data[old_MBSSID].EncrypType = 'AES';
					LoadFields(old_MBSSID);
					if (setPass) {
						document.getElementById('passphrase').focus();
						document.getElementById('passphrase').select();
					}
					else
						checkValues();
				}
			}

			function showWarningEncriptionAlgo() {
				var br = getBrowser();
				var message = (br.browser == 'firefox') ? _('secure lowsecure algo').replace(/_/g, '\r\n') : _('secure lowsecure algo').replace(/_/g, '\r');
				if (confirm(message)) {
					document.getElementById('cipher').value = 1;
					checkValues();
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tbody>
				<tr>
					<td><h1 id="securityTitle">Wireless Security/Encryption Settings </h1>
					<p id="securityIntroduction">Here you can configure wireless security and encryption to prevent unauthorized access to the router.</p>
					<hr />
					<form method="post" name="security_form" action="/goform/wirelessSetSecurity" onSubmit="return checkValues();">
					<iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<!-- ---------------------  MBSSID Selection  --------------------- -->
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="secureSelectSSID">Select SSID</td>
						</tr>
						<tr>
							<td class="head" id="secureSSIDChoice">SSID choice</td>
							<td><select name="ssidIndex" id="ssidIndex" class="mid" onChange="MBSSIDChange();"></select></td>
						</tr>
					</table>
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="sp_title">Security Policy</td>
						</tr>
						<tr id="div_security_infra_mode" name="div_security_infra_mode">
							<td class="head" id="secureSecureMode">Security Mode</td>
							<td>
								<select name="security_mode" id="security_mode" class="mid" onChange="securityChanged = true; securityMode(true);">
								</select>
							</td>
						</tr>
					</table>
					<!-- WEP -->
					<table id="div_wep" name="div_wep" class="form" style="visibility: hidden;">
						<tr>
							<td class="title" colspan="4" id="secureWEP">Wired Equivalent Privacy (WEP)</td>
						</tr>
						<tr>
							<td class="head" colspan="2" id="secureWEPDefaultKey">Default Key</td>
							<td colspan="2">
								<select name="wep_default_key" id="wep_default_key" class="half" onChange="securityChanged = true;">
									<option value="1" id="secureWEPDefaultKey1">Key 1</option>
									<option value="2" id="secureWEPDefaultKey2">Key 2</option>
									<option value="3" id="secureWEPDefaultKey3">Key 3</option>
									<option value="4" id="secureWEPDefaultKey4">Key 4</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head1" rowspan="4" id="secureWEPKey">WEP Keys</td>
							<td class="head2" id="secureWEPKey1">WEP Key 1 :</td>
							<td style="width: 300px">
								<input type="password" name="wep_key_1" id="wep_key_1" maxlength="26" value="" onKeyUp="securityChanged = true;">
								<span id="secureWEPKey1_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_1')"><span id="secureShowPass" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP1Select" class="half" name="WEP1Select" onChange="securityChanged = true;">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head2" id="secureWEPKey2">WEP Key 2 : </td>
							<td>
								<input type="password" name="wep_key_2" id="wep_key_2" maxlength="26" value="" onKeyUp="securityChanged = true;">
								<span id="secureWEPKey2_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_2')"><span id="secureShowPass1" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP2Select" name="WEP2Select" class="half" onChange="securityChanged = true;">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head2" id="secureWEPKey3">WEP Key 3 : </td>
							<td>
								<input type="password" name="wep_key_3" id="wep_key_3" maxlength="26" value="" onKeyUp="securityChanged = true;">
								<span id="secureWEPKey3_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_3')"><span id="secureShowPass2" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP3Select" name="WEP3Select" class="half" onChange="securityChanged = true;">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head2" id="secureWEPKey4">WEP Key 4 : </td>
							<td>
								<input type="password" name="wep_key_4" id="wep_key_4" maxlength="26" value="" onKeyUp="securityChanged = true;">
								<span id="secureWEPKey4_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_4')"><span id="secureShowPass3" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP4Select" name="WEP4Select" class="half" onChange="securityChanged = true;">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
					</table>
					<!-- WPA -->
					<table id="div_wpa" name="div_wpa" class="form" style="visibility: hidden; display: none;">
						<tr>
							<td class="title" colspan="2" id="secureWPA">WPA</td>
						</tr>
						<tr id="div_wpa_algorithms" name="div_wpa_algorithms" style="visibility: hidden;">
							<td class="head" id="secureWPAAlgorithm">WPA Algorithms</td>
							<td>
								<select name="cipher" id="cipher" class="mid" onChange="onWPAAlgorithmsClick()">
									<option value="0">TKIP</option>
									<option value="1">AES</option>
									<option value="2">TKIP/AES</option>
								</select>
							</td>
						</tr>
						<tr id="wpa_passphrase" name="wpa_passphrase" style="visibility: hidden;">
							<td class="head" id="secureWPAPassPhrase">Pass Phrase</td>
							<td>
								<input type="password" name="passphrase" id="passphrase" class="mid" maxlength="64" value="" onKeyUp="securityChanged = true;">
								<span id="passphrase_type" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('passphrase')"><span id="secureShowPass4" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
						</tr>
						<tr id="wpa_passphrase5" name="wpa_passphrase5" style="display: none;">
							<td class="head" id="secureWPAPassPhraseInic">Pass Phrase 5GHz</td>
							<td>
								<input type="password" name="passphraseinic" id="passphraseinic" class="mid" maxlength="64" value="" onKeyUp="securityChanged = true;">
								<span id="passphrase_inic_type" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('passphraseinic')"><span id="secureShowPass5" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
						</tr>
						<tr id="wpa_key_renewal_interval" name="wpa_key_renewal_interval" style="visibility: hidden;">
							<td class="head" id="secureWPAKeyRenewInterval">Key Renewal Interval</td>
							<td><input name="keyRenewalInterval" id="keyRenewalInterval" class="mid" maxlength="6" value="3600" onKeyUp="securityChanged = true;">&nbsp;<span id="secureKeySeconds">seconds</span></td>
						</tr>
						<tr id="wpa_PMK_Cache_Period" name="wpa_PMK_Cache_Period" style="visibility: hidden;">
							<td class="head" id="secureWPAPMKCachePeriod">PMK Cache Period</td>
							<td><input name="PMKCachePeriod" id="PMKCachePeriod" class="mid" maxlength="4" value="" onKeyUp="securityChanged = true;">&nbsp;<span id="secureKeyMinutes">minute</span></td>
						</tr>
						<tr id="wpa_preAuthentication" name="wpa_preAuthentication" style="visibility: hidden;">
							<td class="head" id="secureWPAPreAuth">Pre-Authentication</td>
							<td>
								<select name="PreAuthentication" id="PreAuthentication" class="mid" onChange="onPreAuthenticationClick()">
									<option value="0" id="PreAuthenticationDisable">Disable</option>
									<option value="1" id="PreAuthenticationEnable">Enable</option>
								</select>
							</td>
						</tr>
					</table>
					<table id="div_radius_server" name="div_radius_server" class="form" style="visibility: hidden; display: none;">
						<tr>
							<td class="title" colspan="2" id="secureRadius">Radius Server</td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusIPAddr">IP Address</td>
							<td><input name="RadiusServerIP" id="RadiusServerIP" class="mid" maxlength="32" value="" onKeyUp="securityChanged = true;"></td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusPort">Port</td>
							<td><input name="RadiusServerPort" id="RadiusServerPort" class="mid" maxlength="5" value="" onKeyUp="securityChanged = true;"></td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusSharedSecret">Shared Secret</td>
							<td>
								<input type="password" name="RadiusServerSecret" id="RadiusServerSecret" class="mid" maxlength="64" value="" onKeyUp="securityChanged = true;">
								<span id="secureRadiusSharedSecret_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onChange="showPassPhrase('RadiusServerSecret')"><span id="secureShowPass6" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusSessionTimeout">Session Timeout</td>
							<td><input name="RadiusServerSessionTimeout" id="RadiusServerSessionTimeout" class="mid" maxlength="4" value="0" onKeyUp="securityChanged = true;"></td>
						</tr>
					</table>
					<!--	AccessPolicy for mbssid -->
					<div id="accessPolicyDiv"></div>
					<div id="accessPolicyInput"></div>
					<table class="buttons">
						<tr>
							<td>
								<input class="normal" value="Apply" id="secureApply" type="submit"><input class="normal" value="Cancel" id="secureCancel" type="button" onClick="window.location.reload();"><input class="normal" value="Reset" id="secureReset" type="button" onClick="resetValues(this.form, 25);">
								<input type="hidden" name="reset" value="0">
							</td>
						</tr>
					</table>
				</form>
				<div class="whitespace"></div></td>
			</tr>
		</tbody>
	</table>
</body>
</html>
