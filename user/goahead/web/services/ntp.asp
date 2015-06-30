<!DOCTYPE html>
<html>
<head>
<title>System Management</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/parse.js"></script>
<script type="text/javascript" src="/js/sprintf.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("services");
Butterlate.setTextDomain("buttons");

function initTranslation()
{
	_TR("ntpTitle", "services ntp title");
	_TR("ntpIntroduction", "services ntp introduction");
	_TR("ntpSetup", "services ntp setup");
  _TR("ntpEnabled", "services ntp enabled");
  _TR("ntpDisable", "button disable");
  _TR("ntpEnable", "button enable");
  _TR("ntpCurrentTime", "services ntp current time");
  _TR("ntpTimeZone", "services ntp timezone");
  _TR("ntpServer", "services ntp server");

	_TRV("ntpApply", "button apply");
	_TRV("ntpCancel", "button cancel");
	_TRV("ntpSyncWithHost", "services ntp sync");
}

function initValue()
{
	var tz = "<% getCfgGeneral(1, "TZ"); %>";
	var dateb = "<% getDATEBuilt(); %>";
	var ena = '<% getCfgGeneral(1, "NTPEnabled"); %>';
	var form = document.NTP;
	var cTime = new Date();

	initTranslation();

  form.ntp_enabled.options.selectedIndex = 1*ena;
  
	if (dateb == "1")
	{
		showElement('div_date');
		form.ntpcurrenttime.disabled = false;
		form.ntpcurrenttime.value = sprintf('%02d:%02d:%02d %02d.%02d.%04d', cTime.getHours(), cTime.getMinutes(), cTime.getSeconds(), cTime.getDate(), cTime.getMonth()+1, cTime.getFullYear());
	}
	else
	{
		hideElement('div_date');
		form.ntpcurrenttime.disabled = true;
	}

	ntpChange(form);

	form.time_zone.value = tz;
}

function syncWithHost()
{
	var cTime = new Date();

	var tmp = sprintf('"%04d-%02d-%02d %02d:%02d:%02d"',
			cTime.getFullYear(), cTime.getMonth()+1, cTime.getDate(),
			cTime.getHours(), cTime.getMinutes(), cTime.getSeconds());

	ajaxPostRequest("/goform/NTPSyncWithHost", tmp, true);
}

function ntpChange(form)
{
  enableElements(
    [ form.manNTPSyncWithHost, form.time_zone, form.NTPServerIP ],
    (form.ntp_enabled.options.selectedIndex != 0));
  displayElement(
    [ "div_date", "div_tz", "div_server" ],
    (form.ntp_enabled.options.selectedIndex != 0));
  displayServiceStatus();
}

function displayServiceHandler(response)
{
  var form = document.l2tpConfig;

  var services = [
    // turned_on, row_id, daemon_id
    [ '<% getCfgGeneral(1, "NTPEnabled"); %>', 'ntp_enabled_row', 'ntpd' ],
  ];

  // Create associative array
  var tmp = response.split(',');
  var daemons = [];
  for (var i=0; i<tmp.length; i++)
    daemons[tmp[i]] = 1;

  // Now display all services
  for (var i=0; i<services.length; i++)
  {
    var service = services[i];
    var row = document.getElementById(service[1]);
    var tds = [];
    for (var j=0; j<row.childNodes.length; j++)
      if (row.childNodes[j].nodeName == 'TD')
        tds.push(row.childNodes[j]);

    if (row != null)
    {
      // Fill-up status
      if (service[0]*1 == '0')
        tds[2].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
      else
        tds[2].innerHTML = (daemons[service[2]] == 1) ?
          '<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
          '<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';
    }
  }

  serviceStatusTimer = setTimeout('displayServiceStatus();', 5000);
}

