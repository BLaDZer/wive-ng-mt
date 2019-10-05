<!DOCTYPE html>
<html>
	<head>
		<title>Main Wireless Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_wireless.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/json2.js"></script>
		<script src="/js/scanap.js"></script>
		<script>
			var mbssidapisolated		= NVRAM_NoForwardingBTNBSSID;
			var tx_stream_idx		= NVRAM_HT_TxStream;
			var rx_stream_idx		= NVRAM_HT_RxStream;
			var ht_mcs			= 1*NVRAM_HT_MCS;

			var mbssid_mode			= NVRAM_BssidIfName;

			var ids_built			= BUILD_IDS_ENABLE == '1';

			var rrm_built			= BUILD_RRM == '1';
			var ft_built			= BUILD_FT == '1';

			var bssid_num			= (1*BSSID_NUM) || 0;

			var maxstanum			= NVRAM_MaxStaNum.split(";")[0];
			var keepalive			= NVRAM_StationKeepAlive.split(";")[0];

			var old_MBSSID;
			var WPAAlgorithms;
			var PreAuthentication;
			var network				=	{};

			var security_modes_list	=	[
											[ _("wireless disable"),	"Disable",			0, 0 ],
											[ "WEP",			"WEPAUTO",			0, 0 ],
											[ "WPA (Enterprise)",		"WPA",				1, 1 ],
											[ "WPA-PSK (Personal)",		"WPAPSK",			0, 0 ],
											[ "WPA2 (Enterprise)",		"WPA2",				1, 1 ],
											[ "WPA2-PSK (Personal)",	"WPA2PSK",			0, 0 ],
											[ "WPA1/2-PSK (Personal)",	"WPAPSKWPA2PSK",		0, 0 ],
											[ "WPA1/2 (Enterprise)",	"WPA1WPA2",			1, 1 ]
										];


			var ChannelList_24G =
			[
				"2412MHz (" + _("station channel") + " 1)",
				"2417MHz (" + _("station channel") + " 2)",
				"2422MHz (" + _("station channel") + " 3)",
				"2427MHz (" + _("station channel") + " 4)",
				"2432MHz (" + _("station channel") + " 5)",
				"2437MHz (" + _("station channel") + " 6)",
				"2442MHz (" + _("station channel") + " 7)",
				"2447MHz (" + _("station channel") + " 8)",
				"2452MHz (" + _("station channel") + " 9)",
				"2457MHz (" + _("station channel") + " 10)",
				"2462MHz (" + _("station channel") + " 11)",
				"2467MHz (" + _("station channel") + " 12)",
				"2472MHz (" + _("station channel") + " 13)",
				"2484MHz (" + _("station channel") + " 14)"
			];

			function updateVisibility(form) {

				var enableWirelessAc	= BUILD_5GHZ_SUPPORT && (form.radioWirelessEnabledAc.value == "1");
				var enableWireless	= form.radioWirelessEnabled.value == "1";
				var enableWirelessAny	= enableWireless || enableWirelessAc;
				var is_7615 = WLAN_FIRST_CARD == "7615" || WLAN_SECOND_CARD == "7615";

				var wmode = form.wirelessmode.value;
				var wmodeac = form.wirelessmodeac.value;

				var vhtmode = enableWirelessAc && (1*wmodeac) >= 14;
				var htmode = vhtmode || (enableWireless && (wmode*1) >= 5) || (enableWirelessAc && (1*wmodeac) >= 8);

				var browser = getBrowser();

				// Wireless Network 2.4 GHz
				var updateVisibility_net24 = function() {
					displayElement( 'wlan_header_data', enableWireless && NVRAM_RadioOn != '0');

					// Network name (2.4GHz)
					displayElement( 'div_11g_name', enableWirelessAny); // do not hide in 5GHz-only mode, it has some usable nested options like "hidden" or "clients isolated", and an "Add" button
					// Network mode (2.4GHz)
					displayElement( 'div_11g_basic', enableWireless);
					// TX Power (2.4GHz)
					displayElement( 'div_txpw', enableWireless);
					// Channel (2.4GHz)
					displayElement( 'div_11g_channel', enableWireless);
					updateChannelList(form, !htmode);
					// Channel autoselect (2.4GHz)
					displayElement( 'div_11g_channel_autoselect', enableWireless && form.sz11gChannel.value == 0);
					displayElement( ['autoselect_g_qbss', 'autoselect_a_qbss'], is_7615);
					enableElements( [document.getElementById('autoselect_g_qbss'), document.getElementById('autoselect_a_qbss')], is_7615);
					form.sz11gChannel.disabled = false;
					// Channel bandwidth (2.4GHz)
					displayElement( 'basicChannelBW_tr', enableWireless);
					form.n_bandwidth.disabled = !(enableWireless && (wmode*1) >= 5);
					displayElement( ['n_bandwidth_20_40'], wmode >= 5);

					displayElement('autoselect_g', form.sz11gChannel.value == 0);
					displayElement('checktime_g',  form.sz11gChannel.value == 0);
					displayElement('basicRescanG', form.sz11gChannel.value == 0);

					// Hide 'SCAN' buttons if IE8 (no SVG support)
					displayElement( ['scanapLegendButtonScan', 'scanapLegendButtonScanResult'], browser.browser != 'ie' || browser.versionShort > 8);

					// Disable scan buttons if radio modules are disabled
					form.scanapLegendButtonScan.disabled = NVRAM_RadioOn != '1';

					// Lower channel bandwidth to the first available option
					while (form.n_bandwidth.selectedIndex > 0 && form.n_bandwidth.options[form.n_bandwidth.selectedIndex].style.display == "none")
					{
						form.n_bandwidth.selectedIndex--;
					}
				}

				// Wireless Network 5 GHz
				var updateVisibility_net5 = function() {
					displayElement( 'wlan_ac_header_data', enableWirelessAc && NVRAM_RadioOnINIC != '0');

					displayElement('wireless_5', BUILD_5GHZ_SUPPORT);
					// Network name (5GHz)
					displayElement( 'div_11a_name', enableWirelessAc);
					// Network mode (5GHz)
					displayElement( 'div_11a_basic', enableWirelessAc);
					// TX Power (5GHz)
					displayElement( 'div_txpw_ac', enableWirelessAc);
					// Channel (5GHz)
					displayElement( 'div_11a_channel', enableWirelessAc);
					form.sz11aChannel.disabled = !BUILD_5GHZ_SUPPORT;
					// Channel autoselect (5GHz)
					displayElement( 'div_11a_channel_autoselect', enableWirelessAc && form.sz11aChannel.value == 0);
					// Channel bandwidth (5GHz)
					displayElement( 'basicACBW_tr', enableWirelessAc);
					displayElement( ['ac_bw_20'], wmodeac >= 2);
					displayElement( ['ac_bw_20_40'], wmodeac >= 8);
					displayElement( ['ac_bw_20_40_80'], wmodeac >= 14);
					displayElement( ['ac_bw_20_40_80_160', 'ac_bw_20_40_80_80'], wmodeac >= 14 && BUILD_WLAN_4T4R == "1");

					displayElement('autoselect_a', form.sz11aChannel.value == 0);
					displayElement('checktime_a',  form.sz11aChannel.value == 0);
					displayElement('basicRescanA', form.sz11aChannel.value == 0);

					// Hide 'SCAN' buttons if IE8 (no SVG support)
					displayElement( ['scanapLegendButtonScanINIC', 'scanapLegendButtonScanResultINIC'], browser.browser != 'ie' || browser.versionShort > 8);

					// Disable scan buttons if radio modules are disabled
					form.scanapLegendButtonScanINIC.disabled = NVRAM_RadioOnINIC != '1';

					// Lower channel bandwidth to the first available option
					while (form.ac_bw.selectedIndex > 0 && form.ac_bw.options[form.ac_bw.selectedIndex].style.display == "none")
					{
						form.ac_bw.selectedIndex--;
					}
				}

				// SSID Settings
				var updateVisibility_ssid = function() {
					displayElement('div_all', enableWirelessAny);

					//MBSSID Mode
					var have_mbssid = BUILD_MBSSID == '1' && enableWirelessAny;
					displayElement('basicMbssidModeT', have_mbssid && BUILD_5GHZ_SUPPORT);

					form.mbssid_mode.disabled = !have_mbssid;
					form.mbssidapisolated.disabled = !have_mbssid;

					if (!have_mbssid) {
						form.mbssid_mode.options.selectedIndex = 0;
					}

					var multiple_ssids_to_show = have_mbssid;

					if (form.mbssid_mode.value == "ra" && !enableWireless) multiple_ssids_to_show = false;
					else
					if (form.mbssid_mode.value == "rai" && !enableWirelessAc) multiple_ssids_to_show = false;
					else
					if (form.mbssid_mode.value == "all" && !enableWirelessAny) multiple_ssids_to_show = false;

					ssidDisplay(form, multiple_ssids_to_show);

					//SSID to SSID Isolation
					displayElement("div_mbssidapisolated", have_mbssid);

					// LAN to WLAN Isolation
					displayElement( 'div_LanWifiIsolate', NVRAM_OperationMode != '0' && NVRAM_ApCliBridgeOnly != '1' && form.mbssidapisolated.options.selectedIndex == 1);
					// Rate
					displayElement("div_abg_rate", (wmode < 5) && (BUILD_5GHZ_SUPPORT && wmodeac < 5));
					enableElements(form.abg_rate, (wmode) < 5 && (BUILD_5GHZ_SUPPORT && wmodeac < 5));
				}

				// Security
				var updateVisibility_security = function() {
					displayElement('div_security', enableWirelessAny);

					var wpa1_psk_mode = form.security_mode.value == 'WPAPSK';
					var wpa2_psk_mode = form.security_mode.value == 'WPA2PSK' || form.security_mode.value == 'WPAPSKWPA2PSK';
					var wpa_psk_mode =  wpa1_psk_mode || wpa2_psk_mode;

					var wpa1_ent_mode = form.security_mode.value == 'WPA';
					var wpa2_ent_mode = form.security_mode.value == 'WPA2' || form.security_mode.value == 'WPA1WPA2';
					var wpa_ent_mode =  wpa1_ent_mode || wpa2_ent_mode;

					// Security: WEP
					displayElement('div_wep', form.security_mode.value == 'WEPAUTO');

					// WPA
					displayElement(['div_wpa', 'div_wpa_algorithms','wpa_key_renewal_interval'], wpa_ent_mode || wpa_psk_mode);

					// WPA Enterprise
					displayElement('div_radius_server', wpa_ent_mode);
					displayElement('wpa_preAuthentication', form.security_mode.value == 'WPA2');

					// WPA Pre-shared
					displayElement('wpa_passphrase', wpa_psk_mode);
					displayElement('wpa_passphrase5', wpa_psk_mode && old_MBSSID == 0 && enableWirelessAc && form.mssid_1.value != form.mssidac_1.value);

					displayElement(['wpa_pmf_mode', 'wpa_pmf_sha256'], BUILD_WLAN_PMF && (wpa2_ent_mode || wpa2_psk_mode) && (form.cipher.selectedIndex != 0)); // if PMF built && (WPA2 + AES)


					//WPA Passphrase label (2.4/5 GHz)
					if (old_MBSSID == 0 && enableWirelessAc && form.mssid_1.value != form.mssidac_1.value)
						_TR("secureWPAPassPhrase", "secure wpa pass phrase nic");
					else
						_TR("secureWPAPassPhrase", "secure wpa pass phrase");

					// Save the minimization states of access policy submenus
					var submenus = document.getElementById("accessPolicyDiv").childNodes;
					var pol_min_list = [];
					for (var i=0;i<submenus.length;i++) {
						pol_min_list[submenus[i].id] = submenus[i].getAttribute("minimized");
					}

					createAccessPolicyTable();
					prepareAccessPolicy();

					// Restore previous minimization states
					submenus = document.getElementById("accessPolicyDiv").childNodes;
					for (var i=0;i<submenus.length;i++) {
						if (submenus[i].id in pol_min_list)
						{
							showMenu(submenus[i], 1-1*pol_min_list[submenus[i].id]);
						}
					}
				}

				// HT Physical Mode
				var updateVisibility_ht = function() {
					displayElement('div_11n', htmode);

					displayElement(['basicHTMCS_tr', 'basicHTGI_tr','basicHSTBC_tr', 'basicHTAMSDU_tr', 'basicHTAddBA_tr', 'basicHTOPMode_tr' ], 1);
					updateMCSOptions(form);

					displayElement('extension_channel', form.sz11gChannel.value != 0 && (1*wmode) >= 5 && form.n_bandwidth.value == 1 );

					displayElement('basicHTOPMode', htmode);
					displayElement("basicHTTxStream_tr", htmode);
					displayElement("basicHTRxStream_tr", htmode);

					displayElement("basicHTTx2Stream", NVRAM_TXPath != "1");
					displayElement("basicHtRx2Stream", NVRAM_RXPath != "1");

					form.n_mode.disabled = !htmode;
					form.n_gi.disabled = !htmode;
					form.n_stbc.disabled = !htmode;
					form.n_mcs.disabled = !htmode;
				}

				// VHT Physical Mode
				var updateVisibility_vht = function() {
					displayElement(['basicVHTBWSIGNAL_tr', 'basicVHTGI_tr'], 1);
					displayElement('basicVHTLDCP_tr', BUILD_LDPC_SUPPORT == '1');
					displayElement('basicVHTSTBC_tr', BUILD_VHT_STBC_SUPPORT == '1');

					displayElement("basicVHT", vhtmode)

					if (BUILD_5GHZ_1T1R == 1)
						hideElement("basicVHTLDCP_tr");

					form.ac_gi.disabled = !vhtmode;
					form.ac_stbc.disabled = !vhtmode;
					form.ac_ldpc.disabled = !vhtmode;
				}

				// Advanced wireless
				var updateVisibility_advanced = function() {
					displayElement( 'advWirelessT', enableWirelessAny);

					var AdvWirelessElement = [ 'advBGProtect_tr', 'advDTIM_tr', 'advFrag_tr', 'advRTS_tr', 'advStationKeepAlive_tr', 
								   'advPreambleType_tr', 'advShortSlot_tr', 'advTxBurst_tr', 'advPktAggr_tr', 'advWmm_tr', 'advAckPolicy_tr', 'advMcastRate_tr', 
								   'advStaRegion_tr' ];
					displayElement(AdvWirelessElement, enableWirelessAny);

					// Clear-Channel Assessment Monitor
					displayElement('advEDCCAEnable_tr', BUILD_EDCCA == "1");

					// IEEE 802.11h support
					displayElement('div_dot11h', (BUILD_DFS == '1') && enableWirelessAc);

					// 256QAM support
					displayElement('div_g256qam', (BUILD_WLAN_256QAM == '1') && enableWireless);

					// VOW
					displayElement('div_vow_airtime_fairness', (BUILD_WLAN_VOW == '1'));
				}

				// Basic roaming settings (Fast Roaming)
				var updateVisibility_roaming = function() {
					displayElement([ 'advMaxStaNum_tr', 'fastRoaming_tr', 'basicRRMEnable_tr', 'basicRRMclassINIC_tr', 'advIdleTimeout_tr', 'advEntryLifeCheck_tr', 'advBeaconInterval_tr', 'advBeaconIntervalINIC_tr'], 1);
					displayElement([ 'basicRRMEnable_tr' ], rrm_built);
					displayElement([ 'basicRRMclassINIC_tr' ], BUILD_5GHZ_SUPPORT && rrm_built && form.RRMEnable.value == '1' && form.country_code.value == 'RU');
					displayElement([ 'basicFtSupport_tr', 'basicKickStaRssiLowFT_tr'], ft_built);
					displayElement([ 'basicFtMdId_tr', 'basicFtR0khId_tr'], ft_built && form.FtSupport.value == "1" );
					displayElement('advBeaconIntervalINIC_tr', BUILD_5GHZ_SUPPORT)

					displayElement('div_roaming', enableWirelessAny);
					// Regulatory Class INIC
					displayElement('basicRRMclassINIC_tr', BUILD_5GHZ_SUPPORT && rrm_built && form.RRMEnable.value == '1' && form.country_code.value == 'RU');

					displayElement(["basicApProbeRspTimes_tr", "basicAuthRspFail_tr", "basicBandDeltaRssi_tr", "basicAuthRspRssi_tr", "basicAssocReqRssiThres_tr", "basicAssocRspIgnor_tr", "basicKickStaRssiLow_tr", "basicKickStaRssiLowPSM_tr", "basicKickStaRssiLowFT_tr", "basicKickStaRssiLowDelay_tr", "basicProbeRspRssi_tr", "tmpBlockAfterKick_tr"], form.FastRoaming.value == "1");
					displayElement("basicBandDeltaRssi_tr", form.FastRoaming.value == "1" && BUILD_5GHZ_SUPPORT);
					displayElement("basicKickStaRssiLowFT_tr", form.FastRoaming.value == "1" && ft_built);

					// WNM
					displayElement('div_WNMEnable', (BUILD_WLAN_WNM == '1'));

					// Smart Carrier Sense
					displayElement('advSCSEnable_tr', (BUILD_WLAN_SCS == '1'));
				}

				// Beamforming
				var updateVisibility_beamforming = function() {
					displayElement('div_txbf', BUILD_TXBF == '1' && enableWirelessAny);
					// Sending of sounding and beamforming
					displayElement('div_MUTxRxEnable', form.ETxBfEnCond.value == "1");
				}

				// Band steering settings
				var updateVisibility_bandsteering = function() {
					var have_steering = enableWireless && enableWirelessAc && BUILD_BANDSTEERING == 1;
					var steering_enabled = have_steering && (form.BandSteering.value == "1" || form.BandSteering.value == "2");

					displayElement('div_bandsteering', have_steering);
					displayElement(["row_BndStrgRssiDiff", "row_BndStrgRssiLow", "row_BndStrgAge", "row_BndStrgHoldTime", "row_BndStrgCheckTime" ], steering_enabled);

					if (!have_steering) {
						form.BandSteering.selectedIndex = 0;
					}
				}


				// Intrusion detection settings
				var updateVisibility_intrusiondetect = function() {
					displayElement('div_ids', ids_built && enableWirelessAny);
					displayElement(["row_AuthFloodThreshold", "row_AssocReqFloodThreshold", "row_ReassocReqFloodThreshold", "row_ProbeReqFloodThreshold", "row_DisassocFloodThreshold", "row_DeauthFloodThreshold", "row_EapReqFloodThreshold"], form.IdsEnable.value == "1" && ids_built);
				}

				// Multicast-to-Unicast Converter
				var updateVisibility_m2u = function() {
					// Multicast-to-Unicast
					displayElement('div_m2u', BUILD_WLAN_M2U == '1' && enableWirelessAny);
					// Video turbine
					displayElement('video_turbine_row', (BUILD_WLAN_M2U == '1') && (BUILD_VIDEO_TURBINE == '1'));
				}

				// PA/LNA control
				var updateVisibility_palna = function() {
					displayElement('advSynVGA_table', enableWirelessAny && (WLAN_FIRST_CARD == "7602" || WLAN_FIRST_CARD == "7612" || WLAN_SECOND_CARD == "7602" || WLAN_SECOND_CARD == "7612") );
					// Adaptive LNA Gain
					displayElement('advDynVGA_tr', enableWirelessAny);
					// Skip auto gain for long distance clients
					displayElement('advDynVGALong_tr', BUILD_CLAMP == '1');
					// Clamp maximum LNA GAIN in dynvga mode
					displayElement('advDynVGAClamp_tr', BUILD_CLAMP == '1' && BUILD_5GHZ_1T1R == '0');
				}

				updateVisibility_net24();
				updateVisibility_net5();
				updateVisibility_ssid();
				updateVisibility_security();
				updateVisibility_ht();
				updateVisibility_vht();
				updateVisibility_advanced();
				updateVisibility_roaming();
				updateVisibility_beamforming();
				updateVisibility_bandsteering();
				updateVisibility_intrusiondetect();
				updateVisibility_m2u();
				updateVisibility_palna();

// Extra
				// add subchannel
				GExtChannelDisplay(form);
				show_abg_rate(form);

				disableControlsByAuth();
			}

			function bandSteeringChange(form, userselect) {

				updateVisibility(form);

				if ((form.BandSteering.value == "1" || form.BandSteering.value == "2") && (userselect == 1)) {
					alert(_("basic bandsteering ssid"));
				}
			}

			/* Load autochannel options from nvram */
			function AutoChannelSelect(form) {
				var checktime_g			= +NVRAM_ACSCheckTime - 1;
				var checktime_a			= +NVRAM_ACSCheckTimeINIC - 1;

				form.autoselect_g.value = NVRAM_AutoChannelSelect;
				form.autoselect_a.value = NVRAM_AutoChannelSelectINIC;

				if (form.autoselect_g.selectedIndex == -1 || NVRAM_AutoChannelSelect == 0)
				{
					form.autoselect_g.selectedIndex = form.autoselect_g.options.length-1;
					while (form.autoselect_g.selectedIndex > 0 && form.autoselect_g.options[form.autoselect_g.selectedIndex].style.display == "none")
					{
						form.autoselect_g.selectedIndex--;
					}
				}

				if (form.autoselect_a.selectedIndex == -1 || NVRAM_AutoChannelSelectINIC == 0)
				{
					form.autoselect_a.selectedIndex = form.autoselect_a.options.length-1;
					while (form.autoselect_a.selectedIndex > 0 && form.autoselect_a.options[form.autoselect_a.selectedIndex].style.display == "none")
					{
						form.autoselect_a.selectedIndex--;
					}
				}

				form.checktime_g.options.selectedIndex = (checktime_g < 0) ? 23 : checktime_g;
				form.checktime_a.options.selectedIndex = (checktime_a < 0) ? 23 : checktime_a;
			}

			function GExtChannelDisplay(form) {
				var mode	= +form.wirelessmode.value;
				var channel	= +form.sz11gChannel.value;
				var bandwidth	= +form.n_bandwidth.value;
				var bandwidthinic = +(form.ac_bw.value[0]);

				displayElement('extension_channel', channel != 0 && bandwidth != 0 && mode >= 5);
				enableElements(form.n_extcha, channel != 0 && bandwidth != 0 && mode >= 5);

				// Hide 165 channel if BW > 20MHz
				var channelList = document.getElementById('sz11aChannel');
				for (var i = 0; i < channelList.options.length; i++) {
					if (channelList.options[i].value == '165' && bandwidthinic != 0) {
						displayElement(channelList.options[i], false)
						if (channelList.options.selectedIndex == i)
							channelList.options.selectedIndex = 0;
					}
					else
						displayElement(channelList.options[i], true);
				}

				regulatoryClassSet();
			}

			function regulatoryClassSet() {
				var rcINIC = (NVRAM_RegulatoryClassINIC == '0') ? '1;2;3;4;0'.split(';') : NVRAM_RegulatoryClassINIC.split(';');
				for (var i = 1; i <= 4; i++)
					document.getElementById('RCINIC_' + i).checked = false;

				for (var i = 1; i <= 5; i++)
					if (rcINIC[i - 1] != null)
						switch (rcINIC[i - 1]) {
							case '1':	document.getElementById('RCINIC_1').checked = true;		break;
							case '2':	document.getElementById('RCINIC_2').checked = true;		break;
							case '3':	document.getElementById('RCINIC_3').checked = true;		break;
							case '4':	document.getElementById('RCINIC_4').checked = true;		break;
						}

				for (var i = 1; i <= 4; i++)
					document.getElementById('RCINIC_' + i).disabled = false;

				var ch = document.getElementById('sz11aChannel').value;
				if (ch == 0) {
					for (var i = 1; i <= 4; i++) {
						document.getElementById('RCINIC_' + i).checked = true;
						document.getElementById('RCINIC_' + i).disabled = true;
					}
				} else if (ch >= 36 && ch <= 48) {
					document.getElementById('RCINIC_1').checked = true;
					document.getElementById('RCINIC_1').disabled = true;
				} else if (ch >= 52 && ch <= 64) {
					document.getElementById('RCINIC_2').checked = true;
					document.getElementById('RCINIC_2').disabled = true;
				} else if (ch >= 132 && ch <= 144) {
					document.getElementById('RCINIC_3').checked = true;
					document.getElementById('RCINIC_3').disabled = true;
				} else if (ch >= 149 && ch <= 165) {
					document.getElementById('RCINIC_4').checked = true;
					document.getElementById('RCINIC_4').disabled = true;
				}
			}

			function initChecktime(form) {
				for (var hour = 1; hour <= 24; hour++) {
					form.checktime_a.options[hour-1] = new Option(_("basic rescan every") + hour + _("basic hour"), hour);
					form.checktime_g.options[hour-1] = new Option(_("basic rescan every") + hour + _("basic hour"), hour);
				}
			}

			function ChannelOnChange(form, show_warn)
			{
				AutoChannelSelect(form);
				updateVisibility(form);

				if (show_warn && BUILD_5GHZ_SUPPORT && form.radioWirelessEnabledAc.value == "1" && form.sz11aChannel.value > 64)
					alert(_("basic 11a channel warning"));
			}

			function ssidDisplay(form, show)
			{
				var count = form.bssid_num.value * 1;
				if (show === undefined) show = true;

				for (var i=1; i < MBSSID_MAX; i++)
					displayElement('div_hssid' + i, show && i<count);

				// Allow only 4 BSSID's
				displayElement(form.addBSSIDbtn, true);
				form.addBSSIDbtn.disabled = (count >= bssid_num) || !show;
				mbssid_select_update(form);
			}

			function ssidAdd(form)
			{
				var count = form.bssid_num.value * 1;
				if (count < bssid_num)
				{
					showElement('div_hssid' + count);
					enableElements(form.addBSSIDbtn, ((++count) < bssid_num));
					form.bssid_num.value = count;
					form.elements['mssid_' + count].value = '';

					var i = count-1;

					form.hssid[i].checked = false;
					form.isolated_ssid[i].checked = false;

					form["AuthMode" + i].value = "WPA2PSK";
					form["EncrypType"+i].value = "AES";
					form["RekeyInterval"+i].value = "3600";
					form["passphrase"+i].value = "";
					form["PMFMode"+i].value = "0";
					form["PMFSHA256_"+i].value = "0";

					form["Key1Str"+i].value = "";
					form["Key2Str"+i].value = "";
					form["Key3Str"+i].value = "";
					form["Key4Str"+i].value = "";

					form["Key1Type"+i].value = "0";
					form["Key2Type"+i].value = "0";
					form["Key3Type"+i].value = "0";
					form["Key4Type"+i].value = "0";

					form["DefaultKeyID"+i].value = "0";

					form["PreAuth"+i].value = "0";

					form["RADIUS_Server"+i].value = "";
					form["RADIUS_Port"+i].value = "1812";
					form["RADIUS_Key"+i].value = "wive-ng-mt";
					form["RADIUS_SessionTimeout"+i].value = "0";


				}

				mbssid_select_update(form);
				form.ssidIndex.selectedIndex = count - 1;
				MBSSIDChange(true);
			}

			function ssidRemove(form, index)
			{
				var count = form.bssid_num.value * 1;
				if ((index < 1) || (index >= count))
					return;

				// move security selector from an element which will be deleted
				if (form.ssidIndex.selectedIndex == index)
				{
					form.ssidIndex.selectedIndex = index-1;
					MBSSIDChange(false);
				}

				if (index < (--count))
				{
					// Move values
					for (var i=index; i < count; i++)
					{
						form.elements['mssid_' + (i+1)].value = form.elements['mssid_' + (i+2)].value;
						form.hssid[i].checked = form.hssid[i+1].checked;
						form.isolated_ssid[i].checked = form.isolated_ssid[i+1].checked;
						form.mbcastisolated_ssid[i].checked = form.mbcastisolated_ssid[i+1].checked;

						form["AuthMode" + i].value = form["AuthMode" + (i+1)].value;
						form["EncrypType"+i].value = form["EncrypType"+(i+1)].value;
						form["RekeyInterval"+i].value = form["RekeyInterval"+(i+1)].value;
						form["passphrase"+i].value = form["passphrase"+(i+1)].value;
						form["PMFMode"+i].value = form["PMFMode"+(i+1)].value;
						form["PMFSHA256_"+i].value = form["PMFSHA256_"+(i+1)].value;

						form["Key1Str"+i].value = form["Key1Str"+(i+1)].value;
						form["Key2Str"+i].value = form["Key2Str"+(i+1)].value;
						form["Key3Str"+i].value = form["Key3Str"+(i+1)].value;
						form["Key4Str"+i].value = form["Key4Str"+(i+1)].value;

						form["Key1Type"+i].value = form["Key1Type"+(i+1)].value;
						form["Key2Type"+i].value = form["Key2Type"+(i+1)].value;
						form["Key3Type"+i].value = form["Key3Type"+(i+1)].value;
						form["Key4Type"+i].value = form["Key4Type"+(i+1)].value;

						form["DefaultKeyID"+i].value = form["DefaultKeyID"+(i+1)].value;
						form["PreAuth"+i].value = form["PreAuth"+(i+1)].value;

						form["RADIUS_Server"+i].value = form["RADIUS_Server"+(i+1)].value;
						form["RADIUS_Port"+i].value = form["RADIUS_Port"+(i+1)].value;
						form["RADIUS_Key"+i].value = form["RADIUS_Key"+(i+1)].value;
						form["RADIUS_SessionTimeout"+i].value = form["RADIUS_SessionTimeout"+(i+1)].value;

						document.getElementById('AccessControlList' + i).value = document.getElementById('AccessControlList' + (i+1)).value;
						document.getElementById('AccessPolicy' + i).value = document.getElementById('AccessPolicy' + (i+1)).value;
						if ((i+1) < network.data.length)
						{
							network.data[i].AccessPolicy = network.data[i+1].AccessPolicy;
							network.data[i].AccessControlList = network.data[i+1].AccessControlList;
							network.data[i+1].AccessControlList = "";
							network.data[i+1].AccessPolicy = "0";
						}
					}
				}

				// Clear values
				form.elements['mssid_' + (count+1)].value = '';
				form.hssid[count].checked = false;
				form.isolated_ssid[count].checked = false;
				form.mbcastisolated_ssid[count].checked = false;

				hideElement('div_hssid' + count);
				form.bssid_num.value = count;

				// Enable controls
				enableElements(form.addBSSIDbtn, (count < bssid_num));
				mbssid_select_update(form);
			}

			function calculate_ac_channel(countrycode, channel_indexac)
			{
				var res = 0;

				if (channel_indexac <= 0) return channel_indexac;
				countrycode = countrycode || "NONE";

				switch (countrycode) {
				case 'NONE':
				case 'RU':
				case 'US':
				case 'HK':
				case 'FR':
				case 'IE':
				case 'NO':
						if (channel_indexac <= 64)
						{
							res = channel_indexac;
							res = res / 4;
							if (res != 0)
								res = res - 8;
						} else if ((channel_indexac >= 132) && (channel_indexac <= 144))
						{
							res = channel_indexac;
							res = res / 4;
							if (res != 0)
								res = res - 24;
						}
						else if (channel_indexac >= 149)
						{
							res = channel_indexac - 1;
							res = res / 4;
							res = res - 24;
						}
					break;

				case 'JP':
						if (channel_indexac <= 48)
						{
							res = channel_indexac;
							res = res / 4;
							if (res != 0)
								res = res - 8;
						}
					break;

				case 'TW':
						if (channel_indexac <= 64)
						{
							res = channel_indexac;
							res = res / 4;
							if (res != 0)
								res = res - 12;
						}
						else if (channel_indexac >= 149)
						{
							res = channel_indexac - 1;
							res = res / 4;
							res = res - 32;
						}
					break;

				case 'CN':
						if (channel_indexac >= 149)
						{
							res = channel_indexac - 1;
							res = res / 4;
							res = res - 36;
						}
					break;
				}

				return res;
			}

			// Create access policy table
			function createAccessPolicyTable() {
				var html = '';
				for (var aptable = 0; aptable < MBSSID_MAX; aptable++) {
					html += '<table id="AccessPolicy_' + aptable + '_table" border="1" class="form showHideMenu">' +
							'	<thead onclick="switchShowMenu(this);">' +
							'		<tr>' +
							'			<td class="title" colspan="2">' + _('secure access policy') + '</td>' +
							'		</tr>' +
							'	</thead>' +
							'	<tbody>' + 
							'		<tr>' +
							'			<td bgcolor="#E8F8FF" class="head" >' + _('secure access policy capable') + '</td>' +
							'			<td>' +
							'				<select name="AccessPolicy' + aptable + '" id="AccessPolicy' + aptable + '" class="normal" onChange="accessPolicyClick(' + aptable + ');">' +
							'					<option value="0">' + _('wireless disable') + '</option>' +
							'					<option value="1">' + _('wireless allow') + '</option>' +
							'					<option value="2">' + _('wireless reject') + '</option>' +
							'				</select>' +
							'			</td>' +
							'		</tr>' +
							'		<tr  id="newap_text_' + aptable + '_tr">' +
							'			<td class="head">' + _('secure access policy new') + '</td>' +
							'			<td>' +
							'				<input class="normal" id="newap_text_' + aptable + '" maxlength="17">' +
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
				if (network === undefined || network.data === undefined) return;

				for (i = 0; i < network.data.length; i++) {
					j = 0;
					aplist	= [];
					var have_policy = network.data[i].AccessControlList.length != 0 && network.data[i].AccessPolicy != 0;
					if (have_policy) {
						aplist = network.data[i].AccessControlList.split(";");
						for (j = 0; j < aplist.length; j++) 
							document.getElementById("newap_" + i + "_" + j).innerHTML = aplist[j];
					}

					displayElement(['ap_' + i + '_td', 'ap_' + i + '_title', 'newap_text_' + i + '_tr'], have_policy);

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
				var mac = document.getElementById('newap_text_' + table).value.toUpperCase().split('-').join(':');
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


			function setExtraField(form, container, name, value) {
				if (form[name] === undefined) {
					var input = document.createElement("input");
					input.type = "hidden";
					input.name = name;
					input.value = value;
					container.appendChild(input);
					form[name] = input;
				}
				else
				{
					form[name].value = value;
				}
			}

			// change MBSSID
			function MBSSIDChange(show_warn) {
				var form = document.wireless_basic;

				form["AuthMode"+old_MBSSID].value = form.security_mode.value;

				// WEP
				form["Key1Str"+old_MBSSID].value = form.wep_key_1.value;
				form["Key2Str"+old_MBSSID].value = form.wep_key_2.value;
				form["Key3Str"+old_MBSSID].value = form.wep_key_3.value;
				form["Key4Str"+old_MBSSID].value = form.wep_key_4.value;

				form["Key1Type"+old_MBSSID].value = (form.WEP1Select.selectedIndex == 1)?'0':'1';
				form["Key2Type"+old_MBSSID].value = (form.WEP2Select.selectedIndex == 1)?'0':'1';
				form["Key3Type"+old_MBSSID].value = (form.WEP3Select.selectedIndex == 1)?'0':'1';
				form["Key4Type"+old_MBSSID].value = (form.WEP4Select.selectedIndex == 1)?'0':'1';

				form["DefaultKeyID"+old_MBSSID].value = form.wep_default_key.selectedIndex + 1;

				// WPA
				switch (form.cipher.selectedIndex)
				{
					case 0: form["EncrypType"+old_MBSSID].value = 'TKIP'; break;
					case 2: form["EncrypType"+old_MBSSID].value = 'TKIPAES'; break;
					default: form["EncrypType"+old_MBSSID].value = 'AES'; break;
				}

				form["passphrase"+old_MBSSID].value = form.passphrase.value;
				form["RekeyInterval"+old_MBSSID].value = form.keyRenewalInterval.value;
				form["PMFMode"+old_MBSSID].value = form.PMFMode.value;
				form["PMFSHA256_"+old_MBSSID].value = form.PMFSHA256.value;

				form["PreAuth"+old_MBSSID].value = (form.PreAuthentication.selectedIndex == 0)?'0':'1';

				form["RADIUS_Server"+old_MBSSID].value = form.RadiusServerIP.value;
				form["RADIUS_Port"+old_MBSSID].value = form.RadiusServerPort.value;
				form["RADIUS_Key"+old_MBSSID].value = form.RadiusServerSecret.value;
				form["RADIUS_SessionTimeout"+old_MBSSID].value = form.RadiusServerSessionTimeout.value;

				old_MBSSID	= form.ssidIndex.options.selectedIndex;
				LoadFields(form, old_MBSSID, show_warn);
				showAccessPolicy(old_MBSSID);		// update Access Policy

				WPAAlgorithms		= form["EncrypType"+old_MBSSID];
				PreAuthentication	= (form["PreAuth"+old_MBSSID] == '1')? true : false; 

//				document.getElementById('sp_title').innerHTML = _("secure security policy") + ' "' + network.data[old_MBSSID].SSID + '"';

				createAccessPolicyTable();
				prepareAccessPolicy();
				
				var policy_enabled = document.getElementById('AccessPolicy' + old_MBSSID).value;
				displayElement(['newap_text_' + old_MBSSID + '_tr', 'newap_macs_' + old_MBSSID + '_tr'], policy_enabled != 0);

				updateVisibility(form);

			}

			// WPA algorithm change
			function onWPAAlgorithmsClick() {
				if (document.getElementById('cipher').value != 1)
					setTimeout(function () { showWarningEncriptionAlgo(); }, 100);

				updateVisibility(document.wireless_basic);
			}

			// Pre-Authentication change
			function onPreAuthenticationClick() {
				if(document.getElementById('PreAuthentication').value == 0 && PreAuthentication == false) return;
				if(document.getElementById('PreAuthentication').value == 1 && PreAuthentication == true)  return;
			}

			// AccessPolicy change
			function accessPolicyClick(table) {
				network.data[table].AccessPolicy = document.getElementById('AccessPolicy' + table).value;
				displayElement(['newap_text_' + table + '_tr', 'newap_macs_' + table + '_tr'], document.getElementById('AccessPolicy' + table).value != 0);
				updateVisibility(document.wireless_basic);
			}

			// Show/Hide passphrase
			function showPassPhrase(id) {
				document.getElementById(id).type = (document.getElementById(id).type == 'password') ? 'text' : 'password';
			}


			// Load fields data
			function LoadFields(form, MBSSID, show_warn) {
				var i;
				var hide_ent = false;

				// Security Mode
				form.security_mode.options.length = 0;

				for (i = 0; i < MBSSID; i++) {
					if (form["AuthMode"+i].value.indexOf('PSK') != -1) {
						hide_ent = true;
						break;
					}
				}

				for (i = 0; i < security_modes_list.length; i++)
				{
					is_wep = security_modes_list[i][1] == "WEPAUTO";
					if (is_wep && form["AuthMode"+MBSSID].value != "WEP") continue;
					if (BUILD_8021X == '0' && security_modes_list[i][2] == '1') continue;
					if (hide_ent) {
						if (security_modes_list[i][1] == 'WPA') continue;
						if (security_modes_list[i][1] == 'WPA2') continue;
						if (security_modes_list[i][1] == 'WPA1WPA2') continue;
					}

					form.security_mode.options.add(new Option(security_modes_list[i][0], security_modes_list[i][1]));
				}

				form.security_mode.value = form["AuthMode"+MBSSID].value;

				// WEP
				form.wep_key_1.value = form["Key1Str"+MBSSID].value;
				form.wep_key_2.value = form["Key2Str"+MBSSID].value;
				form.wep_key_3.value = form["Key3Str"+MBSSID].value;
				form.wep_key_4.value = form["Key4Str"+MBSSID].value;

				form.WEP1Select.selectedIndex = (form["Key1Type"+MBSSID].value == '0' ? 1 : 0);
				form.WEP2Select.selectedIndex = (form["Key2Type"+MBSSID].value == '0' ? 1 : 0);
				form.WEP3Select.selectedIndex = (form["Key3Type"+MBSSID].value == '0' ? 1 : 0);
				form.WEP4Select.selectedIndex = (form["Key4Type"+MBSSID].value == '0' ? 1 : 0);

				form.wep_default_key.selectedIndex = 1*form["DefaultKeyID"+MBSSID].value - 1;
				if (form.wep_default_key.selectedIndex < 0)
					form.wep_default_key.selectedIndex = 0;

				// WPA
				if(form["EncrypType"+MBSSID].value == 'TKIP')
					form.cipher[0].selected = true;
				else if(form["EncrypType"+MBSSID].value == 'TKIPAES')
					form.cipher[2].selected = true;
				else
					form.cipher[1].selected = true;

				form.passphrase.value			= form["passphrase"+MBSSID].value;
				form.keyRenewalInterval.value		= form["RekeyInterval"+MBSSID].value;
				form.PMFMode.value			= form["PMFMode"+MBSSID].value;
				form.PMFSHA256.value			= form["PMFSHA256_"+MBSSID].value;

				if (form.PMFSHA256.selectedIndex < 0)
					form.PMFSHA256.selectedIndex = 0;

				if (form["PreAuth"+MBSSID].value == '0')
					form.PreAuthentication[0].selected = true;
				else
					form.PreAuthentication[1].selected = true;

				form.RadiusServerIP.value			= form["RADIUS_Server"+MBSSID].value;
				form.RadiusServerPort.value			= (form["RADIUS_Port"+MBSSID].value.length == 0) ? 1812 : form["RADIUS_Port"+MBSSID].value;
				form.RadiusServerSecret.value			= (form["RADIUS_Key"+MBSSID].value.length == 0) ? "wive-ng-mt" : form["RADIUS_Key"+MBSSID].value;
				form.RadiusServerSessionTimeout.value		= (form["RADIUS_SessionTimeout"+MBSSID].value.length == 0) ? 0 : form["RADIUS_SessionTimeout"+MBSSID].value;

				securityMode(show_warn);
				if (show_warn && form.cipher.value != 1)
					setTimeout(function () { showWarningEncriptionAlgo() }, 100);
			}


			// processing request data
			function securityHandler(responseText) {
				var i, j, aplist;
				var form = document.wireless_basic;

				network = JSON.parse(responseText);

				for (i = 0; i < bssid_num; i++) {
					if (network.data[i] === undefined)
					{
						network.data[i] = {
							SSID:"SSID "+i,
							AccessControlList:"",
							AccessPolicy:"",
							DefaultKeyID:"",
							AuthMode:"",
							EncrypType:"",
							Key1Str:"",
							Key1Type:"",
							Key2Str:"",
							Key2Type:"",
							Key3Str:"",
							Key3Type:"",
							Key4Str:"",
							Key4Type:"",
							PreAuth:"",
							RADIUS_Key:"",
							RADIUS_Port:"",
							RADIUS_Server:"",
							RekeyInterval:"",
							PMFMode:"",
							PMFSHA256:"0",
							RekeyMethod:"",
							WPAPSK:"",
							session_timeout_interval:""
						}
					}

					if ((network.data[i].AuthMode == "OPEN" && network.data[i].EncrypType == "NONE") || network.data[i].AuthMode.length == 0)
						network.data[i].AuthMode = "Disable";
					network.data[i].DefaultKeyID			= (network.data[i].DefaultKeyID.length == 0) 	? '1' 		: network.data[i].DefaultKeyID;
					network.data[i].EncrypType				= (network.data[i].EncrypType.length == 0) 		? 'NONE' 	: network.data[i].EncrypType;
					network.data[i].Key1Type				= (network.data[i].Key1Type.length == 0) 		? '0' 		: network.data[i].Key1Type;
					network.data[i].Key2Type				= (network.data[i].Key2Type.length == 0) 		? '0' 		: network.data[i].Key2Type;
					network.data[i].Key3Type				= (network.data[i].Key3Type.length == 0) 		? '0' 		: network.data[i].Key3Type;
					network.data[i].Key4Type				= (network.data[i].Key4Type.length == 0) 		? '0' 		: network.data[i].Key4Type;
					network.data[i].PreAuth					= (network.data[i].PreAuth.length == 0) 		? '0' 		: network.data[i].PreAuth;
					network.data[i].RekeyMethod				= (network.data[i].RekeyMethod.length == 0) 	? 'TIME'	: network.data[i].RekeyMethod;
				}

				form.ssidIndex.length = 0;
				var cnt = document.getElementById("security-extra-fields");

				for(i = 0; i < bssid_num; i++)
				{
					form.ssidIndex.options[i] = new Option("SSID "+i, i, false, false);

					setExtraField(form, cnt, "AuthMode"+i, network.data[i].AuthMode);
					setExtraField(form, cnt, "EncrypType"+i, network.data[i].EncrypType);
					setExtraField(form, cnt, "PreAuth"+i, network.data[i].PreAuth);
					setExtraField(form, cnt, "RekeyMethod"+i, network.data[i].RekeyMethod);
					setExtraField(form, cnt, "RekeyInterval"+i, network.data[i].RekeyInterval);
					setExtraField(form, cnt, "DefaultKeyID"+i, network.data[i].DefaultKeyID);
					setExtraField(form, cnt, "PMFMode"+i, network.data[i].PMFMode);
					setExtraField(form, cnt, "PMFSHA256_"+i, network.data[i].PMFSHA256);

					setExtraField(form, cnt, "Key1Type"+i, network.data[i].Key1Type);
					setExtraField(form, cnt, "Key2Type"+i, network.data[i].Key2Type);
					setExtraField(form, cnt, "Key3Type"+i, network.data[i].Key3Type);
					setExtraField(form, cnt, "Key4Type"+i, network.data[i].Key4Type);

					setExtraField(form, cnt, "Key1Str"+i, network.data[i].Key1Str);
					setExtraField(form, cnt, "Key2Str"+i, network.data[i].Key2Str);
					setExtraField(form, cnt, "Key3Str"+i, network.data[i].Key3Str);
					setExtraField(form, cnt, "Key4Str"+i, network.data[i].Key4Str);

					setExtraField(form, cnt, "RADIUS_Key"+i, network.data[i].RADIUS_Key);
					setExtraField(form, cnt, "RADIUS_Port"+i, network.data[i].RADIUS_Port);
					setExtraField(form, cnt, "RADIUS_Server"+i, network.data[i].RADIUS_Server);
					setExtraField(form, cnt, "RADIUS_SessionTimeout"+i, network.data[i].session_timeout_interval);

					setExtraField(form, cnt, "passphrase"+i, network.data[i].WPAPSK);
				}

				form.passphraseinic.value		= NVRAM_WPAPSK1INIC;

				mbssid_select_update(form)

				document.getElementById('ssidIndex').options.selectedIndex = network.default_mbssid;
				old_MBSSID = network.default_mbssid;

//				document.getElementById('sp_title').innerHTML = _("secure security policy") + ' "' + network.data[network.default_mbssid].SSID + '"';

				LoadFields(form, network.default_mbssid, true);
				prepareAccessPolicy();

				if (document.getElementById('AccessPolicy' + old_MBSSID).value == 0)
					showMenu('AccessPolicy_' + old_MBSSID + '_table', 0);
			}

			function securityMode(check) {
				var form = document.wireless_basic;
				updateVisibility(form);

				if (check && (form.security_mode.selectedIndex != -1 && form.security_mode.value != 'WPA2PSK' && form.security_mode.value != 'WPA2'))
					setTimeout(function () { showWarningEncriptionMode() }, 100);
			}

			function updateChannelList(form, show_14_channel) {
				var chan = 1*form.sz11gChannel.value;
				var extchan = 1*form.n_extcha.value;

				var region_limits = {
					0: [1,2,3,4,5,6,7,8,9,10,11],
					1: [1,2,3,4,5,6,7,8,9,10,11,12,13],
					2: [10,11],
					3: [10,11,12,13],
					4: [14],
					5: [1,2,3,4,5,6,7,8,9,10,11,12,13,14],
					6: [3,4,5,6,7,8,9],
					7: [5,6,7,8,9,10,11,12,13]
				};

				var region_id = 1*form.country_region.value;

				form.sz11gChannel.options.length = 1; // keep autoselect only

				for (var idx = 1; idx < 14; idx++) {
					var freq = 2412+5*(idx-1);
					var chan_name = freq+"MHz (Channel "+idx+")";

					if (region_limits[region_id].indexOf(idx) > -1)
						form.sz11gChannel.add(new Option(chan_name, idx));
				}

				if (show_14_channel)
					form.sz11gChannel.add(new Option("2484MHz (Channel 14)", 14));

				form.sz11gChannel.value = chan;
				if (form.sz11gChannel.selectedIndex == -1)
					form.sz11gChannel.value = 0;

				chan = 1*form.sz11gChannel.value;

				// Extension Channel
				if (form.wirelessmode.value >= 5)
				{
					form.n_extcha.options.length = 0;

					if (chan < 1)
						addOption(form.n_extcha, _("basic frequency auto"), 0);
					else
					{
						if ((chan-4-1) >= 0)
							addOption(form.n_extcha, ChannelList_24G[chan - 4 - 1], 0);
						if ((chan+4-1) < ChannelList_24G.length) {
							if ((chan+4) != 14 || show_14_channel)
								addOption(form.n_extcha, ChannelList_24G[chan + 4 - 1], 1);
						}
					}

					form.n_extcha.value = extchan;
					if (form.n_extcha.selectedIndex == -1)
						form.n_extcha.selectedIndex = 0;
				}
			}

			function updateMCSOptions(form) {
				var val = form.n_mcs.value;
				form.n_mcs.options.length = 0;
				form.n_mcs.options.add(new Option(_("wireless auto"), "33"));

				for (var i=0; i < Math.min(8*form.tx_stream.value, 32); i++)
				form.n_mcs.options.add(new Option(i, i));

				if (1*NVRAM_HT_BW > 0)
					form.n_mcs.options.add(new Option(32, 32));

				form.n_mcs.value = val;
				if (form.n_mcs.selectedIndex == -1)
					form.n_mcs.value = 33;
			}

			function initValue()
			{
				var form = document.wireless_basic;

				form.radioWirelessEnabled.options.selectedIndex = NVRAM_RadioOn;
				form.radioWirelessEnabledAc.options.selectedIndex = (BUILD_5GHZ_SUPPORT) ? NVRAM_RadioOnINIC : 0;

				form.bssid_num.value				= NVRAM_BssidNum;
				form.mssid_1.value				= NVRAM_SSID1;
				form.mssid_2.value				= NVRAM_SSID2;
				form.mssid_3.value				= NVRAM_SSID3;
				form.mssid_4.value				= NVRAM_SSID4;
				form.mssid_5.value				= NVRAM_SSID5;
				form.mssid_6.value				= NVRAM_SSID6;
				form.mssid_7.value				= NVRAM_SSID7;
				form.mssid_8.value				= NVRAM_SSID8;
				form.mssidac_1.value				= NVRAM_SSID1INIC;
				form.beacon.value				= NVRAM_BeaconPeriod;
				form.beaconINIC.value				= NVRAM_BeaconPeriodINIC;
				form.PMKCachePeriod.value			= NVRAM_PMKCachePeriod.split(";")[0];
				form.dtim.value					= NVRAM_DtimPeriod;
				form.fragment.value				= NVRAM_FragThreshold;
				form.rts.value					= NVRAM_RTSThreshold;
				form.EntryLifeCheck.value			= NVRAM_EntryLifeCheck;
				form.ApProbeRspTimes.value			= NVRAM_ApProbeRspTimes;
				form.BandDeltaRssi.value			= NVRAM_BandDeltaRssi;
				form.AuthRspFail.value				= NVRAM_AuthRspFail;
				form.AuthRspRssi.value				= NVRAM_AuthRspRssi;
				form.AssocReqRssiThres.value			= NVRAM_AssocReqRssiThres;
				form.AssocRspIgnor.value			= NVRAM_AssocRspIgnor;
				form.ProbeRspRssi.value				= NVRAM_ProbeRspRssi;
				form.KickStaRssiLow.value			= NVRAM_KickStaRssiLow;
				form.KickStaRssiLowPSM.value			= NVRAM_KickStaRssiLowPSM;
				form.KickStaRssiLowDelay.value			= NVRAM_KickStaRssiLowDelay;
				form.BndStrgRssiDiff.value			= NVRAM_BndStrgRssiDiff;
				form.BndStrgRssiLow.value			= NVRAM_BndStrgRssiLow;
				form.BndStrgAge.value				= NVRAM_BndStrgAge;
				form.BndStrgHoldTime.value			= NVRAM_BndStrgHoldTime;
				form.BndStrgCheckTime.value			= NVRAM_BndStrgCheckTime;
				form.AuthFloodThreshold.value			= NVRAM_AuthFloodThreshold;
				form.AssocReqFloodThreshold.value		= NVRAM_AssocReqFloodThreshold;
				form.ReassocReqFloodThreshold.value		= NVRAM_ReassocReqFloodThreshold;
				form.ProbeReqFloodThreshold.value		= NVRAM_ProbeReqFloodThreshold;
				form.DisassocFloodThreshold.value		= NVRAM_DisassocFloodThreshold;
				form.DeauthFloodThreshold.value			= NVRAM_DeauthFloodThreshold;
				form.EapReqFloodThreshold.value			= NVRAM_EapReqFloodThreshold;
				form.TmpBlockAfterKick.value			= NVRAM_TmpBlockAfterKick;
				form.KickStaRssiLowFT.value			= NVRAM_KickStaRssiLowFT;

				form.country_region.selectedIndex = -1;
				form.country_region.value			= NVRAM_CountryRegion;
				if (form.country_region.selectedIndex == -1)
					form.country_region.selectedIndex = 2;

				if (mbssid_mode == 'all')
					form.mbssid_mode.options.selectedIndex = 2;
				else if (mbssid_mode == 'rai')
					form.mbssid_mode.options.selectedIndex = 1;
				else
					form.mbssid_mode.options.selectedIndex = 0;

				// Set wmode after add all options
				form.wirelessmode.value     = NVRAM_WirelessMode.split(';')[0];
				form.wirelessmodeac.value   = NVRAM_WirelessModeINIC.split(';')[0];

				// Set-up TX power combo
				for (var i=0; i<form.tx_power.options.length; i++)
				{
					if (form.tx_power.options[i].value > (NVRAM_TxPower*1))
						break;
					form.tx_power.options.selectedIndex = i;
				}

				// Display AC
				if (BUILD_5GHZ_SUPPORT)
				{
					// Set-up TX power AC combo
					for (var i=0; i<form.tx_powerac.options.length; i++)
					{
						if (form.tx_powerac.options[i].value > (NVRAM_TxPowerINIC*1))
							break;
						form.tx_powerac.options.selectedIndex = i;
					}
				}

				// Initialize bssid
				var HiddenSSIDArray			= NVRAM_HideSSID.split(";");
				var APIsolatedArray			= NVRAM_NoForwarding.split(";");
				var NoForwardingMBCastArray		= NVRAM_NoForwardingMBCast.split(";");
				var dot11hArray				= NVRAM_IEEE80211H.split(";");

				for (i=0; i<bssid_num; i++) {
					form.hssid[i].checked			= (HiddenSSIDArray[i] == "1");
					form.isolated_ssid[i].checked		= (APIsolatedArray[i] == "1");
					form.mbcastisolated_ssid[i].checked	= (NoForwardingMBCastArray[i] == "1");
				}

				form.n_bandwidth.options.selectedIndex = 1*NVRAM_HT_BW;
				form.dot11h.options.selectedIndex = 1*dot11hArray[0];
				form.g256qam.options.selectedIndex = 1*(NVRAM_G_BAND_256QAM.split(";")[0]);
				form.vow_airtime_fairness.options.selectedIndex = 1*(NVRAM_VOW_Airtime_Fairness_En.split(";")[0]);
				form.WNMEnable.options.selectedIndex = 1*(NVRAM_WNMEnable.split(";")[0]);
				initChecktime(form);

				updateChannelList(form, true); // generate primary full channel list
				form.sz11gChannel.value = 1*WLAN_CHANNEL_INDEX;
				if (form.sz11gChannel.selectedIndex == -1)
					form.sz11gChannel.value = 0;

				updateChannelList(form, true); // regenerate list to correct extension channels
				form.n_extcha.value = 1*NVRAM_HT_EXTCHA;
				if (form.n_extcha.selectedIndex == -1)
					form.n_extcha.value = 0;

				if (BUILD_5GHZ_SUPPORT)
				{
					// Calculate current channel
					form.sz11aChannel.options.selectedIndex = calculate_ac_channel(NVRAM_CountryCode, 1*WLAN_CHANNEL_INDEX_AC);
				}

				if (BUILD_MBSSID == "1")
				{
					form.mbssidapisolated.options.selectedIndex = (mbssidapisolated == "1") ? 1 : 0;
				}

				// Security
				var html = '';
				for (var i = 0; i < MBSSID_MAX; i++)
					html += '<input type="hidden" name="AccessControlList' + i + '" id="AccessControlList' + i + '" value="">';
				document.getElementById('accessPolicyInput').innerHTML = html;

				createAccessPolicyTable();
				ajaxPostRequest("/goform/wirelessGetSecurity", "n/a", false, securityHandler);
				var br = getBrowser();
				if (br.browser == 'ie' && br.versionShort <= 8)
					displayElement([document.getElementById('passphrase_type'), document.getElementById('passphrase_inic_type'),
									'secureWEPKey1_show', 'secureWEPKey2_show', 'secureWEPKey3_show', 'secureWEPKey4_show',
									'secureRadiusSharedSecret_show'], false);

				// END Security

				form.n_mode.options.selectedIndex = (NVRAM_HT_OpMode == "1") ? 1 : 0;
				form.n_gi.options.selectedIndex = (NVRAM_HT_GI == "1") ? 1 : 0;
				form.n_stbc.options.selectedIndex = (NVRAM_HT_STBC == "1") ? 1 : 0;

				form.n_amsdu.options.selectedIndex = (NVRAM_HT_AMSDU ==  "0") ? 0 : 1;
				form.n_autoba.options.selectedIndex = (NVRAM_HT_AutoBA == "0") ? 0 : 1;

				if (BUILD_WLAN_4T4R == "1")
				{
					form.rx_stream.options[2] = new Option("3", "3");
					form.tx_stream.options[2] = new Option("3", "3");
					form.rx_stream.options[3] = new Option("4", "4");
					form.tx_stream.options[3] = new Option("4", "4");
				}

				if (NVRAM_TXPath == "1")
				{
					tx_stream_idx = 1;
				}

				if (NVRAM_RXPath == "1")
				{
					rx_stream_idx = 1;
				}

				form.rx_stream.options.selectedIndex = rx_stream_idx - 1;
				form.tx_stream.options.selectedIndex = tx_stream_idx - 1;

				updateMCSOptions(form);
				form.n_mcs.value = ht_mcs;

				form.ac_gi.options.selectedIndex = (NVRAM_VHT_SGI ==  "0") ? 0 : 1;
				form.ac_stbc.options.selectedIndex = (NVRAM_VHT_STBC ==  "0") ? 0 : 1;
				form.ac_ldpc.options.selectedIndex = (NVRAM_VHT_LDPC ==  "0") ? 0 : 1;

				form.ac_bw.value = (NVRAM_HT_BWINIC) +""+ (NVRAM_VHT_BW);

				form.ac_bwsig.options.selectedIndex = NVRAM_VHT_BW_SIGNAL;

				AutoChannelSelect(form);
				form.BandSteering.options.selectedIndex = NVRAM_BandSteering;
				form.FastRoaming.options.selectedIndex = NVRAM_FastRoaming;
				form.IdsEnable.options.selectedIndex = NVRAM_IdsEnable;

				if (BUILD_TXBF == '1') {
					form.ITxBfEn.options.selectedIndex = NVRAM_ITxBfEn;
					form.ETxBfEnCond.options.selectedIndex = NVRAM_ETxBfEnCond;
					form.MUTxRxEnable.options.selectedIndex = NVRAM_MUTxRxEnable;
				}

				form.RRMEnable.options.selectedIndex = 1*NVRAM_RRMEnable.split(";")[0];
				form.FtSupport.options.selectedIndex = 1*NVRAM_FtSupport.split(";")[0];
				form.FtMdId.value = NVRAM_FtMdId1;
				form.FtR0khId.value = NVRAM_FtR0khId1;

				form.LanWifiIsolate.options.selectedIndex = 1*NVRAM_LanWifiIsolate;

				form.bg_protection.options.selectedIndex = NVRAM_BGProtection;
				form.preamble_type.options.selectedIndex = NVRAM_TxPreamble;
				form.short_slot.options.selectedIndex = (NVRAM_ShortSlot == '1') ? 1 : 0;
				form.tx_burst.options.selectedIndex = (NVRAM_TxBurst == '1') ? 1 : 0;
				form.pkt_aggregate.options.selectedIndex = (NVRAM_PktAggregate == '1') ? 1 : 0;

				form.McastPhyMode.value = NVRAM_McastPhyMode;
				McastSwitch(form);

				//multicase to unicast converter
				form.m2u_enable.options.selectedIndex = (NVRAM_M2UEnabled == '1') ? 1 : 0;
				form.video_turbine.options.selectedIndex = (NVRAM_VideoTurbine == '1') ? 1 : 0;
				form.WmmCapable.options.selectedIndex = (NVRAM_WmmCapable.split(';')[0] == '1') ? 1 : 0;

				form.dyn_vga.options.selectedIndex = (NVRAM_DyncVgaEnable == '1') ? 1 : 0;

				form.advDynVGALong.options.selectedIndex = (NVRAM_SkipLongRangeVga == '1') ? 1 : 0;
				form.advDynVGAClamp.options.selectedIndex = 1*NVRAM_VgaClamp;

				form.maxstanum.value = inRange(maxstanum, 1, MAX_STA_NUM) ? maxstanum : 1*MAX_STA_NUM;
				form.keepalive.value = inRange(keepalive, 10, 300) ? keepalive : 60;
				form.idletimeout.value = inRange(NVRAM_IdleTimeout, 120, 600) ? NVRAM_IdleTimeout : 480;

				form.AckPolicy.options.selectedIndex = 1*NVRAM_AckPolicy.split(";")[0];

				form.EDCCAEnable.options.selectedIndex = 1*NVRAM_EDCCAEnable.split(";")[0];
				form.SCSEnable.options.selectedIndex = 1*NVRAM_SCSEnable;

				document.getElementById('tmpBlockAfterKick_td_2').title = _('adv tmpblockafterkick title');

				init_translation_model();
				showWarning();

				hideAllMenus();
				updateVisibility(form);
			}

			function show_abg_rate(form)
			{
				var wmode = form.wirelessmode.value;

				//ABG Rate
				if ((wmode == "0") || (wmode == "2") || (wmode == "4"))
				{
					var abg_rate = form.abg_rate;

					abg_rate.options.length = 0;
					abg_rate.options[0] = new Option(_("wireless auto"), "0");
					abg_rate.options[1] = new Option("1 Mbps", "1");
					abg_rate.options[2] = new Option("2 Mbps", "2");
					abg_rate.options[3] = new Option("5.5 Mbps", "5");
					abg_rate.options[4] = new Option("6 Mbps", "6");
					abg_rate.options[5] = new Option("9 Mbps", "9");
					abg_rate.options[6] = new Option("11 Mbps", "11");
					abg_rate.options[7] = new Option("12 Mbps", "12");
					abg_rate.options[8] = new Option("18 Mbps", "18");
					abg_rate.options[9] = new Option("24 Mbps", "24");
					abg_rate.options[10] = new Option("36 Mbps", "36");
					abg_rate.options[11] = new Option("48 Mbps", "48");
					abg_rate.options[12] = new Option("54 Mbps", "54");

					if (NVRAM_FixedTxMode == "CCK" || NVRAM_FixedTxMode == "cck")
					{
						switch (ht_mcs) {
							case 0: abg_rate.value = "1"; break;
							case 1: abg_rate.value = "2"; break;
							case 2: abg_rate.value = "5"; break;
							case 3: abg_rate.value = "11"; break;
							default: abg_rate.value = "0"; break;
						}
					}
					else
					{
						switch (ht_mcs) {
							case 0: abg_rate.value = "6"; break;
							case 1: abg_rate.value = "9"; break;
							case 2: abg_rate.value = "12"; break;
							case 3: abg_rate.value = "18"; break;
							case 4: abg_rate.value = "24"; break;
							case 5: abg_rate.value = "36"; break;
							case 6: abg_rate.value = "48"; break;
							case 7: abg_rate.value = "54"; break;
							default: abg_rate.value = "0"; break;
						}
					}
				}
				else if (wmode == "1")
				{
					var abg_rate = form.abg_rate;

					abg_rate.options.length = 0;
					abg_rate.options[0] = new Option(_("wireless auto"), "0");
					abg_rate.options[1] = new Option("1 Mbps", "1");
					abg_rate.options[2] = new Option("2 Mbps", "2");
					abg_rate.options[3] = new Option("5.5 Mbps", "5");
					abg_rate.options[4] = new Option("11 Mbps", "11");

					switch (ht_mcs) {
						case 0: abg_rate.value = "1"; break;
						case 1: abg_rate.value = "2"; break;
						case 2: abg_rate.value = "5"; break;
						case 3: abg_rate.value = "11"; break;
						default: abg_rate.value = "0"; break;
					}
				}
			}

			// Check values on form submit
			function checkValues_security(form) {
				if (document.getElementById('security_mode').selectedIndex == -1) {
					document.getElementById('security_mode').focus();
					alert(_("secure invalid mode"));
					return false;
				}

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

											if (!validateASCII(document.getElementById('passphrase').value, true, false)) {
												document.getElementById('passphrase').focus();
												document.getElementById('passphrase').select();
												alert(_("apcli chars not allowed"));
												return false;
											}

											if (BUILD_5GHZ_SUPPORT == '1' && form.radioWirelessEnabledAc.options.selectedIndex > 0)
												if (!validateASCII(document.getElementById('passphraseinic').value, true, false)) {
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

				if (form.ssidIndex.value == 0 && form.mssid_1.value == form.mssidac_1.value)
					document.getElementById("passphraseinic").value = document.getElementById("passphrase").value;

				return true;
			}

			// Check WEP keys
			function checkWEP() {

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
				var radius_secret = document.getElementById('RadiusServerSecret').value;
				if( !radius_secret.length || !checkInjection(radius_secret) || /[\\"]/.test(radius_secret) ) {
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

			// Check ASCII 10, 13 and ; , on string
			function checkInjection(str) {
				if (!validateASCII(str, true, false))
					return false;

				var re = /[;,]/;
				if (re.test(str))
					return false;
				else
					return true;
			}

			function checkValues(form)
			{

				if (form.mssidac_1.value == "" &&  form.radioWirelessEnabledAc.value == 1)
				{
					alert(_("adv no mssid_1"));
					form.mssidac_1.focus();
					form.mssidac_1.select();
					return false;
				}

				if (!validateASCII(form.mssidac_1.value, true, false))
				{
					alert(_("basic invalid ssid"));
					form.mssidac_1.focus();
					form.mssidac_1.select();
					return false;
				}

				// multiple ssids
				for (var i=1;i<=form.bssid_num.value;i++)
				{
					var elem = form.elements["mssid_"+i];
					// first SSID
					if (i == 1 && form.radioWirelessEnabled.value == 0)
						continue;
					// visible on page
					if (elem.offsetParent == null)
						continue;

					if (elem.value == "")
					{
						alert(_("adv no mssid_1"));
						elem.focus();
						elem.select();
						return false;
					}

					if (!validateASCII(elem.value, true, false))
					{
						alert(_("basic invalid ssid"));
						elem.focus();
						elem.select();
						return false;
					}
				}

				if (!inRange(form.beacon.value, 20, 999))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid beacon"));
					form.beacon.focus();
					form.beacon.select();
					return false;
				}

				if (!inRange(form.beaconINIC.value, 20, 999) && BUILD_5GHZ_SUPPORT)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid beacon"));
					form.beaconINIC.focus();
					form.beaconINIC.select();
					return false;
				}

				if (!inRange(form.PMKCachePeriod.value, 1, 9999))
				{
					showMenu('div_roaming', 1);
					alert(_("basic roaming invalid pmk cache period"));
					form.PMKCachePeriod.focus();
					form.PMKCachePeriod.select();
					return false;
				}

				if (!inRange(form.dtim.value, 1, 255))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid dtim"));
					form.dtim.focus();
					form.dtim.select();
					return false;
				}

				if (!inRange(form.fragment.value, 256, 2346))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid frag len"));
					form.fragment.focus();
					form.fragment.select();
					return false;
				}

				if (!inRange(form.rts.value, 1, 2347))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid rts"));
					form.rts.focus();
					form.rts.select();
					return false;
				}

				if (form.FtSupport.value == "1" && (form.FtMdId.value.length != 2 || !validateHex(form.FtMdId.value)))
				{
					showMenu('div_roaming', 1);
					alert(_("basic roaming ft invalid mobility domain"));
					form.FtMdId.focus();
					form.FtMdId.select();
					return false;
				}

				if (form.FtSupport.value == "1" && (form.FtR0khId.value.length != 8 || !validateHex(form.FtR0khId.value)))
				{
					showMenu('div_roaming', 1);
					alert(_("basic roaming ft invalid key holder"));
					form.FtR0khId.focus();
					form.FtR0khId.select();
					return false;
				}


				if (!inRange(form.maxstanum.value, 1, MAX_STA_NUM))
				{
					showMenu('div_roaming', 1);
					alert(_("adv invalid max sta num"));
					form.maxstanum.focus();
					form.maxstanum.select();
					return false;
				}

				if (!inRange(form.KickStaRssiLowFT.value, -100, 0))
				{
					showMenu('div_roaming', 1);
					if (form.FastRoaming.options.selectedIndex == 0) {
						form.FastRoaming.options.selectedIndex = 1;
						updateVisibility(document.wireless_basic);
					}
					alert(_("roaming invalid number"));
					form.KickStaRssiLowFT.focus();
					form.KickStaRssiLowFT.select();
					return false;
				}

				if (!inRange(form.TmpBlockAfterKick.value, 0, 200))
				{
					showMenu('div_roaming', 1);
					if (form.FastRoaming.options.selectedIndex == 0) {
						form.FastRoaming.options.selectedIndex = 1;
						updateVisibility(document.wireless_basic);
					}
					alert(_("adv invalid tmpblockafterkick"));
					form.TmpBlockAfterKick.focus();
					form.TmpBlockAfterKick.select();
					return false;
				}

				if (!inRange(form.keepalive.value, 10, 300))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid keepalive"));
					form.keepalive.focus();
					form.keepalive.select();
					return false;
				}

				if (!inRange(form.idletimeout.value, 120, 600))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid idletimeout"));
					form.idletimeout.focus();
					form.idletimeout.select();
					return false;
				}

				if (!inRange(form.EntryLifeCheck.value, 256, 4096))
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid entrylifecheck"));
					form.EntryLifeCheck.focus();
					form.EntryLifeCheck.select();
					return false;
				}

				if (form.BandSteering.value == 1) {
					if (!inRange(form.BndStrgRssiDiff.value, 0, 40))
					{
						alert(_("roaming invalid BndStrgRssiDiff"));
						form.BndStrgRssiDiff.focus();
						form.BndStrgRssiDiff.select();
						return false;
					}

					if (!inRange(form.BndStrgRssiLow.value, -100, 0))
					{
						alert(_("roaming invalid BndStrgRssiLow"));
						form.BndStrgRssiLow.focus();
						form.BndStrgRssiLow.select();
						return false;
					}

					if (!inRange(form.BndStrgAge.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.BndStrgAge.focus();
						form.BndStrgAge.select();
						return false;
					}

					if (!inRange(form.BndStrgHoldTime.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.BndStrgHoldTime.focus();
						form.BndStrgHoldTime.select();
						return false;
					}

					if (!inRange(form.BndStrgCheckTime.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.BndStrgCheckTime.focus();
						form.BndStrgCheckTime.select();
						return false;
					}
				}

				if (form.IdsEnable.value == 1) {
					if (!inRange(form.AuthFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.AuthFloodThreshold.focus();
						form.AuthFloodThreshold.select();
						return false;
					}

					if (!inRange(form.AssocReqFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.AssocReqFloodThreshold.focus();
						form.AssocReqFloodThreshold.select();
						return false;
					}

					if (!inRange(form.ReassocReqFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.ReassocReqFloodThreshold.focus();
						form.ReassocReqFloodThreshold.select();
						return false;
					}

					if (!inRange(form.ProbeReqFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.ProbeReqFloodThreshold.focus();
						form.ProbeReqFloodThreshold.select();
						return false;
					}

					if (!inRange(form.DisassocFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.DisassocFloodThreshold.focus();
						form.DisassocFloodThreshold.select();
						return false;
					}

					if (!inRange(form.DeauthFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.DeauthFloodThreshold.focus();
						form.DeauthFloodThreshold.select();
						return false;
					}

					if (!inRange(form.EapReqFloodThreshold.value, 0, null))
					{
						alert(_("roaming invalid number"));
						form.EapReqFloodThreshold.focus();
						form.EapReqFloodThreshold.select();
						return false;
					}
				}

				form.mbssid_mode.disabled = false;

				// concatenate ssid values before send
				form.isolated_ssids.value = "";
				form.mbcastisolated_ssids.value = "";
				form.hidden_ssids.value = "";

				for (var i = 0; i < form.bssid_num.value && i < form.isolated_ssid.length; i++) {
					var sep = (i>0)?",":"";

					if (form.isolated_ssid[i].checked)
						form.isolated_ssids.value += sep + form.isolated_ssid[i].value;

					if (form.mbcastisolated_ssid[i].checked)
						form.mbcastisolated_ssids.value += sep + form.mbcastisolated_ssid[i].value;

					if (form.hssid[i].checked)
						form.hidden_ssids.value += sep + form.hssid[i].value;
				}

				if (form.country_code.value == 'RU') {
					var rcINIC = [];
					form.RegulatoryClassINIC.value = '';
					for (var i = 1; i <= 4; i++) 
						if (document.getElementById('RCINIC_' + i).checked == true)
							rcINIC.push(i)

					for (var i = 0; i <= 4; i++) {
						form.RegulatoryClassINIC.value += (rcINIC[i] != null) ? rcINIC[i] : '0';
						form.RegulatoryClassINIC.value += (i < 4) ? ';' : '';
					}
				}
				else
					form.RegulatoryClassINIC.value = "0";

				MBSSIDChange(false);

				for (var i=0;i<form.bssid_num.value;i++)
				{
					form.ssidIndex.selectedIndex = i;
					MBSSIDChange(false);
					if (!checkValues_security(form)) return false;
				}

				ajaxShowTimer(form, 'timerReloader', _('message apply'), (document.getElementById('sz11gChannel').value == 0 || document.getElementById('sz11aChannel').value == 0) ? 40 : 30);
				return true;
			}

			function McastSwitch(form) {
				form.McastMcs.options.length = 0;

				var max_mcs = (form.McastPhyMode.value == "2") ? 7 : 15;

				for(var i = 0; i < (max_mcs + 1); i++) {
					addOption(form.McastMcs, "MCS: " + i, i);
					if(i <= NVRAM_McastMcs)
					form.McastMcs.options.selectedIndex = i;
				}
			}
			

			function countryCodeChange() {
				if (document.wireless_basic.country_code.value == 'RU' && NVRAM_RegulatoryClassINIC == '0') {
					document.getElementById('RCINIC_1').checked = true;
					document.getElementById('RCINIC_2').checked = true;
					document.getElementById('RCINIC_3').checked = true;
					document.getElementById('RCINIC_4').checked = true;
				}
			}

			function wmodeChange(form) {
				form.n_bandwidth.selectedIndex = form.n_bandwidth.options.length-1;
				updateVisibility(form);
			}

			function wmodeChangeAC(form) {
				form.ac_bw.selectedIndex = form.ac_bw.options.length-1;
				updateVisibility(form);
			}

			function mbssid_select_update(form) {
			
				for (var i=0;i<form.ssidIndex.options.length;i++)
				{
					network.data[i].SSID = form["mssid_"+(i+1)].value;
					
					if (i == 0 && BUILD_5GHZ_SUPPORT && form.radioWirelessEnabledAc.options.selectedIndex > 0 && form.mssidac_1.value.length > 0 && network.data[i].SSID != form.mssidac_1.value)
					{
						form.ssidIndex.options[i].text = network.data[i].SSID + ' / ' + form.mssidac_1.value;
					}
					else
					{
						form.ssidIndex.options[i].text = network.data[i].SSID;
					}
					
					form.ssidIndex.options[i].style.display = (i<form.bssid_num.value)?"block":"none";
				}
			
				if (form.ssidIndex.selectedIndex >= form.bssid_num.value)
				{
					form.ssidIndex.selectedIndex = form.bssid_num.value-1;
					MBSSIDChange(false);
				}
			}

			// Show warning message
			function showWarning() {
				var warning_access_password		= NVRAM_Password == "Admin" && AUTH_ROLE == 2;
				var warning_wireless_security		= (NVRAM_RadioOn == "1" || (BUILD_5GHZ_SUPPORT && NVRAM_RadioOnINIC == "1")) && NVRAM_AuthMode == "OPEN";
				var warning_wireless_key		= (NVRAM_AuthMode.indexOf("PSK") != -1) && ((NVRAM_RadioOn == "1" && NVRAM_WPAPSK1 == "1234567890") || (BUILD_5GHZ_SUPPORT && NVRAM_RadioOnINIC == "1" && NVRAM_WPAPSK1INIC == "1234567890"));

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
				var form = document.wireless_basic
				if ((form.radioWirelessEnabled.value == "0") && (form.radioWirelessEnabledAc.value == "0"))
					return;

				var setPass = false;
				var br = getBrowser();
				var message = (br.browser == 'firefox') ? _('secure lowsecure mode').replace(/_/g, '\r\n') : _('secure lowsecure mode').replace(/_/g, '\r');
				if (confirm(message)) {
					if (network.data[old_MBSSID].AuthMode != 'WPAPSK' && network.data[old_MBSSID].AuthMode != 'WPAPSKWPA2PSK') {
						setPass = true;
						setTimeout(function () { alert(_('secure lowsecure mode apply')) }, 100);
					}

					form["AuthMode"+old_MBSSID].value = "WPA2PSK";
					form["EncrypType"+old_MBSSID].value = 'AES';

					LoadFields(form, old_MBSSID, false);
					if (setPass) {
						document.getElementById('passphrase').focus();
						document.getElementById('passphrase').select();
					}
				}
			}

			function showWarningEncriptionAlgo() {
				var br = getBrowser();
				var message = (br.browser == 'firefox') ? _('secure lowsecure algo').replace(/_/g, '\r\n') : _('secure lowsecure algo').replace(/_/g, '\r');
				if (confirm(message)) {
					document.getElementById('cipher').value = 1;
				}
			}


		</script>

		<style>
table.form td.head {
    width: 40%;
    text-align: left;
}

table.form td.head-narrow {
    width: 32%;
    white-space: normal;
}

table.form tr.ssid-row {
    text-align: center;
}

#timerReloader {
    width: 0;
    height: 0;
    border: 0px solid #fff;
}

.mssid-label {
    margin-left: 20px;
}

.mssid-button {
    width: 80px;
}
		</style>

	</head>

<body bgcolor="#FFFFFF" onLoad="initValue();">
<div id="warning"></div>
<table class="body">
	<tr>
		<td><h1 data-tr="basic title">SSID Settings </h1>
			<p data-tr="basic introduction"> Here you can configure the most basic settings of Wireless communication, such as Network Name (SSID) and Channel.
			These settings are sufficient to have a working Access Point. </p>
			<hr>
	<form method="POST" name="wireless_basic" action="/goform/wirelessBasic" onSubmit="return checkValues(this);">
	<input id="isolated_ssids" name="isolated_ssids" type="hidden" value="">
	<input id="hidden_ssids" name="hidden_ssids" type="hidden" value="">
	<input id="mbcastisolated_ssids" name="mbcastisolated_ssids" type="hidden" value="">
	<input type="hidden" name="bssid_num" value="">
	<iframe name="timerReloader" id="timerReloader" src=""></iframe>

	<table id="wireless_24" class="form">
		<col style="width: 225px" />
		<col style="width: 515px" />
		<thead>
			<tr>
				<td class="title head-narrow" data-tr="basic wireless network">Wireless Network</td>
				<td class="title" >
					<div id="wlan_header_data" style="margin-left: 2px; float: left; margin-top: 5px;">
						<span><b>BSSID:</b> <% getWlanCurrentMac(); %></span>
						<span style="margin-left: 15px;" ><b><span data-tr="station channel">Channel</span>:</b> <% getWlanChannel_ioctl(); %></span>
					</div>
					<select name="radioWirelessEnabled" class="half" style="float: right;" onChange="updateVisibility(this.form);">
						<option value="0" data-tr="button disable">Disabled</option>
						<option value="1" data-tr="button enable">Enabled</option>
					</select>
				</td>
			</tr>
		</thead>
		<tbody>
			<tr id="div_11g_basic">
				<td class="head head-narrow" data-tr="basic network mode">Network Mode</td>
				<td class="wordwrap"><select name="wirelessmode" id="wirelessmode" class="normal" onChange="wmodeChange(this.form);">
					<option value="0" data-tr="basic bg">BG</option>
					<option value="1" data-tr="basic b">B</option>
					<option value="4" data-tr="basic g">G</option>
					<option value="6" data-tr="basic n">N</option>
					<option value="7" data-tr="basic gn">GN</option>
					<option value="9" data-tr="basic bgn">BGN</option>
				</select>
				</td>
			</tr>
			<tr id="div_txpw">
				<td class="head head-narrow auth-readonly-user" data-tr="basic tx power">TX Power (2.4GHz)</td>
				<td><select name="tx_power" class="normal auth-disable-user">
					<option value="5">5%</option>
					<option value="10">10%</option>
					<option value="20">20%</option>
					<option value="40">40%</option>
					<option value="70">70%</option>
					<option value="100">100%</option>
				</select></td>
			</tr>
			<tr id="div_11g_channel">
				<td class="head head-narrow" data-tr="basic frequency">Channel (2.4GHz)</td>
				<td><select id="sz11gChannel" name="sz11gChannel" class="normal" onChange="ChannelOnChange(this.form, false);">
					<option value="0" data-tr="basic frequency auto">AutoSelect</option>
					</select>
					<input id="scanapLegendButtonScan" name="scanapLegendButtonScan" type="button" class="short" value="Scan" data-tr="scanap legend button scan" onClick="scanAp('2.4', 0, true);">
					<input id="scanapLegendButtonScanResult" name="scanapLegendButtonScanResult" type="button" class="half" value="Scan Results" data-tr="scanap legend button scan result" onClick="scanAp('2.4', 0, false);">
				</td>
			</tr>

			<tr id="div_11g_channel_autoselect">
				<td class="head head-narrow" data-tr="basic frequency autoselect">Channel Autoselect (2.4GHz)</td>
				<td><select name="autoselect_g" class="normal" id="autoselect_g">
					<option value="1" data-tr="basic select by sta">by STA count</option>
					<option value="2" data-tr="basic select by rssi">by RSSI</option>
					<option value="3" data-tr="basic select by qbss" id="autoselect_g_qbss">by QBSS</option>
					</select>&nbsp;&nbsp;<select name="checktime_g" id="checktime_g" class="normal">
				</select>
				</td>
			</tr>

			<tr id="scanAp" style="display: none;">
				<td colspan="2">
					<div style="height: 300px;">
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

			<tr id="basicChannelBW_tr">
				<td class="head" data-tr="basic ht channel bandwidth">Channel BandWidth (2.4GHz)</td>
				<td class="val"><select id="n_bandwidth" name="n_bandwidth" class="normal" onClick="GExtChannelDisplay(this.form);">
					<option value="0" id="n_bandwidth_20">20MHz</option>
					<option value="1" id="n_bandwidth_20_40" selected>20/40MHz</option>
				</select></td>
			</tr>

		</tbody>
	</table>

	<table id="wireless_5" class="form">
		<col style="width: 225px" />
		<col style="width: 515px" />
		<thead>
			<tr>
				<td class="title head-narrow" data-tr="basic wireless network ac">Wireless Network 5GHz</td>
				<td class="title">
					<div id="wlan_ac_header_data" style="margin-left: 2px; float: left; margin-top: 5px;">
						<span><b>BSSID:</b> <% getWlanCurrentMacAC(); %></span>
						<span style="margin-left: 15px;" ><b><span data-tr="station channel">Channel</span>:</b> <% getWlanChannelAC_ioctl(); %></span>
					</div>
					<select name="radioWirelessEnabledAc" class="half" style="float: right;" onChange="updateVisibility(this.form);">
						<option value="0" data-tr="button disable">Disabled</option>
						<option value="1" data-tr="button enable">Enabled</option>
					</select>
				</td>

			</tr>
		</thead>
		<tbody>
			<tr id="div_11a_basic">
				<td class="head head-narrow" data-tr="basic ac network mode">Network Mode (5GHz)</td>
				<td class="wordwrap"><select name="wirelessmodeac" id="wirelessmodeac" class="normal" onChange="wmodeChangeAC(this.form);">
					<option value="2" data-tr="basic a">A</option>
					<option value="8" data-tr="basic aan">A/AN</option>
					<option value="11" data-tr="basic an">AN</option>
					<option value="14" data-tr="basic anc">A/AN/AC</option>
					<option value="15" data-tr="basic anac">AN/AC</option>
				</select>
				</td>
			 </tr>
			<tr id="div_txpw_ac">
				<td class="head head-narrow auth-readonly-user" data-tr="basic tx power ac">TX Power (5GHz)</td>
				<td><select name="tx_powerac" class="normal auth-disable-user">
					<option value="5">5%</option>
					<option value="10">10%</option>
					<option value="20">20%</option>
					<option value="40">40%</option>
					<option value="70">70%</option>
					<option value="100">100%</option>
				</select></td>
			</tr>
			<tr id="div_11a_channel">
				<td class="head head-narrow" data-tr="basic frequency ac">Channel (5GHz)</td>
				<td><select id="sz11aChannel" name="sz11aChannel" class="normal" onChange="ChannelOnChange(this.form, true);">
					<option value="0" data-tr="basic frequency auto">AutoSelect</option>
					<% getWlan11aChannels(); %>
					</select>
					<input id="scanapLegendButtonScanINIC" name="scanapLegendButtonScanINIC" type="button" class="short" value="Scan" data-tr="scanap legend button scan" onClick="scanAp('5', 0, true);">
					<input id="scanapLegendButtonScanResultINIC" name="scanapLegendButtonScanResultINIC" type="button" class="half" value="Scan Results" data-tr="scanap legend button scan result" onClick="scanAp('5', 0, false);">
				</td>
			</tr>

			<tr id="div_11a_channel_autoselect">
				<td class="head head-narrow" data-tr="basic frequency autoselect ac">Channel Autoselect (5GHz)</td>
				<td><select name="autoselect_a" class="normal" id="autoselect_a">
					<option value="1" data-tr="basic select by sta">by STA count</option>
					<option value="2" data-tr="basic select by rssi">by RSSI</option>
					<option value="3" data-tr="basic select by qbss" id="autoselect_a_qbss">by QBSS</option>
					</select>&nbsp;&nbsp;<select name="checktime_a" class="normal" id="checktime_a">
				</select>
				</td>
			</tr>

			<tr id="scanApINIC" style="display: none;">
				<td colspan="2">
					<div style="height: 300px;">
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

			<tr id="basicACBW_tr" >
				<td class="head" data-tr="basic ht channel bandwidth ac">Channel BandWidth (5GHz)</td>
				<td class="val"><select name="ac_bw" class="normal">
					<option id="ac_bw_20" value="00">20MHz</option>
					<option id="ac_bw_20_40" value="10">20/40MHz</option>
					<option id="ac_bw_20_40_80" value="11">20/40/80MHz</option>
					<option id="ac_bw_20_40_80_160" value="12" selected>20/40/80/160MHz</option>
					<option id="ac_bw_20_40_80_80" value="13">20/40/80/80+80MHz</option>
				</select></td>
			</tr>
		</tbody>
	</table>

	<table id="div_all" class="form" style="display:none;">
		<col style="width: 225px" />
		<col style="width: 165px" />
		<col style="width: 80px" />
		<col style="width: 87px" />
		<col style="width: 87px" />
		<col style="width: 90px" />
		<tbody>
			<tr>
				<td class="title" colspan="6" data-tr="basic wireless settings">Basic Settings</td>
			</tr>
			<tr>
				<td class="title"></td>
				<td class="title" align="center" data-tr="basic network name">Network name</td>
				<td class="title" align="center" data-tr="basic hssid">Hidden</td>
				<td class="title wordwrap" align="center" data-tr="basic clients isolated">Clients Isolated</td>
				<td class="title wordwrap" align="center" data-tr="basic broadcast isolated">Broadcast Isolated</td>
				<td class="title" align="center" data-tr="basic action">Action</td>
			</tr>
			<tr id="div_11g_name" class="ssid-row">
				<td class="head head-narrow" data-tr="basic ssid">Network Name (2.4GHz)</td>
				<td><input class="normal" type="text" name="mssid_1" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="0"></td>
				<td><input type="checkbox" name="isolated_ssid" value="0"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="0"></td>
				<td><input type="button" onClick="ssidAdd(this.form);" class="mssid-button" name="addBSSIDbtn" value="Add BSSID" data-tr="button add"></input>
				</td>
			</tr>

			<tr id="div_hssid1" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>1</td>
				<td><input class="normal" name="mssid_2" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="1"></td>
				<td><input type="checkbox" name="isolated_ssid" value="1"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="1"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 1);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_hssid2" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>2</td>
				<td><input class="normal" type="text" name="mssid_3" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="2"></td>
				<td><input type="checkbox" name="isolated_ssid" value="2"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="2"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 2);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_hssid3" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>3</td>
				<td><input class="normal" type="text" name="mssid_4" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="3"></td>
				<td><input type="checkbox" name="isolated_ssid" value="3"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="3"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 3);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_hssid4" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>4</td>
				<td><input class="normal" type="text" name="mssid_5" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="4"></td>
				<td><input type="checkbox" name="isolated_ssid" value="4"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="4"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 4);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_hssid5" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>5</td>
				<td><input class="normal" type="text" name="mssid_6" maxlength="32" onchange="updateVisibility(this.form);"></input></td>
				<td><input type="checkbox" name="hssid" value="5"></td>
				<td><input type="checkbox" name="isolated_ssid" value="5"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="5"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 5);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_hssid6" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>6</td>
				<td><input class="normal" type="text" name="mssid_7" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="6"></td>
				<td><input type="checkbox" name="isolated_ssid" value="6"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="6"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 6);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_hssid7" class="ssid-row">
				<td class="head head-narrow"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>7</td>
				<td><input class="normal" type="text" name="mssid_8" maxlength="32" onchange="updateVisibility(this.form);"></td>
				<td><input type="checkbox" name="hssid" value="7"></td>
				<td><input type="checkbox" name="isolated_ssid" value="7"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="7"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 7);" class="mssid-button" value="Remove" data-tr="button remove"></input>
				</td>
			</tr>
			<tr id="div_11a_name">
				<td class="head head-narrow" data-tr="basic ac ssid">Network Name (5GHz)</td>
				<td colspan="5"><input class="normal" type="text" name="mssidac_1" maxlength="32" onchange="updateVisibility(this.form);"></td>
			</tr>
			<tr id="basicMbssidModeT">
				<td class="head head-narrow" data-tr="basic mbssid mode">MBSSID Mode</td>
				<td colspan="5"><select name="mbssid_mode" class="normal" onChange="updateVisibility(this.form);">
					<option value="ra" selected>2.4GHz</option>
					<option value="rai">5GHz</option>
					<option value="all" data-tr="basic mbssid mode all">All</option>
				</select></td> 
			</tr>		
			<tr id="div_mbssidapisolated">
				<td class="head head-narrow" data-tr="basic mbssidapisolated">SSID to SSID Isolation</td>
				<td colspan="5"><select name="mbssidapisolated" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select></td>
			</tr>
			<tr id="div_LanWifiIsolate">
				<td class="head head-narrow" data-tr="basic lan wifi isolate">LAN to WLAN Isolation</td>
				<td colspan="5"><select name="LanWifiIsolate" class="normal">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select></td>
			</tr>
			<tr id="div_abg_rate">
				<td class="head head-narrow" data-tr="basic rate">Rate</td>
				<td colspan="5"><select name="abg_rate" class="normal">
				</select></td>
			</tr>
		</tbody>
	</table>

