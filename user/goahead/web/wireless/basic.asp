<!DOCTYPE html>
<html>
<head>
<title>Basic Wireless Settings</title>
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

var radio_off = "<% getCfgZero(1, "RadioOff"); %>";
var radio_off_ac = "<% getCfgZero(1, "RadioOffINIC"); %>";

var wmode = "<% getCfgZero(1, "WirelessMode"); %>";
var wmodeac = "<% getCfgZero(1, "WirelessModeINIC"); %>";

var channel_index  = '<% getWlanChannel(); %>';
var channel_indexac  = '<% getWlanChannelAC(); %>';

var txPower = '<% getCfgZero(1, "TxPower"); %>';
var txPowerAC = '<% getCfgZero(1, "TxPowerINIC"); %>';

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

var mbssid = "<% getMBSSIDBuilt(); %>";
var apcli = "<% getWlanApcliBuilt(); %>";
var wds = "<% getWlanWdsBuilt(); %>";
var mbssid_mode = '<% getCfgGeneral(1, "BssidIfName"); %>';
var apcli_mode = '<% getCfgGeneral(1, "ApCliIfName"); %>';
var wds_mode = '<% getCfgGeneral(1, "WdsIfName"); %>';

var is3t3r = '<% is3t3r(); %>';
var is5gh_support = '<% is5gh_support(); %>';
var is5gh_1t1r = '<% is5gh_1t1r(); %>';

var green_on = '<% getGreenAPBuilt(); %>' == '1';

var bssid_num = 1*'<% getBSSIDNum(); %>';

var ids_built='<% getIdsEnableBuilt(); %>' == '1';

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

function fastRoamingChange(form) {
	displayElement('div_roaming', true);
	displayElement(["row_ApProbeRspTimes", "row_AuthRspFail", "row_AuthRspRssi", "row_AssocReqRssiThres", "row_AssocRspIgnor", "row_KickStaRssiLow", "row_ProbeRspRssi"], form.FastRoaming.value == "1");
}

