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
<style type="text/css">
td.port_status {
	background-position: center center;
	background-repeat: no-repeat;
	width: 35px;
	height: 24px;
	text-align: center;
	vertical-align: middle;
	cursor: default;
}
</style>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");

var wan_port = '<% getCfgZero(1, "wan_port"); %>';
var timer = null;

function showOpMode()
{
	var el = document.getElementById('inpOperationMode');
	if (el == null)
		return;

	var opmode = el.value;
	var s_opmode = 'Unknown';
	if (opmode == '0')
		s_opmode = 'Bridge Mode';
	else if (opmode == '1')
		s_opmode = 'Gateway Mode';
	else if (opmode == '2')
		s_opmode = 'Ethernet Converter Mode';
	else if (opmode == '3')
		s_opmode = 'AP Client Mode';

	ajaxModifyElementHTML('tdOperationMode', s_opmode);
}

function showPortStatus()
{
	var el = document.getElementById('inpWanPort');
	if (el == null)
		return;

	var pstatus = el.value.split(';');

	if (pstatus.length <= 0)
	{
		ajaxModifyElementHTML('portStatusRow', '<td>not supported</td>');
		return;
	}

	var form = document.setWanForm;
	var wan = 1 * form.wan_port.value;
	var stb_port = (!form.tv_stbEnabled.checked) ? -1 :
			(wan == 0) ? 1 : wan - 1;
	var sip_port = (!form.sip_stbEnabled.checked) ? -1 :
			(wan == 0) ? 2 : wan - 2;
	var content = '';

	for (i=0; i<pstatus.length; i++)
	{
		var port = pstatus[i].split(',');
		var image = 'empty';

		if (port[0] == '1')
		{
			if (port[1] == '10')
				image = '10';
			else if (port[1] == '100')
				image = '100';
			else if (port[1] == '1000')
				image = '1000';
			else
				image = '100';

			if (port[2] == 'H')
				image += '_h';
		}

		var text = i+1;
		if (i == wan)
			text = '<span style="color: #027fff;">WAN</span>';
		else if (i == stb_port)
			text = '<span style="color: #ff00d2;">TV</span>';
		else if (i == sip_port)
			text = '<span style="color: #ffd200;">SIP</span>';

		content = content + '<td class="port_status" style="background-color: #ffffff; color: #00ffff; border: 0px; background-image: url(\'/graphics/' + image + '.gif\'); "><b>' + text + '</b></td>';
	}

	ajaxModifyElementHTML('portStatusRow', '<table class="small" style="border: 0px;">' + content + '</table>');
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

	_TR("statusEthPortStatus", "status ethernet port status");
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

	var form = document.setWanForm;

	form.wan_port.value = wan_port;
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
		initTranslation();
		showOpMode();
		showPortStatus();
		timer = self.setTimeout('reloadPage();', 3000);
	}

	ajaxLoadElement("sysinfoTable", "/adm/sysinfo.asp", reloader);
}

function setWanPort(form)
{
	if (confirm('The device must be restarted to apply port configuration changes. Do you want to proceed?'))
	{
		clearTimeout(timer);
		ajaxPostForm(null, form, 'setwanReloader', '/messages/rebooting.asp');
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
            <td class="title" colspan="3">Port Management</td>
          </tr>
          <tr>
            <td class="head">WAN port</td>
            <td><select name="wan_port" onChange="showPortStatus();" class="short">
                <option value="0">1</option>
                <option value="4">5</option>
              </select>
              <iframe id="setwanReloader" name="setwanReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe></td>
              <td></td>
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
            <td class="head">Port 1 mode</td>
            <td><select name="port1_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
              <td></td>
          </tr>
          <tr>
            <td class="head">Port 2 mode</td>
            <td><select name="port2_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
              <td></td>
          </tr>
          <tr>
            <td class="head">Port 3 mode</td>
            <td><select name="port3_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
              <td></td>
          </tr>
          <tr>
            <td class="head">Port 4 mode</td>
            <td><select name="port4_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
              <td></td>
          </tr>
          <tr>
            <td class="head">Port 5 mode</td>
            <td><select name="port5_swmode" class="mid">
                <option value="auto">auto</option>
                <option value="10h">10 mbit/s half duplex</option>
                <option value="10f">10 mbit/s full duplex</option>
                <option value="100h">100 mbit/s half duplex</option>
                <option value="100f">100 mbit/s full duplex</option>
              </select></td>
              <td></td>
          </tr>
        </table>
        <input type="button" class="mid" value="Change port configuration" onClick="setWanPort(this.form);" />
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
