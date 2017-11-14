<!DOCTYPE html>
<html>
	<head>
		<title>Accounting</title>
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
			Butterlate.setTextDomain("services");
			Butterlate.setTextDomain("buttons");

			var account_interval;

			function initTranslation() {
				_TR("accountTitle",			"services account title");
				_TR("accountIntroduction",		"services account introduction");
				_TR("fastpath_warning",			"services account warning");
				_TR("accountSettings",			"services account settings");
				_TR("accountIPT",			"services account ipt");
				_TR("accountDisable",			"button disable");
				_TR("accountEnable",			"button enable");
				_TR("accountTableTitle",		"services account table");
				_TR("accountTableNoStatistic",		"services account no statistic");

				_TRV("accountApply",			"button apply");
				_TRV("accountCancel",			"button cancel");
				_TRV("accountReset",			"button reset");
				_TRV("accountResetCounters",		"button reset statistics");
			}

			function initValues() {
				document.formIptAccounting.iptEnable.value = +NVRAM_ipt_account;
				displayElement('fastpath_warning', +NVRAM_offloadMode > 0);
				displayElement('fastpath_form', +NVRAM_offloadMode == 0);
				displayElement([ 'accountStatButton', 'accountStatTable' ], NVRAM_ipt_account == '1');
				initTranslation();
				showWarning();
				if (document.formIptAccounting.iptEnable.value == 1)
					showStatTable();
			}

			function showStatTable() {
				ajaxLoadScript('/services/account_stat.js');
				account_interval = setTimeout('showStatTable();', 5000);
			}

			function accountChange() {
				displayElement([ 'accountStatButton', 'accountStatTable' ], document.formIptAccounting.iptEnable.value == 1);
			}

			function resetAccountCounters() {
				document.formIptAccounting.resetCounters.value = '1';
				document.formIptAccounting.submit();
			}
		</script>
	</head>
	<body bgcolor="#FFFFFF" onLoad="initValues();">
		<div id="warning"></div>
		<table class="body">
			<tr>
				<td>
					<h1 id="accountTitle">IP Accounting</h1>
					<p id="accountIntroduction">Here you can configure Accounting Control.</p>
					<hr>
					<!-- IP Accounting -->
					<div style="display:none;" id="fastpath_warning">
						<p><span style="color: #ff0000;"><b>CAUTION!&nbsp;</b></span> <b>NAT offload mode</b> option is turned on.</p>
						<p>Due to some technical reasons it's not possible to gather correct statistics for NAT offload mode mode now.</p>
						<p>That's why IPT accounting configuration ability is now locked.</p>
						<p>To get correct statistics you need to shut down <b>NAT offload mode</b> option on the <a href="/services/misc.asp#nat_fastpath_ref">MISC&nbsp;Services</a> configuration page.</p>
						<p>Please note that turning off <b>NAT offload mode</b> will increase CPU usage up to 50%.</p>
					</div>
					<iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
					<form action="/goform/formIptAccounting" method="POST" name="formIptAccounting" id="fastpath_form" OnSubmit="ajaxShowTimer(this, 'timerReloader', _('message apply'), 5); clearInterval(account_interval);">
					<table class="form">
						<col style="width: 40%"/>
						<col style="width: 60%"/>
						<tbody>
							<tr>
								<td class="title" colspan="2" id="accountSettings">IP Accounting Settings</td>
							</tr>
							<tr>
								<td class="head" id="accountIPT" style="width:40%">IPT accounting</td>
								<td>
									<select name="iptEnable" class="mid" onChange="accountChange();">
										<option value="0" id="accountDisable">Disable</option>
										<option value="1" id="accountEnable">Enable</option>
									</select>
								</td>
							</tr>
						</tbody>
					</table>
					<table class="buttons">
						<tr>
							<td>
								<input type="submit" class="normal" id="accountApply" value="Apply">&nbsp;&nbsp;
								<input type="button" class="normal" id="accountCancel" value="Cancel" onClick="window.location.reload();">&nbsp;&nbsp;
								<input type="button" class="normal" id="accountReset"  value="Reset"  onClick="resetValues(this.form);">
								<input type="hidden" value="0" name="reset">
								<input type="hidden" value="0" name="resetCounters">
							</td>
						</tr>
					</table>
					<br>
					<div id="accountStatTable">
						<br>
						<table class="form">
						<tbody>
							<tr>
								<td class="title" id="accountTableTitle"></td>
							</tr>
							<tr>
								<td style="text-align: left;" id="accountTableNoStatistic"></td>
							</tr>
						</tbody>
					</table>
					</div>
					<table class="buttons" id="accountStatButton">
						<tr>
							<td>
								<input type="button" class="normal" id="accountResetCounters" onClick="resetAccountCounters();">
							</td>
						</tr>
					</table>
					</form>
					<div class="whitespace">&nbsp;</div>
				</td>
			</tr>
		</table>
	</body>
</html>
