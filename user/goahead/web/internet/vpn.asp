<!DOCTYPE html>
<html>
<head>
<title>VPN Tunnel setup</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/lang/b28n.js"></script>
<script language="javascript">

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("hint");
Butterlate.setTextDomain("buttons");

var vpnType     = '<% getCfgGeneral(1, "vpnType"); %>';
var vpnServerIP = (vpnType != '0') ? '<% getCfgGeneral(1, "vpnServer"); %>' : '';
var vpnACName   = (vpnType == '0') ? '<% getCfgGeneral(1, "vpnServer"); %>' : '';

var table_vpn_params= [ 'table_vpn_params01', 'table_vpn_params02', 'table_vpn_params03', 'table_vpn_params04' ];

function hideHint(ctl)
{
	var row = document.getElementById('vpn_hint_row');
	row.innerHTML = '';
}

function showHint(key)
{
	var row = document.getElementById('vpn_hint_row');
	var form = document.formVPNSetup;
	var text = '<div class="hint"><font color="#0000ff"><b>HINT:</b></font>&nbsp;';
	var show = true;

	if (key=='vpn_enabled')
		text += _("hint vpn enabled");
	else if (key=='vpn_vpn_status')
		text += _("hint vpn status");
	else if (form.vpn_enabled.checked)
	{
		if (key=='vpn_user')
			text += _("hint vpn user");
		else if (key=='vpn_password')
			text += _("hint vpn password");
		else if (key=='vpn_auth_type')
			text += _("hint vpn auth type");
		else if (key=='vpn_pppoe_iface')
			text += _("hint vpn pppoe iface");
		else if (key=='vpn_pppoe_service')
			text += _("hint vpn pppoe service");
		else if (key=='vpn_server')
		{
			if (form.vpn_type.value == '0') // PPPoE
				text += _("hint vpn pppoe");
			else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
				text += _("hint vpn pptp");
		}
		else if (key=='vpn_mtu')
			text += _("hint vpn mtu");
		else if (key=='vpn_mppe')
			text += _("hint vpn mppe");
		else if (key=='vpn_dgw')
		{
			text += _("hint vpn dgw");
			text += '<p class="val">';
			if (form.vpn_dgw.value == '0')
				text += _("hint vpn dgw disabled");
			else if (form.vpn_dgw.value == '1')
				text += _("hint vpn dgw enabled");
			else if (form.vpn_dgw.value == '2')
				text += _("hint vpn dgw multiple");
			text += '</p>';
		}
		else if (key=='vpn_peerdns')
			text += _("hint vpn peerdns");
		else if (key=='vpn_test_reachable')
			text += _("hint vpn test reachable");
		else if (key=='vpn_debug')
			text += _("hint vpn debug");
		else if (key=='vpn_nat')
			text += _("hint vpn nat");
		else if (key=='vpn_lcp')
			text += _("hint vpn lcp");
		else if (key=='vpn_pure_pppoe')
			text += _("hint vpn pure pppoe");
		else if (key=='vpn_lcp_errors')
			text += _("hint vpn lcp errors");
		else if (key=='vpn_lcp_interval')
			text += _("hint vpn lcp interval");
		else if (key=='vpn_type')
		{
			text += _("hint vpn type");
			text += '<p class="val">';

			if (form.vpn_type.value == "0")
				text += _("hint vpn type pppoe");
			else if (form.vpn_type.value == "1")
				text += _("hint vpn type pptp");
			else if (form.vpn_type.value == "2")
			{
				text += _("hint vpn type l2tp");
				text += '</p><p class="val">';
				text += _("hint vpn type l2tp client");
			}
			else if (form.vpn_type.value == "3")
				text += _("hint vpn type kabinet");
			text += '</p>';
		}
		else if (key=='vpn_lanauth_access')
			text += _("hint vpn lanauth");
		else
			show = false;
	}
	else
		show = false;

	if (show)
	{
		text += '</div>';
		row.innerHTML = text;
	}
}

