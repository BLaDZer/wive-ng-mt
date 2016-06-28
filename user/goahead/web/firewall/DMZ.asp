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
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("firewall");
Butterlate.setTextDomain("buttons");

function formCheck(form)
{
	if(!document.DMZ.DMZEnabled.options.selectedIndex){
		// user choose disable
		ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
		return true;
	}

	if ( !validateIP(document.DMZ.DMZIPAddress, 0) ) {
		document.DMZ.DMZIPAddress.focus();
		return false;
	}
	ajaxShowTimer(form, 'timerReloader', _('message apply'), 5);
	return true;
}

function disableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = true;
  else {
    field.oldOnFocus = field.onfocus;
    field.onfocus = skip;
  }
}

function enableTextField (field)
{
  if(document.all || document.getElementById)
    field.disabled = false;
  else {
    field.onfocus = field.oldOnFocus;
  }
}

function initTranslation()
{
	_TR("dmzTitle", "dmz title");
	_TR("dmzIntroduction", "dmz introduction");
	_TR("dmzSetting", "dmz title");
	_TR("dmzSet", "dmz setting");
	_TR("dmzDisable", "button disable");
	_TR("dmzEnable", "button enable");
	_TR("dmzDisable2", "button disable");
	_TR("dmzEnable2", "button enable");
	_TR("dmzIPAddr", "dmz ipaddr");
  _TR("bridge_warning", "firewall bridge warning");

	_TRV("dmzApply", "button apply");
	_TRV("dmzReset", "button reset");
}

function pageInit()
{
	var form = document.DMZ;

	initTranslation();
	var dmz_loopback = "<% getCfgZero(1, "DMZNATLoopback"); %>";
	var opmode = '<% getCfgZero(1, "OperationMode"); %>';

	displayElement('bridge_warning', opmode == '0'); // bridge mode

	form.dmzLoopback.value = dmz_loopback;
	dmzEnableSwitch(form);
}

function dmzEnableSwitch(form)
{
	enableElements([form.DMZIPAddress, form.dmzLoopback], form.DMZEnabled.value == '1');
	displayElement([ 'dmzAdress', 'dmzLoopback' ], form.DMZEnabled.value == '1');
}

function dmzLoopbackWarning(element)
{
	if (element.value == '1')
	{
		if (!confirm(_("dmz confirm")))
			element.value='0';
	}
}

function resetClick(form) {
    form.reset.value = "1";
    form.submit();
}
</script>
</head>
<body bgcolor="#FFFFFF" onLoad="pageInit()">
<table class="body">
  <tr>
    <td><h1 id="dmzTitle"> DMZ Settings </h1>
      <div style="display:none;" id="bridge_warning">
        <p><b>Warning:</b> The current operation mode is "Bridge mode" and these settings may not be functional.</p>
      </div>
      <p id="dmzIntroduction"> Here you can setup the De-Militarized Zone (DMZ) to separate your external services from the rest of LAN.</p>
      <hr>
      <form method="POST" name="DMZ" action="/goform/DMZ" OnSubmit="return formCheck(this);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="dmzSetting">DMZ Settings</td>
          </tr>
          <tr>
            <td class="head" id="dmzSet"> DMZ Settings </td>
            <td><select onChange="dmzEnableSwitch(this.form);" name="DMZEnabled" class="mid">
                <option value="0" <% getDMZEnableASP(0); %> id="dmzDisable">Disable</option>
                <option value="1" <% getDMZEnableASP(1); %> id="dmzEnable">Enable</option>
              </select></td>
          </tr>
          <tr id="dmzAdress">
            <td class="head" id="dmzIPAddr"> DMZ IP Address </td>
            <td><input type="text" class="mid" name="DMZIPAddress" value='<% showDMZIPAddressASP(); %>' ></td>
          </tr>
          <tr id="dmzLoopback">
            <td class="head" id="ldmzLoopback">DMZ NAT loopback</td>
            <td><select name="dmzLoopback" class="mid" onChange="dmzLoopbackWarning(this);">
                <option value="0" id="dmzDisable2">Disable</option>
                <option value="1" id="dmzEnable2">Enable</option>
              </select></td>
          </tr>
        </table>
        <table class="buttons">
          <tr>
            <td><input type="submit" class="normal" value="Apply" id="dmzApply" name="addDMZ"">
              <input type="button" class="normal" value="Reset" id="dmzReset" name="reset_button" onClick="resetClick(this.form);">
              <input type="hidden" name="submit-url" value="/firewall/DMZ.asp" >
              <input type="hidden" name="reset" value="0">
            </td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
