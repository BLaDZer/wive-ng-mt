<!DOCTYPE html>
<html>
<head>
<title>Internet Services Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("services");
Butterlate.setTextDomain("buttons");

var secs;
var timerID = null;
var timerRunning = false;
var rmtManagementPort = '<% getCfgZero(1, "RemoteManagementPort"); %>';
var serviceStatusTimer = null;

function StartTheTimer()
{
	if (secs==0)
	{
		TimeoutReload(10);
		window.location.href=window.location.href;	//reload page
	}
	else
	{
		self.status = secs;
		secs = secs - 1;
		timerRunning = true;
		timerID = self.setTimeout("StartTheTimer()", 1000);
	}
}

function TimeoutReload(timeout)
{
	secs = timeout;
	if (serviceStatusTimer != null)
		clearTimeout(serviceStatusTimer);
	if (timerRunning)
		clearTimeout(timerID);
	serviceStatusTimer = null;
	timerRunning = false;
	StartTheTimer();
}

function initTranslation()
{
	_TR("lTitle", "services misc title");
	_TR("lIntroduction", "services misc introduction");
	_TR("miscOffloadEngine", "services misc offload note");
	_TR("miscSoftwareFastpath", "services misc software note");
	_TR("miscImportant", "services misc important");
	_TR("miscOffloadSetup", "services misc offload setup");
	_TR("miscNATOffload", "services misc nat offload");
	_TR("miscSoftware", "services misc software");
	_TR("miscHardware", "services misc hardware");
	_TR("miscComplex", "services misc complex");
	_TR("miscWiFiOffload", "services misc wifi offload");
	_TR("miscUDPOffload", "services misc udp offload");
	_TR("miscIPV6offload", "services misc ipv6 offload");
	_TR("miscHW_NAT", "services misc hw_nat");
	_TR("miscNATimpl", "services misc nat implementation");
	_TR("miscSoftwareTitle", "services misc software title");
	_TR("miscNATFastpath", "services misc nat fastpath");
	_TR("miscRouteFastpath", "services misc route fastpath");
	_TR("miscNFFastpath", "services misc netfilter fastpath");
	_TR("miscRemoteSetup", "services misc remote setup");
	_TR("miscWebRemote", "services misc web remote");
	_TR("miscWebPort", "services misc web port");
	_TR("miscSSHRemote", "services misc ssh remote");
	_TR("miscSSHPort", "services misc ssh port");
	_TR("miscTelnetRemote", "services misc telnet remote");
	_TR("miscServices", "services misc services");
	_TR("miscValue", "services misc value");
	_TR("miscDetails", "services status details");
	_TR("miscStatus", "services misc status");
	_TR("miscConfigure", "services status configure");
	_TR("lDnsp", "services misc dnsproxy");
	_TR("lUpnp", "services misc upnp");
	_TR("lSnmp", "services misc snmp");
	_TR("lArppt", "services misc arppt");
	_TR("lCron", "services misc cron");
	_TR("lLltd", "services misc lltd");
	_TR("lLldpd", "services misc lldpd");
	_TR("lCdp", "services misc cdp");
	_TR("miscIPTV", "services misc iptv");
	_TR("miscIPTVValue", "services misc value");
	_TR("miscIPTVDetails", "services status details");
	_TR("miscIPTVStatus", "services misc status");
	_TR("miscIPTVConfig", "services status configure");
	_TR("miscDINF", "services misc dinf");
	_TR("miscDINFValue", "services misc value");
	_TR("miscDINFDetails", "services status details");
	_TR("miscDINFStatus", "services misc status");
	_TR("miscDINFConfig", "services status configure");
	_TR("lIgmpp", "services misc igmp");
	_TR("lIgmppMC", "services misc igmp multicast");
	_TR("lIgmppMC2UC", "services misc igmp unicast");
	_TR("ludpxy", "services misc udpxy");
	_TR("miscUDPXYPort", "services misc udpxy port");
	_TR("lxupnpd", "services misc xupnpd");
	_TR("miscPassThrough", "services misc pass through");
	_TR("miscPTPPPoE", "services misc pass through pppoe");
	_TR("miscPTIPv6", "services misc pass through ipv6");
	_TR("miscWatchers", "services misc watchers");
	_TR("miscWatchdog", "services misc watchdog");
	_TR("miscPinger", "services misc pinger");
	_TR("miscPingTime", "services misc ping time");
	_TR("miscPingInterval", "services misc ping interval");
	_TR("miscOthers", "services misc others");
	_TR("miscVLANDoubleTag", "services misc vlan double tag");
	_TR("miscReinitWAN", "services misc reinit wan");
	_TR("miscDHCPRenew", "services misc dhcp renew");
	_TR("sysfwPingFrmWANFilterHead", "services misc ping from wan");
	_TR("lStp", "services misc stp");
	_TR("miscDontModifyTTL", "services misc dont modify ttl");
	_TR("miscDontModifyMCTTL", "services misc multicast ttl");
	_TR("miscUsePMTU", "services misc use pmtu");

	_TRV("lApply", "button apply");
	_TRV("lCancel", "button cancel");

	var elements = document.getElementsByTagName('option');
  		for (var i = 0; i < elements.length; i++)
    		if (elements[i].id == "disable")
				elements[i].innerHTML = _("button disable");
			else if (elements[i].id == "enable")
				elements[i].innerHTML = _("button enable");
}

