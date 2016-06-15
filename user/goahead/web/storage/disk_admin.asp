<!DOCTYPE html>
<html>
<head>
<title>Disk Managament</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("usb");
var dir_count = 0;
var part_count = 0;

/*
function style_display_on()
{
	if (window.ActiveXObject)
	{ // IE
		return "block";
	}
	else if (window.XMLHttpRequest)
	{ // Mozilla, Safari,...
		return "table-row";
	}
}
*/

function initTranslation()
{
	var e = document.getElementById("storageDiskAdmTitle");
	e.innerHTML = _("storage disk title");

	e = document.getElementById("storageAdmUserAdd");
	e.value = _("usb add");
	e = document.getElementById("storageAdmUserDel");
	e.value = _("usb del");
}

function initValue()
{
	// initTranslation();
}

function CheckSelect()
{
	// alert("dir_count: "+dir_count);
	if (dir_count <= 0)
	{
		alert("No any option can be choosed!");
		return false;
	}
	else if (dir_count == 1)
	{
		if (document.storage_disk_adm.dir_path.checked == false)	
		{
			alert("please select one option!");
			return false;
		}
		document.storage_disk_adm.selectDirIndex.value = 0;
	}
	else
	{
		for(i=0;i<dir_count;i++)
		{
			if (document.storage_disk_adm.dir_path[i].checked == true)
			{
				document.storage_disk_adm.selectDirIndex.value = i;
				break;
			}
		}
		if (i == dir_count)
		{
			alert("please select one option!");
			return false;
		}
	}

	return true;
}

function checkData()
{
	// alert("part_count: " +part_count);
	if (part_count <= 0)
	{
		alert("No Partition");
		return false;
	}
	else if (part_count == 1)
	{
		if (document.storage_disk_adm.disk_part.checked == false)
		{
			alert("Please choose one option");
			return false;
		}
		document.storage_disk_adm.selectPartIndex.value = 0;
	}
	else if (part_count > 1)
	{
		for(i=0;i<part_count;i++)
		{
			if (document.storage_disk_adm.disk_part[i].checked == true)
			{
				document.storage_disk_adm.selectPartIndex.value = i;
				break;
			}
		}
		if (i == part_count)
		{
			alert("Please choose one option");
			return false;
		}
	}

	return true;
}

function submit_apply(parm)
{
	if (parm == "delete")
	{
		if (!CheckSelect())
		{
			window.location.reload();
			return;
		}
		document.storage_disk_adm.hiddenButton.value = parm;
		document.storage_disk_adm.submit();
	}
	else if (parm == "format")
	{
		if (checkData())
		{
			var format_ok = confirm("Formatting will erase all data on this partition! Are you sure to format it?");
			if (format_ok == true)
			{
				document.storage_disk_adm.hiddenButton.value = parm;
				document.storage_disk_adm.submit();
			}
		}
		return;
	}
	else if (parm == "remove")
	{
		var remove_ok = confirm("Are you sure to remove disk?");
		if (remove_ok == true)
		{
			document.storage_disk_adm.hiddenButton.value = parm;
			document.storage_disk_adm.submit();
		}
		return;
	}
}

function open_diskadd_window()
{
	window.open("disk_adddir.asp","storage_disk_add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=640")
}

function open_diskrepart_window()
{
	window.open("disk_part.asp","storage_disk_part","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=640")
}

</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="storageDiskAdmTitle">Disk Management</h1>
      <p id="storageDiskAdmIntroduction"></p>
      <hr />
      <form method=post name=storage_disk_adm action="/goform/storageDiskAdm">
        <input type=hidden name=hiddenButton value="">
        <input type=hidden name=selectDirIndex value="">
        <input type=hidden name=selectPartIndex value="">
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="4"><font id="storageShowDisk">Disk Display</font></td>
          </tr>
          <tr align=center>
            <td bgcolor="#E8F8FF" width=15px>&nbsp;</td>
            <td bgcolor="#E8F8FF" id="storageDirPath">Directory Path</td>
            <td bgcolor="#E8F8FF" id="storageDirPart">Partition</td>
          </tr>
          <% ShowAllDir(); %>
          <script language="JavaScript" type="text/javascript">
dir_count = parseInt('<% getCount(1, "AllDir"); %>');
</script>
        </table>
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="center">
            <td><input type="button" style="{width:80px;}" value="Add" id="storageDiskAdmAdd" onClick="open_diskadd_window()">
              &nbsp;&nbsp;
              <input type="button" style="{width:80px;}" value="Delete" id="storageDiskAdmDel" onClick="submit_apply('delete')"></td>
          </tr>
        </table>
        <hr />
        <br />
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="4"><font id="storageShowPart">Partition Display</font></td>
          </tr>
          <tr>
            <td bgcolor="#E8F8FF" width=15px>&nbsp;</td>
            <td align="center" bgcolor="#E8F8FF" width=150px id="storagePartition">Partition</td>
            <td align="center" bgcolor="#E8F8FF" id="storagePartPath">Path</td>
          </tr>
          <% ShowPartition(); %>
          <script language="JavaScript" type="text/javascript">
part_count = parseInt('<% getCount(1, "AllPart"); %>');
</script>
        </table>
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="center">
            <td><input type="button" style="{width:80px;}" value="Format" id="storageDiskFormat" onClick="submit_apply('format')">
              &nbsp;&nbsp;
              <input type="button" style="{width:80px;}" value="Re-allocate" name="realloc_parted" id="storageDiskPart" onClick="open_diskrepart_window()"></td>
          </tr>
        </table>
        <hr />
        <br />
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="right">
            <td><input type="button" style="{width:120px;}" value="Remove Disk" id="storageDiskRemove" onClick="submit_apply('remove')">
              &nbsp;&nbsp; </td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
