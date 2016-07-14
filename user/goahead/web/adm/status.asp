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

function InitValue()
{
    var gwb				= "<% getGWBuilt(); %>"; // Only one port (pure AP device)
	var opmode			= "<% getCfgZero(1, "OperationMode"); %>"; //Operation Mode
    var tv_stb_on       = "<% getCfgZero(1, "tv_port"); %>"; // TV/STB/VLAN1
    var sip_stb_on      = "<% getCfgZero(1, "sip_port"); %>"; // SIP/STB/VLAN2
    var tv_stb_mcast	= "<% getCfgZero(1, "tv_port_mcast"); %>"; // TV/STB/VLAN1
    var sip_stb_mcast	= "<% getCfgZero(1, "sip_port_mcast"); %>"; // SIP/STB/VLAN2

	if (opmode != 1) {
		// hide stb/sip vlan in non gw operation mode
        hideElement(tv_stb_tr);
        hideElement(sip_stb_tr);
	}
	
    if (gwb == "0") {
        // hide unused elements in pure AP mode
        hideElement(tv_stb);
        hideElement(tv_stb_en);
        hideElement(tv_stb_vlan);
        hideElement(sip_stb);
        hideElement(sip_stb_en);
        hideElement(sip_stb_vlan);
        hideElement(statusWANport);
        hideElement(statusWANportSel);
        hideElement(statusFirstLANport);
        hideElement(statusFirstLANportSel);
        hideElement(statusPortMode2);
        hideElement(port2_swmode);
        hideElement(statusPortMode3);
        hideElement(port3_swmode);
        hideElement(statusPortMode4);
        hideElement(port4_swmode);
        hideElement(statusPortMode5);
        hideElement(port5_swmode);
    }

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
    form.tv_stbEnabled.checked = (tv_stb_on == '1');
    form.sip_stbEnabled.checked = (sip_stb_on == '1');
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
    checkSTBform();
    checkSIPform();
	showWarning();
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
		timer = self.setTimeout('reloadPage();', 15000);
    }

    ajaxLoadElement("sysinfoTable", "/adm/sysinfo.asp", reloader);
}

function setWanPort(form)
{
    document.getElementById("sip_stbMcast").disabled = false;
    document.getElementById("sip_stbVLAN").disabled = false;
    document.getElementById("tv_stbVLAN").disabled = false;
    document.getElementById("tv_stbMcast").disabled = false;
    if (!ajaxPostForm(_("status ask reboot"), form, 'setwanReloader', _("message reboot"))) {
		window.location.href=window.location.href;
		return false;
    }
	return true;
}

function checkSTBform()
{
    var stbEnable = document.getElementById("tv_stbEnabled");
    var stbVLAN = document.getElementById("tv_stbVLAN");
    var stbMcast = document.getElementById("tv_stbMcast");
    
    if (stbEnable.checked) 
    {
		stbVLAN.disabled = false;
		if (stbVLAN.value.length > 0) {
			stbMcast.disabled = false;
		} else {
			stbMcast.disabled = true;
		}
    }
    else {
		stbVLAN.disabled = true;
		stbMcast.disabled = true;
    }
}

function checkSIPform()
{
    var sipEnable = document.getElementById("sip_stbEnabled");
    var sipVLAN = document.getElementById("sip_stbVLAN");
    var sipMcast = document.getElementById("sip_stbMcast");
    
    if (sipEnable.checked) 
    {
		sipVLAN.disabled = false;
		if (sipVLAN.value.length > 0) {
			sipMcast.disabled = false;
		} else {
			sipMcast.disabled = true;
		}
	} else {
		sipVLAN.disabled = true;
		sipMcast.disabled = true;
	}
}
</script>
</head>

<body bgcolor="#FFFFFF" onLoad="InitValue();">
<table class="body">
  <tr id="warning"><tr>
  <tr>
    <td><h1 id="statusTitle">Access Point Status</h1>
      <p id="statusIntroduction">Let's take a look at the status. </p>
      <hr>
      <div id="sysinfoTable">
        <table class="form">
        </table>
      </div>
      <form name="setWanForm" method="POST" action="/goform/setWanPort" onSubmit="return setWanPort(this);" >
        <table class="form">
          <tr>
            <td class="title" colspan="3" id="statusPortManagement">Port Management</td>
          </tr>
          <tr>
            <td class="head" id="statusWANport">WAN port</td>
            <td colspan="2" id="statusWANportSel"><select name="wan_port" class="short">
                <option value="0">1</option>
                <option value="4">5</option>
              </select>
              <iframe id="setwanReloader" name="setwanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe></td>
          </tr>
          <tr>
            <td class="head" id="statusFirstLANport">First LAN port</td>
            <td colspan="2" id="statusFirstLANportSel"><select name="lan_port" class="mid">
                <option value="near" id="statusNearToWAN">Near to the WAN port</option>
                <option value="distant" id="statusDistantFromWAN">Distant from the WAN port</option>
              </select>
              </td>
          </tr>
          <tr id="tv_stb_tr">
            <td id="tv_stb" class="head">TV/STB/VLAN1</td>
            <td id="tv_stb_en">Enable:<input id="tv_stbEnabled" name="tv_stbEnabled" type="checkbox" OnClick="checkSTBform();">
                McastProxy:<input id="tv_stbMcast" name="tv_stbMcast" type="checkbox"></td>
            <td id="tv_stb_vlan">VlanID(s):<input id="tv_stbVLAN" name="tv_stbVLAN" class="wide" size="30" maxlength="60" type="text" onKeyUp="checkSTBform();" value="<% getCfgGeneral(1, "tv_portVLAN"); %>"</td>
          </tr>
          <tr id="sip_stb_tr">
            <td id="sip_stb" class="head">SIP/STB/VLAN2</td>
            <td id="sip_stb_en">Enable:<input id="sip_stbEnabled" name="sip_stbEnabled" type="checkbox" OnClick="checkSIPform();">
                McastProxy:<input id="sip_stbMcast" name="sip_stbMcast" type="checkbox"></td>
            <td id="sip_stb_vlan">VlanID(s):<input id="sip_stbVLAN" name="sip_stbVLAN" class="wide" size="30" maxlength="60" type="text" onKeyUp="checkSIPform();" value="<% getCfgGeneral(1, "sip_portVLAN"); %>"</td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode1">Port 1 mode</td>
            <td colspan="2" id="port1_swmode"><select name="port1_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode2">Port 2 mode</td>
            <td colspan="2" id="port2_swmode"><select name="port2_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode3">Port 3 mode</td>
            <td colspan="2" id="port3_swmode"><select name="port3_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode4">Port 4 mode</td>
            <td colspan="2" id="port4_swmode"><select name="port4_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="statusPortMode5">Port 5 mode</td>
            <td colspan="2" id="port5_swmode"><select name="port5_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
          </tr>
        </table>
        <input type="submit" class="mid" id="statusApply" value="Apply">
        <input type="hidden" name="submit-url" value="/adm/status.asp">
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
