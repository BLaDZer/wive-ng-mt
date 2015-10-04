<!DOCTYPE html>
<html>
<head>
<title>Advanced Wireless Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("wireless");
Butterlate.setTextDomain("buttons");

var bgProtection = '<% getCfgZero(1, "BGProtection"); %>';
var shortPreamble = '<% getCfgZero(1, "TxPreamble"); %>';
var shortSlot = '<% getCfgZero(1, "ShortSlot"); %>';
var txBurst = '<% getCfgZero(1, "TxBurst"); %>';
var pktAggregate = '<% getCfgZero(1, "PktAggregate"); %>';
var m2uBuilt = '<% getWlanM2UBuilt(); %>';
var m2uEnabled = '<% getCfgZero(1, "M2UEnabled"); %>';
var mcastMcs = defaultNumber('<% getCfgZero(1, "McastMcs"); %>', '0');
var video_turbine_built='<% getVideoTurbineBuilt(); %>';
var video_turbine = '<% getCfgZero(1, "VideoTurbine"); %>';
var maxstanum = '<% getCfgZero(1, "MaxStaNum"); %>'.split(";")[0];
var keepalive = '<% getCfgZero(1, "StationKeepAlive"); %>'.split(";")[0];
var idletimeout = '<% getCfgZero(1, "IdleTimeout"); %>';
var bandsteeringBuilt = '<% getBandSteeringBuilt(); %>';
var bandsteering = '<% getCfgZero(1, "BandSteering"); %>';

var wmmCapable = '<% getCfgZero(1, "WmmCapable"); %>';

function initTranslation()
{
	_TR("advTitle", "adv title");
	_TR("advIntroduction", "adv introduction");

	_TR("advWireless", "adv wireless");
	_TR("advBGProtect", "adv bgpro");
	_TR("advBGProAuto", "wireless auto");
	_TR("advBGProOn", "wireless on");
	_TR("advBGProOff", "wireless off");

	_TR("advBeaconInterval", "adv beacon interval");
	_TR("advBeaconIntervalRange", "adv beacon interval range");
	_TR("advDTIM", "adv dtim");
	_TR("advDTIMRange", "adv dtim range");
	_TR("advFrag", "adv fragment threshold");
	_TR("advFragRange", "adv fragment threshold range");
	_TR("advRTS", "adv rts threshold");
	_TR("advRTSRange", "adv rts threshold range");
	_TR("advShortPre", "adv short preamble");
	_TR("advShortPreEnable", "wireless enable");
	_TR("advShortPreDisable", "wireless disable");
	_TR("advShortSlot", "adv short slot");
	_TR("advShortSlotEnable", "wireless enable");
	_TR("advShortSlotDisable", "wireless disable");
	_TR("advTxBurst", "adv tx burst");
	_TR("advTxBurstEnable", "wireless enable");
	_TR("advTxBurstDisable", "wireless disable");
	_TR("advPktAggr", "adv pkt aggregate");
	_TR("advPktAggrEnable", "wireless enable");
	_TR("advPktAggrDisable", "wireless disable");
	_TR("advWmm", "adv wmm");
	_TR("advWMMEnable", "wireless enable");
	_TR("advWMMDisable", "wireless disable");
	_TR("advBSEnable", "wireless enable");
	_TR("advBSDisable", "wireless disable");
	_TR("advBSAuto", "wireless auto");

	_TR("staadvCountry", "staadv country");
	_TR("advCountryCode", "adv country code");
	_TR("advCountryCodeUS", "adv country code us");
	_TR("advCountryCodeJP", "adv country code jp");
	_TR("advCountryCodeRU", "adv country code ru");
	_TR("advCountryCodeFR", "adv country code fr");
	_TR("advCountryCodeTW", "adv country code tw");
	_TR("advCountryCodeIE", "adv country code ie");
	_TR("advCountryCodeHK", "adv country code hk");
	_TR("advCountryCodeNONE", "wireless none");

	_TR("advMul2UniConver", "adv multicast2unicast converter");
	_TR("advMul2Uni", "adv multicast2unicast");
	_TR("advMul2UniEnable", "wireless enable");
	_TR("advMul2UniDisable", "wireless disable");
	_TR("advMaxStaNum", "adv maximum stations number");
	_TR("advStationKeepAlive", "adv station keep alive");
	_TR("advIdleTimeout", "adv idletimeout");
	_TR("advEntryLifeCheck", "adv entrylifecheck");

	_TRV("advApply", "button apply");
	_TRV("advCancel", "button cancel");
}

