<!DOCTYPE html>
<html>
<head>
<title>MAC Filtering Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("firewall");
Butterlate.setTextDomain("buttons");

function deleteClick()
{
    return true;
}


function checkRange(str, num, min, max)
{
	d = atoi(str,num);
	if(d > max || d < min)
		return false;
	return true;
}


function atoi(str, num)
{
	i=1;
	if(num != 1 ){
		while (i != num && str.length != 0){
			if(str.charAt(0) == '.'){
				i++;
			}
			str = str.substring(1);
		}
	  	if(i != num )
			return -1;
	}

	for(i=0; i<str.length; i++){
		if(str.charAt(i) == '.'){
			str = str.substring(0, i);
			break;
		}
	}
	if(str.length == 0)
		return -1;
	return parseInt(str, 10);
}


function checkIPAddr(field)
{
    if(field.value == ""){
        alert(_("dmz ip empty"));
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if ( isAllNum(field.value) == 0) {
        alert(_("dmz number"));
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

    if( (!checkRange(field.value,1,0,255)) ||
        (!checkRange(field.value,2,0,255)) ||
        (!checkRange(field.value,3,0,255)) ||
        (!checkRange(field.value,4,1,254)) ){
        alert(_("dmz invalid ip"));
        field.value = field.defaultValue;
        field.focus();
        return false;
    }

   return true;
}


function formCheck()
{
	if(!document.DMZ.DMZEnabled.options.selectedIndex){
		// user choose disable
		return true;
	}

	if ( !checkIPAddr(document.DMZ.DMZIPAddress) ) {
		document.DMZ.DMZIPAddress.focus();
		return false;
	}

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

	_TRV("dmzApply", "button apply");
	_TRV("dmzReset", "button reset");
}

function pageInit()
{
	var form = document.DMZ;

	initTranslation();
	var dmz_loopback = "<% getCfgZero(1, "DMZNATLoopback"); %>";

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
<body onLoad="pageInit()">
<table class="body">
  <tr>
    <td><h1 id="dmzTitle"> DMZ Settings </h1>
      <% checkIfUnderBridgeModeASP(); %>
      <p id="dmzIntroduction"> Here you can setup the De-Militarized Zone (DMZ) to separate your external services from the rest of LAN.</p>
      <hr>
      <form method="POST" name="DMZ" action="/goform/DMZ">
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
            <td><input type="submit" class="normal" value="Apply" id="dmzApply" name="addDMZ" onClick="return formCheck()">
              <input type="button" class="normal" value="Reset" id="dmzReset" name="reset" onClick="resetClick(this.form);">
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
