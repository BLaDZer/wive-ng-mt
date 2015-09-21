<!DOCTYPE html>
<html>
<head>
<title>Access Point Status</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("admin");
Butterlate.setTextDomain("mode");
Butterlate.setTextDomain("buttons");

var wan_port = '<% getCfgZero(1, "wan_port"); %>';
var lan_port = '<% getCfgZero(1, "lan_port"); %>';
var timer = null;

function showOpMode()
{
	var el = document.getElementById('inpOperationMode');
	if (el == null)
		return;

	var opmode = el.value;
	var s_opmode = 'Unknown';
	if (opmode == '0')
		s_opmode = _("opmode mode bridge");
	else if (opmode == '1')
		s_opmode = _("opmode mode gateway");
	else if (opmode == '2')
		s_opmode = _("opmode mode e");
	else if (opmode == '3')
		s_opmode = _("opmode mode a");

	ajaxModifyElementHTML('tdOperationMode', s_opmode);
}

function initTranslation()
{
	_TR("statusTitle", "status title");
	_TR("statusIntroduction", "status introduction");

	_TR("statusSysInfo", "status system information");
	_TR("statusSDKVersion", "status sdk version");
	_TR("statusSysDateTime", "status system date time");
	_TR("statusSysUpTime", "status system up time");
	_TR("statusSysPlatform", "status system platform");
	_TR("statusOPMode", "status op mode");

	_TR("statusInternetConfig", "status internet config");
	_TR("statusConnectedType", "status connect type");
	_TR("statusINTIPAddr", "status int ipaddr");
	_TR("statusWANIPAddr", "status wan ipaddr");
	_TR("statusSubnetMask", "status subnet mask");
	_TR("statusDefaultGW", "status default gateway");
	_TR("statusPrimaryDNS", "status primary dns");
	_TR("statusSecondaryDNS", "status secondary dns");
	_TR("statusWANMAC", "status mac");

	_TR("statusLocalNet", "status local network");
	_TR("statusLANIPAddr", "status lan ipaddr");
	_TR("statusLocalNetmask", "status local netmask");
	_TR("statusLANMAC", "status mac");

	_TR("statusPortManagement", "status port management");
	_TR("statusWANport", "status wan port");
	_TR("statusFirstLANport", "status first lan port");
	_TR("statusNearToWAN", "status near");
	_TR("statusDistantFromWAN", "status distant");
	_TR("statusPortMode1", "status port mode 1");
	_TR("statusPortMode2", "status port mode 2");
	_TR("statusPortMode3", "status port mode 3");
	_TR("statusPortMode4", "status port mode 4");
	_TR("statusPortMode5", "status port mode 5");
	_TR("statusIPv6IntAddr", "status internal ipv6 address");
	_TR("statusIPv6ExtAddr", "status external ipv6 address");

	_TRV("statusApply", "button apply");
}

function PageInit()
{
	var ethtoolb       = "<% getETHTOOLBuilt(); %>";
	var tv_stb         = "<% getCfgZero(1, "tv_port"); %>"; // TV/STB/VLAN1
	var sip_stb        = "<% getCfgZero(1, "sip_port"); %>"; // SIP/STB/VLAN2
	var tv_stb_mcast   = "<% getCfgZero(1, "tv_port_mcast"); %>"; // TV/STB/VLAN1
	var sip_stb_mcast  = "<% getCfgZero(1, "sip_port_mcast"); %>"; // SIP/STB/VLAN2

	if (ethtoolb == "1")
		showElement('div_ethtool');

	if (!((wan_port >= '0') && (wan_port <= '4')))
		wan_port = '4';

	// Only WLLLL, WWLLL, LLLWW, LLLLW modes supported now
	if ((wan_port != '0') && (wan_port != '4'))
		wan_port = '4';

	if (((lan_port != 'near') && (lan_port != 'distant')))
		lan_port = 'near';

	var form = document.setWanForm;

	form.wan_port.value = wan_port;
	form.lan_port.value = lan_port;
	form.tv_stbEnabled.checked = (tv_stb == '1');
	form.sip_stbEnabled.checked = (sip_stb == '1');
	form.tv_stbMcast.checked = (tv_stb_mcast == '1');
	form.sip_stbMcast.checked = (sip_stb_mcast == '1');

	var gigaphy = '<% gigaphy(); %>';

	// Add gigabit mode to combo
	if (gigaphy == 1)
	{
	    index1 = form.port1_swmode.options.length;
	    index2 = form.port2_swmode.options.length;
	    index3 = form.port3_swmode.options.length;
	    index4 = form.port4_swmode.options.length;
	    index5 = form.port5_swmode.options.length;

	    form.port1_swmode.options[index1++] = new Option("1000 mbit/s full duplex", "1000f")
	    form.port2_swmode.options[index2++] = new Option("1000 mbit/s full duplex", "1000f")
	    form.port3_swmode.options[index3++] = new Option("1000 mbit/s full duplex", "1000f")
	    form.port4_swmode.options[index4++] = new Option("1000 mbit/s full duplex", "1000f")
	    form.port5_swmode.options[index5++] = new Option("1000 mbit/s full duplex", "1000f")
	}

	// Show port speeds
	var port_swmode = [ '<% getCfgZero(1, "port1_swmode"); %>', '<% getCfgZero(1, "port2_swmode"); %>', '<% getCfgZero(1, "port3_swmode"); %>', '<% getCfgZero(1, "port4_swmode"); %>', '<% getCfgZero(1, "port5_swmode"); %>' ];

	for (var i=0; i<port_swmode.length; )
	{
		var port_speed = port_swmode[i++]; // Port number is 1..n
		var element = form.elements['port' + i + '_swmode'];
		if (element != null)
			element.value = port_speed;
	}

	reloadPage();
}

