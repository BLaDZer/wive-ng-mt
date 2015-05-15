<!DOCTYPE html>
<html>
<head>
<title>Internet Services Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("hint");
Butterlate.setTextDomain("services");
Butterlate.setTextDomain("buttons");

var secs;
var timerID = null;
var timerRunning = false;

var userList=
[
	<% getL2TPUserList(); %>
];

function hideHint(ctl)
{
	var row = document.getElementById('l2tp_hint_row');
	row.innerHTML = '';
}

function showHint(key)
{
	var row = document.getElementById('l2tp_hint_row');
	var form = document.l2tpConfig;
	var text = '<div class="hint"><font color="#0000ff"><b>HINT:</b></font>&nbsp;';
	var show = true;
	
	if (key=='l2tp_enable')
		text += _("hint l2tp server");
	else if (key=='l2tp_ip')
		text += _("hint l2tp ip");
	else if (key=='l2tp_ip_list')
		text += _("hint vpn range");
	else if (key=='l2tp_mtu')
		text += _("hint vpn mtu");
	else if (key=='l2tp_mru')
		text += _("hint vpn mtu");
	else if (key=='l2tp_nat')
		text += _("hint vpn nat");
	else if (key=='l2tp_mppe')
		text += _("hint vpn mppe");
	else if (key=='l2tp_debug')
		text += _("hint vpn debug");
	else if (key=='l2tp_lcp')
		text += _("hint vpn lcp");
        else if (key=='l2tp_proxyarp')
                text += _("hint l2tp proxyarp");
	else
		show = false;
	
	if (show)
	{
		text += '</div>';
		row.innerHTML = text;
	}
}

function genTable(form)
{
	var dis = (form.l2tp_srv_enabled.options.selectedIndex != 0) ? '' : ' disabled="disabled"';

	var table = '<table class="form" style="width: 100%">';
	table += '<tr><td class="title" colspan="3">' + _("services l2tp users") + '</td></tr>';
	table += '<tr><th>' + _("services l2tp login") + '</th><th>' + _("services l2tp password") + '</th><th>' + _("routing action") + '</th></tr>';
	for (var i=0; i<userList.length; i++)
	{
		var row = userList[i];
		table += '<tr><td>' + row[0] + '<input name="l2tp_srv_user' + i + '" type="hidden" value="' + row[0] + '"></td>';
		table += '<td>' + row[1] + '<input name="l2tp_srv_pass' + i + '" type="hidden" value="' + row[1] + '"></td>';
		var js = (form.l2tp_srv_enabled.options.selectedIndex != 0) ? 'javascript:deleteUser(document.l2tpConfig, ' + i + ');' : 'javascript:void()';
		var color = (form.l2tp_srv_enabled.options.selectedIndex != 0) ? 'ff0000' : '808080';
		table += '<td style="text-align: center;"><a style="color: #' + color + ';" title="' + _("services dhcp delete record") + '" href="' + js + '"><img src="/graphics/cross.png" alt="[x]"></a></td></tr>';
	}
	
	if (userList.length < 10)
	{
		table += '<tr><td><input class="mid" value="" name="l2tpLogin"' + dis + '></td>';
		table += '<td><input class="mid" value="" name="l2tpPassword"' + dis + '></td>';
		table += '<td style="text-align: center;"><input type="button" class="normal" title="' + _("services dhcp add record") + '" value="' + _("button add") + '" onclick="addUser(this.form);"' + dis + '></td></tr>';
	}
	table += '</table>';
	
	var elem = document.getElementById("l2tpUserList");
	if (elem!=null)
		elem.innerHTML = table;
	initTranslation();
}

function addUser(form)
{
	if (userList.length < 10)
	{
		userList.push( [ form.l2tpLogin.value, form.l2tpPassword.value ] );
		genTable(form);
	}
}

function deleteUser(form, index)
{
	if ((index>=0) && (index < userList.length))
	{
		var row = userList[index];
		userList.splice(index, 1);
		genTable(form);
	}
}

