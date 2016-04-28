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
var McastPhyMode = defaultNumber('<% getCfgZero(1, "McastPhyMode"); %>', '2');
var mcastMcs = defaultNumber('<% getCfgZero(1, "McastMcs"); %>', '0');
var video_turbine_built = '<% getVideoTurbineBuilt(); %>';
var video_turbine = '<% getCfgZero(1, "VideoTurbine"); %>';

var clamp = '<% getClampBuilt(); %>';
var dyn_vga = '<% getCfgZero(1, "DyncVgaEnable"); %>';
var dyn_vga_long = '<% getCfgZero(1, "SkipLongRangeVga"); %>';
var dyn_vga_clamp = '<% getCfgZero(1, "VgaClamp"); %>';

var maxstanum = '<% getCfgZero(1, "MaxStaNum"); %>'.split(";")[0];
var keepalive = '<% getCfgZero(1, "StationKeepAlive"); %>'.split(";")[0];
var idletimeout = '<% getCfgZero(1, "IdleTimeout"); %>';
var ackpolicy = '<% getCfgZero(1, "AckPolicy"); %>';
var wmmCapable = '<% getCfgZero(1, "WmmCapable"); %>';
var EDCCABuilt = '<% getEDCCABuilt(); %>';
var ED_MODE = '<% getCfgZero(1, "ED_MODE"); %>';

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
    _TR("advShortSlot", "adv short slot");
    _TR("advTxBurst", "adv tx burst");
    _TR("advPktAggr", "adv pkt aggregate");
    _TR("advWmm", "adv wmm");
    _TR("advBSAuto", "wireless auto");

    _TR("advMaxStaNumRange", "adv maximum stations range");
    _TR("advKeepAliveSec", "adv station keep alive range");
    _TR("advIdleTimeoutSec", "adv idletimeout range");
    _TR("advEntryLifeCheckTimes", "adv entrylifecheck range");

    _TR("staadvRegion", "staadv region");
    _TR("advCountryCodeNONE", "wireless none");

    _TR("advMul2UniConver", "adv multicast2unicast converter");
    _TR("advMul2Uni", "adv multicast2unicast");
    _TR("advMul2UniEnable", "wireless enable");
    _TR("advMul2UniDisable", "wireless disable");
    _TR("advMaxStaNum", "adv maximum stations number");
    _TR("advStationKeepAlive", "adv station keep alive");
    _TR("advIdleTimeout", "adv idletimeout");
    _TR("advEntryLifeCheck", "adv entrylifecheck");
    _TR("advMcastRate", "adv mcast rate");
    _TR("advOFDM", "adv ofdm");
    _TR("advHTMIX", "adv htmix");
    _TR("advAckPolicy", "basic ack policy");
    _TR("advNormalAck", "basic ack policy normal");
    _TR("advNoAck", "basic ack policy no");
    _TR("advED_MODE", "adv ed mode");

    _TR("advDynVGATitle", "adv dynvga title");
    _TR("advDynVGA", "adv dynvga mode");
    _TR("advDynVGALong", "adv dynvga long");
    _TR("advDynVGAClamp", "adv dynvga clamp");

    _TRV("advApply", "button apply");
    _TRV("advCancel", "button cancel");

    var elements = document.getElementsByTagName('option');
    for (var i = 0; i < elements.length; i++)
	if (elements[i].id == "disable")
	    elements[i].innerHTML = _("button disable");
	else if (elements[i].id == "enable")
	    elements[i].innerHTML = _("button enable");

    var elements = document.getElementsByTagName('font');
    for (var i = 0; i < elements.length; i++)
	if (elements[i].id == "disable")
	    elements[i].innerHTML = _("button disable");
	else if (elements[i].id == "enable")
	    elements[i].innerHTML = _("button enable");
}

