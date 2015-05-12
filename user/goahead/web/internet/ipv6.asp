<!DOCTYPE html>
<html>
<head>
<title>IPv6 Settings</title>
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

Butterlate.setTextDomain("buttons");
Butterlate.setTextDomain("network");

var ipv66rdb = "<% getIPv66rdBuilt(); %>";
var ip6to4b = "<% getIP6to4Built(); %>";
var vpn = "<% getCfgZero(1, "vpnEnabled"); %>";
var vpnv6 = "<% getCfgZero(1, "Ipv6InVPN"); %>";

function SwitchOpMode(form)
{
	enableElements( [ form.ipv6_allow_forward ], (form.ipv6_opmode.options.selectedIndex != 0));
	enableElements( [ form.ipv6_Ipv6InVPN ], (form.ipv6_opmode.options.selectedIndex != 0) && (vpn == "on"));
	enableElements( [ form.dhcp6c_enable ], (form.ipv6_opmode.options.selectedIndex == 1));
	enableElements( [ form.ipv6_lan_ipaddr, form.ipv6_lan_prefix_len, form.ipv6_wan_ipaddr, form.ipv6_wan_prefix_len, form.ipv6_static_gw ], (form.ipv6_opmode.options.selectedIndex == 1) && (!form.dhcp6c_enable.checked));
	enableElements( [ form.ipv6_6rd_prefix, form.ipv6_6rd_prefix_len, form.ipv6_6rd_border_ipaddr ], (ipv66rdb == "1") && (form.ipv6_opmode.options.selectedIndex == 2));
	enableElements( [ form.IPv6SrvAddr ], (form.ipv6_opmode.options.selectedIndex == form.ipv6_opmode.options.length-1));

	displayElement( [ 'IPv6AllowForwardRowDisplay' ], (form.ipv6_opmode.options.selectedIndex != 0));
	displayElement( [ 'v6invpn' ], (form.ipv6_opmode.options.selectedIndex != 0) && (vpn == "on"));
	displayElement( [ 'dhcp6cRowDisplay' ], (form.ipv6_opmode.options.selectedIndex == 1));
	displayElement( [ 'v6StaticTable' ], (form.ipv6_opmode.options.selectedIndex == 1) && (!form.dhcp6c_enable.checked));
	displayElement( [ 'v66rdTable' ], (ipv66rdb == "1") && (form.ipv6_opmode.options.selectedIndex == 2));
	displayElement( [ '6to4Table' ], (form.ipv6_opmode.options.selectedIndex == form.ipv6_opmode.options.length-1));
	displayElement( [ 'daemons' ], (form.ipv6_opmode.options.selectedIndex != 0));
}

function initValue()
{
	var form = document.ipv6_cfg;
	var opmode = "<% getCfgZero(1, "IPv6OpMode"); %>";
	var dhcp6c = "<% getCfgZero(1, "IPv6Dhcpc"); %>";
	var ipv6_allow_forward = "<% getCfgZero(1, "IPv6AllowForward"); %>";
	var opmode_len = form.ipv6_opmode.options.length;
	var radvdb = "<% getRadvdBuilt(); %>";
	var dhcpv6b = "<% getDhcpv6Built(); %>";
	var radvd = <% getCfgZero(1, "radvdEnabled"); %>;
	var dhcpv6 = <% getCfgZero(1, "dhcpv6Enabled"); %>;


	if (ipv66rdb == "1") {
		form.ipv6_opmode.options[2] = new Option(_("ipv6 6rd"), "2");
		opmode_len++;
	}
	if (ip6to4b == "1") {
		form.ipv6_opmode.options[opmode_len] = new Option(_("ipv6 6to4"), "3");
		opmode_len++;
	}

	if (opmode == "1")
		form.ipv6_opmode.options.selectedIndex = 1;
	else if (opmode == "2")
		form.ipv6_opmode.options.selectedIndex = 2;
	else if (opmode == "3")
		form.ipv6_opmode.options.selectedIndex = opmode_len-1;

	SwitchOpMode(form);

	if (opmode == "1") {
		form.ipv6_lan_ipaddr.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		form.ipv6_lan_prefix_len.value = "<% getCfgGeneral(1, "IPv6PrefixLen"); %>";
		form.ipv6_wan_ipaddr.value = "<% getCfgGeneral(1, "IPv6WANIPAddr"); %>";
		form.ipv6_wan_prefix_len.value = "<% getCfgGeneral(1, "IPv6WANPrefixLen"); %>";
		form.ipv6_static_gw.value = "<% getCfgGeneral(1, "IPv6GWAddr"); %>";
	} else if (opmode == "2") {
		form.ipv6_6rd_prefix.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		form.ipv6_6rd_prefix_len.value = "<% getCfgGeneral(1, "IPv6PrefixLen"); %>";
		form.ipv6_6rd_border_ipaddr.value = "<% getCfgGeneral(1, "IPv6SrvAddr"); %>";
	} else if (opmode == "3") {
		form.IPv6SrvAddr.value = "<% getCfgGeneral(1, "IPv6SrvAddr"); %>";
	}

	form.dhcp6c_enable.checked = (dhcp6c == "1");
	if (dhcp6c == "1")
	    document.getElementById("v6StaticTable").style.visibility = "hidden";

	form.ipv6_allow_forward.checked = (ipv6_allow_forward == "1");
	form.ipv6_Ipv6InVPN.checked = (vpnv6 == "1");

	form.radvdEnbl.options.selectedIndex = 1*radvd;
	form.dhcpv6Enbl.options.selectedIndex = 1*dhcpv6;

	displayElement('radvd', radvdb == '1');
	displayElement('dhcpv6', dhcpv6b == '1');

	initTranslation();

	displayServiceStatus();
}

