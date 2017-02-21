<!DOCTYPE html>
<html>
	<head>
		<title>Virtual Local Area Network (VLAN) Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			Butterlate.setTextDomain("internet");
			Butterlate.setTextDomain("buttons");

			var wifiNIC					= [];
			var wifiSSID				= [];
			var wifiLan					= [];
			var wifiLanINIC				= [];
			var wifiWan					= [];
			var wifiWanINIC				= [];
			var vlanLan					= [];
			var vlanLanIsolate			= [];

			var vlanTv					= NVRAM_tv_portVLAN.replace(/\s+/g, '').split(",");
			var vlanTvPrio				= NVRAM_tv_portVLANPRIO.replace(/\s+/g, '').split(",");
			var vlanSip					= NVRAM_sip_portVLAN.replace(/\s+/g, '').split(",");
			var vlanSipPrio				= NVRAM_sip_portVLANPRIO.replace(/\s+/g, '').split(",");

			function initTranslation() {
				_TR("vlanTvSipTitle",		"vlan tvsip title");
				_TR("vlanTvSipIntroduction",	"vlan tvsip introduction");
				_TR("tv_stb",			"vlan tvsip tv stb");
				_TR("tv_stb_enable",		"vlan tvsip tv stb enable");
				_TR("tv_stb_mcast",		"vlan tvsip tv stb mcast");
				_TR("tv_stb_vlanid",		"vlan vlanid");
				_TR("tv_stb_prio",		"vlan tvsip vlanid prio");
				_TR("sip_stb",			"vlan tvsip sip stb");
				_TR("sip_stb_enable",		"vlan tvsip sip stb enable");
				_TR("sip_stb_mcast",		"vlan tvsip sip stb mcast");
				_TR("sip_stb_vlanid",		"vlan vlanid");
				_TR("sip_stb_prio",		"vlan tvsip vlanid prio");
				_TRV("tv_stbVLANbutton",	"vlan add button");
				_TRV("sip_stbVLANbutton",	"vlan add button");
				_TR("vlanWlanLanTitle",		"vlan mode title");
				_TR("vlanWlanLanIntro",		"vlan mode introduction");
				_TR("vlanModeSettings",		"vlan mode settings");
				_TR("vlanMode",			"vlan mode select");
				_TR("vlanMode_select_disable",	"vlan mode select disable");
				_TR("vlanMode_select_wlan",	"vlan mode select wlan");
				_TR("vlanMode_select_lan",	"vlan mode select lan");
				_TR("vlanMode_wlan_ap",		"vlan mode wlan ap");
				_TR("vlanMode_wlan_iface",	"vlan mode wlan iface");
				_TR("vlanMode_wlan_vlan",	"vlan vlanid");
				_TRV("vlanMode_wlan_add",	"vlan add button");
				_TR("vlanMode_lan_num",		"vlan mode lan num");
				_TR("vlanMode_lan_vlanid",	"vlan vlanid");
				_TR("vlanMode_lan_isolated",	"vlan mode lan isolated");
				_TRV("vlanMode_lan_add",	"vlan add button");
				_TRV("vlanApply",		"button apply");
				_TRV("vlanCancel",		"button cancel");
				_TRV("vlanReset",		"button reset");
				_TRV("vlanModeApply",		"button apply");
				_TRV("vlanModeCancel",		"button cancel");
				_TRV("vlanModeReset",		"button reset");
				
				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					switch (elements[i].id) {
						case 'disable':		elements[i].innerHTML = _("button disable");	break;
						case 'enable':		elements[i].innerHTML = _("button enable");	break;
					}
			}

			function initValues()
			{
				// Initializing variables
				document.getElementById('tv_stbEnabled').value		= ('<% getCfgGeneral(1, "tv_port"); %>' == '1') ? "1" : "0";
				document.getElementById('tv_stbMcast').value		= ('<% getCfgGeneral(1, "tv_port_mcast"); %>' == '1') ? "1" : "0";
				document.getElementById('sip_stbEnabled').value		= ('<% getCfgGeneral(1, "sip_port"); %>' == '1') ? "1" : "0";
				document.getElementById('sip_stbMcast').value		= ('<% getCfgGeneral(1, "sip_port_mcast"); %>' == '1') ? "1" : "0";

				// Clearing empty table;
				if ((vlanTv.length > 0 && vlanTv[0].length == "") || document.getElementById('tv_stbEnabled').value == 0) {
					vlanTv = [];
					vlanTvPrio = [];
				}
				if ((vlanSip.length > 0 && vlanSip[0].length == "")  || document.getElementById('sip_stbEnabled').value == 0) {
					vlanSip = [];
					vlanSipPrio = [];
				}

				displayElement(['vlanPort_tv_stb_mcast_tr','vlanPort_tv_stb_vlan_tr'], document.getElementById('tv_stbEnabled').value == '1');
				displayElement(['vlanPort_sip_stb_mcast_tr','vlanPort_sip_stb_vlan_tr'], document.getElementById('sip_stbEnabled').value == '1');
				document.getElementById('tv_stbMcast').disabled = (vlanTv.length > 0) ? false : true;
				document.getElementById('sip_stbMcast').disabled = (vlanSip.length > 0) ? false : true;

				getWLANVLANvalues();
				getLANVLANvalues();

				if (NVRAM_VlanWifiLan.length > 0 || NVRAM_VlanWifiLanINIC.length > 0 ||
				    NVRAM_VlanWifiWan.length > 0 || NVRAM_VlanWifiWanINIC.length > 0) 
					document.getElementById('vlanMode_select').selectedIndex = 1;
				if (NVRAM_VlanLan.length > 0 || NVRAM_VlanLanIsolate.length > 0)
					document.getElementById('vlanMode_select').selectedIndex = 2;
				
				showWarning();
				vlanModeChange();
				showTvSipVLAN();
				interceptVLANinput();
				
				displayElement([ 'vlanPort_table' ], ETHER_PORTS == 5 && NVRAM_OperationMode != '0');
			}

			function checkValues(form) 
			{
				// Checking port1/2 VLAN set
				if ((document.getElementById('tv_stbEnabled').value == '1' && vlanTv.length > 0) && (document.getElementById('sip_stbEnabled').value == '1' && vlanSip.length == 0)) {
						alert(_("vlan tvsip need sip vlan"));
						document.getElementById('sip_stbVLANid').select();
						document.getElementById('sip_stbVLANid').focus();
						return false;
				}
				
				if ((document.getElementById('sip_stbEnabled').value == '1' && vlanSip.length > 0) && (document.getElementById('tv_stbEnabled').value == '1' && vlanTv.length == 0)) {
						alert(_("vlan tvsip need tv vlan"));
						document.getElementById('tv_stbVLANid').select();
						document.getElementById('tv_stbVLANid').focus();
						return false;
				}
				
				//Generating TV/STB/VLAN1 VLAN list
				var value		= "";
				var valuePrio	= "";
				for (var i = 0; i < vlanTv.length; i++) {
					value		+= vlanTv[i];
					valuePrio	+= (vlanTvPrio[i].length > 0) ? vlanTvPrio[i] : 0;
					if (i + 1 < vlanTv.length) {
						value		+= ",";
						valuePrio	+= ",";
					}
				}
				document.getElementById('tv_stbVLAN').value = value;
				document.getElementById('tv_stbVLANprio').value = valuePrio;
				
				//Generating SIP/STB/VLAN1 VLAN list
				value		= "";
				valuePrio	= "";
				for (var i = 0; i < vlanSip.length; i++) {
					value		+= vlanSip[i];
					valuePrio	+= (vlanSipPrio[i].length > 0) ? vlanSipPrio[i] : 0;
					if (i + 1 < vlanSip.length) {
						value		+= ",";
						valuePrio	+= ",";
					}
				}
				document.getElementById('sip_stbVLAN').value = value;
				document.getElementById('sip_stbVLANprio').value = valuePrio;

				if (!ajaxPostForm(_('vlan reboot confirm'), form, 'timerReloader', _("message config"), ajaxShowProgress)) {
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
					form.reboot.value = "0";
					form.submit();
				}
				return true;
			}

			function checkVlanLanValues(form)
			{
				var wifi_lan		= "";
				var wifi_wan		= "";
				var wifi_lan_inic	= "";
				var wifi_wan_inic	= "";
				var vlan_lan		= "";
				var vlan_lan_isolate	= "";
				var ra = rai		= 0;
				
				if (form.vlanMode_select.selectedIndex == 1) {
					for (var i = 0; i < NVRAM_BssidNum; i++)
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
					wifi_lan		= (wifi_lan == "0") ? "" : wifi_lan;
					wifi_wan		= (wifi_wan == "0") ? "" : wifi_wan;
					wifi_lan_inic	= (wifi_lan_inic == "0") ? "" : wifi_lan_inic;
					wifi_wan_inic	= (wifi_wan_inic == "0") ? "" : wifi_wan_inic;
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
			
			// Changing TV VLAN state
			function changeTVstate()
			{
				var state = document.getElementById('tv_stbEnabled').value == '1'
				displayElement(['vlanPort_tv_stb_mcast_tr','vlanPort_tv_stb_vlan_tr'], state);
				if (state && vlanSip.length > 0 && vlanTv.length == 0)
					alert(_("vlan tvsip need tv vlan"));
			}

			// Changing SIP VLAN state
			function changeSIPstate()
			{
				var state = document.getElementById('sip_stbEnabled').value == '1';
				displayElement(['vlanPort_sip_stb_mcast_tr','vlanPort_sip_stb_vlan_tr'], state);
				if (state && vlanTv.length > 0 && vlanSip.length == 0)
					alert(_("vlan tvsip need sip vlan"));
			}
			// Show TV/SIP VLAN list
			function showTvSipVLAN()
			{
				var html, i, prio;
				var show = false;

				html  = '<table class="form">';
				html += '<tr>';
				html += '<td class="title" colspan="4">' + _("vlan tvsip status title") + '</td>';
				html += '</tr>';
				html += '<th style="width: 40%">' + _("vlan tvsip status port") + '</th>';
				html += '<th style="width: 20%">' + _("vlan vlanid") + '</th>';
				html += '<th style="width: 20%">' + _("vlan tvsip vlanid prio") + '</th>';
				html += '<th style="width: 20%">' + _("vlan action") + '</th>';

				for (i = 0; i < vlanTv.length; i++)
					if (vlanTv[i].length > 0 && document.getElementById('tv_stbEnabled').value == "1") {
						prio = (vlanTvPrio[i] == '') ? 0 : vlanTvPrio[i];
						html += '<tr>';
						html += '<td style="text-align: center;">' + _("vlan tvsip tv stb") + '</td>';
						html += '<td style="text-align: center;">' + vlanTv[i] + '</td>';
						html += '<td style="text-align: center;">' + prio + '</td>';
						html += '<td style="text-align: center;">';
						html += '<img src="/graphics/edit.png" alt="[+]" title="' + _("vlan edit record") + '" onClick="editTvSipVLAN(' + "'tv', " + i + ');"> ';
						html += '<img src="/graphics/cross.png" alt="[-]" title="' + _("vlan delete record") + '" onClick="deleteTvSipVLAN(' + "'tv', " + i + ');">';
						html += '</tr>';
						show = true;
					}

				for (i = 0; i < vlanSip.length; i++)
					if (vlanSip[i].length > 0 && document.getElementById('sip_stbEnabled').value == "1") {
						prio = (vlanSipPrio[i] == '') ? 0 : vlanSipPrio[i];
						html += '<tr>';
						html += '<td style="text-align: center;">' + _("vlan tvsip sip stb") + '</td>';
						html += '<td style="text-align: center;">' + vlanSip[i] + '</td>';
						html += '<td style="text-align: center;">' + prio + '</td>';
						html += '<td style="text-align: center;">';
						html += '<img src="/graphics/edit.png" alt="[+]" title="' + _("vlan edit record") + '" onClick="editTvSipVLAN(' + "'sip', " + i + ');"> ';
						html += '<img src="/graphics/cross.png" alt="[-]" title="' + _("vlan delete record") + '" onClick="deleteTvSipVLAN(' + "'sip', " + i + ');">';
						html += '</tr>';
						show = true;
					}

				html += '</table>';
				document.getElementById('vlanPort_status_table').innerHTML = html;
				displayElement('vlanPort_status_table', show)
				initTranslation();
			}

			// Add TV/SIP VLAN
			function addTvSipVLAN(type)
			{
				var vlan, prio, num;

				if (type == 'tv') {
					vlan	= document.getElementById('tv_stbVLANid').value;
					prio	= document.getElementById('tv_stbVLANidPrio').value;
					num	= document.getElementById('tv_stbVLANnum').value;

					// Check is VLAN valid
					if (!validateNum(vlan) || vlan <= 0 || vlan > 4095) {
						alert(_("vlan invalid vlanid"));
						document.getElementById('tv_stbVLANid').select();
						document.getElementById('tv_stbVLANid').focus();
						return false
					}

					// Check is VLAN exist
					for (var i = 0; i < vlanTv.length; i++) 
						if (vlan == vlanTv[i] && num != i) {
							alert(_("vlan vlanid exist"));
							document.getElementById('tv_stbVLANid').select();
							document.getElementById('tv_stbVLANid').focus();
							return false;
						}
					for (var i = 0; i < vlanSip.length; i++) 
						if (vlan == vlanSip[i]) {
							alert(_("vlan vlanid exist"));
							document.getElementById('tv_stbVLANid').select();
							document.getElementById('tv_stbVLANid').focus();
							return false;
						}

					if (num == "new") {
						vlanTv.push(+vlan + "");
						vlanTvPrio.push(prio);
					}
					else {
						vlanTv[num]		= +vlan + "";
						vlanTvPrio[num] = prio;
					}
					document.getElementById('tv_stbVLANnum').value		= "new";
					document.getElementById('tv_stbVLANid').value		= "";
					document.getElementById('tv_stbVLANidPrio').value	= 0;
					document.getElementById('tv_stbMcast').disabled		= false;
					
					if (document.getElementById('sip_stbEnabled').value == '1' && vlanSip.length == 0)
						alert(_("vlan tvsip need sip vlan"));					
				}
				else {
					vlan	= document.getElementById('sip_stbVLANid').value;
					prio	= document.getElementById('sip_stbVLANidPrio').value;
					num		= document.getElementById('sip_stbVLANnum').value;

					// Check is VLAN valid
					if (!validateNum(vlan) || vlan <= 0 || vlan > 4095) {
						alert(_("vlan invalid vlanid"));
						document.getElementById('sip_stbVLANid').select();
						document.getElementById('sip_stbVLANid').focus();
						return false;
					}

					// Check is VLAN exist
					for (var i = 0; i < vlanSip.length; i++) 
						if (vlan == vlanSip[i] && num != i) {
							alert(_("vlan vlanid exist"));
							document.getElementById('sip_stbVLANid').select();
							document.getElementById('sip_stbVLANid').focus();
							return false;
						}
					for (var i = 0; i < vlanTv.length; i++) 
						if (vlan == vlanTv[i]) {
							alert(_("vlan vlanid exist"));
							document.getElementById('sip_stbVLANid').select();
							document.getElementById('sip_stbVLANid').focus();
							return false;
						}

					if (num == "new") {
						vlanSip.push(+vlan + "");
						vlanSipPrio.push(prio);
					}
					else {
						vlanSip[num]		= +vlan + "";
						vlanSipPrio[num]	= prio;
					}
					document.getElementById('sip_stbVLANnum').value		= "new";
					document.getElementById('sip_stbVLANid').value		= "";
					document.getElementById('sip_stbVLANidPrio').value	= 0;
					document.getElementById('sip_stbMcast').disabled	= false;

					if (document.getElementById('tv_stbEnabled').value == '1' && vlanTv.length == 0)
						alert(_("vlan tvsip need tv vlan"));					
				}
				showTvSipVLAN();
			}

			// Edit TV/SIP VLAN
			function editTvSipVLAN(type, num)
			{
				if (type == 'tv') {
					document.getElementById('tv_stbVLANid').value		= vlanTv[num];
					document.getElementById('tv_stbVLANidPrio').value	= (vlanTvPrio[num].length > 0) ? vlanTvPrio[num] : 0;
					document.getElementById('tv_stbVLANnum').value		= num;
					document.getElementById('tv_stbVLANid').select();
					document.getElementById('tv_stbVLANid').focus();
				}
				else {
					document.getElementById('sip_stbVLANid').value		= vlanSip[num];
					document.getElementById('sip_stbVLANidPrio').value	= (vlanSipPrio[num].length > 0) ? vlanSipPrio[num] : 0;
					document.getElementById('sip_stbVLANnum').value		= num;
					document.getElementById('sip_stbVLANid').select();
					document.getElementById('sip_stbVLANid').focus();
				}
			}

			// Delete TV/SIP VLAN
			function deleteTvSipVLAN(type, num)
			{
				if (type == 'tv') {
					vlanTv.splice(num, 1);
					vlanTvPrio.splice(num, 1);
					if (vlanTv.length == 0)
						document.getElementById('tv_stbMcast').disabled = true;
					document.getElementById('tv_stbVLANnum').value = "new";
					document.getElementById('tv_stbVLANid').value = "";
					document.getElementById('tv_stbVLANidPrio').value = 0;
				}
				else {
					vlanSip.splice(num, 1);
					vlanSipPrio.splice(num, 1);
					if (vlanSip.length == 0)
						document.getElementById('sip_stbMcast').disabled = true;
					document.getElementById('sip_stbVLANnum').value = "new";
					document.getElementById('sip_stbVLANid').value = "";
					document.getElementById('sip_stbVLANidPrio').value = 0;
				}
				showTvSipVLAN();
			}
			
			function interceptVLANinput() {
				setTimeout('interceptVLANinput();', 500);
				
				if (vlanTv.length > 0 && document.getElementById('tv_stbMcast').disabled == true) {
					document.getElementById('tv_stbMcast').disabled = false;
					return;
				}
				else if (vlanSip.length > 0 && document.getElementById('sip_stbMcast').disabled == true) {
					document.getElementById('sip_stbMcast').disabled = false;
					return;
				}
				
				var vlan_tv		= document.getElementById('tv_stbVLANid').value;
				var vlan_sip	= document.getElementById('sip_stbVLANid').value;
				if (validateNum(vlan_tv) && vlan_tv > 0 && vlan_tv < 4096)
					document.getElementById('tv_stbMcast').disabled = false;
				else
					document.getElementById('tv_stbMcast').disabled = true;

				if (validateNum(vlan_sip) && vlan_sip > 0 && vlan_sip < 4096)
					document.getElementById('sip_stbMcast').disabled = false;
				else
					document.getElementById('sip_stbMcast').disabled = true;
			}

			// Change mode LAN VLAN/WAN VLAN
			function vlanModeChange()
			{
				var mode = document.getElementById('vlanMode_select').selectedIndex;

				displayElement([ 'vlanModeWLANVLAN_table', 'vlanMode_wlan_ap_tr',   'vlanMode_wlan_iface_tr',  'vlanMode_wlan_vlan_tr', 'vlanMode_wlan_vlan_add' ], mode == 1);
				displayElement([ 'vlanMode_wlan_iface_tr' ], mode == 1 && NVRAM_OperationMode != '0');
				displayElement([ 'vlanModeLANVLAN_table', 'vlanMode_lan_vlanid_tr', 'vlanMode_lan_isolated_tr' ],  mode == 2);

				if (mode == 1) {
					changeWLANVLAN();
					showWLANVLAN();
				}
				else if (mode == 2)
					showLANVLAN();
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

			function addWLANVLAN()
			{
				var edit	= document.getElementById('lan_id').value;
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

				if (vlan.match(/[^0-9]/gi) || (vlan <= 0 || vlan >= 4096)) {
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
						wifiWan[id] = +vlan + "";
					}
					else {
						wifiLan[id] = +vlan + "";
						wifiWan[id] = "0";
					}
				else
					if (iface == "WAN") {
						wifiWanINIC[id] = +vlan + "";
						wifiLanINIC[id] = "0";
					}
					else {
						wifiLanINIC[id] = +vlan + "";
						wifiWanINIC[id] = "0";
					}

				changeWLANVLAN();
				showWLANVLAN();
			}

			function editWLANVLAN(num, iface, vlan)
			{
				document.getElementById('vlanMode_wlan_ap_select').value = num;
				document.getElementById('vlanMode_wlan_iface_select').value = (iface == "LAN") ? 0 : 1;
				document.getElementById('vlanMode_wlan_vlan_input').value = vlan;
			}

			function getWLANVLANvalues()
			{
				maxBSSID	= 4;
				wifiLan		= (NVRAM_VlanWifiLan.length > 0) ? NVRAM_VlanWifiLan.split(" ") : "0 0 0 0".split(" ");
				wifiLanINIC	= (NVRAM_VlanWifiLanINIC.length > 0) ? NVRAM_VlanWifiLanINIC.split(" ") : "0 0 0 0".split(" ");
				wifiWan		= (NVRAM_VlanWifiWan.length > 0) ? NVRAM_VlanWifiWan.split(" ") : "0 0 0 0".split(" ");
				wifiWanINIC	= (NVRAM_VlanWifiWanINIC.length > 0) ? NVRAM_VlanWifiWanINIC.split(" ") : "0 0 0 0".split(" ");

				for (var i = 1; i < maxBSSID; i++) {
					if (NVRAM_BssidIfName == "ra")
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
					if (NVRAM_BssidIfName == "ra")
						ssid5 += " [5GHz]";
					else
						ssid24 += " [2.4GHz]";

				try {
					data	= JSON.parse(SSID_LIST);
				} catch(e) { return; }

				if (NVRAM_BssidIfName == "ra" && BUILD_5GHZ_SUPPORT == 1) {
					wifiNIC.push("rai");
					wifiSSID.push(ssid5);
					NVRAM_BssidNum++;
				}
				else if (NVRAM_BssidIfName == "rai" && BUILD_5GHZ_SUPPORT == 1) {
					wifiNIC.push("ra");
					wifiSSID.push(ssid24);
					NVRAM_BssidNum++;
				}

				for (i = 0; i < data.wireless.length; i ++) {
					if (NVRAM_BssidIfName == "ra")
						wifiNIC.push("ra");
					else
						wifiNIC.push("rai");
					wifiSSID.push(data.wireless[i].ssid);
				}
				for (i = 0; i < wifiSSID.length; i++)
					addOption(document.getElementById('vlanMode_wlan_ap_select'), wifiSSID[i], i);
			}

			function showWLANVLAN()
			{
				var html, i, vlan, iface;
				var ra = rai = 0;
				var show = false;

				html  = '<table class="form">';
				html += '<tr>';
				html += '<td class="title" colspan="4">' + _("vlan mode wlan table") + '</td>';
				html += '</tr>';
				html += '<th style="width: 30%">' + _("vlan mode wlan table ap") + '</th>';
				html += '<th style="width: 30%">' + _("vlan mode wlan table iface") + '</th>';
				html += '<th style="width: 20%">' + _("vlan vlanid") + '</th>';
				html += '<th style="width: 20%">' + _("vlan action") + '</th>';

				for (i = 0; i < NVRAM_BssidNum; i++) {
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
						html += '<td style="text-align: center;">';
						html += '<img src="/graphics/edit.png" alt="[+]" title="' + _("vlan edit record") + '" onClick="editWLANVLAN(' + "'" + i + "', '" + iface + "', '" + vlan + "'" + ');"> ';
						html += '<img src="/graphics/cross.png" alt="[-]" title="' + _("vlan delete record") + '" onClick="deleteWLANVLAN(' + "'" + i + "', '" + iface + "'" + ');">' + '</td>';
						html += '</tr>';
						show = true;
					}
				}

				html += '</table>';
				document.getElementById('vlanModeWLANVLAN_table').innerHTML = html;
				displayElement('vlanModeWLANVLAN_table', show)
			}

			// LAN VLAN
			//----------------------------------------------------------------------------
			function dontchangeLANVLAN(id)
			{
				if (document.getElementById(id).checked == true)
					document.getElementById(id).checked = false;
				else
					document.getElementById(id).checked = true;
			}

			function getLANVLANvalues()
			{
				if (NVRAM_VlanLan.length > 0)
					vlanLan	= NVRAM_VlanLan.split(" ");
				if (NVRAM_VlanLanIsolate.length > 0)	
					vlanLanIsolate	= NVRAM_VlanLanIsolate.split(" ");
			}

			function addLANVLAN()
			{
				var num		= document.getElementById('lan_id').value;
				var vlan	= document.getElementById('vlanMode_lan_vlanid_input').value;
				
				if (!validateNum(vlan) || vlan <= 0 || vlan > 4095) {
					alert(_("vlan invalid vlanid"));
					document.getElementById('vlanMode_lan_vlanid_input').focus();
					document.getElementById('vlanMode_lan_vlanid_input').select();
					return false
				}

				if (vlanLan.indexOf(vlan) != -1)
					if (num == "new" || num != vlanLan.indexOf(vlan)) {
						alert(_("vlan vlanid exist"));
						document.getElementById('vlanMode_lan_vlanid_input').focus();
						document.getElementById('vlanMode_lan_vlanid_input').select();
						return false
					}

				if (num == "new") {
					vlanLan.push(+vlan + "");
					if (document.getElementById('vlanMode_lan_isolated_input').checked)
						vlanLanIsolate.push(+vlan + "");
				}
				else {
					if (vlanLanIsolate.indexOf(vlan) != -1)
						vlanLanIsolate.splice(vlanLanIsolate.indexOf(vlan), 1);
					vlanLan[num] = +vlan + "";
					if (document.getElementById('vlanMode_lan_isolated_input').checked)
						vlanLanIsolate.push(+vlan + "");
				}
				document.getElementById('vlanMode_lan_vlanid_input').value	= "";
				document.getElementById('vlanMode_lan_isolated_input').checked	= false;
				showLANVLAN();
			}

			function editLANVLAN(num) 
			{
				document.getElementById('vlanMode_lan_vlanid_input').value = vlanLan[num];
				document.getElementById('vlanMode_lan_vlanid_input').select();
				document.getElementById('vlanMode_lan_vlanid_input').focus();
				document.getElementById('vlanMode_lan_isolated_input').checked = (vlanLanIsolate.indexOf(vlanLan[num]) != -1);
				document.getElementById('lan_id').value = num;

			}

			function deleteLANVLAN(id)
			{
				if (vlanLanIsolate.indexOf(vlanLan[id]) >= 0)
					vlanLanIsolate.splice(vlanLanIsolate.indexOf(vlanLan[id]), 1);
				vlanLan.splice(id, 1);
				showLANVLAN();
			}

			function showLANVLAN()
			{
				var i;
				var isolate;

				html  = '<table class="form">';
				html += '<tr>';
				html += '<td class="title" colspan="4">' + _("vlan mode lan table") + '</td>';
				html += '</tr>';
				html += '<th style="width: 40%">' + _("vlan vlanid") + '</th>';
				html += '<th style="width: 40%">' + _("vlan mode lan table isolated") + '</th>';
				html += '<th style="width: 20%">' + _("vlan action") + '</th>';

				for (i = 0; i <= vlanLan.length; i++) {
					if (vlanLan[i] !== undefined) {
						html += '<tr>';
						html += '<td style="text-align: center;">' + vlanLan[i] + '</td>';

						if (vlanLanIsolate.indexOf(vlanLan[i]) >= 0)
							isolate = "checked";
						else
							isolate = "";

						html += '<td style="text-align: center;">' + '<input type="checkbox" id="vlanLan' + i + '" ' + isolate + ' onChange="dontchangeLANVLAN(' + "'vlanLan" + i + "'" + ');"></td>';
						html += '<td style="text-align: center;">' + '<img src="/graphics/edit.png" alt="[+]" title="' + _("vlan edit record") + '" onClick="editLANVLAN(' + i + ');"> ';
						html += '<img src="/graphics/cross.png" alt="[-]" title="' + _("vlan delete record") + '" onClick="deleteLANVLAN(' + i + ');">' + '</td>';
						html += '</tr>';
					}
				}

				html += '</table>';
				document.getElementById('vlanModeLANVLAN_table').innerHTML = html;
				displayElement('vlanModeLANVLAN_table', vlanLan.length > 0);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
			<td id="vlanPort_table"><h1 id="vlanTvSipTitle"></h1>
				<p id="vlanTvSipIntroduction"></p>
				<hr>
				<form method="POST" name="lanCfg" action="/goform/setTvSipVLAN">
				<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
				<!-- TV/SIP VLAN -->
				<div id="vlanPort_status_table"></div>
				<table id="vlanPort_main_table" class="form">
					<col style="width: 40%;"/>
					<col style="width: 46%;"/>
					<col style="width: 14%;"/>
					<tbody>
						<tr>
							<td class="title" id="tv_stb" colspan="2"></td>
							<td class="title" style="text-align:right;">
								<select id="tv_stbEnabled" name="tv_stbEnabled" class="half" onChange="changeTVstate();">
									<option value="0" id="disable"></option>
									<option value="1" id="enable"></option>
								</select>
							</td>
						</tr>
						<tr id="vlanPort_tv_stb_vlan_tr">
							<td id="tv_stb_vlan" class="head">VLAN</td>
							<td colspan="2">
								<span id="tv_stb_vlanid">VLAN ID:</span>
								<input id="tv_stbVLANid" class="half" maxlength="4" type="text">&nbsp;&nbsp;
								<span id="tv_stb_prio">PRIO:</span>
								<select id="tv_stbVLANidPrio">
									<option value="0">0</option>
									<option value="1">1</option>
									<option value="2">2</option>
									<option value="3">3</option>
									<option value="4">4</option>
									<option value="5">5</option>
									<option value="6">6</option>
									<option value="7">7</option>
								</select>&nbsp;&nbsp;
								<input id="tv_stbVLANbutton" type="button" value="Add / Edit" onClick="addTvSipVLAN('tv');">
								<input id="tv_stbVLANnum" type="hidden" value="new">
								<input id="tv_stbVLAN" name="tv_stbVLAN" type="hidden" value="">
								<input id="tv_stbVLANprio" name="tv_stbVLANprio" type="hidden" value="">
							</td>
						</tr>
						<tr id="vlanPort_tv_stb_mcast_tr">
							<td id="tv_stb_mcast" class="head">McastProxy</td>
							<td id="tv_stb_mcast_en" colspan="2">
								<select id="tv_stbMcast" name="tv_stbMcast"  class="half">
									<option value="0" id="disable">Disable</option>
									<option value="1" id="enable">Enable</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="title" colspan="2" id="sip_stb"></td>
							<td class="title" style="text-align:right;">
								<select id="sip_stbEnabled" name="sip_stbEnabled" class="half" onChange="changeSIPstate();">
									<option value="0" id="disable">Disable</option>
									<option value="1" id="enable">Enable</option>
								</select>
							</td>
						</tr>
						<tr id="vlanPort_sip_stb_vlan_tr">
							<td id="sip_stb_vlan" class="head">VLAN</td>
							<td colspan="2">
								<span id="sip_stb_vlanid">VLAN ID:</span>
								<input id="sip_stbVLANid" class="half" maxlength="4" type="text">&nbsp;&nbsp;
								<span id="sip_stb_prio">PRIO</span>
								<select id="sip_stbVLANidPrio">
									<option value="0">0</option>
									<option value="1">1</option>
									<option value="2">2</option>
									<option value="3">3</option>
									<option value="4">4</option>
									<option value="5">5</option>
									<option value="6">6</option>
									<option value="7">7</option>
								</select>&nbsp;&nbsp;
								<input id="sip_stbVLANbutton" type="button" value="Add / Edit" onClick="addTvSipVLAN('sip');">
								<input id="sip_stbVLANnum" type="hidden" value="new">
								<input id="sip_stbVLAN" name="sip_stbVLAN" type="hidden" value="">
								<input id="sip_stbVLANprio" name="sip_stbVLANprio" type="hidden" value="">
							</td>
						</tr>
						<tr id="vlanPort_sip_stb_mcast_tr">
							<td id="sip_stb_mcast" class="head">McastProxy</td>
							<td id="sip_stb_mcast_en" colspan="2">
								<select id="sip_stbMcast" name="sip_stbMcast" class="half">
									<option value="0" id="disable">Disable</option>
									<option value="1" id="enable">Enable</option>
								</select>
							</td>
						</tr>
					</tbody>
				</table>
				<table id="vlanPort_button_table" class="buttons">
					<tr>
						<td><input type="submit" class="normal" value="Apply"  id="vlanApply"  onClick="return checkValues(this.form);">&nbsp;&nbsp;
							<input type="button" class="normal" value="Cancel" id="vlanCancel" onClick="window.location.reload();">&nbsp;&nbsp;
							<input type="button" class="normal" value="Reset"  id="vlanReset"  onClick="this.form.reboot.value = 0; resetValues(this.form, 5);">
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
					<iframe name="timerReloader2" id="timerReloader2" style="width:0;height:0;border:0px solid #fff;"></iframe>
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
								<select id="vlanMode_wlan_ap_select" class="mid" onChange="changeWLANVLAN();"></select>
							</td>
						</tr>
						<tr id="vlanMode_wlan_iface_tr">
							<td id="vlanMode_wlan_iface" class="head">Interface</td>
							<td>
								<select id="vlanMode_wlan_iface_select" class="half">
									<option value="0">LAN</option>
									<option value="1">WAN</option>
								</select>
							</td>
						</tr>
						<tr id="vlanMode_wlan_vlan_tr">
							<td id="vlanMode_wlan_vlan" class="head">VLAN</td>
							<td>
								<input id="vlanMode_wlan_vlan_input" class="half" maxlength="4">&nbsp;&nbsp;
								<input type="button" value="Add / Edit" id="vlanMode_wlan_add" onClick="addWLANVLAN();">
							</td>
						</tr>
						<!-- LAN VLAN -->
						<tr id="vlanMode_lan_isolated_tr">
							<td id="vlanMode_lan_isolated" class="head">Isolated</td>
							<td><input type="checkbox" id="vlanMode_lan_isolated_input"></td>
						</tr>
						<tr id="vlanMode_lan_vlanid_tr">
							<td id="vlanMode_lan_vlanid" class="head">VLAN ID</td>
							<td>
								<input id="vlanMode_lan_vlanid_input" class="half" maxlength="4">&nbsp;&nbsp;
								<input type="button" value="Add / Edit" id="vlanMode_lan_add" onClick="addLANVLAN();">
							</td>
						</tr>
					</table>
					<table class="buttons">
						<tr>
							<td><input type="submit" class="normal" value="Apply"  id="vlanModeApply"  onClick="return checkVlanLanValues(this.form);">&nbsp;&nbsp;
								<input type="button" class="normal" value="Cancel" id="vlanModeCancel" onClick="window.location.reload();">&nbsp;&nbsp;
								<input type="button" class="normal" value="Reset"  id="vlanModeReset"  onClick="this.form.reboot.value = 0; resetValues(this.form, 5);">
								<input type="hidden" value="new" id="lan_id">
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
