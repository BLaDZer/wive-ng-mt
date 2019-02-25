<!DOCTYPE html>
<html>
	<head>
		<title>Wive-NG-MT</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_admin.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_overview.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/json2.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			NVRAM_stb_port	= (NVRAM_stb_port == '1') ? (NVRAM_wan_port == '0') ? 1 : +NVRAM_wan_port - 1 : -1;
			NVRAM_sip_port	= (NVRAM_sip_port == '1') ? (NVRAM_wan_port == '0') ? 2 : +NVRAM_wan_port - 2 : -1;

			function initTranslation() {
				_TR("ovSelectLang",		"overview select language");
				_TR("ovLangApply",		"button apply");
				_TR("ovStatus",			"overview status");
				_TR("ovStatistic",		"overview statistics");
				_TR("ovManagement",		"overview management");
				_TR("statusTitle",			"status title");
				_TR("statusIntroduction",	"status introduction");
				_TR("fastpath_warning",		"statistic warning");

				_TR("statusSysInfo",		"status system information");
				_TR("statusSDKversion",		"status sdk version");
				_TR("statusSysDateTime",	"status system date time");

				_TR("statusInternetConfig",	"status internet config");
				_TR("statusConnectedType",	"status connect type");
				_TR("statusINTIPAddr",		"status int ipaddr");
				_TR("statusWANIPAddr",		"status wan ipaddr");
				_TR("statusSubnetMask",		"status subnet mask");
				_TR("statusDefaultGW",		"status default gateway");
				_TR("statusPrimaryDNS",		"status primary dns");
				_TR("statusSecondaryDNS",	"status secondary dns");
				_TR("statusIPv6IntAddr",	"status internal ipv6 address");
				_TR("statusIPv6ExtAddr",	"status external ipv6 address");
				_TR("statusWANMAC",			"status mac");

				_TR("statusLocalNet",		"status local network");
				_TR("statusLANIPAddr",		"status local ipaddr");
				_TR("statusLANNetmask",		"status local netmask");
				_TR("statusLANMAC",			"status mac");

				_TR("statisticMMCPU",		"statistic memory and cpu");
				_TR("statisticMMTotal",		"statistic memory total");
				_TR("statisticCpuUse",		"statistic cpu usage");

				_TR("statisticWANLAN",		"statistic wanlan");
				_TR("statisticWANRxPkt",	"statistic wan rx pkt");
				_TR("statisticWANRxBytes",	"statistic wan rx bytes");
				_TR("statisticWANTxPkt",	"statistic wan tx pkt");
				_TR("statisticWANTxBytes",	"statistic wan tx bytes");
				_TR("statisticLANRxPkt",	"statistic lan rx pkt");
				_TR("statisticLANRxBytes",	"statistic lan rx bytes");
				_TR("statisticLANTxPkt",	"statistic lan tx pkt");
				_TR("statisticLANTxBytes",	"statistic lan tx bytes");
			}

			function initValues() {
				var lang = document.getElementById("langSelection");

				initTranslation();
				if (BUILD_LangEN)
					lang.options[lang.length] = new Option('English', 'en');
				if (BUILD_LangRU)
					lang.options[lang.length] = new Option('Russian', 'ru');

				lang.value = NVRAM_Language;

				var IS_DEMO = '<% isDemoMode(); %>' == '1';
				if (!IS_DEMO) {
				    showWarning();
				    displayElement('fastpath_warning', NVRAM_offloadMode == '2' || NVRAM_offloadMode == '3');
				}

				initTranslation();
				reloadStat();
				function reloadStat() {
					ajaxLoadScript('/adm/sysinfo.js');
					setTimeout(reloadStat, 10000);
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<table class="body">
			<tr>
				<td>
					<p id="ovIntroduction"></p>
					<!-- ----------------- Langauge Settings ----------------- -->
					<form method="post" name="Lang" action="/goform/setSysLang">
						<blockquote>
							<fieldset>
								<h1>Wive-NG-MT - opensource firmware to make your life better.</h1>
								<legend id="ovSelectLang">Select Language</legend>
								<select class="half" name="langSelection" id="langSelection">
									<!-- added by initValue -->
								</select>
								&nbsp;&nbsp;
								<input type="hidden" name="submit-url" value="/overview.asp" >
								<input type="submit" class="half" value="Apply" id="ovLangApply" onClick="setTimeout(function () { parent.menu.location.reload(); }, 500)">
								<div id="warning"></div>
							<p>
								<div id="ovAbout"><hr /><br />
								<a href="https://wi-cat.ru" target="_blank"><img src="graphics/wi-cat-logo.jpg" height="100" border="0" align="right" hspace="0" vspace="10"></a>
								WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.
								<br />
								Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
								<br /><br />
								News and changes in Russian:<br />
								<a href="https://wi-cat.ru" target="_blank">wi-cat.ru - Wireless Comprehensive Advanced Technology.</a><br />
								<a href="http://wive-ng.sf.net" target="_blank">wive-ng.ru - development opensource routers firmware.</a><br />
								<br />Bug report of Wive-NG software problem please to <a href="https://wi-cat.ru/forums" target="_blank">forum</a><br />
								</div>
							</p>
							</fieldset>
						</blockquote>
					</form>
					<blockquote>
						<fieldset>
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<table class="form" >
						<col style="width: 40%;" />
						<col style="width: 60%;" />
						<tbody>
							<!-- ================= System Info ================= -->
							<tr>
								<td class="title" colspan="2" id="statusSysInfo">System Info</td>
							</tr>
							<tr>
								<td class="head" id="statusSDKversion">Firmware Version</td>
								<td id="statusSDKversion_value"></td>
							</tr>
							<tr>
								<td class="head" id="statusSysDateTime">System Time</td>
								<td id="statusSysDateTime_value"></td>
							</tr>
							<!-- ==============  MEMORY and CPU  =============== -->
							<tr>
								<td class="title" colspan="2" id="statisticMMCPU">Memory and CPU</td>
							</tr>
							<tr>
								<td class="head" id="statisticMMTotal">Memory total</td>
								<td id="statisticMMTotal_value"></td>
							</tr>
							<tr>
								<td class="head" id="statisticCpuUse">CPU usage</td>
								<td id="statisticCpuUse_value"></td>
							</tr>
						</tbody>
					</table>						
					<!-- =================  SOFT NETORK  ================= -->
					<div id="ethernetStatus"></div>
					<div id="statisticSWStats"></div>
					<table class="form" >
						<col style="width: 40%;" />
						<col style="width: 60%;" />
						<tbody>
							<!-- ================= Internet Configurations ================= -->
							<tr>
								<td class="title" colspan="2" id="statusInternetConfig">Internet Configuration</td>
							</tr>
							<tr id="statusConnectedType_tr">
								<td class="head" id="statusConnectedType">Connection Mode</td>
								<td id="statusConnectedType_value"></td>
							</tr>
							<tr id="statusINTIPAddr_tr">
								<td class="head" id="statusINTIPAddr">Internet/VPN IP Address</td>
								<td id="statusINTIPAddr_value"></td>
							</tr>
							<tr id="statusWANIPAddr_tr">
								<td class="head" id="statusWANIPAddr">Physical WAN IP Address</td>
								<td id="statusWANIPAddr_value"></td>
							</tr>
							<tr>
								<td class="head" id="statusSubnetMask">Subnet Mask</td>
								<td id="statusSubnetMask_value"></td>
							</tr>
							<tr id="statusDefaultGW_tr">
								<td class="head" id="statusDefaultGW">Default Gateway</td>
								<td id="statusDefaultGW_value"></td>
							</tr>
							<tr id="statusPrimaryDNS_tr">
								<td class="head" id="statusPrimaryDNS">Primary Domain Name Server</td>
								<td id="statusPrimaryDNS_value"></td>
							</tr>
							<tr id="statusSecondaryDNS_tr">
								<td class="head" id="statusSecondaryDNS">Secondary Domain Name Server</td>
								<td id="statusSecondaryDNS_value"></td>
							</tr>
							<tr id="statusIPv6IntAddr_tr">
								<td class="head" id="statusIPv6IntAddr">Internal IPv6 Address</td>
								<td id="statusIPv6IntAddr_value"></td>
							</tr>
							<tr id="statusIPv6ExtAddr_tr">
								<td class="head" id="statusIPv6ExtAddr">External IPv6 Address</td>
								<td id="statusIPv6ExtAddr_value"></td>
							</tr>
							<tr>
								<td class="head" id="statusWANMAC">MAC Address</td>
								<td id="statusWANMAC_value"></td>
							</tr>
							<!-- ================= Local Network ================= -->
							<tr id="statusLocalNet_tr">
								<td class="title" colspan="2" id="statusLocalNet">Local Network</td>
							</tr>
							<tr id="statusLANIPAddr_tr">
								<td class="head" id="statusLANIPAddr">Local IP Address</td>
								<td id="statusLANIPAddr_value"></td>
							</tr>
							<tr id="statusLANNetmask_tr">
								<td class="head" id="statusLANNetmask">Local Netmask</td>
								<td id="statusLANNetmask_value"></td>
							</tr>
							<tr id="statusLANMAC_tr">
								<td class="head" id="statusLANMAC">MAC Address</td>
								<td id="statusLANMAC_value"></td>
							</tr>
						</tbody>
					</table>
					<hr>
					<div style="display:none;" id="fastpath_warning"></div>	
				</td>
			</tr>
		</table>
						</fieldset>
					</blockquote>
				</td>
			</tr>
		</table>
	</body>
</html>