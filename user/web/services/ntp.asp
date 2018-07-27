<!DOCTYPE html>
<html>
	<head>
		<title>System Management</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_services.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			function initTranslation() {
				_TR("ntpTitle",			"services ntp title");
				_TR("ntpIntroduction",		"services ntp introduction");
				_TR("ntpSetup",			"services ntp setup");
				_TR("ntpEnabled",		"services ntp enabled");
				_TR("ntpDisable",		"button disable");
				_TR("ntpEnable",		"button enable");
				_TR("ntpCurrentTime",		"services ntp current time");
				_TR("ntpTimeZone",		"services ntp timezone");
				_TR("ntpServer",		"services ntp server");
				_TR("ntpApply",		"button apply");
				_TR("ntpCancel",		"button cancel");
				_TR("ntpSyncWithHost",		"services ntp sync");
			}

			function initValues() {
				var cTime = new Date();

				document.getElementById('time_zone').value			= NVRAM_TZ;
				document.getElementById('ntp_enabled').options.selectedIndex	= +NVRAM_NTPEnabled;
				document.getElementById('NTPServerIP').value			= NVRAM_NTPServerIP; 

				if (BUILD_DATE == '1') {
					showElement('div_date');
					document.getElementById('ntpcurrenttime').disabled	= false;
					document.getElementById('ntpcurrenttime').value		= sprintf('%02d:%02d:%02d %02d.%02d.%04d',
														cTime.getHours(), cTime.getMinutes(), cTime.getSeconds(),
														cTime.getDate(), cTime.getMonth()+1, cTime.getFullYear());
				}
				else {
					hideElement('div_date');
					document.getElementById('ntpcurrenttime').disabled = true;
				}

				ntpChange();
				showWarning();
				initTranslation();
			}

			function syncWithHost() {
				var cTime	= new Date();
				var time	= sprintf('"%04d-%02d-%02d %02d:%02d:%02d"',
								cTime.getFullYear(), cTime.getMonth()+1, cTime.getDate(),
								cTime.getHours(), cTime.getMinutes(), cTime.getSeconds());

				ajaxPostRequest("/goform/NTPSyncWithHost", time, true);
			}

			function ntpChange() {
				enableElements([	document.getElementById('manNTPSyncWithHost'),
							document.getElementById('time_zone'),
							document.getElementById('NTPServerIP')], document.getElementById('ntp_enabled').options.selectedIndex != 0);
				displayElement([ 'div_date', 'div_tz', 'div_server'], document.getElementById('ntp_enabled').options.selectedIndex != 0);
				displayServiceStatus([[ NVRAM_NTPEnabled, 'ntpd', 'ntpd' ]]);
			}

			// function from http://forums.devshed.com/t39065/s84ded709f924610aa44fff827511aba3.html
			// author appears to be Robert Pollard
			function sprintf() {
				if (!arguments || arguments.length < 1 || !RegExp)
					return;

				var str = arguments[0];
				var re = /([^%]*)%('.|0|\x20)?(-)?(\d+)?(\.\d+)?(%|b|c|d|u|f|o|s|x|X)(.*)/;
				var a = b = [], numSubstitutions = 0, numMatches = 0;

				while (a = re.exec(str))
				{
					var leftpart = a[1], pPad = a[2], pJustify = a[3], pMinLength = a[4];
					var pPrecision = a[5], pType = a[6], rightPart = a[7];

					numMatches++;
					if (pType == '%')
						subst = '%';
					else
					{
						numSubstitutions++;
						if (numSubstitutions >= arguments.length)
						{
							alert('Error! Not enough function arguments (' + (arguments.length - 1)
								+ ', excluding the string)\n'
								+ 'for the number of substitution parameters in string ('
								+ numSubstitutions + ' so far).');
						}

						var param = arguments[numSubstitutions];
						var pad = '';

						if (pPad && pPad.substr(0,1) == "'") pad = leftpart.substr(1,1);
						else if (pPad) pad = pPad;

						var justifyRight = true;
						if (pJustify && pJustify === "-") justifyRight = false;

						var minLength = -1;
						if (pMinLength) minLength = parseInt(pMinLength);

						var precision = -1;
						if (pPrecision && pType == 'f')
							precision = parseInt(pPrecision.substring(1));

						var subst = param;
						switch (pType)
						{
							case 'b':
								subst = parseInt(param).toString(2);
								break;
							case 'c':
								subst = String.fromCharCode(parseInt(param));
								break;
							case 'd':
								subst = parseInt(param) ? parseInt(param) : 0;
								break;
							case 'u':
								subst = Math.abs(param);
								break;
							case 'f':
								subst = (precision > -1) ? Math.round(parseFloat(param) * Math.pow(10, precision)) / Math.pow(10, precision) : parseFloat(param);
								break;
							case 'o':
								subst = parseInt(param).toString(8);
								break;
							case 's':
								subst = param;
								break;
							case 'x':
								subst = ('' + parseInt(param).toString(16)).toLowerCase();
								break;
							case 'X':
								subst = ('' + parseInt(param).toString(16)).toUpperCase();
								break;
						}

						var padLeft = minLength - subst.toString().length;
						if (padLeft > 0)
						{
							var arrTmp = new Array(padLeft+1);
							var padding = arrTmp.join(pad?pad:" ");
						}
						else
						{
							var padding = "";
						}
					}
				
					str = leftpart + padding + subst + rightPart;
				}
				return str;
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="ntpTitle">NTP Management</h1>
					<p id="ntpIntroduction">Here you can configure time synchronization.</p>
					<hr />
					<!-- ================= NTP Settings ================= -->
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="NTP" action="/goform/NTP" OnSubmit="ajaxShowTimer(this, 'timerReloader', _('message apply'), 5);">
					<table class="form">
						<col style="width: 40%"/>
						<col style="width: 50%"/>
						<col style="width: 10%"/>
						<tbody>
							<tr>
								<td class="title" colspan="3" id="ntpSetup">NTP Settings</td>
							</tr>
							<tr id="ntpd">
								<td class="head" id="ntpEnabled" style="width: 40%">NTP synchronization</td>
								<td>
									<select name="ntp_enabled" id="ntp_enabled" class="mid" onChange="ntpChange(this.form);">
										<option value="0" id="ntpDisable">Disable</option>
										<option value="1" id="ntpEnable">Enable</option>
									</select>
								</td>
								<td style="text-align: center"></td>
							</tr>
							<tr id="div_date">
								<td class="head" id="ntpCurrentTime">Current Host Time</td>
								<td colspan="2">
									<input class="wide" name="ntpcurrenttime" id="ntpcurrenttime" type="text" readonly="1">&nbsp;&nbsp;
									<input type="button" class="normal" value="Sync with host" id="ntpSyncWithHost" name="manNTPSyncWithHost" onClick="syncWithHost()">
								</td>
							</tr>
							<tr id="div_tz">
								<td class="head" id="ntpTimeZone">Time Zone:</td>
								<td colspan="2">
									<select name="time_zone" id="time_zone" class="wide">
										<option value="UCT_-11" id="ntpMidIsland"			>(UTC-11:00) Midway Island, Samoa</option>
										<option value="UCT_-10" id="ntpHawaii"				>(UTC-10:00) Hawaii</option>
										<option value="UCT_-09" id="ntpAlaska"				>(UTC-09:00) Alaska</option>
										<option value="UCT_-08" id="ntpPacific"				>(UTC-08:00) Pacific Time</option>
										<option value="UCT_-07" id="ntpMountain"			>(UTC-07:00) Mountain Time</option>
										<option value="MST_-07" id="ntpArizona"				>(UTC-07:00) Arizona</option>
										<option value="UCT_-06" id="ntpCentral"				>(UTC-06:00) Central Time</option>
										<option value="EST_-06" id="ntpMidUS"				>(UTC-06:00) Middle America</option>
										<option value="UCT_-05" id="ntpIndianaEast"			>(UTC-05:00) Indiana East, Colombia</option>
										<option value="EST_-05" id="ntpEastern"				>(UTC-05:00) Eastern Time</option>
										<option value="AST_-04" id="ntpAtlantic"			>(UTC-04:00) Atlantic Time, Brazil West</option>
										<option value="UCT_-04" id="ntpBolivia"				>(UTC-04:00) Bolivia, Venezuela</option>
										<option value="UCT_-03" id="ntpGuyana"				>(UTC-03:00) Guyana</option>
										<option value="EBS_-03" id="ntpBrazilEast"			>(UTC-03:00) Brazil East, Greenland</option>
										<option value="NOR_-02" id="ntpMidAtlantic"			>(UTC-02:00) Mid-Atlantic</option>
										<option value="EUT_-01" id="ntpAzoresIslands"			>(UTC-01:00) Azores Islands</option>
										<option value="UCT_000" id="ntpGambia"				>(UTC) Gambia, Liberia, Morocco</option>
										<option value="GMT_000" id="ntpEngland"				>(UTC) England</option>
										<option value="MET_001" id="ntpCzechRepublic"			>(UTC+01:00) Czech Republic, N</option>
										<option value="MEZ_001" id="ntpGermany"				>(UTC+01:00) Germany</option>
										<option value="UCT_001" id="ntpTunisia"				>(UTC+01:00) Tunisia</option>
										<option value="EET_002" id="ntpGreece"				>(UTC+02:00) Greece, Turkey</option>
										<option value="EET-2EEST,M3.5.0/3,M10.5.0/4" id="ntpKiev"	>(UTC+02:00) Ukraine (Kiev), Latvia (Riga)</option>
										<option value="EET-2EEST,M3.5.0,M10.5.0/3" id=""		>(UTC+02:00) Belorussia (Minsk)</option>
										<option value="SAS_002" id="ntpSouthAfrica"			>(UTC+02:00) South Africa</option>
										<option value="IDDT_002"						>(UTC+02:00) Israel (Tel Aviv)</option>
										<option value="EET-2"						>(UTC+02:00) Russia (Kaliningrad)</option>
										<option value="IST_003" id="ntpIraq"				>(UTC+03:00) Iraq, Jordan, Kuwait</option>
										<option value="MSK-3" id="ntpMoscowWinter"			>(UTC+03:00) Russia (Moscow, St.-Petersburg)</option>
										<option value="SAMT-4"						>(UTC+04:00) Russia (Samara)</option>
										<option value="AMT-4AMST,M3.5.0,M10.5.0/3" id="ntpArmenia"	>(UTC+04:00) Armenia (Yerevan)</option>
										<option value="AZT-4AZST,M3.5.0/4,M10.5.0/5"			>(UTC+04:00) Azerbaydzhan (Baku)</option>
										<option value="GET-4"						>(UTC+04:00) Georgia (Tbilisi)</option>
										<option value="TMT-5"						>(UTC+05:00) Ashgabad</option>
										<option value="UZT-5"						>(UTC+05:00) Samarkand, Tashkent</option>
										<option value="UCT_005" id="ntpPakistan"			>(UTC+05:00) Pakistan</option>
										<option value="YEKT-5"						>(UTC+05:00) Russia (Yekaterinburg)</option>
										<option value="UCT_006" id="ntpBangladesh"			>(UTC+06:00) Bangladesh</option>
										<option value="KGT-6"						>(UTC+06:00) Kyrgyzstan (Bishkek)</option>
										<option value="OMST-6"						>(UTC+06:00) Russia (Omsk)</option>
										<option value="NOVT-7"						>(UTC+07:00) Russia (Novosibirsk)</option>
										<option value="KRAT-7"						>(UTC+07:00) Russia (Krasnoyarsk)</option>
										<option value="UCT_007" id="ntpThailand"			>(UTC+07:00) Thailand</option>
										<option value="CST_008" id="ntpChinaCoast"			>(UTC+08:00) China Coast, Hong Kong</option>
										<option value="CCT_008" id="ntpTaipei"				>(UTC+08:00) Taipei</option>
										<option value="SST_008" id="ntpSingapore"			>(UTC+08:00) Singapore</option>
										<option value="AWS_008" id="ntpAustraliaWA"			>(UTC+08:00) Australia (WA)</option>
										<option value="ULAT-8"						>(UTC+08:00) Mongolia (Ulan-Bator)</option>
										<option value="IRKT-8"						>(UTC+08:00) Russia (Irkutsk)</option>
										<option value="JST_009" id="ntpJapan"				>(UTC+09:00) Japan</option>
										<option value="KST_009" id="ntpKorean"				>(UTC+09:00) North Korea, South Korea</option>
										<option value="YAKT-09"						>(UTC+09:00) Russia (Yakutsk)</option>
										<option value="UCT_010" id="ntpGuam"				>(UTC+10:00) Guam</option>
										<option value="VLAT-10"						>(UTC+10:00) Russia (Vladivostok)</option>
										<option value="MAGT-11"						>(UTC+11:00) Russia (Magadan)</option>
										<option value="SAKT-11"						>(UTC+11:00) Russia (Sakhalin)</option>
										<option value="UCT_011" id="ntpSolomonIslands"			>(UTC+11:00) Solomon Islands</option>
										<option value="AES_011" id="ntpAustraliaQLD"			>(UTC+11:00) Australia (QLD, TAS,NSW,ACT,VIC)</option>
										<option value="PETT-12"						>(UTC+12:00) Russia (Kamchatka)</option>
										<option value="UCT_012" id="ntpFiji"				>(UTC+12:00) Fiji</option>
										<option value="NZS_012" id="ntpNewZealand"			>(UTC+12:00) New Zealand</option>
									</select>
								</td>
							</tr>
							<tr id="div_server">
								<td class="head" id="ntpServer">NTP Server</td>
								<td colspan="2">
									<input class="wide" name="NTPServerIP" id="NTPServerIP" type="text">
									<br>
									&nbsp;&nbsp;<font color="#808080">ex:&nbsp;time.nist.gov</font><br>
									&nbsp;&nbsp;<font color="#808080">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ru.pool.ntp.org</font><br>
									&nbsp;&nbsp;<font color="#808080">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;time.stdtime.gov.tw</font>
								</td>
							</tr>
						</tbody>
					</table>
					<table class="buttons">
						<tr>
							<td>
								<input type="hidden" value="/services/ntp.asp" name="submit-url"><input type="submit" class="normal" value="Apply"  id="ntpApply"><input type="reset"  class="normal" value="Cancel" id="ntpCancel" onClick="window.location.reload();">
							</td>
						</tr>
					</table>
					</form>
					<div class="whitespace"></div>
				</td>
			</tr>
		</table>
	</body>
</html>