function initValue()
{
	var datarateArray;
	var form = document.wireless_advanced;

	initTranslation();
	form.bg_protection.options.selectedIndex = 1*bgProtection;

	if (shortPreamble == '1')
	{
		form.short_preamble[0].checked = true;
		form.short_preamble[1].checked = false;
	}
	else
	{
		form.short_preamble[0].checked = false;
		form.short_preamble[1].checked = true;
	}

	if (shortSlot == '1')
		form.short_slot[0].checked = true;
	else
		form.short_slot[1].checked = true;

	if (txBurst == '1')
	{
		form.tx_burst[0].checked = true;
		form.tx_burst[1].checked = false;
	}
	else
	{
		form.tx_burst[0].checked = false;
		form.tx_burst[1].checked = true;
	}

	if (pktAggregate == '1')
	{
		form.pkt_aggregate[0].checked = true;
		form.pkt_aggregate[1].checked = false;
	}
	else
	{
		form.pkt_aggregate[0].checked = false;
		form.pkt_aggregate[1].checked = true;
	}

	//multicase to unicast converter
	hideElement('div_m2u');
	if (m2uBuilt == '1')
	{
		showElement('div_m2u');

		form.McastMcs.value = mcastMcs;
		form.m2u_enable[(m2uEnabled == '1') ? 0 : 1 ].checked = true;
		if (video_turbine_built == '1')
			form.video_turbine[(video_turbine == '1') ? 0 : 1].checked = true;
		else
			displayElement('video_turbine_row', false);
	}

	form.WmmCapable[0].checked = (wmmCapable == '1');
	form.WmmCapable[1].checked = (wmmCapable != '1');

  if (isNaN(maxstanum) || maxstanum < 1 || maxstanum > <% getMaxStaNum(); %>) {
    form.maxstanum.value = 1*'<% getMaxStaNum(); %>';
  } else {
    form.maxstanum.value = maxstanum;
  }
  if (isNaN(keepalive) || keepalive < 10 || keepalive > 300) {
    form.keepalive.value = 60;
  } else {
    form.keepalive.value = keepalive;
  }
  if (isNaN(idletimeout) || idletimeout < 60 || idletimeout > 300) {
    form.idletimeout.value = 200;
  } else {
    form.idletimeout.value = idletimeout;
  }

  form.BandSteering.options.selectedIndex = 1*bandsteering;
  displayElement('bandsteering_row', bandsteeringBuilt == "1");
}

function CheckValue(form)
{
	if (form.beacon.value == "")
	{
		alert(_("adv no beacon"));
		form.beacon.focus();
		form.beacon.select();
		return false;
	}

	if (isNaN(form.beacon.value) || form.beacon.value < 20 || form.beacon.value > 999)
	{
		alert(_("adv invalid beacon"));
		form.beacon.focus();
		form.beacon.select();
		return false;
	}

	if (form.dtim.value == "" )
	{
		alert(_("adv no dtim"));
		form.dtim.focus();
		form.dtim.select();
		return false;
	}

	if (isNaN(form.dtim.value) || form.dtim.value < 1 || form.dtim.value > 255)
	{
		alert(_("adv invalid dtim"));
		form.dtim.focus();
		form.dtim.select();
		return false;
	}

	if (form.fragment.value == "" )
	{
		alert(_("adv no frag len"));
		form.fragment.focus();
		form.fragment.select();
		return false;
	}

	if (isNaN(form.fragment.value) || form.fragment.value < 1 || form.fragment.value > 2346)
	{
		alert(_("adv invalid frag len"));
		form.fragment.focus();
		form.fragment.select();
		return false;
	}

	if (form.rts.value == "" )
	{
		alert(_("adv no rts"));
		form.rts.focus();
		form.rts.select();
		return false;
	}

	if (isNaN(form.rts.value) || form.rts.value < 1 || form.rts.value > 2347)
	{
		alert(_("adv invalid rts"));
		form.rts.focus();
		form.rts.select();
		return false;
	}

  if (isNaN(form.maxstanum.value) || form.maxstanum.value < 1 || form.maxstanum.value > <% getMaxStaNum(); %>)
  {
    alert(_("adv invalid max sta num"));
    form.maxstanum.focus();
    form.maxstanum.select();
    return false;
  }

  if (isNaN(form.keepalive.value) || form.keepalive.value < 10 || form.keepalive.value > 300)
  {
    alert(_("adv invalid keepalive"));
    form.keepalive.focus();
    form.keepalive.select();
    return false;
  }

  if (isNaN(form.idletimeout.value) || form.idletimeout.value < 60 || form.idletimeout.value > 300)
  {
    alert(_("adv invalid idletimeout"));
    form.idletimeout.focus();
    form.idletimeout.select();
    return false;
  }

  if (isNaN(form.EntryLifeCheck.value) || form.EntryLifeCheck.value < 128 || form.EntryLifeCheck.value > 2048)
  {
    alert(_("adv invalid entrylifecheck"));
    form.EntryLifeCheck.focus();
    form.EntryLifeCheck.select();
    return false;
  }

  return true;
}