function l2tpEnableSwitch(form)
{
	enableElements(
		[
			form.l2tp_srv_lcp_adapt, form.l2tp_srv_debug,
			form.l2tp_srv_mtu_sel, form.l2tp_srv_mtu_size,
			form.l2tp_srv_mru_sel, form.l2tp_srv_mru_size,
			form.l2tp_srv_ip_local, form.l2tp_srv_ip_range,
			form.l2tp_srv_nat_enabled, form.l2tp_srv_proxyarp,
			form.l2tp_srv_mppe_enabled
		],
		(form.l2tp_srv_enabled.options.selectedIndex != 0));
	genTable(form);
	displayElement(
		[
			"l2tp_ip", "l2tp_ip_list", "l2tp_mtu",
			"l2tp_mru", "l2tp_additional", "l2tpUserList"
		],
		(form.l2tp_srv_enabled.options.selectedIndex != 0));
}

function initValue()
{
	var form = document.l2tpConfig;
	var l2tp_on = '<% getCfgZero(1, "l2tp_srv_enabled"); %>';
	var adaptive_lcp = '<% getCfgZero(1, "l2tp_srv_lcp_adapt"); %>';
	var debug_on = '<% getCfgZero(1, "l2tp_srv_debug"); %>';
	var nat_on = '<% getCfgZero(1, "l2tp_srv_nat_enabled"); %>';
	var mppe_on = '<% getCfgZero(1, "l2tp_srv_mppe_enabled"); %>';
	var proxyarp_on = '<% getCfgZero(1, "l2tp_srv_proxyarp"); %>';
	var mtu_size = '';

	form.l2tp_srv_enabled.options.selectedIndex = 1*l2tp_on;
	form.l2tp_srv_lcp_adapt.checked = adaptive_lcp == '1';
	form.l2tp_srv_debug.checked = debug_on == '1';
	form.l2tp_srv_nat_enabled.checked = nat_on == '1';
	form.l2tp_srv_mppe_enabled.checked = mppe_on == '1';
	form.l2tp_srv_proxyarp.checked = proxyarp_on == '1';
	
	// Set-up MTU & MRU
	for (var i=0; i < form.l2tp_srv_mtu_sel.options.length; i++)
		if (form.l2tp_srv_mtu_sel.options[i].value == form.l2tp_srv_mtu_size.value)
		{
			form.l2tp_srv_mtu_sel.value = form.l2tp_srv_mtu_sel.options[i].value;
			break;
		}
	
	for (var i=0; i < form.l2tp_srv_mru_sel.options.length; i++)
		if (form.l2tp_srv_mru_sel.options[i].value == form.l2tp_srv_mru_size.value)
		{
			form.l2tp_srv_mru_sel.value = form.l2tp_srv_mru_sel.options[i].value;
			break;
		}

	// Enable/disable
	l2tpEnableSwitch(form);
	mtuChange(form);
	mruChange(form);
	displayServiceStatus();
}

function CheckValue(form)
{
	if (form.l2tp_srv_enabled.options.selectedIndex != 0)
	{
		if (!validateIP(form.l2tp_srv_ip_local, true))
		{
			alert('Please specify correct IP Address!');
			form.l2tp_srv_ip_local.focus();
			return false;
		}
	}
	
	return true;
}

function mtuChange(form)
{
	if (form.l2tp_srv_mtu_sel.value == '1')
	{
		form.l2tp_srv_mtu_size.style.display = '';
		form.l2tp_srv_mtu_sel.setAttribute("class", "half");
		form.l2tp_srv_mtu_size.setAttribute("class", "half");
	}
	else
	{
		form.l2tp_srv_mtu_size.style.display = 'none';
		form.l2tp_srv_mtu_sel.setAttribute("class", "mid");
		form.l2tp_srv_mtu_size.value = form.l2tp_srv_mtu_sel.value;
	}
}

