<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<title>Administration</title>
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("storage");
var admin = '<% getCfgGeneral(1, "Login"); %>';
var guest = '<% getCfgGeneral(1, "FtpAnonymous"); %>';
if (guest = "1")
{
	var user0 = 'anonymous';
}
else
{
	var user0 = "";
}
var user1 = '<% getCfgGeneral(1, "User1"); %>';
var upw1 = '<% getCfgGeneral(1, "Upw1"); %>';
var umax1 = '<% getCfgGeneral(1, "Umax1"); %>';
var umode1 = '<% getCfgGeneral(1, "Umode1"); %>';
var user2 = '<% getCfgGeneral(1, "User2"); %>';
var upw2 = '<% getCfgGeneral(1, "Upw2"); %>';
var umax2 = '<% getCfgGeneral(1, "Umax2"); %>';
var umode2 = '<% getCfgGeneral(1, "Umode2"); %>';
var user3 = '<% getCfgGeneral(1, "User3"); %>';
var upw3 = '<% getCfgGeneral(1, "Upw3"); %>';
var umax3 = '<% getCfgGeneral(1, "Umax3"); %>';
var umode3 = '<% getCfgGeneral(1, "Umode3"); %>';
var user4 = '<% getCfgGeneral(1, "User4"); %>';
var upw4 = '<% getCfgGeneral(1, "Upw4"); %>';
var umax4 = '<% getCfgGeneral(1, "Umax4"); %>';
var umode4 = '<% getCfgGeneral(1, "Umode4"); %>';
var user5 = '<% getCfgGeneral(1, "User5"); %>';
var upw5 = '<% getCfgGeneral(1, "Upw5"); %>';
var umax5 = '<% getCfgGeneral(1, "Umax5"); %>';
var umode5 = '<% getCfgGeneral(1, "Umode5"); %>';
var user6 = '<% getCfgGeneral(1, "User6"); %>';
var upw6 = '<% getCfgGeneral(1, "Upw6"); %>';
var umax6 = '<% getCfgGeneral(1, "Umax6"); %>';
var umode6 = '<% getCfgGeneral(1, "Umode6"); %>';
var user7 = '<% getCfgGeneral(1, "User7"); %>';
var upw7 = '<% getCfgGeneral(1, "Upw7"); %>';
var umax7 = '<% getCfgGeneral(1, "Umax7"); %>';
var umode7 = '<% getCfgGeneral(1, "Umode7"); %>';
var user8 = '<% getCfgGeneral(1, "User8"); %>';
var upw8 = '<% getCfgGeneral(1, "Upw8"); %>';
var umax8 = '<% getCfgGeneral(1, "Umax8"); %>';
var umode8 = '<% getCfgGeneral(1, "Umode8"); %>';

function initTranslation()
{
	var e = document.getElementById("storageAdmTitle");
	e.innerHTML = _("storage adm title");
	e = document.getElementById("storageAdmIntroduction");
	e.innerHTML = _("storage adm introduction");

	e = document.getElementById("storageAdmUser");
	e.innerHTML = _("storage adm user");
	e = document.getElementById("storageAdmUserName");
	e.innerHTML = _("storage user name");
	e = document.getElementById("storageAdmUserMaxLogins");
	e.innerHTML = _("storage user maxlogins");
	e = document.getElementById("storageAdmUserDL");
	e.innerHTML = _("storage user download");
	e = document.getElementById("storageAdmUserUL");
	e.innerHTML = _("storage user upload");
	e = document.getElementById("storageAdmUserOW");
	e.innerHTML = _("storage user overwrite");
	e = document.getElementById("storageAdmUserER");
	e.innerHTML = _("storage user erase");

	e = document.getElementById("storageAdmUserAdd");
	e.value = _("storage add");
	e = document.getElementById("storageAdmUserEdit");
	e.value = _("storage edit");
	e = document.getElementById("storageAdmUserDel");
	e.value = _("storage del");
	e = document.getElementById("storageAdmApply");
	e.value = _("storage apply");
	e = document.getElementById("storageAdmCancel");
	e.value = _("storage cancel");
}

function initValue()
{
	initTranslation();
}

function CheckValue()
{

	return true;
}

function submit_apply(parm)
{
	document.storage_adm.hiddenButton.value = parm;
	document.storage_adm.submit();
}

function open_useradd_window()
{
	window.open("adduser.asp","Storage_User_Add","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=480")
}

function open_useredit_window()
{
	var i = 0;
	while (i < 8)
	{
		if (document.storage_adm.storage_user_select[i].checked == true)
		{
			break;
		}
		i++;
	}
	if (i < 8) {
		var index = document.storage_adm.storage_user_select[i].value;
		document.storage_adm.hiddenIndex.value = index;
		document.storage_adm.hiddenUser.value = eval('user'+index);
		document.storage_adm.hiddenPassword.value = eval('upw'+index);
		document.storage_adm.hiddenMaxLogins.value = eval('umax'+index);
		document.storage_adm.hiddenMode.value = eval('umode'+index);
		window.open("edituser.asp","Storage_User_Edit","toolbar=no, location=no, scrollbars=yes, resizable=no, width=640, height=480")
	}
}

