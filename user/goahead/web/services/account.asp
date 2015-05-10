<!DOCTYPE html>
<html>
<head>
<title>Accounting</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="Javascript" type="text/javascript">
function resetClick(form)
{
	form.reset.value = 1;
	form.submit();
}

function initValue()
{
	var form = document.formIptAccounting;

	form.iptEnable.value = defaultNumber("<% getCfgZero(1, "ipt_account"); %>", '0');

	var nat_fp = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");
	displayElement('fastpath_warning', (nat_fp == '1') || (nat_fp == '2') || (nat_fp == '3'));
	displayElement('fastpath_form', (nat_fp == '0'));
}
</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1>Accounting</h1>
      <p>Here you can configure Accounting Control.</p>
      <hr>
      <!-- IP Accounting -->
      <h2>IP Accounting</h2>
      <div style="display:none;" id="fastpath_warning">
        <p><span style="color: #ff0000;"><b>CAUTION!&nbsp;</b></span> <b>NAT offload mode</b> option is turned on.</p>
        <p>Due to some technical reasons it's not possible to gather correct statistics for NAT offload mode mode now.</p>
        <p>That's why IPT accounting configuration ability is now locked.</p>
        <p>To get correct statistics you need to shut down <b>NAT offload mode</b> option on the <a href="/services/misc.asp#nat_fastpath_ref">MISC&nbsp;Services</a> configuration page.</p>
        <p>Please note that turning off <b>NAT offload mode</b> will increase CPU usage up to 50%.</p>
      </div>
      <form action="/goform/formIptAccounting" method="POST" name="formIptAccounting" id="fastpath_form">
        <table class="form">
          <tr>
            <td class="title" colspan="2">IP Accounting Settings</td>
          </tr>
          <tr>
            <td class="head">IPT accounting</td>
            <td><select name="iptEnable" class="half">
                <option value="0">Disable</option>
                <option value="1">Enable</option>
              </select></td>
          </tr>
          <tr>
            <td colspan="2"><input type="submit" value="Apply"></td>
          </tr>
        </table>
        <table class="form">
          <% iptStatList(); %>
        </table>
        <br>
        <input type="hidden" value="/services/account.asp" name="submit-url">
        <input type="button" class="normal" value="Refresh" onClick="window.location.reload();">
        <input type="hidden" value="0" name="reset">
        <input type="button" class="normal" onClick="resetClick(this.form);" value="Reset Statistics">
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