function mruChange(form)
{
	if (form.l2tp_srv_mru_sel.value == '1')
	{
		form.l2tp_srv_mru_size.style.display = '';
		form.l2tp_srv_mru_sel.setAttribute("class", "half");
		form.l2tp_srv_mru_size.setAttribute("class", "half");
	}
	else
	{
		form.l2tp_srv_mru_size.style.display = 'none';
		form.l2tp_srv_mru_sel.setAttribute("class", "mid");
		form.l2tp_srv_mru_size.value = form.l2tp_srv_mru_sel.value;
	}
}

function initTranslation()
{
	_TR("l2tpServerTitle", "services l2tp title");
	_TR("l2tpServerIntroduction", "services l2tp introduction");
	_TR("l2tpServerSetup", "services l2tp config");
	_TR("l2tpEnabled", "services l2tp enable");
	_TR("l2tpDisable", "button disable");
	_TR("l2tpEnable", "button enable");
	_TR("l2tpLocalIP", "services l2tp ip");
	_TR("l2tpListIP", "services l2tp range ip");
	_TR("l2tpMTU", "services l2tp mtu");
	_TR("l2tpMRU", "services l2tp mru");
	_TR("l2tpAdditional", "wan additional options");
	_TR("l2tpLCP", "vpn adaptive lcp");
	_TR("l2tpDebug", "vpn allow debug");
	_TR("l2tpNAT", "vpn enable nat");
	_TR("l2tpProxyARP", "services l2tp proxy arp");
	_TR("l2tpMPPE", "vpn allow mppe");
	
	_TRV("l2tpApply", "button apply");
}

