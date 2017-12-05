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
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_services.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			function initTranslation() {
				_TR("manTitle",			"services ddns title");
				_TR("manIntroduction",		"services ddns introduction");
				_TR("manDdnsSet",		"services ddns setup");
				_TR("DdnsProvider",		"services ddns provider");
				_TR("manDdnsNone",		"services ddns none");
				_TR("manDdnsAccount",		"services l2tp login");
				if (document.getElementById('DDNSProvider').value != "freedns.afraid.org")
					_TR("manDdnsPasswd", "services l2tp password");
				else
					_TR("manDdnsPasswd", "services ddns key");
				_TR("manDdns",			"services ddns");
				_TR("manDdnsApply",		"button apply");
				_TR("manDdnsCancel",		"button cancel");
			}
			
			function initValues() {
				switch (NVRAM_DDNSProvider) {
					case 'none':			document.getElementById('DDNSProvider').options.selectedIndex = 0;	break;
					case 'dyndns.org':		document.getElementById('DDNSProvider').options.selectedIndex = 1;	break;
					case 'freedns.afraid.org':	document.getElementById('DDNSProvider').options.selectedIndex = 2;	break;
					case 'zoneedit.com':		document.getElementById('DDNSProvider').options.selectedIndex = 3;	break;
					case 'no-ip.com':		document.getElementById('DDNSProvider').options.selectedIndex = 4;	break;
					default:			document.getElementById('DDNSProvider').options.selectedIndex = 0;
				}
				document.getElementById('Account').value	= NVRAM_DDNSAccount;
				document.getElementById('Password').value	= NVRAM_DDNSPassword;
				document.getElementById('DDNS').value		= NVRAM_DDNS;

				DDNSupdateState();
				showWarning();
				displayServiceStatus([[ NVRAM_DDNSProvider, 'inadyn', 'inadyn' ]]);
			}

			function DDNSFormCheck() {
				if (document.getElementById('DDNSProvider').value != 'none' && document.getElementById('DDNSProvider').value != 'freedns.afraid.org' &&
					(document.getElementById('Account').value == '' || document.getElementById('Password').value == '' || document.getElementById('DDNS').value == '')) {
					alert(_("services ddns specify"));
					return false;
				}
				else if (document.getElementById('DDNSProvider').value == 'freedns.afraid.org' && 
						(document.getElementById('Password').value == '' || document.getElementById('DDNS').value == '')) {
					alert(_("services ddns specify"));
					return false;
				}
				ajaxShowTimer(document.DDNS, 'timerReloader', _('message apply'), 5);
				return true;
			}

			function DDNSupdateState() {
				enableElements([ document.getElementById('Password'), document.getElementById('DDNS') ], document.getElementById('DDNSProvider').options.selectedIndex != 0);
				displayElement([ 'div_password', 'div_dynname' ], document.getElementById('DDNSProvider').options.selectedIndex != 0 );
				enableElements(document.getElementById('Account'), document.getElementById('DDNSProvider').options.selectedIndex != 0 && document.getElementById('DDNSProvider').value != 'freedns.afraid.org');
				displayElement('div_login', document.getElementById('DDNSProvider').options.selectedIndex != 0 && document.getElementById('DDNSProvider').value != 'freedns.afraid.org');
				initTranslation();
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues()">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="manTitle">DDNS Settings</h1>
					<p id="manIntroduction">Here you can configure Dynamic DNS settings.</p>
					<hr>
					<!-- ================= DDNS  ================= -->
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="post" name="DDNS" action="/goform/DDNS" OnSubmit="return DDNSFormCheck(this)">
					<table id="div_ddns" class="form">
						<col style="width: 40%"/>
						<col style="width: 50%"/>
						<col style="width: 10%"/>
						<tbody>
							<tr>
								<td class="title" colspan="3" id="manDdnsSet">DDNS Settings</td>
							</tr>
							<tr id="inadyn">
								<td class="head" id="DdnsProvider">Dynamic DNS Provider</td>
								<td>
									<select onChange="DDNSupdateState()" name="DDNSProvider" id="DDNSProvider" class="mid">
										<option value="none" id="manDdnsNone"> None </option>
										<option value="dyndns.org"> dyndns.org </option>
										<option value="freedns.afraid.org"> freedns.afraid.org </option>
										<option value="zoneedit.com"> www.zoneedit.com </option>
										<option value="no-ip.com"> www.no-ip.com </option>
									</select>
								</td>
								<td style="text-align: center;">&nbsp;</td>
							</tr>
							<tr id="div_login">
								<td class="head" id="manDdnsAccount">Login</td>
								<td colspan="2"><input class="mid" name="Account" id="Account" type="text"></td>
							</tr>
							<tr id="div_password">
								<td class="head" id="manDdnsPasswd">Password</td>
								<td colspan="2"><input class="mid" name="Password" id="Password" type="password"></td>
							</tr>
							<tr id="div_dynname">
								<td class="head" id="manDdns">Dynamic Name</td>
								<td colspan="2"><input class="mid" name="DDNS" id="DDNS" type="text"></td>
							</tr>
						</tbody>
					</table>
					<table id="div_ddns_submit" class="buttons">
						<tr>
							<td>
								<input type="hidden" name="submit-url" value="/services/ddns.asp" >
								<input type="submit" class="normal" value="Apply" id="manDdnsApply"><input type="reset" class="normal" value="Cancel" id="manDdnsCancel" onClick="window.location.reload()">
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