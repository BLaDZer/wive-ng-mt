<!DOCTYPE html>
<html>
<head>
<title>DTree</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<link rel="stylesheet" href="/dtree/dtree.css" type="text/css" >
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<script type="text/javascript" src="/dtree/dtree.js"></script>
<script src="/lang/<% getLangDictionary(); %>/dict_main.js"></script>
<script src="/lang/<% getLangDictionary(); %>/dict_tree.js"></script>
<script type="text/javascript" src="/js/nvram.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
</head>
<body bgcolor="#FFFFFF" onLoad="initValue();">
<script language="JavaScript" type="text/javascript">

var isFimwareUpload = 0;

function initValue()
{
	var e = document.getElementById("openall");
	if (e!=null)
		e.innerHTML = _("button openall");
	e = document.getElementById("closeall");
	if (e!=null)
		e.innerHTML = _("button closeall");
}

function setUnderFirmwareUpload(flag){
	isFimwareUpload = flag;
}

function go(zz) {
	top.view.location=zz;
}

function redirect(zz) {
	window.parent.location.replace(zz);
}

function refresh(){
	window.location.reload(false);
}
</script>

<!-- <p><a href="javascript: a.openAll();" id="openall">open</a> | <a href="javascript: a.closeAll();" id="closeall">close</a></p> -->

<form name="rebootForm" style="display: none;" method="GET" action="/goform/reboot" >
  <iframe id="rebootReloader" name="rebootReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
</form>

<script language="JavaScript" type="text/javascript">

a = new dTree('a');
a.config.useStatusText=true;
a.config.useCookies=false;
a.config.closeSameLevel=true;

//  nodeID, parent nodeID,  Name,  URL
a.add(000,  -1, _("treeapp router"),			"javascript:go('overview.asp');");
a.add(200,   0, _("treeapp operation mode"),		"javascript:go('opmode.asp');");
a.add(300,   0, _("treeapp network settings"),		"javascript:a.oo(300);");

a.add(301, 300, _("treeapp lan"),			"javascript:go('internet/lan.asp');");

if (NVRAM_OperationMode != '0')
	a.add(302, 300, _("treeapp wan"),		"javascript:go('internet/wan.asp');");

if (BUILD_IPV6 == "1" && NVRAM_OperationMode != '0') {
	a.add(303, 300, _("treeapp ipv6"), 		    "javascript:go('internet/ipv6.asp');");
}

a.add(304, 300, _("treeapp vpn"),			"javascript:go('internet/vpn.asp');");

if (AUTH_ROLE == 2)
{
	a.add(305, 300, _("treeapp vlan"),			"javascript:go('internet/vlan.asp');");
	a.add(306, 300, _("treeapp ethernet"),			"javascript:go('internet/ethernet.asp');");

	if (NVRAM_OperationMode != '0')
		a.add(307, 300, _("treeapp routing"),		"javascript:go('internet/routing.asp');");

	if (BUILD_SWQOS == '1')
		a.add(308, 300, _("treeapp qos"),		"javascript:go('internet/qos.asp');");

	if (BUILD_SPOT == '1' && NVRAM_OperationMode != '0')
		a.add(309, 300, _("treeapp hotspot"),		"javascript:go('internet/hotspot.asp');");
}

if (NVRAM_OperationMode == '2')
{
    if (AUTH_ROLE == 2)
    {
	a.add(400,   0, _("treeapp wireless settings"),		"javascript:a.oo(400);");
	a.add(401, 400, _("treeapp profile"),			"javascript:go('station/profile.asp');");
	a.add(402, 400, _("treeapp link status"),		"javascript:go('station/link_status.asp');");
	a.add(403, 400, _("treeapp statistics"),		"javascript:go('station/statistics.asp');");
	a.add(404, 400, _("treeapp advance"),			"javascript:go('station/advance.asp');");
    }
}
else
{
	a.add(400,   0, _("treeapp wireless settings"),		"javascript:a.oo(400);");
	a.add(401, 400, _("treeapp main"),			"javascript:go('wireless/main.asp');");

    if (AUTH_ROLE == 2)
    {
	if (BUILD_WDS == "1")
		a.add(403, 400, _("treeapp wds"),                   "javascript:go('wireless/wds.asp');");
	if (NVRAM_OperationMode == '3')
		a.add(404, 400, _("treeapp ap client"),     "javascript:go('wireless/apcli.asp');");
    }
	a.add(405, 400, _("treeapp station list"),          "javascript:go('wireless/stainfo.asp');");
}

