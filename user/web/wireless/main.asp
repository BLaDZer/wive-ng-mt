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
				// IEEE 802.11h support
				displayElement('div_dot11h', (BUILD_DFS == '1') && enableWirelessAc);

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

				if (BUILD_5GHZ_1T1R == 1)
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
				displayElement('advEDMODE_tr', BUILD_EDCCA == "1");

// Basic roaming settings (Fast Roaming)
				displayElement([ 'advMaxStaNum_tr', 'fastRoaming_tr', 'basicRRMEnable_tr', 'basicRRMclassINIC_tr', 'basicFtSupport_tr', 'advIdleTimeout_tr', 'advEntryLifeCheck_tr', 'advBeaconInterval_tr', 'advBeaconIntervalINIC_tr'], 1);
				displayElement([ 'basicRRMEnable_tr' ], rrm_built);
				displayElement([ 'basicRRMclassINIC_tr' ], BUILD_5GHZ_SUPPORT && rrm_built && document.wireless_basic.RRMEnable.value == '1' && document.wireless_basic.country_code.value == 'RU');
				displayElement([ 'basicFtSupport_tr', 'basicKickStaRssiLowFT_tr'], ft_built);
				displayElement('advBeaconIntervalINIC_tr', BUILD_5GHZ_SUPPORT)

				displayElement('div_roaming', enableWirelessAny);
				// Regulatory Class INIC
				displayElement('basicRRMclassINIC_tr', BUILD_5GHZ_SUPPORT && rrm_built && document.wireless_basic.RRMEnable.value == '1' && document.wireless_basic.country_code.value == 'RU');

				displayElement(["basicApProbeRspTimes_tr", "basicAuthRspFail_tr", "basicBandDeltaRssi_tr", "basicAuthRspRssi_tr", "basicAssocReqRssiThres_tr", "basicAssocRspIgnor_tr", "basicKickStaRssiLow_tr", "basicKickStaRssiLowPSM_tr", "basicKickStaRssiLowFT_tr", "basicKickStaRssiLowDelay_tr", "basicProbeRspRssi_tr", "tmpBlockAfterKick_tr"], form.FastRoaming.value == "1");
				displayElement("basicBandDeltaRssi_tr", form.FastRoaming.value == "1" && BUILD_5GHZ_SUPPORT);
				displayElement("basicKickStaRssiLowFT_tr", form.FastRoaming.value == "1" && ft_built);

// Beamforming
				displayElement('div_txbf', BUILD_TXBF == '1' && enableWirelessAny);
				// BFee feature
				displayElement('div_ETxBfeeEn', form.ITxBfEn.value == "1");
				// Sending of sounding and beamforming
				displayElement('div_ETxBfEnCond', form.ITxBfEn.value == "1");


// Band steering settings
				var have_steering = enableWireless && enableWirelessAc && BUILD_BANDSTEERING == 1;
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
				displayElement('div_m2u', BUILD_WLAN_M2U == '1' && enableWirelessAny);
				// Video turbine
				displayElement('video_turbine_row', (BUILD_WLAN_M2U == '1') && (BUILD_VIDEO_TURBINE == '1'));