function initValue()
{
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var stp = <% getCfgZero(1, "stpEnabled"); %>;
	var igmp = <% getCfgZero(1, "igmpEnabled"); %>;
	var igmp_snoop = '<% getCfgGeneral(1, "igmpSnoopMode"); %>';
	var igmpM2UConv = '<% getCfgGeneral(1, "igmpM2UConvMode"); %>';
	var upnp = <% getCfgZero(1, "upnpEnabled"); %>;
	var xupnpd = <% getCfgZero(1, "xupnpd"); %>;
	var dns = <% getCfgZero(1, "dnsPEnabled"); %>;
	var wan = "<% getCfgZero(1, "wanConnectionMode"); %>";
	var cdp = "<% getCfgZero(1, "cdpEnabled"); %>";
	var lltd = "<% getCfgZero(1, "lltdEnabled"); %>";
	var lldpd = "<% getCfgZero(1, "lldpdEnabled"); %>";
	var wpf = "<% getCfgGeneral(1, "WANPingFilter"); %>";
	var arp_pt = "<% getCfgZero(1, "parproutedEnabled"); %>";
	var arpptb = "<% getARPptBuilt(); %>";
	var cdpb = "<% getCdpBuilt(); %>";
	var lltdb = "<% getLltdBuilt(); %>";
	var lldpdb = "<% getLldpdBuilt(); %>";
	var igmpb = "<% getIgmpProxyBuilt(); %>";
	var upnpb = "<% getUpnpBuilt(); %>";
	var xupnpdb = "<% getXupnpdBuilt(); %>";
	var dnsp = "<% getDnsmasqBuilt(); %>";
	var snmpdb = "<% getSNMPDBuilt(); %>";
	var krnl_pppoe = "<% getCfgZero(1, "pppoe_pass"); %>";
	var krnl_ipv6 = "<% getCfgZero(1, "ipv6_pass"); %>";
	var pinger = "<% getCfgZero(1, "pinger_check_on"); %>";
	var telnetd_built = "<% getTelnetdBuilt(); %>";
	var store_ttl = '<% getCfgGeneral(1, "store_ttl"); %>';
	var store_ttl_mcast = '<% getCfgGeneral(1, "store_ttl_mcast"); %>';
	var mss_pmtu = '<% getCfgGeneral(1, "mss_use_pmtu"); %>';
	var hw_nat_wifi_pt = '<% getCfgGeneral(1, "hw_nat_wifi"); %>';
	var hw_nat_udp_pt = '<% getCfgGeneral(1, "hw_nat_udp"); %>';
	var hw_nat_six_pt = '<% getCfgGeneral(1, "hw_nat_six"); %>';

	initTranslation();

	var form = document.miscServiceCfg;

	form.stpEnbl.options.selectedIndex = 1*stp;
	form.igmpEnbl.options.selectedIndex = (igmpb == '1') ? 1*igmp : 0;
	form.igmpSnoop.value = igmp_snoop;
	form.igmpM2UConv.value = igmpM2UConv;
	form.upnpEnbl.options.selectedIndex = 1*upnp;
	form.xupnpdEnbl.options.selectedIndex = 1*xupnpd;
	form.dnspEnbl.options.selectedIndex = 1*dns;
	form.cdpEnbl.options.selectedIndex = 1*cdp;
	form.lltdEnbl.options.selectedIndex = 1*lltd;
	form.lldpdEnbl.options.selectedIndex = 1*lldpd;
	form.krnlPppoePass.options.selectedIndex = 1*krnl_pppoe;
	form.krnlIpv6Pass.options.selectedIndex = 1*krnl_ipv6;
	form.pingWANEnbl.options.selectedIndex = (wpf == '1') ? 1 : 0;
	form.arpPT.options.selectedIndex = 1*arp_pt;
	form.hw_nat_wifiPT.options.selectedIndex = (hw_nat_wifi_pt == "1") ? 1 : 0;
	form.hw_nat_udpPT.options.selectedIndex = (hw_nat_udp_pt == "1") ? 1 : 0;
	form.hw_nat_sixPT.options.selectedIndex = (hw_nat_six_pt == "1") ? 1 : 0;
	form.pingerEnable.value = (pinger == '1') ? '1' : '0';
	form.mssPmtu.value = (mss_pmtu == '0') ? '0' : '1';

	form.natMode.value = defaultNumber("<% getCfgGeneral(1, "nat_mode"); %>", "1");
	form.rmtHTTP.value = defaultNumber("<% getCfgGeneral(1, "RemoteManagement"); %>", "1");
	form.rmtSSH.value = defaultNumber("<% getCfgGeneral(1, "RemoteSSH"); %>", "1");

	if (telnetd_built == '1')
		form.rmtTelnet.value = defaultNumber("<% getCfgGeneral(1, "RemoteTelnet"); %>", "0");
	else
		displayElement('rmt_telnetd', false);

	form.udpxyMode.value = defaultNumber("<% getCfgGeneral(1, "UDPXYMode"); %>", "0");
	form.udpxyPort.value = defaultNumber("<% getCfgGeneral(1, "UDPXYPort"); %>", "81");
	form.watchdogEnable.value = defaultNumber("<% getCfgGeneral(1, "WatchdogEnabled"); %>", "0");
	form.dhcpSwReset.value = defaultNumber("<% getCfgGeneral(1, "vlanDoubleTag"); %>", "0");
	form.vlanDoubleTag.value = defaultNumber("<% getCfgGeneral(1, "dhcpSwReset"); %>", "0");

	form.natFastpath.value = defaultNumber("<% getCfgGeneral(1, "natFastpath"); %>", "1");
	form.routeFastpath.value = defaultNumber("<% getCfgGeneral(1, "routeFastpath"); %>", "1");
	form.filterFastpath.value = defaultNumber("<% getCfgGeneral(1, "filterFastpath"); %>", "1");
	form.CrondEnable.value = defaultNumber("<% getCfgGeneral(1, "CrondEnable"); %>", "0");
	form.ForceRenewDHCP.value = defaultNumber("<% getCfgGeneral(1, "ForceRenewDHCP"); %>", "1");
	form.SnmpdEnabled.value = defaultNumber("<% getCfgGeneral(1, "snmpd"); %>", "0");
	form.ttlStore.value = (store_ttl == '1') ? '1' : '0';
	form.ttlMcastStore.value = (store_ttl_mcast == '1') ? '1' : '0';

	displayElement('parprouted', arpptb == '1');
	displayElement('cdp', cdpb == '1');
	displayElement('lltd', lltdb == '1');
	displayElement('lldpd', lldpdb == '1');
	displayElement('snmpd', snmpdb == '1');
	displayElement('igmpProxy', igmpb == '1');
	displayElement('upnp', upnpb == '1');
	displayElement('xupnpd', xupnpdb == '1');
	displayElement('dnsproxy', dnsp == '1');

	// Set-up fastpaths
	if (opmode == "4")
	{
		form.offloadMode.value = defaultNumber("0", "1");
		form.offloadMode.disabled = true;
	}
	else
		form.offloadMode.value = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");
	offloadModeSelect(form);

	igmpSelect(form);
	httpRmtSelect(form);
	sshRmtSelect(form);
	pingerSelect(form);
	udpxySelect(form);

	displayServiceStatus();
}