function reloadPage()
{
	var reloader = function()
	{
		var defaultGW = "<% getWanGateway(); %>";
		var ipv6b = "<% getIPv6Built(); %>";
		var IPv6Int = "<% getIPv6IntAddr(); %>";
		var IPv6Ext = "<% getIPv6ExtAddr(); %>";
		ajaxModifyElementHTML("statusDGW", defaultGW);
		ajaxModifyElementHTML("IPv6IntAddr", IPv6Int);
		ajaxModifyElementHTML("IPv6ExtAddr", IPv6Ext);
		displayElement('defaultGW', (defaultGW != ""));
		displayElement('statusIPv6Int', (ipv6b == "1") && (IPv6Int != ""));
		displayElement('statusIPv6Ext', (ipv6b == "1") && (IPv6Ext != ""));
		initTranslation();
		showOpMode();
		timer = self.setTimeout('reloadPage();', 3000);
	}

	ajaxLoadElement("sysinfoTable", "/adm/sysinfo.asp", reloader);
}

function setWanPort(form)
{
	if (!ajaxPostForm(_("status ask reboot"), form, 'setwanReloader', _("message reboot"))) {
		form.reboot.value = "0";
    	form.submit();
	}
}

</script>
</head>

<body onLoad="PageInit();">
<table class="body">
  <tr>
    <td><h1 id="statusTitle">Access Point Status</h1>
      <p id="statusIntroduction">Let's take a look at the status. </p>
      <hr>
      <div id="sysinfoTable">
        <table class="form">
        </table>
      </div>
      <form name="setWanForm" method="POST" action="/goform/setWanPort">
        <table class="form">
          <tr>
            <td class="title" colspan="3" id="statusPortManagement">Port Management</td>
          </tr>
          <tr>
            <td class="head" id="statusWANport">WAN port</td>
            <td colspan="2"><select name="wan_port" onChange="showPortStatus();" class="short">
                <option value="0">1</option>
                <option value="4">5</option>
              </select>
              <iframe id="setwanReloader" name="setwanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe></td>
          </tr>
          <tr>
            <td class="head" id="statusFirstLANport">First LAN port</td>
            <td colspan="2"><select name="lan_port" onChange="showPortStatus();" class="mid">
                <option value="near" id="statusNearToWAN">Near to the WAN port</option>
                <option value="distant" id="statusDistantFromWAN">Distant from the WAN port</option>
              </select>
              </td>
          </tr>
          <tr>
            <td class="head" id="tv_stb">TV/STB/VLAN1</td>
            <td>Enable:<input name="tv_stbEnabled" type="checkbox" onChange="showPortStatus();">
                McastProxy:<input name="tv_stbMcast" type="checkbox" onChange="showPortStatus();"></td>
            <td>VlanID(s):<input name="tv_stbVLAN" class="wide" size="30" maxlength="60" type="text" value="<% getCfgGeneral(1, "tv_portVLAN"); %>"</td>
          </tr>
          <tr>
            <td class="head" id="sip_stb">SIP/STB/VLAN2</td>
            <td>Enable:<input name="sip_stbEnabled" type="checkbox" onChange="showPortStatus();">
                McastProxy:<input name="sip_stbMcast" type="checkbox" onChange="showPortStatus();"></td>
            <td>VlanID(s):<input name="sip_stbVLAN" class="wide" size="30" maxlength="60" type="text" value="<% getCfgGeneral(1, "sip_portVLAN"); %>"</td>
          </tr>
          <tr>
          <tr>
            <td class="head" id="statusPortMode1">Port 1 mode</td>
            <td colspan="2"><select name="port1_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode2">Port 2 mode</td>
            <td colspan="2"><select name="port2_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode3">Port 3 mode</td>
            <td colspan="2"><select name="port3_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode4">Port 4 mode</td>
            <td colspan="2"><select name="port4_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode5">Port 5 mode</td>
            <td colspan="2"><select name="port5_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
        </table>
        <input type="button" class="mid" id="statusApply" value="Apply" onClick="setWanPort(this.form);" />
        <input type="hidden" value="1" name="reboot">
        <input type="hidden" name="submit-url" value="/adm/status.asp">
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
