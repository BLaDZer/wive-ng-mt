<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<title>Basic Wireless Settings</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");

var radio_off = "<% getCfgZero(1, "RadioOff"); %>";
var wmode = "<% getCfgZero(1, "WirelessMode"); %>";
var wmodeac = "<% getCfgZero(1, "WirelessModeINIC"); %>";

var channel_index  = '<% getWlanChannel(); %>';
var channel_indexac  = '<% getWlanChannelAC(); %>';

var mbssidapisolated = '<% getCfgZero(1, "NoForwardingBTNBSSID"); %>';
var fxtxmode = '<% getCfgGeneral(1, "FixedTxMode"); %>';
var countrycode = '<% getCfgGeneral(1, "CountryCode"); %>';

var tx_stream_idx = '<% getCfgZero(1, "HT_TxStream"); %>';
var rx_stream_idx = '<% getCfgZero(1, "HT_RxStream"); %>';

var ht_mode = '<% getCfgZero(1, "HT_OpMode"); %>';
var ht_gi = '<% getCfgZero(1, "HT_GI"); %>';
var ht_stbc = '<% getCfgZero(1, "HT_STBC"); %>';
var ht_mcs = '<% getCfgZero(1, "HT_MCS"); %>';
var ht_htc = '<% getCfgZero(1, "HT_HTC"); %>';
var ht_rdg = '<% getCfgZero(1, "HT_RDG"); %>';
var ht_extcha = '<% getCfgZero(1, "HT_EXTCHA"); %>';
var ht_amsdu = '<% getCfgZero(1, "HT_AMSDU"); %>';
var ht_autoba = '<% getCfgZero(1, "HT_AutoBA"); %>';
var ht_badecline = '<% getCfgZero(1, "HT_BADecline"); %>';
var ht_bw = '<% getCfgZero(1, "HT_BW"); %>';

var vht_gi = '<% getCfgZero(1, "VHT_SGI"); %>';
var vht_stbc = '<% getCfgZero(1, "VHT_STBC"); %>';
var vht_ldpc = '<% getCfgZero(1, "VHT_LDPC"); %>';
var vht_bw = '<% getCfgGeneral(1, "VHT_BW"); %>';
var vht_bwsig = '<% getCfgGeneral(1, "VHT_BW_SIGNAL"); %>';

var is3t3r = '<% is3t3r(); %>';
var is5gh_support = '<% is5gh_support(); %>';
var mssidb = '<% getMBSSIDBuilt(); %>';
var green_on = '<% getGreenAPBuilt(); %>' == '1';

var mbss_params =
[
	<% dumpBSSKeys(); %>
];

var ChannelList_24G =
[
	"2412MHz (Channel 1)",
	"2417MHz (Channel 2)",
	"2422MHz (Channel 3)",
	"2427MHz (Channel 4)",
	"2432MHz (Channel 5)",
	"2437MHz (Channel 6)",
	"2442MHz (Channel 7)",
	"2447MHz (Channel 8)",
	"2452MHz (Channel 9)",
	"2457MHz (Channel 10)",
	"2462MHz (Channel 11)",
	"2467MHz (Channel 12)",
	"2472MHz (Channel 13)",
	"2484MHz (Channel 14)"
];

function addOption(list, text, value)
{
	var option = new Option(text, value);

	try
	{
		list.add(option, null); // standards compliant
	}
	catch(ex)
	{
		list.add(option); // IE only
	}
}

function insertExtChannelOption()
{
	var wmode = document.wireless_basic.wirelessmode.value * 1;
	var option_length;

	if (wmode >= 5)
	{
		var x = document.getElementById('n_extcha');
		var tmp_value = x.value; // Store value
		x.options.length = 0;

		var CurrentCh = document.wireless_basic.sz11gChannel.value * 1;
		var option_length = document.wireless_basic.sz11gChannel.options.length;

		if ((CurrentCh >= 1) && (CurrentCh <= 4))
			addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		else if ((CurrentCh >= 5) && (CurrentCh <= 7))
		{
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
			addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		}
		else if ((CurrentCh >= 8) && (CurrentCh <= 9))
		{
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);

			if (option_length >= 14)
				addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		}
		else if (CurrentCh == 10)
		{
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);

			if (option_length > 14)
				addOption(x, ChannelList_24G[CurrentCh + 4 - 1], 1);
		}
		else if (CurrentCh >= 11)
			addOption(x, ChannelList_24G[CurrentCh - 4 - 1], 0);
		else
			addOption(x, "Auto Select", 0);

		x.value = tmp_value;
	}
}

function ChannelOnChange()
{
	if (document.wireless_basic.n_bandwidth[1].checked)
	{
		var w_mode = document.wireless_basic.wirelessmode.value;

		if ((1*w_mode == 6) || (1*w_mode == 7))
			displayElement('extension_channel', document.wireless_basic.n_bandwidth[1].checked);
		else if ((1*w_mode == 3) || (1*w_mode == 4))
		{
			displayElement('extension_channel', document.wireless_basic.n_bandwidth[1].checked);

			if (document.wireless_basic.sz11gChannel.options.selectedIndex == 0)
				hideElement('extension_channel');
		}
	}

	// add subchannel
	insertExtChannelOption();
}