function CheckValue(form)
{
	var thresh = form.offloadMode.value;

	if ((thresh == '2') || (thresh == '3'))
	{
		// Check threshold
		if (!validateNum(form.hwnatThreshold.value, false))
		{
			alert(_("services misc hwnat number"));
			form.hwnatThreshold.focus();
			return false;
		}
		var thr = form.hwnatThreshold.value * 1;
		if ((thr < 0) || (thr >500))
		{
			alert(_("services misc hwnat over"));
			form.hwnatThreshold.focus();
			return false;
		}
	}

	var udpxy_port = form.udpxyPort.value * 1;
	if (!((udpxy_port == 81) || ((udpxy_port >= 1024) && (udpxy_port <= 65535))))
	{
		alert(_("services misc udpxy port"));
		form.udpxyPort.focus();
		return false;
	}

	form.rmt_http_port_changed.value = (form.RemoteManagementPort.value != rmtManagementPort) ? '1' : '0';

	// Timeout reload
	TimeoutReload(20);

	return true;
}

function offloadModeSelect(form)
{
	var thresh = form.offloadMode.value;
	var fastpathb = "<% getFastPathBuilt(); %>";

	// Mode
	displayElement('hwnat_threshold_row', (thresh == '2') || (thresh == '3'));
	// HW extensions
	displayElement('wifihw_row', (thresh == '2') || (thresh == '3'));
	displayElement('udphw_row', (thresh == '2') || (thresh == '3'));
	displayElement('sixhw_row', (thresh == '2') || (thresh == '3'));
	// SW extensions
	displayElement('fastpath_row', (thresh == '1') || (thresh == '3'));
	displayElement('miscSoftwareFastpath', (thresh == '1') || (thresh == '3'))
	displayElement('filter_fastpath_row', (thresh == '1') || (thresh == '3'));
	if (fastpathb == "1") {
	    displayElement('nat_fastpath_row', (thresh == '1') || (thresh == '3'));
	    displayElement('route_fastpath_row', (thresh == '1') || (thresh == '3'));
	}
}

