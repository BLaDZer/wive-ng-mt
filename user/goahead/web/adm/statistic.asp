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
<style type="text/css">
td.port_status {
	background-position: center center;
	background-repeat: no-repeat;
	height: 24px;
	width: 15%;
	text-align: center;
	vertical-align: middle;
	cursor: default;
}
</style>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("admin");

function initTranslation() {
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
	_TR("statusEthPortStatus", "status ethernet port status");
}

function showPortStatistics() {
	var el = document.getElementById('inpWanPort');
	var pstatus = el.value.split(';');
	var wan = 1 * '<% getCfgZero(1, "wan_port"); %>';
	var lan = '<% getCfgZero(1, "lan_port"); %>';
	var stb_port = ('1' != '<% getCfgZero(1, "tv_port"); %>') ? -1 :
			(wan == 0) ? 1 : wan - 1;
	var sip_port = ('1' != '<% getCfgZero(1, "sip_port"); %>') ? -1 :
			(wan == 0) ? 2 : wan - 2;

	if ((el == null) || (pstatus.length <= 0)) {
		return;
	}

	if (!((wan >= 0) && (wan <= 4)))
		wan = 4;
	if ((wan != 0) && (wan != 4))
		wan = 4;
	if (((lan != 'near') && (lan != 'distant')))
		lan = 'near';

	var content = '<td class="head" id="statusEthPortStatus">Port Status</td>';
	for (i=0; i<5; i++) {
		var port = pstatus[i].split(',');
		var image = 'empty';

		if (port[0] == '1')
		{
			if (port[1] == '10')
				image = '10';
			else if (port[1] == '100')
				image = '100';
			else if (port[1] == '1000')
				image = '1000';
			else
				image = '100';

			if (port[2] == 'H')
				image += '_h';
		}

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
			text = '<span style="color: #027fff;">WAN</span>';
		else if (i == stb_port)
			text = '<span style="color: #ff00d2;">TV</span>';
		else if (i == sip_port)
			text = '<span style="color: #ffd200;">SIP</span>';

		content = content + '<td class="port_status" style="background-color: #ffffff; color: #00ffff; background-image: url(\'/graphics/' + image + '.gif\'); "><b>' + text + '</b></td>';
	}
	ajaxModifyElementHTML('statisticPorts', content);
	displayElement('statisticHWStats', '<% getHWStatsBuilt(); %>' == '1');
}

function showLoadedElements() {
	var opmode = '<% getCfgZero(1, "OperationMode"); %>';
	var nat_fp = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");

	displayElement('fastpath_warning', (nat_fp == '2') || (nat_fp == '3'));
	displayElement("wirelessAbout", opmode == '2');
	displayElement("wirelessDriverVersion", opmode == '2');
	displayElement("wirelessMacAddr", opmode == '2');
}

function loadStatistics() {
	var reloader = function(element) {
		showLoadedElements();
		showPortStatistics();
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
      <div class="whitespace">&nbsp;</div>
    </td>
  </tr>
</table>
</body>
</html>
