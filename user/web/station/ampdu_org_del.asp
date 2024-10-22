<!DOCTYPE html>
<html>
<head>
<title>Wireless Station AMPDU Originator Delete</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/js/ajax.js"></script>
<script language="JavaScript" type="text/javascript">

function initValue()
{
	<% setRefreshSta11nConfiguration(); %>
	document.getElementById("div_mac_addr").style.visibility = "hidden";
	document.getElementById("div_mac_addr").style.display = "none";
	document.sta_org_del.mac.disabled = true;

	document.sta_org_del.mpdu_apply.disabled = true;
	var str = document.sta_org_del.selectedbssid.value;
	if (str.length > 0)
	{
		document.getElementById("div_mac_addr").style.visibility = "visible";
		document.getElementById("div_mac_addr").style.display = style_display_on();
		document.sta_org_del.mac.disabled = false;
	}
}

function style_display_on()
{
	if (window.ActiveXObject) { // IE
		return "block";
	}
	else if (window.XMLHttpRequest) { // Mozilla, Safari,...
		return "table-row";
	}
}

function submit_apply()
{
	document.sta_org_del.submit();
	opener.location.reload();
	window.close();
}

function showSelectBssid()
{
	if (document.sta_org_del.selectbssid.value)
	{
		opener.showCwinSelectedBssid();
	}
}

function selectedBSSID()
{
	document.sta_org_del.mpdu_apply.disabled = true;
	if (document.sta_org_del.mac.checked)
		document.sta_org_del.mpdu_apply.disabled = false;
}
</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1>Delete AMPDU Originator</h1>
      <hr />
      <form method=post name=sta_org_del action="/goform/setStaOrgDel">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <input type=hidden name=selectedbssid>
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="2">MPDU Aggregation</td>
          </tr>
          <tr>
            <td class="head">TID</td>
            <td><select name="tid" size="1">
                <option value=0 selected>0</option>
                <option value=1 >1</option>
                <option value=2 >2</option>
                <option value=3 >3</option>
                <option value=4 >4</option>
                <option value=5 >5</option>
                <option value=6 >6</option>
                <option value=7 >7</option>
              </select></td>
          </tr>
        </table>
        <br />
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="2">Connected BSSIDs</td>
          </tr>
          <tr>
            <td class="head">BSSID</td>
          </tr>
          <tr id="div_mac_addr" name="div_mac_addr">
            <td><input type=checkbox name="mac" onclick=selectedBSSID()>
              <script>
	opener.getBssid();
	document.write(document.sta_org_del.selectedbssid.value);
      </script></td>
          </tr>
        </table>
        <br>
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="center">
            <td><input type="button" name="mpdu_apply" style="{width:120px;}" value="Apply" onClick="submit_apply();">
              &nbsp; &nbsp;
              <input type="button" style="{width:120px;}" value="Cancel" onClick="window.close()">
              &nbsp; &nbsp; </td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
