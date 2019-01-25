<!DOCTYPE html>
<html>
<head>
<title>Disk Management</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">

<style>

#fileList tr:hover {
    background-color: #FFFDCF;
}

#fileList tr td {
    padding: 5px;
}

#fileList tr {
	cursor: pointer;
}

td.icons a {
	color: black;
}

.leftIcons {
    text-align: center;
}

.linkIcon {
	text-decoration: none;
	font-size: 11pt !important;
	padding: 3px !important;
}

</style>

<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
<script src="/lang/<% getLangDictionary(); %>/dict_storage.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<script type="text/javascript" src="/js/json2.js"></script>
<script language="JavaScript" type="text/javascript">

var currentDir = "";

function initValue(form)
{
	init_translation_model();
	getDirectory(currentDir, onGetDirectory);
}

function CheckValue(form)
{
	ajaxShowTimer(form, 'timerReloader', _('message apply'), 3);
	return true;
}

function normalizePath(dirname)
{
	var dir_parts = dirname.split("/").filter(function (val) { return val != "" });
	if (dir_parts.length == 0) return "";
	return (dir_parts.join("/"));
}

function getDirectory(dirname, ok_event)
{
	dirname = normalizePath(dirname);

	ajaxPostRequest("/goform/storageGetDir", "name="+dirname, false, function(responseText) {
		var dirinfo = JSON.parse(responseText);

		var dirs = dirinfo.filter(function (val) { return val.type == "DIR" });
		var files = dirinfo.filter(function (val) { return val.type != "DIR" });

		currentDir = dirname;
		ok_event(dirname, dirs, files);
	});

}


function onGetDirectory(parentdir, dirs, files) {

	displayElement('dirName', parentdir != "");
	document.getElementById("dirName").innerHTML = parentdir;


	var flist = document.getElementById("fileList");
	while (flist.firstChild) {
		flist.removeChild(flist.firstChild);
	}

	for (var i=0;i<dirs.length;i++) {
		var dir = dirs[i];
		if (dir.name == ".") continue;
		if (parentdir == "" && dir.name == "..") continue;
		addFileRow(parentdir, dir);
	}

	for (var i=0;i<files.length;i++) {
		var file = files[i];
		addFileRow(parentdir, file);
	}

	if (parentdir == "" && dirs.length == 2 && files.length == 0) {
		var td1 = document.createElement('td');
		var tr1 = document.createElement('tr');
		td1.colSpan = 4;
		tr1.innerHTML = _('storage disk no devices');
		td1.appendChild(tr1);
		flist.appendChild(td1);
	}
}

function addFileRow(dirname, file) {

	var tr = document.createElement('tr');
	var td1 = document.createElement('td');
	var td2 = document.createElement('td');
	var td3 = document.createElement('td');
	var td4 = document.createElement('td');
	td4.className = "icons";

//	td1.innerHTML = "&#128196;";
	td1.className = "linkIcon leftIcons"

	td3.innerHTML = ""; //permToString(file.perm);

	if (file.name == "..")
	{
		file.type = "UP";
	}


	switch (file.type) {
		case "DIR":
			tr.className = "directory";
			td1.innerHTML = "&#128193;";
		break;

		case "LINK":
			tr.className = "symlink";
			td1.innerHTML = "&#128279;";
		break;

		case "DRIVE":
			tr.className = "drive";
			td1.innerHTML = "&#128190;";
			td3.innerHTML = file.fs;

			var a = document.createElement('a');
			a.innerHTML = "&#9432;"; // info
			a.className = "linkIcon";
			a.href = "#";
			a.title = _('storage disk info');
			a.onclick = function() { alert(file.fsflags.split(",").join("\n")); };
			td3.appendChild(a);

			var a = document.createElement('a');
			a.innerHTML = "&#9167;"; // unmount
			a.className = "linkIcon";
			a.href = "#";
			a.title = _('storage disk unmount');
			a.onclick = function() { unmountDrive(dirname, file); };
			td4.appendChild(a);

		break;

		case "FILE":
			td3.innerHTML = formatBytes(file.size);

			var a = document.createElement('a');
			a.innerHTML = "&#10060;"; // remove
			a.className = "linkIcon";
			a.href = "#";
			a.title = _('storage disk remove');
			a.onclick = function() { removeFile(dirname, file); };
			td4.appendChild(a);
		break;


		case "UP":
			td3.innerHTML = "";
			tr.onclick = changeDirUp;
			td1.innerHTML = "&#11013;";
		break;

		default:
			tr.className = "file";
	}

	td2.innerHTML = file.name;


	if (file.type == "DIR" || file.type == "DRIVE")
	{
		var f1 = function() { getDirectory( dirname+"/"+file.name, onGetDirectory ); };
		td1.onclick = f1;
		td2.onclick = f1;
	}

	tr.appendChild(td1);
	tr.appendChild(td2);
	tr.appendChild(td3);
	tr.appendChild(td4);

	document.getElementById("fileList").appendChild(tr);
}

function permToString(perm) {

	if (perm < 0)
		return "";

	var out = "";
	out += (perm & 256) ? "r":"-";
	out += (perm & 128) ? "w":"-";
	out += (perm & 64)  ? "x":"-";

	out += " ";

	out += (perm & 32)  ? "r":"-";
	out += (perm & 16)  ? "w":"-";
	out += (perm & 8)   ? "x":"-";

	out += " ";

	out += (perm & 4)   ? "r":"-";
	out += (perm & 2)   ? "w":"-";
	out += (perm & 1)   ? "x":"-";

	return out
}

function changeDirUp() {
	currentDir = normalizePath(currentDir.split("/").slice(0,-1).join("/"))
	getDirectory(currentDir, onGetDirectory);
}

function removeFile(dir, file) {
	var path = normalizePath(dir+"/"+file.name);
	if (confirm(_("storage disk remove confirm").replace("{path}",path)))
	{
		ajaxPostRequest("/goform/storageRemoveFile", "name="+path, false, function(responseText) {
			getDirectory(currentDir, onGetDirectory);
		});
	}
}

function unmountDrive(dir, file) {
	var path = normalizePath(dir+"/"+file.name);
	if (confirm(_("storage disk unmount confirm").replace("{path}",path)))
	{
		ajaxPostRequest("/goform/storageUnmountDrive", "name="+path, false, function(responseText) {
			getDirectory(currentDir, onGetDirectory);
			if (responseText != 0) {
				alert(_("storage disk unmount error"));
			}
		});
	}
}


</script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue(document.storage_disk);" >
<table class="body">
  <tr>
    <td><h1 data-tr="storage disk title">Disk Management</h1>
      <p data-tr="storage disk introduction"></p>
      <hr />
      <iframe name="timerReloader" id="timerReloader" style="width:0;height:0;border:0px solid #fff;"></iframe>
      <form method=post name=storage_disk action="/goform/storageFileMan" onSubmit="return CheckValue(this)">
        <table class="form">
	<col style="width: 5%"/>
	<col style="width: 75%"/>
	<col style="width: 15%"/>
	<col style="width: 5%"/>
	<thead>
          <tr>
            <td class="title" colspan="4" data-tr="storage disk title">Disk Management</td>
          </tr>
          <tr>
            <td class="title" colspan="4" id="dirName"></td>
          </tr>
	</thead>
	<tbody id="fileList">
	</tbody>
        </table>
      </form></td>
  </tr>
</table>
</body>
</html>
