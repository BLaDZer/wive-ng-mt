<!DOCTYPE html>
<html>
<head>
<title>Statistic</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("admin");

function initTranslation()
{
	_TR("statisticTitle", "statistic title");
	_TR("statisticIntroduction", "statistic introduction");
	_TR("fastpath_warning", "statistic warning");

	_TR("statWiFiAbout", "statistic wireless about");
	_TR("statWiFiVer", "statistic wireless version");
	_TR("statWiFiMAC", "status mac");

	_TR("statisticMMCPU", "statistic memory and cpu");
	_TR("statisticMMTotal", "statistic memory total");
	_TR("statisticMMLeft", "statistic memory left");
	_TR("statisticCpuUse", "statistic cpu usage");

	_TR("statisticWANLAN", "statistic wanlan");
	_TR("statisticWANRxPkt", "statistic wan rx pkt");
	_TR("statisticWANRxBytes", "statistic wan rx bytes");
	_TR("statisticWANTxPkt", "statistic wan tx pkt");
	_TR("statisticWANTxBytes", "statistic wan tx bytes");
	_TR("statisticLANRxPkt", "statistic lan rx pkt");
	_TR("statisticLANRxBytes", "statistic lan rx bytes");
	_TR("statisticLANTxPkt", "statistic lan tx pkt");
	_TR("statisticLANTxBytes", "statistic lan tx bytes");

	_TR("statisticAllIF", "statistic all interface");
	_TR("statisticIfName", "statistic interface name");
	_TR("statisticRxPkt", "statistic rx pkt");
	_TR("statisticRxBytes", "statistic rx bytes");
	_TR("statisticTxPkt", "statistic tx pkt");
	_TR("statisticTxBytes", "statistic tx bytes");
	_TR("statisPorts", "statistic phy ports");
	_TR("stats_rx", "statistic rx bytes");
	_TR("stats_tx", "statistic tx bytes");
}

function showLoadedElements()
{
	var wan = 1 * '<% getCfgZero(1, "wan_port"); %>';
	var lan = '<% getCfgZero(1, "lan_port"); %>';
	var stb_port = ('1' != '<% getCfgZero(1, "tv_port"); %>') ? -1 :
			(wan == 0) ? 1 : wan - 1;
	var sip_port = ('1' != '<% getCfgZero(1, "sip_port"); %>') ? -1 :
			(wan == 0) ? 2 : wan - 2;

	var opmode = '<% getCfgZero(1, "OperationMode"); %>';
	if (opmode == '2')
	{
		showElement("wirelessAbout");
		showElement("wirelessDriverVersion");
		showElement("wirelessMacAddr");
	}

	var nat_fp = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");
	displayElement('fastpath_warning', (nat_fp == '2') || (nat_fp == '3'));

	if (!((wan >= 0) && (wan <= 4)))
		wan = 4;

	// Only WLLLL, WWLLL, LLLWW, LLLLW modes supported now
	if ((wan != 0) && (wan != 4))
		wan = 4;

	if (((lan != 'near') && (lan != 'distant')))
		lan = 'near';

	var content = '<td class="head"></td>';
	for (i=0; i<5; i++) {
		if (lan == 'distant') {
			if (wan == 0) {
				var text = 5-i;
			} else if (wan == 4) {
				var text = i+1;
			}
		} else if (lan == 'near') {
			if (wan == 0) {
				var text = i;
			} else if (wan == 4){
				var text = 4-i;
			}
		}
		if (i == wan)
			text = 'WAN';
		else if (i == stb_port)
			text = 'TV';
		else if (i == sip_port)
			text = 'SIP';
		else
			text = 'LAN' + text;
		content = content + '<td class="head">' + text + '</td>';
	}
	ajaxModifyElementHTML('statisticPorts', content);
	displayElement('statisticHWStats', '<% getHWStatsBuilt(); %>' == '1');
}

function loadStatistics()
{
	var reloader = function(element)
	{
		showLoadedElements();
		initTranslation();
		self.setTimeout(loadStatistics, 5000);
	}

	ajaxLoadElement("statistics_table", "/adm/statistic_table.asp", reloader);
}

</script>
</head>
<body onLoad="loadStatistics();">
<table class="body">
  <tr>
    <td><h1 id="statisticTitle">Statistics</h1>
      <p id="statisticIntroduction"> Take a look at the CPE statistics </p>
      <div style="display:none;" id="fastpath_warning">
        <p><span style="color: #ff0000;"><b>CAUTION!&nbsp;</b></span> Hardware NAT enabled. Traffic counters on interfaces not present real traffic.</p>
        <p>For some technical and software reasons there is no guarantee that gathered statistics for
          interfaces in '<b>Hardware NAT offload mode</b>' mode is correct now.</p>
        <p>To get correct statistics you need to shut down '<b>Hardware NAT offload mode</b>' option on <a href="/services/misc.asp#nat_fastpath_ref">MISC&nbsp;Services</a> configuration page.</p>
      </div>
      <hr>
      <div id="statistics_table" > </div>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
