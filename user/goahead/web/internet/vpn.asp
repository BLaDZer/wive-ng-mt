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
<script language="javascript">
var pptpType     = '<% getCfgGeneral(1, "vpnType"); %>';
var pptpServerIP = (pptpType != '0') ? '<% getCfgGeneral(1, "vpnServer"); %>' : '';
var pptpACName   = (pptpType == '0') ? '<% getCfgGeneral(1, "vpnServer"); %>' : '';

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
		text += 'Enable Virtual Private Network support.';
	else if (key=='vpn_vpn_status')
		text += 'This indicator shows current state of VPN connection.';
	else if (form.vpn_enabled.checked)
	{
		if (key=='vpn_user')
			text += 'Specify user name given by your network provider.';
		else if (key=='vpn_password')
			text += 'Specify password given by your network provider.';
		else if (key=='vpn_auth_type')
			text += 'Select VPN authentication protocol type.';
		else if (key=='vpn_pppoe_iface')
			text += 'Select available interface for PPPoE.';
		else if (key=='vpn_pppoe_service')
			text += 'Some internet providers need to set up special PPPoE service name. You can specify it here.';
		else if (key=='vpn_server')
		{
			if (form.vpn_type.value == '0') // PPPoE
				text += 'Specify Access Concentrator name for PPPoE connection. If no Access Concentrator name is set system will try to connect to first available Access Concentrator.';
			else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
				text += 'Specify host address (IP address or domain name) of VPN server.';
			else if (form.vpn_type.value == '3') // L2TP server
				text += 'Use the following IP as VPN server own IP address in your VPN network.';
			else if ((form.vpn_type.value == '4') || (form.vpn_type.value == '5')) // GSM/CDMA
				text += 'Specify Access Point Name given by network provider. If no Access Point Name is specified it will be taken from modem default settings.';
		}
		else if (key=='vpn_range')
			text += 'Specify range of IP addresses given to clients by VPN server in <b>L2TP server</b> mode.';
		else if (key=='vpn_mtu')
			text += 'Specify Maximum Transfer Unit/Maximum Recieve Unit size in octets.';
		else if (key=='vpn_mppe')
			text += 'Enable automatic Microsoft Point-to-Point Encryption (MPPE) mode. Need set authentication method to mschap.';
		else if (key=='vpn_dgw')
		{
			text += 'Manage default gateway replacing in routing table.</p><p class="val">';
			if (form.vpn_dgw.value == '0')
				text += '<b>Disabled</b> means that no default gateway will be written to routing table.';
			else if (form.vpn_dgw.value == '1')
				text += '<b>Enabled</b> means that default gateway will be replaced by gateway given by network provider.';
			else if (form.vpn_dgw.value == '2')
				text += '<b>Multiple</b> means that default gateway will be added to existing gateway but with metric 10.';
			text += '</p>';
		}
		else if (key=='vpn_peerdns')
			text += 'Allow to get DNS adress from VPN server and write to /etv/ppp/resolv.conf.';
		else if (key=='vpn_test_reachable')
			text += 'Check that VPN server is reachable before initiation of VPN connection.';
		else if (key=='vpn_debug')
			text += 'Allow debug mode for VPN connections.';
		else if (key=='vpn_nat')
			text += 'Add Network Address Translation to new VPN connection.';
		else if (key=='vpn_lcp')
			text += 'Enable adaptive LCP echo interval.';
		else if (key=='vpn_pure_pppoe')
			text += 'Enable PURE mode if you ISP use PPPoE without DHCP (Rostelecom/R-Telecom and others ISP).';
		else if (key=='vpn_lcp_errors')
			text += 'If this option is given, pppd will presume the peer to be dead if n LCP ' +
				'echo-requests are sent without receiving a valid LCP echo-reply. If this happens, ' +
				'pppd will terminate the connection. Use of this option requires a non-zero value ' +
				'for the <b>LCP echo interval</b> parameter. This option can be used to enable ' +
				'pppd to terminate after the physical connection has been broken (e.g., the ' +
				'modem has hung up) in situations where no hardware modem control lines are available.';
		else if (key=='vpn_lcp_interval')
			text += 'If this option is given, pppd will send an LCP echo-request frame to the peer every ' +
				'specified value of seconds. Normally the peer should respond to the echo-request by ' +
				'sending an echo-reply. This option can be used with the <b>LCP echo failure</b> option ' +
				'to detect that the peer is no longer connected.';
		else if (key=='vpn_type')
		{
			text += 'Specify VPN access mode.<p class="val">';

			if (form.vpn_type.value == "0")
				text += '<b>PPPoE</b> (see RFC #2516) means encapsulating Point-to-Point Protocol (PPP) frames ' +
					'inside Ethernet frames. It is used mainly with DSL services where individual ' +
					'users connect to the DSL modem over Ethernet and in plain Metro Ethernet networks.';
			else if (form.vpn_type.value == "1")
				text += '<b>PPTP</b> (see RFC #2637) means a method for implementing virtual private networks. ' +
					'PPTP uses a control channel over TCP and a GRE tunnel operating to encapsulate ' +
					'PPP packets.';
			else if ((form.vpn_type.value == "2") || (form.vpn_type.value == "3"))
			{
				text += '<b>L2TP</b> (see RFC #2661) means a tunneling protocol used to support virtual private networks. ' +
					'It does not provide any encryption or confidentiality by itself; it relies on an ' +
					'encryption protocol that it passes within the tunnel to provide privacy.</p><p class="val">';
				if (form.vpn_type.value == "2")
					text += '<b>L2TP client</b> means a connection to remote L2TP server.';
				else
				text += '<b>L2TP server</b> means a connection from remote machines to L2TP server on this router.';
			}
			else if (form.vpn_type.value == "4")
				text += '<b>Modem GPRS</b> means connection to Internet via GPRS/EDGE in GSM mobile networks.';
			else if (form.vpn_type.value == "5")
				text += '<b>Modem CDMA</b> means connection to Internet via UMTS in CDMA mobile networks.';
			else if (form.vpn_type.value == "6")
				text += '<b>Kabinet Authorization</b> means connection to KABINET provider network.';
			text += '</p>';
		}
		else if (key=='vpn_lanauth_access')
			text += 'Specify access mode to KABINET provider network.';
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
		form.vpn_server, form.vpn_range, form.vpn_auth_type,
		form.vpn_user, form.vpn_pass, form.vpn_mtu, form.vpn_mppe,
		form.vpn_peerdns, form.vpn_debug, form.vpn_nat, form.vpn_dgw,
		form.vpn_mtu_type, form.vpn_pppoe_iface, form.vpn_type,
		form.vpn_lcp, form.lanauth_access, form.vpn_pure_pppoe,
		form.vpn_pppoe_service, form.vpn_test_reachable,
		form.vpn_lcp_errors, form.vpn_lcp_interval
		], form.vpn_enabled.checked );
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
	var l2tp_server_on = form.vpn_type.value == '3';
	var kabinet_on = form.vpn_type.value == '6';

	// Display mode-dependent elements
	displayElement([ 'vpn_pure_pppoe_cell', 'vpn_pppoe_service_row', 'vpn_pppoe_row' ], pppoe_on);
	displayElement([ 'vpn_pppoe_iface_row', 'vpn_server_row', 'vpn_auth_type_row', 'vpn_user_row', 'vpn_mtu_row', 'vpn_dgw_row', table_vpn_params], !kabinet_on);
	displayElement('vpn_test_reachable', pptp_on || l2tp_on);
	displayElement('vpn_lanauth_lvl_row', kabinet_on);
	displayElement('vpn_mppe_row', !l2tp_server_on);
	displayElement('vpn_l2tp_range', l2tp_server_on);

	var vpn_server = 'Host, <acronym title="Internet Protocol">IP</acronym>, <acronym title="Access Concentrator">AC</acronym> or <acronym title="Access Point Name">APN</acronym> name';
	if (form.vpn_type.value == '0') // PPPoE
		vpn_server = '<acronym title="Access Concentrator">AC</acronym> name';
	else if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2')) // PPTP client, L2TP client
		vpn_server = 'Host, <acronym title="Internet Protocol">IP</acronym> or <acronym title="Domain Name System">DNS</acronym> name';
	else if (form.vpn_type.value == '3') // L2TP server
		vpn_server = 'VPN Local <acronym title="Internet Protocol">IP</acronym>';
	else if ((form.vpn_type.value == '4') || (form.vpn_type.value == '5'))
		vpn_server = '<acronym title="Access Point Name">APN</acronym> name'; // GSM/CDMA

	if ((form.vpn_type.value == '1') || (form.vpn_type.value == '2') || (form.vpn_type.value == '3'))
		form.vpn_server.value = pptpServerIP;
	else
		form.vpn_server.value = pptpACName;

	vpn_server_col.innerHTML = '<b>' + vpn_server + ':</b>';
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
		alert("Password can not contain spaces or dollar ('$') sign!");
		form.vpn_pass.focus();
		return false;
	}

	if ((form.vpn_type.value != "0") && (form.vpn_type.value != "6"))
	{
		if (form.vpn_user.value.match(/[\s\$]/))
		{
			alert("User name can not contain spaces or dollar ('$') sign!");
			form.vpn_user.focus();
			return false;
		}

		if ((!validateIP(form.vpn_server, false)) && (!validateDNS(form.vpn_server, false)))
		{
			alert("Invalid IP address or domain name!");
			form.vpn_type.focus();
			return false;
		}
	}

	return true;
}