function atoi(str, num)
{
	i = 1;
	if (num != 1) {
		while (i != num && str.length != 0) {
			if (str.charAt(0) == '.') {
				i++;
			}
			str = str.substring(1);
		}
		if (i != num)
			return -1;
	}

	for (i=0; i<str.length; i++) {
		if (str.charAt(i) == '.') {
			str = str.substring(0, i);
			break;
		}
	}
	if (str.length == 0)
		return -1;
	return parseInt(str, 10);
}

function checkRange(str, num, min, max)
{
	d = atoi(str, num);
	if (d > max || d < min)
		return false;
	return true;
}

function isAllNum(str)
{
	for (var i=0; i<str.length; i++) {
		if ((str.charAt(i) >= '0' && str.charAt(i) <= '9') || (str.charAt(i) == '.' ))
			continue;
		return 0;
	}
	return 1;
}

function checkIpv4Addr(field)
{
	if (field.value == "") {
		alert("Error. IP address is empty.");
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if (isAllNum(field.value) == 0) {
		alert('It should be a [0-9] number.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}

	if ((!checkRange(field.value, 1, 0, 255)) ||
			(!checkRange(field.value, 2, 0, 255)) ||
			(!checkRange(field.value, 3, 0, 255)) ||
			(!checkRange(field.value, 4, 1, 254)))
	{
		alert('IP adress format error.');
		field.value = field.defaultValue;
		field.focus();
		return false;
	}
	return true;
}

function checkIpv6Addr(ip_addr, len)
{
	var ip_item = new Array();
	ip_item = ip_addr.split(":");
	for (var i=0; i<ip_item.length; i++) {
		if (parseInt(ip_item[i], 16) == "NaN") {
			alert('It should be a [0-F] number.');
			return false;
		}
	}

	return true;
}

function CheckValue(form)
{
	if (form.ipv6_opmode.value == "1") {
		if (form.ipv6_lan_ipaddr.value != "") {
			if (!checkIpv6Addr(form.ipv6_lan_ipaddr.value, 128)) {
				alert("invalid IPv6 IP address!");
				form.ipv6_lan_ipaddr.focus();
				form.ipv6_lan_ipaddr.select();
				return false;
			}
			if (form.ipv6_lan_prefix_len.value == "" ||
			    form.ipv6_lan_prefix_len.value > 128 ||
			    form.ipv6_lan_prefix_len.value < 0) {
				alert("invalid prefix length!");
				form.ipv6_lan_prefix_len.focus();
				form.ipv6_lan_prefix_len.select();
				return false;
			}
		}
		if (form.ipv6_wan_ipaddr.value != "") {
			if (!checkIpv6Addr(form.ipv6_wan_ipaddr.value, 128)) {
				alert("invalid IPv6 IP address!");
				form.ipv6_wan_ipaddr.focus();
				form.ipv6_wan_ipaddr.select();
				return false;
			}
			if (form.ipv6_wan_prefix_len.value == "" ||
			    form.ipv6_wan_prefix_len.value > 128 ||
			    form.ipv6_wan_prefix_len.value < 0) {
				alert("invalid prefix length!");
				form.ipv6_wan_prefix_len.focus();
				form.ipv6_wan_prefix_len.select();
				return false;
			}
		}
		if (form.ipv6_static_gw.value != "" &&
		    (!checkIpv6Addr(form.ipv6_static_gw.value, 128))) {
			alert("invalid IPv6 IP address!");
			form.ipv6_static_gw.focus();
			form.ipv6_static_gw.select();
			return false;
		}
	} else if (form.ipv6_opmode.value == "2") {
		if (form.ipv6_6rd_prefix.value == "" ||
		    form.ipv6_6rd_prefix_len.value == "" ||
		    form.ipv6_6rd_border_ipaddr.value == "") {
			alert("please fill all fields!");
			return false;
		}
		if (!checkIpv6Addr(form.ipv6_6rd_prefix.value, 32)) {
			alert("invalid IPv6 IP address!");
			form.ipv6_6rd_prefix.focus();
			form.ipv6_6rd_prefix.select();
			return false;
		}
		if (form.ipv6_6rd_prefix_len.value > 32 ||
		    form.ipv6_6rd_prefix_len.value < 0) {
			alert("invalid prefix length!");
			form.ipv6_6rd_prefix_len.focus();
			form.ipv6_6rd_prefix_len.select();
			return false;
		}
		if (!checkIpv4Addr(form.ipv6_6rd_border_ipaddr.value)) {
			alert("invalid IPv4 ip address!");
			form.ipv6_6rd_border_ipaddr.focus();
			form.ipv6_6rd_border_ipaddr.select();
			return false;
		}
	} else if (form.ipv6_opmode.value == "3") {
		if (form.IPv6SrvAddr.value == "" ) {
			alert("Please fill all fields!");
			return false;
		}
		if (!checkIpv4Addr(form.IPv6SrvAddr.value)) {
			alert("invalid IPv4 ip address!");
			form.IPv6SrvAddr.focus();
			form.IPv6SrvAddr.select();
			return false;
		}
	}
	return true;
}

function initTranslation()
{
	_TR("v6Title", "ipv6 title");
	_TR("v6Introduction", "ipv6 introduction");
	_TR("v6ConnType", "ipv6 connection type");
	_TR("v6OpMode", "ipv6 operation mode");
	_TR("v6Disable", "button disable");
	_TR("v6Static", "ipv6 static");
	_TR("IPv6Dhcpc", "ipv6 dhcp6c");
	_TR("IPv6AllowForward", "ipv6 allow forward");
	_TR("v6StaticIPSetup", "ipv6 static ip");
	_TR("v6StaticIPAddrLan", "ipv6 static lan address");
	_TR("v6StaticIPAddrWan", "ipv6 static wan address");
	_TR("v6StaticGW", "inet gateway");
	_TR("v66rdSetup", "ipv6 6rd enabled");
	_TR("v66rdPrefix", "ipv6 6rd prefix");
	_TR("v66rdBorderIPAddr", "ipv6 6rd relay address");
	_TR("6to4Setup", "ipv6 6to4 enabled");
	_TR("v66to4SrvIpaddr", "ipv6 6to4 server address");
	_TR("Ipv6InVPN", "ipv6 Ipv6InVPN");

	_TR("v6services", "ipv6 services");
	_TR("v6servicename", "ipv6 service name");
	_TR("v6value", "ipv6 service value");
	_TR("v6status", "ipv6 service status");
	_TR("v6Radvd", "ipv6 radvd");
	_TR("v6RadvdD", "button disable");
	_TR("v6RadvdE", "button enable");
	_TR("v6Dhcpv6", "ipv6 dhcp6s");
	_TR("v6Dhcpv6D", "button disable");
	_TR("v6Dhcpv6E", "button enable");


	_TRV("v6Apply", "button apply");
	_TRV("v6Cancel", "button cancel");
}

function displayServiceHandler(response)
{
	var form = document.miscServiceCfg;

	var services = [
		// turned_on, row_id, daemon_id
		[ '<% getCfgGeneral(1, "radvdEnabled"); %>', 'radvd', 'radvd' ],
		[ '<% getCfgGeneral(1, "dhcpv6Enabled"); %>', 'dhcpv6', 'dhcp6s' ]
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
				tds[2].innerHTML = '<span style="color: #808080"><b>off</b></span>';
			else
				tds[2].innerHTML = (daemons[service[2]] == 1) ?
					'<span style="color: #3da42c"><b>work</b></span>' :
					'<span style="color: #808000"><b>starting</b></span>';
		}
	}

	serviceStatusTimer = setTimeout('displayServiceStatus();', 5000);
}

function displayServiceStatus()
{
	ajaxPerformRequest('/internet/services-stat.asp', displayServiceHandler);
}

</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="v6Title">IPv6 Setup</h1>
<p id="v6Introduction"></p>
<hr />

<form method=post name="ipv6_cfg" action="/goform/setIPv6" onSubmit="return CheckValue(this.form);">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="v6ConnType">IPv6 Connection Type</td>
</tr>
<tr>
  <td class="head" id="v6OpMode">IPv6 Operation Mode</td>
  <td>
    <select name="ipv6_opmode" size="1" onChange="SwitchOpMode(this.form);">
      <option value="0" id="v6Disable">Disable</option>
      <option value="1" id="v6Static">Native dynamic/static IP Connection</option>
    </select>
  </td>
</tr>
<tr id="v6invpn">
  <td class="head" id="Ipv6InVPN">IPv6 over VPN</td>
  <td><input name="ipv6_Ipv6InVPN" type="checkbox"></td>
</tr>
<tr id="dhcp6cRowDisplay">
  <td class="head" id="IPv6Dhcpc">IPv6 autoconfigure by dhcp/ra</td>
  <td><input name="dhcp6c_enable" type="checkbox" onChange="SwitchOpMode(this.form);"></td>
</tr>
<tr id="IPv6AllowForwardRowDisplay">
  <td class="head" id="IPv6AllowForward">Allow access to LAN from internet</td>
  <td><input name="ipv6_allow_forward" type="checkbox"></td>
</tr>
</table>
<!-- STATIC/DynaMIC IP -->
<table width="95%" id="v6StaticTable" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="2" id="v6StaticIPSetup">IPv6 Static IP Setup</td>
</tr>
<tr>
  <td class="head" id="v6StaticIPAddrLan">LAN IPv6 Address / Subnet Prefix Length</td>
  <td><input name="ipv6_lan_ipaddr" maxlength=39 size=27> / <input name="ipv6_lan_prefix_len" maxlength=3 size=2></td>
</tr>
<tr>
  <td class="head" id="v6StaticIPAddrWan">WAN IPv6 Address / Subnet Prefix Length</td>
  <td><input name="ipv6_wan_ipaddr" maxlength=39 size=27> / <input name="ipv6_wan_prefix_len" maxlength=3 size=2></td>
</tr>
<tr>
  <td class="head" id="v6StaticGW">Default Gateway</td>
  <td><input name="ipv6_static_gw" maxlength=39 size=27></td>
</tr>
</table>
<!-- 6RD -->
<table width="95%" id="v66rdTable" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="2" id="v66rdSetup">Tunneling Connection (6RD) Setup</td>
</tr>
<tr>
  <td class="head" id="v66rdPrefix">ISP 6rd Prefix / Prefix Length</td>
  <td><input name="ipv6_6rd_prefix" maxlength=9 size=7> / <input name="ipv6_6rd_prefix_len" maxlength=3 size=2></td>
</tr>
<tr>
  <td class="head" id="v66rdBorderIPAddr">ISP Border Relay IPv4 Address</td>
  <td><input name="ipv6_6rd_border_ipaddr" maxlength=15 size=13></td>
</tr>
</table>
<!-- 6to4 -->
<table width="95%" id="6to4Table" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="2" id="6to4Setup">Tunneling Connection (6to4) Setup</td>
</tr>
<tr>
  <td class="head" id="v66to4SrvIpaddr"> IPv4 to IPv6 server address </td>
  <td><input name="IPv6SrvAddr" maxlength=39 size=27></td>
</tr>
</table>
<!-- Settings daemons for lan -->
<table width="95%" id="daemons" border="1" bordercolor="#9babbd" cellpadding="3" cellspacing="1" hspace="2" vspace="2" width="540" style="visibility: hidden;">
<tr>
  <td class="title" colspan="3" id="v6services">Services IPv6</td>
</tr>
<tr>
    <td class="title" id="v6servicename">Service name</td>
    <td class="title" id="v6value">Value</td>
    <td class="title" style="width: 56px;" id="v6status">Status</td>
</tr>
<tr id="radvd">
	<td class="head" id="v6Radvd">Router Advertisement</td>
    <td><select name="radvdEnbl" class="half">
        <option value="0" id="v6RadvdD">Disable</option>
        <option value="1" id="v6RadvdE">Enable</option>
    </select></td>
    <td>&nbsp;</td>
</tr>
<tr id="dhcpv6">
    <td class="head" id="v6Dhcpv6">Dynamic IPv6 configuration</td>
    <td><select name="dhcpv6Enbl" class="half">
        <option value="0" id="v6Dhcpv6D">Disable</option>
        <option value="1" id="v6Dhcpv6E">Enable</option>
    </select></td>
    <td>&nbsp;</td>
</tr>
</table>
<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
  <td>
    <input type=submit style="{width:120px;}" value="Apply" id="v6Apply">&nbsp;&nbsp;
    <input type=reset  style="{width:120px;}" value="Cancel" id="v6Cancel">
    <input type="hidden" value="/internet/ipv6.asp" name="submit-url">
  </td>
</tr>
</table>
</form>
</td></tr></table>
</body>
</html>
