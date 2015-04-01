<!DOCTYPE html>
<html>

<!-- !!! Dont forget to modify   2860 mode treeapp.asp !!! -->

<head>
<title>DTree</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<link rel="stylesheet" href="/dtree/dtree.css" type="text/css" >
<link rel="StyleSheet" href="dtree.css" type="text/css" >
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/dtree/dtree.js"></script>
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
</head>

<body bgcolor="#FFFFFF" onLoad="initValue();">
<script language="JavaScript">

function rebootRouter(form, reloader)
{
	if (!confirm(_('treeapp reboot confirm')))
		return;

	if (parent!=null)
	{
		var obj = parent.document.getElementById("homeFrameset");
		if (obj != null)
			obj.rows = "0,1*"; // Hide top logo
		var obj = parent.document.getElementById("homeMenuFrameset");
		if (obj != null)
			obj.cols = "250,0"; // Hide menu
	}

	var submitForm = function()
	{
		form.submit();
	};

	form.target = reloader;

	ajaxPopupWindow('ajxReboot', '/messages/rebooting.asp', submitForm);
}

var isFimwareUpload = 0;
Butterlate.setTextDomain("main");

function initValue()
{
	var e = document.getElementById("openall");
	if (e!=null)
		e.innerHTML = _("treeapp openall");
	e = document.getElementById("closeall");
	if (e!=null)
		e.innerHTML = _("treeapp closeall");
}

function setUnderFirmwareUpload(flag){
	isFimwareUpload = flag;
}

function go(zz) {
	if(!isFimwareUpload)
		top.view.location=zz;
}

function refresh(){
	window.location.reload(false);
}
</script> 

<!-- <p><a href="javascript: a.openAll();" id="openall">open</a> | <a href="javascript: a.closeAll();" id="closeall">close</a></p> -->

<form name="rebootForm" style="display: none;" method="GET" action="/cgi-bin/reboot.sh" >
  <iframe id="rebootReloader" name="rebootReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
</form>
<script type="text/javascript">
var opmode = '<% getCfgZero(1, "OperationMode"); %>';
var vpnen = '<% getVPNBuilt(); %>';
var ipv6b = '<% getIPv6Built(); %>';
var wdsb = '<% getWDSBuilt(); %>';
var usbb = '<% getUSBBuilt(); %>';
var storageb = '<% getStorageBuilt(); %>';
var ftpb = '<% getFtpBuilt(); %>';
var smbb = '<% getSmbBuilt(); %>';
var printersrvb = '<% getPrinterSrvBuilt(); %>';
var usbmodemb = '<% getUSBModemBuilt(); %>';
var syslogb = '<% getSysLogBuilt(); %>';
var swqos = '<% getSWQoSBuilt(); %>';
var ad = '<% isAntennaDiversityBuilt(); %>';
var transb = '<% getTransmissionBuilt(); %>';

a = new dTree('a');
a.config.useStatusText=true;
a.config.useCookies=false;
a.config.closeSameLevel=true;

//  nodeID, parent nodeID,  Name,  URL
a.add(000,  -1, _("treeapp router"),			"javascript:go('overview.asp');");
a.add(200,   0, _("treeapp operation mode"),		"javascript:go('opmode.asp');");
a.add(300,   0, _("treeapp network settings"),		"javascript:a.oo(300);");

a.add(301, 300, _("treeapp lan"),			"javascript:go('internet/lan.asp');");

if (opmode != '0')
	a.add(302, 300, _("treeapp wan"),		"javascript:go('internet/wan.asp');");

if (ipv6b == "1") {
	a.add(307, 300, _("treeapp ipv6"), 		    "javascript:go('internet/ipv6.asp');");
}

a.add(303, 300, _("treeapp vpn"),			"javascript:go('internet/vpn.asp');");

if (opmode != '0')
	a.add(304, 300, _("treeapp routing"),		"javascript:go('internet/routing.asp');");

if (opmode == '4')
	a.add(305, 300, _("treeapp hotspot"),		"javascript:go('internet/hotspot.asp');");

if (swqos == '1')
	a.add(306, 300, _("treeapp qos"),		"javascript:go('internet/qos.asp');");


