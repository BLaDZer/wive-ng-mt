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
}

function PageInit()
{
	initTranslation();
	showLoadedElements();

	loadStatistics();
}

function showLoadedElements()
{
	var opmode = '<% getCfgZero(1, "OperationMode"); %>';
	if (opmode == '2')
	{
		showElement("wirelessAbout");
		showElement("wirelessDriverVersion");
		showElement("wirelessMacAddr");
	}

	var nat_fp = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");
	displayElement('fastpath_warning', (nat_fp == '2') || (nat_fp == '3'));
}

function loadStatistics()
{
	var reloader = function(element)
	{
		initTranslation();
		showLoadedElements();
		self.setTimeout(loadStatistics, 5000);
	}

	ajaxLoadElement("statistics_table", "/adm/statistic_table.asp", reloader);
}

</script>
</head>
<body onLoad="PageInit()">
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