<!-- Security -->
					<!-- ---------------------  MBSSID Selection  --------------------- -->
	<div id="div_security">
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="sp_title" data-tr="secure security policy">Security Policy</td>
						</tr>
						<tr>
							<td class="head" id="secureSSIDChoice" data-tr="secure ssid choice">SSID choice</td>
							<td><select name="ssidIndex" id="ssidIndex" class="mid" onChange="MBSSIDChange(true);"></select></td>
						</tr>
						<tr id="div_security_infra_mode" name="div_security_infra_mode">
							<td class="head" id="secureSecureMode" data-tr="secure security mode">Security Mode</td>
							<td>
								<select name="security_mode" id="security_mode" class="mid" onChange="securityMode(true);">
								</select>
							</td>
						</tr>
					</table>
					<!-- WEP -->
					<table id="div_wep" name="div_wep" class="form" style="display: none;">
						<tr>
							<td class="title" colspan="4" id="secureWEP" data-tr="secure wep">Wired Equivalent Privacy (WEP)</td>
						</tr>
						<tr>
							<td class="head" colspan="2" id="secureWEPDefaultKey" data-tr="secure wep default key">Default Key</td>
							<td colspan="2">
								<select name="wep_default_key" id="wep_default_key" class="half">
									<option value="1" id="secureWEPDefaultKey1" data-tr="secure wep default key1">Key 1</option>
									<option value="2" id="secureWEPDefaultKey2" data-tr="secure wep default key2">Key 2</option>
									<option value="3" id="secureWEPDefaultKey3" data-tr="secure wep default key3">Key 3</option>
									<option value="4" id="secureWEPDefaultKey4" data-tr="secure wep default key4">Key 4</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head1" rowspan="4" id="secureWEPKey" data-tr="secure wep key">WEP Keys</td>
							<td class="head2" id="secureWEPKey1" data-tr="secure wep key1">WEP Key 1 :</td>
							<td style="width: 300px">
								<input type="password" name="wep_key_1" id="wep_key_1" maxlength="26" value="">
								<span id="secureWEPKey1_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_1')"><span id="secureShowPass" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP1Select" class="half" name="WEP1Select">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head2" id="secureWEPKey2" data-tr="secure wep key2">WEP Key 2 : </td>
							<td>
								<input type="password" name="wep_key_2" id="wep_key_2" maxlength="26" value="">
								<span id="secureWEPKey2_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_2')"><span id="secureShowPass1" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP2Select" name="WEP2Select" class="half">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head2" id="secureWEPKey3" data-tr="secure wep key3">WEP Key 3 : </td>
							<td>
								<input type="password" name="wep_key_3" id="wep_key_3" maxlength="26" value="">
								<span id="secureWEPKey3_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_3')"><span id="secureShowPass2" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP3Select" name="WEP3Select" class="half">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
						<tr>
							<td class="head2" id="secureWEPKey4" data-tr="secure wep key4">WEP Key 4 : </td>
							<td>
								<input type="password" name="wep_key_4" id="wep_key_4" maxlength="26" value="">
								<span id="secureWEPKey4_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('wep_key_4')"><span id="secureShowPass3" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
							<td>
								<select id="WEP4Select" name="WEP4Select" class="half">
									<option value="1">ASCII</option>
									<option value="0">Hex</option>
								</select>
							</td>
						</tr>
					</table>
					<!-- WPA -->
					<table id="div_wpa" name="div_wpa" class="form" style="display: none;">
						<tr>
							<td class="title" colspan="2" id="secureWPA" data-tr="secure wpa">WPA</td>
						</tr>
						<tr id="div_wpa_algorithms" name="div_wpa_algorithms" style="display: none;">
							<td class="head" id="secureWPAAlgorithm" data-tr="secure wpa algorithm">WPA Algorithms</td>
							<td>
								<select name="cipher" id="cipher" class="normal" onChange="onWPAAlgorithmsClick()">
									<option value="0">TKIP</option>
									<option value="1">AES</option>
									<option value="2">TKIP/AES</option>
								</select>
							</td>
						</tr>
						<tr id="wpa_passphrase" name="wpa_passphrase" style="display: none;">
							<td class="head" id="secureWPAPassPhrase">Pass Phrase</td>
							<td>
								<input type="password" name="passphrase" id="passphrase" class="normal" maxlength="64" value="">
								<span id="passphrase_type" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('passphrase')"><span id="secureShowPass4" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
						</tr>
						<tr id="wpa_passphrase5" name="wpa_passphrase5" style="display: none;">
							<td class="head" id="secureWPAPassPhraseInic" data-tr="secure wpa pass phrase inic">Pass Phrase 5GHz</td>
							<td>
								<input type="password" name="passphraseinic" id="passphraseinic" class="normal" maxlength="64" value="">
								<span id="passphrase_inic_type" style="position: absolute; margin-top: 1px;"><input type="checkbox" onClick="showPassPhrase('passphraseinic')"><span id="secureShowPass5" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
						</tr>
						<tr id="wpa_key_renewal_interval" name="wpa_key_renewal_interval" style="display: none;">
							<td class="head" id="secureWPAKeyRenewInterval" data-tr="secure wpa key renew interval">Key Renewal Interval</td>
							<td><input name="keyRenewalInterval" id="keyRenewalInterval" class="normal" maxlength="6" value="3600">&nbsp;<span id="secureKeySeconds" data-tr="secure key seconds">seconds</span></td>
						</tr>
						<tr id="wpa_preAuthentication" name="wpa_preAuthentication" style="display: none;">
							<td class="head" id="secureWPAPreAuth" data-tr="secure wpa preauth">Pre-Authentication</td>
							<td>
								<select name="PreAuthentication" id="PreAuthentication" class="normal" onChange="onPreAuthenticationClick()">
									<option value="0" id="PreAuthenticationDisable" data-tr="button disable">Disable</option>
									<option value="1" id="PreAuthenticationEnable" data-tr="button enable">Enable</option>
								</select>
							</td>
						</tr>

						<tr id="wpa_pmf_mode" name="wpa_pmf_mode">
							<td class="head" data-tr="secure wpa pmf mode">Protection Management Frame Mode</td>
							<td>
								<select name="PMFMode" id="PMFMode" class="normal">
									<option value="0" data-tr="button disable">Disable</option>
									<option value="1" data-tr="secure wpa pmf capable">Capable</option>
									<option value="2" data-tr="secure wpa pmf required">Required</option>
								</select>
							</td>
						</tr>

						<tr id="wpa_pmf_sha256" name="wpa_pmf_sha256">
							<td class="head" data-tr="secure wpa pmf sha256">Support SHA256 for Protection Management Frame</td>
							<td>
								<select name="PMFSHA256" id="PMFSHA256" class="normal">
									<option value="0" data-tr="button disable">Disable</option>
									<option value="1" data-tr="button enable">Enable</option>
								</select>
							</td>
						</tr>


					</table>
					<table id="div_radius_server" name="div_radius_server" class="form" style="display: none;">
						<tr>
							<td class="title" colspan="2" id="secureRadius" data-tr="secure radius">Radius Server</td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusIPAddr" data-tr="secure radius ipaddr">IP Address</td>
							<td><input name="RadiusServerIP" id="RadiusServerIP" class="normal" maxlength="32" value=""></td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusPort" data-tr="secure radius port">Port</td>
							<td><input name="RadiusServerPort" id="RadiusServerPort" class="normal" maxlength="5" value=""></td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusSharedSecret" data-tr="secure radius shared secret">Shared Secret</td>
							<td>
								<input type="password" name="RadiusServerSecret" id="RadiusServerSecret" class="normal" maxlength="64" value="">
								<span id="secureRadiusSharedSecret_show" style="position: absolute; margin-top: 1px;"><input type="checkbox" onChange="showPassPhrase('RadiusServerSecret')"><span id="secureShowPass6" data-tr="secure wpa show pass phrase" style="position: absolute; margin-top: 2px;">(show)</span></span>
							</td>
						</tr>
						<tr>
							<td class="head" id="secureRadiusSessionTimeout" data-tr="secure radius session timeout">Session Timeout</td>
							<td><input name="RadiusServerSessionTimeout" id="RadiusServerSessionTimeout" class="normal" maxlength="4" value="0"></td>
						</tr>
					</table>
					<!--	AccessPolicy for mbssid -->
					<div id="accessPolicyDiv"></div>
					<div id="accessPolicyInput"></div>
	</div>

