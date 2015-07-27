<!DOCTYPE html>
<html>
<head>
<title>Wide Area Network (WAN) Settings</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/share.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("buttons");

function connectionTypeSwitch(form)
{
	var conn_type = form.connectionType.value;
	displayElement('staticDHCP', conn_type == 'STATIC');
	displayElement(['dhcpReqIPRow', 'dhcpVendorRow'], conn_type == 'DHCP');
	displayElement('staticDNSAssignRow', conn_type != 'ZERO');

	if (conn_type == 'STATIC')
		form.wStaticDnsEnable.checked = true;

	dnsSwitchClick(form);
}

function CheckValue(form)
{
	var c_type = form.connectionType.value;

	if (c_type == 'STATIC') // STATIC
	{
		if (!validateIP(form.staticIp, true))
		{
			form.staticIp.focus();
			return false;
		}
		if (!validateIPMask(form.staticNetmask, true))
		{
			form.staticNetmask.focus();
			return false;
		}
		if (form.staticGateway.value != '')
			if (!validateIP(form.staticGateway, true))
			{
				form.staticGateway.focus();
				return false;
			}
	}
	else if (c_type == 'DHCP')
	{
		if (form.dhcpReqIP.value != '')
			if (!validateIP(form.dhcpReqIP, true))
			{
				form.dhcpReqIP.focus();
				return false;
			}
	}
	else if (c_type == 'ZERO')
		form.wStaticDnsEnable.checked = true;

	if (form.wStaticDnsEnable.checked)
	{
		if (!validateIP(form.staticPriDns, true))
		{
			form.staticPriDns.focus();
			return false;
		}

		if (form.staticSecDns.value != '')
			if (!validateIP(form.staticSecDns, true))
			{
				form.staticSecDns.focus();
				return false;
			}
	}

	// Validate MTU
	if (!validateNum(form.wan_mtu.value))
	{
		alert(_("inet invalid mtu"));
		form.wan_mtu_type.focus();
		return false;
	}

	if (!validateMAC(form.wanMac.value))
	{
		form.wanMac.focus();
		return false;
	}

	var wan_mtu = form.wan_mtu.value * 1;
	if ((wan_mtu < 80) && (wan_mtu != 0))
	{
		alert(_("inet invalid mtu"));
		return false;
	}

	return true;
}

function initTranslation()
{
	_TR("wTitle", "wan title");
	_TR("wIntroduction", "wan introduction");

	_TR("wConnectionType", "wan connection type");
	_TR("wConnection", "wan connection");
	_TR("wConnTypeStatic", "wan connection type static");
	_TR("wConnTypeDhcp", "wan connection type dhcp");

	_TR("wStaticMode", "wan static mode");
	_TR("wStaticIp", "inet ip");
	_TR("wStaticNetmask", "inet netmask");
	_TR("wStaticGateway", "inet gateway");
	_TR("wStaticDns", "wan static dns");
	_TR("wStaticPriDns", "inet pri dns");
	_TR("wStaticSecDns", "inet sec dns");

	_TR("wDhcpMode", "wan dhcp mode");
	_TR("wAdditionalOptions", "wan additional options");
	_TR("wReqFromDHCP", "wan request from dhcp");
	_TR("wDHCPVendorClass", "wan dhcp vendor class");
	_TR("wMTU", "wan mtu");
	_TR("wNatEnabled", "wan nat enabled");
	_TR("wMacAddress", "inet mac");
	_TR("wMacAddr", "wan mac");

	_TRV("wApply", "button apply");
	_TRV("wCancel", "button cancel");
	_TRV("WanMacRestore", "button restore factory");
}

function initValue()
{
	var mode = "<% getCfgGeneral(1, "wanConnectionMode"); %>";
	var nat = "<% getCfgZero(1, "natEnabled"); %>";
	var opmode = "<% getCfgZero(1, "OperationMode"); %>";
	var static_dns = "<% getCfgZero(1, "wan_static_dns"); %>";
	var wan_mtu = defaultNumber("<% getCfgGeneral(1, "wan_manual_mtu"); %>", '0');
	var form = document.wanCfg;

	form.natEnabled.checked = (nat == "1");
	var element = document.getElementById("natRowDisplay");
	if (element!=null)
		element.style.display = (opmode != "0") ? "" : "none";

	initTranslation();

	form.connectionType.value = mode;
	form.wStaticDnsEnable.checked = (static_dns == "on");

	/* Check if option was set */
	form.wan_mtu.value = wan_mtu;
	for (var i=0; i < form.wan_mtu_type.options.length; i++)
		if (form.wan_mtu_type.options[i].value == wan_mtu)
		{
			form.wan_mtu_type.value = form.wan_mtu_type.options[i].value;
			break;
		}

	connectionTypeSwitch(form);
	wanMtuChange(form);
}

function dnsSwitchClick(form)
{
	displayElement( ['priDNSrow', 'secDNSrow' ],
		(form.wStaticDnsEnable.checked) || (form.connectionType.value == 'ZERO'));
}

function wanMtuChange(form)
{
	if (form.wan_mtu_type.value == '1')
	{
		form.wan_mtu.style.display = '';
		form.wan_mtu.setAttribute('class', 'half');
		form.wan_mtu_type.setAttribute('class', 'half');
	}
	else
	{
		form.wan_mtu_type.setAttribute('class', 'mid');
		form.wan_mtu.style.display = 'none';
		form.wan_mtu.value = form.wan_mtu_type.value;
	}
}

