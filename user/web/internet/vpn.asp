<!DOCTYPE html>
<html>
	<head>
		<title>VPN Tunnel setup</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_internet.js"></script>
		<script src="/lang/<% getLangDictionary(); %>/dict_hint.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script>
			var vpnServerIP 	= (NVRAM_vpnType != '0') ? NVRAM_vpnServer : '';
			var vpnACName		= (NVRAM_vpnType == '0') ? NVRAM_vpnServer : '';
			var table_vpn_params	= [ 'table_vpn_params01', 'table_vpn_params02', 'table_vpn_params03', 'table_vpn_params04' ];
			var stat_interval;

			function initTranslation() {
				_TR("vTitle",		"vpn title");
				_TR("vIntroduction",	"vpn introduction");
				_TR("vConfig",		"vpn config");
				_TR("vEnabled",		"vpn enable");
				_TR("vMode",		"vpn mode");
				_TR("vPPPoE",		"vpn pppoe");
				_TR("vPPTP",		"vpn pptp");
				_TR("vL2TP",		"vpn l2tp");
				_TR("vPPPoEiface",	"vpn pppoe iface");
				_TR("vServiceName",	"vpn service name");
				_TR("vAuthType",	"vpn auth type");
				_TR("vLanAuthLvl",	"vpn lanauth lvl");
				_TR("vKabOffline",	"vpn kabinet offline");
				_TR("vKabinet",		"vpn kabinet");
				_TR("vKabFull",		"vpn kabinet full");
				_TR("vUsername",	"vpn username");
				_TR("vPassword",	"vpn password");
				_TR("vMTUMRU",		"vpn mtu mru");
				_TR("vAuthTypeAuto",	"inet auto");
				_TR("vMTUAuto",		"inet auto");
				_TR("vCustom",		"inet custom");
				_TR("vGateway",		"inet gateway");
				_TR("vGWdisable",	"button disable");
				_TR("vGWenable",	"button enable");
				_TR("vLCPinterval",	"vpn lcp interval");
				_TR("vLCPfail",		"vpn lcp failure");
				_TR("vAddOpt",		"wan additional options");
				_TR("vAllowMPPE",	"vpn allow mppe");
				_TR("vPeerDNS",		"vpn peer dns");
				_TR("vAllowDebug",	"vpn allow debug");
				_TR("vEnableNAT",	"vpn enable nat");
				_TR("vAdaptiveLCP",	"vpn adaptive lcp");
				_TR("vPurePPPoE",	"vpn pure pppoe");
				_TR("vTestServer",	"vpn test server");
				_TR("vEnable",		"button enable");
				_TR("vDisable",		"button disable");
				_TR("vApplyConn",	"button apply connect");
				_TR("vCancel",		"button cancel");
				_TR("vReset",		"button reset");
			}

			function initValues() {
				var form = document.formVPNSetup;

				if (BUILD_KABINET != '0') {
					if (form.vpn_type.options.length == 3)
						form.vpn_type.options[form.vpn_type.options.length] = new Option(_("vpn kabinet auth"), '3');
					form.lanauth_access.value = NVRAM_LANAUTH_LVL;
				}

				if (NVRAM_vpnInterface == 'WAN')
					form.vpn_pppoe_iface.selectedIndex = '1'
				else if (NVRAM_vpnInterface == 'LAN')
					form.vpn_pppoe_iface.selectedIndex = '0';
					
				form.vpn_enabled.value			= NVRAM_vpnEnabled;
				form.vpn_mppe.checked			= NVRAM_vpnMPPE == 'on';
				form.vpn_peerdns.checked		= NVRAM_vpnPeerDNS == 'on';
				form.vpn_debug.checked			= NVRAM_vpnDebug == 'on';
				form.vpn_nat.checked			= NVRAM_vpnNAT == 'on';
				form.vpn_type.value			= NVRAM_vpnType;
				form.vpn_dgw.value			= NVRAM_vpnDGW;
				form.vpn_lcp.checked			= NVRAM_vpnEnableLCP == 'on';
				form.vpn_pure_pppoe.checked		= NVRAM_vpnPurePPPOE == '1';
				form.vpn_test_reachable.checked		= NVRAM_vpnTestReachable == '1';
				form.vpn_auth_type.value		= NVRAM_vpnAuthProtocol;
				form.vpn_lcp_errors.value		= NVRAM_vpnLCPFailure;
				form.vpn_lcp_interval.value		= NVRAM_vpnLCPInterval;
				form.vpn_mtu_field.value		= NVRAM_vpnMTU;
				form.vpn_user.value			= NVRAM_vpnUser;
				form.vpn_pass.value			= NVRAM_vpnPassword;
				form.vpn_pppoe_service.value		= NVRAM_vpnService;

				/* Check if option was set */
				var vpn_mtu_select = document.getElementById('vpn_mtu_select');
				for (var i = 0; i < vpn_mtu_select.options.length; i++)
					if (form.vpn_mtu_type.options[i].value == form.vpn_mtu.value) {
						form.vpn_mtu_type.value = form.vpn_mtu_select.options[i].value;
						break;
					}

				vpnSwitchClick(form);
				mtuChange(form);
				showWarning();
				initTranslation();

				ajaxLoadScript('/internet/vpn-stat.js');
				stat_interval = setInterval(function () { ajaxLoadScript('/internet/vpn-stat.js') }, 5000);

				disableControlsByAuth();
			}

			function checkValues(form) {
				if (form.vpn_pass.value.match(/[\s\$]/)) {
					alert(_("vpn alert password"));
					form.vpn_pass.focus();
					return false;
				}

				if (form.vpn_type.value != "0" && form.vpn_type.value != "3" ) {
					if (form.vpn_user.value.match(/[\s\$]/)) {
						alert(_("vpn alert username"));
						form.vpn_user.focus();
						return false;
					}

					if ((!validateIP(form.vpn_server, false)) && (!validateDNS(form.vpn_server, false))) {
						alert(_("vpn alert invalid ip"));
						form.vpn_type.focus();
						return false;
					}
				}

				if ((form.vpn_type.value != '0') || (!form.vpn_enabled.value == 'on'))
					form.vpn_pure_pppoe.checked = false;

				clearInterval(stat_interval);
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);

				return true;
			}

			function vpnSwitchClick(form) {
				enableElements( [
					form.vpn_server,
					form.vpn_auth_type,
					form.vpn_user,
					form.vpn_pass,
					form.vpn_mtu,
					form.vpn_mppe,
					form.vpn_peerdns,
					form.vpn_debug,
					form.vpn_nat,
					form.vpn_dgw,
					form.vpn_mtu_type,
					form.vpn_pppoe_iface,
					form.vpn_type,
					form.vpn_lcp,
					form.lanauth_access,
					form.vpn_pure_pppoe,
					form.vpn_pppoe_service,
					form.vpn_test_reachable,
					form.vpn_lcp_errors,
					form.vpn_lcp_interval
					], form.vpn_enabled.value == 'on' );
				displayElement( [ 'vpn_type' ], form.vpn_enabled.value == 'on' );

				if (form.vpn_enabled.value == 'on')
					_TR("vApplyConn", "button apply connect")
				else
					_TR("vApplyConn", "button apply");

					selectType(form);
			}

			function selectType(form) {
				var vpn_server_col	= document.getElementById("vpn_server_col");
				var pppoe_on		= form.vpn_type.value == '0';
				var pptp_on		= form.vpn_type.value == '1';
				var l2tp_on		= form.vpn_type.value == '2';
				var kabinet_on		= form.vpn_type.value == '3';

				displayElement('vpn_password_row', form.vpn_enabled.value == 'on');
				displayElement([ 'vpn_server_row', 'vpn_auth_type_row', 'vpn_user_row', 'vpn_mtu_row', 'vpn_dgw_row', 'vpn_mppe_row', table_vpn_params], !kabinet_on && form.vpn_enabled.value == 'on');
				displayElement([ 'vpn_pppoe_iface_row', 'vpn_pure_pppoe_cell', 'vpn_pppoe_service_row', 'vpn_pppoe_row' ], pppoe_on && form.vpn_enabled.value == 'on');
				displayElement('vpn_test_reachable', (pptp_on || l2tp_on) && form.vpn_enabled.value == 'on');
				displayElement('vpn_lanauth_lvl_row', kabinet_on && form.vpn_enabled.value == 'on');

				var vpn_server = _("vpn server col");
				if (form.vpn_type.value == '0') // PPPoE
					vpn_server = _("vpn server col ac");
				else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
					vpn_server = _("vpn server col host");

				if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2') || (form.vpn_type.value == '3'))
					form.vpn_server.value = vpnServerIP;
				else
					form.vpn_server.value = vpnACName;

				vpn_server_col.innerHTML = vpn_server;
			}

			function mtuChange(form) {
				var vpn_mtu_select = document.getElementById("vpn_mtu_select");
				var vpn_mtu_field  = document.getElementById("vpn_mtu_field");

				if (form.vpn_mtu_type.value == '1') {
					vpn_mtu_field.style.display = '';
					vpn_mtu_select.setAttribute("class", "half");
					vpn_mtu_field.setAttribute("class", "half");
				}
				else {
					vpn_mtu_select.setAttribute("class", "mid");
					vpn_mtu_field.style.display = 'none';
					form.vpn_mtu.value = form.vpn_mtu_type.value;
				}
			}

			function showHint(key) {
				var row = document.getElementById('vpn_hint_row');
				var form = document.formVPNSetup;
				var text = '<div class="hint"><font color="#0000ff"><b>' + _('hint title') + ':</b></font>&nbsp;';
				var show = true;
				switch(key) {
					case 'vpn':			text += _("hint vpn");					break;
					case 'vpn_enabled':		text += _("hint vpn enabled");				break;
					case 'vpn_vpn_status':		text += _("hint vpn status");				break;
					case 'vpn_user':		text += _("hint vpn user");				break;
					case 'vpn_password':		text += _("hint vpn password");				break;
					case 'vpn_auth_type':		text += _("hint vpn auth type");			break;
					case 'vpn_pppoe_iface':		text += _("hint vpn pppoe iface");			break;
					case 'vpn_pppoe_service':	text += _("hint vpn pppoe service");			break;
					case 'vpn_server':		if (form.vpn_type.value == '0') // PPPoE
										text += _("hint vpn pppoe");
									else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
										text += _("hint vpn pptp");			break;
					case 'vpn_mtu':			text += _("hint vpn mtu");				break;
					case 'vpn_mppe':		text += _("hint vpn mppe");				break;
					case 'vpn_dgw':			text += _("hint vpn dgw");
									text += '<p class="val">';
									if (form.vpn_dgw.value == '0')
										text += _("hint vpn dgw disabled");
									else if (form.vpn_dgw.value == '1')
										text += _("hint vpn dgw enabled");
									else if (form.vpn_dgw.value == '2')
										text += _("hint vpn dgw multiple");
									text += '</p>';						break;
					case 'vpn_peerdns':		text += _("hint vpn peerdns");				break;
					case 'vpn_test_reachable':	text += _("hint vpn test reachable");			break;
					case 'vpn_debug':		text += _("hint vpn debug");				break;
					case 'vpn_nat':			text += _("hint vpn nat");				break;
					case 'vpn_lcp':			text += _("hint vpn lcp");				break;
					case 'vpn_pure_pppoe':		text += _("hint vpn pure pppoe");			break;
					case 'vpn_lcp_errors':		text += _("hint vpn lcp errors");			break;
					case 'vpn_lcp_interval':	text += _("hint vpn lcp interval");			break;
					case 'vpn_type':		text += _("hint vpn type");
									text += '<p class="val">';
									if (form.vpn_type.value == "0")
										text += _("hint vpn type pppoe");
									else if (form.vpn_type.value == "1")
										text += _("hint vpn type pptp");
									else if (form.vpn_type.value == "2") {
										text += _("hint vpn type l2tp");
										text += '</p><p class="val">';
										text += _("hint vpn type l2tp client");
									}
									else if (form.vpn_type.value == "3")
										text += _("hint vpn type kabinet");
									text += '</p>';						break;
					case 'vpn_lanauth_access':	text += _("hint vpn lanauth");				break;
					default:					show = false;
				}

				if (show)
				{
					text += '</div>';
					row.innerHTML = text;
				}
			}

			function hideHint() {
				var row = document.getElementById('vpn_hint_row');
				row.innerHTML = '';
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 id="vTitle">Virtual Private Network setup</h1>
					<p id="vIntroduction"> This page is used to configure the VPN tunnel on your Router. </p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form action="/goform/formVPNSetup" method="POST" name="formVPNSetup">
					<table class="form">
						<col style="width: 40%" />
						<col style="width: 45%" />
						<col style="width: 15%" />
						<tbody>
							<tr>
								<td class="title" colspan="3" id="vConfig">VPN configuration</td>
							</tr>
							<tr>
								<td class="head" onMouseOver="showHint('vpn')" onMouseOut="hideHint()">VPN</td>
								<td onMouseOver="showHint('vpn_enabled')" onMouseOut="hideHint()">
									<select name="vpn_enabled" onChange="vpnSwitchClick(this.form)" class="mid auth-disable-user" >
										<option value="off" id="vDisable">Disable</option>
										<option value="on" id="vEnable">Enable</option>
									</select>
								<td onMouseOver="showHint('vpn_vpn_status')" onMouseOut="hideHint()" id="vpn_status_col" align="center">&nbsp;</td>
							</tr>
							<tr onMouseOver="showHint('vpn_type')" onMouseOut="hideHint()" id="vpn_type">
								<td class="head" id="vMode">VPN Mode:</td>
								<td colspan="2">
									<select disabled="disabled" name="vpn_type" onChange="selectType(this.form);" class="mid auth-disable-user" >
										<option value="0" selected="selected" id="vPPPoE">PPPoE client</option>
										<option value="1" id="vPPTP">PPTP  client</option>
										<option value="2" id="vL2TP">L2TP  client</option>
									</select>
								</td>
							</tr>
							<tr id="vpn_pppoe_row" style="display: none;" onMouseOver="showHint('vpn_pppoe_iface')" onMouseOut="hideHint()">
								<td class="head" id="vPPPoEiface">PPPoE interface:</td>
								<td colspan="2">
									<select disabled="disabled" name="vpn_pppoe_iface" class="mid auth-disable-user" >
										<option value="LAN" id="vpnIfLAN">LAN</option>
										<option value="WAN" id="vpnIfWAN">WAN</option>
									</select>
								</td>
							</tr>
							<tr id="vpn_server_row" onMouseOver="showHint('vpn_server')" onMouseOut="hideHint()">
								<td class="head" id="vpn_server_col">Host, IP, AC or APN name:</td>
								<td colspan="2"><input name="vpn_server" class="mid auth-disable-user" value="" disabled="disabled" type="text"></td>
							</tr>
							<tr id="vpn_pppoe_service_row" onMouseOver="showHint('vpn_pppoe_service')" onMouseOut="hideHint()">
								<td class="head" id="vServiceName">Service name:</td>
								<td colspan="2"><input name="vpn_pppoe_service" class="mid auth-disable-user" value="" disabled="disabled" type="text"></td>
							</tr>
							<tr id="vpn_auth_type_row" onMouseOver="showHint('vpn_auth_type')" onMouseOut="hideHint()" >
								<td class="head" id="vAuthType">Authentication method:</td>
								<td colspan="2">
									<select id="vpn_auth_type_select" disabled="disabled" name="vpn_auth_type" class="mid auth-disable-user">
										<option value="0" id="vAuthTypeAuto" selected="selected">AUTO</option>
										<option value="1">PAP</option>
										<option value="2">CHAP</option>
										<option value="3">MSCHAP</option>
									</select>
								</td>
							</tr>
							<tr id="vpn_lanauth_lvl_row" onMouseOver="showHint('vpn_lanauth_access')" onMouseOut="hideHint()">
								<td class="head" id="vLanAuthLvl">KABINET access level:</td>
								<td colspan="2">
									<select name="lanauth_access" class="mid auth-disable-user" disabled="disabled">
										<option value="0" id="vKabOffline">offline</option>
										<option value="1" id="vKabinet">kabinet</option>
										<option value="2" id="vKabFull">full</option>
									</select>
								</td>
							</tr>
							<tr id="vpn_user_row" onMouseOver="showHint('vpn_user')" onMouseOut="hideHint()" >
								<td class="head" id="vUsername">User name:</td>
								<td colspan="2"><input name="vpn_user" class="mid auth-disable-user" size="25" maxlength="60" value="" disabled="disabled" type="text"></td>
							</tr>
							<tr id="vpn_password_row" onMouseOver="showHint('vpn_password')" onMouseOut="hideHint()" >
								<td class="head" id="vPassword">Password:</td>
								<td colspan="2"><input name="vpn_pass" class="mid auth-disable-user" size="25" maxlength="60" value="" disabled="disabled" type="password"></td>
							</tr>
							<tr id="vpn_mtu_row" onMouseOver="showHint('vpn_mtu')" onMouseOut="hideHint()" class="auth-hide-user">
								<td class="head" id="vMTUMRU">MTU/MRU:</td>
								<td colspan="2">
									<input id="vpn_mtu_field" name="vpn_mtu" maxlength="4" disabled="disabled" type="text" class="half" style="display:none; " value="">
									<select id="vpn_mtu_select" disabled="disabled" name="vpn_mtu_type" onChange="mtuChange(this.form);" class="mid auth-disable-user" >
										<option value="AUTO" id="vMTUAuto">AUTO</option>
										<option value="1" selected="selected" id="vCustom">Custom</option>
										<option value="1492">1492</option>
										<option value="1440">1440</option>
										<option value="1400">1400</option>
										<option value="1300">1300</option>
										<option value="1200">1200</option>
										<option value="1100">1100</option>
										<option value="1000">1000</option>
									</select>
								</td>
							</tr>
							<tr id="vpn_dgw_row" onMouseOver="showHint('vpn_dgw')" onMouseOut="hideHint()" class="auth-hide-user" >
								<td class="head" id="vGateway">Default gateway:</td>
								<td colspan="2">
									<select disabled="disabled" name="vpn_dgw" class="mid auth-disable-user" >
										<option value="0" selected="selected" id="vGWdisable">Disabled</option>
										<option value="1" id="vGWenable">Enabled</option>
									</select>
								</td>
							</tr>
							<!-- VPN params -->
							<tr id="table_vpn_params02" onMouseOver="showHint('vpn_lcp_interval')" onMouseOut="hideHint()" class="auth-hide-user">
								<td class="head" id="vLCPinterval">LCP echo interval:</td>
								<td colspan="2">
									<select id="vpn_lcp_interval_select" disabled="disabled" name="vpn_lcp_interval" class="mid">
										<option value="5">5</option>
										<option value="10">10</option>
										<option value="15">15</option>
										<option value="20">20</option>
										<option value="25">25</option>
										<option value="30" selected="selected">30</option>
										<option value="35">35</option>
										<option value="40">40</option>
										<option value="45">45</option>
										<option value="50">50</option>
										<option value="55">55</option>
										<option value="60">60</option>
										<option value="65">65</option>
										<option value="70">70</option>
										<option value="75">75</option>
										<option value="80">80</option>
										<option value="85">85</option>
										<option value="90">90</option>
										<option value="95">95</option>
										<option value="100">100</option>
									</select>
								</td>
							</tr>
							<tr id="table_vpn_params03" onMouseOver="showHint('vpn_lcp_errors')" onMouseOut="hideHint()" class="auth-hide-user">
								<td class="head" id="vLCPfail">LCP echo failure:</td>
								<td colspan="2">
									<select id="vpn_lcp_errors_select" disabled="disabled" name="vpn_lcp_errors" class="mid">
										<option value="5" selected="selected">5</option>
										<option value="10">10</option>
										<option value="15">15</option>
										<option value="20">20</option>
										<option value="25">25</option>
										<option value="30">30</option>
										<option value="35">35</option>
										<option value="40">40</option>
										<option value="45">45</option>
										<option value="50">50</option>
										<option value="55">55</option>
										<option value="60">60</option>
										<option value="65">65</option>
										<option value="70">70</option>
										<option value="75">75</option>
										<option value="80">80</option>
										<option value="85">85</option>
										<option value="90">90</option>
										<option value="95">95</option>
										<option value="100">100</option>
								  </select>
								</td>
							</tr>
						</tbody>
					</table>
					<table class="form auth-hide-user" id="table_vpn_params04">
						<col style="width: 50%" />
						<col style="width: 50%" />
						<tbody>	
							<tr>
								<td colspan="2" class="title" id="vAddOpt">Additional options</td>
							</tr>
							<tr id="vpn_mppe_row">
								<td class="head" onMouseOver="showHint('vpn_mppe')" onMouseOut="hideHint()">
									<input disabled="disabled" name="vpn_mppe" type="checkbox">
									<span id="vAllowMPPE" style="position: absolute; margin-top: 3px;">Allow MPPE</span>
								</td>
								<td class="head" onMouseOver="showHint('vpn_peerdns')" onMouseOut="hideHint()">
									<input disabled="disabled" name="vpn_peerdns" type="checkbox">
									<span id="vPeerDNS" style="position: absolute; margin-top: 3px;">Peer DNS</span>
								</td>
							</tr>
							<tr>
								<td class="head" onMouseOver="showHint('vpn_debug')" onMouseOut="hideHint()">
									<input disabled="disabled" name="vpn_debug" type="checkbox">
									<span id="vAllowDebug" style="position: absolute; margin-top: 3px;">Allow debug</span>
								</td>
								<td class="head" onMouseOver="showHint('vpn_nat')" onMouseOut="hideHint()">
									<input disabled="disabled" name="vpn_nat" type="checkbox">
									<span id="vEnableNAT" style="position: absolute; margin-top: 3px;">Enable NAT</span>
								</td>
							</tr>
							<tr>
								<td class="head" onMouseOver="showHint('vpn_lcp')" onMouseOut="hideHint()">
									<input disabled="disabled" name="vpn_lcp" type="checkbox">
									<span id="vAdaptiveLCP" style="position: absolute; margin-top: 3px;">Adaptive LCP</span>
								</td>
								<td class="head" onMouseOver="showHint('vpn_pure_pppoe')" onMouseOut="hideHint()" id="vpn_pure_pppoe_cell">
									<input disabled="disabled" name="vpn_pure_pppoe" type="checkbox">
									<span id="vPurePPPoE" style="position: absolute; margin-top: 3px;">Pure PPPoE</span></td>
								<td class="head" id="vpn_test_reachable" onMouseOver="showHint('vpn_test_reachable')" onMouseOut="hideHint()">
									<input disabled="disabled" name="vpn_test_reachable" type="checkbox">
									<span id="vTestServer" style="position: absolute; margin-top: 3px;">Test server reachable</span>
								</td>
							</tr>
						</tbody>
					</table>
					<table class="buttons auth-hide-user">
						<tr>
							<td>
								<input name="lanauth_pass_changed" type="hidden">
								<input value="0" name="reset" type="hidden">
								<input class="mid"    id="vApplyConn" value="Apply and connect" name="save" type="submit" onClick="return checkValues(this.form);" ><input class="normal" id="vCancel" value="Cancel" name="save" type="button" onClick="window.location.reload();" ><input class="normal" id="vReset" value="Reset" name="reset_button" onClick="resetValues(this.form);" type="button">
							</td>
						</tr>
					</table>
				</form>
				<br>
				<div id="vpn_hint_row"></div>
				<div class="whitespace"></div>
			</tr>
		</table>
	</body>
</html>