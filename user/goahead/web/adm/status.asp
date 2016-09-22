<!DOCTYPE html>
<html>
	<head>
		<title>CPE Status</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			Butterlate.setTextDomain("admin");
			Butterlate.setTextDomain("mode");
			Butterlate.setTextDomain("buttons");

			var timer = null;

			function initTranslation()
			{
				_TR("statusTitle", 			"status title");
				_TR("statusIntroduction",	"status introduction");

				_TR("statusSysInfo",		"status system information");
				_TR("statusSDKVersion",		"status sdk version");
				_TR("statusSysDateTime",	"status system date time");
				_TR("statusSysUpTime",		"status system up time");
				_TR("statusSysPlatform",	"status system platform");
				_TR("statusOPMode",			"status op mode");

				_TR("statusInternetConfig", "status internet config");
				_TR("statusConnectedType",	"status connect type");
				_TR("statusINTIPAddr",		"status int ipaddr");
				_TR("statusWANIPAddr",		"status wan ipaddr");
				_TR("statusSubnetMask",		"status subnet mask");
				_TR("statusDefaultGW",		"status default gateway");
				_TR("statusPrimaryDNS",		"status primary dns");
				_TR("statusSecondaryDNS",	"status secondary dns");
				_TR("statusWANMAC",			"status mac");

				_TR("statusLocalNet",		"status local network");
				_TR("statusLANIPAddr",		"status lan ipaddr");
				_TR("statusLocalNetmask",	"status local netmask");
				_TR("statusLANMAC",			"status mac");

				_TR("statusIPv6IntAddr",	"status internal ipv6 address");
				_TR("statusIPv6ExtAddr",	"status external ipv6 address");

			}

			function InitValues()
			{
				reloadPage();
				showWarning();
			}

			function reloadPage()
			{
				var reloader = function()
				{
					var defaultGW	= "<% getWanGateway(); %>";
					var ipv6b		= "<% getIPv6Built(); %>";
					var IPv6Int		= "<% getIPv6IntAddr(); %>";
					var IPv6Ext		= "<% getIPv6ExtAddr(); %>";
					ajaxModifyElementHTML("statusDGW", defaultGW);
					ajaxModifyElementHTML("IPv6IntAddr", IPv6Int);
					ajaxModifyElementHTML("IPv6ExtAddr", IPv6Ext);
					displayElement('defaultGW', (defaultGW != ""));
					displayElement('statusIPv6Int', (ipv6b == "1") && (IPv6Int != ""));
					displayElement('statusIPv6Ext', (ipv6b == "1") && (IPv6Ext != ""));
					showOpMode();
					showUpTime();
					initTranslation();
					timer = self.setTimeout('reloadPage();', 1000);
				}
				ajaxLoadElement("sysinfoTable", "/adm/sysinfo.asp", reloader);
			}

			function showOpMode()
			{
				var text;
				var opmode = document.getElementById('inpOperationMode').value;
				switch (opmode) {
					case '0':		text = _("opmode mode bridge"); 	break;
					case '1':		text = _("opmode mode gateway");	break;
					case '2':		text = _("opmode mode e");			break;
					case '3':		text = _("opmode mode a");			break;
					default:		text = _("opmode mode unknown");
				}
				ajaxModifyElementHTML('tdOperationMode', text);
			}
			
			function showUpTime()
			{
				var uptime, text;
				try {
					uptime = JSON.parse(document.getElementById('inpSysUpTime').value);
				} catch (e) {}

				text = uptime.day + " " + declOfNum(uptime.day, [ _("status system up time day1"), _("status system up time day2_4"), _("status system up time day5_0") ]);
				text += ", " + uptime.hour + " " + declOfNum(uptime.hour, [ _("status system up time hour1"), _("status system up time hour2_4"), _("status system up time hour5_0") ]);
				text += ", " + uptime.min + " " + declOfNum(uptime.min, [ _("status system up time min1"), _("status system up time min2_4"), _("status system up time min5_0") ]);
				text += ", " + uptime.sec + " " + declOfNum(uptime.sec, [ _("status system up time sec1"), _("status system up time sec2_4"), _("status system up time sec5_0") ]);

				ajaxModifyElementHTML('tdSysUpTime', text);
			}

			function declOfNum(num, titl)
			{
				cases = [2, 0, 1, 1, 1, 2];
				return titl[ (num % 100 > 4 && num % 100 < 20) ? 2 : cases[ (num % 10 < 5) ? num % 10 : 5] ];
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="InitValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td><h1 id="statusTitle"></h1>
					<p id="statusIntroduction"></p>
					<hr>
					<div id="sysinfoTable"></div>
				</td>
			</tr>
		</table>
	</body>
</html>