function vpnSwitchClick(form)
{
	enableElements( [
		form.vpn_server, form.vpn_auth_type,
		form.vpn_user, form.vpn_pass, form.vpn_mtu, form.vpn_mppe,
		form.vpn_peerdns, form.vpn_debug, form.vpn_nat, form.vpn_dgw,
		form.vpn_mtu_type, form.vpn_pppoe_iface, form.vpn_type,
		form.vpn_lcp, form.lanauth_access, form.vpn_pure_pppoe,
		form.vpn_pppoe_service, form.vpn_test_reachable,
		form.vpn_lcp_errors, form.vpn_lcp_interval
		], form.vpn_enabled.checked );
	displayElement( [ 'vpn_type' ], form.vpn_enabled.checked );
	if (form.vpn_enabled.checked)
	{
		_TRV("vApplyConn", "button apply connect");
	} else {
		_TRV("vApplyConn", "button apply");
	}
	selectType(form);
}

function mtuChange(form)
{
	var vpn_mtu_select = document.getElementById("vpn_mtu_select");
	var vpn_mtu_field  = document.getElementById("vpn_mtu_field");

	if (form.vpn_mtu_type.value == '1')
	{
		vpn_mtu_field.style.display = '';
		vpn_mtu_select.setAttribute("class", "half");
		vpn_mtu_field.setAttribute("class", "half");
	}
	else
	{
		vpn_mtu_select.setAttribute("class", "mid");
		vpn_mtu_field.style.display = 'none';
		form.vpn_mtu.value = form.vpn_mtu_type.value;
	}
}

function bodyOnLoad(form)
{
	initializeForm(form);
	initTranslation();

	/* Check if option was set */
	var vpn_mtu_select = document.getElementById('vpn_mtu_select');
	for (var i=0; i < vpn_mtu_select.options.length; i++)
		if (form.vpn_mtu_type.options[i].value == form.vpn_mtu.value)
		{
			form.vpn_mtu_type.value = form.vpn_mtu_select.options[i].value;
			break;
		}

	vpnSwitchClick(form);
	selectType(form);
	mtuChange(form);
	showVPNStatus();
}

