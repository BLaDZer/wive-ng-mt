<!DOCTYPE html>
<html>
<head>
<title>Create Directory</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");
var part_count = 0;

function initTranslation()
{
	var e = document.getElementById("adddirName");
	e.innerHTML = _("smb adddir name");

	e = document.getElementById("adddirApply");
	e.value = _("usb apply");
	e = document.getElementById("adddirCancel");
	e.value = _("usb cancel");
}

function initValue()
{
	initTranslation();
}

function checkData()
{
	// alert("part_count: " +part_count);
	if (document.disk_adddir.adddir_name.value == "")
	{
		alert('Please specify Directory Name');
		document.disk_adddir.adddir_name.focus();
		document.disk_adddir.adddir_name.select();
		return false;
	}
	else if(document.disk_adddir.adddir_name.value.match(/[`~!@#$%^&*\()+\|{}\[\]:;\"\'<,>.\/\\?]/)) 
	{
		alert("Don't input /[ `~!@#$%^&*\()+\|{}\[\]:;\"\'<,>.\/\\?]/");
		return false;
	}

	if (part_count <= 0)
	{
		alert("No Partition");
		return false;
	}
	else if (part_count == 1)
	{
		if (document.disk_adddir.disk_part.checked == false)
		{
			alert("Please choose one option");
			return false;
		}
	}
	else if (part_count > 1)
	{
		for(i=0;i<part_count;i++)
		{
			if (document.disk_adddir.disk_part[i].checked == true)
				break;
		}
		if (i == part_count)
		{
			alert("Please choose one option");
			return false;
		}
	}

	return true;
}

function addDirClose()
{
	opener.location.reload();
}

function submit_apply()
{
	if (checkData() == true)
	{
		document.disk_adddir.hiddenButton.value = "add";
		document.disk_adddir.submit();
		opener.location.reload();
	}
}
</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue()" onUnload="addDirClose()">
<table class="body">
  <tr>
    <td><form method=post name="disk_adddir" action="/goform/storageDiskAdm">
        <input type="hidden" name="hiddenButton" value="">
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" id="adddirName">Directory Name</td>
            <td><input type=text name=adddir_name size=16 maxlength=16 value=""></td>
          </tr>
        </table>
        <hr />
        <br />
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="3" id="adddirPart">Partition</td>
          </tr>
          <tr>
            <td bgcolor="#E8F8FF" width=15px>&nbsp;</td>
            <td align="center" bgcolor="#E8F8FF" width=150px id="adddirPartition">Partition</td>
            <td align="center" bgcolor="#E8F8FF" id="adddirPath">Path</td>
          </tr>
          <% ShowPartition(); %>
          <script language="JavaScript" type="text/javascript">
part_count = parseInt('<% getCount(1, "AllPart"); %>');
</script>
        </table>
        <hr />
        <br />
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="center">
            <td><input type=button style="{width:120px;}" value="Apply" id="adddirApply" onClick="submit_apply()">
              &nbsp; &nbsp;
              <input type=reset  style="{width:120px;}" value="Cancel" id="adddirCancel" onClick="window.close()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