if (opmode == '2')
{
	a.add(400,   0, _("treeapp wireless settings"),		"javascript:a.oo(400);");
	a.add(401, 400, _("treeapp profile"),			"javascript:go('station/profile.asp');");
	a.add(402, 400, _("treeapp link status"),		"javascript:go('station/link_status.asp');");
	a.add(403, 400, _("treeapp statistics"),		"javascript:go('station/statistics.asp');");
	a.add(404, 400, _("treeapp advance"),			"javascript:go('station/advance.asp');");
}
else
{
	a.add(400,   0, _("treeapp wireless settings"),		"javascript:a.oo(400);");
	a.add(401, 400, _("treeapp basic"),			"javascript:go('wireless/basic.asp');");
	a.add(402, 400, _("treeapp advanced"),			"javascript:go('wireless/advanced.asp');");
	a.add(403, 400, _("treeapp security"),			"javascript:go('wireless/security.asp');");
	if (wdsb == "1")
		a.add(404, 400, _("treeapp wds"),                   "javascript:go('wireless/wds.asp');");
	if (opmode == '3')
		a.add(406, 400, _("treeapp ap client"),     "javascript:go('wireless/apcli.asp');");
	a.add(407, 400, _("treeapp station list"),          "javascript:go('wireless/stainfo.asp');");
	if (ad == '1')
		a.add(408, 400, "Antenna Diversity",		"javascript:go('wireless/ant_diversity.asp');");
}

// Firewall
if (opmode != '0') {
	a.add(700,   0, _("treeapp firewall"),              "javascript:a.oo(700);");
	a.add(701, 700, _("treeapp firewall"),              "javascript:go('firewall/firewall.asp');");
	a.add(703, 700, _("treeapp alg"),                   "javascript:go('firewall/alg.asp');");
	a.add(704, 700, _("treeapp dmz"),                   "javascript:go('firewall/DMZ.asp');");
	a.add(705, 700, _("treeapp content filtering"),     "javascript:go('firewall/content_filtering.asp');");
}

// Services
a.add(500, 0,   _("treeapp services"),          "javascript:a.oo(500);");
if (opmode != '4')	a.add(501, 500, _("treeapp dhcp server"),       "javascript:go('services/dhcp.asp');");
a.add(502, 500, _("treeapp l2tp server"),       "javascript:go('services/l2tp.asp');");
a.add(503, 500, _("treeapp ntp settings"),      "javascript:go('services/ntp.asp');");
a.add(504, 500, _("treeapp ddns settings"),     "javascript:go('services/ddns.asp');");
if (smbb == "1") a.add(505, 500, _("treeapp samba"),             "javascript:go('services/samba.asp');");
//a.add(506, 500, _("treeapp accounting"),        "javascript:go('services/account.asp');");
a.add(510, 500, _("treeapp miscellaneous"),     "javascript:go('services/misc.asp');");

if (usbb == "1")
{
	a.add(800,   0, _("treeapp usb"),		"javascript:a.oo(800);");
	if (printersrvb == "1")	a.add(802, 800, _("treeapp printersrv"),	"javascript:go('usb/P910NDprintersrv.asp');");
    if (usbmodemb == "1")	a.add(803, 800, _("treeapp usbmodem"),		"javascript:go('usb/USBmodem.asp');");
	if (storageb == "1")
	{
		a.add(850,   0, _("treeapp storage"),		"javascript:a.oo(850);");
		a.add(852, 850, _("treeapp disk"),		"javascript:go('storage/disk_admin.asp');");
		if (ftpb == "1")
			a.add(853, 850, _("treeapp ftpsrv"),		"javascript:go('storage/ftpsrv.asp');");
		if (transb == "1")
			a.add(854, 850, _("treeapp transmission"),	"javascript:go('storage/transmission.asp');");
		if (smbb == "1")
			a.add(855, 850, _("treeapp sambasrv"),		"javascript:go('services/samba.asp');");
	}
}

a.add(900,   0, _("treeapp administration"),        "javascript:a.oo(900);");
a.add(901, 900, _("treeapp management"),            "javascript:go('adm/management.asp');");
a.add(904, 900, _("treeapp status"),                "javascript:go('adm/status.asp');");
a.add(905, 900, _("treeapp statistics"),            "javascript:go('adm/statistic.asp');");
if (syslogb == "1")
{
	a.add(908, 900, _("treeapp system log"),            "javascript:go('adm/syslog.asp');");
}
a.add(907, 900, _("treeapp sdk history"),           "javascript:go('cgi-bin/history.sh');");
a.add(909, 900, _("treeapp reboot"),                       "javascript:rebootRouter(document.rebootForm, 'rebootReloader');");
document.write(a);

</script>
</body>
</html>