// Firewall
if (NVRAM_OperationMode != '0') {
	a.add(700,   0, _("treeapp firewall"),              "javascript:a.oo(700);");
	a.add(701, 700, _("treeapp firewall"),              "javascript:go('firewall/firewall.asp');");
	a.add(702, 700, _("treeapp alg"),                   "javascript:go('firewall/alg.asp');");
	a.add(703, 700, _("treeapp dmz"),                   "javascript:go('firewall/DMZ.asp');");
	if (BUILD_WEBSTR == "1") a.add(704, 700, _("treeapp content filtering"),     "javascript:go('firewall/content_filtering.asp');");
}

// Services
a.add(500, 0,   _("treeapp services"),          "javascript:a.oo(500);");

if (NVRAM_OperationMode != '0') a.add(501, 500, _("treeapp dhcp server"),       "javascript:go('services/dhcp.asp');");

if (AUTH_ROLE == 2)
{
    a.add(502, 500,                          _("treeapp l2tp server"),         "javascript:go('services/l2tp.asp');");
    if (NVRAM_OperationMode != '2')
    {
        a.add(503, 500,                          _("treeapp l2tpv3 server"),       "javascript:go('services/l2tpv3.asp');");
        if (BUILD_EOIP == "1")   a.add(504, 500, _("treeapp eoip server"),         "javascript:go('services/eoip.asp');");
    }

    if (BUILD_RADIUS == "1") a.add(505, 500, _("treeapp radius"),              "javascript:go('services/radius.asp');");
    if (BUILD_CWMP == "1")   a.add(506, 500, _("treeapp cwmp"),                "javascript:go('services/cwmp.asp');");
    a.add(507, 500,                          _("treeapp ntp settings"),        "javascript:go('services/ntp.asp');");
}

if (BUILD_DDNS == "1") a.add(506, 500, _("treeapp ddns settings"),     "javascript:go('services/ddns.asp');");

if (AUTH_ROLE == 2)
{
    if (BUILD_SMB == "1" && BUILD_STORAGE != "1") a.add(507, 500, _("treeapp samba"),             "javascript:go('services/samba.asp');");
    if (BUILD_IPTACCOUNT == "1" && NVRAM_OperationMode != '0') a.add(508, 500, _("treeapp accounting"),        "javascript:go('services/account.asp');");
}

a.add(509, 500, _("treeapp miscellaneous"),     "javascript:go('services/misc.asp');");

if (BUILD_USB == "1")
{
    a.add(800,   0, _("treeapp usb"),		"javascript:a.oo(800);");
    if (BUILD_PRINTER_SRV == "1")	a.add(802, 800, _("treeapp printersrv"),	"javascript:go('usb/P910NDprintersrv.asp');");
    if (BUILD_USBMODEM == "1")	a.add(803, 800, _("treeapp usbmodem"),		"javascript:go('usb/USBmodem.asp');");
    if (BUILD_STORAGE == "1")
    {
	    a.add(850,   0, _("treeapp storage"),		"javascript:a.oo(850);");
	    a.add(852, 850, _("treeapp disk"),			"javascript:go('storage/disk_admin.asp');");
	    if (BUILD_FTP == "1")
		a.add(853, 850, _("treeapp ftpsrv"),		"javascript:go('storage/ftpsrv.asp');");
	    if (BUILD_SMB == "1")
		a.add(855, 850, _("treeapp samba"),		"javascript:go('services/samba.asp');");
	    if (BUILD_TRANSMISSION == "1")
		a.add(854, 850, _("treeapp transmission"),	"javascript:go('storage/transmission.asp');");
    }
}

a.add(900,   0, _("treeapp administration"),        "javascript:a.oo(900);");
a.add(901, 900, _("treeapp management"),            "javascript:go('adm/management.asp');");
a.add(902, 900, _("treeapp status"),                "javascript:go('adm/status.asp');");

if (AUTH_ROLE == 2)
{

    if (BUILD_SYSLOG == "1")
        	a.add(903, 900, _("treeapp system log"),   "javascript:go('adm/syslog.asp');");
    a.add(904, 900, _("treeapp sdk history"),              "javascript:go('history.html');");
}

a.add(905, 900, _("treeapp reboot"),                       "javascript:go('reboot.asp');");

if (IS_NGINX)
{
    a.add(1000,   0, _("treeapp logout"),                      "javascript:redirect('logout.asp');");
}

document.write(a);

</script>
</body>
</html>