</script>
</head>

<body onLoad="initValue()">
<table class="body">
  <tr>
    <td><h1 id="storageAdmTitle">Administration</h1>
      <p id="storageAdmIntroduction"></p>
      <hr />
      <form method=post name=storage_adm action="/goform/storageAdm" onSubmit="return CheckValue()">
        <input type=hidden name=hiddenButton value="">
        <input type=hidden name=hiddenIndex value="">
        <input type=hidden name=hiddenUser value="">
        <input type=hidden name=hiddenPassword value="">
        <input type=hidden name=hiddenMaxLogins value="">
        <input type=hidden name=hiddenMode value="">
        <table width="90%" border="1" cellspacing="1" cellpadding="3" vspace="2" hspace="2" bordercolor="#9BABBD">
          <tr>
            <td class="title" colspan="7"><font id="storageAdmUser">User Management</font></td>
          </tr>
          <tr align=center>
            <td bgcolor="#E8F8FF" width=15px id="storageAdmUserSelect">&nbsp;</td>
            <td bgcolor="#E8F8FF" id="storageAdmUserName">User Name</td>
            <td bgcolor="#E8F8FF" id="storageAdmUserMaxLogins">Max. Logins</td>
            <td bgcolor="#E8F8FF" id="storageAdmUserDL">Download</td>
            <td bgcolor="#E8F8FF" id="storageAdmUserUL">Upload</td>
            <td bgcolor="#E8F8FF" id="storageAdmUserOW">Overwrite</td>
            <td bgcolor="#E8F8FF" id="storageAdmUserER">Erase</td>
          </tr>
          <script language="JavaScript" type="text/javascript">
var TRDHeader = "<tr align=center><td>";
var TDConneter = "</td><td>";
var TDFooter = "</td><tr>";

document.write(TRDHeader+"--"+TDConneter+admin+TDConneter+"3"+TDConneter+"Y"+TDConneter+"Y"+TDConneter+"Y"+TDConneter+"Y"+TDFooter);
if (guest == "1")
	document.write(TRDHeader+"--"+TDConneter+user0+TDConneter+"3"+TDConneter+"Y"+TDConneter+"N"+TDConneter+"N"+TDConneter+"N"+TDFooter);
for (var i=1;i<9;i++)
{
	if (eval("user"+i) != "")
	{
		var item = TRDHeader;
		item += "<input type=radio name=storage_user_select value="+i+">";
		item += TDConneter;
		item += eval("user"+i);
		item += TDConneter;
		if (eval("umode"+i).indexOf("M") >= 0)
		{
			item += eval("umax"+i);
			item += TDConneter;
		}
		else
		{
			item += "1";
			item += TDConneter;
		}
		if (eval("umode"+i).indexOf("D") >= 0)
		{
			item += "Y";
			item += TDConneter;
		}
		else
		{
			item += "N";
			item += TDConneter;
		}
		if (eval("umode"+i).indexOf("U") >= 0)
		{
			item += "Y";
			item += TDConneter;
		}
		else
		{
			item += "N";
			item += TDConneter;
		}
		if (eval("umode"+i).indexOf("O") >= 0)
		{
			item += "Y";
			item += TDConneter;
		}
		else
		{
			item += "N";
			item += TDConneter;
		}
		if (eval("umode"+i).indexOf("E") >= 0)
		{
			item += "Y";
			item += TDConneter;
		}
		else
		{
			item += "N";
			item += TDConneter;
		}
		item += TDFooter;		
		document.write(item);	
	}
}
</script>
        </table>
        <hr />
        <br>
        <table width = "90%" border = "0" cellpadding = "2" cellspacing = "1">
          <tr align="center">
            <td><input type="button" style="{width:80px;}" value="Add" id="storageAdmUserAdd" onClick="open_useradd_window()">
              &nbsp;&nbsp;
              <input type="button" style="{width:80px;}" value="Add" id="storageAdmUserEdit" onClick="open_useredit_window()">
              &nbsp;&nbsp;
              <input type="button" style="{width:80px;}" value="Delete" id="storageAdmUserDel" onClick="submit_apply('delete')">
              &nbsp; &nbsp;
              <input type="button" style="{width:80px;}" value="Apply" id="storageAdmApply" onClick="submit_apply('apply')">
              &nbsp; &nbsp;
              <input type="button"  style="{width:80px;}" value="Cancel" id="storageAdmCancel" onClick="window.location.reload()"></td>
          </tr>
        </table>
      </form></td>
  </tr>
</table>
</body>
