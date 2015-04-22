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
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("buttons");
Butterlate.setTextDomain("network");

var ipv66rdb = "<% getIPv66rdBuilt(); %>";
var ip6to4b = "<% getIP6to4Built(); %>";

function display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Firefox, Safari,...
		return "table-row";
	}
}

function SwitchOpMode()
{
	document.getElementById("v6StaticTable").style.visibility = "hidden";
	document.getElementById("v6StaticTable").style.display = "none";
	document.ipv6_cfg.ipv6_lan_ipaddr.disabled = true;
	document.ipv6_cfg.ipv6_lan_prefix_len.disabled = true;
	document.ipv6_cfg.ipv6_wan_ipaddr.disabled = true;
	document.ipv6_cfg.ipv6_wan_prefix_len.disabled = true;
	document.ipv6_cfg.ipv6_static_gw.disabled = true;
	document.getElementById("v66rdTable").style.visibility = "hidden";
	document.getElementById("v66rdTable").style.display = "none";
	document.ipv6_cfg.ipv6_6rd_prefix.disabled = true;
	document.ipv6_cfg.ipv6_6rd_prefix_len.disabled = true;
	document.ipv6_cfg.ipv6_6rd_border_ipaddr.disabled = true;
	document.getElementById("6to4Table").style.visibility = "hidden";
	document.getElementById("6to4Table").style.display = "none";
	document.ipv6_cfg.IPv6SrvAddr.disabled = true;

	if (document.ipv6_cfg.ipv6_opmode.options.selectedIndex == 1) {
		document.getElementById("v6StaticTable").style.visibility = "visible";
		document.getElementById("v6StaticTable").style.display = display_on();
		document.ipv6_cfg.ipv6_lan_ipaddr.disabled = false;
		document.ipv6_cfg.ipv6_lan_prefix_len.disabled = false;
		document.ipv6_cfg.ipv6_wan_ipaddr.disabled = false;
		document.ipv6_cfg.ipv6_wan_prefix_len.disabled = false;
		document.ipv6_cfg.ipv6_static_gw.disabled = false;
	} else if (ipv66rdb == "1" && document.ipv6_cfg.ipv6_opmode.options.selectedIndex == 2) {
		document.getElementById("v66rdTable").style.visibility = "visible";
		document.getElementById("v66rdTable").style.display = display_on();
		document.ipv6_cfg.ipv6_6rd_prefix.disabled = false;
		document.ipv6_cfg.ipv6_6rd_prefix_len.disabled = false;
		document.ipv6_cfg.ipv6_6rd_border_ipaddr.disabled = false;
	} else if (document.ipv6_cfg.ipv6_opmode.options.selectedIndex == document.ipv6_cfg.ipv6_opmode.options.length-1) {
		document.getElementById("6to4Table").style.visibility = "visible";
		document.getElementById("6to4Table").style.display = display_on();
		document.ipv6_cfg.IPv6SrvAddr.disabled = false;
	}
}