function initValue()
{
    var datarateArray;
    var form = document.wireless_advanced;
    var AckPolicyArray = ackpolicy.split(";");

    initTranslation();
    form.bg_protection.options.selectedIndex = 1*bgProtection;
    form.short_preamble.options.selectedIndex = (shortPreamble == '1') ? 1 : 0;
    form.short_slot.options.selectedIndex = (shortSlot == '1') ? 1 : 0;
    form.tx_burst.options.selectedIndex = (txBurst == '1') ? 1 : 0;
    form.pkt_aggregate.options.selectedIndex = (pktAggregate == '1') ? 1 : 0;

    form.McastPhyMode.value = McastPhyMode;
    McastSwitch(form);

    //multicase to unicast converter
    displayElement('div_m2u', m2uBuilt == '1');
    form.m2u_enable.options.selectedIndex = (m2uEnabled == '1') ? 1 : 0;
    displayElement('video_turbine_row', (m2uBuilt == '1') && (video_turbine_built == '1'));
    form.video_turbine.options.selectedIndex = (video_turbine == '1') ? 1 : 0;
    form.WmmCapable.options.selectedIndex = (wmmCapable == '1') ? 1 : 0;

    displayElement('dynvga_row', true);
    form.dyn_vga.options.selectedIndex = (dyn_vga == '1') ? 1 : 0;
    if (clamp == '1')
    {
	displayElement('dynvga_long_row', true);
	form.dyn_vga_long.options.selectedIndex = (dyn_vga_long == '1') ? 1 : 0;
	displayElement('dynvga_clamp_row', true);
	form.dyn_vga_clamp.options.selectedIndex = 1*dyn_vga_clamp;
    }

    if (isNaN(maxstanum) || maxstanum < 1 || maxstanum > <% getMaxStaNum(); %>)
	form.maxstanum.value = 1*'<% getMaxStaNum(); %>';
    else
	form.maxstanum.value = maxstanum;

    if (isNaN(keepalive) || keepalive < 10 || keepalive > 300)
	form.keepalive.value = 60;
    else
	form.keepalive.value = keepalive;

    if (isNaN(idletimeout) || idletimeout < 60 || idletimeout > 300)
	form.idletimeout.value = 200;
    else
	form.idletimeout.value = idletimeout;

    form.AckPolicy.options.selectedIndex = 1*AckPolicyArray[0];

    form.ED_MODE.options.selectedIndex = (ED_MODE == '1') ? 1 : 0;
    displayElement('div_ED_MODE', EDCCABuilt == "1");
}