function selectType(form)
{
	var vpn_server_col = document.getElementById("vpn_server_col");

	var pppoe_on = form.vpn_type.value == '0';
	var pptp_on = form.vpn_type.value == '1';
	var l2tp_on = form.vpn_type.value == '2';
	var kabinet_on = form.vpn_type.value == '3';

	// Display mode-dependent elements
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

function resetClick(form)
{
	form.reset();
	bodyOnLoad(form);
	return true;
}

function submitClick(form)
{
	if (form.vpn_pass.value.match(/[\s\$]/))
	{
		alert(_("vpn alert password"));
		form.vpn_pass.focus();
		return false;
	}

	if (form.vpn_type.value != "0" && form.vpn_type.value != "3" )
	{
		if (form.vpn_user.value.match(/[\s\$]/))
		{
			alert(_("vpn alert username"));
			form.vpn_user.focus();
			return false;
		}

		if ((!validateIP(form.vpn_server, false)) && (!validateDNS(form.vpn_server, false)))
		{
			alert(_("vpn alert invalid ip"));
			form.vpn_type.focus();
			return false;
		}
	}

	return true;
}

function initializeForm(form)
{
	var vpnEnabled = '<% getCfgGeneral(1, "vpnEnabled"); %>';
	var vpnType   = '<% getCfgGeneral(1, "vpnType"); %>';
	var mppe       = '<% getCfgGeneral(1, "vpnMPPE"); %>';
	var peerdns    = '<% getCfgGeneral(1, "vpnPeerDNS"); %>';
	var debug      = '<% getCfgGeneral(1, "vpnDebug"); %>';
	var nat        = '<% getCfgGeneral(1, "vpnNAT"); %>';
	var dgw        = '<% getCfgGeneral(1, "vpnDGW"); %>';
	var vpn_auth   = '<% getCfgGeneral(1, "vpnAuthProtocol"); %>';
	var lcp        = '<% getCfgGeneral(1, "vpnEnableLCP"); %>';
	var pure_pppoe = '<% getCfgGeneral(1, "vpnPurePPPOE"); %>';
	var lcp_errors = '<% getCfgGeneral(1, "vpnLCPFailure"); %>';
	var lcp_int    = '<% getCfgGeneral(1, "vpnLCPInterval"); %>';
	var vpn_test   = '<% getCfgGeneral(1, "vpnTestReachable"); %>';

	var kabinet_built = '<% getLANAUTHBuilt(); %>';

	if (kabinet_built != '0')
	{
		var lanauth_access   = '<% getCfgGeneral(1, "LANAUTH_LVL"); %>';

		// Add specific option
		if (form.vpn_type.options.length == 3)
			form.vpn_type.options[form.vpn_type.options.length] = new Option(_("vpn kabinet auth"), '3');
		form.lanauth_access.value      = lanauth_access;
	}

	form.vpn_enabled.checked = (vpnEnabled == 'on');
	form.vpn_mppe.checked    = (mppe == 'on');
	form.vpn_peerdns.checked = (peerdns == 'on');
	form.vpn_debug.checked   = (debug == 'on');
	form.vpn_nat.checked     = (nat == 'on');
	form.vpn_type.value      = vpnType;
	form.vpn_dgw.value       = dgw;
	form.vpn_lcp.checked     = (lcp == 'on');
	form.vpn_pure_pppoe.checked = (pure_pppoe == '1');
	form.vpn_test_reachable.checked = (vpn_test == '1');
	form.vpn_auth_type.value = vpn_auth;
	form.vpn_lcp_errors.value= lcp_errors;
	form.vpn_lcp_interval.value = lcp_int;

	selectType(form);
}

function showVPNStatus()
{
	var reset_timer = function() { self.setTimeout('showVPNStatus();', 5000); };

	ajaxLoadElement(
		'vpn_status_col',
		'/internet/vpn_status.asp',
		reset_timer
	);
}

function formCheck(form)
{
	if ((form.vpn_type.value != '0') || (!form.vpn_enabled.checked))
		form.vpn_pure_pppoe.checked = false;
}

function initTranslation()
{
	_TR("vTitle", "vpn title");
	_TR("vIntroduction", "vpn introduction");
	_TR("vConfig", "vpn config");
	_TR("vEnabled", "vpn enable");
	_TR("vMode", "vpn mode");
	_TR("vPPPoE", "vpn pppoe");
	_TR("vPPTP", "vpn pptp");
	_TR("vL2TP", "vpn l2tp");
	_TR("vPPPoEiface", "vpn pppoe iface");
	_TR("vServerCol", "vpn server col");
	_TR("vServiceName", "vpn service name");
	_TR("vAuthType", "vpn auth type");
	_TR("vLanAuthLvl", "vpn lanauth lvl");
	_TR("vKabOffline", "vpn kabinet offline");
	_TR("vKabinet", "vpn kabinet");
	_TR("vKabFull", "vpn kabinet full");
	_TR("vUsername", "vpn username");
	_TR("vPassword", "vpn password");
	_TR("vMTUMRU", "vpn mtu mru");
	_TR("vAuthTypeAuto", "inet auto");
	_TR("vMTUAuto", "inet auto");
	_TR("vCustom", "routing custom");
	_TR("vGateway", "inet gateway");
	_TR("vGWdisable", "button disable");
	_TR("vGWenable", "button enable");
	_TR("vLCPinterval", "vpn lcp interval");
	_TR("vLCPfail", "vpn lcp failure");
	_TR("vAddOpt", "wan additional options");
	_TR("vAllowMPPE", "vpn allow mppe");
	_TR("vPeerDNS", "vpn peer dns");
	_TR("vAllowDebug", "vpn allow debug");
	_TR("vEnableNAT", "vpn enable nat");
	_TR("vAdaptiveLCP", "vpn adaptive lcp");
	_TR("vPurePPPoE", "vpn pure pppoe");
	_TR("vTestServer", "vpn test server");
	_TRV("vApplyConn", "button apply connect");
	_TRV("vReset", "button reset");
}

</script>
</head>

<body onLoad="bodyOnLoad(document.formVPNSetup);">
<table class="body">
  <tr>
    <td><h1 id="vTitle">Virtual Private Network setup</h1>
      <p id="vIntroduction"> This page is used to configure the <acronym title="Virtual Private Network">VPN</acronym> tunnel on your Router. </p>
      <hr>
      <form action="/goform/formVPNSetup" method="POST" name="formVPNSetup" onSubmit="return formCheck(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="vConfig">VPN configuration</td>
          </tr>
          <tr>
            <td class="head" onMouseOver="showHint('vpn_enabled')" onMouseOut="hideHint('vpn_enabled')" ><input name="vpn_enabled" onClick="vpnSwitchClick(this.form)" type="checkbox">
              <b id="vEnabled">Enable <acronym title="Virtual Private Network">VPN</acronym></b></td>
            <td onMouseOver="showHint('vpn_vpn_status')" onMouseOut="hideHint('vpn_vpn_status')" id="vpn_status_col">&nbsp;</td>
          </tr>
          <tr onMouseOver="showHint('vpn_type')" onMouseOut="hideHint('vpn_type')" id="vpn_type">
            <td class="head"><b id="vMode"><acronym title="Virtual Private Network">VPN</acronym> Mode:</b></td>
            <td><select disabled="disabled" name="vpn_type" onChange="selectType(this.form);" class="mid" >
                <option value="0" selected="selected" id="vPPPoE">PPPoE client</option>
                <option value="1" id="vPPTP">PPTP  client</option>
                <option value="2" id="vL2TP">L2TP  client</option>
              </select></td>
          </tr>
          <tr id="vpn_pppoe_row" style="display: none;" onMouseOver="showHint('vpn_pppoe_iface')" onMouseOut="hideHint('vpn_pppoe_iface')">
            <td class="head"><b id="vPPPoEiface">PPPoE interface:</b></td>
            <td><select disabled="disabled" name="vpn_pppoe_iface" class="mid" >
                <% vpnIfaceList(); %>
              </select></td>
          </tr>
          <tr id="vpn_server_row" onMouseOver="showHint('vpn_server')" onMouseOut="hideHint('vpn_server')">
            <td class="head" id="vpn_server_col"><b id="vServerCol">Host, <acronym title="Internet Protocol">IP</acronym>, <acronym title="Access Concentrator">AC</acronym> or <acronym title="Access Point Name">APN</acronym> name:</b></td>
            <td><input name="vpn_server" class="mid" value="<% getCfgGeneral(1, "vpnServer"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr id="vpn_pppoe_service_row" onMouseOver="showHint('vpn_pppoe_service')" onMouseOut="hideHint('vpn_pppoe_service')">
            <td class="head"><b id="vServiceName">Service name:</b></td>
            <td><input name="vpn_pppoe_service" class="mid" value="<% getCfgGeneral(1, "vpnService"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr id="vpn_auth_type_row" onMouseOver="showHint('vpn_auth_type')" onMouseOut="hideHint('vpn_auth_type')" >
            <td class="head"><b id="vAuthType">Authentication method:</b></td>
            <td><select id="vpn_auth_type_select" disabled="disabled" name="vpn_auth_type" class="mid">
                <option value="0" id="vAuthTypeAuto" selected="selected">AUTO</option>
                <option value="1">PAP</option>
                <option value="2">CHAP</option>
                <option value="3">MSCHAP</option>
              </select></td>
          </tr>
          <tr id="vpn_lanauth_lvl_row" onMouseOver="showHint('vpn_lanauth_access')" onMouseOut="hideHint('vpn_lanauth_access')">
            <td class="head"><b id="vLanAuthLvl">KABINET access level:</b></td>
            <td><select name="lanauth_access" class="mid" disabled="disabled">
                <option value="0" id="vKabOffline">offline</option>
                <option value="1" id="vKabinet">kabinet</option>
                <option value="2" id="vKabFull">full</option>
              </select></td>
          </tr>
          <tr id="vpn_user_row" onMouseOver="showHint('vpn_user')" onMouseOut="hideHint('vpn_user')" >
            <td class="head"><b id="vUsername">User name:</b></td>
            <td><input name="vpn_user" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnUser"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr id="vpn_password_row" onMouseOver="showHint('vpn_password')" onMouseOut="hideHint('vpn_password')" >
            <td class="head"><b id="vPassword">Password:</b></td>
            <td><input name="vpn_pass" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnPassword"); %>" disabled="disabled" type="password"></td>
          </tr>
          <tr id="vpn_mtu_row" onMouseOver="showHint('vpn_mtu')" onMouseOut="hideHint('vpn_mtu')" >
            <td class="head"><b id="vMTUMRU"><acronym title="Maximum Transfer Unit">MTU</acronym>/<acronym title="Maximum Recieve Unit">MRU:</acronym></b></td>
            <td><input id="vpn_mtu_field" name="vpn_mtu" maxlength="4" disabled="disabled" type="text" class="half" style="display:none; " value="<% getCfgGeneral(1, "vpnMTU"); %>" >
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
          <tr id="vpn_dgw_row" onMouseOver="showHint('vpn_dgw')" onMouseOut="hideHint('vpn_dgw')" >
            <td class="head"><b id="vGateway">Default gateway:</b></td>
            <td><select disabled="disabled" name="vpn_dgw" class="mid" class="mid" >
              <option value="0" selected="selected" id="vGWdisable">Disabled</option>
              <option value="1" id="vGWenable">Enabled</option>
              </select></td>
          </tr>

          <!-- VPN params -->
          <tr id="table_vpn_params02" onMouseOver="showHint('vpn_lcp_interval')" onMouseOut="hideHint('vpn_lcp_interval')">
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
          <tr id="table_vpn_params03" onMouseOver="showHint('vpn_lcp_errors')" onMouseOut="hideHint('vpn_lcp_errors')">
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
            <td width="50%" onMouseOver="showHint('vpn_mppe')" onMouseOut="hideHint('vpn_mppe')" ><input disabled="disabled" name="vpn_mppe" type="checkbox">
              <b id="vAllowMPPE">Allow <acronym title="Microsoft Point-to-Point Encryption">MPPE</acronym></b></td>
            <td width="50%" onMouseOver="showHint('vpn_peerdns')" onMouseOut="hideHint('vpn_peerdns')" ><input disabled="disabled" name="vpn_peerdns" type="checkbox">
              <b id="vPeerDNS">Peer <acronym title="Domain Name Server">DNS</acronym></b></td>
          </tr>
          <tr>
            <td width="50%" onMouseOver="showHint('vpn_debug')" onMouseOut="hideHint('vpn_debug')" ><input disabled="disabled" name="vpn_debug" type="checkbox">
              <b id="vAllowDebug">Allow debug</b></td>
            <td width="50%" onMouseOver="showHint('vpn_nat')" onMouseOut="hideHint('vpn_nat')" ><input disabled="disabled" name="vpn_nat" type="checkbox">
              <b id="vEnableNAT">Enable <acronym title="Network Address Translation">NAT</acronym></b></td>
          </tr>
          <tr>
            <td width="50%" onMouseOver="showHint('vpn_lcp')" onMouseOut="hideHint('vpn_lcp')" ><input disabled="disabled" name="vpn_lcp" type="checkbox">
              <b id="vAdaptiveLCP">Adaptive LCP</b></td>
            <td width="50%" onMouseOver="showHint('vpn_pure_pppoe')" onMouseOut="hideHint('vpn_pure_pppoe')" id="vpn_pure_pppoe_cell"><input disabled="disabled" name="vpn_pure_pppoe" type="checkbox">
              <b id="vPurePPPoE">Pure PPPoE</b></td>
            <td width="50%" id="vpn_test_reachable" onMouseOver="showHint('vpn_test_reachable')" onMouseOut="hideHint('vpn_test_reachable')" ><input disabled="disabled" name="vpn_test_reachable" type="checkbox">
              <b id="vTestServer">Test server reachable</b></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input name="lanauth_pass_changed" type="hidden">
              <input value="/internet/vpn.asp" name="submit-url" type="hidden">
              <input class="mid" id="vApplyConn" value="Apply and connect" name="save" type="submit" onClick="return submitClick(this.form);" >
              &nbsp;&nbsp;
              <input class="normal" id="vReset" value="Reset" name="reset_button" onClick="resetClick(this.form);" type="button"></td>
          </tr>
        </table>
      </form>
      <div id="vpn_hint_row">&nbsp;</div>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