function initValue()
{
	var opmode = "<% getCfgZero(1, "IPv6OpMode"); %>";
	var dhcp6c = "<% getCfgZero(1, "IPv6Dhcpc"); %>";
	var ipv6_allow_forward = "<% getCfgZero(1, "IPv6AllowForward"); %>";
	var opmode_len = document.ipv6_cfg.ipv6_opmode.options.length;

	if (ipv66rdb == "1") {
		document.ipv6_cfg.ipv6_opmode.options[2] = new Option(_("ipv6 6rd"), "2");
		opmode_len++;
	}
	if (ip6to4b == "1") {
		document.ipv6_cfg.ipv6_opmode.options[opmode_len] = new Option(_("ipv6 6to4"), "3");
		opmode_len++;
	}

	if (opmode == "1")
		document.ipv6_cfg.ipv6_opmode.options.selectedIndex = 1;
	else if (opmode == "2")
		document.ipv6_cfg.ipv6_opmode.options.selectedIndex = 2;
	else if (opmode == "3")
		document.ipv6_cfg.ipv6_opmode.options.selectedIndex = opmode_len-1;

	SwitchOpMode();

	if (opmode == "1") {
		document.ipv6_cfg.ipv6_lan_ipaddr.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		document.ipv6_cfg.ipv6_lan_prefix_len.value = "<% getCfgGeneral(1, "IPv6PrefixLen"); %>";
		document.ipv6_cfg.ipv6_wan_ipaddr.value = "<% getCfgGeneral(1, "IPv6WANIPAddr"); %>";
		document.ipv6_cfg.ipv6_wan_prefix_len.value = "<% getCfgGeneral(1, "IPv6WANPrefixLen"); %>";
		document.ipv6_cfg.ipv6_static_gw.value = "<% getCfgGeneral(1, "IPv6GWAddr"); %>";
	} else if (opmode == "2") {
		document.ipv6_cfg.ipv6_6rd_prefix.value = "<% getCfgGeneral(1, "IPv6IPAddr"); %>";
		document.ipv6_cfg.ipv6_6rd_prefix_len.value = "<% getCfgGeneral(1, "IPv6PrefixLen"); %>";
		document.ipv6_cfg.ipv6_6rd_border_ipaddr.value = "<% getCfgGeneral(1, "IPv6SrvAddr"); %>";
	} else if (opmode == "3") {
		document.ipv6_cfg.IPv6SrvAddr.value = "<% getCfgGeneral(1, "IPv6SrvAddr"); %>";
	}

	document.ipv6_cfg.IPv6Dhcpc.checked = (dhcp6c == "1");
	if (dhcp6c == "1")
	    document.getElementById("v6StaticTable").style.visibility = "hidden";

	document.ipv6_cfg.IPv6AllowForward.checked = (ipv6_allow_forward == "1");
	initTranslation();
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

function CheckValue()
{
	if (document.ipv6_cfg.ipv6_opmode.value == "1") {
		if (document.ipv6_cfg.ipv6_lan_ipaddr.value != "") {
			if (!checkIpv6Addr(document.ipv6_cfg.ipv6_lan_ipaddr.value, 128)) {
				alert("invalid IPv6 IP address!");
				document.ipv6_cfg.ipv6_lan_ipaddr.focus();
				document.ipv6_cfg.ipv6_lan_ipaddr.select();
				return false;
			}
			if (document.ipv6_cfg.ipv6_lan_prefix_len.value == "" ||
			    document.ipv6_cfg.ipv6_lan_prefix_len.value > 128 ||
			    document.ipv6_cfg.ipv6_lan_prefix_len.value < 0) {
				alert("invalid prefix length!");
				document.ipv6_cfg.ipv6_lan_prefix_len.focus();
				document.ipv6_cfg.ipv6_lan_prefix_len.select();
				return false;
			}
		}
		if (document.ipv6_cfg.ipv6_wan_ipaddr.value != "") {
			if (!checkIpv6Addr(document.ipv6_cfg.ipv6_wan_ipaddr.value, 128)) {
				alert("invalid IPv6 IP address!");
				document.ipv6_cfg.ipv6_wan_ipaddr.focus();
				document.ipv6_cfg.ipv6_wan_ipaddr.select();
				return false;
			}
			if (document.ipv6_cfg.ipv6_wan_prefix_len.value == "" ||
			    document.ipv6_cfg.ipv6_wan_prefix_len.value > 128 ||
			    document.ipv6_cfg.ipv6_wan_prefix_len.value < 0) {
				alert("invalid prefix length!");
				document.ipv6_cfg.ipv6_wan_prefix_len.focus();
				document.ipv6_cfg.ipv6_wan_prefix_len.select();
				return false;
			}
		}
		if (document.ipv6_cfg.ipv6_static_gw.value != "" &&
		    (!checkIpv6Addr(document.ipv6_cfg.ipv6_static_gw.value, 128))) {
			alert("invalid IPv6 IP address!");
			document.ipv6_cfg.ipv6_static_gw.focus();
			document.ipv6_cfg.ipv6_static_gw.select();
			return false;
		}
	} else if (document.ipv6_cfg.ipv6_opmode.value == "2") {
		if (document.ipv6_cfg.ipv6_6rd_prefix.value == "" ||
		    document.ipv6_cfg.ipv6_6rd_prefix_len.value == "" ||
		    document.ipv6_cfg.ipv6_6rd_border_ipaddr.value == "") {
			alert("please fill all fields!");
			return false;
		}
		if (!checkIpv6Addr(document.ipv6_cfg.ipv6_6rd_prefix.value, 32)) {
			alert("invalid IPv6 IP address!");
			document.ipv6_cfg.ipv6_6rd_prefix.focus();
			document.ipv6_cfg.ipv6_6rd_prefix.select();
			return false;
		}
		if (document.ipv6_cfg.ipv6_6rd_prefix_len.value > 32 ||
		    document.ipv6_cfg.ipv6_6rd_prefix_len.value < 0) {
			alert("invalid prefix length!");
			document.ipv6_cfg.ipv6_6rd_prefix_len.focus();
			document.ipv6_cfg.ipv6_6rd_prefix_len.select();
			return false;
		}
		if (!checkIpv4Addr(document.ipv6_cfg.ipv6_6rd_border_ipaddr.value)) {
			alert("invalid IPv4 ip address!");
			document.ipv6_cfg.ipv6_6rd_border_ipaddr.focus();
			document.ipv6_cfg.ipv6_6rd_border_ipaddr.select();
			return false;
		}
	} else if (document.ipv6_cfg.ipv6_opmode.value == "3") {
		if (document.ipv6_cfg.IPv6SrvAddr.value == "" ) {
			alert("Please fill all fields!");
			return false;
		}
		if (!checkIpv4Addr(document.ipv6_cfg.IPv6SrvAddr.value)) {
			alert("invalid IPv4 ip address!");
			document.ipv6_cfg.IPv6SrvAddr.focus();
			document.ipv6_cfg.IPv6SrvAddr.select();
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
	_TR("IPv6Dhcpc", "ipv6 dhcp");
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

	_TRV("v6Apply", "button apply");
	_TRV("v6Cancel", "button cancel");
}
</script>
</head>

<body onLoad="initValue()">
<table class="body"><tr><td>

<h1 id="v6Title">IPv6 Setup</h1>
<p id="v6Introduction"></p>
<hr />

<form method=post name="ipv6_cfg" action="/goform/setIPv6" onSubmit="return CheckValue()">
<table width="95%" border="1" cellpadding="2" cellspacing="1">
<tr>
  <td class="title" colspan="2" id="v6ConnType">IPv6 Connection Type</td>
</tr>
<tr>
  <td class="head" id="v6OpMode">IPv6 Operation Mode</td>
  <td>
    <select name="ipv6_opmode" size="1" onChange="SwitchOpMode()">
      <option value="0" id="v6Disable">Disable</option>
      <option value="1" id="v6Static">Native dynamic/static IP Connection</option>
    </select>
  </td>
</tr>
<tr id="dhp6cRowDisplay">
  <td class="head" id="IPv6Dhcpc">IPv6 autoconfigure by dhcp/ra</td>
  <td><input name="IPv6Dhcpc" type="checkbox"></td>
</tr>
<tr id="IPv6AllowForwardRowDisplay">
  <td class="head" id="IPv6AllowForward">Allow access to LAN from internet</td>
  <td><input name="IPv6AllowForward" type="checkbox"></td>
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
<table width="95%" cellpadding="2" cellspacing="1">
<tr align="center">
  <td>
    <input type=submit style="{width:120px;}" value="Apply" id="v6Apply">&nbsp;&nbsp;
    <input type=reset  style="{width:120px;}" value="Cancel" id="v6Cancel">
  </td>
</tr>
</table>
</form>
</td></tr></table>
</body>
</html>
