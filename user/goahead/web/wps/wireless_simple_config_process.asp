<html>
<head>

<% wscRedirectUIPageASP(); %>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">

<link href="css.css" rel="stylesheet" type="text/css">
<title>Wireless Access Point</title>

<script language="JavaScript" type="text/javascript">

function initValue()
{
	var current_status = <% wscCurrentStatusASP(); %>; 
	var wsc_configured = <% getWlanWscConfiguredASP(); %>;

	document.getElementById("div_wsc_process").style.visibility = "hidden";
	document.getElementById("div_wsc_process").style.display = "none";

	if (1*wsc_configured == 1)
	{
		if (1*current_status == 1 || 1*current_status == 34) /* Idle || Configured */
		{
			opener.location.href = "/wps/wps.asp";
			window.close();
		}
		else if (1*current_status == 2)
		{
			alert('WiFi Protected Setup Process Failed 1');
			window.close();
		}
		else
		{
			document.getElementById("div_wsc_process").style.visibility = "visible";
			document.getElementById("div_wsc_process").style.display = "block";
		}
	}
	else	/* if (wsc_configured == 0) */
	{
		if (1*current_status != 2)
		{
			if (1*current_status == 1)
			{
				alert('WiFi Protected Setup Process Failed 2');
				window.close();
			}
			else
			{
				document.getElementById("div_wsc_process").style.visibility = "visible";
				document.getElementById("div_wsc_process").style.display = "block";
			}
		}
	}
}
</script>
</head>
<body background="ralink-bg.gif" leftmargin="50" topmargin="25" marginwidth="0" marginheight="0" onLoad="initValue()">
  <table id="div_wsc_process" name="div_wsc_process" width="90%" border="0" cellspacing="0" cellpadding="0" vspace="0" hspace="0" bordercolor="#9BABBD">
    <tr>
      <th height=250>
        <font size=4 face=Verdana>System Processing...Please wait... <p>
      </th>
    </tr>
  </table>

  <table id="div_wsc_process_info" name="div_wsc_process_info" width="90%" border="0" cellspacing="0" cellpadding="0" vspace="0" hspace="0" bordercolor="#9BABBD">
    <tr>
      <th height=250>
        <font size=2 face=Verdana> <% getWscStatusASP(); %> <p>
      </th>
    </tr>
  </table>

</body>
</html>