function idsChange(form) {
	displayElement('div_ids', ids_built);
	displayElement(["row_AuthFloodThreshold", "row_AssocReqFloodThreshold", "row_ReassocReqFloodThreshold", "row_ProbeReqFloodThreshold", "row_DisassocFloodThreshold", "row_DeauthFloodThreshold", "row_EapReqFloodThreshold"], (form.IdsEnable.value == "1") && ids_built);
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

function AutoChannelSelect(form) {
	var autoselectmode_g = 1*<% getCfgZero(1, "AutoChannelSelect"); %>;
	var autoselectmode_a = 1*<% getCfgZero(1, "AutoChannelSelectINIC"); %>;
	var checktime_g = <% getCfgZero(1, "ACSCheckTime"); %> - 1;
	var checktime_a = <% getCfgZero(1, "ACSCheckTimeINIC"); %> - 1;

	form.autoselect_g.options.selectedIndex = (autoselectmode_g <= 1) ? 0 : 1;
	form.autoselect_a.options.selectedIndex = (autoselectmode_a <= 1) ? 0 : 1;
	form.checktime_g.options.selectedIndex = (checktime_g < 0) ? 23 : checktime_g;
	form.checktime_a.options.selectedIndex = (checktime_a < 0) ? 23 : checktime_a;

	displayElement('autoselect_g', (form.sz11gChannel.options.selectedIndex == 0));
	displayElement('autoselect_a', (form.sz11aChannel.options.selectedIndex == 0));
	displayElement('checktime_g', (form.sz11gChannel.options.selectedIndex == 0));
	displayElement('checktime_a', (form.sz11aChannel.options.selectedIndex == 0));
	displayElement('basicRescanG', (form.sz11gChannel.options.selectedIndex == 0));
	displayElement('basicRescanA', (form.sz11aChannel.options.selectedIndex == 0));

	if (form.sz11gChannel.options.selectedIndex == 0)
		form.sz11gChannel.style.width = "100px";
	else
		form.sz11gChannel.style.width = "170px";
	if (form.sz11aChannel.options.selectedIndex == 0)
		form.sz11aChannel.style.width = "100px";
	else
		form.sz11aChannel.style.width = "170px";
}

function GExtChannelDisplay(form) {
	var mode = form.wirelessmode.value * 1;
	var channel = form.sz11gChannel.value * 1;
	var bandwidth = form.n_bandwidth.value * 1;

	displayElement('extension_channel', (channel != 0) && (bandwidth != 0) && (mode >= 5));
	enableElements(('form.n_extcha'), (channel != 0) && (bandwidth != 0) && (mode >= 5));
}

function initChecktime(form) {
	for (var hour = 1; hour <= 24; hour++) {
		form.checktime_a.options[hour-1] = new Option(_("basic rescan every") + hour + _("basic hour"), hour);
		form.checktime_g.options[hour-1] = new Option(_("basic rescan every") + hour + _("basic hour"), hour);
	}
}

function ChannelOnChange(form)
{
	var w_mode = form.wirelessmode.value;
	// add subchannel
	insertExtChannelOption(form);
	GExtChannelDisplay(form);
	AutoChannelSelect(form);
}

function ssidDisplay(form)
{
	var no_mbssid = mbssid != '1';
	var count = form.bssid_num.value * 1;

	for (var i=1; i < count; i++)
		showElement('div_hssid' + i);

	// Allow only 4 BSSID's
	showElement(form.addBSSIDbtn, no_mbssid);
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
		}
	}

	// Clear values
	form.elements['mssid_' + (count+1)].value = '';
	form.hssid[count].checked = false;
	form.isolated_ssid[count].checked = false;

	hideElement('div_hssid' + count);
	form.bssid_num.value = count;

	// Enable controls
	enableElements(form.addBSSIDbtn, (count < bssid_num));
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
	_TR("basicHSSID", "basic hssid");
	_TR("basicNetworkName", "basic network name");
	_TR("basicBroadcastSSID", "basic broadcast ssid");
	_TR("basicIsolatedSSID", "basic isolated");
	_TR("basicIsolatedSSID1", "basic isolated");
	_TR("basicClientsSSID", "basic clients");
	_TR("basicMBSSIDApIsolated", "basic mbssidapisolated");
	_TR("basicMBSSIDMode", "basic mbssid mode");
	_TR("basicWDSMode", "basic wds mode");
	_TR("basicAPCLIMode", "basic apcli mode");

	_TR("basicFreqA", "basic frequency ac");
	_TR("basicFreqAAuto", "basic frequency auto");
	_TR("basicFreqG", "basic frequency");
	_TR("basicFreqGAuto", "basic frequency auto");
	_TR("basicRate", "basic rate");

	_TR("basicTxPW", "basic tx power");
	_TR("basicTxPWAc", "basic tx power ac");

	_TR("basicHTPhyMode", "basic ht phy mode");
	_TR("basicHTOPMode", "basic ht op mode");
	_TR("basicHTMixedDisable", "basic ht op mixed");
	_TR("basicHTMixedEnable", "basic ht op green");
	_TR("basicHTChannelBW", "basic ht channel bandwidth");
	_TR("basicHTSTBC", "basic stbc");
	_TR("basicHTGI", "basic ht guard interval");
	_TR("basicHTLongGI", "wireless long");
	_TR("basicHTAutoGI", "wireless auto");
	_TR("basicHTAutoMCS", "wireless auto");
	_TR("basicHTRDG", "basic ht rdg");
	_TR("basicHTExtChannel", "basic ht extension channel");
	_TR("basicHTAMSDU", "basic ht amsdu");
	_TR("basicHTAddBA", "basic ht addba");
	_TR("basicHTDelBA", "basic ht delba");
	_TR("basicHSTBC", "basic block coding");

	_TR("basicOther", "basic other");
	_TR("basicHTTxStream", "basic ht txstream");
	_TR("basicHTRxStream", "basic ht rxstream");

	_TR("basicVHTPhyMode", "basic vht phy mode");
	_TR("basicVHTGI", "basic ht guard interval");
	_TR("basicVHTLongGI", "wireless long");
	_TR("basicVHTAutoGI", "wireless auto");
	_TR("basicVHTBandWidth", "basic ht channel bandwidth");
	_TR("basicVHTSTBC", "basic block coding");
	_TR("basicVHTSignalMode", "basic signal mode");
	_TR("basicSTBC", "basic stbc");
	_TR("basicLDPC", "basic ldpc");
	_TR("basicAction", "basic action");

	_TR("basicFastRoaming", "basic roaming");
	_TR("fast_roaming", "basic roaming");
	_TR("basicApProbeRspTimes", "basic roaming probe times");
	_TR("basicAuthRspFail", "basic roaming auth fail");
	_TR("basicAuthRspRssi", "basic roaming auth rssi");
	_TR("basicAssocReqRssiThres", "basic roaming rssi thres");
	_TR("basicAssocRspIgnor", "basic roaming assoc ignor");
	_TR("basicKickStaRssiLow", "basic roaming rssi low");
	_TR("basicProbeRspRssi", "basic roaming probe rssi");
	_TR("basic80211h", "basic dot11h");

	_TR("basicIDS", "basic ids");
	_TR("ids", "basic ids");
	_TR("basicAuthFloodThreshold", "basic ids auth");
	_TR("basicAssocReqFloodThreshold", "basic ids assoc");
	_TR("basicReassocReqFloodThreshold", "basic ids reassoc");
	_TR("basicProbeReqFloodThreshold", "basic ids probe");
	_TR("basicDisassocFloodThreshold", "basic ids disassoc");
	_TR("basicDeauthFloodThreshold", "basic ids deauth");
	_TR("basicEapReqFloodThreshold", "basic ids eap");

	_TRV("basicApply", "button apply");
	_TRV("basicCancel", "button cancel");
	_TRV("basicAddBSSID", "button add");

	var elements = document.getElementsByTagName('option');
  	for (var i = 0; i < elements.length; i++)
    	if (elements[i].id == "disable")
			elements[i].innerHTML = _("button disable");
		else if (elements[i].id == "enable")
			elements[i].innerHTML = _("button enable");
		else if (elements[i].id == "basicAutoBySTA")
			elements[i].innerHTML = _("basic select by sta");
		else if (elements[i].id == "basicAutoByRSSI")
			elements[i].innerHTML = _("basic select by rssi");
	elements = document.getElementsByTagName('input');
  	for (i = 0; i < elements.length; i++)
    	if (elements[i].id == "basicRemove")
			elements[i].value = _("button remove");
	elements = document.getElementsByTagName('span');
  	for (i = 0; i < elements.length; i++)
    	if (elements[i].id == "basicMSSID")
			elements[i].innerHTML = _("basic multiple ssid");
		else if (elements[i].id == "basicAutoChannelMode")
			elements[i].innerHTML = _("basic auto channel mode");
		else if (elements[i].id == "basicChannel")
			elements[i].innerHTML = _("station channel");
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

	radio = (radio_off == "0") ? 1 : 0;
	radioac = (radio_off_ac == "0") ? 1 : 0;

	form.radioWirelessEnabled.options.selectedIndex = radio;
	form.radioWirelessEnabledAc.options.selectedIndex = radioac;

	// Hide & disable elements
	hideElement("div_11a_basic");
	hideElement("div_11a_name");
	hideElement("basicAcBSSID");
	hideElement("div_txpw_ac");
	hideElement("div_11a_channel");
	hideElement("div_11g_channel");
	hideElement("div_ht_tx_stream");
	hideElement("div_ht_rx_stream");
	hideElement("div_abg_rate");
	hideElement("div_all");
	hideElement("div_11n");
	hideElement("div_ac");
	hideElement("basicWirelessEnabledAc");
	hideElement("div_dot11h");

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

	form.wirelessmode.options[index++] = new Option(_("basic bg"), "0");
	form.wirelessmode.options[index++] = new Option(_("basic b"), "1");
	form.wirelessmode.options[index++] = new Option(_("basic g"), "4");
	form.wirelessmode.options[index++] = new Option(_("basic n"), "6");
	form.wirelessmode.options[index++] = new Option(_("basic gn"), "7");
	form.wirelessmode.options[index++] = new Option(_("basic bgn"), "9");

	// Init 5GHz
	if (is5gh_support == 1)
	{
		indexac = form.wirelessmodeac.options.length;

		form.wirelessmodeac.options[indexac++] = new Option(_("basic a"), "2");
		form.wirelessmodeac.options[indexac++] = new Option(_("basic aan"), "8");
		form.wirelessmodeac.options[indexac++] = new Option(_("basic an"), "11");
		form.wirelessmodeac.options[indexac++] = new Option(_("basic anc"), "14");
		form.wirelessmodeac.options[indexac++] = new Option(_("basic anac"), "15");

	}

        if (is5gh_support != '1' || mbssid != '1') {
	    form.mbssid_mode.options.selectedIndex = 0;
	    document.getElementById("basicMbssidModeT").style.visibility = "hidden";
	    hideElement(basicMbssidModeT);
	}

        if (is5gh_support != '1' || apcli != '1') {
	    form.wds_mode.options.selectedIndex = 0;
	    document.getElementById("basicWdsModeT").style.visibility = "hidden";
	    hideElement(basicWdsModeT);
	}

        if (is5gh_support != '1' || wds != '1') {
	    form.apcli_mode.options.selectedIndex = 0;
	    document.getElementById("basicApcliModeT").style.visibility = "hidden";
	    hideElement(basicApcliModeT);
	}

        if (wds_mode == 'wdsi')
	    form.wds_mode.options.selectedIndex = 1;
	else
	    form.wds_mode.options.selectedIndex = 0;

        if (apcli_mode == 'apclii0')
	    form.apcli_mode.options.selectedIndex = 1;
	else
	    form.apcli_mode.options.selectedIndex = 0;

	if (mbssid_mode == 'rai')
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

	// Display basic
	form.sz11gChannel.disabled = false;
	showElementEx("div_11g_channel", style_display_on());
	showElement("div_all");

	// Display AC
	if (is5gh_support == 1)
	{
		// Set-up TX power AC combo
		for (var i=0; i<form.tx_powerac.options.length; i++)
		{
			if (form.tx_powerac.options[i].value > (txPowerAC*1))
				break;
			form.tx_powerac.options.selectedIndex = i;
		}

		showElementEx("basicWirelessEnabledAc", style_display_on());

		showElementEx("div_11a_basic", style_display_on());
		showElementEx("div_11a_name", style_display_on());
		showElementEx("basicAcBSSID", style_display_on());
		showElementEx("div_txpw_ac", style_display_on());

		form.sz11aChannel.disabled = false;
		showElementEx("div_11a_channel", style_display_on());
		showElementEx("div_dot11h", style_display_on());
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

		if (is5gh_1t1r == 1)
		{
		    hideElement("div_11a_ldpc");
		}

		form.ac_gi.disabled = false;
		form.ac_stbc.disabled = false;
		form.ac_ldpc.disabled = false;
	}

	ssidDisplay(form);

	// Initialize bssid
	var HiddenSSID  = '<% getCfgZero(1, "HideSSID"); %>';
	var APIsolated = '<% getCfgZero(1, "NoForwarding"); %>';
	var NoForwardingMBCast = '<% getCfgZero(1, "NoForwardingMBCast"); %>';
	var IEEE80211H = '<% getCfgZero(1, "IEEE80211H"); %>';
	var HiddenSSIDArray = HiddenSSID.split(";");
	var APIsolatedArray = APIsolated.split(";");
	var NoForwardingMBCastArray = NoForwardingMBCast.split(";");
	var dot11hArray = IEEE80211H.split(";");

	for (i=0; i<bssid_num; i++) {
		form.hssid[i].checked = (HiddenSSIDArray[i] == "1");
		form.isolated_ssid[i].checked = (APIsolatedArray[i] == "1");
		form.mbcastisolated_ssid[i].checked = (NoForwardingMBCastArray[i] == "1");
	}

	form.n_bandwidth.options.selectedIndex = 1*ht_bw;
	form.dot11h.options.selectedIndex = 1*dot11hArray[0];
	initChecktime(form);
	GExtChannelDisplay(form);

	channel_index = 1*channel_index;
	form.sz11gChannel.options.selectedIndex = channel_index;
	current_channel_length = form.sz11gChannel.options.length;
	if ((channel_index + 1) > current_channel_length)
	    form.sz11gChannel.options.selectedIndex = 0;

	if (is5gh_support == 1)
	{
		// Calculate current channel
		var channel_indexacnew;
		if ((countrycode == 'RU') || (countrycode == 'NONE'))
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
		else if ((countrycode == 'US') || (countrycode == 'HK') || (countrycode == 'FR') || (countrycode == 'IE') || (countrycode == 'NO'))
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
	insertExtChannelOption(form);

	if (mbssid == "1")
	{
		showElementEx("div_mbssidapisolated", style_display_on());
		form.mbssidapisolated.disabled = false;
		form.mbssidapisolated.options.selectedIndex = (mbssidapisolated == "1") ? 1 : 0;
	}

	show_abg_rate(form);

	if (green_on)
	{
		if (ht_mode == "1")
			form.n_mode.options.selectedIndex = 1;
			// WTF??? How to choose the third of the two states?
			//		else if (ht_mode == "2")
			//			form.n_mode[2].checked = true;
		else
			form.n_mode.options.selectedIndex = 0;
	} else
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

	form.ac_gi.options.selectedIndex = (vht_gi ==  "0") ? 0 : 1;

	form.ac_stbc.options.selectedIndex = (vht_stbc ==  "0") ? 0 : 1;

	form.ac_ldpc.options.selectedIndex = (vht_ldpc ==  "0") ? 0 : 1;

	form.ac_bw.options.selectedIndex = vht_bw;
	form.ac_bwsig.options.selectedIndex = vht_bwsig;

	hideElement("div_auto_a");
	hideElement("div_auto_g");
	AutoChannelSelect(form);
	form.FastRoaming.options.selectedIndex = ('<% getCfgGeneral(1, "FastRoaming"); %>' ==  '1') ? 1 : 0;
	fastRoamingChange(form);
	form.IdsEnable.options.selectedIndex = ('<% getCfgGeneral(1, "IdsEnable"); %>' ==  '1') ? 1 : 0;
	idsChange(form);
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
	hideElement("basicWirelessEnabledAc");
	hideElement("div_dot11h");
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

	insertExtChannelOption(form);
	GExtChannelDisplay(form);

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
		showElementEx("basicWirelessEnabledAc", style_display_on());
		showElementEx("div_dot11h", style_display_on());

    		// Display VHT modes
    		if ((1*wmodeac) >= 14)
    		{
		    showElement("div_ac");

		    if (is5gh_1t1r == 1)
		    {
			hideElement("div_11a_ldpc");
		    }

		    form.ac_gi.disabled = false;
		    form.ac_stbc.disabled = false;
		    form.ac_ldpc.disabled = false;
		}
	}
	fastRoamingChange(form);
	show_abg_rate(form);
}