function Channel_BandWidth_onClick()
{
	var form_basic = document.wireless_basic;
	var w_mode = form_basic.wirelessmode.value;

	if (form_basic.n_bandwidth[0].checked)
	{
		hideElement("extension_channel");
		form_basic.n_extcha.disabled = true;
	} else {
		showElementEx("extension_channel", style_display_on());
		form_basic.n_extcha.disabled = false;
	}
}

function ssidDisplay(form)
{
	var no_mssidb = mssidb != '1';
	var count = form.bssid_num.value * 1;

	for (var i=1; i < count; i++)
		showElement('div_hssid' + i);

	// Allow only 8 BSSID's
	showElement(form.addBSSIDbtn, no_mssidb);
	form.addBSSIDbtn.disabled = ((count >= 8) || (no_mssidb));
}

function ssidAdd(form)
{
	var count = form.bssid_num.value * 1;
	if (count < 8)
	{
		showElement('div_hssid' + count);
		enableElements(form.addBSSIDbtn, ((++count) < 8));
		form.bssid_num.value = count;
		form.elements['mssid_' + count].value = '';
		form.hssid[count-1].checked = false;
		form.isolated_ssid[count-1].checked = false;

		// Clear values
		for (var p=0; p<mbss_params.length; p++)
			form.elements[mbss_params[p] + count].value = '';
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

			for (var p=0; p<mbss_params.length; p++)
				form.elements[mbss_params[p] + (i + 1)].value = 
					form.elements[mbss_params[p] + (i + 2)].value;
		}
	}

	// Clear values
	form.elements['mssid_' + (count+1)].value = '';
	form.hssid[count].checked = false;
	form.isolated_ssid[count].checked = false;
	for (var p=0; p<mbss_params.length; p++)
		form.elements[mbss_params[p] + (count + 1)].value = '';

	hideElement('div_hssid' + count);
	form.bssid_num.value = count;

	// Enable controls
	enableElements(form.addBSSIDbtn, (count < 8));
}

function initTranslation()
{
	_TR("basicTitle", "basic title");
	_TR("basicIntroduction", "basic introduction");

	_TR("basicWirelessNet", "basic wireless network");
	_TR("basicWirelessSettings", "basic wireless settings");
	_TR("basicRadioButton", "basic radio button");
	_TR("basicAcNetMode", "basic ac network mode");
	_TR("basicNetMode", "basic network mode");
	_TR("basicSSID", "basic ssid");
	_TR("basicAcSSID", "basic ac ssid");
	_TR("basicHSSID0", "basic hssid");
	_TR("basicHSSID1", "basic hssid");
	_TR("basicHSSID2", "basic hssid");
	_TR("basicHSSID3", "basic hssid");
	_TR("basicHSSID4", "basic hssid");
	_TR("basicHSSID5", "basic hssid");
	_TR("basicHSSID6", "basic hssid");
	_TR("basicHSSID7", "basic hssid");
	_TR("basicMSSID1", "basic multiple ssid");
	_TR("basicMSSID2", "basic multiple ssid");
	_TR("basicMSSID3", "basic multiple ssid");
	_TR("basicMSSID4", "basic multiple ssid");
	_TR("basicMSSID5", "basic multiple ssid");
	_TR("basicMSSID6", "basic multiple ssid");
	_TR("basicMSSID7", "basic multiple ssid");
	_TR("basicBroadcastSSIDEnable", "wireless enable");
	_TR("basicBroadcastSSIDDisable", "wireless disable");
	_TR("basicBroadcastSSID", "basic broadcast ssid");
	_TR("basicApIsolatedEnable", "wireless enable");
	_TR("basicApIsolatedDisable", "wireless disable");
	_TR("basicApIsolated", "basic apisolated");
	_TR("basicIsolatedSSID0", "basic isolated");
	_TR("basicIsolatedSSID1", "basic isolated");
	_TR("basicIsolatedSSID2", "basic isolated");
	_TR("basicIsolatedSSID3", "basic isolated");
	_TR("basicIsolatedSSID4", "basic isolated");
	_TR("basicIsolatedSSID5", "basic isolated");
	_TR("basicIsolatedSSID6", "basic isolated");
	_TR("basicIsolatedSSID7", "basic isolated");
	_TR("basicMBSSIDApIsolatedEnable", "wireless enable");
	_TR("basicMBSSIDApIsolatedDisable", "wireless disable");
	_TR("basicMBSSIDApIsolated", "basic mbssidapisolated");

	_TR("basicBSSID", "basic bssid");
	_TR("basicAcBSSID", "basic ac bssid");

	_TR("basicFreqA", "basic frequency ac");
	_TR("basicFreqAAuto", "basic frequency auto");
	_TR("basicFreqG", "basic frequency");
	_TR("basicFreqGAuto", "basic frequency auto");
	_TR("basicRate", "basic rate");

	_TR("basicHTPhyMode", "basic ht phy mode");
	_TR("basicHTOPMode", "basic ht op mode");
	_TR("basicHTMixedDisable", "basic ht op mixed");
	_TR("basicHTMixedEnable", "basic ht op green");
	_TR("basicHTChannelBW", "basic ht channel bandwidth");
	_TR("basicHTSTBC", "basic stbc");
	_TR("basicHTSTBCDisable", "wireless disable");
	_TR("basicHTSTBCEnable", "wireless enable");
	_TR("basicHTGI", "basic ht guard interval");
	_TR("basicHTLongGI", "wireless long");
	_TR("basicHTAutoGI", "wireless auto");
	_TR("basicHTAutoMCS", "wireless auto");
	_TR("basicHTRDG", "basic ht rdg");
	_TR("basicHTRDGDisable", "wireless disable");
	_TR("basicHTRDGEnable", "wireless enable");
	_TR("basicHTExtChannel", "basic ht extension channel");
	_TR("basicHTAMSDU", "basic ht amsdu");
	_TR("basicHTAMSDUDisable", "wireless disable");
	_TR("basicHTAMSDUEnable", "wireless enable");
	_TR("basicHTAddBA", "basic ht addba");
	_TR("basicHTAddBADisable", "wireless disable");
	_TR("basicHTAddBAEnable", "wireless enable");
	_TR("basicHTDelBA", "basic ht delba");
	_TR("basicHTDelBADisable", "wireless disable");
	_TR("basicHTDelBAEnable", "wireless enable");

	_TR("basicOther", "basic other");
	_TR("basicHTTxStream", "basic ht txstream");
	_TR("basicHTRxStream", "basic ht rxstream");

	_TR("basicVHTGI", "basic ht guard interval");
	_TR("basicVHTLongGI", "wireless long");
	_TR("basicVHTAutoGI", "wireless auto");
	_TR("basicSTBC", "basic stbc");
	_TR("basicSTBCDisable", "wireless disable");
	_TR("basicSTBCEnable", "wireless enable");
	_TR("basicLDPC", "basic ldpc");
	_TR("basicLDPCEnable", "wireless enable");
	_TR("basicLDPCEnable", "wireless enable");

	_TRV("basicApply", "wireless apply");
	_TRV("basicCancel", "wireless cancel");
}