</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="advTitle">Advanced Wireless Settings </h1>
      <p id="advIntroduction">Here you can change advanced wireless settings, such as Beacon Interval, Control Tx Rates and Basic Data Rates. </p>
      <hr>
      <form method="post" name="wireless_advanced" action="/goform/wirelessAdvanced" onsubmit="return CheckValue(this)">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="advWireless">Advanced Wireless</td>
          </tr>
          <tr>
            <td class="head" id="advBGProtect">BG Protection Mode</td>
            <td><select name="bg_protection" size="1" class="half">
                <option value="0" selected id="advBGProAuto">Auto</option>
                <option value="1" id="advBGProOn">On</option>
                <option value="2" id="advBGProOff">Off</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="advBeaconInterval">Beacon Interval</td>
            <td><input type="text" name="beacon" class="half" maxlength="3" value="<% getCfgZero(1, "BeaconPeriod"); %>">
              ms <font color="#808080" id="advBeaconIntervalRange">(range 20 - 999)</font></td>
          </tr>
          <tr>
            <td class="head" id="advDTIM">Data Beacon Rate</td>
            <td><input type="text" name="dtim" class="half" maxlength="3" value="<% getCfgZero(1, "DtimPeriod"); %>">
              ms <font color="#808080" id="advDTIMRange">(range 1 - 255)</font></td>
          </tr>
          <tr>
            <td class="head" id="advFrag">Fragment Threshold</td>
            <td><input type="text" name="fragment" class="half" maxlength="4" value="<% getCfgZero(1, "FragThreshold"); %>">
              <font color="#808080" id="advFragRange">(range 256 - 2346)</font></td>
          </tr>
          <tr>
            <td class="head" id="advRTS">RTS Threshold</td>
            <td><input type="text" name="rts" class="half" maxlength="4" value="<% getCfgZero(1, "RTSThreshold"); %>">
              <font color="#808080" id="advRTSRange">(range 1 - 2347)</font></td>
          </tr>
          <tr>
            <td class="head" id="advMaxStaNum">Maximum clients per SSID</td>
            <td><input type="text" name="maxstanum" class="half" maxlength="3" value=""><font color="#808080"> (1 - <% getMaxStaNum(); %>)</font></td>
          </tr>
          <tr>
            <td class="head" id="advStationKeepAlive">Station Keep-Alive</td>
            <td><input type="text" name="keepalive" class="half" maxlength="3" value="">s<font color="#808080"> (10 - 300)</font></td>
          </tr>
          <tr>
            <td class="head" id="advIdleTimeout">IdleTimeout</td>
            <td><input type="text" name="idletimeout" class="half" maxlength="3" value="">s<font color="#808080"> (60 - 300)</font></td>
          </tr>
          <tr>
            <td class="head" id="advEntryLifeCheck">EntryLifeCheck</td>
            <td><input type="text" name="EntryLifeCheck" class="half" maxlength="4" value="<% getCfgZero(1, "EntryLifeCheck"); %>"><font color="#808080"> (128 - 2048)</font></td>
          </tr>
          <tr>
            <td class="head" id="advShortPre">Short Preamble</td>
            <td><input type="radio" name="short_preamble" value="1">
              <font id="advShortPreEnable">Enable</font>&nbsp;
              <input type="radio" name="short_preamble" value="0">
              <font id="advShortPreDisable">Disable</font></td>
          </tr>
          <tr>
            <td class="head" id="advShortSlot">Short Slot</td>
            <td><input type="radio" name="short_slot" value="1" checked>
              <font id="advShortSlotEnable">Enable</font>&nbsp;
              <input type="radio" name="short_slot" value="0">
              <font id="advShortSlotDisable">Disable</font></td>
          </tr>
          <tr>
            <td class="head" id="advTxBurst">Tx Burst</td>
            <td><input type="radio" name="tx_burst" value="1" checked>
              <font id="advTxBurstEnable">Enable</font>&nbsp;
              <input type="radio" name="tx_burst" value="0">
              <font id="advTxBurstDisable">Disable</font></td>
          </tr>
          <tr>
            <td class="head" id="advPktAggr">Pkt_Aggregate</td>
            <td><input type="radio" name="pkt_aggregate" value="1">
              <font id="advPktAggrEnable">Enable</font>&nbsp;
              <input type="radio" name="pkt_aggregate" value="0" checked>
              <font id="advPktAggrDisable">Disable</font></td>
          </tr>
          <tr>
            <td id="advWmm" class="head">WMM Capable</td>
            <td><input type="radio" name="WmmCapable" value="1">
              <font id="advWMMEnable">Enable</font>&nbsp;
              <input type="radio" name="WmmCapable" value="0" checked>
              <font id="advWMMDisable">Disable</font></td>
          </tr>
          <tr id="bandsteering_row">
            <td class="head">BandSteering</td>
            <td><select name="BandSteering" size="1" class="half">
                <option value="0" selected id="advBSDisable">Disable</option>
                <option value="1" id="advBSEnable">Enable</option>
                <option value="2" id="advBSAuto">Auto</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="staadvCountry">Country Region Code</td>
            <td><select id="country_region" name="country_region" class="mid">
                <option value=0 <% var cr_bg = getCfgZero(0, "CountryRegion"); if (cr_bg == "0") write("selected"); %> >0: CH1-11 (FCC)</option>
                <option value=1 <% if (cr_bg == "1") write("selected"); %> >1: CH1-13 (IC)</option>
                <option value=2 <% if (cr_bg == "2") write("selected"); %> >2: CH10-11 (ETSI)</option>
                <option value=3 <% if (cr_bg == "3") write("selected"); %> >3: CH10-13 (SPAIN)</option>
                <option value=4 <% if (cr_bg == "4") write("selected"); %> >4: CH14 (France)</option>
                <option value=5 <% if (cr_bg == "5") write("selected"); %> >5: CH1-14 (MKK)</option>
                <option value=6 <% if (cr_bg == "6") write("selected"); %> >6: CH3-9 (MKK1)</option>
                <option value=7 <% if (cr_bg == "7") write("selected"); %> >7: CH5-13 (Israel)</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="advCountryCode"> Country Code </td>
            <td><select name="country_code" class="mid">
                <% listCountryCodes(); %>
              </select></td>
          </tr>
        </table>
        <input type="hidden" name="rebootAP" value="0">
        <table id="div_m2u" name="div_m2u" class="form">
          <tr>
            <td class="title" colspan="2" id="advMul2UniConver">Multicast-to-Unicast Converter</td>
          </tr>
          <tr>
            <td class="head" id="advMul2Uni">Multicast-to-Unicast</td>
            <td><input type="radio" name="m2u_enable" value="1">
              <font id="advMul2UniEnable">Enable</font>&nbsp;
              <input type="radio" name="m2u_enable" value="0">
              <font id="advMul2UniDisable">Disable</font></td>
          </tr>
          <tr>
            <td class="head">Multicast TX rate</td>
            <td><select name="McastMcs" class="half">
                <option value="0">0</option>
                <option value="1">1</option>
                <option value="2">2</option>
                <option value="3">3</option>
                <option value="4">4</option>
                <option value="5">5</option>
                <option value="6">6</option>
                <option value="7">7</option>
                <option value="8">8</option>
                <option value="9">9</option>
                <option value="10">10</option>
                <option value="11">11</option>
                <option value="12">12</option>
                <option value="13">13</option>
                <option value="14">14</option>
                <option value="15">15</option>
              </select></td>
          </tr>
          <tr id="video_turbine_row">
            <td class="head">Video turbine</td>
            <td><input type="radio" name="video_turbine" value="1">
              Enable&nbsp;
              <input type="radio" name="video_turbine" value="0">
              Disable </td>
          </tr>
        </table>
        <br>
        <table class="buttons">
          <tr align="center">
            <td><input type="submit" class="normal" value="Apply" id="advApply">
              &nbsp;&nbsp;
              <input type="reset"  class="normal" value="Cancel" id="advCancel" onClick="window.location.reload()">
              <input type="hidden" name="submit-url" value="/wireless/advanced.asp" ></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
