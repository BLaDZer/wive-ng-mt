<!DOCTYPE html>
<html>
	<head>
		<title>QoS/Bandwidth Management Settings</title>
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
		<script src="/js/controls.js"></script>
		<script src="/js/validation.js"></script>
		<script>
			Butterlate.setTextDomain("buttons");
			Butterlate.setTextDomain("network");

			function initTranslation() {
			  _TR("QoSTitleStr",		"qos title");
			  _TR("QoSIntroStr",		"qos introduction");
			  _TR("QoSSetupStr",		"qos setup");
			  _TR("QoSStr",				"qos type");
			  _TR("QoSSetupBW",			"qos bandwidth settings");

			  _TR("QoSModeStr",			"qos mode");
			  _TR("QoSAll",				"qos mode all");
			  _TR("QoSWAN",				"qos mode wan");
			  _TR("QoSVPN",				"qos mode vpn");

			  _TR("QoSUpBWStr",			"qos upload rate");
			  _TR("QoSUpBWLStr",		"qos upload limit");
			  _TR("QoSDownBWStr",		"qos download rate");
			  _TR("QoSDownBWLStr",		"qos download limit");
			  _TR("QoSUPVPNBWStr",		"qos upload vpn rate");
			  _TR("QoSUPVPNBWLStr",		"qos upload vpn limit");
			  _TR("QoSUPMDMBWStr",		"qos upload mdm rate");
			  _TR("QoSUPMDMBWLStr",		"qos upload mdm limit");
			  _TR("QoSSetupPPort",		"qos priority port");
			  _TR("QoSHPP",				"qos high priority ports");
			  _TR("QoSLPP",				"qos low priority ports");
			  _TR("QoSSetupPDSCP",		"qos priority dscp");
			  _TR("QoSHDSCP",			"qos high priority dscps");
			  _TR("QoSLDSCP",			"qos low priority dscps");
			  _TR("QoSDisable",			"button disable");
			  _TR("QoSSimple",			"qos simple");
			  _TR("QoSShaper",			"qos shaper");
			  _TR("QoSCODEL",			"qos codel");
			  _TR("fastpath_warning",	"qos warning");

			  _TRV("QoSApply",			"button apply");
			  _TRV("QoSCancell",		"button cancel")
			  _TRV("QoSReset",			"button reset");
			}

			function initValues() {
				var form = document.QoSSetup;
				
				form.QoSSelect.value	= NVRAM_QoSEnable;
				form.QoSMode.value		= +NVRAM_QoSMode;
				form.UpBW.value			= NVRAM_QoS_rate_up;
				form.UpBWL.value		= NVRAM_QoS_rate_limit_up;
				form.DownBW.value		= NVRAM_QoS_rate_down;
				form.DownBWL.value		= NVRAM_QoS_rate_limit_down;
				form.UpVPNBW.value		= NVRAM_QoS_rate_vpn_up;
				form.UpVPNBWL.value		= NVRAM_QoS_rate_vpn_limit_up;
				form.UpMDMBW.value		= NVRAM_QoS_rate_mdm_up;
				form.UpMDMBWL.value		= NVRAM_QoS_rate_mdm_limit_up;
				form.QoS_HPP.value		= NVRAM_QoS_high_pp;
				form.QoS_LPP.value		= NVRAM_QoS_low_pp;
				form.QoS_HDSCP.value	= NVRAM_QoS_high_dscp;
				form.QoS_LDSCP.value	= NVRAM_QoS_low_dscp;
					
				QoSSelectChange(form);
				showWarning();
				initTranslation();
			}

			function checkValues(form) {
				if (!validateNum(form.UpBW.value)) {
					alert(_("qos invalid bw"));
					form.UpBW.focus();
					return false;
				}
				if (!validateNum(form.UpBWL.value)) {
					alert(_("qos invalid bw"));
					form.UpBWL.focus();
					return false;
				}
				if (!validateNum(form.DownBW.value)) {
					alert(_("qos invalid bw"));
					form.DownBW.focus();
					return false;
				}
				if (!validateNum(form.DownBWL.value)) {
					alert(_("qos invalid bw"));
					form.DownBWL.focus();
					return false;
				}
				if (!validateNum(form.UpVPNBW.value)) {
					alert(_("qos invalid bw"));
					form.UpVPNBW.focus();
					return false;
				}
				if (!validateNum(form.UpVPNBWL.value)) {
					alert(_("qos invalid bw"));
					form.UpVPNBWL.focus();
					return false;
				}
				if (!validateNum(form.UpMDMBW.value)) {
					alert(_("qos invalid bw"));
					form.UpMDMBW.focus();
					return false;
				}
				if (!validateNum(form.UpMDMBWL.value)) {
					alert(_("qos invalid bw"));
					form.UpMDMBWL.focus();
					return false;
				}
				if (form.QoS_HPP.value.match(/[^0-9\s]/gi)) {
					alert(_("qos invalid bw"));
					form.QoS_HPP.focus();
					return false;				
				}
				else {
					var port = form.QoS_HPP.value.split(' ');
					for (var i=0; i<port.length; i++) {
						if (!checkDigitRange(port[i], 1, 65535)) {
							alert(_("qos invalid bw"));
							form.QoS_HPP.focus();
							return false;				
						}
					}
				}
				if (form.QoS_LPP.value.match(/[^0-9\s]/gi)) {
					alert(_("qos invalid bw"));
					form.QoS_LPP.focus();
					return false;				
				}
				else {
					var port = form.QoS_LPP.value.split(' ');
					for (var i=0; i<port.length; i++) {
						if (!checkDigitRange(port[i], 1, 65535)) {
							alert(_("qos invalid bw"));
							form.QoS_LPP.focus();
							return false;				
						}
					}
				}				
				if (form.QoS_HDSCP.value.match(/[^A-F0-9]/gi)) {
					alert(_("qos invalid bw"));
					form.QoS_HDSCP.focus();
					return false;				
				}
				if (form.QoS_LDSCP.value.match(/[^A-F0-9]/gi)) {
					alert(_("qos invalid bw"));
					form.QoS_LDSCP.focus();
					return false;				
				}
				
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function QoSSelectChange(form) {
				displayElement( [ 'BWSettings', 'QoSMode', 'PPortSettings', 'QoSUpBW', 'QoSUpBWL', 'QoSDownBW', 'QoSDownBWL', 'QoSUPVPNBW', 'QoSUPVPNBWL', 'QoSUPMDMBW', 'QoSUPMDMBWL', 'user_Qos_HPP', 'user_Qos_LPP', 'user_Qos_HDSCP', 'user_Qos_LDSCP', 'PDSCPSettings' ], form.QoSSelect.value == '2' );
				displayElement('fastpath_warning', NVRAM_offloadMode == '2' || NVRAM_offloadMode == '3');
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td><h1 id="QoSTitleStr">Quality of Service Settings </h1>
					<p id="QoSIntroStr"> Here you can setup rules to provide desired Quality of Service for specific applications.</p>
					<div style="display:none;" id="fastpath_warning">For correct operation of QoS (modes: Simple Priority-Based, Complex, Codel) need turning off <b>NAT offload mode</b>. Please note that turning off <b>NAT offload mode</b> will very increase CPU usage.</div>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="post" name="QoSSetup" action="/goform/QoSSetup" OnSubmit="return checkValues(this);">
						<table class="form">
							<tr>
								<td class="title" colspan="2" id="QoSSetupStr">QoS Setup</td>
							</tr>
							<tr>
								<td class="head" id="QoSStr" width="40%">Type of QoS</td>
								<td width="60%"><select name="QoSSelect" onChange="QoSSelectChange(this.form);" style="width: 98%">
									<option value="0" id="QoSDisable">Disable</option>
									<option value="1" id="QoSSimple">Simple priority based QoS</option>
									<option value="2" id="QoSShaper">Complex with shaper (ports based mark + HTB limit)</option>
									<option value="3" id="QoSCODEL">Automatic Fair Queue Controlled Delay AQM (CODEL)</option>
								</select></td>
							</tr>
							<tr id="BWSettings">
								<td class="title" colspan="2" id="QoSSetupBW">Bandwidth settings</td>
							</tr>
							<tr id="QoSMode">
								<td class="head" id="QoSModeStr" width="40%">Shape from:</td>
								<td width="60%"><select name="QoSMode" class="half">
									<option value="0" id="QoSAll">All</option>
									<option value="1" id="QoSWAN">Only WAN</option>
									<option value="2" id="QoSVPN">Only VPN</option>
								</select></td>
							</tr>
							<tr id="QoSUpBW">
								<td class="head" id="QoSUpBWStr" width="40%">Upload rate:</td>
								<td width="60%"><input name="UpBW" class="half" size="7" maxlength="7" type="text">
									&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSUpBWL">
								<td class="head" id="QoSUpBWLStr" width="40%">Upload rate limit:</td>
								<td width="60%"><input name="UpBWL" class="half" size="7" maxlength="7" type="text">
									&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSDownBW">
								<td class="head" id="QoSDownBWStr" width="40%">Download rate:</td>
								<td width="60%"><input name="DownBW" class="half" size="7" maxlength="7" type="text">
									&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSDownBWL">
								<td class="head" id="QoSDownBWLStr" width="40%">Download rate limit:</td>
								<td width="60%"><input name="DownBWL" class="half" size="7" maxlength="7" type="text">
									&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSUPVPNBW">
								<td class="head" id="QoSUPVPNBWStr" width="40%">Upload VPN rate:</td>
								<td width="60%"><input name="UpVPNBW" class="half" size="7" maxlength="7" type="text">
									&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSUPVPNBWL">
								<td class="head" id="QoSUPVPNBWLStr" width="40%">Upload VPN rate limit:</td>
								<td width="60%"><input name="UpVPNBWL" class="half" size="7" maxlength="7" type="text">
								&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSUPMDMBW">
								<td class="head" id="QoSUPMDMBWStr" width="40%">Upload modem rate:</td>
								<td width="60%"><input name="UpMDMBW" class="half" size="7" maxlength="7" type="text">
									&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="QoSUPMDMBWL">
								<td class="head" id="QoSUPMDMBWLStr" width="40%">Upload modem rate limit:</td>
								<td width="60%"><input name="UpMDMBWL" class="half" size="7" maxlength="7" type="text">
								&nbsp;&nbsp;KBits/sec</td>
							</tr>
							<tr id="PPortSettings">
								<td class="title" colspan="2" id="QoSSetupPPort">Priority port settings</td>
							</tr>
							<tr id="user_Qos_HPP">
								<td class="head" id="QoSHPP" width="40%">User High Priority Ports</td>
								<td width="60%"><input name="QoS_HPP" style="width: 98%" size="60" maxlength="1024" type="text"></td>
							</tr>
							<tr id="user_Qos_LPP">
								<td class="head" id="QoSLPP" width="40%">User Low Priority Ports</td>
								<td width="60%"><input name="QoS_LPP" style="width: 98%" size="60" maxlength="1024" type="text"></td>
							</tr>
							<tr id="PDSCPSettings">
								<td class="title" colspan="2" id="QoSSetupPDSCP">Priority DSCP settings</td>
							</tr>
							<tr id="user_Qos_HDSCP">
								<td class="head" id="QoSHDSCP" width="40%">User High Priority DSCPs</td>
								<td width="60%"><input name="QoS_HDSCP" style="width: 98%" size="60" maxlength="1024" type="text"></td>
							</tr>
							<tr id="user_Qos_LDSCP">
								<td class="head" id="QoSLDSCP" width="40%">User Low Priority DSCPs</td>
								<td width="60%"><input name="QoS_LDSCP" style="width: 98%" size="60" maxlength="1024" type="text"></td>
							</tr>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="submit" class="normal" value="Apply" id="QoSApply">&nbsp;&nbsp;
									<input type="button" class="normal" value="Cancel" id="QoSCancell" onClick="window.location.reload();">&nbsp;&nbsp;
									<input type="button" class="normal" value="Reset" id="QoSReset" onClick="resetValues(this.form);"></td>
									<input value="0" name="reset" type="hidden">
							</tr>
						</table>
					</form>
					<div class="whitespace">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