var channel_list = [ 'sz11aChannel', 'sz11gChannel' ];
var channels = [];

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

function initValue()
{
	var ssidArray;
	var HiddenSSIDArray;
	var current_channel_length;

	saveRadioChannels();
	initTranslation();

	if (countrycode == '')
		countrycode = 'NONE';

	var form = document.wireless_basic;

	form.radioWirelessEnabled.checked = (radio_off == "0");

	// Hide & disable elements
	hideElement("div_11a_basic");
	hideElement("div_11a_name");
	hideElement("div_11a_mac");
	hideElement("div_11a_channel");
	hideElement("div_11g_channel");
	hideElement("div_ht_tx_stream");
	hideElement("div_ht_rx_stream");
	hideElement("div_abg_rate");
	hideElement("div_all");
	hideElement("div_11n");
	hideElement("div_ac");

	form.sz11aChannel.disabled = true;
	form.sz11gChannel.disabled = true;
	form.abg_rate.disabled = true;
	form.n_mode.disabled = true;
	form.n_bandwidth.disabled = true;
	form.n_rdg.disabled = true;
	form.n_gi.disabled = true;
	form.n_stbc.disabled = true;
	form.n_mcs.disabled = true;

	form.ac_gi.disabled = true;
	form.ac_stbc.disabled = true;
	form.ac_ldpc.disabled = true;

	hideElement("div_mbssidapisolated");
	form.mbssidapisolated.disabled = true;
	show14channel(true);

	// Init 2.4GHz
	index = form.wirelessmode.options.length;

	form.wirelessmode.options[index++] = new Option("11b/g mixed mode", "0");
	form.wirelessmode.options[index++] = new Option("11b only", "1");
	form.wirelessmode.options[index++] = new Option("11g only", "4");
	form.wirelessmode.options[index++] = new Option("11n only", "6");
	form.wirelessmode.options[index++] = new Option("11g/n mixed mode", "7");
	form.wirelessmode.options[index++] = new Option("11b/g/n mixed mode", "9");

	// Init 5GHz
	if (is5gh_support == 1)
	{
		indexac = form.wirelessmodeac.options.length;

		form.wirelessmodeac.options[indexac++] = new Option("11a only", "2");
		form.wirelessmodeac.options[indexac++] = new Option("11a/an mixed mode", "8");
		form.wirelessmodeac.options[indexac++] = new Option("11an only", "11");
		form.wirelessmodeac.options[indexac++] = new Option("11a/an/ac", "14");
		form.wirelessmodeac.options[indexac++] = new Option("11an/ac", "15");

	}

	// Set wmode after add all options
	form.wirelessmode.value     = wmode;
	form.wirelessmodeac.value   = wmodeac;

	// Display basic
	form.sz11gChannel.disabled = false;
	showElementEx("div_11g_channel", style_display_on());
	showElement("div_all");

	// Display AC
	if (is5gh_support == 1)
	{
		showElementEx("div_11a_basic", style_display_on());
		showElementEx("div_11a_name", style_display_on());
		showElementEx("div_11a_mac", style_display_on());

		form.sz11aChannel.disabled = false;
		showElementEx("div_11a_channel", style_display_on());
	}

        // Display HT modes
	if ((wmode*1) >= 5)
	{
		showElement("div_11n");
		displayElement('htOpModeRow', green_on);
		showElementEx("div_ht_tx_stream", style_display_on());
		showElementEx("div_ht_rx_stream", style_display_on());
		show14channel(false);

		form.n_mode.disabled = false;
		form.n_bandwidth.disabled = false;
		form.n_rdg.disabled = false;
		form.n_gi.disabled = false;
		form.n_stbc.disabled = false;
		form.n_mcs.disabled = false;
	}

        // Display VHT modes
	if ((is5gh_support == 1) && ((wmodeac*1) >= 14))
	{
		showElement("div_ac");

		form.ac_gi.disabled = false;
		form.ac_stbc.disabled = false;
		form.ac_ldpc.disabled = false;
	}

	ssidDisplay(form);

	// Initialize bssid
	var HiddenSSID  = '<% getCfgZero(1, "HideSSID"); %>';
	var HiddenSSIDArray = HiddenSSID.split(";");

	form.broadcastssid[ (HiddenSSID.indexOf("0") >= 0) ? 1 : 0 ].checked = true;
	for (i=0; i<8; i++)
		form.hssid[i].checked = (HiddenSSIDArray[i] == "1");

	var APIsolated = '<% getCfgZero(1, "NoForwarding"); %>';
	var APIsolatedArray = APIsolated.split(";");

	form.apisolated[ (APIsolated.indexOf("1") >= 0) ? 1 : 0 ].checked = true;
	for (i=0; i<8; i++)
		form.isolated_ssid[i].checked = (APIsolatedArray[i] == "1");

	if (1*ht_bw == 0)
	{
		form.n_bandwidth[0].checked = true;
		form.n_extcha.disabled      = true;
		hideElement("extension_channel");
	}
	else
	{
		form.n_bandwidth[1].checked = true;
		form.n_extcha.disabled      = false;
		showElementEx("extension_channel", style_display_on());
	}

	channel_index = 1*channel_index;
	form.sz11gChannel.options.selectedIndex = channel_index;
	current_channel_length = form.sz11gChannel.options.length;
	if ((channel_index + 1) > current_channel_length)
	    form.sz11gChannel.options.selectedIndex = 0;

	if (is5gh_support == 1)
	{
		// Calculate current channel
		var channel_indexacnew;
		if (countrycode == 'NONE')
		{
			if (channel_indexac <= 64)
			{
				channel_indexacnew = channel_indexac;
				channel_indexacnew = channel_indexacnew / 4;
				if (channel_indexac != 0)
					channel_indexacnew = channel_indexacnew - 8;
			}
			else if ((channel_indexac >= 100) && (channel_indexac <= 140))
			{
				channel_indexacnew = channel_indexac;
				channel_indexacnew = channel_indexacnew / 4;
				channel_indexacnew = channel_indexacnew - 16;
			}
			else if (channel_indexac >= 149)
			{
				channel_indexacnew = channel_indexac - 1;
				channel_indexacnew = channel_indexacnew / 4;
				channel_indexacnew = channel_indexacnew - 17;
			}
			else
			{
				channel_indexacnew = 0;
			}
		}
		else if ((countrycode == 'RU') || (countrycode == 'US') || (countrycode == 'HK') || (countrycode == 'FR') || (countrycode == 'IE'))
		{
			if (channel_indexac <= 64)
			{
				channel_indexacnew = channel_indexac;
				channel_indexacnew = channel_indexacnew / 4;
				if (channel_indexacnew != 0)
					channel_indexacnew = channel_indexacnew - 8;
			}
			else if (channel_indexac >= 149)
			{
				channel_indexacnew = channel_indexac - 1;
				channel_indexacnew = channel_indexacnew / 4;
				channel_indexacnew = channel_indexacnew - 28;
			}
			else
			{
				channel_indexacnew = 0;
			}
		}
		else if (countrycode == 'JP')
		{
			if (channel_indexac <= 48)
			{
				channel_indexacnew = channel_indexac;
				channel_indexacnew = channel_indexacnew / 4;
				if (channel_indexacnew != 0)
					channel_indexacnew = channel_indexacnew - 8;
			}
			else
			{
				channel_indexacnew = 0;
			}
		}
		else if (countrycode == 'TW')
		{
			if (channel_indexac <= 64)
			{
				channel_indexacnew = channel_indexac;
				channel_indexacnew = channel_indexacnew / 4;
				if (channel_indexacnew != 0)
					channel_indexacnew = channel_indexacnew - 12;
			}
			else if (channel_indexac >= 149)
			{
				channel_indexacnew = channel_indexac - 1;
				channel_indexacnew = channel_indexacnew / 4;
				channel_indexacnew = channel_indexacnew - 32;
			}
			else
			{
				channel_indexacnew = 0;
			}
		}
		else if (countrycode == 'CN')
		{
			if (channel_indexac >= 149)
			{
				channel_indexacnew = channel_indexac - 1;
				channel_indexacnew = channel_indexacnew / 4;
				channel_indexacnew = channel_indexacnew - 36;
			}
			else
			{
				channel_indexacnew = 0;
			}
		}
		else
		{
			channel_indexacnew = 0;
		}

		if (channel_indexac > 0)
			form.sz11aChannel.options.selectedIndex = channel_indexacnew;
		else
			form.sz11aChannel.options.selectedIndex = channel_indexac;
	}

	// add subchannel
	insertExtChannelOption();

	if (mssidb == "1")
	{
		showElementEx("div_mbssidapisolated", style_display_on());
		form.mbssidapisolated.disabled = false;
		form.mbssidapisolated[ (mbssidapisolated == "1") ? 1 : 0 ].checked = true;
	}

	show_abg_rate(form);

	if (green_on)
	{
		if (ht_mode == "1")
			form.n_mode[1].checked = true;
		else if (ht_mode == "2")
			form.n_mode[2].checked = true;
		else
			form.n_mode[0].checked = true;
	} else
		form.n_mode[0].checked = true;

	if (ht_gi == "0")
		form.n_gi[0].checked = true;
	else
		form.n_gi[1].checked = true;

	if (ht_stbc == "0")
		form.n_stbc[0].checked = true;
	else
		form.n_stbc[1].checked = true;

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
		form.n_mcs.options[mcs_length] = new Option("Auto", "33");
	}

	if (1*ht_mcs <= mcs_length-1)
		form.n_mcs.options.selectedIndex = ht_mcs;
	else if (1*ht_mcs == 32)
		form.n_mcs.options.selectedIndex = mcs_length-2;
	else if (1*ht_mcs == 33)
		form.n_mcs.options.selectedIndex = mcs_length-1;

	form.n_rdg[(ht_rdg == "0") ? 0 : 1].checked = true;

	var option_length = form.n_extcha.options.length;

	if (1*ht_extcha == 0) {
		if (option_length > 1)
			form.n_extcha.options.selectedIndex = 0;
	} else if (1*ht_extcha == 1) {
		if (option_length > 1)
			form.n_extcha.options.selectedIndex = 1;
	} else {
		form.n_extcha.options.selectedIndex = 0;
	}
	
	if ((wmode == "9") || (wmode == "6"))
	{
		if (form.sz11gChannel.options.selectedIndex == 0)
		{
			hideElement("extension_channel");
			form.n_extcha.disabled = true;
		}
	}

	form.n_amsdu [ (ht_amsdu ==  "0") ? 0 : 1 ].checked = true;
	form.n_autoba[ (ht_autoba == "0") ? 0 : 1 ].checked = true;
	form.n_badecline[ (ht_badecline == "0") ? 0 : 1].checked = true;

	if (is3t3r == "1")
	{
		form.rx_stream.options[2] = new Option("3", "3");
		form.tx_stream.options[2] = new Option("3", "3");
	}

	var txpath = '<% getCfgGeneral(1, "TXPath"); %>';
	var rxpath = '<% getCfgGeneral(1, "RXPath"); %>';
	if (txpath == "1")
	{
		hideElement("div_HtTx2Stream");
		tx_stream_idx = 1;
	} else
		showElementEx("div_HtTx2Stream", style_display_on());

	if (rxpath == "1")
	{
		hideElement("div_HtRx2Stream");
		rx_stream_idx = 1;
	} else {
		showElementEx("div_HtRx2Stream", style_display_on());
	}

	form.rx_stream.options.selectedIndex = rx_stream_idx - 1;
	form.tx_stream.options.selectedIndex = tx_stream_idx - 1;

	if (vht_gi == "0")
		form.ac_gi[0].checked = true;
	else
		form.ac_gi[1].checked = true;

	if (vht_stbc == "0")
		form.ac_stbc[0].checked = true;
	else
		form.ac_stbc[1].checked = true;

	if (vht_ldpc == "0")
		form.ac_ldpc[0].checked = true;
	else
		form.ac_ldpc[1].checked = true;

	form.ac_bw.options.selectedIndex = vht_bw;
	form.ac_bwsig.options.selectedIndex = vht_bwsig;
}