function pingerSelect(form)
{
	displayElement( [ 'pinger_row1', 'pinger_row2' ] , form.pingerEnable.value == '1');
}

function igmpSelect(form)
{
	displayElement( [ 'igmpSnoop', 'mcast_store_ttl_row' ] , form.igmpEnbl.value == '1');
	displayElement( [ 'igmpM2UConv', 'mcast_store_ttl_row' ] , form.igmpEnbl.value == '1');
}

function httpRmtSelect(form)
{
	displayElement( 'http_rmt_port', form.rmtHTTP.value != '0');
}

function sshRmtSelect(form)
{
	displayElement( 'ssh_rmt_port', form.rmtSSH.value != '0');
}

function udpxySelect(form)
{
	displayElement( 'udpxy_port_row', form.udpxyMode.value != '0');
}

function displayServiceHandler(response)
{
	var form = document.miscServiceCfg;

	var services = [
		// turned_on, row_id, daemon_id, url-finish, about
		[ '<% getCfgGeneral(1, "UDPXYMode"); %>', 'udpxy', 'udpxy', '<% getCfgGeneral(1, "UDPXYPort"); %>/status/', 'udpxy.sourceforge.net' ],
		[ '<% getCfgGeneral(1, "xupnpd"); %>', 'xupnpd', 'xupnpd', '4044/', 'xupnpd.org/' ],
		[ '<% getCfgGeneral(1, "CrondEnable"); %>', 'crond', 'crond', null, 'crontab.org/' ],
		[ '<% getCfgGeneral(1, "snmpd"); %>', 'snmpd', 'snmpd', null, 'www.net-snmp.org/docs/man/snmpd.html' ],
		[ '<% getCfgGeneral(1, "igmpEnabled"); %>', 'igmpProxy', 'igmpproxy', null, 'sourceforge.net/projects/igmpproxy' ],
		[ '<% getCfgGeneral(1, "lltdEnabled"); %>', 'lltd', 'lld2d', null, 'msdn.microsoft.com/en-us/windows/hardware/gg463061.aspx' ],
		[ '<% getCfgGeneral(1, "lldpdEnabled"); %>', 'lldpd', 'lldpd', null, 'http://vincentbernat.github.io/lldpd' ],
		[ '<% getCfgGeneral(1, "upnpEnabled"); %>', 'upnp', 'miniupnpd', null, 'miniupnp.free.fr/' ],
		[ '<% getCfgGeneral(1, "cdpEnabled"); %>', 'cdp', 'cdp-send', null, 'freecode.com/projects/cdp-tools' ],
		[ '<% getCfgGeneral(1, "dnsPEnabled"); %>', 'dnsproxy', 'dnsmasq', null, 'thekelleys.org.uk/dnsmasq/doc.html' ],
		[ '<% getCfgGeneral(1, "parproutedEnabled"); %>', 'parprouted', 'parprouted', null, 'freecode.com/projects/parprouted' ],
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
			// Fill-up about
			tds[2].innerHTML = (service[4] != null) ? '<a href="http://' + service[4] + '" target="_blank">' + _("services status about") + '</a>' : "&nbsp;";

			// Fill-up status
			if (service[0]*1 == '0')
				tds[3].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
			else
				tds[3].innerHTML = (daemons[service[2]] == 1) ?
					'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
					'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';

			// Fill-up configure
			tds[4].innerHTML = ((service[0]*1 > '0') && (daemons[service[2]] == 1) && (service[3] != null)) ?
				'<a href="http://<% getLanIp(); %>:' + service[3] + '">' + _("services status configure") + '</a>' : '&nbsp;';
		}
	}

	serviceStatusTimer = setTimeout('displayServiceStatus();', 5000);
}

