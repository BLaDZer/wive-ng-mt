<!DOCTYPE html>
<html>
	<head>
		<title>Virtual Local Area Network (VLAN) Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("internet");
			Butterlate.setTextDomain("buttons");

			var wifiNIC			= [];
			var wifiSSID		= [];
			var wifiLan 		= [];
			var wifiLanINIC 	= [];
			var wifiWan			= [];
			var wifiWanINIC 	= [];
			var mbssid_mode 	= "";
			var ssid_num		= "";
			var vlanLan			= [];
			var vlanLanIsolate	= [];

			var is5gh_support	= '<% is5gh_support(); %>';

			function initTranslation()
			{
				_TR("vlanPortTitle",				"vlan port title");
				_TR("vlanPortIntroduction",			"vlan port introduction");
				_TR("vlanPortStatus",				"vlan port status");
				_TR("vlanPortNumber",				"vlan port status number");
				_TR("vlanPortType",					"vlan port status type");
				_TR("vlanPortVLAN",					"vlan port status vlan");
				_TR("vlanPortMode",					"vlan port status mode");
				_TR("vlanPortState",				"vlan port status state");
				_TR("vlanPortManagement", 			"vlan port management");
				_TR("vlanPort_wan_port",			"vlan port management wan port");
				_TR("vlanPort_lan_port",			"vlan port management lan port");
				_TR("vlanPort_lan_port_near",		"vlan port management lan port near");
				_TR("vlanPort_lan_port_distant",	"vlan port management lan port distant");
				_TR("tv_stb",						"vlan port tv stb");
				_TR("tv_stb_enable",				"vlan port tv stb enable");
				_TR("tv_stb_mcast",					"vlan port tv stb mcast");
				_TR("tv_stb_vlanid",				"vlan port tv stb vlanid");
				_TR("sip_stb",						"vlan port sip stb");
				_TR("sip_stb_enable",				"vlan port sip stb enable");
				_TR("sip_stb_mcast",				"vlan port sip stb mcast");
				_TR("sip_stb_vlanid",				"vlan port sip stb vlanid");
				_TR("vlanPortMode",					"vlan port mode");
				_TR("port_mode_auto",				"vlan port speed auto");
				_TR("port_mode_10h",				"vlan port speed 10h");
				_TR("port_mode_10f",				"vlan port speed 10f");
				_TR("port_mode_100h",				"vlan port speed 100h");
				_TR("port_mode_100f",				"vlan port speed 100f");
				_TR("port_mode_1000f",				"vlan port speed 1000f");
				_TR("vlanWlanLanTitle",				"vlan mode title");
				_TR("vlanWlanLanIntro",				"vlan mode introduction");
				_TR("vlanModeSettings",				"vlan mode settings");
				_TR("vlanMode",						"vlan mode select");
				_TR("vlanMode_select_disable",		"vlan mode select disable");
				_TR("vlanMode_select_wlan",			"vlan mode select wlan");
				_TR("vlanMode_select_lan",			"vlan mode select lan");
				_TR("vlanMode_wlan_ap",				"vlan mode wlan ap");
				_TR("vlanMode_wlan_iface",			"vlan mode wlan iface");
				_TR("vlanMode_wlan_vlan",			"vlan mode wlan vlan");
				_TRV("vlanMode_wlan_apply",			"vlan mode wlan apply");
				_TR("vlanMode_lan_num",				"vlan mode lan num");
				_TR("vlanMode_lan_vlanid",			"vlan mode lan vlanid");
				_TR("vlanMode_lan_isolated",		"vlan mode lan isolated");
				_TRV("vlanMode_lan_add",			"vlan mode lan add");
				_TRV("vlanMode_lan_apply",			"vlan mode lan apply");
				_TRV("port_mode_setup",				"vlan port setup button");
				_TR("vlanMode_lan_table",			"vlan mode lan table");
				_TR("vlanMode_lan_table_num",		"vlan mode lan table num");
				_TR("vlanMode_lan_table_vlan",		"vlan mode lan table vlan");
				_TR("vlanMode_lan_table_isolated",	"vlan mode lan table isolated");
				_TR("vlanMode_lan_table_action",	"vlan mode lan table action");
				_TR("vlanMode_wlan_table",			"vlan mode wlan table");
				_TR("vlanMode_wlan_table_ap",		"vlan mode wlan table ap");
				_TR("vlanMode_wlan_table_vlan",		"vlan mode wlan table vlan");
				_TR("vlanMode_wlan_table_iface",	"vlan mode wlan table iface");
				_TR("vlanMode_wlan_table_action",	"vlan mode wlan table action");
				_TRV("vlanApply",					"button apply");
				_TRV("vlanCancel",					"button cancel");
				_TRV("vlanReset",					"button reset");
				_TRV("vlanModeApply",				"button apply");
				_TRV("vlanModeCancel",				"button cancel");
				_TRV("vlanModeReset",				"button reset");
			}

			function initValues()
			{
				var gigaphy					= '<% gigaphy(); %>';
				var form					= document.lanCfg;

				// Initializing variables
				form.wan_port.selectedIndex	= ('<% getCfgGeneral(1, "wan_port"); %>' == '4') ? 1 : 0;
				form.lan_port.selectedIndex	= ('<% getCfgGeneral(1, "lan_port"); %>' == 'near') ? 0 : 1;
				form.port1_swmode.value		= '<% getCfgGeneral(1, "port1_swmode"); %>';
				form.port2_swmode.value		= '<% getCfgGeneral(1, "port2_swmode"); %>';
				form.port3_swmode.value		= '<% getCfgGeneral(1, "port3_swmode"); %>';
				form.port4_swmode.value		= '<% getCfgGeneral(1, "port4_swmode"); %>';
				form.port5_swmode.value		= '<% getCfgGeneral(1, "port5_swmode"); %>';
				form.tv_stbEnabled.checked	= ('<% getCfgGeneral(1, "tv_port"); %>' == '1') ? true : false;
				form.tv_stbMcast.checked	= ('<% getCfgGeneral(1, "tv_port_mcast"); %>' == '1') ? true : false;
				form.tv_stbVLAN.value		= '<% getCfgGeneral(1, "tv_portVLAN"); %>';
				form.sip_stbEnabled.checked	= ('<% getCfgGeneral(1, "sip_port"); %>' == '1') ? true : false;
				form.sip_stbMcast.checked	= ('<% getCfgGeneral(1, "sip_port_mcast"); %>' == '1') ? true : false;
				form.sip_stbVLAN.value		= '<% getCfgGeneral(1, "sip_portVLAN"); %>';

				if (gigaphy == 0) {
					form.port_mode.remove(5);
				}

				switch (form.port1_swmode.value) {
					case 'auto':	form.port_mode.selectedIndex = 0;	break;
					case '10h':		form.port_mode.selectedIndex = 1;	break;
					case '10f':		form.port_mode.selectedIndex = 2;	break;
					case '100h':	form.port_mode.selectedIndex = 3;	break;
					case '100f':	form.port_mode.selectedIndex = 4;	break;
					case '1000f':	form.port_mode.selectedIndex = 5;	break;
				}

				showWarning();
				vlanModeChange();

				ajaxLoadElement("vlanData", "/internet/vlan-portstatus.asp", initVariables);
				setInterval('ajaxLoadElement("vlanData", "/internet/vlan-portstatus.asp", showPortStatus);', 2000);

				initTranslation();
			}

			function checkValues(form) 
			{
				// Checking TV/STB VLANs ID
				if (form.tv_stbEnabled.checked)
					if (!form.tv_stbVLAN.value.match(/[^0-9,\s]/gi)) {
						var tmp = form.tv_stbVLAN.value.replace(/\s+/g, '').split(",");
						for (var i = 0; i < tmp.length; i++)
							if ((+tmp[i] <= 0) || (+tmp[i] > 4096)) {
								alert(_("vlan invalid vlan range"));
								form.tv_stbVLAN.select();
								form.tv_stbVLAN.focus();
								return false
							}
					}
					else {
						alert(_("vlan invalid vlanid"));
						form.tv_stbVLAN.select();
						form.tv_stbVLAN.focus();
						return false
					}

				// Checking SIP/STB VLANs ID
				if (form.sip_stbEnabled.checked)
					if (!form.sip_stbVLAN.value.match(/[^0-9,\s]/gi)) {
						var tmp = form.sip_stbVLAN.value.replace(/\s+/g, '').split(",");
						for (var i = 0; i < tmp.length; i++)
							if ((tmp[i] <= 0) || (tmp[i] > 4096)) {
								alert(_("vlan invalid vlan range"));
								form.sip_stbVLAN.select();
								form.sip_stbVLAN.focus();
								return false
							}
					}
					else {
						alert(_("vlan invalid vlanid"));
						form.sip_stbVLAN.select();
						form.sip_stbVLAN.focus();
						return false
					}

				if (!ajaxPostForm(_('vlan reboot confirm'), form, 'timerReloader', _("message config"), ajaxShowProgress)) {
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
					form.reboot.value = "0";
					form.submit();
				}
				return true;
			}

			function checkVlanLanValues(form)
			{
				var wifi_lan			= "";
				var wifi_wan			= "";
				var wifi_lan_inic		= "";
				var wifi_wan_inic		= "";
				var vlan_lan			= "";
				var vlan_lan_isolate	= "";
				var ra = rai 			= 0;
				
				if (form.vlanMode_select.selectedIndex == 1) {
					for (var i = 0; i < ssid_num; i++)
						if (wifiNIC[i] == "ra")
							ra++;
						else 
							rai++;
			
					for (var i = 0; i < ra; i++) {
						wifi_lan += wifiLan[i];
						if ((i + 1) < ra)
							wifi_lan += " ";

						wifi_wan += wifiWan[i];
						if ((i + 1) < ra)
							wifi_wan += " ";
					}
					
					for (var i = 0; i < rai; i++) {
						wifi_lan_inic += wifiLanINIC[i];
						if ((i + 1) < rai)
							wifi_lan_inic += " ";

						wifi_wan_inic += wifiWanINIC[i];
						if ((i + 1) < rai)
							wifi_wan_inic += " ";
					}
				}
				if (form.vlanMode_select.selectedIndex == 2) {
					for (var i = 0; i < vlanLan.length; i++) {
						vlan_lan += vlanLan[i];
						if ((i + 1) < vlanLan.length)
							vlan_lan += " ";
					}
					for (var i = 0; i < vlanLanIsolate.length; i++) {
						vlan_lan_isolate += vlanLanIsolate[i];
						if ((i + 1) < vlanLanIsolate.length)
							vlan_lan_isolate += " ";
					}
				}

				form.wifi_lan.value			= wifi_lan;
				form.wifi_wan.value			= wifi_wan;
				form.wifi_lan_inic.value	= wifi_lan_inic;
				form.wifi_wan_inic.value	= wifi_wan_inic;
				form.vlan_lan.value			= vlan_lan;
				form.vlan_lan_isolate.value	= vlan_lan_isolate;

				if (!ajaxPostForm(_('vlan reboot confirm'), form, 'timerReloader', _("message config"), ajaxShowProgress)) {
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
					form.reboot.value = "0";
					form.submit();
				}
				return true;
			}

			function selectPort(port)
			{
				if (port === undefined)
					port = document.lanCfg.port_num.selectedIndex + 1;
				else 
					document.lanCfg.port_num.selectedIndex = port - 1;
				switch (document.getElementById('port' + port + '_swmode').value) {
					case 'auto':	document.lanCfg.port_mode.selectedIndex = 0;	break;
					case '10h':		document.lanCfg.port_mode.selectedIndex = 1;	break;
					case '10f':		document.lanCfg.port_mode.selectedIndex = 2;	break;
					case '100h':	document.lanCfg.port_mode.selectedIndex = 3;	break;
					case '100f':	document.lanCfg.port_mode.selectedIndex = 4;	break;
					case '1000f':	document.lanCfg.port_mode.selectedIndex = 5;	break;
				}
				showPortStatus();
			}

			function setPort()
			{
				var port = document.lanCfg.port_num.selectedIndex + 1;
				switch (document.lanCfg.port_mode.selectedIndex) {
					case 0:		document.getElementById('port' + port + '_swmode').value = 'auto';	break;
					case 1:		document.getElementById('port' + port + '_swmode').value = '10h';	break;
					case 2:		document.getElementById('port' + port + '_swmode').value = '10f';	break;
					case 3:		document.getElementById('port' + port + '_swmode').value = '100h';	break;
					case 4:		document.getElementById('port' + port + '_swmode').value = '100f';	break;
					case 5:		document.getElementById('port' + port + '_swmode').value = '1000f';	break;
				}					
			}

			function checkSTBform()
			{
				var stbEnable	= document.getElementById("tv_stbEnabled");
				var stbVLAN		= document.getElementById("tv_stbVLAN");
				var stbMcast	= document.getElementById("tv_stbMcast");

				if (stbEnable.checked) 
				{
					stbVLAN.disabled = false;
					if (stbVLAN.value.length > 0) {
						stbMcast.disabled = false;
					} else {
						stbMcast.disabled = true;
					}
				}
				else {
					stbVLAN.disabled = true;
					stbMcast.disabled = true;
				}
			}

			function checkSIPform()
			{
				var sipEnable	= document.getElementById("sip_stbEnabled");
				var sipVLAN		= document.getElementById("sip_stbVLAN");
				var sipMcast	= document.getElementById("sip_stbMcast");

				if (sipEnable.checked) 
				{
					sipVLAN.disabled = false;
					if (sipVLAN.value.length > 0) {
						sipMcast.disabled = false;
					} else {
						sipMcast.disabled = true;
					}
				} else {
					sipVLAN.disabled = true;
					sipMcast.disabled = true;
				}
			}

			function showPortStatus()
			{
				var portData;
				try {
					portData	= JSON.parse(document.getElementById('vlanPort_status').value);
				} catch(e) {};
				var portName	= "";
				var portType	= "";
				var portVLAN	= "";
				var portMode	= [ document.getElementById('port1_swmode').value, 
									document.getElementById('port2_swmode').value, 
									document.getElementById('port3_swmode').value,
									document.getElementById('port4_swmode').value,
									document.getElementById('port5_swmode').value
								  ];
				var portState	= "";

				var platform	= '<% getPlatform(); %>';

				var wan			= '<% getCfgZero(1, "wan_port"); %>';

				var stb_port	= ('<% getCfgZero(1, "tv_port"); %>' != '1') ? -1 : (wan == 0) ? 1 : wan - 1;
				var stb_vlan	= '<% getCfgZero(1, "tv_portVLAN"); %>';
				var sip_port	= ('<% getCfgZero(1, "sip_port"); %>' != '1') ? -1 : (wan == 0) ? 2 : wan - 2;
				var sip_vlan	= '<% getCfgZero(1, "sip_portVLAN"); %>';

				var html		= "";
				var port_img	= "";
				var port_text	= "";
				var i;

				html += '<table class="form">';
				html += '<tr>';
				html += '<td class="title" colspan="5" id="vlanPortStatus">Ethernet Port Status</td>';
				html += '</tr>';
				html += '<tr>'
				html += '<th id="vlanPortNumber" style="width: 10%"># (Name)</th>';
				html += '<th id="vlanPortType"   style="width: 20%">Type</th>';
				html += '<th id="vlanPortVLAN"   style="width: 20%">VLAN</th>';
				html += '<th id="vlanPortMode"   style="width: 20%">Mode</th>';
				html += '<th id="vlanPortState"  style="width: 20%">State</th>';
				html += '</tr>'

				for (i = (portData.ethernet.length - 1); i >= 0; i--) {

					html += '<tr>';

					// Set port number and name 
					if (portData.ethernet[i].port == 4) {
						portName = "WAN";
					}
					else
						if (platform == "MT7620 2T2R 2.4GHz, 100FDX")	// SNR-CPE-W4N-MT
							portName = (4 - portData.ethernet[i].port) + "X";
						else if (platform == "MT7620 2T2R 2.4GHz, MT7610 1T1R 5GHz, 100FDX")	// SNR-CPE-MD1
							portName = "LAN" + (4 - +portData.ethernet[i].port);
					html += '<td style="text-align: center;">' + (+portData.ethernet[i].port + 1) + ' (' + portName + ')</td>';

					// Set port type
					if (portData.ethernet[i].port == wan)
						portType = "WAN";
					else if (portData.ethernet[i].port == stb_port)
						portType = "STB";
					else if (portData.ethernet[i].port == sip_port)
						portType = "SIP";
					else
						portType = "LAN";
					html += '<td style="text-align: center;">' + portType + '</td>';

					if (document.getElementById('vlanPortStatus_type_port' + i).innerHTML != portType)
						document.getElementById('vlanPortStatus_type_port' + i).innerHTML = portType;

					// Set port vlan
					if (i == stb_port)
						portVLAN = stb_vlan;
					else if (i == sip_port)
						portVLAN = sip_vlan;
					else
						portVLAN = "-";
					html += '<td style="text-align: center;">' + portVLAN + '</td>';

					// Set port mode
					switch (portMode[i]) {
						case 'auto':		portMode[i] = _("vlan port speed auto");	break;
						case '10h':			portMode[i] = _("vlan port speed 10h");		break;
						case '10f':			portMode[i] = _("vlan port speed 10f");		break;
						case '100h':		portMode[i] = _("vlan port speed 100h");	break;
						case '100f':		portMode[i] = _("vlan port speed 100f");	break;
						case '1000h':		portMode[i] = _("vlan port speed 1000h");	break;
						case '1000f':		portMode[i] = _("vlan port speed 1000f");	break;
					}
					html += '<td style="text-align: center;">' + portMode[i] + '</td>';

					// Set port state and image
					if (portData.ethernet[i].link == "1") {
						portState = "Up ( " + portData.ethernet[i].speed + "Mbit/s ";
						if (portData.ethernet[i].duplex == "F") {
							portState += "Full-Duplex";
							port_img = '<img src="/graphics/port_' + portData.ethernet[i].speed + 'f.gif">';
						}
						else {
							portState += "Half-Duplex";
							port_img = '<img src="/graphics/port_' + portData.ethernet[i].speed + 'h.gif">';
						}
						portState += " )";
					}
					else {
						portState = "Down";
						port_img = '<img src="/graphics/port_empty.gif">';
					}
					html += '<td style="text-align: center;">' + portState + '</td>';
					
					if (document.getElementById('vlanPortStatus_img_port' + (4 - i)).innerHTML != port_img)
						document.getElementById('vlanPortStatus_img_port' + (4 - i)).innerHTML = port_img;

					html += '</tr>';
				}

				html += '</table>';

				document.getElementById('vlanPortStatus_table').innerHTML = html;
				initTranslation();
			}

			function initVariables()
			{
				getWLANVLANvalues();
				getLANVLANvalues();

				if (document.getElementById('vlanMode_WifiLan').value.length > 0 || document.getElementById('vlanMode_WifiLanINIC').value.length > 0 ||
				    document.getElementById('vlanMode_WifiWan').value.length > 0 || document.getElementById('vlanMode_WifiWanINIC').value.length > 0) 
					document.getElementById('vlanMode_select').selectedIndex = 1;
				if (document.getElementById('vlanMode_VLANlan').value.length > 0 ||
				    document.getElementById('vlanMode_VLANlanIsolate').value.length > 0)
					document.getElementById('vlanMode_select').selectedIndex = 2;

				vlanModeChange();
				showPortStatus();
			}

			// Change mode LAN VLAN/WAN VLAN
			function vlanModeChange()
			{
				var mode = document.getElementById('vlanMode_select').selectedIndex;

				displayElement([ 'vlanModeWLANVLAN_table', 'vlanMode_wlan_ap_tr',   'vlanMode_wlan_iface_tr',  'vlanMode_wlan_vlan_tr', 'vlanMode_wlan_vlan_add' ], mode == 1);
				displayElement([ 'vlanModeLANVLAN_table', 'vlanMode_lan_vlan_tr', 'vlanMode_lan_vlanid_tr', 'vlanMode_lan_isolated_tr', 'vlanMode_lan_vlan_add' ],  mode == 2);

				if (mode == 1) {
					changeWLANVLAN();
					showWLANVLAN();
				}
				else if (mode == 2) {
					changeLANVLAN();
					showLANVLAN();
				}
			}

			// Change interface and VLAN in settings table
			function changeWLANVLAN()
			{
				var num		= document.getElementById('vlanMode_wlan_ap_select').value;
				var vlan	= document.getElementById('vlanMode_wlan_vlan_input').value;
				var iface	= document.getElementById("vlanMode_wlan_iface_select").selectedIndex;
				var id = i = 0;
				for (i = 0; i <= num; i++)
					if (wifiNIC[num] == "ra" && wifiNIC[i] == "ra")
						id++;
					else if (wifiNIC[num] == "rai" && wifiNIC[i] == "rai")
						id++;
				id--;

				if (wifiNIC[num] == "ra")
					if (wifiWan[id] != "0") {
						vlan = wifiWan[id];
						iface = "1";
					}
					else if (wifiLan[id] != "0") {
						vlan = wifiLan[id];
						iface = "0";
					}
					else {
						vlan = "";
						iface = "0";
					}
				else 
					if (wifiWanINIC[id] != "0") {
						vlan = wifiWanINIC[id];
						iface = "1";
					}
					else if (wifiLanINIC[id] != "0") {
						vlan = wifiLanINIC[id];
						iface = "0";
					}
					else {
						vlan = "";
						iface = "0";
					}
				document.getElementById('vlanMode_wlan_vlan_input').value = vlan;
				document.getElementById("vlanMode_wlan_iface_select").selectedIndex = iface;
			}

			// Delete LAN VLAN
			function deleteWLANVLAN(num, iface)
			{
				var id = 0;
				
				for (var i = 0; i <= num; i++) 
					if (wifiNIC[num] == "ra" && wifiNIC[i] == "ra")
						id++;
					else if (wifiNIC[num] == "rai" && wifiNIC[i] == "rai")
						id++;
				id--;
				
				if (wifiNIC[num] == "ra") 
					if (iface == "WAN") 
						wifiWan[id] = "0";
					else
						wifiLan[id] = "0";
				else
					if (iface == "WAN") 
						wifiWanINIC[id] = "0";
					else
						wifiLanINIC[id] = "0";
				changeWLANVLAN();
				showWLANVLAN();
			}

			function applyWLANVLAN()
			{
				var num		= document.getElementById('vlanMode_wlan_ap_select').value;
				var iface	= (document.getElementById('vlanMode_wlan_iface_select').value == 0) ? "LAN" : "WAN";
				var vlan	= document.getElementById('vlanMode_wlan_vlan_input').value;
				var id		= 0;

				for (var i = 0; i <= num; i++) 
					if (wifiNIC[num] == "ra" && wifiNIC[i] == "ra")
						id++;
					else if (wifiNIC[num] == "rai" && wifiNIC[i] == "rai")
						id++;
				id--;

				if (vlan.match(/[^0-9]/gi) || (vlan > 4096)) {
					alert(_("vlan invalid vlanid"));
					document.getElementById('vlanMode_wlan_vlan_input').select();
					document.getElementById('vlanMode_wlan_vlan_input').focus();
					return false;
				}

				if (wifiLan.indexOf(vlan) != -1 ||
					wifiWan.indexOf(vlan) != -1 ||
					wifiLanINIC.indexOf(vlan) != -1 ||
					wifiWanINIC.indexOf(vlan) != -1 ) {
						alert(_("vlan vlanid exist"));
						document.getElementById('vlanMode_wlan_vlan_input').select();
						document.getElementById('vlanMode_wlan_vlan_input').focus();
						return false
				}

				if (wifiNIC[num] == "ra")
					if (iface == "WAN") {
						wifiLan[id] = "0";
						wifiWan[id] = vlan;
					}
					else {
						wifiLan[id] = vlan;
						wifiWan[id] = "0";
					}
				else 
					if (iface == "WAN") {
						wifiWanINIC[id] = vlan;
						wifiLanINIC[id] = "0";
					}
					else {
						wifiLanINIC[id] = vlan;
						wifiWanINIC[id] = "0";
					}

				changeWLANVLAN();
				showWLANVLAN();
			}			

			// Get SSID list
			function getWLANVLANvalues()
			{
				maxBSSID	= 4;
				mbssid_mode = document.getElementById('vlanMode_MBSSID_mode').value;
				ssid_num	= document.getElementById('vlanMode_SSID_num').value;
				wifiLan 	= (document.getElementById('vlanMode_WifiLan').value.length > 0) ? document.getElementById('vlanMode_WifiLan').value.split(" ") : "0 0 0 0".split(" ");
				wifiLanINIC = (document.getElementById('vlanMode_WifiLanINIC').value.length > 0) ? document.getElementById('vlanMode_WifiLanINIC').value.split(" ") : "0 0 0 0".split(" ");
				wifiWan		= (document.getElementById('vlanMode_WifiWan').value.length > 0) ? document.getElementById('vlanMode_WifiWan').value.split(" ") : "0 0 0 0".split(" ");
				wifiWanINIC = (document.getElementById('vlanMode_WifiWanINIC').value.length > 0) ? document.getElementById('vlanMode_WifiWanINIC').value.split(" ") : "0 0 0 0".split(" ");

				for (var i = 1; i < maxBSSID; i++) {
					if (mbssid_mode == "ra")
						if (wifiLanINIC[i] != 0 || wifiWanINIC[i] != 0) {
							wifiLanINIC[i] = 0;
							wifiWanINIC[i] = 0;
						}
					else
						if (wifiLan[i] != 0 || wifiWan[i] != 0) {
							wifiLan[i] = 0;
							wifiWan[i] = 0;
						}
					if (wifiLan[i] === undefined)
						wifiLan[i] = 0;
					if (wifiWan[i] === undefined)
						wifiWan[i] = 0;
					if (wifiLanINIC[i] === undefined)
						wifiLanINIC[i] = 0;
					if (wifiWanINIC[i] === undefined)
						wifiWanINIC[i] = 0;
				}

				var ssid24	= '<% getCfgZero(1, "SSID1"); %>';
				var ssid5	= '<% getCfgZero(1, "SSID1INIC"); %>';
				var data, i;

				if (ssid24 == ssid5) 
					if (mbssid_mode == "ra")
						ssid5 += " [5GHz]";
					else 
						ssid24 += " [2.4GHz]";
				
				try {
					data	= JSON.parse(document.getElementById('vlanMode_SsidList').value);
				} catch(e) {}

				if (mbssid_mode == "ra" && is5gh_support == 1) {
					wifiNIC.push("rai");
					wifiSSID.push(ssid5);
					ssid_num++;
				}
				else if (mbssid_mode == "rai" && is5gh_support == 1) {
					wifiNIC.push("ra");
					wifiSSID.push(ssid24);
					ssid_num++;
				}

				for (i = 0; i < data.wireless.length; i ++) {
					if (mbssid_mode == "ra") 
						wifiNIC.push("ra");
					else
						wifiNIC.push("rai");
					wifiSSID.push(data.wireless[i].ssid);
				}
				for (i = 0; i < wifiSSID.length; i++)
					addOption(document.wlanLanCfg.vlanMode_wlan_ap_select, wifiSSID[i], i);
			}

			function showWLANVLAN()
			{
				var html, i, vlan, iface;
				var ra = rai = 0;

				html  = '<table class="form">';
				html += '<tr>';
				html += '<td class="title" colspan="4" id="vlanMode_wlan_table">WLAN VLAN Status</td>';
				html += '</tr>';
				html += '<th id="vlanMode_wlan_table_ap" style="width: 30%">Access Point</th>';
				html += '<th id="vlanMode_wlan_table_iface" style="width: 30%">Interface</th>';
				html += '<th id="vlanMode_wlan_table_vlan" style="width: 30%">VLAN</th>';
				html += '<th id="vlanMode_wlan_table_action" style="width: 10%">Action</th>';

				for (i = 0; i < ssid_num; i++) {
					if (wifiNIC[i] == "ra") {
						if (wifiLan[ra] != "0") {
							vlan  = wifiLan[ra];
							iface = "LAN";
						} else if (wifiWan[ra] != "0") {
							vlan  = wifiWan[ra];
							iface = "WAN";
						}
						else {
							vlan  = "-";
							iface = "-";
						}
						ra++;
					}
					else {
						if (wifiLanINIC[rai] != "0") {
							vlan  = wifiLanINIC[rai];
							iface = "LAN";
						} else if (wifiWanINIC[rai] != "0") {
							vlan  = wifiWanINIC[rai];
							iface = "WAN";
						}
						else {
							vlan  = "-";
							iface = "-";
						}
						rai++;
					}

					if (vlan != "-") {
						html += '<tr>';
						html += '<td style="text-align: center;">' + wifiSSID[i] + '</td>';
						html += '<td style="text-align: center;">' + iface + '</td>';
						html += '<td style="text-align: center;">' + vlan + '</td>';
						html += '<td style="text-align: center;">' + '<img src="/graphics/cross.png" onClick="deleteWLANVLAN(' + "'" + i + "', '" + iface + "'" + ');">' + '</td>';
						html += '</tr>';
					}
				}

				html += '</table>';
				document.getElementById('vlanModeWLANVLAN_table').innerHTML = html;
				initTranslation();
			}

			function changeLANVLAN()
			{
				var vlanid = vlanLan[document.getElementById('vlanMode_lan_vlan_select').value - 1];
				document.getElementById('vlanMode_lan_vlanid_input').value = vlanid;
				if (vlanLanIsolate.indexOf(vlanid) >= 0)
					document.getElementById('vlanMode_lan_isolated_input').checked = true;
				else
					document.getElementById('vlanMode_lan_isolated_input').checked = false;
			}

			function dontchangeLANVLAN(id)
			{
				if (document.getElementById(id).checked == true)
					document.getElementById(id).checked = false;
				else
					document.getElementById(id).checked = true;
			}

			function getLANVLANvalues()
			{
				if (document.getElementById('vlanMode_VLANlan').value.length > 0)
					vlanLan	= document.getElementById('vlanMode_VLANlan').value.split(" ");
				if (document.getElementById('vlanMode_VLANlanIsolate').value.length > 0)	
					vlanLanIsolate	= document.getElementById('vlanMode_VLANlanIsolate').value.split(" ");
					
				if (vlanLan.length == 0)
					vlanLan.push("");
				for (var i = 0; i <= vlanLan.length; i ++)
					if (vlanLan[i] !== undefined)
						addOption(document.wlanLanCfg.vlanMode_lan_vlan_select, i + 1, i + 1);
			}

			function addLANVLAN()
			{
				var createVlan = true;
				for (var i = 0; i < vlanLan.length; i++) {
					if (vlanLan[i] == "") {
						document.getElementById('vlanMode_lan_vlan_select').value = i + 1;
						createVlan = false;
					}
				}
				if (createVlan == true) {
					addOption(document.wlanLanCfg.vlanMode_lan_vlan_select, vlanLan.length + 1, vlanLan.length + 1)
					vlanLan.push("");
					document.getElementById('vlanMode_lan_vlan_select').value = vlanLan.length;
				}
				changeLANVLAN();
				showLANVLAN();
			}

			function applyLANVLAN()
			{
				var id		= document.getElementById('vlanMode_lan_vlan_select').value - 1;
				var vlan	= document.getElementById('vlanMode_lan_vlanid_input').value;

				if (vlan.match(/[^0-9]/gi) || vlan > 4096 || vlan.length == 0) {
					alert(_("vlan invalid vlanid"));
					document.getElementById('vlanMode_lan_vlanid_input').select();
					document.getElementById('vlanMode_lan_vlanid_input').focus();
					return false;
				}

				if (vlanLan.indexOf(vlan) != "-1") {
					alert(_("vlan vlanid exist"));
					document.getElementById('vlanMode_lan_vlanid_input').select();
					document.getElementById('vlanMode_lan_vlanid_input').focus();
					return false
				}

				vlanLan[id] = vlan;
				if (document.getElementById('vlanMode_lan_isolated_input').checked == true) 
					if (vlanLanIsolate.indexOf(vlan) == -1)
						vlanLanIsolate.push(vlan);
				showLANVLAN();
			}

			function deleteLANVLAN(id)
			{
				if (vlanLanIsolate.indexOf(vlanLan[id]) >= 0)
					vlanLanIsolate.splice(vlanLanIsolate.indexOf(vlanLan[id]), 1);
				vlanLan.splice(id, 1);
				document.wlanLanCfg.vlanMode_lan_vlan_select.options.length = 0;
				for (var i = 0; i <= vlanLan.length; i ++)
					if (vlanLan[i] !== undefined)
						addOption(document.wlanLanCfg.vlanMode_lan_vlan_select, i + 1, i + 1);
				if (document.getElementById('vlanMode_lan_vlan_select').value > vlanLan.length) {
					document.getElementById('vlanMode_lan_vlan_select').value = 1;
				}
				if (vlanLan.length == 0)
					addLANVLAN();

				changeLANVLAN();
				showLANVLAN();
			}

			function showLANVLAN()
			{
				var i;
				var isolate;

				html  = '<table class="form">';
				html += '<tr>';
				html += '<td class="title" colspan="4" id="vlanMode_lan_table">LAN VLAN Status</td>';
				html += '</tr>';
				html += '<th id="vlanMode_lan_table_num" style="width: 10%">#</th>';
				html += '<th id="vlanMode_lan_table_vlan" style="width: 40%">VLAN</th>';
				html += '<th id="vlanMode_lan_table_isolated" style="width: 40%">Isolated</th>';
				html += '<th id="vlanMode_lan_table_action" style="width: 10%">Action</th>';

				for (i = 0; i <= vlanLan.length; i++) {
					if (vlanLan[i] !== undefined) {
						html += '<tr>';
						html += '<td style="text-align: center;">' + (i + 1) + '</td>';
						html += '<td style="text-align: center;">' + vlanLan[i] + '</td>';

						if (vlanLanIsolate.indexOf(vlanLan[i]) >= 0)
							isolate = "checked";
						else
							isolate = "";

						html += '<td style="text-align: center;">' + '<input type="checkbox" id="vlanLan' + i + '" ' + isolate + ' onChange="dontchangeLANVLAN(' + "'vlanLan" + i + "'" + ');"></td>';
						html += '<td style="text-align: center;">' + '<img src="/graphics/cross.png" onClick="deleteLANVLAN(' + i + ');">' + '</td>';
						html += '</tr>';
					}
				}

				html += '</table>';
				document.getElementById('vlanModeLANVLAN_table').innerHTML = html;
				initTranslation();
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
			<td><h1 id="vlanPortTitle"></h1>
				<p id="vlanPortIntroduction"></p>
				<hr>
				<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
				<form method="POST" name="lanCfg" action="/goform/setPortVLAN">
				<div id="vlanData" style="display: none;"></div>
				<table align="center">
					<tr>
						<td id="vlanPortStatus_type_port0" style="text-align: center; font-weight: bold;"></td>
						<td id="vlanPortStatus_type_port1" style="text-align: center; font-weight: bold;"></td>
						<td id="vlanPortStatus_type_port2" style="text-align: center; font-weight: bold;"></td>
						<td id="vlanPortStatus_type_port3" style="text-align: center; font-weight: bold;"></td>
						<td id="vlanPortStatus_type_port4" style="text-align: center; font-weight: bold;"></td>
					</tr>
					<tr>
						<td onClick="selectPort(5);"><img src="/graphics/port_yellow.gif"></td>
						<td onClick="selectPort(4);"><img src="/graphics/port_blue.gif"></td>
						<td onClick="selectPort(3);"><img src="/graphics/port_blue.gif"></td>
						<td onClick="selectPort(2);"><img src="/graphics/port_blue.gif"></td>
						<td onClick="selectPort(1);"><img src="/graphics/port_blue.gif"></td>
					</tr>
					<tr>
						<td id="vlanPortStatus_img_port0" style="width: 64px; height: 17px;"></td>
						<td id="vlanPortStatus_img_port1" style="width: 64px; height: 17px;"></td>
						<td id="vlanPortStatus_img_port2" style="width: 64px; height: 17px;"></td>
						<td id="vlanPortStatus_img_port3" style="width: 64px; height: 17px;"></td>
						<td id="vlanPortStatus_img_port4" style="width: 64px; height: 17px;"></td>
					</tr>
				</table>
				<br>
				<div id="vlanPortStatus_table"></div>
				<br>
				<table class="form">
					<tr>
						<td class="title" colspan="3" id="vlanPortManagement">Ethernet Port Management</td>
					</tr>
					<tr>
						<td class="head" id="vlanPort_wan_port">WAN port</td>
						<td colspan="2" id="vlanPort_wan_port_select">
							<select name="wan_port" class="short">
								<option value="0">1</option>
								<option value="4">5</option>
							</select>
						</td>
					</tr>
					<tr>
						<td class="head" id="vlanPort_lan_port">First LAN port</td>
						<td colspan="2" id="vlanPort_lan_port_select">
							<select name="lan_port" class="mid">
								<option value="near" id="vlanPort_lan_port_near">Near to the WAN port</option>
								<option value="distant" id="vlanPort_lan_port_distant">Distant from the WAN port</option>
							</select>
						</td>
					</tr>
					<tr id="vlanPort_tv_stb_tr">
						<td id="tv_stb" class="head">TV/STB/VLAN1</td>
						<td id="tv_stb_en">
							<font id="tv_stb_enable">Enable:</font><input id="tv_stbEnabled" name="tv_stbEnabled" type="checkbox" OnClick="checkSTBform();">
							<font id="tv_stb_mcast">McastProxy:</font><input id="tv_stbMcast" name="tv_stbMcast" type="checkbox"></td>
						<td id="tv_stb_vlan">
							<font id="tv_stb_vlanid">VlanID(s):</font><input id="tv_stbVLAN" name="tv_stbVLAN" class="wide" size="30" maxlength="60" type="text" onKeyUp="checkSTBform();"></td>
					</tr>
					<tr id="sip_stb_tr">
						<td id="sip_stb" class="head">SIP/STB/VLAN2</td>
						<td id="sip_stb_en">
							<font id="sip_stb_enable">Enable:</font><input id="sip_stbEnabled" name="sip_stbEnabled" type="checkbox" OnClick="checkSIPform();">
							<font id="sip_stb_mcast">McastProxy:</font><input id="sip_stbMcast" name="sip_stbMcast" type="checkbox"></td>
						<td id="sip_stb_vlan">
							<font id="sip_stb_vlanid">VlanID(s):</font><input id="sip_stbVLAN" name="sip_stbVLAN" class="wide" size="30" maxlength="60" type="text" onKeyUp="checkSIPform();"></td>
					</tr>
					<tr>
						<td class="head" id="vlanPortMode">Port Mode</td>
						<td colspan="2" id="port_swmode">
							<select name="port_num" class="short" onChange="selectPort();">
								<option value="1">1</option>
								<option value="2">2</option>
								<option value="3">3</option>
								<option value="4">4</option>
								<option value="5">5</option>
							</select>&nbsp;&nbsp;
							<select name="port_mode" class="mid">
								<option id="port_mode_auto"  value="auto" >Auto-Negotiation</option>
								<option id="port_mode_10h"   value="10h"  >10Mbit/s Half-Duplex</option>
								<option id="port_mode_10f"   value="10f"  >10Mbit/s Full-Duplex</option>
								<option id="port_mode_100h"  value="100h" >100Mbit/s Half-Duplex</option>
								<option id="port_mode_100f"  value="100f" >100Mbit/s Full-Duplex</option>
								<option id="port_mode_1000f" value="1000f">1Gbit/s Full-Duplex</option>
							</select>&nbsp;&nbsp;
							<input type="button" id="port_mode_setup" value="Set" onClick="setPort();">
						</td>
					</tr>
				</table>
				<br>
				<table class="buttons">
					<tr>
						<td><input type="submit" class="normal" value="Apply"  id="vlanApply"  onClick="return checkValues(this.form);">&nbsp; &nbsp;
							<input type="button" class="normal" value="Cancel" id="vlanCancel" onClick="window.location.reload();">&nbsp; &nbsp;
							<input type="button" class="normal" value="Reset"  id="vlanReset"  onClick="this.form.reboot.value = 0; resetValues(this.form, 5);">
							<input type="hidden" id="port1_swmode" name="port1_swmode">
							<input type="hidden" id="port2_swmode" name="port2_swmode">
							<input type="hidden" id="port3_swmode" name="port3_swmode">
							<input type="hidden" id="port4_swmode" name="port4_swmode">
							<input type="hidden" id="port5_swmode" name="port5_swmode">
							<input type="hidden" value="1" name="reboot">
							<input type="hidden" value="0" name="reset">
						</td>
					</tr>
				</table>
			</td>
			</tr>
			</form>
			<tr>
				<td>
					<br>
					<h1 id="vlanWlanLanTitle"></h1>
					<p id="vlanWlanLanIntro"></p>
					<hr>
					<div id="vlanModeWLANVLAN_table" style="display: none"></div>
					<div id="vlanModeLANVLAN_table" style="display: none"></div>
					<form method="POST" name="wlanLanCfg" action="/goform/setWlanLanVLAN">
					<table class="form">
						<tr>
							<td class="title" colspan="3" id="vlanModeSettings">WLAN VLANs / LAN VLANs Settings</td>
						</tr>
						<tr>
							<td id="vlanMode" class="head" style="width: 40%;">Mode</td>
							<td>
								<select id="vlanMode_select" class="mid" onChange="vlanModeChange();">
									<option value="0" id="vlanMode_select_disable">Disable</option>
									<option value="1" id="vlanMode_select_wlan">WLAN VLANs</option>
									<option value="2" id="vlanMode_select_lan">LAN VLANs</option>
								</select>
							</td>
						</tr>
						<tr id="vlanMode_wlan_ap_tr">
							<td id="vlanMode_wlan_ap" class="head">AP</td>
							<td>
								<select id="vlanMode_wlan_ap_select" name="vlanMode_wlan_ap_select" class="mid" onChange="changeWLANVLAN();"></select>
							</td>
						</tr>
						<tr id="vlanMode_wlan_iface_tr">
							<td id="vlanMode_wlan_iface" class="head">Interface</td>
							<td>
								<select id="vlanMode_wlan_iface_select" name="vlanMode_wlan_iface_select" class="mid">
									<option value="0">LAN</option>
									<option value="1">WAN</option>
								</select>
							</td>
						</tr>
						<tr id="vlanMode_wlan_vlan_tr">
							<td id="vlanMode_wlan_vlan" class="head">VLAN</td>
							<td><input id="vlanMode_wlan_vlan_input" name="vlanMode_wlan_vlan_input" class="mid"></td>
						</tr>

						
						<tr id="vlanMode_lan_vlan_tr">
							<td id="vlanMode_lan_num" class="head">#</td>
							<td>
								<select id="vlanMode_lan_vlan_select" name="vlanMode_lan_vlan_select" class="mid" onChange="changeLANVLAN();"></select>&nbsp; &nbsp;
								<input type="button" class="normal" value="Add VLAN" id="vlanMode_lan_add" onClick="addLANVLAN();">
							</td>
						</tr>
						<tr id="vlanMode_lan_vlanid_tr">
							<td id="vlanMode_lan_vlanid" class="head">VLAN ID</td>
							<td><input id="vlanMode_lan_vlanid_input" name="vlanMode_lan_vlanid_input" class="mid"></td>
						</tr>
						<tr id="vlanMode_lan_isolated_tr">
							<td id="vlanMode_lan_isolated" class="head">Isolated</td>
							<td><input type="checkbox" id="vlanMode_lan_isolated_input" name="vlanMode_lan_isolated_input"></td>
						</tr>
					</table>
					<table class="buttons">
						<tr id="vlanMode_wlan_vlan_add">
							<td>
								<input type="button" class="normal" value="Apply VLAN" id="vlanMode_wlan_apply" onClick="applyWLANVLAN();">
							</td>
						</tr>
					</table>
					<table class="buttons">
						<tr id="vlanMode_lan_vlan_add">
							<td>
								<input type="button" class="normal" value="Apply VLAN" id="vlanMode_lan_apply" onClick="applyLANVLAN();">
							</td>
						</tr>
					</table>
					<br>
					<table class="buttons">
						<tr>
							<td><input type="submit" class="normal" value="Apply"  id="vlanModeApply"  onClick="return checkVlanLanValues(this.form);">&nbsp; &nbsp;
								<input type="button" class="normal" value="Cancel" id="vlanModeCancel" onClick="window.location.reload();">&nbsp; &nbsp;
								<input type="button" class="normal" value="Reset"  id="vlanModeReset"  onClick="this.form.reboot.value = 0; resetValues(this.form, 5);">
								<input type="hidden" value="" name="wifi_lan">
								<input type="hidden" value="" name="wifi_lan_inic">
								<input type="hidden" value="" name="wifi_wan">
								<input type="hidden" value="" name="wifi_wan_inic">
								<input type="hidden" value="" name="vlan_lan">
								<input type="hidden" value="" name="vlan_lan_isolate">
								<input type="hidden" value="1" name="reboot">
								<input type="hidden" value="0" name="reset">
							</td>
						</tr>
					</table>
				</td>
			</tr>
		</table>
	</body>
</html>