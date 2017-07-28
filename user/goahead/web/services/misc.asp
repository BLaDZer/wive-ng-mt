<!DOCTYPE html>
<html>
	<head>
		<title>Internet Services Settings</title>
		<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
		<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
		<meta http-equiv="Pragma" content="no-cache">
		<meta http-equiv="Expires" content="-1">
		<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
		<link rel="stylesheet" href="/style/controls.css" type="text/css">
		<link rel="stylesheet" href="/style/windows.css" type="text/css">
		<script src="/lang/b28n.js"></script>
		<script src="/js/nvram.js"></script>
		<script src="/js/ajax.js"></script>
		<script src="/js/validation.js"></script>
		<script src="/js/controls.js"></script>
		<script>
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			var statusOffloadEngineMenu		= 1;
			var statusSoftwareFastpathMenu		= 1;
			var statusRemoteManagementMenu		= 0;
			var statusServicesMenu			= 0;
			var statusServicesIPTVMenu		= 0;
			var statusServicesInfoMenu		= 1;
			var statusPassThroughMenu		= 1;
			var statusWatchersMenu			= 1;
			var statusOthersMenu			= 1;

			function initTranslation() {
				_TR("miscTitle",			"services misc title");
				_TR("miscIntroduction",			"services misc introduction");
				_TR("miscOffloadEngine",		"services misc offload note");
				_TR("miscSoftwareFastpath",		"services misc software note");
				_TR("miscImportant",			"services misc important");
				_TR("miscNATOffload",			"services misc nat offload");
				_TR("miscSoftware",			"services misc software");
				_TR("miscHardware",			"services misc hardware");
				_TR("miscComplex",			"services misc complex");
				_TR("miscWiFiOffload",			"services misc wifi offload");
				_TR("miscUDPOffload",			"services misc udp offload");
				_TR("miscIPV6offload",			"services misc ipv6 offload");
				_TR("miscHW_NAT",			"services misc hw_nat");
				_TR("miscNATimpl",			"services misc nat implementation");
				_TR("miscNATFastpath",			"services misc nat fastpath");
				_TR("miscRouteFastpath",		"services misc route fastpath");
				_TR("miscNFFastpath",			"services misc netfilter fastpath");
				_TR("miscSmbFastpath",			"services misc samba fastpath");
				_TR("miscWebRemote",			"services misc web remote");
				_TR("miscWebPort",			"services misc web port");
				_TR("miscSSHRemote",			"services misc ssh remote");
				_TR("miscSSHPort",			"services misc ssh port");
				_TR("miscTelnetRemote",			"services misc telnet remote");
				_TR("miscDnsp",				"services misc dnsproxy");
				_TR("miscUpnp",				"services misc upnp");
				_TR("miscSnmp",				"services misc snmp");
				_TR("miscSnmpCommunity",		"services misc snmp community");
				_TR("miscArppt",			"services misc arppt");
				_TR("miscCron",				"services misc cron");
				_TR("miscLltd",				"services misc lltd");
				_TR("miscLldpd",			"services misc lldpd");
				_TR("miscCdp",				"services misc cdp");
				_TR("miscArpwatch",			"services misc arpwatch");
				_TR("miscIgmpp",			"services misc igmp");
				_TR("miscIgmppMC",			"services misc igmp multicast");
				_TR("miscIgmppFL",			"services misc igmp fastleave");
				_TR("miscIgmppMC2UC",			"services misc igmp unicast");
				_TR("miscIgmppMC2UCall",		"services misc igmp unicast all");
				_TR("miscUdpxy",			"services misc udpxy");
				_TR("miscUDPXYPort",			"services misc udpxy port");
				_TR("miscXupnpd",			"services misc xupnpd");
				_TR("miscPTPPPoE",			"services misc pass through pppoe");
				_TR("miscPTIPv6",			"services misc pass through ipv6");
				_TR("miscWatchdog",			"services misc watchdog");
				_TR("miscPinger",			"services misc pinger");
				_TR("miscPingTime",			"services misc ping time");
				_TR("miscPingInterval",			"services misc ping interval");
				_TR("miscVLANDoubleTag",		"services misc vlan double tag");
				_TR("miscReinitWAN",			"services misc reinit wan");
				_TR("miscDHCPRenew",			"services misc dhcp renew");
				_TR("miscSysfwPingFrmWANFilterHead",	"services misc ping from wan");
				_TR("miscStp",				"services misc stp");
				_TR("miscDontModifyTTL",		"services misc dont modify ttl");
				_TR("miscDontModifyMCTTL",		"services misc multicast ttl");
				_TR("miscUsePMTU",			"services misc use pmtu");
				_TR("miscIRQBalance",			"services misc irqbalance");
				_TR("miscIGMPAuto",			"services misc auto");
				_TR("miscIGMPHW",			"services misc hw");
				_TR("miscAuto",				"services misc auto");
				_TR("miscRouter",			"services misc router");
				_TR("miscStorage",			"services misc storage");
				_TR("servicesMiscServicesShow",		"services misc services");
				_TR("servicesMiscServicesHide",		"services misc services");
				_TR("servicesMiscValues",		"services misc value");
				_TR("servicesMiscDetails",		"services status details");
				_TR("servicesMiscStatus",		"services misc status");
				_TR("servicesMiscConfigure",		"services status configure");
				_TR("servicesMiscIPTVshow",		"services misc iptv");
				_TR("servicesMiscIPTVhide",		"services misc iptv");
				_TR("servicesMiscIPTVvalue",		"services misc value");
				_TR("servicesMiscIPTVdetails",		"services status details");
				_TR("servicesMiscIPTVstatus",		"services misc status");
				_TR("servicesMiscIPTVconfigure",	"services status configure");
				_TR("servicesMiscDINFshow",		"services misc dinf");
				_TR("servicesMiscDINFhide",		"services misc dinf");
				_TR("servicesMiscDINFvalue",		"services misc value");
				_TR("servicesMiscDINFdetails",		"services status details");
				_TR("servicesMiscDINFstatus",		"services misc status");
				_TR("servicesMiscDINFconfigure",	"services status configure");
				_TRV("miscApply",			"button apply");
				_TRV("miscCancel",			"button cancel");
				_TRV("miscReset",			"button reset");

				var elements = document.getElementsByTagName('option');
				for (var i = 0; i < elements.length; i++)
					if (elements[i].id == "disable")
						elements[i].innerHTML = _("button disable");
					else if (elements[i].id == "enable")
						elements[i].innerHTML = _("button enable");
			}

			function initValues() {

				var form = document.miscServiceCfg;

				form.RemoteManagementPort.value			= NVRAM_RemoteManagementPort;
				form.RemoteSSHPort.value			= NVRAM_RemoteSSHPort;
				form.hwnatThreshold.value			= NVRAM_hw_nat_bind;
				form.stpEnbl.options.selectedIndex		= NVRAM_stpEnabled;
				form.igmpEnbl.options.selectedIndex		= (BUILD_IGMPPROXY) ? NVRAM_igmpEnabled : 0;
				form.igmpSnoop.value				= NVRAM_igmpSnoopMode;
				form.igmpFastL.value				= NVRAM_igmpFastLeave;
				form.igmpM2UConv.value				= NVRAM_igmpM2UConvMode;
				form.upnpEnbl.options.selectedIndex		= NVRAM_upnpEnabled;
				form.xupnpdEnbl.options.selectedIndex		= NVRAM_xupnpd;
				form.dnspEnbl.options.selectedIndex		= NVRAM_dnsPEnabled;
				form.cdpEnbl.options.selectedIndex		= NVRAM_cdpEnabled;
				form.arpwatchEnbl.options.selectedIndex		= NVRAM_arpwatch;
				form.lltdEnbl.options.selectedIndex		= NVRAM_lltdEnabled;
				form.lldpdEnbl.options.selectedIndex		= NVRAM_lldpdEnabled;
				form.krnlPppoePass.options.selectedIndex	= NVRAM_pppoe_pass;
				form.krnlIpv6Pass.options.selectedIndex		= NVRAM_ipv6_pass;
				form.pingWANEnbl.options.selectedIndex		= NVRAM_WANPingFilter;
				form.arpPT.options.selectedIndex		= NVRAM_parproutedEnabled;
				form.hw_nat_wifiPT.options.selectedIndex	= NVRAM_hw_nat_wifi;
				form.hw_nat_udpPT.options.selectedIndex		= NVRAM_hw_nat_udp;
				form.hw_nat_sixPT.options.selectedIndex		= NVRAM_hw_nat_six;
				form.pingerEnable.value				= NVRAM_pinger_check_on;
				form.mssPmtu.value				= NVRAM_mss_use_pmtu;
				form.natMode.value				= NVRAM_nat_mode;
				form.rmtHTTP.value				= NVRAM_RemoteManagement;
				form.rmtSSH.value				= NVRAM_RemoteSSH;
				form.rmtTelnet.value				= NVRAM_RemoteTelnet;
				form.udpxyMode.value				= NVRAM_UDPXYMode;
				form.udpxyPort.value				= NVRAM_UDPXYPort;
				form.watchdogEnable.value			= NVRAM_WatchdogEnabled;
				form.dhcpSwReset.value				= NVRAM_vlanDoubleTag;
				form.vlanDoubleTag.value			= NVRAM_dhcpSwReset;
				form.natFastpath.value				= NVRAM_natFastpath;
				form.routeFastpath.value			= NVRAM_routeFastpath;
				form.filterFastpath.value			= NVRAM_filterFastpath;
				form.CrondEnable.value				= NVRAM_CrondEnable;
				form.ForceRenewDHCP.value			= NVRAM_ForceRenewDHCP;
				form.SnmpdEnabled.value				= NVRAM_snmpd;
				form.ttlStore.value				= NVRAM_store_ttl;
				form.ttlMcastStore.value			= NVRAM_store_ttl_mcast;
				form.offloadMode.value				= NVRAM_offloadMode;
				form.IRQBalance.value				= NVRAM_IRQBalance;
				form.ping_check_time.value			= NVRAM_ping_check_time;
				form.ping_check_interval.value			= NVRAM_ping_check_interval;
				form.smbFastpath.options.selectedIndex		= NVRAM_smbFastpath;
				form.snmpdcommunity.value			= NVRAM_snmpdcommunity;

				offloadModeSelect(form);
				httpRmtSelect(form);
				sshRmtSelect(form);
				igmpSelect(form);
				udpxySelect(form);
				snmpdRmtSelect(form);
				pingerSelect(form);

				displayServiceStatus(
					[
						[ NVRAM_UDPXYMode,		'udpxy',		'udpxy',		NVRAM_UDPXYPort + '/status/',		'udpxy.sourceforge.net' ],
						[ NVRAM_xupnpd,			'xupnpd',		'xupnpd',		'4044/',				'xupnpd.org/' ],
						[ NVRAM_CrondEnable,		'crond',		'crond',		null,					'crontab.org/' ],
						[ NVRAM_snmpd,			'snmpd',		'snmpd',		null,					'www.net-snmp.org/docs/man/snmpd.html' ],
						[ NVRAM_igmpEnabled,		'igmpProxy',		'igmpproxy',		null,					'sourceforge.net/projects/igmpproxy' ],
						[ NVRAM_lltdEnabled,		'lltd',			'lld2d',		null,					'msdn.microsoft.com/en-us/windows/hardware/gg463061.aspx' ],
						[ NVRAM_lldpdEnabled,		'lldpd',		'lldpd',		null,					'vincentbernat.github.io/lldpd' ],
						[ NVRAM_upnpEnabled,		'upnp',			'miniupnpd',		null,					'miniupnp.free.fr/' ],
						[ NVRAM_cdpEnabled,		'cdp',			'cdp-send',		null,					'freecode.com/projects/cdp-tools' ],
						[ NVRAM_dnsPEnabled,		'dnsproxy',		'dnsmasq',		null,					'thekelleys.org.uk/dnsmasq/doc.html' ],
						[ NVRAM_parproutedEnabled,	'parprouted',		'parprouted',		null,					'freecode.com/projects/parprouted' ],
						[ NVRAM_arpwatch,		'arpwatch',		'arpwatch',		null,					'wikipedia.org/wiki/Arpwatch' ]
					]
				);

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

			function CheckValues(form) {
				var rmtManagementPort = NVRAM_RemoteManagementPort;

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

			function offloadModeSelect(form) {
				var thresh	= document.miscServiceCfg.offloadMode.value;
				var fastpathb	= BUILD_FASTPATH;

				// Mode
				displayElement('hwnat_threshold_row',		thresh == '2' || thresh == '3');
				// HW extensions
				displayElement('wifihw_row',			thresh == '2' || thresh == '3');
				displayElement('udphw_row',			thresh == '2' || thresh == '3');
				displayElement('sixhw_row',			thresh == '2' || thresh == '3');
				// SW extensions
				displayElement('fastpath_row',			thresh == '1' || thresh == '3');
				displayElement('miscSoftwareFastpath',		thresh == '1' || thresh == '3')
				displayElement('filter_fastpath_row',		thresh == '1' || thresh == '3');
				if (fastpathb == "1") {
					displayElement('nat_fastpath_row',	thresh == '1' || thresh == '3');
					displayElement('route_fastpath_row',	thresh == '1' || thresh == '3');
				}
			}

			function pingerSelect(form) {
				displayElement([ 'pinger_row1', 'pinger_row2' ], form.pingerEnable.value == '1');
			}

			function igmpSelect(form) {
				displayElement('mcast_store_ttl_row', form.igmpEnbl.value == '1' && statusOthersMenu == '1');
			}

			function httpRmtSelect(form) {
				displayElement('http_rmt_port', form.rmtHTTP.value != '0');
			}

			function sshRmtSelect(form) {
				displayElement('ssh_rmt_port', form.rmtSSH.value != '0');
			}

			function udpxySelect(form) {
				displayElement('udpxy_port_row', form.udpxyMode.value != '0');
			}

			function snmpdRmtSelect(form) {
				displayElement('snmpCommunityRow', form.SnmpdEnabled.value != '0');
			}

			function showOffloadEngineMenu() {
				var elements = [ 'miscNATOffload_row', 'wifihw_row', 'udphw_row', 'sixhw_row', 'hwnat_threshold_row', 'miscNATimpl_row' ];
				if (statusOffloadEngineMenu == 0) {
					ajaxModifyElementHTML('miscOffloadSetup', '<img src="/graphics/menu_minus.gif" width="25" height="11">' + _("services misc offload setup"));
					statusOffloadEngineMenu = 1;
					displayElement(elements, true);

					var thresh = document.miscServiceCfg.offloadMode.value;
					displayElement([	'hwnat_threshold_row',
								'wifihw_row',
								'udphw_row',
								'sixhw_row' ],			thresh == '2' || thresh == '3');
					displayElement([	'fastpath_row',
								'miscSoftwareFastpath' ],	thresh == '1' || thresh == '3');
				} else {
					ajaxModifyElementHTML('miscOffloadSetup', '<img src="/graphics/menu_plus.gif" width="25" height="11">' + _("services misc offload setup"));
					statusOffloadEngineMenu = 0;
					displayElement(elements, false);
				}
			}

			function showSoftwareFastpathMenu() {
				var elements = [ 'nat_fastpath_row', 'route_fastpath_row', 'filter_fastpath_row', 'smb_fastpath_row' ];
				if (statusSoftwareFastpathMenu == 0) {
					ajaxModifyElementHTML('miscSoftwareTitle', '<img src="/graphics/menu_minus.gif" width="25" height="11">' + _("services misc software title"));
					statusSoftwareFastpathMenu = 1;
					displayElement(elements, true);
					displayElement('smb_fastpath_row',	BUILD_SMBFP);
				} else {
					ajaxModifyElementHTML('miscSoftwareTitle', '<img src="/graphics/menu_plus.gif" width="25" height="11">' + _("services misc software title"));
					statusSoftwareFastpathMenu = 0;
					displayElement(elements, false);
				}
			}

			function showRemoteManagementMenu() {
				var elements = [ 'miscWebRemote_row', 'http_rmt_port', 'miscSSHRemote_row', 'ssh_rmt_port', 'rmt_telnetd' ];
				if (statusRemoteManagementMenu == 0) {
					ajaxModifyElementHTML('miscRemoteSetup', '<img src="/graphics/menu_minus.gif" width="25" height="11">' + _("services misc remote setup"));
					statusRemoteManagementMenu = 1;
					displayElement(elements, true);
					displayElement('rmt_telnetd', BUILD_TELNETD);
					httpRmtSelect(document.miscServiceCfg);
					sshRmtSelect(document.miscServiceCfg);
				}
				else {
					ajaxModifyElementHTML('miscRemoteSetup', '<img src="/graphics/menu_plus.gif" width="25" height="11">' + _("services misc remote setup"));
					statusRemoteManagementMenu = 0;
					displayElement(elements, false);
				}
			}

			function showServicesMenu() {
				var elements = [ 'dnsproxy', 'upnp', 'parprouted', 'crond' ];
				if (statusServicesMenu == 0) {
					statusServicesMenu = 1;
					displayElement('miscServices_show', true);
					displayElement('miscServices_hide', false);
					displayElement(elements, true);
					displayElement('dnsproxy',	BUILD_DNSMASQ);
					displayElement('upnp',		BUILD_UPNP);
					displayElement('parprouted',	BUILD_ARPPT);
				}
				else {
					statusServicesMenu = 0;
					displayElement('miscServices_show', false);
					displayElement('miscServices_hide', true);
					displayElement(elements, false);
				}
			}

			function showServicesIPTVMenu() {
				var elements = [ 'igmpProxy', 'igmpSnoop', 'igmpFastL', 'igmpM2UConv', 'udpxy', 'udpxy_port_row', 'xupnpd' ];
				if (statusServicesIPTVMenu == 0) {
					statusServicesIPTVMenu = 1;
					displayElement('miscIPTV_show', true);
					displayElement('miscIPTV_hide', false);
					displayElement(elements, true);
					displayElement('igmpProxy', BUILD_IGMPPROXY && NVRAM_OperationMode != '0' && NVRAM_ApCliBridgeOnly != '1');
					displayElement('igmpFastL', (BUILD_IGMPPROXY && NVRAM_OperationMode != '0' && NVRAM_ApCliBridgeOnly != '1') || NVRAM_OperationMode != '2');
					displayElement('igmpM2UConv', NVRAM_OperationMode != '2');
					displayElement('udpxy_port_row', document.getElementById('udpxyMode').selectedIndex != '0');
					displayElement('xupnpd', BUILD_XUPNPD);
				}
				else {
					statusServicesIPTVMenu = 0;
					displayElement('miscIPTV_show', false);
					displayElement('miscIPTV_hide', true);
					displayElement(elements, false);
				}
			}

			function showServicesInfoMenu() {
				var elements = [ 'snmpd', 'snmpCommunityRow', 'lltd', 'lldpd', 'cdp', 'arpwatch' ];
				if (statusServicesInfoMenu == 0) {
					statusServicesInfoMenu = 1;
					displayElement('miscDINF_show', true);
					displayElement('miscDINF_hide', false);
					displayElement(elements, true);
					displayElement('cdp', BUILD_CDP);
					displayElement('lltd', BUILD_LLTD);
					displayElement('lldpd', BUILD_LLDPD);
					displayElement('snmpd', BUILD_SNMPD);
					displayElement('arpwatch', BUILD_ARPWATCH);
					displayElement('snmpCommunityRow', document.getElementById('SnmpdEnabled').value != '0');
				}
				else {
					statusServicesInfoMenu = 0;
					displayElement('miscDINF_show', false);
					displayElement('miscDINF_hide', true);
					displayElement(elements, false);
				}
			}

			function showPassThroughMenu() {
				var elements = [ 'miscPTPPPoE_row', 'miscPTIPv6_row' ];
				if (statusPassThroughMenu == 0) {
					ajaxModifyElementHTML('miscPassThrough', '<img src="/graphics/menu_minus.gif" width="25" height="11">' + _("services misc pass through"));
					statusPassThroughMenu = 1;
					displayElement(elements, true);
				} else {
					ajaxModifyElementHTML('miscPassThrough', '<img src="/graphics/menu_plus.gif" width="25" height="11">' + _("services misc pass through"));
					statusPassThroughMenu = 0;
					displayElement(elements, false);
				}
			}

			function showWatchersMenu() {
				var elements = [ 'miscWatchdog_row', 'miscPinger_row', 'pinger_row1', 'pinger_row2' ];
				if (statusWatchersMenu == 0) {
					ajaxModifyElementHTML('miscWatchers', '<img src="/graphics/menu_minus.gif" width="25" height="11">' + _("services misc watchers"));
					statusWatchersMenu = 1;
					displayElement(elements, true);
					pingerSelect(document.miscServiceCfg);
				} else {
					ajaxModifyElementHTML('miscWatchers', '<img src="/graphics/menu_plus.gif" width="25" height="11">' + _("services misc watchers"));
					statusWatchersMenu = 0;
					displayElement(elements, false);
				}
			}

			function showOthersMenu() {
				var elements = [ 'miscVLANDoubleTag_row', 'irqbalance', 'miscReinitWAN_row', 'miscDHCPRenew_row', 'miscSysfwPingFrmWANFilterHead_row', 'miscStp_row', 'miscDontModifyTTL_row', 'mcast_store_ttl_row', 'miscUsePMTU_row' ];
				if (statusOthersMenu == 0) {
					ajaxModifyElementHTML('miscOthers', '<img src="/graphics/menu_minus.gif" width="25" height="11">' + _("services misc others"));
					statusOthersMenu = 1;
					displayElement(elements, true);
					displayElement('irqbalance', BUILD_SMP);
					displayElement('mcast_store_ttl_row', document.miscServiceCfg.igmpEnbl.value == '1');
				} else {
					ajaxModifyElementHTML('miscOthers', '<img src="/graphics/menu_plus.gif" width="25" height="11">' + _("services misc others"));
					statusOthersMenu = 0;
					displayElement(elements, false);
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="miscTitle">Miscellaneous Services Setup</h1>
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
					<tr id="miscServices_show">
						<td class="title" onClick="showServicesMenu();"><img src="/graphics/menu_minus.gif" width="25" height="11"><span id="servicesMiscServicesShow"></span></td>
						<td class="title" onClick="showServicesMenu();"><span id="servicesMiscValues"></span></td>
						<td class="title" onClick="showServicesMenu();" style="width: 88px;"><span id="servicesMiscDetails"></span></td>
						<td class="title" onClick="showServicesMenu();" style="width: 56px;"><span id="servicesMiscStatus"></span></td>
						<td class="title" onClick="showServicesMenu();" style="width: 80px;"><span id="servicesMiscConfigure"></span></td>
					</tr>
					<tr id="miscServices_hide">
						<td class="title" colspan="5" onClick="showServicesMenu();"><img src="/graphics/menu_plus.gif" width="25" height="11"><span id="servicesMiscServicesHide"></span></td>
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
					<tr id="miscIPTV_show">
						<td class="title" onClick="showServicesIPTVMenu();"><img src="/graphics/menu_minus.gif" width="25" height="11"><span id="servicesMiscIPTVshow"></span></td>
						<td class="title" onClick="showServicesIPTVMenu();"><span id="servicesMiscIPTVvalue"></span></td>
						<td class="title" onClick="showServicesIPTVMenu();" style="width: 88px;"><span id="servicesMiscIPTVdetails"></span></td>
						<td class="title" onClick="showServicesIPTVMenu();" style="width: 56px;"><span id="servicesMiscIPTVstatus"></span></td>
						<td class="title" onClick="showServicesIPTVMenu();" style="width: 80px;"><span id="servicesMiscIPTVconfigure"></span></td>
					</tr>
					<tr id="miscIPTV_hide">
						<td class="title" colspan="5" onClick="showServicesIPTVMenu();"><img src="/graphics/menu_plus.gif" width="25" height="11"><span id="servicesMiscIPTVhide"></span></td>
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
							<select name="udpxyMode" id="udpxyMode" class="normal" onChange="udpxySelect(this.form);">
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
					<tr id="miscDINF_show">
						<td class="title" onClick="showServicesInfoMenu();"><img src="/graphics/menu_minus.gif" width="25" height="11"><span id="servicesMiscDINFshow"></span></td>
						<td class="title" onClick="showServicesInfoMenu();"><span id="servicesMiscDINFvalue"></span></td>
						<td class="title" onClick="showServicesInfoMenu();" style="width: 88px;"><span id="servicesMiscDINFdetails"></span></td>
						<td class="title" onClick="showServicesInfoMenu();" style="width: 56px;"><span id="servicesMiscDINFstatus"></span></td>
						<td class="title" onClick="showServicesInfoMenu();" style="width: 80px;"><span id="servicesMiscDINFconfigure"></span></td>
					</tr>
					<tr id="miscDINF_hide">
						<td class="title" colspan="5" onClick="showServicesInfoMenu();"><img src="/graphics/menu_plus.gif" width="25" height="11"><span id="servicesMiscDINFhide"></span></td>
					</tr>
					<tr id="snmpd">
						<td class="head" id="miscSnmp" style="width: 45%">SNMP daemon</td>
						<td>
							<select name="SnmpdEnabled" id="SnmpdEnabled" class="normal" onchange="snmpdRmtSelect(this.form);">
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
							<input class="normal" name="snmpdcommunity">
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
					<tr id="arpwatch">
						<td class="head" id="miscArpwatch" style="width: 45%">ARPwatch</td>
						<td>
							<select name="arpwatchEnbl" class="normal">
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
				<table class="buttons">
					<tr>
						<td>
							<input type="submit" class="normal" value="Apply"  id="miscApply">&nbsp;&nbsp;
							<input type="button" class="normal" value="Cancel" id="miscCancel" onClick="window.location.reload();">&nbsp;&nbsp;
							<input type="button" class="normal" value="Reset"  id="miscReset"  onClick="resetValues(this.form);">
							<input type="hidden" value="1" name="goaheadrestart">
							<input type="hidden" value="0" name="reset">
						</td>
					</tr>
				</table>
				</form>
				<div class="whitespace">&nbsp;</div>
			</tr>
		</table>
	</body>
</html>