function displayServiceStatus()
{
  ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
}
</script>
</head>
<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="ntpTitle">NTP Management</h1>
      <p id="ntpIntroduction">Here you can configure time synchronization.</p>
      <hr />
      
      <!-- ================= NTP Settings ================= -->
      
      <form method="POST" name="NTP" action="/goform/NTP" >
        <table class="form">
          <tbody>
            <tr>
              <td class="title" colspan="3" id="ntpSetup">NTP Settings</td>
            </tr>
            <tr id="ntp_enabled_row">
              <td class="head" id="ntpEnabled">NTP synchronization</td>
              <td><select name="ntp_enabled" class="half" onChange="ntpChange(this.form);">
                <option value="0" id="ntpDisable">Disable</option>
                <option value="1" id="ntpEnable">Enable</option>
              </select></td>
              <td style="width: 56px;">&nbsp;</td>
            </tr>
            <tr id="div_date">
              <td class="head" id="ntpCurrentTime">Current Host Time</td>
              <td colspan="2"><input class="wide" name="ntpcurrenttime" type="text" readonly="1">
                <input type="button" class="normal" value="Sync with host" id="ntpSyncWithHost" name="manNTPSyncWithHost" onClick="syncWithHost()"></td>
            </tr>
            <tr id="div_tz">
              <td class="head" id="ntpTimeZone">Time Zone:</td>
              <td colspan="2"><select name="time_zone" class="wide">
                  <option value="UCT_-11" id="ntpMidIsland">(UTC-11:00) Midway Island, Samoa</option>
                  <option value="UCT_-10" id="ntpHawaii">(UTC-10:00) Hawaii</option>
                  <option value="NAS_-09" id="ntpAlaska">(UTC-09:00) Alaska</option>
                  <option value="PST_-08" id="ntpPacific">(UTC-08:00) Pacific Time</option>
                  <option value="MST_-07" id="ntpMountain">(UTC-07:00) Mountain Time</option>
                  <option value="MST_-07" id="ntpArizona">(UTC-07:00) Arizona</option>
                  <option value="CST_-06" id="ntpCentral">(UTC-06:00) Central Time</option>
                  <option value="UCT_-06" id="ntpMidUS">(UTC-06:00) Middle America</option>
                  <option value="UCT_-05" id="ntpIndianaEast">(UTC-05:00) Indiana East, Colombia</option>
                  <option value="EST_-05" id="ntpEastern">(UTC-05:00) Eastern Time</option>
                  <option value="AST_-04" id="ntpAtlantic">(UTC-04:00) Atlantic Time, Brazil West</option>
                  <option value="UCT_-04" id="ntpBolivia">(UTC-04:00) Bolivia, Venezuela</option>
                  <option value="UCT_-03" id="ntpGuyana">(UTC-03:00) Guyana</option>
                  <option value="EBS_-03" id="ntpBrazilEast">(UTC-03:00) Brazil East, Greenland</option>
                  <option value="NOR_-02" id="ntpMidAtlantic">(UTC-02:00) Mid-Atlantic</option>
                  <option value="EUT_-01" id="ntpAzoresIslands">(UTC-01:00) Azores Islands</option>
                  <option value="UCT_000" id="ntpGambia">(UTC) Gambia, Liberia, Morocco</option>
                  <option value="GMT_000" id="ntpEngland">(UTC) England</option>
                  <option value="MET_001" id="ntpCzechRepublic">(UTC+01:00) Czech Republic, N</option>
                  <option value="MEZ_001" id="ntpGermany">(UTC+01:00) Germany</option>
                  <option value="UCT_001" id="ntpTunisia">(UTC+01:00) Tunisia</option>
                  <option value="EET_002" id="ntpGreece">(UTC+02:00) Greece, Turkey</option>
                  <option value="EET-2EEST,M3.5.0/3,M10.5.0/4" id="">(UTC+02:00) Ukraine (Kiev), Latvia (Riga)</option>
                  <option value="EET-2EEST,M3.5.0,M10.5.0/3" id="">(UTC+02:00) Belorussia (Minsk)</option>
                  <option value="SAS_002" id="ntpSouthAfrica">(UTC+02:00) South Africa</option>
                  <option value="IDDT">(UTC+02:00) Israel (Tel Aviv)</option>
                  <option value="IST_003" id="ntpIraq">(UTC+03:00) Iraq, Jordan, Kuwait</option>
                  <option value="EET-2" id="">(UTC+02:00) Russia (Kaliningrad)</option>
                  <option value="MSK-3" id="ntpMoscowWinter">(UTC+03:00) Russia (Moscow, St.-Petersburg)</option>
                  <option value="AMT-4AMST,M3.5.0,M10.5.0/3" id="ntpArmenia">(UTC+04:00) Armenia (Yerevan)</option>
                  <option value="AZT-4AZST,M3.5.0/4,M10.5.0/5">(UTC+04:00) Azebaydzhan (Baku)</option>
                  <option value="GET-4">(UTC+04:00) Georgia (Tbilisi)</option>
                  <option value="TMT-5">(UTC+05:00) Ashgabad</option>
                  <option value="UZT-5">(UTC+04:00) Samarkand, Russia (Tashkent)</option>
                  <option value="UCT_005" id="ntpPakistan">(UTC+05:00) Pakistan</option>
                  <option value="YEKT-5">(UTC+05:00) Russia (Yekaterinburg)</option>
                  <option value="UCT_006" id="ntpBangladesh">(UTC+06:00) Bangladesh</option>
                  <option value="KGT-6" id="ntpBangladesh">(UTC+06:00) Kyrgyzstan (Bishkek)</option>
                  <option value="NOVT-6">(UTC+06:00) Russia (Novosibirsk)</option>
                  <option value="OMST-6">(UTC+06:00) Russia (Omsk)</option>
                  <option value="UCT_007" id="ntpThailand">(UTC+07:00) Thailand</option>
                  <option value="KRAT-7" id="ntpAustraliaWA">(UTC+07:00) Russia (Krasnoyarsk)</option>
                  <option value="CST_008" id="ntpChinaCoast">(UTC+08:00) China Coast, Hong Kong</option>
                  <option value="CCT_008" id="ntpTaipei">(UTC+08:00) Taipei</option>
                  <option value="SST_008" id="ntpSingapore">(UTC+08:00) Singapore</option>
                  <option value="AWS_008" id="ntpAustraliaWA">(UTC+08:00) Australia (WA)</option>
                  <option value="ULAT-8">(UTC+08:00) Mongolia (Ulan-Bator)</option>
                  <option value="IRKT-8">(UTC+08:00) Russia (Irkutsk)</option>
                  <option value="JST_009" id="ntpJapan">(UTC+09:00) Japan</option>
                  <option value="KST_009" id="ntpKorean">(UTC+09:00) North Korea, South Korea</option>
                  <option value="AES_010" id="ntpAustraliaQLD">(UTC+10:00) Australia (QLD, TAS,NSW,ACT,VIC)</option>
                  <option value="YAKT-09">(UTC+09:00) Russia (Yakutsk)</option>
                  <option value="UCT_010" id="ntpGuam">(UTC+10:00) Guam</option>
                  <option value="SAKT-10">(UTC+10:00) Russia (Sakhalin)</option>
                  <option value="VLAT-10">(UTC+10:00) Russia (Vladivostok)</option>
                  <option value="UCT_011" id="ntpSolomonIslands">(UTC+11:00) Solomon Islands</option>
                  <option value="MAGT-10">(UTC+10:00) Russia (Magadan)</option>
                  <option value="PETT-12">(UTC+12:00) Russia (Kamchatka)</option>
                  <option value="UCT_012" id="ntpFiji">(UTC+12:00) Fiji</option>
                  <option value="NZS_012" id="ntpNewZealand">(UTC+12:00) New Zealand</option>
                </select></td>
            </tr>
            <tr id="div_server">
              <td class="head" id="ntpServer">NTP Server</td>
              <td colspan="2"><input class="wide" name="NTPServerIP" value="<% getCfgGeneral(1, "NTPServerIP"); %>" type="text">
                <br>
                &nbsp;&nbsp;<font color="#808080">ex:&nbsp;time.nist.gov</font> <br>
                &nbsp;&nbsp;<font color="#808080">&nbsp;&nbsp;&nbsp;&nbsp;ru.pool.ntp.org</font> <br>
                &nbsp;&nbsp;<font color="#808080">&nbsp;&nbsp;&nbsp;&nbsp;time.stdtime.gov.tw</font></td>
            </tr>
          </tbody>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="hidden" value="/services/ntp.asp" name="submit-url">
              <input type="submit" class="normal" value="Apply"  id="ntpApply">
              &nbsp;&nbsp;
              <input type="reset"  class="normal" value="Cancel" id="ntpCancel" onClick="window.location.reload();"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