// PA/LNA control
				displayElement('advSynVGA_table', enableWirelessAny);
				// Adaptive LNA Gain
				displayElement('advDynVGA_tr', enableWirelessAny);
				// Skip auto gain for long distance clients
				displayElement('advDynVGALong_tr', BUILD_CLAMP == '1');
				// Clamp maximum LNA GAIN in dynvga mode
				displayElement('advDynVGAClamp_tr', BUILD_CLAMP == '1' && BUILD_5GHZ_1T1R == '0');

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

					if (1*NVRAM_HT_EXTCHA == 1)
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
				var count = form.bssid_num.value * 1;
				if (show === undefined) show = true;

				for (var i=1; i < 8; i++)
					displayElement('div_hssid' + i, show && i<count);

				// Allow only 4 BSSID's
				displayElement(form.addBSSIDbtn, true);
				form.addBSSIDbtn.disabled = (count >= bssid_num) || !show;
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
				var trlist = document.getElementsByAttribute("data-tr");
				for (var i=0; i<trlist.length; i++) {
					var elem = trlist[i];
					_TR(elem, elem.getAttribute("data-tr"));
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

			function initValue()
			{
				saveRadioChannels();

				var form = document.wireless_basic;

				form.radioWirelessEnabled.options.selectedIndex = radio_on;
				form.radioWirelessEnabledAc.options.selectedIndex = (BUILD_5GHZ_SUPPORT) ? radio_on_ac : 0;

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
				form.country_region.value			= NVRAM_CountryRegion;

				if (mbssid_mode == 'all')
					form.mbssid_mode.options.selectedIndex = 2;
				else if (mbssid_mode == 'rai')
					form.mbssid_mode.options.selectedIndex = 1;
				else
					form.mbssid_mode.options.selectedIndex = 0;

				// Set wmode after add all options
				form.wirelessmode.value     = NVRAM_WirelessMode;
				form.wirelessmodeac.value   = NVRAM_WirelessModeINIC;

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
				form.n_bandwidthinic.options.selectedIndex = 1*NVRAM_HT_BWINIC;
				form.dot11h.options.selectedIndex = 1*dot11hArray[0];
				initChecktime(form);

				var channel_index = 1*WLAN_CHANNEL_INDEX;
				form.sz11gChannel.options.selectedIndex = channel_index;
				var current_channel_length = form.sz11gChannel.options.length;
				if ((channel_index + 1) > current_channel_length)
					form.sz11gChannel.options.selectedIndex = 0;

				if (BUILD_5GHZ_SUPPORT)
				{
					// Calculate current channel
					form.sz11aChannel.options.selectedIndex = calculate_ac_channel(NVRAM_CountryCode, 1*WLAN_CHANNEL_INDEX_AC);
				}

				if (BUILD_MBSSID == "1")
				{
					form.mbssidapisolated.options.selectedIndex = (mbssidapisolated == "1") ? 1 : 0;
				}

				form.n_mode.options.selectedIndex = (NVRAM_HT_OpMode == "1") ? 1 : 0;
				form.n_gi.options.selectedIndex = (NVRAM_HT_GI == "1") ? 1 : 0;
				form.n_stbc.options.selectedIndex = (NVRAM_HT_STBC == "1") ? 1 : 0;

				//get last element position
				var mcs_lastelem = form.n_mcs.options.length-1;

				//add for 4t4r devices
				if (BUILD_WLAN_4T4R == "1")
				{
					for (i = 16; i < 32; i++)
						form.n_mcs.options[i] = new Option(i, i);

					// recalc lastelem afrer add with replace options
					mcs_lastelem = form.n_mcs.options.length-1;
					mcs_lastelem++;
					form.n_mcs.options[mcs_lastelem] = new Option("32", "32");
					mcs_lastelem++;
					form.n_mcs.options[mcs_lastelem] = new Option(_("wireless auto"), "33");
				}

				if (ht_mcs <= mcs_lastelem-1)
					form.n_mcs.options.selectedIndex = ht_mcs;
				else if (ht_mcs == 32)
					form.n_mcs.options.selectedIndex = mcs_lastelem-1;
				else if (ht_mcs == 33)
					form.n_mcs.options.selectedIndex = mcs_lastelem;

				form.n_rdg.options.selectedIndex = (NVRAM_HT_RDG == "0") ? 0 : 1;

				form.n_amsdu.options.selectedIndex = (NVRAM_HT_AMSDU ==  "0") ? 0 : 1;
				form.n_autoba.options.selectedIndex = (NVRAM_HT_AutoBA == "0") ? 0 : 1;
				form.n_badecline.options.selectedIndex = (NVRAM_HT_BADecline == "0") ? 0 : 1;

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

				form.ac_gi.options.selectedIndex = (NVRAM_VHT_SGI ==  "0") ? 0 : 1;
				form.ac_stbc.options.selectedIndex = (NVRAM_VHT_STBC ==  "0") ? 0 : 1;
				form.ac_ldpc.options.selectedIndex = (NVRAM_VHT_LDPC ==  "0") ? 0 : 1;

				form.ac_bw.options.selectedIndex = NVRAM_VHT_BW;
				form.ac_bwsig.options.selectedIndex = NVRAM_VHT_BW_SIGNAL;

				AutoChannelSelect(form);
				form.BandSteering.options.selectedIndex = NVRAM_BandSteering;
				form.FastRoaming.options.selectedIndex = NVRAM_FastRoaming;
				form.IdsEnable.options.selectedIndex = NVRAM_IdsEnable;

				if (BUILD_TXBF == '1') {
					form.ITxBfEn.options.selectedIndex = NVRAM_ITxBfEn;
					form.ETxBfeeEn.options.selectedIndex = NVRAM_ETxBfeeEn;
					form.ETxBfEnCond.options.selectedIndex = NVRAM_ETxBfEnCond;
				}

				form.RRMEnable.options.selectedIndex = 1*NVRAM_RRMEnable.split(";")[0];
				form.FtSupport.options.selectedIndex = 1*NVRAM_FtSupport.split(";")[0];

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
				form.WmmCapable.options.selectedIndex = (NVRAM_WmmCapable == '1') ? 1 : 0;

				form.dyn_vga.options.selectedIndex = (NVRAM_DyncVgaEnable == '1') ? 1 : 0;

				form.advDynVGALong.options.selectedIndex = (NVRAM_SkipLongRangeVga == '1') ? 1 : 0;
				form.advDynVGAClamp.options.selectedIndex = 1*NVRAM_VgaClamp;

				form.maxstanum.value = inRange(maxstanum, 1, MAX_STA_NUM) ? maxstanum : 1*MAX_STA_NUM;
				form.keepalive.value = inRange(keepalive, 10, 300) ? keepalive : 60;
				form.idletimeout.value = inRange(NVRAM_IdleTimeout, 60, 600) ? NVRAM_IdleTimeout : 480;

				form.AckPolicy.options.selectedIndex = 1*NVRAM_AckPolicy.split(";")[0];

				form.ED_MODE.options.selectedIndex = (NVRAM_ED_MODE == '1') ? 1 : 0;

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

				if (!inRange(form.fragment.value, 1, 2346))
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

				if (!inRange(form.idletimeout.value, 60, 600))
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

				for (var i = 0; i < form.isolated_ssid.length; i++) {
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

			function showMenu(elem, show){
				if (typeof elem == "string" && document.getElementById(elem) !== undefined) elem = document.getElementById(elem);
				if (elem.tagName == "THEAD" || elem.tagName == "TBODY") elem = elem.parentNode;
				if (elem.tagName != "TABLE") return false;

				var table_head = elem.tHead;
				var table_body = elem.tBodies[0];

				if (table_head === undefined || table_body === undefined) return false;

				if (show === undefined || show != 0)
				{
					if (table_body.style.removeProperty === undefined)
					{
						table_body.style.removeAttribute('display'); // IE<9
					}
					else
					{
						table_body.style.removeProperty('display');
					}
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

		<style>
table.form td.head {
    width: 40%;
    text-align: left;
}

table.form td.head-narrow {
    width: 30%;
}

table.form td.val {
    width: 50%;
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
	<form method="POST" name="wireless_basic" action="/goform/wirelessBasic" onSubmit="return CheckValue(this);">
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
				<td class="title" colspan="2" data-tr="basic wireless network">Wireless Network</td>
			</tr>
		</thead>
		<tbody>
			<tr id="basicWirelessEnabled">
				<td class="head head-narrow" data-tr="basic wireless">Wireless</td>
				<td><select name="radioWirelessEnabled" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disabled</option>
					<option value="1" data-tr="button enable">Enabled</option>
				</select>
				<span style="display: inline-block; margin-left: 20px; width: 180px"><b>BSSID:</b> <% getWlanCurrentMac(); %></span><span id="wlan_channel_span"></span></td>
			</tr>
			<tr id="div_11g_basic">
				<td class="head head-narrow" data-tr="basic network mode">Network Mode</td>
				<td><select name="wirelessmode" id="wirelessmode" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="basic bg">BG</option>
					<option value="1" data-tr="basic b">B</option>
					<option value="4" data-tr="basic g">G</option>
					<option value="6" data-tr="basic n">N</option>
					<option value="7" data-tr="basic gn">GN</option>
					<option value="9" data-tr="basic bgn">BGN</option>
				</select></td>
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
				<td><select id="sz11gChannel" name="sz11gChannel" class="normal" onChange="ChannelOnChange(this.form);">
					<option value="0" data-tr="basic frequency auto">AutoSelect</option>
					<% getWlan11gChannels(); %>
					</select>&nbsp;&nbsp;<select name="autoselect_g" class="normal" id="autoselect_g">
					<option value="1" data-tr="basic select by sta">by STA count</option>
					<option value="2" data-tr="basic select by rssi">by RSSI</option>
					</select>&nbsp;&nbsp;<select name="checktime_g" id="checktime_g">
				</select>
					<input id="scanapLegendButtonScan" name="scanapLegendButtonScan" type="button" class="short" value="Scan" data-tr="scanap legend button scan" onClick="scanAp('2.4');">
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
		</tbody>
	</table>

	<table id="wireless_5" class="form">
		<col style="width: 225px" />
		<col style="width: 515px" />
		<thead>
			<tr>
				<td class="title" colspan="2" data-tr="basic wireless network ac">Wireless Network 5GHz</td>
			</tr>
		</thead>
		<tbody>
			<tr id="basicWirelessEnabledAc">
				<td class="head head-narrow" data-tr="basic wireless ac">Wireless (5GHz)</td>
				<td><select name="radioWirelessEnabledAc" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disabled</option>
					<option value="1" data-tr="button enable">Enabled</option>
				</select>
				<span style="display: inline-block; margin-left: 20px; width: 180px"><b>BSSID:</b> <% getWlanCurrentMacAC(); %></span><span id="wlan_ac_channel_span"></span></td>
			</tr>
			<tr id="div_11a_basic">
				<td class="head head-narrow" data-tr="basic ac network mode">Network Mode (5GHz)</td>
				<td><select name="wirelessmodeac" id="wirelessmodeac" class="normal" onChange="updateVisibility(this.form);">
					<option value="2" data-tr="basic a">A</option>
					<option value="8" data-tr="basic aan">A/AN</option>
					<option value="11" data-tr="basic an">AN</option>
					<option value="14" data-tr="basic anc">A/AN/AC</option>
					<option value="15" data-tr="basic anac">AN/AC</option>
				</select></td>
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
				<td><select id="sz11aChannel" name="sz11aChannel" class="normal" onChange="ChannelOnChange(this.form);">
					<option value="0" data-tr="basic frequency auto">AutoSelect</option>
					<% getWlan11aChannels(); %>
					</select>&nbsp;&nbsp;<select name="autoselect_a" class="normal" id="autoselect_a">
					<option value="1" data-tr="basic select by sta">by STA count</option>
					<option value="2" data-tr="basic select by rssi">by RSSI</option>
					</select>&nbsp;&nbsp;<select name="checktime_a" class="normal" id="checktime_a">
				</select>
					<input id="scanapLegendButtonScanINIC" name="scanapLegendButtonScanINIC" type="button" class="short" value="Scan" data-tr="scanap legend button scan" onClick="scanAp('5');">
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
				<td class="title" data-tr="basic network name">Network name</td>
				<td class="title" align="center" data-tr="basic hssid">Hidden</td>
				<td class="title" align="center"><span data-tr="basic clients">Clients</span><br><span data-tr="basic isolated">Isolated</span></td>
				<td class="title" align="center"><span id="basicBroadcast">Broadcast</span><br><span data-tr="basic isolated">Isolated</span></td>
				<td class="title" align="center" data-tr="basic action">Action</td>
			</tr>
			<tr id="div_11g_name" class="ssid-row">
				<td class="head" data-tr="basic ssid">Network Name (2.4GHz)</td>
				<td><input class="normal" type="text" name="mssid_1" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="0"></td>
				<td><input type="checkbox" name="isolated_ssid" value="0"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="0"></td>
				<td><input type="button" onClick="ssidAdd(this.form);" class="mssid-button" name="addBSSIDbtn" value="Add BSSID" data-tr="button add"></td>
			</tr>
			<tr id="div_hssid1" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>1</td>
				<td><input class="normal" name="mssid_2" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="1"></td>
				<td><input type="checkbox" name="isolated_ssid" value="1"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="1"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 1);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_hssid2" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>2</td>
				<td><input class="normal" type="text" name="mssid_3" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="2"></td>
				<td><input type="checkbox" name="isolated_ssid" value="2"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="2"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 2);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_hssid3" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>3</td>
				<td><input class="normal" type="text" name="mssid_4" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="3"></td>
				<td><input type="checkbox" name="isolated_ssid" value="3"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="3"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 3);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_hssid4" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>4</td>
				<td><input class="normal" type="text" name="mssid_5" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="4"></td>
				<td><input type="checkbox" name="isolated_ssid" value="4"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="4"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 4);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_hssid5" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>5</td>
				<td><input class="normal" type="text" name="mssid_6" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="5"></td>
				<td><input type="checkbox" name="isolated_ssid" value="5"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="5"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 5);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_hssid6" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>6</td>
				<td><input class="normal" type="text" name="mssid_7" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="6"></td>
				<td><input type="checkbox" name="isolated_ssid" value="6"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="6"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 6);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_hssid7" class="ssid-row">
				<td class="head"><span class="mssid-label" data-tr="basic multiple ssid">Multiple SSID</span>7</td>
				<td><input class="normal" type="text" name="mssid_8" maxlength="32"></td>
				<td><input type="checkbox" name="hssid" value="7"></td>
				<td><input type="checkbox" name="isolated_ssid" value="7"></td>
				<td><input type="checkbox" name="mbcastisolated_ssid" value="7"></td>
				<td><input type="button" onClick="ssidRemove(this.form, 7);" class="mssid-button" value="Remove" data-tr="button remove"></td>
			</tr>
			<tr id="div_11a_name">
				<td class="head" data-tr="basic ac ssid">Network Name (5GHz)</td>
				<td colspan="5"><input class="normal" type="text" name="mssidac_1" maxlength="32"></td>
			</tr>
			<tr id="basicMbssidModeT">
				<td class="head" data-tr="basic mbssid mode">MBSSID Mode</td>
				<td colspan="5"><select name="mbssid_mode" class="normal" onChange="updateVisibility(this.form);">
					<option value="ra" selected>2.4GHz</option>
					<option value="rai">5GHz</option>
					<option value="all" data-tr="basic mbssid mode all">All</option>
				</select></td> 
			</tr>		
			<tr id="div_mbssidapisolated">
				<td class="head" data-tr="basic mbssidapisolated">SSID to SSID Isolation</td>
				<td colspan="5"><select name="mbssidapisolated" class="normal" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select></td>
			</tr>
			<tr id="div_LanWifiIsolate">
				<td class="head" data-tr="basic lan wifi isolate">LAN to WLAN Isolation</td>
				<td colspan="5"><select name="LanWifiIsolate" class="normal">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select></td>
			</tr>
			<tr id="div_abg_rate">
				<td class="head" data-tr="basic rate">Rate</td>
				<td colspan="5"><select name="abg_rate" class="normal">
				</select></td>
			</tr>
			<tr id="div_dot11h">
				<td class="head" data-tr="basic dot11h">IEEE802.11H</td>
				<td colspan="5"><select name="dot11h" class="normal">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
				</select></td>
			</tr>
		</tbody>
	</table>

	<table id="div_11n" class="form showHideMenu auth-hide-user">
	<thead onclick="switchShowMenu(this);">
		<tr>
			<td class="title" colspan="2" data-tr="basic ht phy mode">HT Physical Mode</td>
		</tr>
	</thead>
	<tbody>
		<tr id="basicHTChannelBW_tr">
			<td class="head" data-tr="basic ht channel bandwidth">Channel BandWidth</td>
			<td class="val"><select id="n_bandwidth" name="n_bandwidth" class="normal" onClick="GExtChannelDisplay(this.form);">
				<option value="0">20MHz</option>
				<option value="1">20/40MHz</option>
			</select></td>
		</tr>
		<tr id="basicHTChannelBWINIC_tr">
			<td class="head" data-tr="basic ht channel bandwidth inic">Channel BandWidth (5GHz)</td>
			<td class="val"><select id="n_bandwidthinic" name="n_bandwidthinic" class="normal" onClick="GExtChannelDisplay(this.form);">
				<option value="0">20MHz</option>
				<option value="1">20/40MHz</option>
			</select></td>
		</tr>
		<tr id="extension_channel">
			<td class="head" data-tr="basic ht extension channel">Extension Channel</td>
			<td class="val"><select id="n_extcha" name="n_extcha" class="normal">
				<option value="1" selected>2412MHz (Channel 1)</option>
			</select></td>
		</tr>
		<tr id="basicHTTxStream_tr">
			<td class="head" data-tr="basic ht txstream">HT TxStream</td>
			<td class="val"><select name="tx_stream" class="normal">
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
			<td class="head" data-tr="basic block coding">Space-Time Block Coding</td>
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
		<tr id="basicHTDelBA_tr">
			<td class="head" data-tr="basic ht delba">Decline Block ACK Request</td>
			<td class="val"><select name="n_badecline" class="normal">
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
		<tr id="basicHTRDG_tr">
			<td class="head" data-tr="basic ht rdg">Reverse Direction Grant</td>
			<td class="val"><select name="n_rdg" class="normal">
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
		<tr id="basicVHTBW_tr" >
			<td class="head" data-tr="basic ht channel bandwidth">Channel BandWidth</td>
			<td class="val"><select name="ac_bw" class="normal">
				<option value="0" selected>20/40MHz</option>
				<option value="1">20/40/80MHz</option>
			</select></td>
		</tr>
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
			<td class="head" data-tr="basic block coding">Space-Time Block Coding</td>
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
		<tr id="advMcastRate_tr">
			<td class="head" data-tr="adv mcast rate">Multicast TX mode</td>
			<td class="val"><select name="McastPhyMode" class="normal" onClick="McastSwitch(this.form);">
				<option value="2" data-tr="adv ofdm">Legacy OFDM</option>
				<option value="3" data-tr="adv htmix">HTMIX</option>
				</select>&nbsp;&nbsp;&nbsp;
				<select name="McastMcs" class="normal"></select></td>
		</tr>
		<tr id="advEDMODE_tr">
			<td class="head" data-tr="adv ed mode">Clear-Channel Assessment Monitor</td>
			<td class="val"><select name="ED_MODE" class="normal">
				<option value="0" selected data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advStaRegion_tr">
			<td class="head" data-tr="staadv region">Region settings</td>
			<td class="val"><select id="country_region" name="country_region" class="normal">
				<option value="0">0: CH1-11 (FCC)</option>
				<option value="1">1: CH1-13 (IC)</option>
				<option value="2">2: CH10-11 (ETSI)</option>
				<option value="3">3: CH10-13 (SPAIN)</option>
				<option value="4">4: CH14 (France)</option>
				<option value="5">5: CH1-14 (MKK)</option>
				<option value="6">6: CH3-9 (MKK1)</option>
				<option value="7">7: CH5-13 (Israel)</option>
				</select>&nbsp;&nbsp;&nbsp;
				<select name="country_code" class="normal" onChange="countryCodeChange();"><% listCountryCodes(); %>
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
				<span class="range" data-tr="basic roaming rssi low delay range"> 0 - 200 seconds, default 5 </span></td>
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
			<td class="val"><select name="FtSupport" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="advMaxStaNum_tr">
			<td class="head" data-tr="adv maximum stations number">Maximum clients per SSID</td>
			<td class="val"><input type="text" name="maxstanum" class="normal" maxlength="3" value="">
				<span class="range" data-tr="adv maximum stations range">num (1 - <% getMaxStaNum(); %>)</span></td>
		</tr>
		<tr id="advIdleTimeout_tr">
			<td class="head" data-tr="adv idletimeout">IdleTimeout</td>
			<td class="val"><input type="text" name="idletimeout" class="normal" maxlength="3" value="">
				<span class="range" data-tr="adv idletimeout range">sec (60 - 600)</span></td>
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

	<table id="div_txbf" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr>
			<td class="title" width="50%" data-tr="basic txbf">Beamforming</td>
			<td class="title" width="50%" style="text-align:right">
				<select name="ITxBfEn" class="half" onChange="updateVisibility(this.form);">
					<option value="0" data-tr="button disable">Disable</option>
					<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
	</thead>
	<tbody>
		<tr id="div_ETxBfeeEn" style="display:none;">
			<td class="head" data-tr="basic etxbfeeen">BFee feature</td>
			<td class="val"><select name="ETxBfeeEn" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
		<tr id="div_ETxBfEnCond" style="display:none;">
			<td class="head" data-tr="basic etxbfencond">Sending of sounding and beamforming</td>
			<td class="val"><select name="ETxBfEnCond" class="normal">
				<option value="0" data-tr="button disable">Disable</option>
				<option value="1" data-tr="button enable">Enable</option>
			</select></td>
		</tr>
	</tbody>
	</table>
	<table id="div_bandsteering" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr id="row_BndStrg">
			<td class="title" width="50%" data-tr="basic bandsteering">Band steering</td>
			<td class="title" width="50%" style="text-align:right">
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
			<td class="head" data-tr="basic bandsteering rssidiff">Allow fallback to 2.4GHz if bands RSSI diff bigger</td>
			<td class="val"><input type="text" name="BndStrgRssiDiff" class="normal" maxlength="2">
				<span class="range" data-tr="basic bandsteering rssidiff default">0 - 40 db, default 15</span></td>
		</tr>
		<tr id="row_BndStrgRssiLow" style="display:none;">
			<td class="head" data-tr="basic bandsteering rssilow">Force fallback to 2.4GHz if rssi smaller</td>
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
				<span class="range" data-tr="basic bandsteering hold default">ms, default 3000</span></td>
		</tr>
		<tr id="row_BndStrgCheckTime" style="display:none;">
			<td class="head" data-tr="basic bandsteering check">Time for deciding if a client is 2.4G only</td>
			<td class="val"><input type="text" name="BndStrgCheckTime" class="normal" maxlength="4">
				<span class="range" data-tr="basic bandsteering check default">ms, default 5000</span></td>
		</tr>
	</tbody>
	</table>
	<table id="div_ids" class="form auth-hide-user" style="display:none;">
	<thead>
		<tr>
			<td class="title" width="50%" data-tr="basic ids">Intrusion Detection settings</td>
			<td class="title" width="50%" style="text-align:right">
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
			<td class="head" data-tr="basic ids reassoc">Reassociation request</td>
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
	</form>
	<div class="whitespace"></div></td>
	</tr>
</table>
</body>
</html>
