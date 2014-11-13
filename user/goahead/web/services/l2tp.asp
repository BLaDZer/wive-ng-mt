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

Butterlate.setTextDomain("internet");
Butterlate.setTextDomain("services");

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
		text += 'Enable L2TP server.';
	else if (key=='l2tp_ip')
		text += 'Use the following IP as VPN server own IP address in your VPN network.';
	else if (key=='l2tp_ip_list')
		text += 'Specify range of IP addresses given to clients by VPN server in L2TP server.'
	else if (key=='l2tp_mtu')
		text += 'Specify Maximum Transfer Unit size in octets.';
	else if (key=='l2tp_mru')
		text += 'Specify Maximum Recieve Unit size in octets.';
	else if (key=='l2tp_nat')
		text += 'Allow Network Address Translation to L2TP server.';
	else if (key=='l2tp_mppe')
		text += 'Enable Microsoft Point-to-Point Encryption (MPPE) mode for L2TP server connections.';
	else if (key=='l2tp_debug')
		text += 'Allow debug mode for L2TP server connections.';
	else if (key=='l2tp_lcp')
		text += 'Enable adaptive LCP echo interval.';
        else if (key=='l2tp_proxyarp')
                text += 'Enable proxyarp for VPN server.';
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
	var dis = (form.l2tp_srv_enabled.checked) ? '' : ' disabled="disabled"';

	var table = '<table class="form" style="width: 100%">';
	table += '<tr><td class="title" colspan="3">L2TP users:</td></tr>';
	table += '<tr><th>Login</th><th>Password</th><th>Action</th></tr>';
	for (var i=0; i<userList.length; i++)
	{
		var row = userList[i];
		table += '<tr><td>' + row[0] + '<input name="l2tp_srv_user' + i + '" type="hidden" value="' + row[0] + '"></td>';
		table += '<td>' + row[1] + '<input name="l2tp_srv_pass' + i + '" type="hidden" value="' + row[1] + '"></td>';
		var js = (form.l2tp_srv_enabled.checked) ? 'javascript:deleteUser(document.l2tpConfig, ' + i + ');' : 'javascript:void()';
		var color = (form.l2tp_srv_enabled.checked) ? 'ff0000' : '808080';
		table += '<td style="text-align: center;"><a style="color: #' + color + ';" title="Delete record" href="' + js + '"><b>[x]</b></a></td></tr>';
	}
	
	if (userList.length < 10)
	{
		table += '<tr><td><input class="mid" value="" name="l2tpLogin"' + dis + '></td>';
		table += '<td><input class="mid" value="" name="l2tpPassword"' + dis + '></td>';
		table += '<td style="text-align: center;"><input type="button" class="normal" title="Add record" value="Add" onclick="addUser(this.form);"' + dis + '></td></tr>';
	}
	table += '</table>';
	
	var elem = document.getElementById("l2tpUserList");
	if (elem!=null)
		elem.innerHTML = table;
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
		form.l2tp_srv_enabled.checked);
	genTable(form);
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

	form.l2tp_srv_enabled.checked = l2tp_on == '1';
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
}

function CheckValue(form)
{
	if (form.l2tp_srv_enabled.checked)
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

</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1>L2TP Server Setup</h1>
      <p>Here you can configure L2TP server settings.</p>
      <hr>
      <div id="dhcpClientsTable"> </div>
      <form method="POST" name="l2tpConfig" action="/goform/l2tpConfig" onSubmit="return CheckValue(this);">
        <table class="form">
          <tr>
            <td class="title" colspan="2">L2TP Server Setup</td>
          </tr>
          <tr>
            <td class="head" onMouseOver="showHint('l2tp_enable')" onMouseOut="hideHint('l2tp_enable')"><input type="checkbox" name="l2tp_srv_enabled" onChange="l2tpEnableSwitch(this.form);">
              &nbsp;Enable L2TP server </td>
            <td>&nbsp;</td>
          </tr>
          <tr onMouseOver="showHint('l2tp_ip')" onMouseOut="hideHint('l2tp_ip')">
            <td class="head">Our local IP to use</td>
            <td><input name="l2tp_srv_ip_local" class="mid" value="<% getCfgGeneral(1, "l2tp_srv_ip_local"); %>"></td>
          </tr>
          <tr onMouseOver="showHint('l2tp_ip_list')" onMouseOut="hideHint('l2tp_ip_list')">
            <td class="head">Allocate from this IP range</td>
            <td><input name="l2tp_srv_ip_range" class="mid" value="<% getCfgGeneral(1, "l2tp_srv_ip_range"); %>"></td>
          </tr>
          <tr onMouseOver="showHint('l2tp_mtu')" onMouseOut="hideHint('l2tp_mtu')">
            <td class="head"><acronym title="Maximum Transfer Unit">Tunnel MTU</acronym>:</td>
            <td><input name="l2tp_srv_mtu_size" type="text" class="half" style="display:none;" value="<% getCfgGeneral(1, "l2tp_srv_mtu_size"); %>" >
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
          <tr onMouseOver="showHint('l2tp_mru')" onMouseOut="hideHint('l2tp_mru')">
            <td class="head"><acronym title="Maximum Receive Unit">Tunnel MRU</acronym>:</td>
            <td><input name="l2tp_srv_mru_size" type="text" class="half" style="display:none;" value="<% getCfgGeneral(1, "l2tp_srv_mru_size"); %>" >
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
        <table class="form">
          <tr>
            <td class="title" colspan="2">Additional options</td>
          </tr>
          <tr>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_lcp')" onMouseOut="hideHint('l2tp_lcp')"><input name="l2tp_srv_lcp_adapt" type="checkbox">
              &nbsp;<b>Adaptive LCP</b></td>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_debug')" onMouseOut="hideHint('l2tp_debug')"><input name="l2tp_srv_debug" type="checkbox">
              &nbsp;<b>L2TP debugging</b></td>
          </tr>
          <tr>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_nat')" onMouseOut="hideHint('l2tp_nat')"><input name="l2tp_srv_nat_enabled" type="checkbox">
              &nbsp;<b>Enable NAT</b></td>
            <td style="width: 50%;" onMouseOver="showHint('l2tp_proxyarp')" onMouseOut="hideHint('l2tp_proxyarp')"><input name="l2tp_srv_proxyarp" type="checkbox">
              &nbsp;<b>L2TP proxyarp</b></td>
          </tr>
          <tr>
            <td style="width: 50%;" onmouseover="showHint('l2tp_mppe')" onmouseout="hideHint('l2tp_mppe')"><input name="l2tp_srv_mppe_enabled" type="checkbox">
              &nbsp;<b>Require MPPE</b></td>
          </tr>
        </table>
        <div id="l2tpUserList"> </div>
        <table class="buttons">
          <tr>
            <td><input type="submit" class="normal" value="Apply">
              &nbsp;&nbsp;
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
