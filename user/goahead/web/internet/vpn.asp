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
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/lang/b28n.js"></script>
		<script>
			Butterlate.setTextDomain("network");
			Butterlate.setTextDomain("hint");
			Butterlate.setTextDomain("buttons");

			var vpnServerIP 		= (NVRAM_vpnType != '0') ? NVRAM_vpnServer : '';
			var vpnACName			= (NVRAM_vpnType == '0') ? NVRAM_vpnServer : '';
			var table_vpn_params	= [ 'table_vpn_params01', 'table_vpn_params02', 'table_vpn_params03', 'table_vpn_params04' ];

			function initTranslation() {
				_TR("vTitle",			"vpn title");
				_TR("vIntroduction",	"vpn introduction");
				_TR("vConfig",			"vpn config");
				_TR("vEnabled",			"vpn enable");
				_TR("vMode",			"vpn mode");
				_TR("vPPPoE",			"vpn pppoe");
				_TR("vPPTP",			"vpn pptp");
				_TR("vL2TP",			"vpn l2tp");
				_TR("vPPPoEiface",		"vpn pppoe iface");
				_TR("vServerCol",		"vpn server col");
				_TR("vServiceName",		"vpn service name");
				_TR("vAuthType",		"vpn auth type");
				_TR("vLanAuthLvl",		"vpn lanauth lvl");
				_TR("vKabOffline",		"vpn kabinet offline");
				_TR("vKabinet",			"vpn kabinet");
				_TR("vKabFull",			"vpn kabinet full");
				_TR("vUsername",		"vpn username");
				_TR("vPassword",		"vpn password");
				_TR("vMTUMRU",			"vpn mtu mru");
				_TR("vAuthTypeAuto",	"inet auto");
				_TR("vMTUAuto",			"inet auto");
				_TR("vCustom",			"routing custom");
				_TR("vGateway",			"inet gateway");
				_TR("vGWdisable",		"button disable");
				_TR("vGWenable",		"button enable");
				_TR("vLCPinterval",		"vpn lcp interval");
				_TR("vLCPfail",			"vpn lcp failure");
				_TR("vAddOpt",			"wan additional options");
				_TR("vAllowMPPE",		"vpn allow mppe");
				_TR("vPeerDNS",			"vpn peer dns");
				_TR("vAllowDebug",		"vpn allow debug");
				_TR("vEnableNAT",		"vpn enable nat");
				_TR("vAdaptiveLCP",		"vpn adaptive lcp");
				_TR("vPurePPPoE",		"vpn pure pppoe");
				_TR("vTestServer",		"vpn test server");
				_TRV("vApplyConn",		"button apply connect");
				_TRV("vCancel",			"button cancel");
				_TRV("vReset",			"button reset");
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
					
				form.vpn_enabled.checked		= NVRAM_vpnEnabled == 'on';
				form.vpn_mppe.checked			= NVRAM_vpnMPPE == 'on';
				form.vpn_peerdns.checked		= NVRAM_vpnPeerDNS == 'on';
				form.vpn_debug.checked			= NVRAM_vpnDebug == 'on';
				form.vpn_nat.checked			= NVRAM_vpnNAT == 'on';
				form.vpn_type.value				= NVRAM_vpnType;
				form.vpn_dgw.value				= NVRAM_vpnDGW;
				form.vpn_lcp.checked			= NVRAM_vpnEnableLCP == 'on';
				form.vpn_pure_pppoe.checked		= NVRAM_vpnPurePPPOE == '1';
				form.vpn_test_reachable.checked	= NVRAM_vpnTestReachable == '1';
				form.vpn_auth_type.value		= NVRAM_vpnAuthProtocol;
				form.vpn_lcp_errors.value		= NVRAM_vpnLCPFailure;
				form.vpn_lcp_interval.value		= NVRAM_vpnLCPInterval;
				form.vpn_mtu_field.value		= NVRAM_vpnMTU;
				form.vpn_user.value				= NVRAM_vpnUser;
				form.vpn_pass.value				= NVRAM_vpnPassword;
				form.vpn_pppoe_service.value	= NVRAM_vpnService;

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
				setInterval(function () { ajaxLoadScript('/internet/vpn-stat.js') }, 5000); 
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

				if ((form.vpn_type.value != '0') || (!form.vpn_enabled.checked))
					form.vpn_pure_pppoe.checked = false;
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
					], form.vpn_enabled.checked );
				displayElement( [ 'vpn_type' ], form.vpn_enabled.checked );

				if (form.vpn_enabled.checked)
					_TRV("vApplyConn", "button apply connect")
				else
					_TRV("vApplyConn", "button apply");

					selectType(form);
			}

			function selectType(form) {
				var vpn_server_col	= document.getElementById("vpn_server_col");
				var pppoe_on		= form.vpn_type.value == '0';
				var pptp_on			= form.vpn_type.value == '1';
				var l2tp_on			= form.vpn_type.value == '2';
				var kabinet_on		= form.vpn_type.value == '3';

				displayElement('vpn_password_row', form.vpn_enabled.checked);
				displayElement([ 'vpn_server_row', 'vpn_auth_type_row', 'vpn_user_row', 'vpn_mtu_row', 'vpn_dgw_row', 'vpn_mppe_row', table_vpn_params], (!kabinet_on) && form.vpn_enabled.checked);
				displayElement([ 'vpn_pppoe_iface_row', 'vpn_pure_pppoe_cell', 'vpn_pppoe_service_row', 'vpn_pppoe_row' ], pppoe_on && form.vpn_enabled.checked);
				displayElement('vpn_test_reachable', (pptp_on || l2tp_on) && form.vpn_enabled.checked);
				displayElement('vpn_lanauth_lvl_row', kabinet_on && form.vpn_enabled.checked);

				var vpn_server = _("vpn server col");
				if (form.vpn_type.value == '0') // PPPoE
					vpn_server = _("vpn server col ac");
				else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
					vpn_server = _("vpn server col host");

				if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2') || (form.vpn_type.value == '3'))
					form.vpn_server.value = vpnServerIP;
				else
					form.vpn_server.value = vpnACName;

				vpn_server_col.innerHTML = '<b>' + vpn_server + '</b>';
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
				var text = '<div class="hint"><font color="#0000ff"><b>HINT:</b></font>&nbsp;';
				var show = true;
				switch(key) {
					case 'vpn_enabled':			text += _("hint vpn enabled");			break;
					case 'vpn_vpn_status':		text += _("hint vpn status");			break;
					case 'vpn_user':			text += _("hint vpn user");				break;
					case 'vpn_password':		text += _("hint vpn password");			break;
					case 'vpn_auth_type':		text += _("hint vpn auth type");		break;
					case 'vpn_pppoe_iface':		text += _("hint vpn pppoe iface");		break;
					case 'vpn_pppoe_service':	text += _("hint vpn pppoe service");	break;
					case 'vpn_server':			if (form.vpn_type.value == '0') // PPPoE
							    						text += _("hint vpn pppoe");
												else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
													text += _("hint vpn pptp");			break;
					case 'vpn_mtu':				text += _("hint vpn mtu");				break;
					case 'vpn_mppe':			text += _("hint vpn mppe");				break;
					case 'vpn_dgw':				text += _("hint vpn dgw");
												text += '<p class="val">';
												if (form.vpn_dgw.value == '0')
													text += _("hint vpn dgw disabled");
												else if (form.vpn_dgw.value == '1')
													text += _("hint vpn dgw enabled");
												else if (form.vpn_dgw.value == '2')
													text += _("hint vpn dgw multiple");
												text += '</p>';							break;
					case 'vpn_peerdns':			text += _("hint vpn peerdns");			break;
					case 'vpn_test_reachable':	text += _("hint vpn test reachable");	break;
					case 'vpn_debug':			text += _("hint vpn debug");			break;
					case 'vpn_nat':				text += _("hint vpn nat");				break;
					case 'vpn_lcp':				text += _("hint vpn lcp");				break;
					case 'vpn_pure_pppoe':		text += _("hint vpn pure pppoe");		break;
					case 'vpn_lcp_errors':		text += _("hint vpn lcp errors");		break;
					case 'vpn_lcp_interval':	text += _("hint vpn lcp interval");		break;
					case 'vpn_type':			text += _("hint vpn type");
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
												text += '</p>';							break;
					case 'vpn_lanauth_access':	text += _("hint vpn lanauth");			break;
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
					<p id="vIntroduction"> This page is used to configure the <acronym title="Virtual Private Network">VPN</acronym> tunnel on your Router. </p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form action="/goform/formVPNSetup" method="POST" name="formVPNSetup">
					<table class="form">
						<tr>
							<td class="title" colspan="2" id="vConfig">VPN configuration</td>
						</tr>
						<tr>
							<td class="head" style="width: 50%;" onMouseOver="showHint('vpn_enabled')" onMouseOut="hideHint()" ><input name="vpn_enabled" onClick="vpnSwitchClick(this.form)" type="checkbox">
								<b id="vEnabled">Enable <acronym title="Virtual Private Network">VPN</acronym></b></td>
							<td onMouseOver="showHint('vpn_vpn_status')" onMouseOut="hideHint()" id="vpn_status_col">&nbsp;</td>
						</tr>
						<tr onMouseOver="showHint('vpn_type')" onMouseOut="hideHint()" id="vpn_type">
							<td class="head"><b id="vMode"><acronym title="Virtual Private Network">VPN</acronym> Mode:</b></td>
							<td><select disabled="disabled" name="vpn_type" onChange="selectType(this.form);" class="mid" >
								<option value="0" selected="selected" id="vPPPoE">PPPoE client</option>
								<option value="1" id="vPPTP">PPTP  client</option>
								<option value="2" id="vL2TP">L2TP  client</option>
							</select></td>
						</tr>
						<tr id="vpn_pppoe_row" style="display: none;" onMouseOver="showHint('vpn_pppoe_iface')" onMouseOut="hideHint()">
							<td class="head"><b id="vPPPoEiface">PPPoE interface:</b></td>
							<td><select disabled="disabled" name="vpn_pppoe_iface" class="mid" >
								<option value="LAN" id="vpnIfLAN">LAN</option>
								<option value="WAN" id="vpnIfWAN">WAN</option>
							</select></td>
						</tr>
						<tr id="vpn_server_row" onMouseOver="showHint('vpn_server')" onMouseOut="hideHint()">
							<td class="head" id="vpn_server_col"><b id="vServerCol">Host, <acronym title="Internet Protocol">IP</acronym>, <acronym title="Access Concentrator">AC</acronym> or <acronym title="Access Point Name">APN</acronym> name:</b></td>
							<td><input name="vpn_server" class="mid" value="" disabled="disabled" type="text"></td>
						</tr>
						<tr id="vpn_pppoe_service_row" onMouseOver="showHint('vpn_pppoe_service')" onMouseOut="hideHint()">
							<td class="head"><b id="vServiceName">Service name:</b></td>
							<td><input name="vpn_pppoe_service" class="mid" value="" disabled="disabled" type="text"></td>
						</tr>
						<tr id="vpn_auth_type_row" onMouseOver="showHint('vpn_auth_type')" onMouseOut="hideHint()" >
							<td class="head"><b id="vAuthType">Authentication method:</b></td>
							<td><select id="vpn_auth_type_select" disabled="disabled" name="vpn_auth_type" class="mid">
								<option value="0" id="vAuthTypeAuto" selected="selected">AUTO</option>
								<option value="1">PAP</option>
								<option value="2">CHAP</option>
								<option value="3">MSCHAP</option>
							</select></td>
						</tr>
						<tr id="vpn_lanauth_lvl_row" onMouseOver="showHint('vpn_lanauth_access')" onMouseOut="hideHint()">
							<td class="head"><b id="vLanAuthLvl">KABINET access level:</b></td>
							<td><select name="lanauth_access" class="mid" disabled="disabled">
								<option value="0" id="vKabOffline">offline</option>
								<option value="1" id="vKabinet">kabinet</option>
								<option value="2" id="vKabFull">full</option>
							</select></td>
						</tr>
						<tr id="vpn_user_row" onMouseOver="showHint('vpn_user')" onMouseOut="hideHint()" >
							<td class="head"><b id="vUsername">User name:</b></td>
							<td><input name="vpn_user" class="mid" size="25" maxlength="60" value="" disabled="disabled" type="text"></td>
						</tr>
						<tr id="vpn_password_row" onMouseOver="showHint('vpn_password')" onMouseOut="hideHint()" >
							<td class="head"><b id="vPassword">Password:</b></td>
							<td><input name="vpn_pass" class="mid" size="25" maxlength="60" value="" disabled="disabled" type="password"></td>
						</tr>
						<tr id="vpn_mtu_row" onMouseOver="showHint('vpn_mtu')" onMouseOut="hideHint()" >
							<td class="head"><b id="vMTUMRU"><acronym title="Maximum Transfer Unit">MTU</acronym>/<acronym title="Maximum Recieve Unit">MRU:</acronym></b></td>
							<td><input id="vpn_mtu_field" name="vpn_mtu" maxlength="4" disabled="disabled" type="text" class="half" style="display:none; " value="" >
							<select id="vpn_mtu_select" disabled="disabled" name="vpn_mtu_type" onChange="mtuChange(this.form);" class="mid" >
								<option value="AUTO" id="vMTUAuto">AUTO</option>
								<option value="1" selected="selected" id="vCustom">Custom</option>
								<option value="1492">1492</option>
								<option value="1440">1440</option>
								<option value="1400">1400</option>
								<option value="1300">1300</option>
								<option value="1200">1200</option>
								<option value="1100">1100</option>
								<option value="1000">1000</option>
						  </select></td>
						</tr>
						<tr id="vpn_dgw_row" onMouseOver="showHint('vpn_dgw')" onMouseOut="hideHint()" >
							<td class="head"><b id="vGateway">Default gateway:</b></td>
							<td><select disabled="disabled" name="vpn_dgw" class="mid" class="mid" >
								<option value="0" selected="selected" id="vGWdisable">Disabled</option>
								<option value="1" id="vGWenable">Enabled</option>
							</select></td>
						</tr>
						<!-- VPN params -->
						<tr id="table_vpn_params02" onMouseOver="showHint('vpn_lcp_interval')" onMouseOut="hideHint()">
							<td class="head"><b id="vLCPinterval">LCP echo interval:</b></td>
							<td><select id="vpn_lcp_interval_select" disabled="disabled" name="vpn_lcp_interval" class="mid">
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
							</select></td>
						</tr>
						<tr id="table_vpn_params03" onMouseOver="showHint('vpn_lcp_errors')" onMouseOut="hideHint()">
							<td class="head"><b id="vLCPfail">LCP echo failure:</b></td>
							<td><select id="vpn_lcp_errors_select" disabled="disabled" name="vpn_lcp_errors" class="mid">
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
						  </select></td>
						</tr>
					</table>
					<table class="form" id="table_vpn_params04">
						<tr>
							<td colspan="2" class="title" id="vAddOpt">Additional options</td>
						</tr>
						<tr id="vpn_mppe_row">
							<td width="50%" onMouseOver="showHint('vpn_mppe')" onMouseOut="hideHint()" ><input disabled="disabled" name="vpn_mppe" type="checkbox">
								<b id="vAllowMPPE">Allow <acronym title="Microsoft Point-to-Point Encryption">MPPE</acronym></b></td>
							<td width="50%" onMouseOver="showHint('vpn_peerdns')" onMouseOut="hideHint()" ><input disabled="disabled" name="vpn_peerdns" type="checkbox">
								<b id="vPeerDNS">Peer <acronym title="Domain Name Server">DNS</acronym></b></td>
						</tr>
						<tr>
							<td width="50%" onMouseOver="showHint('vpn_debug')" onMouseOut="hideHint()" ><input disabled="disabled" name="vpn_debug" type="checkbox">
								<b id="vAllowDebug">Allow debug</b></td>
							<td width="50%" onMouseOver="showHint('vpn_nat')" onMouseOut="hideHint()" ><input disabled="disabled" name="vpn_nat" type="checkbox">
								<b id="vEnableNAT">Enable <acronym title="Network Address Translation">NAT</acronym></b></td>
						</tr>
						<tr>
							<td width="50%" onMouseOver="showHint('vpn_lcp')" onMouseOut="hideHint()" ><input disabled="disabled" name="vpn_lcp" type="checkbox">
								<b id="vAdaptiveLCP">Adaptive LCP</b></td>
							<td width="50%" onMouseOver="showHint('vpn_pure_pppoe')" onMouseOut="hideHint()" id="vpn_pure_pppoe_cell"><input disabled="disabled" name="vpn_pure_pppoe" type="checkbox">
								<b id="vPurePPPoE">Pure PPPoE</b></td>
							<td width="50%" id="vpn_test_reachable" onMouseOver="showHint('vpn_test_reachable')" onMouseOut="hideHint()" ><input disabled="disabled" name="vpn_test_reachable" type="checkbox">
								<b id="vTestServer">Test server reachable</b></td>
						</tr>
					</table>
					<table class="buttons">
						<tr>
							<td><input name="lanauth_pass_changed" type="hidden">
								<input value="0" name="reset" type="hidden">
								<input class="mid"    id="vApplyConn" value="Apply and connect" name="save" type="submit" onClick="return checkValues(this.form);" >&nbsp;&nbsp;
								<input class="normal" id="vCancel" value="Cancel" name="save" type="button" onClick="window.location.reload();" >&nbsp;&nbsp;
								<input class="normal" id="vReset" value="Reset" name="reset_button" onClick="resetValues(this.form);" type="button"></td>
						</tr>
					</table>
				</form>
				<br>
				<div id="vpn_hint_row">&nbsp;</div>
				<div class="whitespace">&nbsp;</div>
			</tr>
		</table>
	</body>
</html>
