<!DOCTYPE html>
<html>
	<head>
		<title>Dynamic DNS</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			function initTranslation() {
				_TR("manTitle",			"services ddns title");
				_TR("manIntroduction",	"services ddns introduction");
				_TR("manDdnsSet",		"services ddns setup");
				_TR("DdnsProvider",		"services ddns provider");
				_TR("manDdnsNone",		"services ddns none");
				_TR("manDdnsAccount",	"services l2tp login");
				if(document.DDNS.DDNSProvider.value != "freedns.afraid.org")
					_TR("manDdnsPasswd", "services l2tp password");
				else
					_TR("manDdnsPasswd", "services ddns key");
				_TR("manDdns",			"services ddns");
				_TRV("manDdnsApply",	"button apply");
				_TRV("manDdnsCancel",	"button cancel");
			}
			
			function initValues() {
				switch (NVRAM_DDNSProvider) {
					case 'none':				document.DDNS.DDNSProvider.options.selectedIndex = 0;	break;
					case 'dyndns.org':			document.DDNS.DDNSProvider.options.selectedIndex = 1;	break;
					case 'freedns.afraid.org':	document.DDNS.DDNSProvider.options.selectedIndex = 2;	break;
					case 'zoneedit.com':		document.DDNS.DDNSProvider.options.selectedIndex = 3;	break;
					case 'no-ip.com':			document.DDNS.DDNSProvider.options.selectedIndex = 4;	break;
					default:					document.DDNS.DDNSProvider.options.selectedIndex = 0;
				}
				document.DDNS.Account.value		= NVRAM_DDNSAccount;
				document.DDNS.Password.value	= NVRAM_DDNSPassword;
				document.DDNS.DDNS.value		= NVRAM_DDNS;
				
				DDNSupdateState();
				showWarning();
			}

			function DDNSFormCheck(form) {
				if (document.DDNS.DDNSProvider.value != "none" && document.DDNS.DDNSProvider.value != "freedns.afraid.org" &&
					(document.DDNS.Account.value == "" || document.DDNS.Password.value == "" || document.DDNS.DDNS.value == "")) {
					alert(_("services ddns specify"));
					return false;
				} else if (document.DDNS.DDNSProvider.value == "freedns.afraid.org" && 
						  (document.DDNS.Password.value == "" || document.DDNS.DDNS.value == "")) {
					alert(_("services ddns specify"));
					return false;
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function DDNSupdateState() {
				var form = document.DDNS;
				enableElements( [ form.Password, form.DDNS ], form.DDNSProvider.options.selectedIndex != 0 );
				displayElement( [ "div_password", "div_dynname" ], form.DDNSProvider.options.selectedIndex != 0 );
				enableElements( form.Account, form.DDNSProvider.options.selectedIndex != 0 && form.DDNSProvider.value != "freedns.afraid.org");
				displayElement( "div_login", form.DDNSProvider.options.selectedIndex != 0 && form.DDNSProvider.value != "freedns.afraid.org");
				displayServiceStatus();
				initTranslation();
			}

			function displayServiceHandler(response) {
				var form = document.DDNS;

				var services = [
					// turned_on, row_id, daemon_id
					[ NVRAM_DDNSProvider != 0, 'inadyn', 'inadyn' ]
				];

				// Create associative array
				var tmp = response.split(',');
				var daemons = [];
				for (var i = 0; i < tmp.length; i++)
					daemons[tmp[i]] = 1;

				// Now display all services
				for (var i=0; i<services.length; i++)
				{
					var service = services[i];
					var row = document.getElementById(service[1]);
					var tds = [];
					for (var j = 0; j < row.childNodes.length; j++)
						if (row.childNodes[j].nodeName == 'TD')
							tds.push(row.childNodes[j]);

					if (row != null) {
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

			function displayServiceStatus() {
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<table class="body">
			<tr id="warning"></tr>
			<tr>
				<td>
					<h1 id="manTitle">DDNS Settings</h1>
					<p id="manIntroduction">Here you can configure Dynamic DNS settings.</p>
					<hr>
					<!-- ================= DDNS  ================= -->
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="post" name="DDNS" action="/goform/DDNS" OnSubmit="return DDNSFormCheck(this)">
					<table id="div_ddns" class="form">
						<tr>
							<td class="title" colspan="3" id="manDdnsSet">DDNS Settings</td>
						</tr>
						<tr id="inadyn">
							<td class="head" style="width: 40%" id="DdnsProvider">Dynamic DNS Provider</td>
							<td>
								<select onChange="DDNSupdateState()" name="DDNSProvider" class="mid">
									<option value="none" id="manDdnsNone"> None </option>
									<option value="dyndns.org"> dyndns.org </option>
									<option value="freedns.afraid.org"> freedns.afraid.org </option>
									<option value="zoneedit.com"> www.zoneedit.com </option>
									<option value="no-ip.com"> www.no-ip.com </option>
								</select>
							</td>
							<td style="width: 56px;">&nbsp;</td>
						</tr>
						<tr id="div_login">
							<td class="head" id="manDdnsAccount">Login</td>
							<td colspan="2"><input class="mid" name="Account" type="text"></td>
						</tr>
						<tr id="div_password">
							<td class="head" id="manDdnsPasswd">Password</td>
							<td colspan="2"><input class="mid" name="Password" type="password"></td>
						</tr>
						<tr id="div_dynname">
							<td class="head" id="manDdns">Dynamic Name</td>
							<td colspan="2"><input class="mid" name="DDNS" type="text"></td>
						</tr>
					</table>
					<table id="div_ddns_submit" class="buttons">
						<tr>
							<td>
								<input type="hidden" name="submit-url" value="/services/ddns.asp" >
								<input type="submit" class="normal" value="Apply" id="manDdnsApply">&nbsp;&nbsp;
								<input type="reset" class="normal" value="Cancel" id="manDdnsCancel" onClick="window.location.reload()">
							</td>
						</tr>
					</table>
					</form>
					<div class="whitespace">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
