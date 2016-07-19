<!DOCTYPE html>
<html>
	<head>
		<title>MAC Filtering Settings</title>
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
			Butterlate.setTextDomain("firewall");
			Butterlate.setTextDomain("buttons");

			function initTranslation()
			{
				_TR("dmzTitle", 		"dmz title");
				_TR("dmzIntroduction",	"dmz introduction");
				_TR("dmzSetting",		"dmz title");
				_TR("dmzSet",			"dmz setting");
				_TR("dmzDisable",		"button disable");
				_TR("dmzEnable",		"button enable");
				_TR("dmzDisable2",		"button disable");
				_TR("dmzEnable2",		"button enable");
				_TR("dmzIPAddr",		"dmz ipaddr");
				_TR("bridge_warning",	"firewall bridge warning");
				_TRV("dmzApply",		"button apply");
				_TRV("dmzReset",		"button reset");
			}

			function initValues()
			{
				var form	= document.DMZ;
				var dmzEnable	= '<% getCfgZero(1, "DMZEnable"); %>';
				var dmzLoopback = '<% getCfgZero(1, "DMZNATLoopback"); %>';
				var dmzIPAddr	= '<% getCfgZero(1, "DMZIPAddress"); %>';
				var opMode	= '<% getCfgZero(1, "OperationMode"); %>';

				if (dmzIPAddr == "0")
					dmzIPAddr = "";
				
				displayElement('bridge_warning', opMode == '0');

				if (dmzEnable == '1')
					form.elements.DMZEnabled.options.selectedIndex = 1;
					
				form.dmzLoopback.value = dmzLoopback;
				form.DMZIPAddress.value = dmzIPAddr;
				
				dmzEnableSwitch(form);
				showWarning();
				initTranslation();
			}

			function checkValues(form)
			{
				if (!ipaddr.IPv4.isValid(form.DMZIPAddress.value) && form.DMZEnabled.options.selectedIndex) {
					alert(_("dmz invalid ip"));
					form.DMZIPAddress.focus();
					form.DMZIPAddress.select();
					return false;
				}
				ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);
				return true;
			}

			function dmzEnableSwitch(form)
			{
				enableElements([form.DMZIPAddress, form.dmzLoopback], form.DMZEnabled.value == '1');
				displayElement([ 'dmzAdress', 'dmzLoopback' ], form.DMZEnabled.value == '1');
			}

			function dmzLoopbackWarning(form)
			{
				if (form.dmzLoopback.value == '1')
				{
					if (!confirm(_("dmz confirm")))
						form.dmzLoopback.value='0';
				}
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();"> 
		<table class="body">
			<tr id="warning"><tr>
			<tr>
				<td><h1 id="dmzTitle"> DMZ Settings </h1>
					<div style="display:none;" id="bridge_warning">
						<p><b>Warning:</b> The current operation mode is "Bridge mode" and these settings may not be functional.</p>
					</div>
					<p id="dmzIntroduction"> Here you can setup the De-Militarized Zone (DMZ) to separate your external services from the rest of LAN.</p>
					<hr>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form method="POST" name="DMZ" action="/goform/setFirewallDMZ" OnSubmit="return checkValues(this);">
						<table class="form">
							<tr>
								<td class="title" colspan="2" id="dmzSetting">DMZ Settings</td>
							</tr>
							<tr>
								<td class="head" id="dmzSet"> DMZ Settings </td>
								<td><select onChange="dmzEnableSwitch(this.form);" name="DMZEnabled" class="normal">
									<option id="dmzDisable" value="0">Disable</option>
									<option id="dmzEnable"  value="1">Enable</option>
								</select></td>
							</tr>
							<tr id="dmzAdress">
								<td class="head" id="dmzIPAddr"> DMZ IP Address </td>
								<td><input type="text" name="DMZIPAddress" class="normal"></td>
							</tr>
							<tr id="dmzLoopback">
								<td class="head" id="ldmzLoopback">DMZ NAT loopback</td>
								<td><select name="dmzLoopback" class="normal" onChange="dmzLoopbackWarning(this.form);">
									<option value="0" id="dmzDisable2">Disable</option>
									<option value="1" id="dmzEnable2">Enable</option>
								</select></td>
							</tr>
						</table>
						<table class="buttons">
							<tr>
								<td><input type="submit" class="normal" value="Apply" id="dmzApply" name="addDMZ"">
									<input type="button" class="normal" value="Reset" id="dmzReset" name="reset_button" onClick="window.location.reload();">
								</td>
							</tr>
						</table>
					</form>
				</td>
			</tr>
		</table>
	</body>
</html>