function CheckValue(form)
{
	return true;
}
</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="basicTitle">SSID Settings </h1>
      <p id="basicIntroduction"> Here you can configure the most basic settings of Wireless communication, such as Network Name (SSID) and Channel.
				These settings are sufficient to have a working Access Point. </p>
      <hr>
      <form method="POST" name="wireless_basic" action="/goform/wirelessBasic" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="3" id="basicWirelessNet">Wireless Network</td>
          </tr>
          <tr id="basicWirelessEnabledAc">
            <td class="head" colspan="1">Wireless (5GHz)</td>
	    	<td colspan="1"><select name="radioWirelessEnabledAc" class="half">
                <option value="0" id="disable">Disabled</option>
                <option value="1" id="enable">Enabled</option>
              </select></td>
            <td colspan="1">BSSID: <% getWlanCurrentMacAC(); %></td>
          </tr>
          <tr id="basicWirelessEnabled">
            <td class="head" colspan="1">Wireless (2.4GHz)</td>
            <td colspan="1"><select name="radioWirelessEnabled" class="half">
                <option value="0" id="disable">Disabled</option>
                <option value="1" id="enable">Enabled</option>
              </select></td>
            <td colspan="1">BSSID: <% getWlanCurrentMac(); %></td>
          <tr id="div_11a_basic" name="div_11a_basic">
            <td class="head" id="basicAcNetMode" colspan="1">Network Mode (5GHz)</td>
            <td colspan="2"><select name="wirelessmodeac" id="wirelessmodeac" class="mid" onChange="wirelessModeChange(this.form);">
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicNetMode" colspan="1">Network Mode (2.4GHz)</td>
            <td colspan="2"><select name="wirelessmode" id="wirelessmode" class="mid" onChange="wirelessModeChange(this.form);">
              </select></td>
          </tr>
          <tr id="div_11a_channel" name="div_11a_channel">
            <td class="head" colspan="1" id="basicFreqA">Channel (5GHz)</td>
            <td colspan="2"><select id="sz11aChannel" name="sz11aChannel" style="width: 170px;" onChange="ChannelOnChange(this.form);">
                <option value="0" id="basicFreqAAuto">AutoSelect</option>
                <% getWlan11aChannels(); %>
              </select>&nbsp;&nbsp;<select name="autoselect_a" style="width: 170px;" id="autoselect_a">
                <option value="1" id="basicAutoBySTA">by STA count</option>
                <option value="2" id="basicAutoByRSSI">by RSSI</option>
              </select>&nbsp;&nbsp;<select name="checktime_a" style="width: 170px;" id="checktime_a">
              </select></td>
          </tr>
          <tr id="div_11g_channel" name="div_11g_channel">
            <td class="head" colspan="1" id="basicFreqG">Channel (2.4GHz)</td>
            <td colspan="2"><select id="sz11gChannel" name="sz11gChannel" style="width: 170px;" onChange="ChannelOnChange(this.form);">
                <option value="0" id="basicFreqGAuto">AutoSelect</option>
                <% getWlan11gChannels(); %>
              </select>&nbsp;&nbsp;<select name="autoselect_g" style="width: 170px;" id="autoselect_g">
                <option value="1" id="basicAutoBySTA">by STA count</option>
                <option value="2" id="basicAutoByRSSI">by RSSI</option>
              </select>&nbsp;&nbsp;<select name="checktime_g" style="width: 170px;" id="checktime_g">
              </select></td>
          </tr>
          <tr id="div_txpw_ac" name="div_txpw_ac">
            <td class="head" id="basicTxPWAc" colspan="1">TX Power (5GHz)</td>
            <td colspan="2"><select name="tx_powerac" class="half">
                <option value="5">5%</option>
                <option value="10">10%</option>
                <option value="20">20%</option>
                <option value="40">40%</option>
                <option value="70">70%</option>
                <option value="100">100%</option>
              </select></td>
          </tr>
          <tr id="div_txpw" name="div_txpw">
            <td class="head" id="basicTxPW" colspan="1">TX Power (2.4GHz)</td>
            <td colspan="2"><select name="tx_power" class="half">
                <option value="5">5%</option>
                <option value="10">10%</option>
                <option value="20">20%</option>
                <option value="40">40%</option>
                <option value="70">70%</option>
                <option value="100">100%</option>
              </select></td>
          </tr>
          <tr id="basicMbssidModeT">
            <td class="head" id="basicMBSSIDMode" colspan="1">MBSSID Mode</td>
            <td colspan="2"><select name="mbssid_mode" size="1" class="half">
                <option value="ra" selected id="1">2.4GHz</option>
                <option value="rai" id="2">5GHz</option>
		</select></td>
          </tr>
          <tr id="basicWdsModeT">
            <td class="head" id="basicWDSMode" colspan="1">WDS Mode</td>
            <td colspan="2"><select name="wds_mode" size="1" class="half">
                <option value="wds" selected id="1">2.4GHz</option>
                <option value="wdsi" id="2">5GHz</option>
		</select></td>
          </tr>
          <tr id="basicApcliModeT">
            <td class="head" id="basicAPCLIMode" colspan="1">APCLI Mode</td>
            <td colspan="2"><select name="apcli_mode" size="1" class="half">
                <option value="apcli0" selected id="1">2.4GHz</option>
                <option value="apclii0" id="2">5GHz</option>
		</select></td>
          </tr>
        </table>
        <table id="div_all" name="div_all" class="form" style="display:none;">
          <tr>
            <td class="title" colspan="6" id="basicWirelessSettings">Basic Settings</td>
          </tr>
          <tr>
          	<td class="title" colspan="1"></td>
          	<td class="title" colspan="1" id="basicNetworkName">Network name</td>
          	<td class="title" colspan="1" id="basicHSSID">Hidden</td>
          	<td class="title" colspan="1"><span id="basicClientsSSID">Clients</span><br><span id="basicIsolatedSSID">Isolated</span></td>
			<td class="title" colspan="1"><span id="basicBroadcast">Broadcast</span><br><span id="basicIsolatedSSID1">Isolated</span></td>
			<td class="title" colspan="1" id="basicAction">Action</td>
          </tr>
          <tr id="div_11a_name" name="div_11a_name">
            <td class="head" id="basicAcSSID" colspan="1">Network Name (5GHz)</td>
            <td colspan="5"><input class="normal" type="text" name="mssidac_1" maxlength="32" value="<% getCfgGeneral(1, "SSID1INIC"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="basicSSID" colspan="1">Network Name (2.4GHz)</td>
            <input type="hidden" name="bssid_num" value="<% getCfgGeneral(1, "BssidNum"); %>">
            <td colspan="1"><input class="normal" type="text" name="mssid_1" maxlength="32" value="<% getCfgGeneral(1, "SSID1"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="0"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="0"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="0"></td>
            <td colspan="1"><input type="button" onClick="ssidAdd(this.form);" class="half" name="addBSSIDbtn" value="Add BSSID" id="basicAddBSSID"></td>
          </tr>
          <tr id="div_hssid1" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>1</td>
            <td colspan="1"><input class="normal" name="mssid_2" maxlength="32" value="<% getCfgGeneral(1, "SSID2"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="1"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="1"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="1"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 1);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_hssid2" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>2</td>
            <td colspan="1"><input class="normal" type="text" name="mssid_3" maxlength="32" value="<% getCfgGeneral(1, "SSID3"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="2"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="2"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="2"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 2);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_hssid3" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>3</td>
            <td colspan="1"><input class="normal" type="text" name="mssid_4" maxlength="32" value="<% getCfgGeneral(1, "SSID4"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="3"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="3"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="3"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 3);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_hssid4" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>4</td>
            <td colspan="1"><input class="normal" type="text" name="mssid_5" maxlength="32" value="<% getCfgGeneral(1, "SSID5"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="4"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="4"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="4"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 4);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_hssid5" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>5</td>
            <td colspan="1"><input class="normal" type="text" name="mssid_6" maxlength="32" value="<% getCfgGeneral(1, "SSID6"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="5"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="5"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="5"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 5);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_hssid6" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>6</td>
            <td colspan="1"><input class="normal" type="text" name="mssid_7" maxlength="32" value="<% getCfgGeneral(1, "SSID7"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="6"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="6"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="6"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 6);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_hssid7" style="display:none;">
            <td class="head" colspan="1">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<span id="basicMSSID">Multiple SSID</span>7</td>
            <td colspan="1"><input class="normal" type="text" name="mssid_8" maxlength="32" value="<% getCfgGeneral(1, "SSID8"); %>"></td>
            <td colspan="1"><input type="checkbox" name="hssid" value="7"></td>
            <td colspan="1"><input type="checkbox" name="isolated_ssid" value="7"></td>
            <td colspan="1"><input type="checkbox" name="mbcastisolated_ssid" value="7"></td>
            <td colspan="1"><input type="button" onClick="ssidRemove(this.form, 7);" class="half" value="Remove" id="basicRemove"></td>
          </tr>
          <tr id="div_mbssidapisolated">
            <td class="head" colspan="1" id="basicMBSSIDApIsolated">SSID to SSID Isolation</td>
            <td colspan="5"><select name="mbssidapisolated" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="div_abg_rate">
            <td class="head" colspan="1" id="basicRate">Rate</td>
            <td colspan="5"><select name="abg_rate" class="half">
              </select></td>
          </tr>
          <tr id="div_dot11h">
            <td class="head" colspan="1" id="basic80211h">IEEE802.11H</td>
            <td colspan="5"><select name="dot11h" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
        </table>
        <table id="div_11n" name="div_11n" class="form" style="display:none;">
          <tr>
            <td class="title" colspan="2" id="basicHTPhyMode">HT Physical Mode</td>
          </tr>
          <tr>
            <td class="head" id="basicHTChannelBW" width="50%">Channel BandWidth</td>
            <td width="50%"><select name="n_bandwidth" class="half" onClick="GExtChannelDisplay(this.form);">
                <option value="0">20MHz</option>
                <option value="1">20/40MHz</option>
              </select></td>
          </tr>
          <tr name="extension_channel" id="extension_channel">
            <td class="head" id="basicHTExtChannel" width="50%">Extension Channel</td>
            <td width="50%"><select id="n_extcha" name="n_extcha" class="mid">
                <option value="1" selected>2412MHz (Channel 1)</option>
              </select></td>
          </tr>
          <tr id="div_ht_tx_stream">
            <td class="head" id="basicHTTxStream" width="50%">HT TxStream</td>
            <td width="50%"><select name="tx_stream" class="half">
                <option value="1">1</option>
                <option value="2" id="div_HtTx2Stream">2</option>
              </select></td>
          </tr>
          <tr id="div_ht_rx_stream">
            <td class="head" id="basicHTRxStream" width="50%">HT RxStream</td>
            <td width="50%"><select name="rx_stream" class="half">
                <option value="1">1</option>
                <option value="2" id="div_HtRx2Stream">2</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" width="50%">MCS</td>
            <td width="50%"><select name="n_mcs" class="half">
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
            <td class="head" id="basicHTGI" width="50%">Guard Interval</td>
            <td width="50%"><select name="n_gi" class="half">
                <option value="0" id="basicHTLongGI">Long</option>
                <option value="1" id="basicHTAutoGI">Auto</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicHSTBC" width="50%">Space-Time Block Coding</td>
            <td width="50%"><select name="n_stbc" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicHTAMSDU" width="50%">Aggregation MSDU</td>
            <td width="50%"><select name="n_amsdu" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicHTAddBA" width="50%">Auto Block ACK</td>
            <td width="50%"><select name="n_autoba" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicHTDelBA" width="50%">Decline Block ACK Request</td>
            <td width="50%"><select name="n_badecline" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="htOpModeRow" style="display: none;">
            <td class="head" id="basicHTOPMode" width="50%">Green Filed Mode</td>
            <td width="50%"><select name="n_mode" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="basicHTRDG" width="50%">Reverse Direction Grant</td>
            <td width="50%"><select name="n_rdg" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
        </table>
        <table id="div_ac" name="div_ac" class="form" style="display:none;">
          <tr>
            <td class="title" colspan="2" id="basicVHTPhyMode">VHT Physical Mode</td>
          </tr>
          <tr id="basicVHTBW" >
            <td class="head" id="basicVHTBandWidth" width="50%">Channel BandWidth</td>
            <td width="50%"><select name="ac_bw" size="1" class="half">
                <option value="0" selected id="1">20/40MHz</option>
                <option value="1" id="2">20/40/80MHz</option>
		</select></td>
          </tr>
          <tr id="basicVHTBWSIGNAL">
            <td class="head" id="basicVHTSignalMode" width="50%">BandWidth Signaling Mode</td>
            <td width="50%"><select name="ac_bwsig" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="2">Static</option>
                <option value="2" id="3">Dynamic</option>
		</select></td>
          </tr>
          <tr>
            <td class="head" id="basicVHTGI" width="50%">Guard Interval</td>
            <td width="50%"><select name="ac_gi" class="half">
                <option value="0" id="basicVHTLongGI">Long</option>
                <option value="1" id="basicVHTAutoGI">Auto</option>
              </select></td>
          </tr>
          <tr id="div_11a_stbc" name="div_11a_stbc">
            <td class="head" id="basicVHTSTBC" width="50%">Space-Time Block Coding</td>
            <td width="50%"><select name="ac_stbc" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="div_11a_ldpc" name="div_11a_ldpc">
            <td class="head" id="basicLDPC" width="50%">Low Disenty parity check</td>
            <td width="50%"><select name="ac_ldpc" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
        </table>
        <table id="div_roaming" name="div_roaming" class="form" style="display:none;">
        	<tr>
        		<td class="title" id="fast_roaming" width="50%">Fast-roaming</td>
        		<td class="title" width="50%" style="text-align:right">
        			<select name="FastRoaming" size="1" class="half" onChange="fastRoamingChange(this.form);">
        				<option value="0" id="disable">Disable</option>
        				<option value="1" id="enable">Enable</option>
        			</select>
        		</td>
        	</tr>
        	<tr id="row_ApProbeRspTimes" style="display:none;">
        		<td class="head" id="basicApProbeRspTimes" width="50%">Limit probe reqest per client</td>
        		<td width="50%"><input type="text" name="ApProbeRspTimes" class="half" maxlength="4" value="<% getCfgZero(1, "ApProbeRspTimes"); %>"><font color="#808080"> 0 - 10 times, default 3</font></td>
        	</tr>
        	<tr id="row_AuthRspFail" style="display:none;">
        		<td class="head" id="basicAuthRspFail" width="50%">Reject auth req due to weak signal</td>
        		<td width="50%"><input type="text" name="AuthRspFail" class="half" maxlength="4" value="<% getCfgZero(1, "AuthRspFail"); %>"><font color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
        	</tr>
        	<tr id="row_AuthRspRssi" style="display:none;">
        		<td class="head" id="basicAuthRspRssi" width="50%">Ignore auth req due to weak signal</td>
        		<td width="50%"><input type="text" name="AuthRspRssi" class="half" maxlength="4" value="<% getCfgZero(1, "AuthRspRssi"); %>"><font color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
        	</tr>
        	<tr id="row_AssocReqRssiThres" style="display:none;">
        		<td class="head" id="basicAssocReqRssiThres" width="50%">Reject assoc req due to weak signal</td>
        		<td width="50%"><input type="text" name="AssocReqRssiThres" class="half" maxlength="4" value="<% getCfgZero(1, "AssocReqRssiThres"); %>"><font color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
        	</tr>
        	<tr id="row_AssocRspIgnor" style="display:none;">
        		<td class="head" id="basicAssocRspIgnor" width="50%">Ignore assoc req due to weak signal</td>
        		<td width="50%"><input type="text" name="AssocRspIgnor" class="half" maxlength="4" value="<% getCfgZero(1, "AssocRspIgnor"); %>"><font color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
        	</tr>
        	<tr id="row_ProbeRspRssi" style="display:none;">
        		<td class="head" id="basicProbeRspRssi" width="50%">Auto disonnect sta if rssi low at probe requests</td>
        		<td width="50%"><input type="text" name="ProbeRspRssi" class="half" maxlength="4" value="<% getCfgZero(1, "ProbeRspRssi"); %>"><font color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
        	</tr>
        	<tr id="row_KickStaRssiLow" style="display:none;">
        		<td class="head" id="basicKickStaRssiLow" width="50%">Auto disonnect sta if rssi low</td>
        		<td width="50%"><input type="text" name="KickStaRssiLow" class="half" maxlength="4" value="<% getCfgZero(1, "KickStaRssiLow"); %>"><font color="#808080"> 0 - -100 dBm, default 0 (off)</font></td>
        	</tr>
        </table>
        <table id="div_ids" name="div_ids" class="form" style="display:none;">
        	<tr>
        		<td class="title" id="basicIDS" width="50%">Intrusion Detection settings</td>
        		<td class="title" width="50%" style="text-align:right">
        			<select name="IdsEnable" size="1" class="half" onChange="idsChange(this.form);">
        				<option value="0" id="disable">Disable</option>
        				<option value="1" id="enable">Enable</option>
        			</select>
        		</td>
        	</tr>
        	<tr id="row_AuthFloodThreshold" style="display:none;">
        		<td class="head" id="basicAuthFloodThreshold" width="50%">Authentication</td>
        		<td width="50%"><input type="text" name="AuthFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "AuthFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
        	</tr>
        	<tr id="row_AssocReqFloodThreshold" style="display:none;">
        		<td class="head" id="basicAssocReqFloodThreshold" width="50%">Association request</td>
        		<td width="50%"><input type="text" name="AssocReqFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "AssocReqFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
        	</tr>
        	<tr id="row_ReassocReqFloodThreshold" style="display:none;">
        		<td class="head" id="basicReassocReqFloodThreshold" width="50%">Reassociation request</td>
        		<td width="50%"><input type="text" name="ReassocReqFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "ReassocReqFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
        	</tr>
        	<tr id="row_ProbeReqFloodThreshold" style="display:none;">
        		<td class="head" id="basicProbeReqFloodThreshold" width="50%">Probe request</td>
        		<td width="50%"><input type="text" name="ProbeReqFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "ProbeReqFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
        	</tr>
        	<tr id="row_DisassocFloodThreshold" style="display:none;">
        		<td class="head" id="basicDisassocFloodThreshold" width="50%">Disassociation</td>
        		<td width="50%"><input type="text" name="DisassocFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "DisassocFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
        	</tr>
        	<tr id="row_DeauthFloodThreshold" style="display:none;">
        		<td class="head" id="basicDeauthFloodThreshold" width="50%">Deauthentication</td>
        		<td width="50%"><input type="text" name="DeauthFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "DeauthFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
        	</tr>
        	<tr id="row_EapReqFloodThreshold" style="display:none;">
        		<td class="head" id="basicEapReqFloodThreshold" width="50%">EAP request</td>
        		<td width="50%"><input type="text" name="EapReqFloodThreshold" class="half" maxlength="4" value="<% getCfgZero(1, "EapReqFloodThreshold"); %>"><font color="#808080"> default 64</font></td>
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
