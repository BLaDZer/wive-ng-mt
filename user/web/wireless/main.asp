<!DOCTYPE html>
<html>
	<head>
		<title>Main Wireless Settings</title>
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
		<script src="/js/scanap.js"></script>
		<script>
			var radio_on			= NVRAM_RadioOn;
			var radio_on_ac			= NVRAM_RadioOnINIC;
			var wmode				= NVRAM_WirelessMode;
			var wmodeac				= NVRAM_WirelessModeINIC;
			var channel_index		= WLAN_CHANNEL_INDEX;
			var channel_indexac		= WLAN_CHANNEL_INDEX_AC;
			var txPower				= NVRAM_TxPower;
			var txPowerAC			= NVRAM_TxPowerINIC;
			var mbssidapisolated 	= NVRAM_NoForwardingBTNBSSID;
			var fxtxmode			= NVRAM_FixedTxMode;
			var countrycode			= NVRAM_CountryCode;
			var tx_stream_idx		= NVRAM_HT_TxStream;
			var rx_stream_idx		= NVRAM_HT_RxStream;
			var ht_mode				= NVRAM_HT_OpMode;
			var ht_gi				= NVRAM_HT_GI;
			var ht_stbc				= NVRAM_HT_STBC;
			var ht_mcs				= NVRAM_HT_MCS;
			var ht_htc				= NVRAM_HT_HTC;
			var ht_rdg				= NVRAM_HT_RDG;
			var ht_extcha			= NVRAM_HT_EXTCHA;
			var ht_amsdu			= NVRAM_HT_AMSDU;
			var ht_autoba			= NVRAM_HT_AutoBA;
			var ht_badecline		= NVRAM_HT_BADecline;
			var ht_bw				= NVRAM_HT_BW;
			var ht_bwinic			= NVRAM_HT_BWINIC;

			var vht_gi				= NVRAM_VHT_SGI;
			var vht_stbc			= NVRAM_VHT_STBC;
			var vht_ldpc			= NVRAM_VHT_LDPC;
			var vht_bw				= NVRAM_VHT_BW;
			var vht_bwsig			= NVRAM_VHT_BW_SIGNAL;

			var bandsteeringBuilt	= BUILD_BANDSTEERING;
			var mbssid				= BUILD_MBSSID;
			var mbssid_mode			= NVRAM_BssidIfName;

			var is3t3r				= BUILD_WLAN_3T3R;
			var is5gh_1t1r			= BUILD_5GHZ_1T1R;
			var platform			= PLATFORM;

			var ids_built			= BUILD_IDS_ENABLE == '1';
			var txbf_built			= BUILD_TXBF;

			var dfs_built			= BUILD_DFS == '1';
			var rrm_built			= BUILD_RRM == '1';
			var ft_built			= BUILD_FT == '1';

			var bssid_num			= BSSID_NUM;

			var bgProtection		= NVRAM_BGProtection;
			var preambleType		= NVRAM_TxPreamble;
			var shortSlot			= NVRAM_ShortSlot;
			var txBurst				= NVRAM_TxBurst;
			var pktAggregate		= NVRAM_PktAggregate;
			var m2uBuilt			= BUILD_WLAN_M2U;
			var m2uEnabled			= NVRAM_M2UEnabled;
			var McastPhyMode		= NVRAM_McastPhyMode;
			var mcastMcs			= NVRAM_McastMcs;
			var video_turbine_built	= BUILD_VIDEO_TURBINE;
			var video_turbine		= NVRAM_VideoTurbine;

			var clamp				= BUILD_CLAMP;
			var dyn_vga				= NVRAM_DyncVgaEnable;
			var advDynVGALong		= NVRAM_SkipLongRangeVga;
			var advDynVGAClamp		= NVRAM_VgaClamp;

			var maxstanum			= NVRAM_MaxStaNum.split(";")[0];
			var keepalive			= NVRAM_StationKeepAlive.split(";")[0];
			var idletimeout			= NVRAM_IdleTimeout;
			var ackpolicy			= NVRAM_AckPolicy;
			var wmmCapable			= NVRAM_WmmCapable;
			var EDCCABuilt			= BUILD_EDCCA;
			var ED_MODE				= NVRAM_ED_MODE;

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


			var channel_list = [ 'sz11aChannel', 'sz11gChannel' ];
			var channels = [];

			function updateVisibility(form) {

				var enableWirelessAc	= BUILD_5GHZ_SUPPORT && (form.radioWirelessEnabledAc.value == "1");
				var enableWireless	= form.radioWirelessEnabled.value == "1";
				var enableWirelessAny	= enableWireless || enableWirelessAc;

				var wmode = form.wirelessmode.value;
				var wmodeac = form.wirelessmodeac.value;

				var vhtmode = enableWirelessAc && (1*wmodeac) >= 14;
				var htmode = vhtmode || (enableWireless && (wmode*1) >= 5) || (enableWirelessAc && (1*wmodeac) >= 8);

				// Hide 'SCAN' buttons if IE8 (no SVG support)
				var browser = getBrowser();
				displayElement( ['scanapLegendButtonScan', 'scanapLegendButtonScanINIC'], browser.browser != 'ie' || browser.versionShort > 8);

				// Disable scan buttons if radio modules are disabled
				form.scanapLegendButtonScan.disabled = radio_on != '1';
				form.scanapLegendButtonScanINIC.disabled = radio_on_ac != '1';

				displayElement('div_all', enableWirelessAny);

// Wireless Network 2.4 GHz
				// Network name (2.4GHz)
				displayElement( 'div_11g_name', enableWirelessAny); // do not hide in 5GHz-only mode, it has some usable nested options like "hidden" or "clients isolated", and an "Add" button
				// Network mode (2.4GHz)
				displayElement( 'div_11g_basic', enableWireless);
				// TX Power (2.4GHz)
				displayElement( 'div_11g_channel', enableWireless);
				// Channel (2.4GHz)
				displayElement( 'div_txpw', enableWireless);
				form.sz11gChannel.disabled = false;

				if (enableWireless)
					document.getElementById('wlan_channel_span').innerHTML = '<b>' + _('station channel') +':</b> ' + '<% getWlanChannel_ioctl(); %>';
				else
					document.getElementById('wlan_channel_span').innerHTML = '';

				displayElement('autoselect_g', (form.sz11gChannel.options.selectedIndex == 0));
				displayElement('checktime_g', (form.sz11gChannel.options.selectedIndex == 0));
				displayElement('basicRescanG', (form.sz11gChannel.options.selectedIndex == 0));



// Wireless Network 5 GHz
				displayElement('wireless_5', BUILD_5GHZ_SUPPORT);
				// Enabled
				displayElement('basicWirelessEnabledAc', BUILD_5GHZ_SUPPORT);
				// Network name (5GHz)
				displayElement( 'div_11a_name', enableWirelessAc);
				// Network mode (5GHz)
				displayElement( 'div_11a_basic', enableWirelessAc);
				// TX Power (5GHz)
				displayElement( 'div_txpw_ac', enableWirelessAc);
				// Channel (5GHz)
				displayElement( 'div_11a_channel', enableWirelessAc);

				form.sz11aChannel.disabled = !BUILD_5GHZ_SUPPORT;

				if (enableWirelessAc)
					document.getElementById('wlan_ac_channel_span').innerHTML = '<b>' + _('station channel') +':</b> ' + '<% getWlanChannelAC_ioctl(); %>';
				else
					document.getElementById('wlan_ac_channel_span').innerHTML = '';

				displayElement('autoselect_a', (form.sz11aChannel.options.selectedIndex == 0));
				displayElement('checktime_a', (form.sz11aChannel.options.selectedIndex == 0));
				displayElement('basicRescanA', (form.sz11aChannel.options.selectedIndex == 0));

// Basic Settings

				//MBSSID Mode
				var have_mbssid = mbssid == '1' && ((enableWireless && BUILD_5GHZ_SUPPORT) || enableWirelessAc)
				displayElement('basicMbssidModeT', have_mbssid);

				form.mbssid_mode.disabled = !have_mbssid;
				form.mbssidapisolated.disabled = !have_mbssid;

				if (!have_mbssid) {
					form.mbssid_mode.options.selectedIndex = 0;
				}

/*
				// Automatically changes MBSSID mode on radio module disabling
				if (!enableWireless) {
					form.mbssid_mode.value = "rai";
				}
				if (!enableWirelessAc) {
					form.mbssid_mode.value = "ra";
				}
*/

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
				// IEEE 802.11h support
				displayElement('div_dot11h', dfs_built && enableWirelessAc);

// HT Physical Mode
				displayElement('div_11n', htmode);

				displayElement(['basicHTChannelBW_tr', 'extension_channel', 'basicHTMCS_tr', 'basicHTGI_tr','basicHSTBC_tr', 'basicHTAMSDU_tr', 'basicHTAddBA_tr', 'basicHTDelBA_tr', 'basicHTOPMode_tr', 'basicHTRDG_tr' ], 1);
				displayElement('extension_channel', document.getElementById('sz11gChannel').value != 0 && (1*wmode) >= 5 && document.getElementById('n_bandwidth').value == 1 );
				displayElement('basicHTChannelBWINIC_tr', BUILD_5GHZ_SUPPORT && form.radioWirelessEnabledAc.value == 1);

				displayElement('basicHTOPMode', htmode);
				displayElement("basicHTTxStream_tr", htmode);
				displayElement("basicHTRxStream_tr", htmode);

				displayElement("basicHTTx2Stream", NVRAM_TXPath != "1");
				displayElement("basicHtRx2Stream", NVRAM_RXPath != "1");

				form.n_mode.disabled = !htmode;
				form.n_bandwidth.disabled = !htmode;
				form.n_bandwidthinic.disabled = !(BUILD_5GHZ_SUPPORT && htmode);
				form.n_rdg.disabled = !htmode;
				form.n_gi.disabled = !htmode;
				form.n_stbc.disabled = !htmode;
				form.n_mcs.disabled = !htmode;

// VHT Physical Mode

				displayElement(['basicVHTBW_tr', 'basicVHTBWSIGNAL_tr', 'basicVHTGI_tr'], 1);
				displayElement('basicVHTLDCP_tr', BUILD_LDPC_SUPPORT == '1');
				displayElement('basicVHTSTBC_tr', BUILD_VHT_STBC_SUPPORT == '1');

				displayElement("basicVHT", vhtmode)

				if (is5gh_1t1r == 1)
					hideElement("basicVHTLDCP_tr");

				form.ac_gi.disabled = !vhtmode;
				form.ac_stbc.disabled = !vhtmode;
				form.ac_ldpc.disabled = !vhtmode;

// Advanced wireless
				displayElement( 'advWirelessT', enableWirelessAny);

				var AdvWirelessElement = [ 'advBGProtect_tr', 'advDTIM_tr', 'advFrag_tr', 'advRTS_tr', 'advStationKeepAlive_tr', 
							   'advPreambleType_tr', 'advShortSlot_tr', 'advTxBurst_tr', 'advPktAggr_tr', 'advWmm_tr', 'advAckPolicy_tr', 'advMcastRate_tr', 
							   'advStaRegion_tr' ];
				displayElement(AdvWirelessElement, enableWirelessAny);

				// Clear-Channel Assessment Monitor
				displayElement('advEDMODE_tr', EDCCABuilt == "1");

// Basic roaming settings (Fast Roaming)
				displayElement([ 'advMaxStaNum_tr', 'fastRoaming_tr', 'basicRRMEnable_tr', 'basicRRMclassINIC_tr', 'basicFtSupport_tr', 'advIdleTimeout_tr', 'advEntryLifeCheck_tr', 'advBeaconInterval_tr', 'advBeaconIntervalINIC_tr'], 1);
				displayElement([ 'basicRRMEnable_tr' ], rrm_built);
				displayElement([ 'basicRRMclassINIC_tr' ], BUILD_5GHZ_SUPPORT && rrm_built && document.wireless_basic.RRMEnable.value == '1' && document.wireless_basic.country_code.value == 'RU');
				displayElement([ basicFtSupport_tr, "basicKickStaRssiLowFT_tr"], ft_built);
				displayElement('advBeaconIntervalINIC_tr', BUILD_5GHZ_SUPPORT)

				displayElement('div_roaming', enableWirelessAny);
				// Regulatory Class INIC
				displayElement('basicRRMclassINIC_tr', BUILD_5GHZ_SUPPORT && rrm_built && document.wireless_basic.RRMEnable.value == '1' && document.wireless_basic.country_code.value == 'RU');

				displayElement(["basicApProbeRspTimes_tr", "basicAuthRspFail_tr", "basicBandDeltaRssi_tr", "basicAuthRspRssi_tr", "basicAssocReqRssiThres_tr", "basicAssocRspIgnor_tr", "basicKickStaRssiLow_tr", "basicKickStaRssiLowPSM_tr", "basicKickStaRssiLowFT_tr", "basicKickStaRssiLowDelay_tr", "basicProbeRspRssi_tr", "tmpBlockAfterKick_tr"], form.FastRoaming.value == "1");
				displayElement("basicBandDeltaRssi_tr", form.FastRoaming.value == "1" && BUILD_5GHZ_SUPPORT);
				displayElement("basicKickStaRssiLowFT_tr", form.FastRoaming.value == "1" && ft_built);

// Beamforming
				displayElement('div_txbf', txbf_built == '1' && enableWirelessAny);
				// BFee feature
				displayElement('div_ETxBfeeEn', form.ITxBfEn.value == "1");
				// Sending of sounding and beamforming
				displayElement('div_ETxBfEnCond', form.ITxBfEn.value == "1");


// Band steering settings
				var have_steering = enableWireless && enableWirelessAc && bandsteeringBuilt == 1;
				var steering_enabled = have_steering && (form.BandSteering.value == "1" || form.BandSteering.value == "2");

				displayElement('div_bandsteering', have_steering);
				displayElement(["row_BndStrgRssiDiff", "row_BndStrgRssiLow", "row_BndStrgAge", "row_BndStrgHoldTime", "row_BndStrgCheckTime" ], steering_enabled);

				if (!have_steering) {
					form.BandSteering.selectedIndex = 0;
				}


// Intrusion detection settings
				displayElement('div_ids', ids_built && enableWirelessAny);
				displayElement(["row_AuthFloodThreshold", "row_AssocReqFloodThreshold", "row_ReassocReqFloodThreshold", "row_ProbeReqFloodThreshold", "row_DisassocFloodThreshold", "row_DeauthFloodThreshold", "row_EapReqFloodThreshold"], form.IdsEnable.value == "1" && ids_built);

// Multicast-to-Unicast Converter
				// Multicast-to-Unicast
				displayElement('div_m2u', m2uBuilt == '1' && enableWirelessAny);
				// Video turbine
				displayElement('video_turbine_row', (m2uBuilt == '1') && (video_turbine_built == '1'));

// PA/LNA control
				displayElement('advSynVGA_table', enableWirelessAny);
				// Adaptive LNA Gain
				displayElement('advDynVGA_tr', enableWirelessAny);
				// Skip auto gain for long distance clients
				displayElement('advDynVGALong_tr', clamp == '1');
				// Clamp maximum LNA GAIN in dynvga mode
				displayElement('advDynVGAClamp_tr', clamp == '1' && is5gh_1t1r == '0');

// Extra
				// add subchannel
				insertExtChannelOption(form);
				GExtChannelDisplay(form);
				show14channel(!htmode);
				show_abg_rate(form);

				disableControlsByAuth();
			}

			function bandSteeringChange(form, userselect) {

				updateVisibility(form);

				if ((form.BandSteering.value == "1" || form.BandSteering.value == "2") && (userselect == 1)) {
					alert(_("basic bandsteering ssid"));
				}
			}

			function insertExtChannelOption(form)
			{
				var wmode = form.wirelessmode.value * 1;
				var length = form.sz11gChannel.options.length;
				var CurrentCh = form.sz11gChannel.value * 1;
				var x = form.n_extcha;

				if (wmode >= 5)
				{
					x.options.length = 0;

					if ((CurrentCh >= 1) && (CurrentCh <= 4)) {
						addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
					} else if ((CurrentCh >= 5) && (CurrentCh <= 7)) {
						addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
						addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
					} else if ((CurrentCh >= 8) && (CurrentCh <= 9)) {
						addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
						if (length >= 14)
							addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
					} else if (CurrentCh == 10) {
						addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
						if (length > 14)
							addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
					} else if (CurrentCh >= 11) {
						addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
					} else {
						addOption(x, _("basic frequency auto"), 0);
					}

					if (1*ht_extcha == 1)
						x.options.selectedIndex = (x.options.length > 1) ? 1 : 0;
					else
						x.options.selectedIndex = 0;
				}
			}

			/* Load autochannel options from nvram */
			function AutoChannelSelect(form) {
				var autoselectmode_g	= +NVRAM_AutoChannelSelect;
				var autoselectmode_a	= +NVRAM_AutoChannelSelectINIC;
				var checktime_g			= +NVRAM_ACSCheckTime - 1;
				var checktime_a			= +NVRAM_ACSCheckTimeINIC - 1;

				form.autoselect_g.options.selectedIndex = (autoselectmode_g == 1) ? 0 : 1;
				form.autoselect_a.options.selectedIndex = (autoselectmode_a == 1) ? 0 : 1;
				form.checktime_g.options.selectedIndex = (checktime_g < 0) ? 23 : checktime_g;
				form.checktime_a.options.selectedIndex = (checktime_a < 0) ? 23 : checktime_a;
			}

			function GExtChannelDisplay(form) {
				var mode	= +form.wirelessmode.value;
				var channel	= +form.sz11gChannel.value;
				var bandwidth	= +form.n_bandwidth.value;

				displayElement('extension_channel', channel != 0 && bandwidth != 0 && mode >= 5);
				enableElements(form.n_extcha, channel != 0 && bandwidth != 0 && mode >= 5);

				// Hide 165 channel if BW > 20MHz
				var channelList = document.getElementById('sz11aChannel');
				for (var i = 0; i < channelList.options.length; i++) {
					if (channelList.options[i].value == '165' && form.n_bandwidthinic.value == '1') {
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

			function ChannelOnChange(form)
			{
				AutoChannelSelect(form);
				updateVisibility(form);

				if (BUILD_5GHZ_SUPPORT && document.getElementById('sz11aChannel').value > 64)
					alert(_("basic 11a channel warning"));
			}

			function ssidDisplay(form, show)
			{
				var no_mbssid = mbssid != '1';
				var count = form.bssid_num.value * 1;
				if (show === undefined) show = true;

				for (var i=1; i < bssid_num; i++)
					displayElement('div_hssid' + i, show && i<count);

				// Allow only 4 BSSID's
				displayElement(form.addBSSIDbtn, !no_mbssid);
				form.addBSSIDbtn.disabled = ((count >= bssid_num) || (no_mbssid));
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
					form.hssid[count-1].checked = false;
					form.isolated_ssid[count-1].checked = false;
				}
			}

			function ssidRemove(form, index)
			{
				var count = form.bssid_num.value * 1;
				if ((index < 1) || (index >= count))
					return;
				if (index < (--count))
				{
					// Move values
					for (var i=index; i < count; i++)
					{
						form.elements['mssid_' + (i+1)].value = form.elements['mssid_' + (i+2)].value;
						form.hssid[i].checked = form.hssid[i+1].checked;
						form.isolated_ssid[i].checked = form.isolated_ssid[i+1].checked;
						form.mbcastisolated_ssid[i].checked = form.mbcastisolated_ssid[i+1].checked;
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
			}

			function initTranslation()
			{
				_TR("basicTitle", "basic title");
				_TR("basicIntroduction", "basic introduction");

				_TR("basicWireless", "basic wireless");
				_TR("basicWirelessAC", "basic wireless ac");
				_TR("basicWirelessNet", "basic wireless network");
				_TR("basicWirelessNetAc", "basic wireless network ac");
				_TR("basicWirelessSettings", "basic wireless settings");
				_TR("basicRadioButton", "basic radio button");
				_TR("basicAcNetMode", "basic ac network mode");
				_TR("basicNetMode", "basic network mode");
				_TR("basicSSID", "basic ssid");
				_TR("basicAcSSID", "basic ac ssid");
				_TR("basicHSSID", "basic hssid");
				_TR("basicNetworkName", "basic network name");
				_TR("basicBroadcastSSID", "basic broadcast ssid");
				_TR("basicIsolatedSSID", "basic isolated");
				_TR("basicIsolatedSSID1", "basic isolated");
				_TR("basicClientsSSID", "basic clients");
				_TR("basicMBSSIDApIsolated", "basic mbssidapisolated");
				_TR("basicLanWifiIsolate", "basic lan wifi isolate");
				_TR("basicMBSSIDMode", "basic mbssid mode");
				_TR("mbssid_mode_all", "basic mbssid mode all");

				_TR("basicFreqA", "basic frequency ac");
				_TR("basicFreqAAuto", "basic frequency auto");
				_TR("basicFreqG", "basic frequency");
				_TR("basicFreqGAuto", "basic frequency auto");
				_TR("basicRate", "basic rate");

				_TR("basicTxPW", "basic tx power");
				_TR("basicTxPWAc", "basic tx power ac");

				_TR("basicHTPhyMode", "basic ht phy mode");
				_TR("basicHTOPMode_td_1", "basic ht op mode");
				_TR("basicHTMixedDisable", "basic ht op mixed");
				_TR("basicHTMixedEnable", "basic ht op green");
				_TR("basicHTChannelBW_td_1", "basic ht channel bandwidth");
				_TR("basicHTChannelBWINIC_td_1", "basic ht channel bandwidth inic");
				_TR("basicHTSTBC", "basic stbc");
				_TR("basicHTGI", "basic ht guard interval");
				_TR("basicHTLongGI", "wireless long");
				_TR("basicHTAutoGI", "wireless auto");
				_TR("basicHTAutoMCS", "wireless auto");
				_TR("basicHTRDG_td_1", "basic ht rdg");
				_TR("basicHTExtChannel", "basic ht extension channel");
				_TR("basicHTAMSDU_td_1", "basic ht amsdu");
				_TR("basicHTAddBA_td_1", "basic ht addba");
				_TR("basicHTDelBA_td_1", "basic ht delba");
				_TR("basicHSTBC_td_1", "basic block coding");

				_TR("basicOther", "basic other");
				_TR("basicHTTxStream_td_1", "basic ht txstream");
				_TR("basicHTRxStream_td_1", "basic ht rxstream");

				_TR("basicVHTPhyMode", "basic vht phy mode");
				_TR("basicVHTGI_td_1", "basic ht guard interval");
				_TR("basicVHTLongGI", "wireless long");
				_TR("basicVHTAutoGI", "wireless auto");
				_TR("basicVHTBandWidth_td_1", "basic ht channel bandwidth");
				_TR("basicVHTSTBC_td_1", "basic block coding");
				_TR("basicVHTSignalMode_td_1", "basic signal mode");
				_TR("basicVHTStatic", "basic signal mode static");
				_TR("basicVHTDynamic", "basic signal mode dynamic");
				_TR("basicSTBC", "basic stbc");
				_TR("basicVHTLDPC_td_1", "basic ldpc");
				_TR("basicAction", "basic action");

				_TR("fast_roaming", "basic roaming");
				_TR("fastRoaming_td_1", "basic roaming hangover");
				_TR("basicApProbeRspTimes_td_1", "basic roaming probe times");
				_TR("basicBandDeltaRssi_td_1", "basic roaming delta rssi");
				_TR("basicAuthRspFail_td_1", "basic roaming auth fail");
				_TR("basicAuthRspRssi_td_1", "basic roaming auth rssi");
				_TR("basicAssocReqRssiThres_td_1", "basic roaming rssi thres");
				_TR("basicAssocRspIgnor_td_1", "basic roaming assoc ignor");
				_TR("basicKickStaRssiLow_td_1", "basic roaming rssi low");
				_TR("basicKickStaRssiLowPSM_td_1", "basic roaming rssi low psm");
				_TR("basicKickStaRssiLowDelay_td_1", "basic roaming rssi low delay");
				_TR("basicProbeRspRssi_td_1", "basic roaming probe rssi");
				_TR("basicApProbeRspTimes_range", "basic roaming probe times range");
				_TR("basicBandDeltaRssi_range", "basic roaming delta rssi range");
				_TR("basicAuthRspFail_range", "basic roaming auth fail range");
				_TR("basicAuthRspRssi_range", "basic roaming auth rssi range");
				_TR("basicAssocReqRssiThres_range", "basic roaming rssi thres range");
				_TR("basicAssocRspIgnor_range", "basic roaming assoc ignor range");
				_TR("basicKickStaRssiLow_range", "basic roaming rssi low range");
				_TR("basicKickStaRssiLowPSM_range", "basic roaming rssi low psm range");
				_TR("basicKickStaRssiLowDelay_range", "basic roaming rssi low delay range");
				_TR("basicProbeRspRssi_range", "basic roaming probe rssi range");
				_TR("basic80211h", "basic dot11h");
				_TR("basicRRMEnable_td_1",			"basic roaming rrm");
				_TR("basicFtSupport_td_1",			"basic roaming ft");
				_TR("basicRRMclassINIC_td_1",		"basic roaming rc inic");

				_TR("RCINIC_1_channel",			"basic roaming rc inic channel 1");
				_TR("RCINIC_2_channel",			"basic roaming rc inic channel 2");
				_TR("RCINIC_3_channel",			"basic roaming rc inic channel 3");
				_TR("RCINIC_4_channel",			"basic roaming rc inic channel 4");

				_TR("band_steering",			"basic bandsteering");
				_TR("basicBndStrgRssiDiff",		"basic bandsteering rssidiff");
				_TR("basicBndStrgRssiLow",		"basic bandsteering rssilow");
				_TR("basicBndStrgAge",			"basic bandsteering age");
				_TR("basicBndStrgHoldTime",		"basic bandsteering hold");
				_TR("basicBndStrgCheckTime",		"basic bandsteering check");
				_TR("basicBndStrgRssiDiff_default",	"basic bandsteering rssidiff default");
				_TR("basicBndStrgRssiLow_default",	"basic bandsteering rssilow default");
				_TR("basicBndStrgAge_default",		"basic bandsteering age default");
				_TR("basicBndStrgHoldTime_default",	"basic bandsteering hold default");
				_TR("basicBndStrgCheckTime_default",	"basic bandsteering check default");

				_TR("basicIDS", "basic ids");
				_TR("ids", "basic ids");
				_TR("basicAuthFloodThreshold", "basic ids auth");
				_TR("basicAssocReqFloodThreshold", "basic ids assoc");
				_TR("basicReassocReqFloodThreshold", "basic ids reassoc");
				_TR("basicProbeReqFloodThreshold", "basic ids probe");
				_TR("basicDisassocFloodThreshold", "basic ids disassoc");
				_TR("basicDeauthFloodThreshold", "basic ids deauth");
				_TR("basicEapReqFloodThreshold", "basic ids eap");
				_TR("basicAuthFloodThreshold_default",			"basic ids auth default");
				_TR("basicAssocReqFloodThreshold_default",		"basic ids assoc default");
				_TR("basicReassocReqFloodThreshold_default",		"basic ids reassoc default");
				_TR("basicProbeReqFloodThreshold_default",		"basic ids probe default");
				_TR("basicDisassocFloodThreshold_default",		"basic ids disassoc default");
				_TR("basicDeauthFloodThreshold_default",		"basic ids deauth default");
				_TR("basicEapReqFloodThreshold_default",		"basic ids eap default");

				_TR("basicTxBf", "basic txbf");
				_TR("basicETxBfeeEn", "basic etxbfeeen");
				_TR("basicETxBfEnCond", "basic etxbfencond");

				_TR("basicApply", "button apply");
				_TR("basicCancel", "button cancel");
				_TR("basicAddBSSID", "button add");
				
				_TR("advTitle", "adv title");
				_TR("advIntroduction", "adv introduction");

				_TR("advWireless", "adv wireless");
				_TR("advBGProtect_td_1", "adv bgpro");
				_TR("advBGProAuto", "wireless auto");
				_TR("advBGProOn", "wireless on");
				_TR("advBGProOff", "wireless off");

				_TR("advBeaconInterval_td_1", "adv beacon interval");
				_TR("advBeaconIntervalINIC_td_1", "adv beacon interval inic");
				_TR("advBeaconIntervalRange", "adv beacon interval range");
				_TR("advBeaconIntervalINICRange", "adv beacon interval range");
				_TR("advDTIM_td_1", "adv dtim");
				_TR("advDTIMRange", "adv dtim range");
				_TR("advFrag_td_1", "adv fragment threshold");
				_TR("advFragRange", "adv fragment threshold range");
				_TR("advRTS_td_1", "adv rts threshold");
				_TR("advRTSRange", "adv rts threshold range");
				_TR("advPreambleType_td_1", "adv preamble type");
				_TR("preamble_long", "adv preamble type long");
				_TR("preamble_short", "adv preamble type short");
				_TR("preamble_auto", "adv preamble type auto");
				_TR("advShortSlot_td_1", "adv short slot");
				_TR("advTxBurst_td_1", "adv tx burst");
				_TR("advPktAggr_td_1", "adv pkt aggregate");
				_TR("advWmm_td_1", "adv wmm");
				_TR("advBSAuto", "wireless auto");

				_TR("advMaxStaNumRange", "adv maximum stations range");
				_TR("advKeepAliveSec", "adv station keep alive range");
				_TR("advIdleTimeoutSec", "adv idletimeout range");
				_TR("advEntryLifeCheckTimes", "adv entrylifecheck range");

				_TR("advStaRegion_td_1", "staadv region");
				_TR("advCountryCodeNONE", "wireless none");

				_TR("advMul2UniConver", "adv multicast2unicast converter");
				_TR("advMul2Uni_td_1", "adv multicast2unicast");
				_TR("advMaxStaNum_td_1", "adv maximum stations number");
				_TR("advStationKeepAlive_td_1", "adv station keep alive");
				_TR("advIdleTimeout_td_1", "adv idletimeout");
				_TR("advEntryLifeCheck_td_1", "adv entrylifecheck");
				_TR("advMcastRate_td_1", "adv mcast rate");
				_TR("advOFDM", "adv ofdm");
				_TR("advHTMIX", "adv htmix");
				_TR("advAckPolicy_td_1", "basic ack policy");
				_TR("advNormalAck", "basic ack policy normal");
				_TR("advNoAck", "basic ack policy no");
				_TR("advEDMODE_td_1", "adv ed mode");

				_TR("advDynVGATitle_td", "adv dynvga title");
				_TR("advDynVGA_td_1", "adv dynvga mode");
				_TR("advDynVGALong_td_1", "adv dynvga long");
				_TR("advDynVGAClamp", "adv dynvga clamp");
				_TR("bandauto", "adv preamble type auto");
				
				_TR("tmpBlockAfterKick_td_1", "adv tmpblockafterkick");
				_TR("tmpBlockAfterKickRange", "adv tmpblockafterkick range");

				_TR("basicKickStaRssiLowFT_td_1", "adv kickstarssilowft");
				_TR("basicKickStaRssiLowFTRange", "adv kickstarssilowft range");

				_TR("scanapLegendButtonScan", "scanap legend button scan");
				_TR("scanapLegendButtonScanINIC", "scanap legend button scan");
				
				_TR("advApply", "button apply");
				_TR("advCancel", "button cancel");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					switch (elements[i].id) {
						case "disable":             elements[i].innerHTML = _("button disable"); break;
						case "enable":              elements[i].innerHTML = _("button enable"); break;
						case "basicAutoBySTA":      elements[i].innerHTML = _("basic select by sta"); break;
						case "basicAutoByRSSI":     elements[i].innerHTML = _("basic select by rssi"); break;
					}

				elements = document.getElementsByTagName('input');
				for (i = 0; i < elements.length; i++)
					switch (elements[i].id) {
						case "basicRemove":          elements[i].value = _("button remove"); break;
					}

				elements = document.getElementsByTagName('span');
				for (i = 0; i < elements.length; i++)
					switch (elements[i].id) {
						case "basicMSSID":           elements[i].innerHTML = _("basic multiple ssid"); break;
						case "basicAutoChannelMode": elements[i].innerHTML = _("basic auto channel mode"); break;
						case "basicChannel":         elements[i].innerHTML = _("station channel"); break;
					}
			}

			function saveRadioChannels()
			{
				// Store all comboBox options in arrays
				for (var i=0; i<channel_list.length; i++)
				{
					var id = channel_list[i];
					var select = document.getElementById(id);
					var options = [];

					if (select != null)
					{
						for (var j=0; j<select.options.length; j++)
							options[j] = select.options[j];
						channels[id] = options;
					}
				}
			}

			function show14channel(show)
			{
				// Display all options except 14 frequency
				for (var i=0; i<channel_list.length; i++)
				{
					var id = channel_list[i];
					var select = document.getElementById(id);
					var options = channels[id];

					if ((select != null) && (options != null))
					{
						// Store selected option
						var index = select.selectedIndex;

						// remove all options
						select.options.length = 0;

						for (var j=0; j<options.length; j++)
						{
							if ((options[j].value != '14') || (show))
								select.options[select.options.length] = options[j];
						}

						// Restore selected option
						select.selectedIndex = (index >= select.options.length) ? 0 : index;
					}
				}
			}

			function calculate_ac_channel(countrycode, channel_indexac)
			{
				var res = 0;

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

			function initValue()
			{
				var ssidArray;
				var HiddenSSIDArray;
				var current_channel_length;

				saveRadioChannels();

				if (countrycode == '')
					countrycode = 'NONE';

				var form = document.wireless_basic;

				form.radioWirelessEnabled.options.selectedIndex = radio_on;
				form.radioWirelessEnabledAc.options.selectedIndex = (BUILD_5GHZ_SUPPORT) ? radio_on_ac : 0;

				form.bssid_num.value				= NVRAM_BssidNum;
				form.mssid_1.value					= NVRAM_SSID1;
				form.mssid_2.value					= NVRAM_SSID2;
				form.mssid_3.value					= NVRAM_SSID3;
				form.mssid_4.value					= NVRAM_SSID4;
				form.mssid_5.value					= NVRAM_SSID5;
				form.mssid_6.value					= NVRAM_SSID6;
				form.mssid_7.value					= NVRAM_SSID7;
				form.mssid_8.value					= NVRAM_SSID8;
				form.mssidac_1.value				= NVRAM_SSID1INIC;
				form.beacon.value					= NVRAM_BeaconPeriod;
				form.beaconINIC.value				= NVRAM_BeaconPeriodINIC;
				form.dtim.value						= NVRAM_DtimPeriod;
				form.fragment.value					= NVRAM_FragThreshold;
				form.rts.value						= NVRAM_RTSThreshold;
				form.EntryLifeCheck.value			= NVRAM_EntryLifeCheck;
				form.ApProbeRspTimes.value			= NVRAM_ApProbeRspTimes;
				form.BandDeltaRssi.value			= NVRAM_BandDeltaRssi;
				form.AuthRspFail.value				= NVRAM_AuthRspFail;
				form.AuthRspRssi.value				= NVRAM_AuthRspRssi;
				form.AssocReqRssiThres.value		= NVRAM_AssocReqRssiThres;
				form.AssocRspIgnor.value			= NVRAM_AssocRspIgnor;
				form.ProbeRspRssi.value				= NVRAM_ProbeRspRssi;
				form.KickStaRssiLow.value			= NVRAM_KickStaRssiLow;
				form.KickStaRssiLowPSM.value		= NVRAM_KickStaRssiLowPSM;
				form.KickStaRssiLowDelay.value		= NVRAM_KickStaRssiLowDelay;
				form.BndStrgRssiDiff.value			= NVRAM_BndStrgRssiDiff;
				form.BndStrgRssiLow.value			= NVRAM_BndStrgRssiLow;
				form.BndStrgAge.value				= NVRAM_BndStrgAge;
				form.BndStrgHoldTime.value			= NVRAM_BndStrgHoldTime;
				form.BndStrgCheckTime.value			= NVRAM_BndStrgCheckTime;
				form.AuthFloodThreshold.value		= NVRAM_AuthFloodThreshold;
				form.AssocReqFloodThreshold.value	= NVRAM_AssocReqFloodThreshold;
				form.ReassocReqFloodThreshold.value	= NVRAM_ReassocReqFloodThreshold;
				form.ProbeReqFloodThreshold.value	= NVRAM_ProbeReqFloodThreshold;
				form.DisassocFloodThreshold.value	= NVRAM_DisassocFloodThreshold;
				form.DeauthFloodThreshold.value		= NVRAM_DeauthFloodThreshold;
				form.EapReqFloodThreshold.value		= NVRAM_EapReqFloodThreshold;
				form.TmpBlockAfterKick.value		= NVRAM_TmpBlockAfterKick;
				form.KickStaRssiLowFT.value			= NVRAM_KickStaRssiLowFT;

				// Init 2.4GHz
				index = form.wirelessmode.options.length;

				form.wirelessmode.options[index++] = new Option(_("basic bg"), "0");
				form.wirelessmode.options[index++] = new Option(_("basic b"), "1");
				form.wirelessmode.options[index++] = new Option(_("basic g"), "4");
				form.wirelessmode.options[index++] = new Option(_("basic n"), "6");
				form.wirelessmode.options[index++] = new Option(_("basic gn"), "7");
				form.wirelessmode.options[index++] = new Option(_("basic bgn"), "9");

				// Init 5GHz
				if (BUILD_5GHZ_SUPPORT)
				{
					indexac = form.wirelessmodeac.options.length;

					form.wirelessmodeac.options[indexac++] = new Option(_("basic a"), "2");
					form.wirelessmodeac.options[indexac++] = new Option(_("basic aan"), "8");
					form.wirelessmodeac.options[indexac++] = new Option(_("basic an"), "11");
					form.wirelessmodeac.options[indexac++] = new Option(_("basic anc"), "14");
					form.wirelessmodeac.options[indexac++] = new Option(_("basic anac"), "15");

				}

				if (mbssid_mode == 'all')
					form.mbssid_mode.options.selectedIndex = 2;
				else if (mbssid_mode == 'rai')
					form.mbssid_mode.options.selectedIndex = 1;
				else
					form.mbssid_mode.options.selectedIndex = 0;

				// Set wmode after add all options
				form.wirelessmode.value     = wmode;
				form.wirelessmodeac.value   = wmodeac;

				// Set-up TX power combo
				for (var i=0; i<form.tx_power.options.length; i++)
				{
					if (form.tx_power.options[i].value > (txPower*1))
						break;
					form.tx_power.options.selectedIndex = i;
				}

				// Display AC
				if (BUILD_5GHZ_SUPPORT)
				{
					// Set-up TX power AC combo
					for (var i=0; i<form.tx_powerac.options.length; i++)
					{
						if (form.tx_powerac.options[i].value > (txPowerAC*1))
							break;
						form.tx_powerac.options.selectedIndex = i;
					}
				}

				// Initialize bssid
				var HiddenSSID				= NVRAM_HideSSID;
				var APIsolated				= NVRAM_NoForwarding;
				var NoForwardingMBCast		= NVRAM_NoForwardingMBCast;
				var IEEE80211H				= NVRAM_IEEE80211H;
				var HiddenSSIDArray			= HiddenSSID.split(";");
				var APIsolatedArray			= APIsolated.split(";");
				var NoForwardingMBCastArray	= NoForwardingMBCast.split(";");
				var dot11hArray				= IEEE80211H.split(";");

				for (i=0; i<bssid_num; i++) {
					form.hssid[i].checked = (HiddenSSIDArray[i] == "1");
					form.isolated_ssid[i].checked = (APIsolatedArray[i] == "1");
					form.mbcastisolated_ssid[i].checked = (NoForwardingMBCastArray[i] == "1");
				}

				form.n_bandwidth.options.selectedIndex = 1*ht_bw;
				form.n_bandwidthinic.options.selectedIndex = 1*ht_bwinic;
				form.dot11h.options.selectedIndex = 1*dot11hArray[0];
				initChecktime(form);

				channel_index = 1*channel_index;
				form.sz11gChannel.options.selectedIndex = channel_index;
				current_channel_length = form.sz11gChannel.options.length;
				if ((channel_index + 1) > current_channel_length)
					form.sz11gChannel.options.selectedIndex = 0;

				if (BUILD_5GHZ_SUPPORT)
				{
					// Calculate current channel

					if (channel_indexac > 0)
						form.sz11aChannel.options.selectedIndex = calculate_ac_channel(countrycode, channel_indexac);
					else
						form.sz11aChannel.options.selectedIndex = channel_indexac;
				}

				if (mbssid == "1")
				{
					form.mbssidapisolated.options.selectedIndex = (mbssidapisolated == "1") ? 1 : 0;
				}

				if (ht_mode == "1")
					form.n_mode.options.selectedIndex = 1;
				else
					form.n_mode.options.selectedIndex = 0;

				form.n_gi.options.selectedIndex = (ht_gi == "1") ? 1 : 0;

				form.n_stbc.options.selectedIndex = (ht_stbc == "1") ? 1 : 0;

				if (is3t3r == "1")
				{
					for (i = 16; i < 24; i++)
						form.n_mcs.options[i] = new Option(i, i);
				}

				var mcs_length = form.n_mcs.options.length;
				if (1*is3t3r == 1)
				{
					form.n_mcs.options[mcs_length] = new Option("32", "32");
					mcs_length++;
					form.n_mcs.options[mcs_length] = new Option(_("wireless auto"), "33");
				}

				if (1*ht_mcs <= mcs_length-1)
					form.n_mcs.options.selectedIndex = ht_mcs;
				else if (1*ht_mcs == 32)
					form.n_mcs.options.selectedIndex = mcs_length-2;
				else if (1*ht_mcs == 33)
					form.n_mcs.options.selectedIndex = mcs_length-1;

				form.n_rdg.options.selectedIndex = (ht_rdg == "0") ? 0 : 1;

				form.n_amsdu.options.selectedIndex = (ht_amsdu ==  "0") ? 0 : 1;
				form.n_autoba.options.selectedIndex = (ht_autoba == "0") ? 0 : 1;
				form.n_badecline.options.selectedIndex = (ht_badecline == "0") ? 0 : 1;

				if (is3t3r == "1")
				{
					form.rx_stream.options[2] = new Option("3", "3");
					form.tx_stream.options[2] = new Option("3", "3");
				}

				if (BUILD_WLAN_4T4R)
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

				form.ac_gi.options.selectedIndex = (vht_gi ==  "0") ? 0 : 1;
				form.ac_stbc.options.selectedIndex = (vht_stbc ==  "0") ? 0 : 1;
				form.ac_ldpc.options.selectedIndex = (vht_ldpc ==  "0") ? 0 : 1;

				form.ac_bw.options.selectedIndex = vht_bw;
				form.ac_bwsig.options.selectedIndex = vht_bwsig;

				AutoChannelSelect(form);
				form.BandSteering.options.selectedIndex = NVRAM_BandSteering;
				form.FastRoaming.options.selectedIndex = NVRAM_FastRoaming;
				form.IdsEnable.options.selectedIndex = NVRAM_IdsEnable;

				if (txbf_built == '1') {
					form.ITxBfEn.options.selectedIndex = NVRAM_ITxBfEn;
					form.ETxBfeeEn.options.selectedIndex = NVRAM_ETxBfeeEn;
					form.ETxBfEnCond.options.selectedIndex = NVRAM_ETxBfEnCond;
				}

				var rrm = NVRAM_RRMEnable;
				var ft = NVRAM_FtSupport;
				var rrmArray = rrm.split(";");
				var ftArray = ft.split(";");
				form.RRMEnable.options.selectedIndex = 1*rrmArray[0];
				form.FtSupport.options.selectedIndex = 1*ftArray[0];

				var LanWifiIsolate = NVRAM_LanWifiIsolate;
				form.LanWifiIsolate.options.selectedIndex = 1*LanWifiIsolate;

				var datarateArray;
				var AckPolicyArray = ackpolicy.split(";");

				form.bg_protection.options.selectedIndex = bgProtection;
				form.preamble_type.options.selectedIndex = preambleType;
				form.short_slot.options.selectedIndex = (shortSlot == '1') ? 1 : 0;
				form.tx_burst.options.selectedIndex = (txBurst == '1') ? 1 : 0;
				form.pkt_aggregate.options.selectedIndex = (pktAggregate == '1') ? 1 : 0;

				form.McastPhyMode.value = McastPhyMode;
				McastSwitch(form);

				//multicase to unicast converter
				form.m2u_enable.options.selectedIndex = (m2uEnabled == '1') ? 1 : 0;
				form.video_turbine.options.selectedIndex = (video_turbine == '1') ? 1 : 0;
				form.WmmCapable.options.selectedIndex = (wmmCapable == '1') ? 1 : 0;

				form.dyn_vga.options.selectedIndex = (dyn_vga == '1') ? 1 : 0;

				form.advDynVGALong.options.selectedIndex = (advDynVGALong == '1') ? 1 : 0;
				form.advDynVGAClamp.options.selectedIndex = 1*advDynVGAClamp;

				if (isNaN(maxstanum) || maxstanum < 1 || maxstanum > MAX_STA_NUM)
					form.maxstanum.value = 1*MAX_STA_NUM;
				else
					form.maxstanum.value = maxstanum;

				if (isNaN(keepalive) || keepalive < 10 || keepalive > 300)
					form.keepalive.value = 60;
				else
					form.keepalive.value = keepalive;

				if (isNaN(idletimeout) || idletimeout < 60 || idletimeout > 600)
					form.idletimeout.value = 480;
				else
					form.idletimeout.value = idletimeout;

				form.AckPolicy.options.selectedIndex = 1*AckPolicyArray[0];

				form.ED_MODE.options.selectedIndex = (ED_MODE == '1') ? 1 : 0;

				document.getElementById('tmpBlockAfterKick_td_2').title = _('adv tmpblockafterkick title');

				if (radio_on == 1)
					document.getElementById('wlan_channel_span').innerHTML = '<b>' + _('station channel') +':</b> ' + '<% getWlanChannel_ioctl(); %>';
				if (radio_on_ac == 1 && BUILD_5GHZ_SUPPORT)
					document.getElementById('wlan_ac_channel_span').innerHTML = '<b>' + _('station channel') +':</b> ' + '<% getWlanChannelAC_ioctl(); %>';

				initTranslation();
				showWarning();

				// Minimize all menus
				var menus = document.getElementsByClassName('showHideMenu');
				for (var i=0;i<menus.length;i++) {
					showMenu(menus[i],0);
				}

				updateVisibility(form);
			}

			function show_abg_rate(form)
			{
				var wmode = form.wirelessmode.value;

				//ABG Rate
				if ((wmode == "0") || (wmode == "2") || (wmode == "4"))
				{
					ht_mcs = 1*ht_mcs;
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

					if (fxtxmode == "CCK" || fxtxmode == "cck")
					{
						if (ht_mcs == 0)
							abg_rate.value = "1";
						else if (ht_mcs == 1)
							abg_rate.value = "2";
						else if (ht_mcs == 2)
							abg_rate.value = "5";
						else if (ht_mcs == 3)
							abg_rate.value = "11";
						else
							abg_rate.value = "0";
					}
					else
					{
						if (ht_mcs == 0)
							abg_rate.value = "6";
						else if (ht_mcs == 1)
							abg_rate.value = "9";
						else if (ht_mcs == 2)
							abg_rate.value = "12";
						else if (ht_mcs == 3)
							abg_rate.value = "18";
						else if (ht_mcs == 4)
							abg_rate.value = "24";
						else if (ht_mcs == 5)
							abg_rate.value = "36";
						else if (ht_mcs == 6)
							abg_rate.value = "48";
						else if (ht_mcs == 7)
							abg_rate.value = "54";
						else
							abg_rate.value = "0";
					}
				}
				else if (wmode == "1")
				{
					ht_mcs = 1*ht_mcs;
					var abg_rate = form.abg_rate;

					abg_rate.options.length = 0;
					abg_rate.options[0] = new Option(_("wireless auto"), "0");
					abg_rate.options[1] = new Option("1 Mbps", "1");
					abg_rate.options[2] = new Option("2 Mbps", "2");
					abg_rate.options[3] = new Option("5.5 Mbps", "5");
					abg_rate.options[4] = new Option("11 Mbps", "11");

					if (ht_mcs == 0)
						abg_rate.value = "1";
					else if (ht_mcs == 1)
						abg_rate.value = "2";
					else if (ht_mcs == 2)
						abg_rate.value = "5";
					else if (ht_mcs == 3)
						abg_rate.value = "11";
					else
						abg_rate.value = "0";
				}
			}

			function CheckValue(form)
			{
				if (form.mssid_1.value == "" &&  form.radioWirelessEnabled.value == 1)
				{
					alert(_("adv no mssid_1"));
					form.mssid_1.focus();
					form.mssid_1.select();
					return false;
				}

				if (form.mssidac_1.value == "" &&  form.radioWirelessEnabledAc.value == 1)
				{
					alert(_("adv no mssid_1"));
					form.mssidac_1.focus();
					form.mssidac_1.select();
					return false;
				}

				// multiple ssids
				for (var i=2;i<=form.bssid_num.value;i++)
				{
					var elem = form.elements["mssid_"+i];
					// if empty and visible on page
					if (elem.value == "" && elem.offsetParent != null)
					{
						alert(_("adv no mssid_1"));
						elem.focus();
						elem.select();
						return false;
					}
				}

				if (isNaN(form.beacon.value) || form.beacon.value < 20 || form.beacon.value > 999)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid beacon"));
					form.beacon.focus();
					form.beacon.select();
					return false;
				}

				if ((isNaN(form.beaconINIC.value) || form.beaconINIC.value < 20 || form.beaconINIC.value > 999) && BUILD_5GHZ_SUPPORT)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid beacon"));
					form.beaconINIC.focus();
					form.beaconINIC.select();
					return false;
				}

				if (isNaN(form.dtim.value) || form.dtim.value < 1 || form.dtim.value > 255)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid dtim"));
					form.dtim.focus();
					form.dtim.select();
					return false;
				}

				if (isNaN(form.fragment.value) || form.fragment.value < 1 || form.fragment.value > 2346)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid frag len"));
					form.fragment.focus();
					form.fragment.select();
					return false;
				}

				if (isNaN(form.rts.value) || form.rts.value < 1 || form.rts.value > 2347)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid rts"));
					form.rts.focus();
					form.rts.select();
					return false;
				}

				if (isNaN(form.maxstanum.value) || form.maxstanum.value < 1 || form.maxstanum.value > MAX_STA_NUM)
				{
					showMenu('div_roaming', 1);
					alert(_("adv invalid max sta num"));
					form.maxstanum.focus();
					form.maxstanum.select();
					return false;
				}

				if (isNaN(form.KickStaRssiLowFT.value) || form.KickStaRssiLowFT.value < -100 || form.KickStaRssiLowFT.value > 0)
				{
					showMenu('div_roaming', 1);
					if (form.FastRoaming.options.selectedIndex == 0) {
						form.FastRoaming.options.selectedIndex = 1;
						updateVisibility(document.wireless_basic);
					}
					alert(_("raoming invalid number"));
					form.KickStaRssiLowFT.focus();
					form.KickStaRssiLowFT.select();
					return false;
				}

				if (isNaN(form.TmpBlockAfterKick.value) || form.TmpBlockAfterKick.value < 0 || form.TmpBlockAfterKick.value > 200)
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

				if (isNaN(form.keepalive.value) || form.keepalive.value < 10 || form.keepalive.value > 300)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid keepalive"));
					form.keepalive.focus();
					form.keepalive.select();
					return false;
				}

				if (isNaN(form.idletimeout.value) || form.idletimeout.value < 60 || form.idletimeout.value > 600)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid idletimeout"));
					form.idletimeout.focus();
					form.idletimeout.select();
					return false;
				}

				if (isNaN(form.EntryLifeCheck.value) || form.EntryLifeCheck.value < 256 || form.EntryLifeCheck.value > 4096)
				{
					showMenu('advWirelessT', 1);
					alert(_("adv invalid entrylifecheck"));
					form.EntryLifeCheck.focus();
					form.EntryLifeCheck.select();
					return false;
				}

				if (form.BandSteering.value == 1) {
					if (isNaN(form.BndStrgRssiDiff.value) || form.BndStrgRssiDiff.value < 0 || form.BndStrgRssiDiff.value > 40)
					{
						alert(_("raoming invalid BndStrgRssiDiff"));
						form.BndStrgRssiDiff.focus();
						form.BndStrgRssiDiff.select();
						return false;
					}

					if (isNaN(form.BndStrgRssiLow.value) || form.BndStrgRssiLow.value < -100 || form.BndStrgRssiLow.value > 0)
					{
						alert(_("raoming invalid BndStrgRssiLow"));
						form.BndStrgRssiLow.focus();
						form.BndStrgRssiLow.select();
						return false;
					}

					if (isNaN(form.BndStrgAge.value) && form.BndStrgAge.value > 0)
					{
						alert(_("raoming invalid number"));
						form.BndStrgAge.focus();
						form.BndStrgAge.select();
						return false;
					}

					if (isNaN(form.BndStrgHoldTime.value) && form.BndStrgHoldTime.value > 0)
					{
						alert(_("raoming invalid number"));
						form.BndStrgHoldTime.focus();
						form.BndStrgHoldTime.select();
						return false;
					}

					if (isNaN(form.BndStrgCheckTime.value) && form.BndStrgCheckTime.value > 0)
					{
						alert(_("raoming invalid number"));
						form.BndStrgCheckTime.focus();
						form.BndStrgCheckTime.select();
						return false;
					}
				}

				if (form.IdsEnable.value == 1) {
					if (isNaN(form.AuthFloodThreshold.value) && form.AuthFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.AuthFloodThreshold.focus();
						form.AuthFloodThreshold.select();
						return false;
					}

					if (isNaN(form.AssocReqFloodThreshold.value) && form.AssocReqFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.AssocReqFloodThreshold.focus();
						form.AssocReqFloodThreshold.select();
						return false;
					}

					if (isNaN(form.ReassocReqFloodThreshold.value) && form.ReassocReqFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.ReassocReqFloodThreshold.focus();
						form.ReassocReqFloodThreshold.select();
						return false;
					}

					if (isNaN(form.ProbeReqFloodThreshold.value) && form.ProbeReqFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.ProbeReqFloodThreshold.focus();
						form.ProbeReqFloodThreshold.select();
						return false;
					}

					if (isNaN(form.DisassocFloodThreshold.value) && form.DisassocFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.DisassocFloodThreshold.focus();
						form.DisassocFloodThreshold.select();
						return false;
					}

					if (isNaN(form.DeauthFloodThreshold.value) && form.DeauthFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.DeauthFloodThreshold.focus();
						form.DeauthFloodThreshold.select();
						return false;
					}

					if (isNaN(form.EapReqFloodThreshold.value) && form.EapReqFloodThreshold.value >= 0)
					{
						alert(_("raoming invalid number"));
						form.EapReqFloodThreshold.focus();
						form.EapReqFloodThreshold.select();
						return false;
					}
				}

				form.mbssid_mode.disabled = false;

				// concatenate ssid values before send
				form.isolated_ssids.value = "";
				for (var i = 0; i < form.isolated_ssid.length; i++) {
				    if (form.isolated_ssid[i].checked) {
				        if (i > 0) form.isolated_ssids.value += ",";
				        form.isolated_ssids.value += form.isolated_ssid[i].value;
				    }
				}

				form.mbcastisolated_ssids.value = "";
				for (var i = 0; i < form.mbcastisolated_ssid.length; i++) {
				    if (form.mbcastisolated_ssid[i].checked) {
				        if (i > 0) form.mbcastisolated_ssids.value += ",";
				        form.mbcastisolated_ssids.value += form.mbcastisolated_ssid[i].value;
				    }
				}

				form.hidden_ssids.value = "";
				for (var i = 0; i < form.hssid.length; i++) {
				    if (form.hssid[i].checked) {
				        if (i > 0) form.hidden_ssids.value += ",";
				        form.hidden_ssids.value += form.hssid[i].value;
				    }
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

				ajaxShowTimer(form, 'timerReloader', _('message apply'), (document.getElementById('sz11gChannel').value == 0 || document.getElementById('sz11aChannel').value == 0) ? 40 : 30);
				return true;
			}

			function McastSwitch(form) {
				form.McastMcs.options.length = 0;

				var max_mcs = (form.McastPhyMode.value == "2") ? 7 : 15;

				for(var i = 0; i < (max_mcs + 1); i++) {
					addOption(form.McastMcs, "MCS: " + i, i);
					if(i <= mcastMcs)
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

			function showMenu(elem, show){
				if (typeof elem == "string" && document.getElementById(elem) !== undefined) elem = document.getElementById(elem);
				if (elem.tagName == "THEAD" || elem.tagName == "TBODY") elem = elem.parentNode;
				if (elem.tagName != "TABLE") return false;

				var table_head = elem.tHead;
				var table_body = elem.tBodies[0];

				if (table_head === undefined || table_body === undefined) return false;

				if (show === undefined || show != 0)
				{
					table_body.style.removeProperty('display');
					elem.setAttribute('minimized', '0');
				}
				else
				{
					table_body.style.display = "none";
					elem.setAttribute('minimized', '1');
				}

				return true;
			}

			function switchShowMenu(elem){
				if (typeof elem == "string" && document.getElementById(elem) !== undefined) elem = document.getElementById(elem);
				if (elem.tagName == "THEAD" || elem.tagName == "TBODY") elem = elem.parentNode;
				if (elem.tagName != "TABLE") return false;

				var table_head = elem.tHead;
				var table_body = elem.tBodies[0];

				if (table_head === undefined || table_body === undefined) return false;

				if (table_body.style.display == "none")
				{
					showMenu(elem, 1);
				}
				else
				{
					showMenu(elem, 0);
				}

				return true;
			}

		</script>
	</head>

<body bgcolor="#FFFFFF" onLoad="initValue();">
<div id="warning"></div>
<table class="body">
	<tr>
		<td><h1 id="basicTitle">SSID Settings </h1> 
			<p id="basicIntroduction"> Here you can configure the most basic settings of Wireless communication, such as Network Name (SSID) and Channel.
			These settings are sufficient to have a working Access Point. </p>
			<hr>
	<form method="POST" name="wireless_basic" action="/goform/wirelessBasic" onSubmit="return CheckValue(this);">
	<input id="isolated_ssids" name="isolated_ssids" type="hidden" value="">
	<input id="hidden_ssids" name="hidden_ssids" type="hidden" value="">
	<input id="mbcastisolated_ssids" name="mbcastisolated_ssids" type="hidden" value="">
	<iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
	<table id="wireless_24" class="form">
		<col style="width: 225px" />
		<col style="width: 515px" />
		<thead>
			<tr>
				<td class="title" colspan="2" id="basicWirelessNet">Wireless Network</td>
			</tr>
		</thead>
		<tbody>
			<tr id="basicWirelessEnabled">
				<td id="basicWireless" class="head" width="25%">Wireless</td>
				<td><select name="radioWirelessEnabled" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disabled</option>
					<option value="1" id="enable">Enabled</option>
				</select>
				<span style="display: inline-block; margin-left: 20px; width: 180px"><b>BSSID:</b> <% getWlanCurrentMac(); %></span><span id="wlan_channel_span"></span></td>
			</tr>
			<tr id="div_11g_basic" name="div_11g_basic">
				<td class="head" id="basicNetMode" colspan="1">Network Mode</td>
				<td><select name="wirelessmode" id="wirelessmode" class="normal" onChange="updateVisibility(this.form);">
				</select></td>
			</tr>
			<tr id="div_txpw" name="div_txpw">
				<td class="head auth-readonly-user" id="basicTxPW" colspan="1">TX Power (2.4GHz)</td>
				<td><select name="tx_power" class="normal auth-disable-user">
					<option value="5">5%</option>
					<option value="10">10%</option>
					<option value="20">20%</option>
					<option value="40">40%</option>
					<option value="70">70%</option>
					<option value="100">100%</option>
				</select></td>
			</tr>
			<tr id="div_11g_channel" name="div_11g_channel">
				<td class="head" colspan="1" id="basicFreqG">Channel (2.4GHz)</td>
				<td><select id="sz11gChannel" name="sz11gChannel" class="normal" onChange="ChannelOnChange(this.form);">
					<option value="0" id="basicFreqGAuto">AutoSelect</option>
					<% getWlan11gChannels(); %>
					</select>&nbsp;&nbsp;<select name="autoselect_g" style="normal" id="autoselect_g">
					<option value="1" id="basicAutoBySTA">by STA count</option>
					<option value="2" id="basicAutoByRSSI">by RSSI</option>
					</select>&nbsp;&nbsp;<select name="checktime_g" id="checktime_g">
				</select>
					<input id="scanapLegendButtonScan" name="scanapLegendButtonScan" type="button" class="short" value="Scan" onClick="scanAp('2.4');">
				</td>
			</tr>
			<tr id="scanAp" style="display: none;">
				<td colspan="2">
					<div style="width: 785; height: 300px;">
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
		</tbody>
	</table>
	<table id="wireless_5" class="form">
		<col style="width: 225px" />
		<col style="width: 515px" />
		<thead>
			<tr>
				<td class="title" colspan="2" id="basicWirelessNetAc">Wireless Network 5GHz</td>
			</tr>
		</thead>
		<tbody>
			<tr id="basicWirelessEnabledAc">
				<td class="head" id="basicWirelessAC">Wireless (5GHz)</td>
				<td><select name="radioWirelessEnabledAc" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disabled</option>
					<option value="1" id="enable">Enabled</option>
				</select>
				<span style="display: inline-block; margin-left: 20px; width: 180px"><b>BSSID:</b> <% getWlanCurrentMacAC(); %></span><span id="wlan_ac_channel_span"></span></td>
			</tr>
			<tr id="div_11a_basic" name="div_11a_basic">
				<td id="basicAcNetMode" class="head">Network Mode (5GHz)</td>
				<td><select name="wirelessmodeac" id="wirelessmodeac" class="normal" onChange="updateVisibility(this.form);">
				</select></td>
			 </tr>
			<tr id="div_txpw_ac" name="div_txpw_ac">
				<td class="head auth-readonly-user" id="basicTxPWAc" colspan="1">TX Power (5GHz)</td>
				<td><select name="tx_powerac" class="normal auth-disable-user">
					<option value="5">5%</option>
					<option value="10">10%</option>
					<option value="20">20%</option>
					<option value="40">40%</option>
					<option value="70">70%</option>
					<option value="100">100%</option>
				</select></td>
			</tr>
			<tr id="div_11a_channel" name="div_11a_channel">
				<td class="head" colspan="1" id="basicFreqA">Channel (5GHz)</td>
				<td><select id="sz11aChannel" name="sz11aChannel" class="normal" onChange="ChannelOnChange(this.form);">
					<option value="0" id="basicFreqAAuto">AutoSelect</option>
					<% getWlan11aChannels(); %>
					</select>&nbsp;&nbsp;<select name="autoselect_a" style="normal" id="autoselect_a">
					<option value="1" id="basicAutoBySTA">by STA count</option>
					<option value="2" id="basicAutoByRSSI">by RSSI</option>
					</select>&nbsp;&nbsp;<select name="checktime_a" style="normal" id="checktime_a">
				</select>
					<input id="scanapLegendButtonScanINIC" name="scanapLegendButtonScanINIC" type="button" class="short" value="Scan" onClick="scanAp('5');">
				</td>
			</tr>
			<tr id="scanApINIC" style="display: none;">
				<td colspan="2">
					<div style="width: 785; height: 300px;">
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
		</tbody>
	</table>
	<table id="div_all" name="div_all" class="form" style="display:none;">
		<col style="width: 225px" />
		<col style="width: 165px" />
		<col style="width: 80px" />
		<col style="width: 87px" />
		<col style="width: 87px" />
		<col style="width: 90px" />
		<tbody>
			<tr>
				<td class="title" colspan="6" id="basicWirelessSettings">Basic Settings</td>
			</tr>
			<tr>
				<td class="title" colspan="1"></td>
				<td id="basicNetworkName" class="title" colspan="1" >Network name</td>
				<td id="basicHSSID" class="title" colspan="1" align="center">Hidden</td>
				<td class="title" colspan="1" align="center"><span id="basicClientsSSID">Clients</span><br><span id="basicIsolatedSSID">Isolated</span></td>
				<td class="title" colspan="1" align="center"><span id="basicBroadcast">Broadcast</span><br><span id="basicIsolatedSSID1">Isolated</span></td>
				<td id="basicAction"class="title" colspan="1" align="center">Action</td>
			</tr>
			<tr id="div_11g_name" name="div_11g_name">
				<td class="head" id="basicSSID" colspan="1">Network Name (2.4GHz)</td>
					<input type="hidden" name="bssid_num">
				<td colspan="1"><input class="normal" type="text" name="mssid_1" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="0"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="0"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="0"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidAdd(this.form);" style="width: 80px;" name="addBSSIDbtn" value="Add BSSID" id="basicAddBSSID"></td>
			</tr>
			<tr id="div_hssid1" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>1</td>
				<td colspan="1"><input class="normal" name="mssid_2" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="1"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="1"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="1"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 1);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_hssid2" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>2</td>
				<td colspan="1"><input class="normal" type="text" name="mssid_3" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="2"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="2"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="2"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 2);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_hssid3" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>3</td>
				<td colspan="1"><input class="normal" type="text" name="mssid_4" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="3"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="3"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="3"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 3);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_hssid4" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>4</td>
				<td colspan="1"><input class="normal" type="text" name="mssid_5" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="4"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="4"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="4"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 4);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_hssid5" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>5</td>
				<td colspan="1"><input class="normal" type="text" name="mssid_6" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="5"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="5"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="5"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 5);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_hssid6" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>6</td>
				<td colspan="1"><input class="normal" type="text" name="mssid_7" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="6"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="6"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="6"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 6);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_hssid7" style="display:none;">
				<td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>7</td>
				<td colspan="1"><input class="normal" type="text" name="mssid_8" maxlength="32"></td>
				<td colspan="1" align="center"><input type="checkbox" name="hssid" value="7"></td>
				<td colspan="1" align="center"><input type="checkbox" name="isolated_ssid" value="7"></td>
				<td colspan="1" align="center"><input type="checkbox" name="mbcastisolated_ssid" value="7"></td>
				<td colspan="1" align="center"><input type="button" onClick="ssidRemove(this.form, 7);" style="width: 80px;" value="Remove" id="basicRemove"></td>
			</tr>
			<tr id="div_11a_name" name="div_11a_name">
				<td class="head" id="basicAcSSID" colspan="1">Network Name (5GHz)</td>
				<td colspan="5"><input class="normal" type="text" name="mssidac_1" maxlength="32"></td>
			</tr>
			<tr id="basicMbssidModeT">
				<td class="head" id="basicMBSSIDMode" colspan="1">MBSSID Mode</td>
				<td colspan="5"><select name="mbssid_mode" size="1" class="normal" onChange="updateVisibility(this.form);">
					<option value="ra" selected>2.4GHz</option>
					<option value="rai">5GHz</option>
					<option value="all" id="mbssid_mode_all">All</option>
				</select></td> 
			</tr>		
			<tr id="div_mbssidapisolated">
				<td class="head" colspan="1" id="basicMBSSIDApIsolated">SSID to SSID Isolation</td>
				<td colspan="5"><select name="mbssidapisolated" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
				</select></td>
			</tr>
			<tr id="div_LanWifiIsolate">
				<td class="head" colspan="1" id="basicLanWifiIsolate">LAN to WLAN Isolation</td>
				<td colspan="5"><select name="LanWifiIsolate" class="normal">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
				</select></td>
			</tr>
			<tr id="div_abg_rate">
				<td class="head" colspan="1" id="basicRate">Rate</td>
				<td colspan="5"><select name="abg_rate" class="normal">
				</select></td>
			</tr>
			<tr id="div_dot11h">
				<td class="head" colspan="1" id="basic80211h">IEEE802.11H</td>
				<td colspan="5"><select name="dot11h" class="normal">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
				</select></td>
			</tr>
		</tbody>
	</table>
	<table id="div_11n" name="div_11n" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td id="basicHTPhyMode" class="title" colspan="2">HT Physical Mode</td>
		</tr>
	</thead>
	<tbody>
		<tr id="basicHTChannelBW_tr">
			<td id="basicHTChannelBW_td_1" class="head" width="50%">Channel BandWidth</td>
			<td id="basicHTChannelBW_td_1" width="50%"><select id="n_bandwidth" name="n_bandwidth" class="normal" onClick="GExtChannelDisplay(this.form);">
				<option value="0">20MHz</option>
				<option value="1">20/40MHz</option>
			</select></td>
		</tr>
		<tr id="basicHTChannelBWINIC_tr">
			<td id="basicHTChannelBWINIC_td_1" class="head" width="50%">Channel BandWidth (5GHz)</td>
			<td id="basicHTChannelBWINIC_td_1" width="50%"><select id="n_bandwidthinic" name="n_bandwidthinic" class="normal" onClick="GExtChannelDisplay(this.form);">
				<option value="0">20MHz</option>
				<option value="1">20/40MHz</option>
			</select></td>
		</tr>
		<tr id="extension_channel" name="extension_channel">
			<td class="head" width="50%" id="basicHTExtChannel">Extension Channel</td>
			<td width="50%"><select id="n_extcha" name="n_extcha" class="normal">
				<option value="1" selected>2412MHz (Channel 1)</option>
			</select></td>
		</tr>
		<tr id="basicHTTxStream_tr">
			<td id="basicHTTxStream_td_1" class="head" width="50%">HT TxStream</td>
			<td id="basicHTTxStream_td_2" width="50%"><select name="tx_stream" class="normal">
				<option value="1">1</option>
				<option id="basicHTTx2Stream" value="2">2</option>
			</select></td>
		</tr>
		<tr id="basicHTRxStream_tr">
			<td id="basicHTRxStream_td_1" class="head" width="50%">HT RxStream</td>
			<td id="basicHTRxStream_td_2" width="50%"><select name="rx_stream" class="normal">
				<option value="1">1</option>
				<option value="2" id="div_HtRx2Stream">2</option>
			</select></td>
		</tr>
		<tr id="basicHTMCS_tr">
			<td id="basicHTMCS_td_1" class="head" width="50%">MCS</td>
			<td id="basicHTMCS_td_2" width="50%"><select name="n_mcs" class="normal">
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
				<option value = "33" id="basicHTAutoMCS" selected >Auto</option>
			</select></td>
		</tr>
		<tr id="basicHTGI_tr">
			<td id="basicHTGI_td_1" class="head" width="50%" id="basicHTGI">Guard Interval</td>
			<td id="basicHTGI_td_2" width="50%"><select name="n_gi" class="normal">
				<option value="0" id="basicHTLongGI">Long</option>
				<option value="1" id="basicHTAutoGI">Auto</option>
			</select></td>
		</tr>
		<tr id="basicHSTBC_tr">
			<td id="basicHSTBC_td_1" class="head" width="50%">Space-Time Block Coding</td>
			<td id="basicHSTBC_td_2" width="50%"><select name="n_stbc" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTAMSDU_tr">
			<td id="basicHTAMSDU_td_1" class="head" width="50%" >Aggregation MSDU</td>
			<td id="basicHTAMSDU_td_2" width="50%"><select name="n_amsdu" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTAddBA_tr">
			<td id="basicHTAddBA_td_1" class="head" width="50%">Auto Block ACK</td>
			<td id="basicHTAddBA_td_2" width="50%"><select name="n_autoba" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTDelBA_tr">
			<td id="basicHTDelBA_td_1" class="head" width="50%">Decline Block ACK Request</td>
			<td id="basicHTDelBA_td_2" width="50%"><select name="n_badecline" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTOPMode_tr" style="display: none;">
			<td id="basicHTOPMode_td_1" class="head" width="50%">Greenfield Mode</td>
			<td id="basicHTOPMode_td_2" width="50%"><select name="n_mode" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicHTRDG_tr">
			<td id="basicHTRDG_td_1" class="head" width="50%">Reverse Direction Grant</td>
			<td id="basicHTRDG_td_2" width="50%"><select name="n_rdg" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="basicVHT" name="basicVHT" class="form showHideMenu auth-hide-user" style="display:none;">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td id="basicVHTPhyMode" class="title" colspan="2">VHT Physical Mode</td>
		</tr>
	</thead>
	<tbody>
		<tr id="basicVHTBW_tr" >
			<td id="basicVHTBandWidth_td_1" class="head" width="50%">Channel BandWidth</td>
			<td id="basicVHTBandWidth_td_1" width="50%"><select name="ac_bw" size="1" class="normal">
				<option value="0" selected id="1">20/40MHz</option>
				<option value="1" id="2">20/40/80MHz</option>
			</select></td>
		</tr>
		<tr id="basicVHTBWSIGNAL_tr">
			<td id="basicVHTSignalMode_td_1" class="head" width="50%">BandWidth Signaling Mode</td>
			<td id="basicVHTSignalMode_td_2" width="50%"><select name="ac_bwsig" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="2" id="basicVHTStatic">Static</option>
				<option value="2" id="3" id="basicVHTDynamic">Dynamic</option>
			</select></td>
		</tr>
		<tr id="basicVHTGI_tr">
			<td id="basicVHTGI_td_1" class="head" width="50%">Guard Interval</td>
			<td id="basicVHTGI_td_1" width="50%"><select name="ac_gi" class="normal">
				<option value="0" id="basicVHTLongGI">Long</option>
				<option value="1" id="basicVHTAutoGI">Auto</option>
			</select></td>
		</tr>
		<tr id="basicVHTSTBC_tr" name="basicVHTSTBC_tr">
			<td id="basicVHTSTBC_td_1" class="head" width="50%">Space-Time Block Coding</td>
			<td id="basicVHTSTBC_td_2" width="50%"><select name="ac_stbc" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="basicVHTLDCP_tr" name="basicVHTLDCP_tr">
			<td id="basicVHTLDPC_td_1" class="head" width="50%">Low Disenty parity check</td>
			<td id="basicVHTLDPC_td_2" width="50%"><select name="ac_ldpc" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="advWirelessT" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td id="advWireless" class="title" colspan="2">Advanced Wireless</td>
		</tr>
	</thead>
	<tbody>
		<tr id="advBGProtect_tr">
			<td id="advBGProtect_td_1" class="head" width="50%">BG Protection Mode</td>
			<td id="advBGProtect_td_2" width="50%"><select id="bg_protection" name="bg_protection" size="1" class="normal">
				<option value="0" selected id="advBGProAuto">Auto</option>
				<option value="1" id="advBGProOn">On</option>
				<option value="2" id="advBGProOff">Off</option>
			</select></td>
		</tr>
		<tr id="advDTIM_tr">
			<td id="advDTIM_td_1" class="head" width="50%">Data Beacon Rate</td>
			<td id="advDTIM_td_2" width="50%"><input type="text" name="dtim" class="normal" maxlength="3">
				<font color="#808080" id="advDTIMRange">(range 1 - 255)</font></td>
		</tr>
		<tr id="advFrag_tr">
			<td id="advFrag_td_1" class="head" width="50%">Fragment Threshold</td>
			<td id="advFrag_td_2" width="50%"><input type="text" name="fragment" class="normal" maxlength="4">
				<font color="#808080" id="advFragRange">(range 256 - 2346)</font></td>
		</tr>
		<tr id="advRTS_tr">
			<td id="advRTS_td_1" class="head" width="50%">RTS Threshold</td>
			<td id="advRTS_td_2" width="50%"><input type="text" name="rts" class="normal" maxlength="4">
				<font color="#808080" id="advRTSRange">(range 1 - 2347)</font></td>
		</tr>
		<tr id="advStationKeepAlive_tr">
			<td id="advStationKeepAlive_td_1" class="head" width="50%" >Station Keep-Alive</td>
			<td id="advStationKeepAlive_td_2" width="50%"><input type="text" name="keepalive" class="normal" maxlength="3" value="">
				<font color="#808080" id="advKeepAliveSec"> (10 - 300)</font></td>
		</tr>
		<tr id="advPreambleType_tr">
			<td id="advPreambleType_td_1" class="head" width="50%">Preamble Type</td>
			<td id="advPreambleType_td_2" width="50%"><select name="preamble_type" size="1" class="normal">
				<option value="0" selected id="preamble_long">Long</option>
				<option value="1" id="preamble_short">Short</option>
				<option value="2" id="preamble_auto">AUTO</option>
			</select></td>
		</tr>
		<tr id="advShortSlot_tr">
			<td id="advShortSlot_td_1" class="head" width="50%">Short Slot</td>
			<td id="advShortSlot_td_1" width="50%"><select name="short_slot" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advTxBurst_tr">
			<td id="advTxBurst_td_1" class="head" width="50%">Tx Burst</td>
			<td id="advTxBurst_td_2" width="50%"><select name="tx_burst" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advPktAggr_tr">
			<td id="advPktAggr_td_1" class="head" width="50%">Pkt_Aggregate</td>
			<td id="advPktAggr_td_2" width="50%"><select name="pkt_aggregate" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advWmm_tr">
			<td id="advWmm_td_1" class="head" width="50%">WMM Capable</td>
			<td id="advWmm_td_2" width="50%"><select name="WmmCapable" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advAckPolicy_tr">
			<td id="advAckPolicy_td_1" class="head" width="50%" colspan="1">ACK Policy</td>
			<td id="advAckPolicy_td_2" colspan="5" width="50%"><select name="AckPolicy" class="normal">
				<option value="0" id="advNormalAck">Normal ACK</option>
				<option value="1" id="advNoAck">No ACK</option>
			</select></td>
		</tr>
		<tr id="advMcastRate_tr">
			<td id="advMcastRate_td_1" class="head" width="50%">Multicast TX mode</td>
			<td id="advMcastRate_td_2" width="50%"><select name="McastPhyMode" class="normal" onClick="McastSwitch(this.form);">
				<option value="2" id="advOFDM">Legacy OFDM</option>
				<option value="3" id="advHTMIX">HTMIX</option>
				</select>&nbsp;&nbsp;&nbsp;
				<select name="McastMcs" class="normal"></select></td>
		</tr>
		<tr id="advEDMODE_tr">
			<td id="advEDMODE_td_1" class="head" width="50%">Clear-Channel Assessment Monitor</td>
			<td id="advEDMODE_td_2" width="50%"><select name="ED_MODE" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advStaRegion_tr">
			<td id="advStaRegion_td_1" class="head" width="50%">Region settings</td>
			<td id="advStaRegion_td_2" width="50%"><select id="country_region" name="country_region" class="normal">
				<option value=0 <% writeIfCfgZeroEq("CountryRegion", "0", "selected", ""); %> >0: CH1-11 (FCC)</option>
				<option value=1 <% writeIfCfgZeroEq("CountryRegion", "1", "selected", ""); %> >1: CH1-13 (IC)</option>
				<option value=2 <% writeIfCfgZeroEq("CountryRegion", "2", "selected", ""); %> >2: CH10-11 (ETSI)</option>
				<option value=3 <% writeIfCfgZeroEq("CountryRegion", "3", "selected", ""); %> >3: CH10-13 (SPAIN)</option>
				<option value=4 <% writeIfCfgZeroEq("CountryRegion", "4", "selected", ""); %> >4: CH14 (France)</option>
				<option value=5 <% writeIfCfgZeroEq("CountryRegion", "5", "selected", ""); %> >5: CH1-14 (MKK)</option>
				<option value=6 <% writeIfCfgZeroEq("CountryRegion", "6", "selected", ""); %> >6: CH3-9 (MKK1)</option>
				<option value=7 <% writeIfCfgZeroEq("CountryRegion", "7", "selected", ""); %> >7: CH5-13 (Israel)</option>
				</select>&nbsp;&nbsp;&nbsp;
				<select name="country_code" class="normal" onChange="countryCodeChange();"><% listCountryCodes(); %>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="div_roaming" name="div_roaming" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr id="fast_roaming_tr">
			<td id="fast_roaming" class="title" colspan="2">Fast roaming</td>
		</tr>
	</thead>
	<tbody>
		<tr id="fastRoaming_tr">
			<td id="fastRoaming_td_1" class="head" width="50%">Handover settings</td>
			<td id="fastRoaming_td_1" width="50%"><select name="FastRoaming" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
				</select>
			</td>
		</tr>
		<tr id="basicApProbeRspTimes_tr">
			<td id="basicApProbeRspTimes_td_1" class="head" width="50%">Limit probe response to client</td>
			<td id="basicApProbeRspTimes_td_2" width="50%"><input type="text" name="ApProbeRspTimes" class="normal" maxlength="4"><font id="basicApProbeRspTimes_range" color="#808080"> 0 - 10 times, default 3</font></td>
		</tr>
		<tr id="basicBandDeltaRssi_tr">
			<td id="basicBandDeltaRssi_td_1" class="head" width="50%">Delta threshold rssi level for 5GHz band</td>
			<td id="basicBandDeltaRssi_td_2" width="50%"><input type="text" name="BandDeltaRssi" class="normal" maxlength="4"><font id="basicBandDeltaRssi_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicAuthRspFail_tr">
			<td id="basicAuthRspFail_td_1" class="head" width="50%">Reject auth req due to weak signal</td>
			<td id="basicAuthRspFail_td_2" width="50%"><input type="text" name="AuthRspFail" class="normal" maxlength="4"><font id="basicAuthRspFail_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicAuthRspRssi_tr">
			<td id="basicAuthRspRssi_td_1" class="head" width="50%">Ignore auth req due to weak signal</td>
			<td id="basicAuthRspRssi_td_2" width="50%"><input type="text" name="AuthRspRssi" class="normal" maxlength="4"><font id="basicAuthRspRssi_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicAssocReqRssiThres_tr">
			<td id="basicAssocReqRssiThres_td_1" class="head" width="50%">Reject assoc req due to weak signal</td>
			<td id="basicAssocReqRssiThres_td_2" width="50%"><input type="text" name="AssocReqRssiThres" class="normal" maxlength="4"><font id="basicAssocReqRssiThres_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicAssocRspIgnor_tr">
			<td id="basicAssocRspIgnor_td_1" class="head" width="50%">Ignore assoc req due to weak signal</td>
			<td id="basicAssocRspIgnor_td_2" width="50%"><input type="text" name="AssocRspIgnor" class="normal" maxlength="4"><font id="basicAssocRspIgnor_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicProbeRspRssi_tr">
			<td id="basicProbeRspRssi_td_1" class="head" width="50%">Auto disonnect sta if rssi low at probe requests</td>
			<td id="basicProbeRspRssi_td_2" width="50%"><input type="text" name="ProbeRspRssi" class="normal" maxlength="4"><font id="basicProbeRspRssi_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicKickStaRssiLow_tr">
			<td id="basicKickStaRssiLow_td_1" class="head" width="50%">Auto disonnect sta if rssi low (active clients)</td>
			<td id="basicKickStaRssiLow_td_2" width="50%"><input type="text" name="KickStaRssiLow" class="normal" maxlength="4"><font id="basicKickStaRssiLow_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicKickStaRssiLowPSM_tr">
			<td id="basicKickStaRssiLowPSM_td_1" class="head" width="50%">Auto disonnect sta if rssi low (powersave clients)</td>
			<td id="basicKickStaRssiLowPSM_td_2" width="50%"><input type="text" name="KickStaRssiLowPSM" class="normal" maxlength="4"><font id="basicKickStaRssiLowPSM_range" color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
		</tr>
		<tr id="basicKickStaRssiLowFT_tr">
			<td id="basicKickStaRssiLowFT_td_1" class="head" width="50%">Auto disonnect STA if rssi low (FT clients)</td>
			<td id="basicKickStaRssiLowFT_td_2" width="50%">
				<input type="text" name="KickStaRssiLowFT" class="normal" maxlength="4" value="">
				<font color="#808080" id="basicKickStaRssiLowFTRange">0 - -100 dBm, default: 0 (off)</font>
			</td>
		</tr>
		<tr id="basicKickStaRssiLowDelay_tr">
			<td id="basicKickStaRssiLowDelay_td_1" class="head" width="50%">How time rssi check before kick</td>
			<td id="basicKickStaRssiLowDelay_td_2" width="50%"><input type="text" name="KickStaRssiLowDelay" class="normal" maxlength="4"><font id="basicKickStaRssiLowDelay_range" color="#808080"> 0 - 200 seconds, default 5 </font></td>
		</tr>
		<tr id="tmpBlockAfterKick_tr">
			<td id="tmpBlockAfterKick_td_1" class="head" width="50%">Temporary block probe/assoc req from kicked client</td>
			<td id="tmpBlockAfterKick_td_2" width="50%">
				<input type="text" name="TmpBlockAfterKick" class="normal" maxlength="4" value="">
				<font color="#808080" id="tmpBlockAfterKickRange"> 0 - 200 times, default: 14</font>
			</td>
		</tr>
		<tr id="basicRRMEnable_tr">
			<td id="basicRRMEnable_td_1" class="head" width="50%">RRMEnable</td>
			<td id="basicRRMEnable_td_2" width="50%">
				<select name="RRMEnable" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
				</select>
			</td>
		</tr>
		<tr id="basicRRMclassINIC_tr">
			<td id="basicRRMclassINIC_td_1" class="head" width="50%">Regulatory Class INIC</td>
			<td id="basicRRMclassINIC_td_2">
				<input type="checkbox" id="RCINIC_1" value="0">&nbsp;<span id="RCINIC_1_channel" style="position: absolute; margin-top: 3px;">36, 40, 44, 48 channels</span><br>
				<input type="checkbox" id="RCINIC_2" value="0">&nbsp;<span id="RCINIC_2_channel" style="position: absolute; margin-top: 3px;">52, 56, 60, 64 channels</span><br>
				<input type="checkbox" id="RCINIC_3" value="0">&nbsp;<span id="RCINIC_3_channel" style="position: absolute; margin-top: 3px;">132, 136, 140, 144 channels</span><br>
				<input type="checkbox" id="RCINIC_4" value="0">&nbsp;<span id="RCINIC_4_channel" style="position: absolute; margin-top: 3px;">149, 153, 157, 161, 165 channels</span>
				<input name="RegulatoryClassINIC" type="hidden" value="1;2;3;4;0">
			</td>
		</tr>
		<tr id="basicFtSupport_tr">
			<td id="basicFtSupport_td_1" class="head" width="50%">FtSupport</td>
			<td id="basicFtSupport_td_2" width="50%"><select name="FtSupport" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advMaxStaNum_tr">
			<td id="advMaxStaNum_td_1" class="head" width="50%">Maximum clients per SSID</td>
			<td id="advMaxStaNum_td_2" width="50%"><input type="text" name="maxstanum" class="normal" maxlength="3" value="">
				<font color="#808080" id="advMaxStaNumRange">num</font><font color="#808080"> (1 - <% getMaxStaNum(); %>)</font></td>
		</tr>
		<tr id="advIdleTimeout_tr">
			<td id="advIdleTimeout_td_1" class="head" width="50%">IdleTimeout</td>
			<td id="advIdleTimeout_td_2" width="50%"><input type="text" name="idletimeout" class="normal" maxlength="3" value="">
				<font color="#808080" id="advIdleTimeoutSec"> (60 - 300)</font></td>
		</tr>
		<tr id="advEntryLifeCheck_tr">
			<td id="advEntryLifeCheck_td_1" class="head" width="50%">EntryLifeCheck</td>
			<td id="advEntryLifeCheck_td_2" width="50%"><input type="text" name="EntryLifeCheck" class="normal" maxlength="4">
				<font color="#808080" id="advEntryLifeCheckTimes"> (256 - 4096)</font></td>
		</tr>
		<tr id="advBeaconInterval_tr">
			<td id="advBeaconInterval_td_1" class="head" width="50%">Beacon Interval</td>
			<td id="advBeaconInterval_td_2" width="50%"><input type="text" name="beacon" class="normal" maxlength="3">
				<font color="#808080" id="advBeaconIntervalRange">(range 20 - 999)</font></td>
		</tr>
		<tr id="advBeaconIntervalINIC_tr">
			<td id="advBeaconIntervalINIC_td_1" class="head" width="50%">Beacon Interval (5GHz)</td>
			<td id="advBeaconIntervalINIC_td_2" width="50%"><input type="text" name="beaconINIC" class="normal" maxlength="3">
				<font color="#808080" id="advBeaconIntervalINICRange">(range 20 - 999)</font></td>
		</tr>
	</tbody>
	</table>
	<table id="div_txbf" name="div_txbf" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr>
			<td class="title" id="basicTxBf" width="50%" >Beamforming</td>
			<td class="title" size="1"  width="50%" style="text-align:right">
				<select name="ITxBfEn" class="half" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
	</thead>
	<tbody>
		<tr id="div_ETxBfeeEn" style="display:none;">
			<td class="head" id="basicETxBfeeEn" width="50%">BFee feature</td>
			<td width="50%"><select name="ETxBfeeEn" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="div_ETxBfEnCond" style="display:none;">
			<td class="head" id="basicETxBfEnCond" width="50%">Sending of sounding and beamforming</td>
			<td width="50%"><select name="ETxBfEnCond" class="normal">
				<option value="0" id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="div_bandsteering" name="div_bandsteering" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr id="row_BndStrg">
			<td class="title" id="band_steering" width="50%">Band steering</td>
			<td class="title" width="50%" style="text-align:right">
				<select name="BandSteering" size="1" class="half" onChange="bandSteeringChange(this.form, 1);">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
					<!-- <option value="2" id="bandauto">Auto</option> -->
				</select>
			</td>
		</tr>
	</thead>
	<tbody>
		<tr id="row_BndStrgRssiDiff" style="display:none;">
			<td class="head" id="basicBndStrgRssiDiff" width="50%">Allow fallback to 2.4GHz if bands RSSI diff bigger</td>
			<td width="50%"><input type="text" name="BndStrgRssiDiff" class="normal" maxlength="2">&nbsp;<span id="basicBndStrgRssiDiff_default" style="color: #808080">0 - 40 db, default 15</span></td>
		</tr>
		<tr id="row_BndStrgRssiLow" style="display:none;">
			<td class="head" id="basicBndStrgRssiLow" width="50%">Force fallback to 2.4GHz if rssi smaller</td>
			<td width="50%"><input type="text" name="BndStrgRssiLow" class="normal" maxlength="4">&nbsp;<span id="basicBndStrgRssiLow_default" style="color: #808080">0 - -100 db, default -88</span></td>
		</tr>
		<tr id="row_BndStrgAge" style="display:none;">
			<td class="head" id="basicBndStrgAge" width="50%">Inactive client entry age time</td>
			<td width="50%"><input type="text" name="BndStrgAge" class="normal" maxlength="6">&nbsp;<span id="basicBndStrgAge_default" style="color: #808080">ms, default 600000</span></td>
		</tr>
		<tr id="row_BndStrgHoldTime" style="display:none;">
			<td class="head" id="basicBndStrgHoldTime" width="50%">Time for holding 2.4G connection</td>
			<td width="50%"><input type="text" name="BndStrgHoldTime" class="normal" maxlength="4">&nbsp;<span id="basicBndStrgHoldTime_default" style="color: #808080">ms, default 3000</span></td>
		</tr>
		<tr id="row_BndStrgCheckTime" style="display:none;">
			<td class="head" id="basicBndStrgCheckTime" width="50%">Time for deciding if a client is 2.4G only</td>
			<td width="50%"><input type="text" name="BndStrgCheckTime" class="normal" maxlength="4">&nbsp;<span id="basicBndStrgCheckTime_default" style="color: #808080">ms, default 5000</span></td>
		</tr>
	</tbody>
	</table>
	<table id="div_ids" name="div_ids" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr>
			<td class="title" id="basicIDS" width="50%">Intrusion Detection settings</td>
			<td class="title" width="50%" style="text-align:right">
				<select name="IdsEnable" size="1" class="half" onChange="updateVisibility(this.form);">
					<option value="0" id="disable">Disable</option>
					<option value="1" id="enable">Enable</option>
				</select>
			</td>
		</tr>
	</thead>
	<tbody>
		<tr id="row_AuthFloodThreshold" style="display:none;">
			<td class="head" id="basicAuthFloodThreshold" width="50%">Authentication</td>
			<td width="50%"><input type="text" name="AuthFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicAuthFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
		<tr id="row_AssocReqFloodThreshold" style="display:none;">
			<td class="head" id="basicAssocReqFloodThreshold" width="50%">Association request</td>
			<td width="50%"><input type="text" name="AssocReqFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicAssocReqFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
		<tr id="row_ReassocReqFloodThreshold" style="display:none;">
			<td class="head" id="basicReassocReqFloodThreshold" width="50%">Reassociation request</td>
			<td width="50%"><input type="text" name="ReassocReqFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicReassocReqFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
		<tr id="row_ProbeReqFloodThreshold" style="display:none;">
			<td class="head" id="basicProbeReqFloodThreshold" width="50%">Probe request</td>
			<td width="50%"><input type="text" name="ProbeReqFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicProbeReqFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
		<tr id="row_DisassocFloodThreshold" style="display:none;">
			<td class="head" id="basicDisassocFloodThreshold" width="50%">Disassociation</td>
			<td width="50%"><input type="text" name="DisassocFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicDisassocFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
		<tr id="row_DeauthFloodThreshold" style="display:none;">
			<td class="head" id="basicDeauthFloodThreshold" width="50%">Deauthentication</td>
			<td width="50%"><input type="text" name="DeauthFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicDeauthFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
		<tr id="row_EapReqFloodThreshold" style="display:none;">
			<td class="head" id="basicEapReqFloodThreshold" width="50%">EAP request</td>
			<td width="50%"><input type="text" name="EapReqFloodThreshold" class="normal" maxlength="4">&nbsp;<span id="basicEapReqFloodThreshold_default" style="color: #808080">default 64</span></td>
		</tr>
	</tbody>
	</table>
	<table id="div_m2u" name="div_m2u" class="form auth-hide-user">
	<thead>
		<tr>
			<td class="title" colspan="2" id="advMul2UniConver">Multicast-to-Unicast Converter</td>
		</tr>
	</thead>
	<tbody>
		<tr>
			<td class="head"  width="50%" id="advMul2Uni">Multicast-to-Unicast</td>
			<td width="50%"><select name="m2u_enable" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="video_turbine_row">
			<td class="head" width="50%">Video turbine</td>
			<td  width="50%"><select name="video_turbine" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="advSynVGA_table" class="form auth-hide-user">
	<thead>
		<tr id="advDynVGATitle_tr">
			<td id="advDynVGATitle_td" class="title" colspan="2">PA/LNA control (WARNING!!! FOR EXPERTS ONLY!)</td>
		</tr>
	</thead>
	<tbody>
		<tr id="advDynVGA_tr">
			<td id="advDynVGA_td_1" class="head" width="50%">Adaptive LNA Gain</td>
			<td id="advDynVGA_td_2" width="50%"><select name="dyn_vga" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advDynVGALong_tr">
			<td id="advDynVGALong_td_1" class="head" width="50%">Skip auto gain for long distance clients</td>
			<td id="advDynVGALong_td_2" width="50%"><select name="advDynVGALong" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
				<option value="1" id="enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advDynVGAClamp_tr">
			<td id="advDynVGAClamp_td_1" class="head" width="50%">Clamp maximum LNA GAIN in dynvga mode</td>
			<td id="advDynVGAClamp_td_2"  width="50%"><select name="advDynVGAClamp" size="1" class="normal">
				<option value="0" selected id="disable">Disable</option>
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
				<input type="submit" class="normal" value="Apply" id="basicApply"><input type="button" class="normal" value="Cancel" id="basicCancel" onClick="window.location.reload();">
				<input type="hidden" name="submit-url" value="/wireless/main.asp">
			</td>
		</tr>
	</table>
	</form>
	<div class="whitespace"></div></td>
	</tr>
</table>
</body>
</html>
