<html>
<head>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link href="/style/normal_ws.css" rel="stylesheet" type="text/css">
<title>Ralink Wireless Access Point</title>

<script language="JavaScript" type="text/javascript">
var wsc_function_enable = "<% getWlanWscConfigModeASP(); %>";
var wsc_configured = "<% getWlanWscConfiguredASP(); %>";
var wsc_use_proxy = "<% getWlanWscUseProxyASP(); %>"; 
var wsc_use_internal_reg = "<% getWlanWscUseInternalRegASP(); %>";
	
function initValue()
{
	document.getElementById("div_wsc_configure").style.visibility = "hidden";
	document.getElementById("div_wsc_configure").style.display = "none";
	document.wireless_simple_config_setup.wsc_proxy.disabled = true;
	document.wireless_simple_config_setup.wsc_reg.disabled = true;

	if (1*wsc_function_enable != 0)
	{
		document.getElementById("div_wsc_configure").style.visibility = "visible";
		document.getElementById("div_wsc_configure").style.display = "block";
		document.wireless_simple_config_setup.wsc_function_enable.options.selectedIndex = 0;
	}
	else
	{
		document.wireless_simple_config_setup.wsc_function_enable.options.selectedIndex = 1;
	}

	if (1*wsc_use_proxy == 0)
		document.wireless_simple_config_setup.wsc_proxy[0].checked = true;
	else
		document.wireless_simple_config_setup.wsc_proxy[1].checked = true;

	if (1*wsc_use_internal_reg == 0)
		document.wireless_simple_config_setup.wsc_reg[0].checked = true;
	else
		document.wireless_simple_config_setup.wsc_reg[1].checked = true;
}

function WscFunctionEnable()
{
	document.getElementById("div_wsc_configure").style.visibility = "hidden";
	document.getElementById("div_wsc_configure").style.display = "none";
	document.wireless_simple_config_setup.wsc_proxy.disabled = true;
	document.wireless_simple_config_setup.wsc_reg.disabled = true;

	if (document.wireless_simple_config_setup.wsc_function_enable.options.selectedIndex == 0)
	{
		document.getElementById("div_wsc_configure").style.visibility = "visible";
		document.getElementById("div_wsc_configure").style.display = "block";
	}
}

function check_configured()
{
	if (wsc_configured == 0)
		document.write("Unconfigure");
	else
		document.write("Configured");
}

function checkValue()
{
	return true;
}
</script>
</head>

<body leftmargin="50" topmargin="15" marginwidth="0" marginheight="0" onLoad="initValue()">
<form method=post name=wireless_simple_config_setup action=/goform/wireless_simple_config_setup onSubmit="return checkValue()">
	<span>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;The AP supports Simple Config settings for your network.</span>

	<br><br><br>
	<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
		<tr align="center">
			<td><b>Wi-Fi Protected Setup</b></td>
			<td>
				<select name="wsc_function_enable" size="1" style="{font-family:arial; width:160px; color:#003366; font-weight: bold; font-size: 9pt;}" onChange="WscFunctionEnable()">
					<option value=1 selected>Enable</option>
					<option value=0>Disable</option>

				</select>
			</td>
		</tr>
	</table>
	<br>
	<hr width="90%" align="left">
	<br>
	<table id="div_wsc_configure" name="div_wsc_configure" width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
	<br>

		<tr> 
			<td bgcolor="#2C5EA4" colspan="3"><font color="#FFFFFF"><b>WPS Current Status : &nbsp;&nbsp;<script>check_configured();</script>
																	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
																	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
																	&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
																	PinCode : <% getWlanWscPinCodeASP(); %></b></font></td>
		</tr>
		<tr> 
			<td width="45%" bgcolor="#E8F8FF" nowrap>Enable Proxy</td>

			<td bgcolor="#FFFFFF" colspan="2"><font color="#003366" face=arial><b>
				<input type=radio name=wsc_proxy value="0">Disabled &nbsp;
				<input type=radio name=wsc_proxy value="1" checked>Enabled
			</b></font></td>
		</tr>
		<tr> 
			<td width="45%" bgcolor="#E8F8FF" nowrap>Use Internal Registrar</td>
			<td bgcolor="#FFFFFF" colspan="2"><font color="#003366" face=arial><b>
				<input type=radio name=wsc_reg value="0">Disabled &nbsp;

				<input type=radio name=wsc_reg value="1" checked>Enabled
			</b></font></td>
		</tr>
	</table>
	<br>
	<table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
		<tr align="center">
			<td>
				<input type=submit style="{width:120px;}" value="Apply"> &nbsp; &nbsp; <input type=reset  style="{width:120px;}" value="Cancel" onClick="window.location.reload()">

			</td>
		</tr>
	</table>
</form>


<!--   ----------------- Out of box --------------------------- !!!!!  -->
<form method=post name=wireless_simple_config_reset action=/goform/wireless_simple_config_reset>
  <br><br>
  <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">

    <tr> 
      <td bgcolor="#2C5EA4"colspan="2"><font color="#FFFFFF"><b>Reset to Out-Of-Box Configuration</b></font></td>
    </tr>
    <tr> 
      <td bgcolor="#E8F8FF" nowrap>Reset</td>
      <td bgcolor="#FFFFFF">
        <input type=radio name=reset_simple_config value="1">Yes &nbsp;
        <input type=radio name=reset_simple_config value="0" checked>No
      </td>

    </tr>
  </table>
  <br>
  <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
    <tr align="center">
      <td>
        <input type=submit style="{width:120px;}" value="Apply"> &nbsp; &nbsp; <input type=reset  style="{width:120px;}" value="Cancel" onClick="window.location.reload()">

      </td>
    </tr>
  </table>
</form>
</body>
</html>

</body>
</html>