<!-- END Security -->

	<table id="div_11n" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td class="title" colspan="2" data-tr="basic ht phy mode">HT Physical Mode</td>
		</tr>
	</thead>
	<tbody>
		<tr id="extension_channel">
			<td class="head" data-tr="basic ht extension channel">Extension Channel</td>
			<td class="val"><select id="n_extcha" name="n_extcha" class="normal">
				<option value="1" selected>2412MHz (Channel 1)</option>
			</select></td>
		</tr>
		<tr id="basicHTTxStream_tr">
			<td class="head" data-tr="basic ht txstream">HT TxStream</td>
			<td class="val"><select name="tx_stream" class="normal" onChange="updateVisibility(this.form);">
				<option value="1">1</option>
				<option id="basicHTTx2Stream" value="2">2</option>
			</select></td>
		</tr>
		<tr id="basicHTRxStream_tr">
			<td class="head" data-tr="basic ht rxstream">HT RxStream</td>
			<td class="val"><select name="rx_stream" class="normal">
				<option value="1">1</option>
				<option value="2" id="div_HtRx2Stream">2</option>
			</select></td>
		</tr>
		<tr id="basicHTMCS_tr">
			<td class="head">MCS</td>
			<td class="val"><select name="n_mcs" class="normal">
				<option value = "0">0</option>
				<option value = "1">1</option>
				<option value = "2">2</option>
				<option value = "3">3</option>
				<option value = "4">4</option>
				<option value = "5">5</option>
				<option value = "6">6</option>
				<option value = "7">7</option>
				<option value = "8">8</option>
				<option value = "9">9</option>
				<option value = "10">10</option>
				<option value = "11">11</option>
				<option value = "12">12</option>
				<option value = "13">13</option>
				<option value = "14">14</option>
				<option value = "15">15</option>
				<option value = "32">32</option>
				<option value = "33" data-tr="wireless auto" selected >Auto</option>
			</select></td>
		</tr>
		<tr id="basicHTGI_tr">
			<td class="head" data-tr="basic ht guard interval">Guard Interval</td>
			<td class="val"><select name="n_gi" class="normal">
				<option value="0" data-tr="wireless long">Long</option>
				<option value="1" data-tr="wireless auto">Auto</option>
			</select></td>
		</tr>
		<tr id="basicHSTBC_tr">
			<td class="head wordwrap" data-tr="basic block coding">Space-Time Block Coding</td>
			<td class="val"><select name="n_stbc" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTAMSDU_tr">
			<td class="head" data-tr="basic ht amsdu">Aggregation MSDU</td>
			<td class="val"><select name="n_amsdu" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTAddBA_tr">
			<td class="head" data-tr="basic ht addba">Auto Block ACK</td>
			<td class="val"><select name="n_autoba" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTOPMode_tr" style="display: none;">
			<td class="head" data-tr="basic ht op mode">Greenfield Mode</td>
			<td class="val"><select name="n_mode" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>

	<table id="basicVHT" class="form showHideMenu auth-hide-user" style="display:none;">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td class="title" colspan="2" data-tr="basic vht phy mode">VHT Physical Mode</td>
		</tr>
	</thead>
	<tbody>
		<tr id="basicVHTBWSIGNAL_tr">
			<td class="head" data-tr="basic signal mode">BandWidth Signaling Mode</td>
			<td class="val"><select name="ac_bwsig" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="basic signal mode static">Static</option>
				<option value="2" data-tr="basic signal mode dynamic">Dynamic</option>
			</select></td>
		</tr>
		<tr id="basicVHTGI_tr">
			<td class="head" data-tr="basic ht guard interval">Guard Interval</td>
			<td class="val"><select name="ac_gi" class="normal">
				<option value="0" data-tr="wireless long">Long</option>
				<option value="1" data-tr="wireless auto">Auto</option>
			</select></td>
		</tr>
		<tr id="basicVHTSTBC_tr">
			<td class="head wordwrap" data-tr="basic block coding">Space-Time Block Coding</td>
			<td class="val"><select name="ac_stbc" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicVHTLDCP_tr">
			<td class="head" data-tr="basic ldpc">Low Disenty parity check</td>
			<td class="val"><select name="ac_ldpc" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>

	<table id="advWirelessT" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td class="title" colspan="2" data-tr="adv wireless">Advanced Wireless</td>
		</tr>
	</thead>
	<tbody>
		<tr id="advBGProtect_tr">
			<td class="head" data-tr="adv bgpro">BG Protection Mode</td>
			<td class="val"><select id="bg_protection" name="bg_protection" class="normal">
				<option value="0" selected data-tr="wireless auto">Auto</option>
				<option value="1" data-tr="wireless on">On</option>
				<option value="2" data-tr="wireless off">Off</option>
			</select></td>
		</tr>
		<tr id="advDTIM_tr">
			<td class="head" data-tr="adv dtim">Data Beacon Rate</td>
			<td class="val"><input type="text" name="dtim" class="normal" maxlength="3">
				<span class="range" data-tr="adv dtim range">(range 1 - 255)</span></td>
		</tr>
		<tr id="advFrag_tr">
			<td class="head" data-tr="adv fragment threshold">Fragment Threshold</td>
			<td class="val"><input type="text" name="fragment" class="normal" maxlength="4">
				<span class="range" data-tr="adv fragment threshold range">(range 256 - 2346)</span></td>
		</tr>
		<tr id="advRTS_tr">
			<td class="head" data-tr="adv rts threshold">RTS Threshold</td>
			<td class="val"><input type="text" name="rts" class="normal" maxlength="4">
				<span class="range" data-tr="adv rts threshold range">(range 1 - 2347)</span></td>
		</tr>
		<tr id="advStationKeepAlive_tr">
			<td class="head" data-tr="adv station keep alive">Station Keep-Alive</td>
			<td class="val"><input type="text" name="keepalive" class="normal" maxlength="3" value="">
				<span class="range" data-tr="adv station keep alive range"> (10 - 300)</span></td>
		</tr>
		<tr id="advPreambleType_tr">
			<td class="head" data-tr="adv preamble type">Preamble Type</td>
			<td class="val"><select name="preamble_type" class="normal">
				<option value="0" selected data-tr="adv preamble type long">Long</option>
				<option value="1" data-tr="adv preamble type short">Short</option>
				<option value="2" data-tr="adv preamble type auto">AUTO</option>
			</select></td>
		</tr>
		<tr id="advShortSlot_tr">
			<td class="head" data-tr="adv short slot">Short Slot</td>
			<td class="val"><select name="short_slot" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advTxBurst_tr">
			<td class="head" data-tr="adv tx burst">Tx Burst</td>
			<td class="val"><select name="tx_burst" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advPktAggr_tr">
			<td class="head" data-tr="adv pkt aggregate">Pkt_Aggregate</td>
			<td class="val"><select name="pkt_aggregate" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advWmm_tr">
			<td class="head" data-tr="adv wmm">WMM Capable</td>
			<td class="val"><select name="WmmCapable" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advAckPolicy_tr">
			<td class="head" data-tr="basic ack policy">ACK Policy</td>
			<td class="val"><select name="AckPolicy" class="normal">
				<option value="0" data-tr="basic ack policy normal">Normal ACK</option>
				<option value="1" data-tr="basic ack policy no">No ACK</option>
			</select></td>
		</tr>
		<tr id="div_dot11h">
			<td class="head" data-tr="adv dot11h">IEEE802.11H</td>
			<td colspan="5"><select name="dot11h" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="div_g256qam">
			<td class="head" data-tr="adv g256qam">256QAM</td>
			<td colspan="5"><select name="g256qam" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="div_vow_airtime_fairness">
			<td class="head" data-tr="adv vow airtime fairness">Airtime Fairness</td>
			<td colspan="5"><select name="vow_airtime_fairness" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advMcastRate_tr">
			<td class="head" data-tr="adv mcast rate">Multicast TX mode</td>
			<td class="val"><select name="McastPhyMode" class="normal" onClick="McastSwitch(this.form);">
				<option value="2" data-tr="adv ofdm">Legacy OFDM</option>
				<option value="3" data-tr="adv htmix">HTMIX</option>
				</select>&nbsp;&nbsp;&nbsp;
				<select name="McastMcs" class="normal"></select></td>
		</tr>
		<tr id="advEDCCAEnable_tr">
			<td class="head" data-tr="adv edcca enable">Clear-Channel Assessment Energy Detection</td>
			<td class="val"><select name="EDCCAEnable" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advSCSEnable_tr">
			<td class="head" data-tr="adv scs enable">Smart Carrier Sense</td>
			<td class="val"><select name="SCSEnable" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advStaRegion_tr">
			<td class="head" data-tr="staadv region">Region settings</td>
			<td class="val"><select id="country_region" name="country_region" class="normal" onChange="updateVisibility(this.form);">
				<option value="0">CH1-11 (FCC)</option>
				<option value="1">CH1-13 (IC)</option>
				<option value="5">CH1-14 (MKK)</option>
				</select>&nbsp;&nbsp;&nbsp;
				<select name="country_code" class="normal" onChange="countryCodeChange();"><% listCountryCodes(); %>
			</select></td>
		</tr>
	</tbody>
	</table>

	<table id="div_txbf" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td class="title" colspan=2  data-tr="basic tx beam title">TX Beamforming</td>
		</tr>
	</thead>
	<tbody>
		<tr id="div_ITxBfEn">
			<td class="head" data-tr="basic tx beam implicit">Implicit TX Beamforming</td>
			<td class="val"><select name="ITxBfEn" class="normal">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>

		<tr id="div_ETxBfEnCond">
			<td class="head" data-tr="basic tx beam explicit">Explicit TX Beamforming</td>
			<td class="val"><select name="ETxBfEnCond" class="normal" onChange="updateVisibility(this.form);">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>

		<tr id="div_MUTxRxEnable">
			<td class="head" data-tr="basic tx beam mu">Multiuser TX Beamforming</td>
			<td class="val"><select name="MUTxRxEnable" class="normal">
				<option value="0" data-tr="basic tx beam mu off">Off</option>
				<option value="1" data-tr="basic tx beam mu beamformer">Beamformer</option>
				<option value="2" data-tr="basic tx beam mu beamformee">Beamformee</option>
				<option value="3" data-tr="basic tx beam mu all">All</option>
			</select></td>
		</tr>
	</tbody>
	</table>

	<table id="div_roaming" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr id="fast_roaming_tr">
			<td class="title" colspan="2" data-tr="basic roaming">Fast roaming</td>
		</tr>
	</thead>
	<tbody>
		<tr id="fastRoaming_tr">
			<td class="head" data-tr="basic roaming hangover">Handover settings</td>
			<td class="val"><select name="FastRoaming" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select>
			</td>
		</tr>
		<tr id="basicApProbeRspTimes_tr">
			<td class="head" data-tr="basic roaming probe times">Limit probe response to client</td>
			<td class="val"><input type="text" name="ApProbeRspTimes" class="normal" maxlength="4">
						<span class="range" data-tr="basic roaming probe times range"> 0 - 10 times, default 3</span></td>
		</tr>
		<tr id="basicBandDeltaRssi_tr">
			<td class="head" data-tr="basic roaming delta rssi">Delta threshold rssi level for 5GHz band</td>
			<td class="val"><input type="text" name="BandDeltaRssi" class="normal" maxlength="4">
						<span class="range" data-tr="basic roaming delta rssi range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicAuthRspFail_tr">
			<td class="head" data-tr="basic roaming auth fail">Reject auth req due to weak signal</td>
			<td class="val"><input type="text" name="AuthRspFail" class="normal" maxlength="4">
						<span class="range" data-tr="basic roaming auth fail range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicAuthRspRssi_tr">
			<td class="head" data-tr="basic roaming auth rssi">Ignore auth req due to weak signal</td>
			<td class="val"><input type="text" name="AuthRspRssi" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming auth rssi range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicAssocReqRssiThres_tr">
			<td class="head" data-tr="basic roaming rssi thres">Reject assoc req due to weak signal</td>
			<td class="val"><input type="text" name="AssocReqRssiThres" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming rssi thres range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicAssocRspIgnor_tr">
			<td class="head" data-tr="basic roaming assoc ignor">Ignore assoc req due to weak signal</td>
			<td class="val"><input type="text" name="AssocRspIgnor" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming assoc ignor range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicProbeRspRssi_tr">
			<td class="head" data-tr="basic roaming probe rssi">Auto disonnect sta if rssi low at probe requests</td>
			<td class="val"><input type="text" name="ProbeRspRssi" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming probe rssi range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicKickStaRssiLow_tr">
			<td class="head" data-tr="basic roaming rssi low">Auto disonnect sta if rssi low (active clients)</td>
			<td class="val"><input type="text" name="KickStaRssiLow" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming rssi low range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicKickStaRssiLowPSM_tr">
			<td class="head" data-tr="basic roaming rssi low psm">Auto disonnect sta if rssi low (powersave clients)</td>
			<td class="val"><input type="text" name="KickStaRssiLowPSM" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming rssi low psm range"> 0 - -100 dBm, default 0 (off)</span></td>
		</tr>
		<tr id="basicKickStaRssiLowFT_tr">
			<td class="head" data-tr="adv kickstarssilowft">Auto disonnect STA if rssi low (FT clients)</td>
			<td class="val">
				<input type="text" name="KickStaRssiLowFT" class="normal" maxlength="4" value="">
				<span class="range" data-tr="adv kickstarssilowft range">0 - -100 dBm, default: 0 (off)</span>
			</td>
		</tr>
		<tr id="basicKickStaRssiLowDelay_tr">
			<td class="head" data-tr="basic roaming rssi low delay">How time rssi check before kick</td>
			<td class="val"><input type="text" name="KickStaRssiLowDelay" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming rssi low delay range"> 0 - 200 seconds, default 60 </span></td>
		</tr>
		<tr id="tmpBlockAfterKick_tr">
			<td class="head" data-tr="adv tmpblockafterkick">Temporary block probe/assoc req from kicked client</td>
			<td id="tmpBlockAfterKick_td_2" class="val">
				<input type="text" name="TmpBlockAfterKick" class="normal" maxlength="4" value="">
				<span class="range" data-tr="adv tmpblockafterkick range"> 0 - 200 times, default: 14</span>
			</td>
		</tr>
		<tr id="basicRRMEnable_tr">
			<td class="head" data-tr="basic roaming rrm">RRMEnable</td>
			<td class="val">
				<select name="RRMEnable" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select>
			</td>
		</tr>
		<tr id="basicRRMclassINIC_tr">
			<td class="head" data-tr="basic roaming rc inic">Regulatory Class INIC</td>
			<td class="val">
				<input type="checkbox" id="RCINIC_1" value="0">&nbsp;<span data-tr="basic roaming rc inic channel 1" style="position: absolute; margin-top: 3px;">36, 40, 44, 48 channels</span><br>
				<input type="checkbox" id="RCINIC_2" value="0">&nbsp;<span data-tr="basic roaming rc inic channel 2" style="position: absolute; margin-top: 3px;">52, 56, 60, 64 channels</span><br>
				<input type="checkbox" id="RCINIC_3" value="0">&nbsp;<span data-tr="basic roaming rc inic channel 3" style="position: absolute; margin-top: 3px;">132, 136, 140, 144 channels</span><br>
				<input type="checkbox" id="RCINIC_4" value="0">&nbsp;<span data-tr="basic roaming rc inic channel 4" style="position: absolute; margin-top: 3px;">149, 153, 157, 161, 165 channels</span>
				<input name="RegulatoryClassINIC" type="hidden" value="1;2;3;4;0">
			</td>
		</tr>
		<tr id="basicFtSupport_tr">
			<td class="head" data-tr="basic roaming ft">FtSupport</td>
			<td class="val"><select name="FtSupport" class="normal" onChange="updateVisibility(this.form);">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="div_WNMEnable">
			<td class="head" data-tr="roaming wnm enable">Wireless Network Management</td>
			<td colspan="5"><select name="WNMEnable" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>

		<tr id="basicFtMdId_tr">
			<td class="head" data-tr="basic roaming ft mobility domain id">Mobility Domain ID</td>
			<td class="val"><input type="text" name="FtMdId" class="normal" maxlength="2" value=""></td>
		</tr>

		<tr id="basicFtR0khId_tr">
			<td class="head" data-tr="basic roaming ft key holder id">APs Key Holder</td>
			<td class="val"><input type="text" name="FtR0khId" class="normal" maxlength="8" value=""></td>
		</tr>

		<tr id="advMaxStaNum_tr">
			<td class="head" data-tr="adv maximum stations number">Maximum clients per SSID</td>
			<td class="val"><input type="text" name="maxstanum" class="normal" maxlength="3" value="">
				<span class="range" data-tr="adv maximum stations range">num (1 - <% getMaxStaNum(); %>)</span></td>
		</tr>
		<tr id="advIdleTimeout_tr">
			<td class="head" data-tr="adv idletimeout">IdleTimeout</td>
			<td class="val"><input type="text" name="idletimeout" class="normal" maxlength="3" value="">
				<span class="range" data-tr="adv idletimeout range">sec (120 - 600)</span></td>
		</tr>
		<tr id="advEntryLifeCheck_tr">
			<td class="head" data-tr="adv entrylifecheck">EntryLifeCheck</td>
			<td class="val"><input type="text" name="EntryLifeCheck" class="normal" maxlength="4">
				<span class="range" data-tr="adv entrylifecheck range">times (256 - 4096)</span></td>
		</tr>
		<tr id="advBeaconInterval_tr">
			<td class="head" data-tr="adv beacon interval">Beacon Interval</td>
			<td class="val"><input type="text" name="beacon" class="normal" maxlength="3">
				<span class="range" data-tr="adv beacon interval range">ms (range 20 - 999)</span></td>
		</tr>
		<tr id="advBeaconIntervalINIC_tr">
			<td class="head" data-tr="adv beacon interval inic">Beacon Interval (5GHz)</td>
			<td class="val"><input type="text" name="beaconINIC" class="normal" maxlength="3">
				<span class="range" data-tr="adv beacon interval range">ms (range 20 - 999)</span></td>
		</tr>
		<tr id="advPMKCachePeriod_tr">
			<td class="head" data-tr="basic roaming pmk cache period">PMK Cache Period</td>
			<td class="val"><input type="text" name="PMKCachePeriod" class="normal" maxlength="4">
				<span class="range" data-tr="basic roaming pmk cache period range">minutes (range 1 - 9999)</span></td>
		</tr>
	</tbody>
	</table>

	<table id="div_bandsteering" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr id="row_BndStrg">
			<td class="title head" data-tr="basic bandsteering">Band steering</td>
			<td class="title" style="text-align:right">
				<select name="BandSteering" class="half" onChange="bandSteeringChange(this.form, 1);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
					<!-- <option value="2" data-tr="adv preamble type auto">Auto</option> -->
				</select>
			</td>
		</tr>
	</thead>
	<tbody>
		<tr id="row_BndStrgRssiDiff" style="display:none;">
			<td class="head wordwrap" data-tr="basic bandsteering rssidiff">Allow fallback to 2.4GHz if bands RSSI diff bigger</td>
			<td class="val"><input type="text" name="BndStrgRssiDiff" class="normal" maxlength="2">
				<span class="range" data-tr="basic bandsteering rssidiff default">0 - 40 db, default 30</span></td>
		</tr>
		<tr id="row_BndStrgRssiLow" style="display:none;">
			<td class="head wordwrap" data-tr="basic bandsteering rssilow">Force fallback to 2.4GHz if rssi smaller</td>
			<td class="val"><input type="text" name="BndStrgRssiLow" class="normal" maxlength="4">
				<span class="range" data-tr="basic bandsteering rssilow default">0 - -100 db, default -88</span></td>
		</tr>
		<tr id="row_BndStrgAge" style="display:none;">
			<td class="head" data-tr="basic bandsteering age">Inactive client entry age time</td>
			<td class="val"><input type="text" name="BndStrgAge" class="normal" maxlength="6">
				<span class="range" data-tr="basic bandsteering age default">ms, default 600000</span></td>
		</tr>
		<tr id="row_BndStrgHoldTime" style="display:none;">
			<td class="head" data-tr="basic bandsteering hold">Time for holding 2.4G connection</td>
			<td class="val"><input type="text" name="BndStrgHoldTime" class="normal" maxlength="4">
				<span class="range" data-tr="basic bandsteering hold default">ms, default 5000</span></td>
		</tr>
		<tr id="row_BndStrgCheckTime" style="display:none;">
			<td class="head wordwrap" data-tr="basic bandsteering check">Time for deciding if a client is 2.4G only</td>
			<td class="val"><input type="text" name="BndStrgCheckTime" class="normal" maxlength="4">
				<span class="range" data-tr="basic bandsteering check default">ms, default 7000</span></td>
		</tr>
	</tbody>
	</table>
	<table id="div_ids" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr>
			<td class="title head" data-tr="basic ids">Intrusion Detection settings</td>
			<td class="title" style="text-align:right">
				<select name="IdsEnable" class="half" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select>
			</td>
		</tr>
	</thead>
	<tbody>
		<tr id="row_AuthFloodThreshold">
			<td class="head" data-tr="basic ids auth">Authentication</td>
			<td class="val"><input type="text" name="AuthFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids auth default">default 64</span></td>
		</tr>
		<tr id="row_AssocReqFloodThreshold">
			<td class="head" data-tr="basic ids assoc">Association request</td>
			<td class="val"><input type="text" name="AssocReqFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids assoc default">default 64</span></td>
		</tr>
		<tr id="row_ReassocReqFloodThreshold">
			<td class="head wordwrap" data-tr="basic ids reassoc">Reassociation request</td>
			<td class="val"><input type="text" name="ReassocReqFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids reassoc default">default 64</span></td>
		</tr>
		<tr id="row_ProbeReqFloodThreshold">
			<td class="head" data-tr="basic ids probe">Probe request</td>
			<td class="val"><input type="text" name="ProbeReqFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids probe default">default 64</span></td>
		</tr>
		<tr id="row_DisassocFloodThreshold">
			<td class="head" data-tr="basic ids disassoc">Disassociation</td>
			<td class="val"><input type="text" name="DisassocFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids disassoc default">default 64</span></td>
		</tr>
		<tr id="row_DeauthFloodThreshold">
			<td class="head" data-tr="basic ids deauth">Deauthentication</td>
			<td class="val"><input type="text" name="DeauthFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids deauth default">default 64</span></td>
		</tr>
		<tr id="row_EapReqFloodThreshold">
			<td class="head" data-tr="basic ids eap">EAP request</td>
			<td class="val"><input type="text" name="EapReqFloodThreshold" class="normal" maxlength="4">
				<span class="range" data-tr="basic ids eap default">default 64</span></td>
		</tr>
	</tbody>
	</table>
	<table id="div_m2u" class="form auth-hide-user">
	<thead>
		<tr>
			<td class="title" colspan="2" data-tr="adv multicast2unicast converter">Multicast-to-Unicast Converter</td>
		</tr>
	</thead>
	<tbody>
		<tr>
			<td class="head">Multicast-to-Unicast</td>
			<td class="val"><select name="m2u_enable" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="video_turbine_row">
			<td class="head">Video turbine</td>
			<td class="val"><select name="video_turbine" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="advSynVGA_table" class="form auth-hide-user">
	<thead>
		<tr id="advDynVGATitle_tr">
			<td class="title" colspan="2" data-tr="adv dynvga title">PA/LNA control (WARNING!!! FOR EXPERTS ONLY!)</td>
		</tr>
	</thead>
	<tbody>
		<tr id="advDynVGA_tr">
			<td class="head" data-tr="adv dynvga mode">Adaptive LNA Gain</td>
			<td class="val"><select name="dyn_vga" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advDynVGALong_tr">
			<td class="head" data-tr="adv dynvga long">Skip auto gain for long distance clients</td>
			<td class="val"><select name="advDynVGALong" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advDynVGAClamp_tr">
			<td class="head" data-tr="adv dynvga clamp">Clamp maximum LNA GAIN in dynvga mode</td>
			<td class="val"><select name="advDynVGAClamp" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1">-4dB</option>
				<option value="2">-8dB</option>
				<option value="3">-12dB</option>
				<option value="4">-16dB</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table class="buttons">
		<tr align="center">
			<td>
				<input type="submit" class="normal" value="Apply" data-tr="button apply">
				<input type="button" class="normal" value="Cancel" data-tr="button cancel" onClick="window.location.reload();">
				<input type="hidden" name="submit-url" value="/wireless/main.asp">
			</td>
		</tr>
	</table>
        <span id="security-extra-fields"></span>
	</form>
	<div class="whitespace"></div></td>
	</tr>
</table>
</body>
</html>