function submitForm(form) {
	if (form.wanMac.value != "<% getCfgGeneral(1, "WAN_MAC_ADDR"); %>") {
		if (!ajaxPostForm(_('wan reboot confirm'), form, 'MACReloader', _("message config"), ajaxShowProgress)) {
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
<form name="rebootForm" style="display: none;" method="GET" action="/goform/restoremac" >
  <iframe id="rebootReloader" name="rebootReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
</form>
<table class="body">
  <tr>
    <td><h1 id="wTitle"></h1>
      <p id="wIntroduction"></p>
      <hr>
      <form method="POST" name="wanCfg" action="/goform/setWan" onSubmit="return CheckValue(this);">
      	<table class="form">
          <tr>
            <td class="title" colspan="2" id="wConnection">WAN connection type</td>
          </tr>
          <tr>
            <td class="head"><b id="wConnectionType">Connection type</b></td>
            <td><select name="connectionType" class="mid" onChange="connectionTypeSwitch(this.form);">
                <option value="STATIC" id="wConnTypeStatic" selected="selected">Static Mode (fixed IP)</option>
                <option value="DHCP" id="wConnTypeDhcp">DHCP (Auto Config)</option>
                <option value="ZERO" id="wConnTypeDhcp">Zeroconf</option>
              </select></td>
          </tr>
        </table>
        <!-- ================= STATIC Mode ================= -->
        <table id="staticDHCP" class="form">
          <tr>
            <td class="title" colspan="2" id="wStaticMode">Static Mode</td>
          </tr>
          <tr>
            <td class="head" id="wStaticIp">IP Address</td>
            <td><input name="staticIp" class="mid" value="<% getCfgZero(1, "wan_ipaddr"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="wStaticNetmask">Subnet Mask</td>
            <td><input name="staticNetmask" class="mid" value="<% getCfgZero(1, "wan_netmask"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="wStaticGateway">Default Gateway</td>
            <td><input name="staticGateway" class="mid" value="<% getCfgZero(1, "wan_gateway"); %>"></td>
          </tr>
        </table>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="wAdditionalOptions">Additional Options</td>
          </tr>
          <tr id="dhcpReqIPRow">
            <td class="head" id="wReqFromDHCP">Request IP from DHCP (optional)</td>
            <td><input name="dhcpReqIP" class="mid" value="<% getCfgGeneral(1, "dhcpRequestIP"); %>"></td>
          </tr>
          <tr id="dhcpVendorRow">
            <td class="head" id="wDHCPVendorClass">Type of device (optional)</td>
            <td><input name="dhcpVendorClass" class="mid" value="<% getCfgGeneral(1, "dhcpVendorClass"); %>"></td>
          </tr>
          <tr>
            <td class="head" id="wMTU">WAN MTU</td>
            <td><input name="wan_mtu" type="text" class="half" style="display:none;">
              <select name="wan_mtu_type" onChange="wanMtuChange(this.form);" class="half">
                <option value="0">AUTO</option>
                <option value="1" selected="selected">Custom</option>
                <option value="1500">1500</option>
                <option value="1492">1492</option>
                <option value="1460">1460</option>
                <option value="1440">1440</option>
                <option value="1400">1400</option>
                <option value="1300">1300</option>
                <option value="1200">1200</option>
                <option value="1000">1000</option>
              </select></td>
          </tr>
          <tr id="staticDNSAssignRow">
            <td class="head" id="wStaticDns">Assign static DNS Server</td>
            <td><input name="wStaticDnsEnable" type="checkbox" onClick="dnsSwitchClick(this.form);" ></td>
          </tr>
          <tr id="priDNSrow" style="display:none;" >
            <td class="head" id="wStaticPriDns">Primary DNS Server</td>
            <td><input name="staticPriDns" class="mid" value="<% getDns(1); %>"></td>
          </tr>
          <tr id="secDNSrow" style="display:none;" >
            <td class="head" id="wStaticSecDns">Secondary DNS Server</td>
            <td><input name="staticSecDns" class="mid" value="<% getDns(2); %>"></td>
          </tr>
          <tr id="natRowDisplay">
            <td class="head" id="wNatEnabled">Enable NAT</td>
            <td><input name="natEnabled" type="checkbox"></td>
          </tr>
        </table>
        <br>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="wMacAddress">MAC address</td>
          </tr>
          <tr id="MACrow">
            <td class="head" id="wMacAddr">WAN MAC address</td>
            <td>
		<input name="wanMac" id="wanMac" class="mid" value="<% getCfgGeneral(1, "WAN_MAC_ADDR"); %>">
		<input type="button" value="Restore Factory" id="WanMacRestore" name="restoremac" onClick="ajaxPostForm(_('wan reboot confirm'), document.rebootForm, 'rebootReloader', _('message config'), ajaxShowProgress);">
	    </td>
          </tr>
        </table>
        <br>
        <table class="buttons">
          <tr>
            <td><input type="button" class="normal" value="Apply" id="wApply" onClick="submitForm(this.form);">
              &nbsp;&nbsp;
              <input type="reset" class="normal" value="Cancel" id="wCancel" onClick="window.location.reload();">
              <input type="hidden" value="/internet/wan.asp" name="submit-url">
              <input type="hidden" value="1" name="reboot">
              <iframe id="MACReloader" name="MACReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
            </td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