function displayServiceStatus()
{
	ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
}

function submitForm(form) {
	if (form.RemoteManagementPort.value != rmtManagementPort) {
		if (!ajaxPostForm(_("services misc ask reboot"), form, 'setMiscReloader', _("message reboot"), ajaxShowProgress)) {
			form.reboot.value = "0";
    		form.submit();
		}
	} else {
		form.reboot.value = "0";
		form.submit();
	}
}
</script>
</head>

<body onLoad="initValue();">
<table class="body">
  <tr>
    <td><h1 id="lTitle">Miscellaneous Services Setup</h1>
      <p id="lIntroduction"></p>
      <p id="miscOffloadEngine">In section "Offload engine" you will select the offload mode (for IPOE/PPPOE recommended HARDWARE mode, for L2TP/PPTP - COMPLEX).</p>
      <p id="miscSoftwareFastpath">In the section "Software fastpaths" (work only if offload mode complex or software) it is possible to disable selectively one of mechanisms of program offload (not recommended disable for PPTP/L2TP).</p>
      <p id="miscImportant">IMPORTANT: If you have problems with SIP or other applications by using UDP, try to disable UDP offload(some ppe revisions not correct work with udp).</p>
      <hr>
      <form method="POST" name="miscServiceCfg" action="/goform/setMiscServices" onSubmit="return CheckValue(this);">
        <table class="form">
          <!-- Offload engine -->
          <tr>
            <td class="title" colspan="5" id="miscOffloadSetup">Offload engine</td>
          </tr>
          <tr>
            <td class="head" id="miscNATOffload">NAT offload mode</td>
            <td colspan="4"><select name="offloadMode" class="half" onChange="offloadModeSelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="miscSoftware">Software</option>
                <option value="2" id="miscHardware">Hardware</option>
                <option value="3" id="miscComplex">Complex</option>
              </select></td>
          </tr>
          <tr id="wifihw_row">
            <td class="head" id="miscWiFiOffload">WiFi hardware nat offload</td>
            <td colspan="4"><select name="hw_nat_wifiPT" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="udphw_row">
            <td class="head" id="miscUDPOffload">UDP hardware nat offload</td>
            <td colspan="4"><select name="hw_nat_udpPT" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="sixhw_row">
            <td class="head" id="miscIPV6offload">IPV6 hardware route offload</td>
            <td colspan="4"><select name="hw_nat_sixPT" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="hwnat_threshold_row" style="display: none;">
            <td class="head" id="miscHW_NAT">HW_NAT binding threshold</td>
            <td colspan="4"><input name="hwnatThreshold" value="<% getCfgZero(1, "hw_nat_bind"); %>" class="half">
              &nbsp; <span style="color: #c0c0c0;">(0-500)</span></td>
          </tr>
          <tr>
            <td class="head" id="miscNATimpl">NAT implementation</td>
            <td colspan="4"><select name="natMode" class="half">
                <option value="0">Linux</option>
                <option value="1" selected>Fcone</option>
                <option value="2">Rcone</option>
              </select></td>
          </tr>
          <!-- Software fastpaths -->
          <tr id="fastpath_row">
            <td class="title" colspan="5" id="miscSoftwareTitle">Software fastpaths</td>
          </tr>
          <tr id="nat_fastpath_row" style="display: none;">
            <td class="head" id="miscNATFastpath">NAT fastpath</td>
            <td colspan="4"><select name="natFastpath" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="route_fastpath_row" style="display: none;">
            <td class="head" id="miscRouteFastpath">Route fastpath</td>
            <td colspan="4"><select name="routeFastpath" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="filter_fastpath_row" style="display: none;">
            <td class="head" id="miscNFFastpath">Netfilter fastpath</td>
            <td colspan="4"><select name="filterFastpath" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <!-- Remote management -->
          <tr>
            <td class="title" colspan="5" id="miscRemoteSetup">Remote management</td>
          </tr>
          <tr>
            <td class="head" id="miscWebRemote">HTTP Remote Management</td>
            <td colspan="4"><select name="rmtHTTP" class="half" onChange="httpRmtSelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1">LAN</option>
                <option value="2">LAN &amp; WAN</option>
              </select></td>
          </tr>
          <tr id="http_rmt_port" style="display: none;">
            <td class="head" id="miscWebPort">Remote HTTP port</td>
            <td colspan="4"><input class="half" name="RemoteManagementPort" value="<% getCfgZero(1, "RemoteManagementPort"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="miscSSHRemote">SSH Remote Management</td>
            <td colspan="4"><select name="rmtSSH" class="half" onchange="sshRmtSelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1">LAN</option>
                <option value="2">LAN &amp; WAN</option>
              </select></td>
          </tr>
          <tr id="ssh_rmt_port" style="display: none;">
            <td class="head" id="miscSSHPort">Remote SSH port</td>
            <td colspan="4"><input class="half" name="RemoteSSHPort" value="<% getCfgZero(1, "RemoteSSHPort"); %>"></td>
          </tr>
          <tr id="rmt_telnetd">
            <td class="head" id="miscTelnetRemote">Remote Telnet</td>
            <td colspan="4"><select name="rmtTelnet" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <!-- Services -->
          <tr>
            <td class="title" id="miscServices">Services</td>
            <td class="title" id="miscValue">Value</td>
            <td class="title" id="miscDetails" style="width: 88px;">Details</td>
            <td class="title" id="miscStatus" style="width: 56px;">Status</td>
            <td class="title" id="miscConfigure" style="width: 80px;">Configure</td>
          </tr>
          <tr id="dnsproxy">
            <td class="head" id="lDnsp">DNS cached proxy</td>
            <td><select name="dnspEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="upnp">
            <td class="head" id="lUpnp">UPNP support</td>
            <td><select name="upnpEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="parprouted">
            <td class="head" id="lArppt">ARP Proxy</td>
            <td><select name="arpPT" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="crond">
            <td class="head" id="lCron">Cron daemon</td>
            <td><select name="CrondEnable" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <!-- IPTV -->
          <tr>
            <td class="title" id="miscIPTV">Services IPTV</td>
            <td class="title" id="miscIPTVValue">Value</td>
            <td class="title" id="miscIPTVDetails" style="width: 88px;">Details</td>
            <td class="title" id="miscIPTVStatus" style="width: 56px;">Status</td>
            <td class="title" id="miscIPTVConfig" style="width: 80px;">Configure</td>
          </tr>
          <tr id="igmpProxy">
            <td class="head" id="lIgmpp">IGMP proxy</td>
            <td><select name="igmpEnbl" class="half" onChange="igmpSelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="igmpSnoop">
            <td class="head" id="lIgmppMC">Multicast IGMP snooping</td>
            <td colspan="4"><select name="igmpSnoop" class="half">
                <option value="">Auto</option>
                <option value="n" id="disable">Disable</option>
              </select></td>
          </tr>
          <tr id="igmpM2UConv">
            <td class="head" id="lIgmppMC2UC">Multicast to Unicast conversion</td>
            <td colspan="4"><select name="igmpM2UConv" class="half">
                <option value="wlan">Wlan</option>
                <option value="lan">Lan</option>
                <option value="all">All</option>
                <option value="" id="disable">Disable</option>
              </select></td>
          </tr>
          <tr id="udpxy">
            <td class="head" id="ludpxy">Multicast to http proxy (udpxy)</td>
            <td><select name="udpxyMode" class="half" onChange="udpxySelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1">LAN</option>
                <option value="2">LAN &amp; WAN</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="udpxy_port_row" style="display:none;">
            <td class="head" id="miscUDPXYPort">UDPXY port</td>
            <td colspan="4"><input name="udpxyPort" class="half"></td>
          </tr>
          <tr id="xupnpd">
            <td class="head" id="lxupnpd">UPNP media server (xupnpd)</td>
            <td><select name="xupnpdEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <!-- INFO -->
          <tr>
            <td class="title" id="miscDINF">Device info services</td>
            <td class="title" id="miscDINFValue">Value</td>
            <td class="title" id="miscDINFDetails" style="width: 88px;">Details</td>
            <td class="title" id="miscDINFStatus" style="width: 56px;">Status</td>
            <td class="title" id="miscDINFConfig" style="width: 80px;">Configure</td>
          </tr>
          <tr id="snmpd">
            <td class="head" id="lSnmp">SNMP daemon</td>
            <td><select name="SnmpdEnabled" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="lltd">
            <td class="head" id="lLltd">LLTD daemon</td>
            <td><select name="lltdEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="lldpd">
            <td class="head" id="lLldpd">LLDP daemon</td>
            <td><select name="lldpdEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <tr id="cdp">
            <td class="head" id="lCdp">CDP daemon</td>
            <td><select name="cdpEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
            <td>&nbsp;</td>
          </tr>
          <!-- Pass Through -->
          <tr>
            <td class="title" colspan="5" id="miscPassThrough">Pass Through</td>
          </tr>
          <tr>
            <td class="head" id="miscPTPPPoE">PPPOE pass through</td>
            <td colspan="4"><select name="krnlPppoePass" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="miscPTIPv6">IPv6 pass through</td>
            <td colspan="4"><select name="krnlIpv6Pass" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <!-- Watchers -->
          <tr>
            <td class="title" colspan="5" id="miscWatchers">Watchers</td>
          </tr>
          <tr>
            <td class="head" id="miscWatchdog">Watchdog service</td>
            <td colspan="4"><select name="watchdogEnable" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="miscPinger">Pinger service</td>
            <td colspan="4"><select name="pingerEnable" class="half" onChange="pingerSelect(this.form);">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="pinger_row1">
            <td class="head" id="miscPingTime">Ping check time</td>
            <td colspan="4"><input name="ping_check_time" class="half" value="<% getCfgGeneral(1, "ping_check_time"); %>"></td>
          </tr>
          <tr id="pinger_row2">
            <td class="head" id="miscPingInterval">Ping check interval</td>
            <td colspan="4"><input name="ping_check_interval" class="half" value="<% getCfgGeneral(1, "ping_check_interval"); %>"></td>
          </tr>
          <!-- Others -->
          <tr>
            <td class="title" colspan="5" id="miscOthers">Others</td>
          </tr>
          <tr>
            <td class="head" id="miscVLANDoubleTag">Vlan double tag QinQ support</td>
            <td colspan="4"><select name="vlanDoubleTag" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="miscReinitWAN">Reinit WAN if DHCP lease fail</td>
            <td colspan="4"><select name="dhcpSwReset" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="miscDHCPRenew">DHCP renew lease at WAN UP</td>
            <td colspan="4"><select name="ForceRenewDHCP" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="sysfwPingFrmWANFilterHead">Accept ping from WAN</td>
            <td colspan="4"><select name="pingWANEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="lStp">802.1d Spanning Tree</td>
            <td colspan="4"><select name="stpEnbl" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="miscDontModifyTTL">Do not modify TTL</td>
            <td colspan="4"><select name="ttlStore" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr id="mcast_store_ttl_row">
            <td class="head" id="miscDontModifyMCTTL">Do not modify multicast TTL</td>
            <td colspan="4"><select name="ttlMcastStore" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="miscUsePMTU">Use PMTU discovery</td>
            <td colspan="4"><select name="mssPmtu" class="half">
                <option value="0" id="disable">Disable</option>
                <option value="1" id="enable">Enable</option>
              </select></td>
          </tr>
          <tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="button" class="normal" value="Apply"  id="lApply" onClick="submitForm(this.form);">
              &nbsp;
              <input type="reset"  class="normal" value="Cancel" id="lCancel" onClick="window.location.reload()">
              <input type="hidden" value="/services/misc.asp" name="submit-url">
              <input type="hidden" value="0" name="rmt_http_port_changed">
              <input type="hidden" value="1" name="reboot">
              <iframe id="setMiscReloader" name="setMiscReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
