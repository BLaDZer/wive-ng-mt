<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/share.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<title>Dynamic DNS</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("admin");
Butterlate.setTextDomain("services");

function DDNSFormCheck()
{
	if (document.DDNS.DDNSProvider.value != "none" && 
		(document.DDNS.Account.value == "" ||
		 document.DDNS.Password.value == "" ||
		 document.DDNS.DDNS.value == ""))
	{
		alert("Please specify login, password, and desired dynamic name.");
		return false;
	}

	return true;
}

function DDNSupdateState()
{
	var form = document.DDNS;
	
	if (form.DDNSProvider.options.selectedIndex != 0)
	{
		enableTextField(form.Account);
		enableTextField(form.Password);
		enableTextField(form.DDNS);
	}
	else
	{
		disableTextField(form.Account);
		disableTextField(form.Password);
		disableTextField(form.DDNS);
	}
}

function initTranslation()
{
	_TR("manTitle", "services ddns title");
	_TR("manIntroduction", "services ddns introduction");

	_TR("manDdnsSet", "man ddns setting");
	_TR("DdnsProvider", "man ddns provider");
	_TR("manDdnsNone", "man ddns none");
	_TR("manDdnsAccount", "man ddns account");
	_TR("manDdnsPasswd", "man ddns passwd");
	_TR("manDdns", "man ddns");

	_TRV("manDdnsApply", "admin apply");
	_TRV("manDdnsCancel", "admin cancel");
}

function initValue()
{
	var ddnsb = "<% getDDNSBuilt(); %>";
	var form = document.DDNS;

	if (ddnsb == "1")
	{
		var ddns_provider = "<% getCfgGeneral(1, "DDNSProvider"); %>";

		document.getElementById("div_ddns").style.visibility = "visible";
		document.getElementById("div_ddns").style.display = '';
		document.getElementById("div_ddns_submit").style.visibility = "visible";
		document.getElementById("div_ddns_submit").style.display = '';

		form.Account.disabled = false;
		form.Password.disabled = false;
		form.DDNS.disabled = false;

		if (ddns_provider == "none")
			form.DDNSProvider.options.selectedIndex = 0;
		else if (ddns_provider == "dyndns.org")
			form.DDNSProvider.options.selectedIndex = 1;
		else if (ddns_provider == "freedns.afraid.org")
			form.DDNSProvider.options.selectedIndex = 2;
		else if (ddns_provider == "zoneedit.com")
			form.DDNSProvider.options.selectedIndex = 3;
		else if (ddns_provider == "no-ip.com")
			form.DDNSProvider.options.selectedIndex = 4;

		DDNSupdateState();
	}
	else
	{
		document.getElementById("div_ddns").style.visibility = "hidden";
		document.getElementById("div_ddns").style.display = "none";
		document.getElementById("div_ddns_submit").style.visibility = "hidden";
		document.getElementById("div_ddns_submit").style.display = "none";

		form.Account.disabled = true;
		form.Password.disabled = true;
		form.DDNS.disabled = true;
	}
}

</script>
</head>
<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="manTitle">DDNS Settings</h1>
      <p id="manIntroduction">Here you can configure Dynamic DNS settings.</p>
      <p>IMPORTANT: if you use freedns.afraid.org - must input key in login and password fields.</p>
      <hr>
      <!-- ================= DDNS  ================= -->
      <form method="post" name="DDNS" action="/goform/DDNS">
        <table id="div_ddns" class="form">
          <tr>
            <td class="title" colspan="2" id="manDdnsSet">DDNS Settings</td>
          </tr>
          <tr>
            <td class="head" id="DdnsProvider">Dynamic DNS Provider</td>
            <td><select onChange="DDNSupdateState()" name="DDNSProvider">
                <option value="none" id="manDdnsNone"> None </option>
                <option value="dyndns.org"> dyndns.org </option>
                <option value="freedns.afraid.org"> freedns.afraid.org </option>
                <option value="zoneedit.com"> www.zoneedit.com </option>
                <option value="no-ip.com"> www.no-ip.com </option>
              </select></td>
          </tr>
          <tr>
            <td class="head" id="manDdnsAccount">Login</td>
            <td><input size="16" name="Account" value="<% getCfgGeneral(1, "DDNSAccount"); %>" type="text"></td>
          </tr>
          <tr>
            <td class="head" id="manDdnsPasswd">Password</td>
            <td><input size="16" name="Password" value="<% getCfgGeneral(1, "DDNSPassword"); %>" type="password"></td>
          </tr>
          <tr>
            <td class="head" id="manDdns">Dynamic Name</td>
            <td><input size="32" name="DDNS" value="<% getCfgGeneral(1, "DDNS"); %>" type="text"></td>
          </tr>
        </table>
        <table id="div_ddns_submit" class="buttons">
          <tr>
            <td><input type="hidden" name="submit-url" value="/services/ddns.asp" >
              <input type="submit" class="normal" value="Apply" id="manDdnsApply" onClick="return DDNSFormCheck()">
              &nbsp; &nbsp;
              <input type="reset" class="normal" value="Cancel" id="manDdnsCancel" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