function initializeForm(form)
{
	var vpnEnabled = '<% getCfgGeneral(1, "vpnEnabled"); %>';
	var pptpType   = '<% getCfgGeneral(1, "vpnType"); %>';
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
		form.vpn_type.options[form.vpn_type.options.length] = new Option('KABINET Authorization', '6');
		form.lanauth_access.value      = lanauth_access;
	}

	form.vpn_enabled.checked = (vpnEnabled == 'on');
	form.vpn_mppe.checked    = (mppe == 'on');
	form.vpn_peerdns.checked = (peerdns == 'on');
	form.vpn_debug.checked   = (debug == 'on');
	form.vpn_nat.checked     = (nat == 'on');
	form.vpn_type.value      = pptpType;
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

</script>
</head>

<body onLoad="bodyOnLoad(document.formVPNSetup);">
<table class="body">
  <tr>
    <td><h1>Virtual Private Network setup</h1>
      <p> This page is used to configure the <acronym title="Virtual Private Network">VPN</acronym> tunnel on your Router. </p>
      <hr>
      <form action="/goform/formVPNSetup" method="POST" name="formVPNSetup" onSubmit="return formCheck(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="2">VPN configuration</td>
          </tr>
          <tr>
            <td class="head" onMouseOver="showHint('vpn_enabled')" onMouseOut="hideHint('vpn_enabled')" ><input name="vpn_enabled" onClick="vpnSwitchClick(this.form)" type="checkbox">
              <b>Enable <acronym title="Virtual Private Network">VPN</acronym></b></td>
            <td onMouseOver="showHint('vpn_vpn_status')" onMouseOut="hideHint('vpn_vpn_status')" id="vpn_status_col">&nbsp;</td>
          </tr>
          <tr onMouseOver="showHint('vpn_type')" onMouseOut="hideHint('vpn_type')" >
            <td class="head"><b><acronym title="Virtual Private Network">VPN</acronym> Mode:</b></td>
            <td><select disabled="disabled" name="vpn_type" onChange="selectType(this.form);" class="mid" >
                <option value="0" selected="selected">PPPoE client</option>
                <option value="1">PPTP  client</option>
                <option value="2">L2TP  client</option>
                <!-- No L2TP support now
				<option value="3">L2TP  server</option> -->
              </select></td>
          </tr>
          <tr id="vpn_pppoe_row" style="display: none;" onMouseOver="showHint('vpn_pppoe_iface')" onMouseOut="hideHint('vpn_pppoe_iface')">
            <td class="head"><b>PPPoE interface:</b></td>
            <td><select disabled="disabled" name="vpn_pppoe_iface" class="mid" >
                <% vpnIfaceList(); %>
              </select></td>
          </tr>
          <tr id="vpn_server_row" onMouseOver="showHint('vpn_server')" onMouseOut="hideHint('vpn_server')">
            <td class="head" id="vpn_server_col"><b>Host, <acronym title="Internet Protocol">IP</acronym>, <acronym title="Access Concentrator">AC</acronym> or <acronym title="Access Point Name">APN</acronym> name:</b></td>
            <td><input name="vpn_server" class="mid" value="<% getCfgGeneral(1, "vpnServer"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr id="vpn_pppoe_service_row" onMouseOver="showHint('vpn_pppoe_service')" onMouseOut="hideHint('vpn_pppoe_service')">
            <td class="head"><b>Service name:</b></td>
            <td><input name="vpn_pppoe_service" class="mid" value="<% getCfgGeneral(1, "vpnService"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr id="vpn_l2tp_range" onMouseOver="showHint('vpn_range')" onMouseOut="hideHint('vpn_range')" style="display: none;" >
            <td class="head"><b><acronym title="Virtual Private Network">VPN</acronym> range <acronym title="Internet Protocol">IP</acronym> adresses:</b></td>
            <td><input name="vpn_range" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnRange"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr id="vpn_auth_type_row" onMouseOver="showHint('vpn_auth_type')" onMouseOut="hideHint('vpn_auth_type')" >
            <td class="head"><b>Authentication method:</b></td>
            <td><select id="vpn_auth_type_select" disabled="disabled" name="vpn_auth_type" class="mid">
                <option value="0" selected="selected">AUTO</option>
                <option value="1">PAP</option>
                <option value="2">CHAP</option>
                <option value="3">MSCHAP</option>
              </select></td>
          </tr>
          <tr id="vpn_lanauth_lvl_row" onMouseOver="showHint('vpn_lanauth_access')" onMouseOut="hideHint('vpn_lanauth_access')">
            <td class="head"><b>KABINET access level:</b></td>
            <td><select name="lanauth_access" class="mid" disabled="disabled">
                <option value="0">offline</option>
                <option value="1">kabinet</option>
                <option value="2">full</option>
              </select></td>
          </tr>
          <tr id="vpn_user_row" onMouseOver="showHint('vpn_user')" onMouseOut="hideHint('vpn_user')" >
            <td class="head"><b>User name:</b></td>
            <td><input name="vpn_user" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnUser"); %>" disabled="disabled" type="text"></td>
          </tr>
          <tr onMouseOver="showHint('vpn_password')" onMouseOut="hideHint('vpn_password')" >
            <td class="head"><b>Password:</b></td>
            <td><input name="vpn_pass" class="mid" size="25" maxlength="60" value="<% getCfgGeneral(1, "vpnPassword"); %>" disabled="disabled" type="password"></td>
          </tr>
          <tr id="vpn_mtu_row" onMouseOver="showHint('vpn_mtu')" onMouseOut="hideHint('vpn_mtu')" >
            <td class="head"><b><acronym title="Maximum Transfer Unit">MTU</acronym>/<acronym title="Maximum Recieve Unit">MRU:</acronym></b></td>
            <td><input id="vpn_mtu_field" name="vpn_mtu" maxlength="4" disabled="disabled" type="text" class="half" style="display:none; " value="<% getCfgGeneral(1, "vpnMTU"); %>" >
              <select id="vpn_mtu_select" disabled="disabled" name="vpn_mtu_type" onChange="mtuChange(this.form);" class="mid" >
                <option value="AUTO">AUTO</option>
                <option value="1" selected="selected">Custom</option>
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
            <td class="head"><b>Default gateway:</b></td>
            <td><select disabled="disabled" name="vpn_dgw" class="mid" class="mid" >
              <option value="0" selected="selected">Disabled</option>
              <option value="1">Enabled</option>
              </select></td>
          </tr>

          <!-- VPN params -->
          <tr id="table_vpn_params02" onMouseOver="showHint('vpn_lcp_interval')" onMouseOut="hideHint('vpn_lcp_interval')">
            <td class="head"><b>LCP echo interval:</b></td>
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
            <td class="head"><b>LCP echo failure:</b></td>
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
            <td colspan="2" class="title">Additional options</td>
          </tr>
          <tr id="vpn_mppe_row">
            <td width="50%" onMouseOver="showHint('vpn_mppe')" onMouseOut="hideHint('vpn_mppe')" ><input disabled="disabled" name="vpn_mppe" type="checkbox">
              <b>Allow <acronym title="Microsoft Point-to-Point Encryption">MPPE</acronym></b></td>
            <td width="50%" onMouseOver="showHint('vpn_peerdns')" onMouseOut="hideHint('vpn_peerdns')" ><input disabled="disabled" name="vpn_peerdns" type="checkbox">
              <b>Peer <acronym title="Domain Name Server">DNS</acronym></b></td>
          </tr>
          <tr>
            <td width="50%" onMouseOver="showHint('vpn_debug')" onMouseOut="hideHint('vpn_debug')" ><input disabled="disabled" name="vpn_debug" type="checkbox">
              <b>Allow debug</b></td>
            <td width="50%" onMouseOver="showHint('vpn_nat')" onMouseOut="hideHint('vpn_nat')" ><input disabled="disabled" name="vpn_nat" type="checkbox">
              <b>Enable <acronym title="Network Address Translation">NAT</acronym></b></td>
          </tr>
          <tr>
            <td width="50%" onMouseOver="showHint('vpn_lcp')" onMouseOut="hideHint('vpn_lcp')" ><input disabled="disabled" name="vpn_lcp" type="checkbox">
              <b>Adaptive LCP</b></td>
            <td width="50%" onMouseOver="showHint('vpn_pure_pppoe')" onMouseOut="hideHint('vpn_pure_pppoe')" id="vpn_pure_pppoe_cell"><input disabled="disabled" name="vpn_pure_pppoe" type="checkbox">
              <b>Pure PPPoE</b></td>
            <td width="50%" id="vpn_test_reachable" onMouseOver="showHint('vpn_test_reachable')" onMouseOut="hideHint('vpn_test_reachable')" ><input disabled="disabled" name="vpn_test_reachable" type="checkbox">
              <b>Test server reachable</b></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input name="lanauth_pass_changed" type="hidden">
              <input value="/internet/vpn.asp" name="submit-url" type="hidden">
              <input class="normal" value="Apply and connect" name="save" type="submit" onClick="return submitClick(this.form);" >
              &nbsp;&nbsp;
              <input class="normal" value="Reset" name="reset_button" onClick="resetClick(this.form);" type="button"></td>
          </tr>
        </table>
      </form>
      <div id="vpn_hint_row">&nbsp;</div>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