function McastSwitch(form) {
  form.McastMcs.options.length = 0;

  max_mcs = (form.McastPhyMode.value == "2") ? 7 : 15;

  for(var i = 0; i < (max_mcs + 1); i++) {
    addOption(form.McastMcs, "MCS: " + i, i);
    if(i <= mcastMcs)
      form.McastMcs.options.selectedIndex = i;
  }
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
            <font color="#808080" id="advBeaconIntervalRange">(range 20 - 999)</font></td>
          </tr>
          <tr>
            <td class="head" id="advDTIM">Data Beacon Rate</td>
            <td><input type="text" name="dtim" class="half" maxlength="3" value="<% getCfgZero(1, "DtimPeriod"); %>">
            <font color="#808080" id="advDTIMRange">(range 1 - 255)</font></td>
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
            <td><input type="text" name="maxstanum" class="half" maxlength="3" value="">
            <font color="#808080" id="advMaxStaNumRange">num</font><font color="#808080"> (1 - <% getMaxStaNum(); %>)</font></td>
          </tr>
          <tr>
            <td class="head" id="advStationKeepAlive">Station Keep-Alive</td>
            <td><input type="text" name="keepalive" class="half" maxlength="3" value="">
            <font color="#808080" id="advKeepAliveSec"> (10 - 300)</font></td>
          </tr>
          <tr>
            <td class="head" id="advIdleTimeout">IdleTimeout</td>
            <td><input type="text" name="idletimeout" class="half" maxlength="3" value="">
            <font color="#808080" id="advIdleTimeoutSec"> (60 - 300)</font></td>
          </tr>
          <tr>
            <td class="head" id="advEntryLifeCheck">EntryLifeCheck</td>
            <td><input type="text" name="EntryLifeCheck" class="half" maxlength="4" value="<% getCfgZero(1, "EntryLifeCheck"); %>">
            <font color="#808080" id="advEntryLifeCheckTimes"> (128 - 2048)</font></td>
          </tr>
          <tr>
            <td class="head" id="advShortPre">Short Preamble</td>
            <td><select name="short_preamble" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="advShortSlot">Short Slot</td>
            <td><select name="short_slot" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="advTxBurst">Tx Burst</td>
            <td><select name="tx_burst" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="advPktAggr">Pkt_Aggregate</td>
            <td><select name="pkt_aggregate" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td id="advWmm" class="head">WMM Capable</td>
            <td><select name="WmmCapable" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="div_ackpolicy">
            <td class="head" colspan="1" id="advAckPolicy">ACK Policy</td>
            <td colspan="5"><select name="AckPolicy" style="width: 150px;">
                <option value="0" id="advNormalAck">Normal ack</option>
                <option value="1" id="advNoAck">No ack</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="advMcastRate">Multicast TX mode</td>
            <td><select name="McastPhyMode" style="width: 150px;" onClick="McastSwitch(this.form);">
              <option value="2" id="advOFDM">Legacy OFDM</option>
              <option value="3" id="advHTMIX">HTMIX</option>
            </select>&nbsp;&nbsp;&nbsp;
            <select name="McastMcs" style="width: 150px;"></select></td>
          </tr>
          <tr id="div_ED_MODE">
            <td class="head" colspan="1" id="advED_MODE">Clear-Channel Assessment Monitor</td>
            <td><select name="ED_MODE" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="staadvRegion">Region settings</td>
            <td><select id="country_region" name="country_region" style="width: 150px;">
                <option value=0 <% var cr_bg = getCfgZero(0, "CountryRegion"); if (cr_bg == "0") write("selected"); %> >0: CH1-11 (FCC)</option>
                <option value=1 <% if (cr_bg == "1") write("selected"); %> >1: CH1-13 (IC)</option>
                <option value=2 <% if (cr_bg == "2") write("selected"); %> >2: CH10-11 (ETSI)</option>
                <option value=3 <% if (cr_bg == "3") write("selected"); %> >3: CH10-13 (SPAIN)</option>
                <option value=4 <% if (cr_bg == "4") write("selected"); %> >4: CH14 (France)</option>
                <option value=5 <% if (cr_bg == "5") write("selected"); %> >5: CH1-14 (MKK)</option>
                <option value=6 <% if (cr_bg == "6") write("selected"); %> >6: CH3-9 (MKK1)</option>
                <option value=7 <% if (cr_bg == "7") write("selected"); %> >7: CH5-13 (Israel)</option>
              </select>&nbsp;&nbsp;&nbsp;
              <select name="country_code" class="mid">
                <% listCountryCodes(); %>
              </select></td>
          </tr>
        </table>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="advDynVGATitle">PA/LNA control (WARNING!!! FOR EXPERTS ONLY!)</td>
          </tr>
          <tr id="dynvga_row">
            <td class="head" id="advDynVGA">Adaptive LNA Gain</td>
            <td><select name="dyn_vga" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="dynvga_long_row">
            <td class="head" id="advDynVGALong">Skip auto gain for long distance clients</td>
            <td><select name="dyn_vga_long" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="dynvga_clamp_row">
            <td class="head" id="advDynVGAClamp">Clamp maximum LNA GAIN in dynvga mode</td>
            <td><select name="dyn_vga_clamp" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1">-4dB</option>
                <option value="2">-8dB</option>
                <option value="3">-12dB</option>
                <option value="4">-16dB</option>
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
            <td><select name="m2u_enable" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="video_turbine_row">
            <td class="head">Video turbine</td>
            <td><select name="video_turbine" size="1" class="half">
                <option value="0" selected id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
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
