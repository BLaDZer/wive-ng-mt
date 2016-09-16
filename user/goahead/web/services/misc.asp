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
		<script src="/lang/b28n.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script src="/js/ajax.js"></script>
		<script>
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			var statusOffloadEngineMenu		= 1;
			var statusSoftwareFastpathMenu	= 1;
			var statusRemoteManagementMenu	= 0;
			var statusServicesMenu			= 0;
			var statusServicesIPTVMenu		= 0;
			var statusServicesInfoMenu		= 1;
			var statusPassThroughMenu		= 1;
			var statusWatchersMenu			= 1;
			var statusOthersMenu			= 1;
			
			function initTranslation()
			{
				_TR("miscTitle",					"services misc title");
				_TR("miscIntroduction",				"services misc introduction");
				_TR("miscOffloadEngine",			"services misc offload note");
				_TR("miscSoftwareFastpath",			"services misc software note");
				_TR("miscImportant",				"services misc important");
				_TR("miscNATOffload",				"services misc nat offload");
				_TR("miscSoftware",					"services misc software");
				_TR("miscHardware",					"services misc hardware");
				_TR("miscComplex",					"services misc complex");
				_TR("miscWiFiOffload",				"services misc wifi offload");
				_TR("miscUDPOffload",				"services misc udp offload");
				_TR("miscIPV6offload",				"services misc ipv6 offload");
				_TR("miscHW_NAT",					"services misc hw_nat");
				_TR("miscNATimpl",					"services misc nat implementation");
				_TR("miscNATFastpath",				"services misc nat fastpath");
				_TR("miscRouteFastpath",			"services misc route fastpath");
				_TR("miscNFFastpath",				"services misc netfilter fastpath");
				_TR("miscSmbFastpath",				"services misc samba fastpath");
				_TR("miscWebRemote",				"services misc web remote");
				_TR("miscWebPort",					"services misc web port");
				_TR("miscSSHRemote",				"services misc ssh remote");
				_TR("miscSSHPort",					"services misc ssh port");
				_TR("miscTelnetRemote",				"services misc telnet remote");
				_TR("miscDnsp",						"services misc dnsproxy");
				_TR("miscUpnp",						"services misc upnp");
				_TR("miscSnmp",						"services misc snmp");
				_TR("miscSnmpCommunity",			"services misc snmp community");
				_TR("miscArppt",					"services misc arppt");
				_TR("miscCron",						"services misc cron");
				_TR("miscLltd",						"services misc lltd");
				_TR("miscLldpd",					"services misc lldpd");
				_TR("miscCdp",						"services misc cdp");
				_TR("miscIgmpp",					"services misc igmp");
				_TR("miscIgmppMC",					"services misc igmp multicast");
				_TR("miscIgmppFL",					"services misc igmp fastleave");
				_TR("miscIgmppMC2UC",				"services misc igmp unicast");
				_TR("miscIgmppMC2UCall",			"services misc igmp unicast all");
				_TR("miscUdpxy",					"services misc udpxy");
				_TR("miscUDPXYPort",				"services misc udpxy port");
				_TR("miscXupnpd",					"services misc xupnpd");
				_TR("miscPTPPPoE",					"services misc pass through pppoe");
				_TR("miscPTIPv6",					"services misc pass through ipv6");
				_TR("miscWatchdog",					"services misc watchdog");
				_TR("miscPinger",					"services misc pinger");
				_TR("miscPingTime",					"services misc ping time");
				_TR("miscPingInterval",				"services misc ping interval");
				_TR("miscVLANDoubleTag",			"services misc vlan double tag");
				_TR("miscReinitWAN",				"services misc reinit wan");
				_TR("miscDHCPRenew",				"services misc dhcp renew");
				_TR("miscSysfwPingFrmWANFilterHead","services misc ping from wan");
				_TR("miscStp",						"services misc stp");
				_TR("miscDontModifyTTL",			"services misc dont modify ttl");
				_TR("miscDontModifyMCTTL",			"services misc multicast ttl");
				_TR("miscUsePMTU",					"services misc use pmtu");
				_TR("miscIRQBalance",				"services misc irqbalance");
				_TR("miscIGMPAuto",					"services misc auto");
				_TR("miscIGMPHW",					"services misc hw");
				_TR("miscAuto",						"services misc auto");
				_TR("miscRouter",					"services misc router");
				_TR("miscStorage",					"services misc storage");

				_TRV("miscApply",					"button apply");
				_TRV("miscCancel",					"button cancel");
				_TRV("miscReset",					"button reset");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					if (elements[i].id == "disable")
						elements[i].innerHTML = _("button disable");
					else if (elements[i].id == "enable")
						elements[i].innerHTML = _("button enable");
			}

			function initValues()
			{
				var opmode									= '<% getCfgZero(1, "OperationMode"); %>';
				var apcli_bridge							= '<% getCfgGeneral(1, "ApCliBridgeOnly"); %>';
				var igmpb									= '<% getIgmpProxyBuilt(); %>';
				var form 									= document.miscServiceCfg;

				form.RemoteManagementPort.value				= '<% getCfgZero(1, "RemoteManagementPort"); %>';
				form.RemoteSSHPort.value					= '<% getCfgZero(1, "RemoteSSHPort"); %>';
				form.cwmpACSUrl.value						= '<% getCfgGeneral(1, "cwmp_acs_url"); %>';
				form.hwnatThreshold.value					= '<% getCfgZero(1, "hw_nat_bind"); %>';
				form.stpEnbl.options.selectedIndex			= '<% getCfgZero(1, "stpEnabled"); %>';
				form.igmpEnbl.options.selectedIndex			= (igmpb == '1') ? '<% getCfgZero(1, "igmpEnabled"); %>' : 0;
				form.igmpSnoop.value						= '<% getCfgGeneral(1, "igmpSnoopMode"); %>';
				form.igmpFastL.value						= '<% getCfgGeneral(1, "igmpFastLeave"); %>';
				form.igmpM2UConv.value						= '<% getCfgGeneral(1, "igmpM2UConvMode"); %>';
				form.upnpEnbl.options.selectedIndex			= '<% getCfgZero(1, "upnpEnabled"); %>';
				form.xupnpdEnbl.options.selectedIndex		= '<% getCfgZero(1, "xupnpd"); %>';
				form.dnspEnbl.options.selectedIndex			= '<% getCfgZero(1, "dnsPEnabled"); %>';
				form.cdpEnbl.options.selectedIndex			= '<% getCfgZero(1, "cdpEnabled"); %>';
				form.lltdEnbl.options.selectedIndex			= '<% getCfgZero(1, "lltdEnabled"); %>';
				form.lldpdEnbl.options.selectedIndex		= '<% getCfgZero(1, "lldpdEnabled"); %>';
				form.cwmpdEnbl.options.selectedIndex		= '<% getCfgZero(1, "cwmpdEnabled"); %>';
				form.krnlPppoePass.options.selectedIndex	= '<% getCfgZero(1, "pppoe_pass"); %>';
				form.krnlIpv6Pass.options.selectedIndex		= '<% getCfgZero(1, "ipv6_pass"); %>';
				form.pingWANEnbl.options.selectedIndex		= ('<% getCfgGeneral(1, "WANPingFilter"); %>' == '1') ? 1 : 0;
				form.arpPT.options.selectedIndex			= '<% getCfgZero(1, "parproutedEnabled"); %>';
				form.hw_nat_wifiPT.options.selectedIndex	= ('<% getCfgGeneral(1, "hw_nat_wifi"); %>' == '1') ? 1 : 0;
				form.hw_nat_udpPT.options.selectedIndex		= ('<% getCfgGeneral(1, "hw_nat_udp"); %>' == '1') ? 1 : 0;
				form.hw_nat_sixPT.options.selectedIndex		= ('<% getCfgGeneral(1, "hw_nat_six"); %>' == '1') ? 1 : 0;
				form.pingerEnable.value						= ('<% getCfgZero(1, "pinger_check_on"); %>' == '1') ? '1' : '0';
				form.mssPmtu.value							= ('<% getCfgGeneral(1, "mss_use_pmtu"); %>' == '0') ? '0' : '1';
				form.natMode.value							= defaultNumber('<% getCfgGeneral(1, "nat_mode"); %>', '1');
				form.rmtHTTP.value							= defaultNumber('<% getCfgGeneral(1, "RemoteManagement"); %>', '1');
				form.rmtSSH.value							= defaultNumber('<% getCfgGeneral(1, "RemoteSSH"); %>', '1');
				form.rmtTelnet.value						= defaultNumber('<% getCfgGeneral(1, "RemoteTelnet"); %>', '0');
				form.udpxyMode.value						= defaultNumber('<% getCfgGeneral(1, "UDPXYMode"); %>', '0');
				form.udpxyPort.value						= defaultNumber('<% getCfgGeneral(1, "UDPXYPort"); %>', '81');
				form.watchdogEnable.value					= defaultNumber('<% getCfgGeneral(1, "WatchdogEnabled"); %>', '0');
				form.dhcpSwReset.value						= defaultNumber('<% getCfgGeneral(1, "vlanDoubleTag"); %>', '0');
				form.vlanDoubleTag.value					= defaultNumber('<% getCfgGeneral(1, "dhcpSwReset"); %>', '0');
				form.natFastpath.value						= defaultNumber('<% getCfgGeneral(1, "natFastpath"); %>', '1');
				form.routeFastpath.value					= defaultNumber('<% getCfgGeneral(1, "routeFastpath"); %>', '1');
				form.filterFastpath.value					= defaultNumber('<% getCfgGeneral(1, "filterFastpath"); %>', '1');
				form.CrondEnable.value						= defaultNumber('<% getCfgGeneral(1, "CrondEnable"); %>', '0');
				form.ForceRenewDHCP.value					= defaultNumber('<% getCfgGeneral(1, "ForceRenewDHCP"); %>', '1');
				form.SnmpdEnabled.value						= defaultNumber('<% getCfgGeneral(1, "snmpd"); %>', '0');
				form.ttlStore.value							= ('<% getCfgGeneral(1, "store_ttl"); %>' == '1') ? '1' : '0';
				form.ttlMcastStore.value					= ('<% getCfgGeneral(1, "store_ttl_mcast"); %>' == '1') ? '1' : '0';
				form.offloadMode.value						= defaultNumber('<% getCfgGeneral(1, "offloadMode"); %>', '1');
				form.IRQBalance.value						= '<% getCfgGeneral(1, "IRQBalance"); %>';
				form.ping_check_time.value					= '<% getCfgGeneral(1, "ping_check_time"); %>';
				form.ping_check_interval.value				= '<% getCfgGeneral(1, "ping_check_interval"); %>';
				form.smbFastpath.options.selectedIndex		= ('<% getCfgGeneral(1, "smbFastpath"); %>' == '1') ? 1 : 0;

				displayElement('rmt_telnetd',				'<% getTelnetdBuilt(); %>' == '1');
				displayElement('parprouted',				'<% getARPptBuilt(); %>' == '1');
				displayElement('cdp', 						'<% getCdpBuilt(); %>' == '1');
				displayElement('lltd',						'<% getLltdBuilt(); %>' == '1');
				displayElement('lldpd',						'<% getLldpdBuilt(); %>' == '1');
				displayElement('snmpd',						'<% getSNMPDBuilt(); %>' == '1');
				displayElement('igmpProxy',					(igmpb == '1') && (opmode != '0') && (apcli_bridge != '1'));
				displayElement('igmpFastL',					((igmpb == '1') && (opmode != '0') && (apcli_bridge != '1')) || (opmode != '2'));
				displayElement('igmpM2UConv',				opmode != '2');
				displayElement('upnp',						'<% getUpnpBuilt(); %>' == '1');
				displayElement('xupnpd',					'<% getXupnpdBuilt(); %>' == '1');
				displayElement('dnsproxy',					'<% getDnsmasqBuilt(); %>' == '1');
				displayElement('irqbalance',				'<% isSMP(); %>' == '1');
				displayElement('smb_fastpath_row', 			'<% getSmbFPBuilt(); %>' == '1');

				offloadModeSelect(form);
				httpRmtSelect(form);
				sshRmtSelect(form);
				cwmpRmtSelect(form);
				igmpSelect(form);
				udpxySelect(form);
				snmpdRmtSelect(form);
				pingerSelect(form);

				displayServiceStatus();
				
				showWarning();
				initTranslation();

				showOffloadEngineMenu();
				showSoftwareFastpathMenu();
				showRemoteManagementMenu();
				showServicesMenu();
				showServicesIPTVMenu();
				showServicesInfoMenu();
				showPassThroughMenu();
				showWatchersMenu();
				showOthersMenu();
			}

			function CheckValues(form)
			{
				var rmtManagementPort	= '<% getCfgZero(1, "RemoteManagementPort"); %>';

				// NAT Threshold 
				if (form.offloadMode.value >= 2)
					if (!validateNum(form.hwnatThreshold.value, false) || form.hwnatThreshold.value < 0 || form.hwnatThreshold.value > 500) {
						window.scrollTo(0, 0);
						alert(_("services misc hwnat over"));
						form.hwnatThreshold.select();
						form.hwnatThreshold.focus();
						return false;
					}
				// HTTP Remote management
				if (form.rmtHTTP.value == 1)
					if (!validateNum(form.RemoteManagementPort.value, false) || form.RemoteManagementPort.value < 0 || form.RemoteManagementPort.value > 65535) {
						window.scrollTo(0, 0);
						alert(_("services misc http over"));
						form.RemoteManagementPort.select();
						form.RemoteManagementPort.focus();
						return false;
					}
				// SSH Remote management
				if (form.rmtSSH.value == 1)
					if (!validateNum(form.RemoteSSHPort.value, false) || form.RemoteSSHPort.value < 0 || form.RemoteSSHPort.value > 65535) {
						window.scrollTo(0, 0);
						alert(_("services misc ssh over"));
						form.RemoteSSHPort.select();
						form.RemoteSSHPort.focus();
						return false;
					}
					
				if (form.RemoteManagementPort.value == rmtManagementPort) {
					form.goaheadrestart.value = 0;
					ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				}
				else
					ajaxPostForm(null, form, 'timerReloader', _('message apply'), ajaxShowProgress)

				return true;
			}

			function offloadModeSelect(form)
			{
				var thresh		= document.miscServiceCfg.offloadMode.value;
				var fastpathb	= '<% getFastPathBuilt(); %>';

				// Mode
				displayElement('hwnat_threshold_row',		thresh == '2' || thresh == '3');
				// HW extensions
				displayElement('wifihw_row',				thresh == '2' || thresh == '3');
				displayElement('udphw_row',					thresh == '2' || thresh == '3');
				displayElement('sixhw_row',					thresh == '2' || thresh == '3');
				// SW extensions
				displayElement('fastpath_row',				thresh == '1' || thresh == '3');
				displayElement('miscSoftwareFastpath',		thresh == '1' || thresh == '3')
				displayElement('filter_fastpath_row',		thresh == '1' || thresh == '3');
				if (fastpathb == "1") {
					displayElement('nat_fastpath_row',		thresh == '1' || thresh == '3');
					displayElement('route_fastpath_row',	thresh == '1' || thresh == '3');
				}
			}

			function pingerSelect(form)
			{
				displayElement([ 'pinger_row1', 'pinger_row2' ] , form.pingerEnable.value == '1');
			}

			function igmpSelect(form)
			{
				displayElement('mcast_store_ttl_row', form.igmpEnbl.value == '1');
			}

			function httpRmtSelect(form)
			{
				displayElement('http_rmt_port', form.rmtHTTP.value != '0');
			}

			function sshRmtSelect(form)
			{
				displayElement('ssh_rmt_port', form.rmtSSH.value != '0');
			}

			function cwmpRmtSelect(form)
			{
				displayElement('cwmp_acs_url', form.cwmpdEnbl.value != '0');
			}

			function udpxySelect(form)
			{
				displayElement('udpxy_port_row', form.udpxyMode.value != '0');
			}

			function snmpdRmtSelect(form)
			{
				displayElement('snmpCommunityRow', form.SnmpdEnabled.value != '0');
			}

			function displayServiceStatus()
			{
				ajaxPerformRequest('/services/misc-stat.asp', displayServiceHandler);
			}

			function displayServiceHandler(response)
			{
				var form = document.miscServiceCfg;
				var services = [
					// turned_on,										row_id,			daemon_id,		url-finish,	about
					[ '<% getCfgGeneral(1, "UDPXYMode"); %>',			'udpxy',		'udpxy', 		'<% getCfgGeneral(1, "UDPXYPort"); %>/status/', 'udpxy.sourceforge.net' ],
					[ '<% getCfgGeneral(1, "xupnpd"); %>',				'xupnpd',		'xupnpd',		'4044/',	'xupnpd.org/' ],
					[ '<% getCfgGeneral(1, "CrondEnable"); %>',			'crond',		'crond', 		null,		'crontab.org/' ],
					[ '<% getCfgGeneral(1, "snmpd"); %>',				'snmpd',		'snmpd', 		null,		'www.net-snmp.org/docs/man/snmpd.html' ],
					[ '<% getCfgGeneral(1, "igmpEnabled"); %>',			'igmpProxy',	'igmpproxy', 	null,		'sourceforge.net/projects/igmpproxy' ],
					[ '<% getCfgGeneral(1, "lltdEnabled"); %>',			'lltd',			'lld2d', 		null,		'msdn.microsoft.com/en-us/windows/hardware/gg463061.aspx' ],
					[ '<% getCfgGeneral(1, "cwmpdEnabled"); %>',		'cwmpd',		'cwmpd', 		null,		'www.broadband-forum.org/technical/trlist.php' ],
					[ '<% getCfgGeneral(1, "lldpdEnabled"); %>',		'lldpd',		'lldpd', 		null,		'vincentbernat.github.io/lldpd' ],
					[ '<% getCfgGeneral(1, "upnpEnabled"); %>',			'upnp',			'miniupnpd',	null,		'miniupnp.free.fr/' ],
					[ '<% getCfgGeneral(1, "cdpEnabled"); %>',			'cdp',			'cdp-send',		null,		'freecode.com/projects/cdp-tools' ],
					[ '<% getCfgGeneral(1, "dnsPEnabled"); %>',			'dnsproxy',		'dnsmasq',		null,		'thekelleys.org.uk/dnsmasq/doc.html' ],
					[ '<% getCfgGeneral(1, "parproutedEnabled"); %>',	'parprouted',	'parprouted',	null,		'freecode.com/projects/parprouted' ]
				];

				// Create associative array
				var tmp = response.split(',');
				var daemons = [];
				for (var i = 0; i < tmp.length; i++)
					daemons[tmp[i]] = 1;

				// Now display all services
				for (var i = 0; i < services.length; i++)
				{
					var service = services[i];
					var row		= document.getElementById(service[1]);
					var tds		= [];
					for (var j = 0; j < row.childNodes.length; j++)
						if (row.childNodes[j].nodeName == 'TD')
							tds.push(row.childNodes[j]);

					if (row != null)
					{
						// Fill-up about
						tds[2].innerHTML = (service[4] != null) ? '<a href="http://' + service[4] + '" target="_blank">' + _("services status about") + '</a>' : "&nbsp;";

						// Fill-up status
						if (service[0] == '0')
							tds[3].innerHTML = '<span style="color: #808080"><b>' + _("services status off") + '</b></span>';
						else
							tds[3].innerHTML = (daemons[service[2]] == 1) ?
								'<span style="color: #3da42c"><b>' + _("services status work") + '</b></span>' :
								'<span style="color: #808000"><b>' + _("services status starting") + '</b></span>';

						// Fill-up configure
						tds[4].innerHTML = ((service[0] > '0') && (daemons[service[2]] == 1) && (service[3] != null)) ?
							'<a href="http://<% getLanIp(); %>:' + service[3] + '">' + _("services status configure") + '</a>' : '&nbsp;';
					}
				}

				serviceStatusTimer = setTimeout('displayServiceStatus();', 5000);
			}

			function showOffloadEngineMenu() {
				var elements = [ 'miscNATOffload_row', 'wifihw_row', 'udphw_row', 'sixhw_row', 'hwnat_threshold_row', 'miscNATimpl_row' ];
				if (statusOffloadEngineMenu == 0) {
					ajaxModifyElementHTML('miscOffloadSetup', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc offload setup"));
					statusOffloadEngineMenu = 1;
					displayElement(elements, true);

					var thresh		= document.miscServiceCfg.offloadMode.value;
					displayElement( [ 'hwnat_threshold_row',
									  'wifihw_row',
									  'udphw_row',
									  'sixhw_row' ],			thresh == '2' || thresh == '3');
					displayElement( [ 'fastpath_row', 
									  'miscSoftwareFastpath' ],	thresh == '1' || thresh == '3');
				} else {
					ajaxModifyElementHTML('miscOffloadSetup', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc offload setup"));
					statusOffloadEngineMenu = 0;
					displayElement(elements, false);
				}
			}

			function showSoftwareFastpathMenu() {
				var elements = [ 'nat_fastpath_row', 'route_fastpath_row', 'filter_fastpath_row', 'smb_fastpath_row' ];
				if (statusSoftwareFastpathMenu == 0) {
					ajaxModifyElementHTML('miscSoftwareTitle', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc software title"));
					statusSoftwareFastpathMenu = 1;
					displayElement(elements, true);
					displayElement('smb_fastpath_row',	'<% getSmbFPBuilt(); %>' == '1');
				} else {
					ajaxModifyElementHTML('miscSoftwareTitle', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc software title"));
					statusSoftwareFastpathMenu = 0;
					displayElement(elements, false);
				}
			}

			function showRemoteManagementMenu() {
				var elements = [ 'miscWebRemote_row', 'http_rmt_port', 'miscSSHRemote_row', 'ssh_rmt_port', 'cwmpd', 'cwmp_acs_url', 'rmt_telnetd' ];
				if (statusRemoteManagementMenu == 0) {
					ajaxModifyElementHTML('miscRemoteSetup', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc remote setup"));
					statusRemoteManagementMenu = 1;
					displayElement(elements, true);
					displayElement('rmt_telnetd', '<% getTelnetdBuilt(); %>' == '1');
					httpRmtSelect(document.miscServiceCfg);
					sshRmtSelect(document.miscServiceCfg);
					cwmpRmtSelect(document.miscServiceCfg);
				} else {
					ajaxModifyElementHTML('miscRemoteSetup', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc remote setup"));
					statusRemoteManagementMenu = 0;
					displayElement(elements, false);
				}
			}

			function showServicesMenu() {
				var elements = [ 'dnsproxy', 'upnp', 'parprouted', 'crond' ];
				var html;
				if (statusServicesMenu == 0) {
					html =  '<td class="title" onClick="showServicesMenu();"><img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc services") + '</td>';
					html += '<td class="title" onClick="showServicesMenu();">' + _("services misc value") + '</td>';
					html +=	'<td class="title" onClick="showServicesMenu();" style="width: 88px;">' + _("services status details") + '</td>';
					html +=	'<td class="title" onClick="showServicesMenu();" style="width: 56px;">' + _("services misc status") + '</td>';
					html +=	'<td class="title" onClick="showServicesMenu();" style="width: 80px;">' + _("services status configure") + '</td>';
					ajaxModifyElementHTML('miscServices_row', html);
					statusServicesMenu = 1;
					displayElement(elements, 		true);
					displayElement('dnsproxy',		'<% getDnsmasqBuilt(); %>' == '1');
					displayElement('upnp',			'<% getUpnpBuilt(); %>' == '1');
					displayElement('parprouted',	'<% getARPptBuilt(); %>' == '1');
				} else {
					ajaxModifyElementHTML('miscServices_row', '<td class="title" colspan="5" onClick="showServicesMenu();"><img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc services") + '</td>');
					statusServicesMenu = 0;
					displayElement(elements, false);
				}
			}

			function showServicesIPTVMenu() {
				var elements = [ 'igmpProxy', 'igmpSnoop', 'igmpFastL', 'igmpM2UConv', 'udpxy', 'udpxy_port_row', 'xupnpd' ];
				var html;
				if (statusServicesIPTVMenu == 0) {
					html =  '<td class="title" onClick="showServicesIPTVMenu();"><img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc iptv") + '</td>';
					html += '<td class="title" onClick="showServicesIPTVMenu();">' + _("services misc value") + '</td>';
					html +=	'<td class="title" onClick="showServicesIPTVMenu();" style="width: 88px;">' + _("services status details") + '</td>';
					html +=	'<td class="title" onClick="showServicesIPTVMenu();" style="width: 56px;">' + _("services misc status") + '</td>';
					html +=	'<td class="title" onClick="showServicesIPTVMenu();" style="width: 80px;">' + _("services status configure") + '</td>';
					ajaxModifyElementHTML('miscIPTV_row', html);
					statusServicesIPTVMenu = 1;

					var opmode			= '<% getCfgZero(1, "OperationMode"); %>';
					var apcli_bridge	= '<% getCfgGeneral(1, "ApCliBridgeOnly"); %>';
					var igmpb			= '<% getIgmpProxyBuilt(); %>';

					displayElement(elements, 			true);
					displayElement('igmpProxy',			igmpb == '1' && opmode != '0' && apcli_bridge != '1');
					displayElement('igmpFastL',			(igmpb == '1' && opmode != '0' && apcli_bridge != '1') || opmode != '2');
					displayElement('igmpM2UConv',		opmode != '2');
					displayElement('udpxy_port_row',	document.miscServiceCfg.udpxyMode.selectedIndex == '1');
					displayElement('xupnpd',			'<% getXupnpdBuilt(); %>' == '1');
				} else {
					ajaxModifyElementHTML('miscIPTV_row', '<td class="title" colspan="5" onClick="showServicesIPTVMenu();"><img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc iptv") + '</td>');
					statusServicesIPTVMenu = 0;
					displayElement(elements, false);
				}
			}

			function showServicesInfoMenu() {
				var elements = [ 'snmpd', 'snmpCommunityRow', 'lltd', 'lldpd', 'cdp' ];
				var html;
				if (statusServicesInfoMenu == 0) {
					html =  '<td class="title" onClick="showServicesInfoMenu();"><img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc dinf") + '</td>';
					html += '<td class="title" onClick="showServicesInfoMenu();">' + _("services misc value") + '</td>';
					html +=	'<td class="title" onClick="showServicesInfoMenu();" style="width: 88px;">' + _("services status details") + '</td>';
					html +=	'<td class="title" onClick="showServicesInfoMenu();" style="width: 56px;">' + _("services misc status") + '</td>';
					html +=	'<td class="title" onClick="showServicesInfoMenu();" style="width: 80px;">' + _("services status configure") + '</td>';
					ajaxModifyElementHTML('miscDINF_row', html);
					statusServicesInfoMenu = 1;
					displayElement(elements,			true);
					displayElement('snmpCommunityRow',	document.miscServiceCfg.SnmpdEnabled.value != '0');
				} else {
					ajaxModifyElementHTML('miscDINF_row', '<td class="title" colspan="5" onClick="showServicesInfoMenu();"><img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc dinf") + '</td>');
					statusServicesInfoMenu = 0;
					displayElement(elements, false);
				}
			}

			function showPassThroughMenu() {
				var elements = [ 'miscPTPPPoE_row', 'miscPTIPv6_row' ];
				if (statusPassThroughMenu == 0) {
					ajaxModifyElementHTML('miscPassThrough', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc pass through"));
					statusPassThroughMenu = 1;
					displayElement(elements, true);
				} else {
					ajaxModifyElementHTML('miscPassThrough', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc pass through"));
					statusPassThroughMenu = 0;
					displayElement(elements, false);
				}
			}

			function showWatchersMenu() {
				var elements = [ 'miscWatchdog_row', 'miscPinger_row', 'pinger_row1', 'pinger_row2' ];
				if (statusWatchersMenu == 0) {
					ajaxModifyElementHTML('miscWatchers', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc watchers"));
					statusWatchersMenu = 1;
					displayElement(elements, true);
					pingerSelect(document.miscServiceCfg);
				} else {
					ajaxModifyElementHTML('miscWatchers', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc watchers"));
					statusWatchersMenu = 0;
					displayElement(elements, false);
				}
			}

			function showOthersMenu() {
				var elements = [ 'miscVLANDoubleTag_row', 'irqbalance', 'miscReinitWAN_row', 'miscDHCPRenew_row', 'miscSysfwPingFrmWANFilterHead_row', 'miscStp_row', 'miscDontModifyTTL_row', 'mcast_store_ttl_row', 'miscUsePMTU_row' ];
				if (statusOthersMenu == 0) {
					ajaxModifyElementHTML('miscOthers', '<img src="/graphics/menu_minus.gif" width=25 height=11>' + _("services misc others"));
					statusOthersMenu = 1;
					displayElement(elements,				true);
					displayElement('irqbalance',			'<% isSMP(); %>' == '1');
					displayElement('mcast_store_ttl_row',	document.miscServiceCfg.igmpEnbl.value == '1');
				} else {
					ajaxModifyElementHTML('miscOthers', '<img src="/graphics/menu_plus.gif" width=25 height=11>' + _("services misc others"));
					statusOthersMenu = 0;
					displayElement(elements, false);
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td><h1 id="miscTitle">Miscellaneous Services Setup</h1>
					<p  id="miscIntroduction"></p>
					<p  id="miscOffloadEngine">In section "Offload engine" you will select the offload mode (for IPOE/PPPOE recommended HARDWARE mode, for L2TP/PPTP - COMPLEX).</p>
					<p  id="miscSoftwareFastpath">In the section "Software fastpaths" (work only if offload mode complex or software) it is possible to disable selectively one of mechanisms of program offload (not recommended disable for PPTP/L2TP).</p>
					<p  id="miscImportant">IMPORTANT: If you have problems with SIP or other applications by using UDP, try to disable UDP offload(some ppe revisions not correct work with udp).</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="miscServiceCfg" action="/goform/setMiscServices" onSubmit="return CheckValues(this);">
					<table class="form">
						<!-- Offload engine -->
						<tr>
							<td class="title" colspan="5" id="miscOffloadSetup" onClick="showOffloadEngineMenu();">Offload engine</td>
						</tr>
						<tr id="miscNATOffload_row">
							<td class="head" id="miscNATOffload" style="width: 45%">NAT offload mode</td>
							<td colspan="4">
								<select name="offloadMode" class="normal" onChange="offloadModeSelect();">
									<option value="0" id="disable">Disable</option>
									<option value="1" id="miscSoftware">Software</option>
									<option value="2" id="miscHardware">Hardware</option>
									<option value="3" id="miscComplex">Complex</option>
								</select>
							</td>
						</tr>
					<tr id="wifihw_row">
						<td class="head" id="miscWiFiOffload" style="width: 45%">WiFi hardware nat offload</td>
						<td colspan="4">
							<select name="hw_nat_wifiPT" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="udphw_row">
						<td class="head" id="miscUDPOffload" style="width: 45%">UDP hardware nat offload</td>
						<td colspan="4">
							<select name="hw_nat_udpPT" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="sixhw_row">
						<td class="head" id="miscIPV6offload" style="width: 45%">IPV6 hardware route offload</td>
						<td colspan="4">
							<select name="hw_nat_sixPT" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="hwnat_threshold_row" style="display: none;">
						<td class="head" id="miscHW_NAT" style="width: 45%">HW_NAT binding threshold</td>
						<td colspan="4">
							<input name="hwnatThreshold" class="normal">&nbsp;<span style="color: #c0c0c0;">(0-500)</span>
						</td>
					</tr>
					<tr id="miscNATimpl_row">
						<td class="head" id="miscNATimpl" style="width: 45%">NAT implementation</td>
						<td colspan="4">
							<select name="natMode" class="normal">
								<option value="0">Linux</option>
								<option value="1" selected>Fcone</option>
								<option value="2">Rcone</option>
							</select>
						</td>
					</tr>
					<!-- Software fastpaths -->
					<tr id="fastpath_row">
						<td class="title" colspan="5" id="miscSoftwareTitle" onClick="showSoftwareFastpathMenu();">Software fastpaths</td>
					</tr>
					<tr id="nat_fastpath_row" style="display: none;">
						<td class="head" id="miscNATFastpath" style="width: 45%">NAT fastpath</td>
						<td colspan="4">
							<select name="natFastpath" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="route_fastpath_row" style="display: none;">
						<td class="head" id="miscRouteFastpath" style="width: 45%">Route fastpath</td>
						<td colspan="4">
							<select name="routeFastpath" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="filter_fastpath_row" style="display: none;">
						<td class="head" id="miscNFFastpath" style="width: 45%">Netfilter fastpath</td>
						<td colspan="4">
							<select name="filterFastpath" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="smb_fastpath_row" style="display: none;">
						<td class="head" id="miscSmbFastpath" style="width: 45%">Fastpath for samba traffic</td>
						<td colspan="4">
							<select name="smbFastpath" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<!-- Remote management -->
					<tr>
						<td class="title" colspan="5" id="miscRemoteSetup" onClick="showRemoteManagementMenu();">Remote management</td>
					</tr>
					<tr id="miscWebRemote_row">
						<td class="head" id="miscWebRemote" style="width: 45%">HTTP Remote Management</td>
						<td colspan="4">
							<select name="rmtHTTP" class="normal" onChange="httpRmtSelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1">LAN</option>
								<option value="2">LAN &amp; WAN</option>
							</select>
						</td>
					</tr>
					<tr id="http_rmt_port" style="display: none;">
						<td class="head" id="miscWebPort" style="width: 45%">Remote HTTP port</td>
						<td colspan="4">
							<input class="normal" name="RemoteManagementPort">
						</td>
					</tr>
					<tr id="miscSSHRemote_row">
						<td class="head" id="miscSSHRemote" style="width: 45%">SSH Remote Management</td>
						<td colspan="4">
							<select name="rmtSSH" class="normal" onchange="sshRmtSelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1">LAN</option>
								<option value="2">LAN &amp; WAN</option>
							</select>
						</td>
					</tr>
					<tr id="ssh_rmt_port" style="display: none;">
						<td class="head" id="miscSSHPort" style="width: 45%">Remote SSH port</td>
						<td colspan="4"><input class="normal" name="RemoteSSHPort"></td>
					</tr>
					<tr id="cwmpd">
						<td class="head" id="lCwmpd" style="width: 45%">CWMP (TR-069) agent</td>
						<td>
							<select name="cwmpdEnbl" class="normal" onchange="cwmpRmtSelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="cwmp_acs_url" style="display: none;">
						<td class="head" id="miscCwmpACSUrl" style="width: 45%">CWMP ACS address</td>
						<td colspan="4"><input class="superwide" name="cwmpACSUrl"></td>
					</tr>
					<tr id="rmt_telnetd">
						<td class="head" id="miscTelnetRemote" style="width: 45%">Remote Telnet</td>
						<td colspan="4">
							<select name="rmtTelnet" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<!-- Services -->
					<tr id="miscServices_row">
					</tr>
					<tr id="dnsproxy">
						<td class="head" id="miscDnsp" style="width: 45%">DNS cached proxy</td>
						<td>
							<select name="dnspEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="upnp">
						<td class="head" id="miscUpnp" style="width: 45%">UPNP support</td>
						<td>
							<select name="upnpEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="parprouted">
						<td class="head" id="miscArppt" style="width: 45%">ARP Proxy</td>
						<td>
							<select name="arpPT" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="crond">
						<td class="head" id="miscCron" style="width: 45%">Cron daemon</td>
						<td>
							<select name="CrondEnable" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					  </tr>
					<!-- IPTV -->
					<tr id="miscIPTV_row">
					</tr>
					<tr id="igmpProxy">
						<td class="head" id="miscIgmpp" style="width: 45%">IGMP proxy</td>
						<td>
							<select name="igmpEnbl" class="normal" onChange="igmpSelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="igmpSnoop">
						<td class="head" id="miscIgmppMC" style="width: 45%">IGMP snooping support</td>
						<td colspan="4">
							<select name="igmpSnoop" class="normal">
								<option value="" id="miscIGMPAuto">Auto</option>
								<option value="h" id="miscIGMPHW">HW Only</option>
								<option value="n" id="disable">Disable</option>
							</select>
						</td>
					</tr>
					<tr id="igmpFastL">
						<td class="head" id="miscIgmppFL" style="width: 45%">IGMP fast leave support</td>
						<td colspan="4">
							<select name="igmpFastL" class="normal">
								<option value="1" id="enable">Enable</option>
								<option value="0" id="disable">Disable</option>
							</select>
						</td>
					</tr>
					<tr id="igmpM2UConv">
						<td class="head" id="miscIgmppMC2UC" style="width: 45%">Multicast to Unicast conversion</td>
						<td colspan="4">
							<select name="igmpM2UConv" class="normal">
								<option value="wlan" id="miscIgmppMC2UCwlan">WLAN</option>
								<option value="lan"  id="miscIgmppMC2UClan">LAN</option>
								<option value="all"  id="miscIgmppMC2UCall">All</option>
								<option value="" id="disable">Disable</option>
							</select>
						</td>
					</tr>
					<tr id="udpxy">
						<td class="head" id="miscUdpxy" style="width: 45%">Multicast to http proxy (udpxy)</td>
						<td>
							<select name="udpxyMode" class="normal" onChange="udpxySelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1">LAN</option>
								<option value="2">LAN &amp; WAN</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="udpxy_port_row" style="display:none;">
						<td class="head" id="miscUDPXYPort" style="width: 45%">UDPXY port</td>
						<td colspan="4">
							<input name="udpxyPort" class="normal">
						</td>
					</tr>
					<tr id="xupnpd">
						<td class="head" id="miscXupnpd" style="width: 45%">UPNP media server (xupnpd)</td>
						<td>
							<select name="xupnpdEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<!-- INFO -->
					<tr id="miscDINF_row">
					</tr>
					<tr id="snmpd">
						<td class="head" id="miscSnmp" style="width: 45%">SNMP daemon</td>
						<td>
							<select name="SnmpdEnabled" class="normal" onchange="snmpdRmtSelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1">LAN</option>
								<option value="2">LAN &amp; WAN</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="snmpCommunityRow" style="display: none;">
						<td class="head" id="miscSnmpCommunity" style="width: 45%">SNMP Community</td>
						<td colspan="4">
							<input class="normal" name="snmpdcommunity" value="<% getCfgGeneral(1, "snmpdcommunity"); %>">
						</td>
					</tr>
					<tr id="lltd">
						<td class="head" id="miscLltd" style="width: 45%">LLTD daemon</td>
						<td>
							<select name="lltdEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="lldpd">
						<td class="head" id="miscLldpd" style="width: 45%">LLDP daemon</td>
						<td>
							<select name="lldpdEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<tr id="cdp">
						<td class="head" id="miscCdp" style="width: 45%">CDP daemon</td>
						<td>
							<select name="cdpEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
						<td>&nbsp;</td>
					</tr>
					<!-- Pass Through -->
					<tr>
						<td class="title" colspan="5" id="miscPassThrough" onClick="showPassThroughMenu();">Pass Through</td>
					</tr>
					<tr id="miscPTPPPoE_row">
						<td class="head" id="miscPTPPPoE" style="width: 45%">PPPOE pass through</td>
						<td colspan="4">
							<select name="krnlPppoePass" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscPTIPv6_row">
						<td class="head" id="miscPTIPv6" style="width: 45%">IPv6 pass through</td>
						<td colspan="4">
							<select name="krnlIpv6Pass" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<!-- Watchers -->
					<tr>
						<td class="title" colspan="5" id="miscWatchers" onClick="showWatchersMenu();">Watchers</td>
					</tr>
					<tr id="miscWatchdog_row">
						<td class="head" id="miscWatchdog" style="width: 45%">Watchdog service</td>
						<td colspan="4">
							<select name="watchdogEnable" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscPinger_row">
						<td class="head" id="miscPinger" style="width: 45%">Pinger service</td>
						<td colspan="4">
							<select name="pingerEnable" class="normal" onChange="pingerSelect(this.form);">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="pinger_row1">
						<td class="head" id="miscPingTime" style="width: 45%">Ping check time</td>
						<td colspan="4">
							<input name="ping_check_time" class="normal">
						</td>
					</tr>
					<tr id="pinger_row2">
						<td class="head" id="miscPingInterval" style="width: 45%">Ping check interval</td>
						<td colspan="4">
							<input name="ping_check_interval" class="normal">
						</td>
					</tr>
					<!-- Others -->
					<tr>
						<td class="title" colspan="5" id="miscOthers" onClick="showOthersMenu();">Others</td>
					</tr>
					<tr id="miscVLANDoubleTag_row">
						<td class="head" id="miscVLANDoubleTag" style="width: 45%">Vlan double tag QinQ support</td>
						<td colspan="4">
							<select name="vlanDoubleTag" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="irqbalance" style="display:none;">
						<td class="head" id="miscIRQBalance" style="width: 45%">IRQBalance</td>
						<td colspan="4">
							<select name="IRQBalance" class="normal">
								<option value="auto" id="miscAuto">Auto</option>
								<option value="router" id="miscRouter">Router</option>
								<option value="storage" id="miscStorage">Storage</option>
							</select>
						</td>
					</tr>
					<tr id="miscReinitWAN_row">
						<td class="head" id="miscReinitWAN" style="width: 45%">Reinit WAN if DHCP lease fail</td>
						<td colspan="4">
							<select name="dhcpSwReset" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscDHCPRenew_row">
						<td class="head" id="miscDHCPRenew" style="width: 45%">DHCP renew lease at WAN UP</td>
						<td colspan="4">
							<select name="ForceRenewDHCP" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscSysfwPingFrmWANFilterHead_row">
						<td class="head" id="miscSysfwPingFrmWANFilterHead" style="width: 45%">Accept ping from WAN</td>
						<td colspan="4">
							<select name="pingWANEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscStp_row">
						<td class="head" id="miscStp" style="width: 45%">802.1d Spanning Tree</td>
						<td colspan="4">
							<select name="stpEnbl" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscDontModifyTTL_row">
						<td class="head" id="miscDontModifyTTL" style="width: 45%">Do not modify TTL</td>
						<td colspan="4">
							<select name="ttlStore" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="mcast_store_ttl_row">
						<td class="head" id="miscDontModifyMCTTL" style="width: 45%">Do not modify multicast TTL</td>
						<td colspan="4">
							<select name="ttlMcastStore" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
					<tr id="miscUsePMTU_row">
						<td class="head" id="miscUsePMTU" style="width: 45%">Use PMTU discovery</td>
						<td colspan="4">
							<select name="mssPmtu" class="normal">
								<option value="0" id="disable">Disable</option>
								<option value="1" id="enable">Enable</option>
							</select>
						</td>
					</tr>
				</table>
				<br>
				<table class="buttons">
					<tr>
						<td>
							<input type="submit" class="normal" value="Apply"  id="miscApply">&nbsp;&nbsp;
							<input type="button" class="normal" value="Cancel" id="miscCancel" onClick="window.location.reload()">&nbsp;&nbsp;
							<input type="button" class="normal" value="Reset"  id="miscReset"  onClick="resetValues(this.form)">&nbsp;&nbsp;
							<input type="hidden" value="1" name="goaheadrestart">
							<input type="hidden" value="0" name="reset">
						</td>
					</tr>
				</table>
				</form>
			</tr>
		</table>
	</body>
</html>