function displayServiceHandler(response)
{
	var form = document.l2tpConfig;

	var services = [
		// turned_on, row_id, daemon_id
		[ '<% getCfgGeneral(1, "l2tp_srv_enabled"); %>', 'xl2tpd', 'xl2tpd-srv' ]
	];

	// Create associative array
	var tmp = response.split(',');
	var daemons = [];
	for (var i=0; i<tmp.length; i++)
		daemons[tmp[i]] = 1;

	// Now display all services
	for (var i=0; i<services.length; i++)
	{
		var service = services[i];
		var row = document.getElementById(service[1]);
		var tds = [];
		for (var j=0; j<row.childNodes.length; j++)
			if (row.childNodes[j].nodeName == 'TD')
				tds.push(row.childNodes[j]);

		if (row != null)
		{
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

function displayServiceStatus()
{
	ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
}

</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="l2tpServerTitle">L2TP Server Setup</h1>
      <p id="l2tpServerIntroduction">Here you can configure L2TP server settings.</p>
      <hr>
      <form method="POST" name="l2tpConfig" action="/goform/l2tpConfig" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="3" id="l2tpServerSetup">L2TP Server Setup</td>
          </tr>
          <tr id="xl2tpd" onMouseOver="showHint('l2tp_enable')" onMouseOut="hideHint('l2tp_enable')">
          	<td class="head" id="l2tpEnabled">L2TP server</td>
          	<td><select name="l2tp_srv_enabled" class="half" onChange="l2tpEnableSwitch(this.form);">
          		<option value="0" id="l2tpDisable">Disable</option>
          		<option value="1" id="l2tpEnable">Enable</option>
          	</select></td>
          	<td style="width: 56px;">&nbsp;</td>
          </tr>
          <tr onMouseOver="showHint('l2tp_ip')" onMouseOut="hideHint('l2tp_ip')" id="l2tp_ip">
            <td class="head" id="l2tpLocalIP">Our local IP to use</td>
            <td colspan="2"><input name="l2tp_srv_ip_local" class="mid" value="<% getCfgGeneral(1, "l2tp_srv_ip_local"); %>"></td>
          </tr>
          <tr onMouseOver="showHint('l2tp_ip_list')" onMouseOut="hideHint('l2tp_ip_list')" id="l2tp_ip_list">
            <td class="head" id="l2tpListIP">Allocate from this IP range</td>
            <td colspan="2"><input name="l2tp_srv_ip_range" class="mid" value="<% getCfgGeneral(1, "l2tp_srv_ip_range"); %>"></td>
          </tr>
          <tr onMouseOver="showHint('l2tp_mtu')" onMouseOut="hideHint('l2tp_mtu')" id="l2tp_mtu">
            <td class="head" id="l2tpMTU">Tunnel <acronym title="Maximum Transfer Unit">MTU</acronym>:</td>
            <td colspan="2"><input name="l2tp_srv_mtu_size" type="text" class="half" style="display:none;" value="<% getCfgGeneral(1, "l2tp_srv_mtu_size"); %>" >
              <select name="l2tp_srv_mtu_sel" onChange="mtuChange(this.form);" class="mid" >
                <option value="AUTO">AUTO</option>
                <option value="1" selected="selected">Custom</option>
                <option value="1460">1460</option>
                <option value="1440">1440</option>
                <option value="1400">1400</option>
                <option value="1300">1300</option>
                <option value="1200">1200</option>
                <option value="1100">1100</option>
                <option value="1000">1000</option>
              </select></td>
          </tr>
          <tr onMouseOver="showHint('l2tp_mru')" onMouseOut="hideHint('l2tp_mru')" id="l2tp_mru">
            <td class="head" id="l2tpMRU">Tunnel <acronym title="Maximum Receive Unit">MRU</acronym>:</td>
            <td colspan="2"><input name="l2tp_srv_mru_size" type="text" class="half" style="display:none;" value="<% getCfgGeneral(1, "l2tp_srv_mru_size"); %>" >
              <select name="l2tp_srv_mru_sel" onChange="mruChange(this.form);" class="mid" >
                <option value="AUTO">AUTO</option>
                <option value="1" selected="selected">Custom</option>
                <option value="1460">1460</option>
                <option value="1440">1440</option>
                <option value="1400">1400</option>
                <option value="1300">1300</option>
                <option value="1200">1200</option>
                <option value="1100">1100</option>
                <option value="1000">1000</option>
              </select></td>
          </tr>
        </table>
        <table class="form" id="l2tp_additional">
          <tr>
            <td class="title" colspan="2" id="l2tpAdditional">Additional options</td>
          </tr>
          <tr>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_lcp')" onMouseOut="hideHint('l2tp_lcp')"><input name="l2tp_srv_lcp_adapt" type="checkbox">
              &nbsp;<b id="l2tpLCP">Adaptive LCP</b></td>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_debug')" onMouseOut="hideHint('l2tp_debug')"><input name="l2tp_srv_debug" type="checkbox">
              &nbsp;<b id="l2tpDebug">L2TP debugging</b></td>
          </tr>
          <tr>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_nat')" onMouseOut="hideHint('l2tp_nat')"><input name="l2tp_srv_nat_enabled" type="checkbox">
              &nbsp;<b id="l2tpNAT">Enable NAT</b></td>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_proxyarp')" onMouseOut="hideHint('l2tp_proxyarp')"><input name="l2tp_srv_proxyarp" type="checkbox">
              &nbsp;<b id="l2tpProxyARP">L2TP proxyarp</b></td>
          </tr>
          <tr>
            <td style="width: 50%;" onmouseover="showHint('l2tp_mppe')" onmouseout="hideHint('l2tp_mppe')"><input name="l2tp_srv_mppe_enabled" type="checkbox">
              &nbsp;<b id="l2tpMPPE">Require MPPE</b></td>
          </tr>
        </table>
        <div id="l2tpUserList"> </div>
        <table class="buttons">
          <tr>
            <td><input type="submit" class="normal" id="l2tpApply" value="Apply">
              <input type="hidden" value="/services/l2tp.asp" name="submit-url"></td>
          </tr>
        </table>
      </form>
      <div id="l2tp_hint_row">&nbsp;</div>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