function show_abg_rate(form)
{
	var wmode = form.wirelessmode.value;

	hideElement("div_abg_rate");
	form.abg_rate.disabled = true;

	//ABG Rate
	if ((wmode == "0") || (wmode == "2") || (wmode == "4"))
	{
		ht_mcs = 1*ht_mcs;
		var abg_rate = form.abg_rate;

		abg_rate.options.length = 0;
		abg_rate.options[0] = new Option("Auto", "0");
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
		abg_rate.options[0] = new Option("Auto", "0");
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
	else
	{
		hideElement("div_abg_rate");
		form.abg_rate.disabled = true;
	}
}

function wirelessModeChange(form)
{
	hideElement("div_11a_channel");
	hideElement("div_11g_channel");
	hideElement("div_ht_tx_stream");
	hideElement("div_ht_rx_stream");
	hideElement("div_abg_rate");
	hideElement("div_all");
	hideElement("div_11n");
	hideElement("div_ac");
	show14channel(true);

	form.sz11aChannel.disabled = true;
	form.sz11gChannel.disabled = true;
	form.abg_rate.disabled = true;
	form.n_mode.disabled = true;
	form.n_bandwidth.disabled = true;
	form.n_rdg.disabled = true;
	form.n_gi.disabled = true;
	form.n_stbc.disabled = true;
	form.n_mcs.disabled = true;

	form.ac_gi.disabled = true;
	form.ac_stbc.disabled = true;
	form.ac_ldpc.disabled = true;

	// Hide & disable elements
	var wmode = form.wirelessmode.value;
	var wmodeac = form.wirelessmodeac.value;

	// Display basic
	showElement("div_all");
	form.sz11gChannel.disabled = false;
	showElementEx("div_11g_channel", style_display_on());

	insertExtChannelOption();
	displayElement("extension_channel", form.sz11gChannel.options.selectedIndex != 0);

        // Display HT modes
	if ((wmode*1) >= 5)
	{
		showElement("div_11n");
		displayElement('htOpModeRow', green_on);

		form.n_mode.disabled = false;
		form.n_bandwidth.disabled = false;
		form.n_rdg.disabled = false;
		form.n_gi.disabled = false;
		form.n_stbc.disabled = false;
		form.n_mcs.disabled = false;

		showElementEx("div_ht_tx_stream", style_display_on());
		showElementEx("div_ht_rx_stream", style_display_on());
		show14channel(false);
	}

	if (is5gh_support == 1)
	{
		form.sz11aChannel.disabled = false;
		showElementEx("div_11a_channel", style_display_on());

    		// Display VHT modes
    		if ((1*wmodeac) >= 14)
    		{
		    showElement("div_ac");

		    form.ac_gi.disabled = false;
		    form.ac_stbc.disabled = false;
		    form.ac_ldpc.disabled = false;
		}
	}

	show_abg_rate(form);
}

function switch_hidden_ssid()
{
	for (i=0; i<8; i++)
		document.wireless_basic.hssid[i].checked = document.wireless_basic.broadcastssid[0].checked;
}

function switch_isolated_ssid()
{
	for (i=0; i<8; i++)
		document.wireless_basic.isolated_ssid[i].checked = document.wireless_basic.apisolated[1].checked;
}

function CheckValue(form)
{
	return true;
}

</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="basicTitle">SSID Settings </h1>
      <p id="basicIntroduction"> Here you can configure the most basic settings of Wireless communication, such as Network Name (SSID) and Channel.
				These settings are sufficient to have a working Access Point. </p>
      <hr>
      <form method="POST" name="wireless_basic" action="/goform/wirelessBasic" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="basicWirelessNet">Wireless Network</td>
          </tr>
          <tr>
            <td class="head">Wireless enable</td>
            <td><input type="checkbox" name="radioWirelessEnabled">&nbsp;enabled</td>
          </tr>
          <tr id="div_11a_basic" name="div_11a_basic">
            <td class="head" id="basicAcNetMode">Network Mode (5GHz)</td>
            <td><select name="wirelessmodeac" id="wirelessmodeac" class="mid" onChange="wirelessModeChange(this.form);">
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicNetMode">Network Mode (2.4GHz)</td>
            <td><select name="wirelessmode" id="wirelessmode" class="mid" onChange="wirelessModeChange(this.form);">
              </select></td>
          </tr>
          <tr id="div_11a_channel" name="div_11a_channel">
            <td class="head"><font id="basicFreqA">Channel (5GHz)</font></td>
            <td><select id="sz11aChannel" name="sz11aChannel" class="mid" onChange="ChannelOnChange()">
                <option value="0" id="basicFreqAAuto">AutoSelect</option>
                <% getWlan11aChannels(); %>
              </select></td>
          </tr>
          <tr id="div_11g_channel" name="div_11g_channel">
            <td class="head"><font id="basicFreqG">Channel (2.4GHz)</font></td>
            <td><select id="sz11gChannel" name="sz11gChannel" class="mid" onChange="ChannelOnChange()">
                <option value="0" id="basicFreqGAuto">AutoSelect</option>
                <% getWlan11gChannels(); %>
              </select></td>
          </tr>
          <tr id="div_11a_name" name="div_11a_name">
            <td class="head" id="basicAcSSID">Network Name (5GHz)</td>
            <td><input class="mid" type="text" name="mssidac_1" maxlength="32" value="<% getCfgGeneral(1, "SSID1INIC"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="basicSSID">Network Name (2.4GHz)</td>
            <td><input type="hidden" name="bssid_num" value="<% getCfgGeneral(1, "BssidNum"); %>">
              <input class="mid" type="text" name="mssid_1" maxlength="32" value="<% getCfgGeneral(1, "SSID1"); %>">
              <font id="basicHSSID0">Hidden</font>
              <input type="checkbox" name="hssid" value="0">
              &nbsp; <font id="basicIsolatedSSID0">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="0">
              <input type="button" onClick="ssidAdd(this.form);" class="normal" name="addBSSIDbtn" value="Add BSSID">
              <% dumpBSS(0); %></td>
          </tr>
          <tr id="div_hssid1" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID1">Multiple SSID</font>1</td>
            <td><input class="mid" name="mssid_2" maxlength="32" value="<% getCfgGeneral(1, "SSID2"); %>">
              <font id="basicHSSID1">Hidden</font>
              <input type=checkbox name=hssid value="1">
              &nbsp; <font id="basicIsolatedSSID1">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="1">
              <input type="button" onClick="ssidRemove(this.form, 1);" class="normal" value="Remove">
              <% dumpBSS(1); %></td>
          </tr>
          <tr id="div_hssid2" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID2">Multiple SSID</font>2</td>
            <td><input class="mid" type="text" name="mssid_3" maxlength="32" value="<% getCfgGeneral(1, "SSID3"); %>">
              <font id="basicHSSID2">Hidden</font>
              <input type="checkbox" name="hssid" value="2">
              &nbsp; <font id="basicIsolatedSSID2">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="2">
              <input type="button" onClick="ssidRemove(this.form, 2);" class="normal" value="Remove">
              <% dumpBSS(2); %></td>
          </tr>
          <tr id="div_hssid3" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID3">Multiple SSID</font>3</td>
            <td><input class="mid" type="text" name="mssid_4" maxlength="32" value="<% getCfgGeneral(1, "SSID4"); %>">
              <font id="basicHSSID3">Hidden</font>
              <input type="checkbox" name="hssid" value="3">
              &nbsp; <font id="basicIsolatedSSID3">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="3">
              <input type="button" onClick="ssidRemove(this.form, 3);" class="normal" value="Remove">
              <% dumpBSS(3); %></td>
          </tr>
          <tr id="div_hssid4" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID4">Multiple SSID</font>4</td>
            <td><input class="mid" type="text" name="mssid_5" maxlength="32" value="<% getCfgGeneral(1, "SSID5"); %>">
              <font id="basicHSSID4">Hidden</font>
              <input type="checkbox" name="hssid" value="4">
              &nbsp; <font id="basicIsolatedSSID4">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="4">
              <input type="button" onClick="ssidRemove(this.form, 4);" class="normal" value="Remove">
              <% dumpBSS(4); %></td>
          </tr>
          <tr id="div_hssid5" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID5">Multiple SSID</font>5</td>
            <td><input class="mid" type="text" name="mssid_6" maxlength="32" value="<% getCfgGeneral(1, "SSID6"); %>">
              <font id="basicHSSID5">Hidden</font>
              <input type="checkbox" name="hssid" value="5">
              &nbsp; <font id="basicIsolatedSSID5">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="5">
              <input type="button" onClick="ssidRemove(this.form, 5);" class="normal" value="Remove">
              <% dumpBSS(5); %></td>
          </tr>
          <tr id="div_hssid6" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID6">Multiple SSID</font>6</td>
            <td><input class="mid" type="text" name="mssid_7" maxlength="32" value="<% getCfgGeneral(1, "SSID7"); %>">
              <font id="basicHSSID6">Hidden</font>
              <input type="checkbox" name="hssid" value="6">
              &nbsp; <font id="basicIsolatedSSID6">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="6">
              <input type="button" onClick="ssidRemove(this.form, 6);" class="normal" value="Remove">
              <% dumpBSS(6); %></td>
          </tr>
          <tr id="div_hssid7" style="display:none;">
            <td class="head">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<font id="basicMSSID7">Multiple SSID</font>7</td>
            <td><input class="mid" type="text" name="mssid_8" maxlength="32" value="<% getCfgGeneral(1, "SSID8"); %>">
              <font id="basicHSSID7">Hidden</font>
              <input type="checkbox" name="hssid" value="7">
              &nbsp; <font id="basicIsolatedSSID7">Isolated</font>
              <input type="checkbox" name="isolated_ssid" value="7">
              <input type="button" onClick="ssidRemove(this.form, 7);" class="normal" value="Remove">
              <% dumpBSS(7); %></td>
          </tr>
        </table>
        <table id="div_all" name="div_all" class="form" style="display:none;">
          <tr>
            <td class="title" colspan="2" id="basicWirelessSettings">Basic Settings</td>
          </tr>
          <tr id="div_11a_mac" name="div_11a_mac">
            <td class="head" id="basicAcBSSID">BSSID (5GHz)</td>
            <td><% getWlanCurrentMacAC(); %></td>
          </tr>
          <tr>
            <td class="head" id="basicBSSID">BSSID (2.4GHz)</td>
            <td><% getWlanCurrentMac(); %></td>
          </tr>
          <tr>
            <td class="head" id="basicBroadcastSSID">Broadcast Network Name</td>
            <td><span class="radio">
              <input type="radio" name="broadcastssid" value="0" onClick="switch_hidden_ssid()">
              <font id="basicBroadcastSSIDDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="broadcastssid" value="1" checked onClick="switch_hidden_ssid()">
              <font id="basicBroadcastSSIDEnable">Enable</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicApIsolated">AP Isolation</td>
            <td><span class="radio">
              <input type="radio" name="apisolated" value="0" checked onClick="switch_isolated_ssid()">
              <font id="basicApIsolatedDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="apisolated" value="1" onClick="switch_isolated_ssid()">
              <font id="basicApIsolatedEnable">Enable</font></span></td>
          </tr>
          <tr id="div_mbssidapisolated">
            <td class="head" id="basicMBSSIDApIsolated">MBSSID AP Isolation</td>
            <td><span class="radio">
              <input type="radio" name="mbssidapisolated" value="0" checked>
              <font id="basicMBSSIDApIsolatedDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="mbssidapisolated" value="1">
              <font id="basicMBSSIDApIsolatedEnable">Enable</font></span></td>
          </tr>
          <tr id="div_abg_rate">
            <td class="head"><font id="basicRate">Rate</font></td>
            <td><select name="abg_rate" class="half">
              </select></td>
          </tr>
        </table>
        <table id="div_11n" name="div_11n" class="form" style="display:none;">
          <tr>
            <td class="title" colspan="2" id="basicHTPhyMode">HT Physical Mode</td>
          </tr>
          <tr>
            <td class="head" id="basicHTChannelBW">Channel BandWidth</td>
            <td><span class="radio">
              <input type="radio" name="n_bandwidth" value="0" onClick="Channel_BandWidth_onClick()" checked>20MHz</span> <span class="radio">
              <input type="radio" name="n_bandwidth" value="1" onClick="Channel_BandWidth_onClick()">20/40MHz</span></td>
          </tr>
          <tr name="extension_channel" id="extension_channel">
            <td class="head" id="basicHTExtChannel">Extension Channel</td>
            <td><select id="n_extcha" name="n_extcha" class="mid">
                <option value="1" selected>2412MHz (Channel 1)</option>
              </select></td>
          </tr>
          <tr id="div_ht_tx_stream">
            <td class="head" id="basicHTTxStream">HT TxStream</td>
            <td><select name="tx_stream" class="half">
                <option value="1">1</option>
                <option value="2" id="div_HtTx2Stream">2</option>
              </select></td>
          </tr>
          <tr id="div_ht_rx_stream">
            <td class="head" id="basicHTRxStream">HT RxStream</td>
            <td><select name="rx_stream" class="half">
                <option value="1">1</option>
                <option value="2" id="div_HtRx2Stream">2</option>
              </select></td>
          </tr>
          <tr>
            <td class="head">MCS</td>
            <td><select name="n_mcs" class="half">
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
                <option value = "33" selected id="basicHTAutoMCS">Auto</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicHTGI">Guard Interval</td>
            <td><span class="radio">
              <input type="radio" name="n_gi" value="0" checked>
              <font id="basicHTLongGI">Long</font></span> <span class="radio">
              <input type="radio" name="n_gi" value="1">
              <font id="basicHTAutoGI">Auto</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicHSTBC">Space-Time Block Coding</td>
            <td><span class="radio">
              <input type="radio" name="n_stbc" value="0" checked>
              <font id="basicHTSTBCDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="n_stbc" value="1">
              <font id="basicHTSTBCEnable">Enable</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicHTAMSDU">Aggregation MSDU</td>
            <td><span class="radio">
              <input type="radio" name="n_amsdu" value="0" checked>
              <font id="basicHTAMSDUDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="n_amsdu" value="1">
              <font id="basicHTAMSDUEnable">Enable</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicHTAddBA">Auto Block ACK</td>
            <td><span class="radio">
              <input type="radio" name="n_autoba" value="0" checked>
              <font id="basicHTAddBADisable">Disable</font></span> <span class="radio">
              <input type="radio" name="n_autoba" value="1">
              <font id="basicHTAddBAEnable">Enable</font></span></td>
          </tr>
          <tr id="htOpModeRow" style="display: none;">
            <td class="head" id="basicHTOPMode">Green Filed Mode</td>
            <td><span class="radio">
              <input type="radio" name="n_mode" value="0" checked>
              <font id="basicHTMixedDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="n_mode" value="1">
              <font id="basicHTMixedEnable">Enable</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicHTRDG">Reverse Direction Grant</td>
            <td><span class="radio">
              <input type="radio" name="n_rdg" value="0" checked>
              <font id="basicHTRDGDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="n_rdg" value="1">
              <font id="basicHTRDGEnable">Enable</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicHTDelBA">Decline BA Request</td>
            <td><span class="radio">
              <input type="radio" name="n_badecline" value="0" checked>
              <font id="basicHTDelBADisable">Disable</font></span> <span class="radio">
              <input type="radio" name="n_badecline" value="1">
              <font id="basicHTDelBAEnable">Enable</font></span></td>
          </tr>
        </table>
        <table id="div_ac" name="div_ac" class="form" style="display:none;">
          <tr>
            <td class="title" colspan="2" id="basicHTPhyMode">VHT Physical Mode</td>
          </tr>
          <tr id="basicVHTBW">
            <td class="head">Channel BandWidth</td>
            <td><select name="ac_bw" size="1" class="half">
                <option value="0" selected id="1">20/40MHz</option>
                <option value="1" id="2">20/40/80MHz</option>
		</select></td>
          </tr>
          <tr id="basicVHTBWSIGNAL">
            <td class="head">BandWidth Signaling Mode</td>
            <td><select name="ac_bwsig" size="1" class="half">
                <option value="0" selected id="1">Disable</option>
                <option value="1" id="2">Static</option>
                <option value="2" id="3">Dynamic</option>
		</select></td>
          </tr>
          <tr>
            <td class="head" id="basicVHTGI">Guard Interval</td>
            <td><span class="radio">
              <input type="radio" name="ac_gi" value="0" checked>
              <font id="basicVHTLongGI">Long</font></span> <span class="radio">
              <input type="radio" name="ac_gi" value="1">
              <font id="basicVHTAutoGI">Auto</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicVHTSTBC">Space-Time Block Coding</td>
            <td><span class="radio">
              <input type="radio" name="ac_stbc" value="0" checked>
              <font id="basicVHTSTBCDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="ac_stbc" value="1">
              <font id="basicVHTSTBC">Enable</font></span></td>
          </tr>
          <tr>
            <td class="head" id="basicLDPC">Low Disenty parity check</td>
            <td><span class="radio">
              <input type="radio" name="ac_ldpc" value="0" checked>
              <font id="basicLDPCDisable">Disable</font></span> <span class="radio">
              <input type="radio" name="ac_ldpc" value="1">
              <font id="basicLDPCEnable">Enable</font></span></td>
          </tr>
        </table>
        <br>
        <table class="buttons">
          <tr align="center">
            <td><input type="submit" class="normal" value="Apply" id="basicApply">&nbsp;&nbsp;
              <input type="button" class="normal" value="Cancel" id="basicCancel" onClick="window.location.reload();">
              <input type="hidden" name="submit-url" value="/wireless/basic.asp"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
