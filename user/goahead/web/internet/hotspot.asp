<!DOCTYPE html>
<html>
<head>
<title>Hotspot</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/validation.js"></script>
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script language="JavaScript" type="text/javascript">

Butterlate.setTextDomain("network");
Butterlate.setTextDomain("internet");
Butterlate.setTextDomain("hint");
Butterlate.setTextDomain("buttons");

var old_profile = "<% getCfgZero(1, "chilli_profile"); %>";
var chilli_built = '<% getChilliBuilt(); %>' == '1';
var nodog_built = '<% getNoDogBuilt(); %>' == '1';

var Profiles = [
  // "Profile", "Provider name", "IP", "Netmask", "Start DHCP IP", "End DHCP IP", "Primary DNS", "Secondary DNS", "Domain name", "Lease time", "Radius server auth address", "Radius server acct address", "Radius server secret", "Radius NAS ID", "Radius Location ID", "Radius Location Name", "Radius CoA/DM port", "No IP check for CoA/DM request", "UAM server URL", "UAM homepage URL", "UAM secret", "UAM allowed hosts", "UAM allowed domains", "Allow all DNS requests", "Allowed MAC"
  ["manual", _("hotspot manual"), false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
  ["hotspotsystem", "HotSpotSystem.com", "192.168.182.0", "255.255.255.0", false, false, "192.168.182.1", "192.168.182.1", "key.chillispot.info", false, "radius.hotspotsystem.com", "radius2.hotspotsystem.com", false, false, false, false, "3779", "off", "https://customer.hotspotsystem.com/customer/hotspotlogin.php", "", false, "194.149.46.0/24,198.241.128.0/17,66.211.128.0/17,216.113.128.0/17,70.42.128.0/17,128.242.125.0/24,216.52.17.0/24,62.249.232.74,155.136.68.77,155.136.66.34,66.4.128.0/17,66.211.128.0/17,66.235.128.0/17,88.221.136.146,195.228.254.149,195.228.254.152,203.211.140.157,203.211.150.204,www.paypal.com,mobile.paypal.com,www.paypalobjects.com,sstats.paypal-metrics.com,altfarm.mediaplex.com,live.adyen.com,www.worldpay.com,secure.worldpay.com,www.directebanking.com,betalen.rabobank.nl,ideal.ing.nl,ideal.abnamro.nl,www.ing.nl,www.hotspotsystem.com,customer.hotspotsystem.com,tech.hotspotsystem.com,a1.hotspotsystem.com,a2.hotspotsystem.com,a3.hotspotsystem.com,a4.hotspotsystem.com,a5.hotspotsystem.com,a6.hotspotsystem.com,a7.hotspotsystem.com,a8.hotspotsystem.com,a9.hotspotsystem.com,a10.hotspotsystem.com,a11.hotspotsystem.com,a12.hotspotsystem.com,a13.hotspotsystem.com,a14.hotspotsystem.com,a15.hotspotsystem.com,a16.hotspotsystem.com,a17.hotspotsystem.com,a18.hotspotsystem.com,a19.hotspotsystem.com,a20.hotspotsystem.com", "paypal.com,paypalobjects.com,paypal-metrics.com,mediaplex.com,worldpay.com,adyen.com,hotspotsystem.com,geotrust.com", "off", false],
  ["mywifi", "MyWifi", "192.168.182.0", "255.255.255.0", false, false, "192.168.182.1", "192.168.182.1", "key.chillispot.info", false, "radiusm.mywifi.com", "radiuss.mywifi.com", false, false, false, false, "3779", false, "http://access.mywifi.com", "", false, "212.118.48.0/24,212.158.173.0/24,91.227.52.0/24,91.227.53.0/24,91.200.28.0/24,91.200.30.0/24,91.232.115.0/26,webmoney.ru,wmtransfer.com,webmoney.com.mx", "login.wmtransfer.com,security.webmoney.ru,access.mywifi.com,webmoney.ru,wmtransfer.com,webmoney.com.mx", false, false],
  ["saiwifi", "SAIWifi", "192.168.182.0", "255.255.255.0", false, false, "192.168.182.1", "192.168.182.1", "key.chillispot.info", false, "176.111.96.30", "92.63.105.213", false, false, false, false, "3779", false, "http://hotspot.saiwifi.ru", "", false, "77.88.8.2,176.111.96.30,176.111.96.13", "stat.saiwifi.ru,hotspot.saiwifi.ru", false, false],
  ["wifisystem", "Wi-Fi System", "192.168.182.0", "255.255.255.0", false, false, "192.168.182.1", "192.168.182.1", "key.chillispot.info", false, "radius.wifisystem.ru", "radius2.wifisystem.ru", "wfs123", false, false, false, "3779", false, "https://auth.wifisystem.ru/hotspotlogin", "", false, "95.167.165.64/27,195.14.118.0/23,91.229.116.0/22,213.59.200.64/28,91.200.28.0/24,91.227.52.0/24,198.241.128.0/17,66.211.128.0/17,216.113.128.0/17,paypal.com,paypalobjects.com,paymaster.ru,wifisystem.ru,www.paymaster.ru,www.paypal.com,www.paypalobjects.com,www.wifisystem.ru,auth.wifisystem.ru,support.wifisystem.ru,lk.wifisystem.ru,account.wifisystem.ru,s1.wifisystem.ru,s2.wifisystem.ru,s3.wifisystem.ru,s4.wifisystem.ru,s5.wifisystem.ru,s6.wifisystem.ru,s7.wifisystem.ru,s8.wifisystem.ru,s9.wifisystem.ru,s10.wifisystem.ru,s11.wifisystem.ru,s12.wifisystem.ru,s13.wifisystem.ru,s14.wifisystem.ru,s15.wifisystem.ru,s16.wifisystem.ru,s17.wifisystem.ru,s18.wifisystem.ru,s19.wifisystem.ru,s20.wifisystem.ru", "wifisystem.ru,paymaster.ru,paypal.com,paypalobjects.com", false, false],
/*  ["enforta", "Enforta", "192.168.182.0", "255.255.255.0", false, false, "87.241.223.68", "192.168.182.1", "key.chillispot.info", false, "185.12.28.167", "0.0.0.0", "enforta1242-8201-service_36Cloud", false, false, false, "3779", false, "http://hs.enforta.ru", "", false, "185.12.28.167,87.241.223.68,192.168.182.1", "", false, false] */
];

function hideHint() {
  var row = document.getElementById('hint_row');
  row.innerHTML = '';
}

function showHint(key){
  var row = document.getElementById('hint_row');
  var text = '<div class="hint"><font color="#0000ff"><b>' + _("hint title") + ':</b></font>&nbsp;';
  var show = true;

  if (key == 'spot_enable')
    text += _("hint spot enable");
/*  else if (document.spotCfg.spotEnable.value == "1") // hints for chilli
    if (key == 'spot_profile')
      text += _("hint spot profile");
    else if (key == 'spot_ip')
      text += _("hint spot ip");
    else if (key == 'spot_mask')
      text += _("hint spot mask");
    else if (key == 'spot_startip')
      text += _("hint spot startip");
    else if (key == 'spot_endip')
      text += _("hint spot endip");
    else if (key == 'spot_dns')
      text += _("hint spot dns");
    else if (key == 'spot_domain')
      text += _("hint spot domain");
    else if (key == 'spot_lease')
      text += _("hint spot lease");
    else if (key == 'spot_auth')
      text += _("hint spot auth");
    else if (key == 'spot_acct')
      text += _("hint spot acct");
    else if (key == 'spot_rad_secret')
      text += _("hint spot rad secret");
    else if (key == 'spot_nasid')
      text += _("hint spot nasid");
    else if (key == 'spot_locid')
      text += _("hint spot locid");
    else if (key == 'spot_locname')
      text += _("hint spot locname");
    else if (key == 'spot_port')
      text += _("hint spot port");
    else if (key == 'spot_ipcheck')
      text += _("hint spot ipcheck");
    else if (key == 'spot_uamserver')
      text += _("hint spot uamserver");
    else if (key == 'spot_uamhomepage')
      text += _("hint spot uamhomepage");
    else if (key == 'spot_uamsecret')
      text += _("hint spot uamsecret");
    else if (key == 'spot_uamallowed')
      text += _("hint spot uamallowed");
    else if (key == 'spot_uamdomain')
      text += _("hint spot uamdomain");
    else if (key == 'spot_uamdns')
      text += _("hint spot uamdns");
    else if (key == 'spot_macallowed')
      text += _("hint spot macallowed");
    else
      show = false;
  else */if (document.spotCfg.spotEnable.value == "2") // hints for nodog
    if (key == 'GatewayIPRange')
      text += _("hint nodog ip range");
    else if (key == 'RedirectURL')
      text += _("hint nodog url");
    else if (key == 'MaxClients')
      text += _("hint nodog max clients");
    else if (key == 'ClientIdleTimeout')
      text += _("hint nodog idle");
    else if (key == 'ClientForceTimeout')
      text += _("hint nodog force");
    else if (key == 'AuthenticateImmediately')
      text += _("hint nodog auth immediately");
    else if (key == 'MACMechanism')
      text += _("hint nodog mac");
    else if (key == 'BlockedMACList')
      text += _("hint nodog blocked");
    else if (key == 'AllowedMACList')
      text += _("hint nodog allowed");
    else if (key == 'TrustedMACList')
      text += _("hint nodog trusted");
    else if (key == 'PasswordAuthentication')
      text += _("hint nodog auth pass");
    else if (key == 'Password')
      text += _("hint nodog pass");
    else if (key == 'UsernameAuthentication')
      text += _("hint nodog auth user");
    else if (key == 'Username')
      text += _("hint nodog user");
    else if (key == 'PasswordAttempts')
      text += _("hint nodog attempts");
    else
      show = false;
  else
    show = false;

  if (show) {
    text += '</div>';
    row.innerHTML = text;
  }
}

function initTranslation() {
  _TR("sTitle", "hotspot title");
  _TR("sIntroduction", "hotspot introduction");
  _TR("spotSetup", "hotspot setup");
  _TR("spotProfile", "hotspot profile");
  _TR("sIp", "inet ip");
  _TR("sNetmask", "inet netmask");
  _TR("sStartIP", "lan dhcp start");
  _TR("sEndIP", "lan dhcp end");
  _TR("sPriDns", "inet pri dns");
  _TR("sSecDns", "inet sec dns");
  _TR("sDomain", "hotspot domain");
  _TR("sLease", "lan dhcp lease");
  _TR("sRadServer1", "hotspot radius auth");
  _TR("sRadServer2", "hotspot radius acct");
  _TR("sRadSecret", "hotspot radius secret");
  _TR("sNasId", "hotspot radius nas");
  _TR("sRadLocId", "hotspot radius location id");
  _TR("sRadLocName", "hotspot radius location name");
  _TR("sRadCoaPort", "hotspot radius coa port");
  _TR("sRadCoaNoIpCheck", "hotspot radius no check ip");
  _TR("sUamServer", "hotspot uam server");
  _TR("sUamHomepage", "hotspot uam homepage");
  _TR("sUamSecret", "hotspot uam secret");
  _TR("sUamAllowed", "hotspot uam allowed");
  _TR("sUamDomain", "hotspot uam domain");
  _TR("sUamAnyDNS", "hotspot uam any dns");
  _TR("sMacAllowed", "hotspot uam mac");
  _TR("GatewayIPRange", "hotspot nodog ip range");
  _TR("RedirectURL", "hotspot nodog url");
  _TR("MaxClients", "hotspot nodog max clients");
  _TR("ClientIdleTimeout", "hotspot nodog idle timeout");
  _TR("ClientForceTimeout", "hotspot nodog force timeout");
  _TR("AuthenticateImmediately", "hotspot nodog auth");
  _TR("MACMechanism", "hotspot nodog mechanism");
  _TR("BlockedMACList", "hotspot nodog blocked list");
  _TR("AllowedMACList", "hotspot nodog allowed list");
  _TR("TrustedMACList", "hotspot nodog trusted list");
  _TR("PasswordAuthentication", "hotspot nodog pass");
  _TR("Password", "vpn password");
  _TR("UsernameAuthentication", "hotspot nodog user");
  _TR("Username", "vpn username");
  _TR("PasswordAttempts", "hotspot nodog pass attempts");
  _TRV("sApply", "button apply");
  _TRV("sCancel", "button cancel");
  _TRV("sReset", "button reset");
  var elements = document.getElementsByTagName('option');
    for (var i = 0; i < elements.length; i++)
      if (elements[i].id == "disable")
      elements[i].innerHTML = _("button disable");
    else if (elements[i].id == "enable")
      elements[i].innerHTML = _("button enable");
}

function ModeOnChange(form) {
  displayElement(["row_chilli", "row_sIp", "row_sNetmask", "row_sStartIP", "row_sEndIP", "row_sPriDns", "row_sSecDns", "row_sDomain", "row_sLease", "row_sRadServer1", "row_sRadServer2", "row_sRadSecret", "row_sNasId", "row_sRadLocId", "row_sRadLocName", "row_sRadCoaPort", "row_sRadCoaNoIpCheck", "row_sUamServer", "row_sUamHomepage", "row_sUamSecret", "row_sUamAllowed", "row_sUamDomain", "row_sUamAnyDNS", "row_sMacAllowed"], (form.spotEnable.value == "1"));
  displayElement(["row_GatewayIPRange", "row_RedirectURL", "row_MaxClients", "row_ClientIdleTimeout", "row_ClientForceTimeout", "row_AuthenticateImmediately", "row_MACMechanism", "row_TrustedMACList", "row_AllowedMACList", "row_BlockedMACList", "row_PasswordAuthentication", "row_Password", "row_UsernameAuthentication", "row_Username", "row_PasswordAttempts"], (form.spotEnable.value == "2"));

  form.chilliEnable.value = (form.spotEnable.value == "1") ? "on" : "off";
  form.nodogEnable.value = (form.spotEnable.value == "2") ? "1" : "0";

  if (form.spotEnable.value == "1")
    ProfileOnChange(form);
  else if (form.spotEnable.value == "2")
    MACMechanismOnChange(form);
}

function insertVal(form, row, val, disp) {
  form.elements[row].value = val;
  displayElement("row_" + row, disp == false);
}

function ProfileOnChange(form) {
  for(var i = 0; i < Profiles.length; i++) {
    var tmp = Profiles[i];
    if (tmp[0] == form.spotProfile.value) {
      insertVal(form, "sIp", (!tmp[2]) ? "<% getSpotIp(); %>" : tmp[2], tmp[2]);
      insertVal(form, "sNetmask", (!tmp[3]) ? "<% getSpotNetmask(); %>" : tmp[3], tmp[3]);
      insertVal(form, "sStartIP", (!tmp[4]) ? "<% getCfgGeneral(1, "chilli_dhcpstart"); %>" : tmp[4], tmp[4]);
      insertVal(form, "sEndIP", (!tmp[5]) ? "<% getCfgGeneral(1, "chilli_dhcpend"); %>" : tmp[5], tmp[5]);
      insertVal(form, "sPriDns", (!tmp[6]) ? "<% getCfgGeneral(1, "chilli_dns1"); %>" : tmp[6], tmp[6]);
      insertVal(form, "sSecDns", (!tmp[7]) ? "<% getCfgGeneral(1, "chilli_dns2"); %>" : tmp[7], tmp[7]);
      insertVal(form, "sDomain", (!tmp[8]) ? "<% getCfgGeneral(1, "chilli_domain"); %>" : tmp[8], tmp[8]);
      insertVal(form, "sLease", (!tmp[9]) ? "<% getCfgGeneral(1, "chilli_lease"); %>" : tmp[9], tmp[9]);
      insertVal(form, "sRadServer1", (!tmp[10]) ? "<% getCfgGeneral(1, "chilli_radiusserver1"); %>" : tmp[10], tmp[10]);
      insertVal(form, "sRadServer2", (!tmp[11]) ? "<% getCfgGeneral(1, "chilli_radiusserver2"); %>" : tmp[11], tmp[11]);
      insertVal(form, "sRadSecret", (!tmp[12]) ? "<% getCfgGeneral(1, "chilli_radiussecret"); %>" : tmp[12], tmp[12]);
      insertVal(form, "sNasId", (!tmp[13]) ? "<% getCfgGeneral(1, "chilli_radiusnasid"); %>" : tmp[13], tmp[13]);
      insertVal(form, "sRadLocId", (!tmp[14]) ? "<% getCfgGeneral(1, "chilli_radiuslocationid"); %>" : tmp[14], tmp[14]);
      insertVal(form, "sRadLocName", (!tmp[15]) ? "<% getCfgGeneral(1, "chilli_radiuslocationname"); %>" : tmp[15], tmp[15]);
      insertVal(form, "sRadCoaPort", (!tmp[16]) ? "<% getCfgGeneral(1, "chilli_coaport"); %>" : tmp[16], tmp[16]);
      insertVal(form, "sRadCoaNoIpCheck", (!tmp[17]) ? "<% getCfgGeneral(1, "chilli_coanoipcheck"); %>" : tmp[17], tmp[17]);
      if(!tmp[17])
        form.sRadCoaNoIpCheck.value = ("<% getCfgGeneral(1, "chilli_coanoipcheck"); %>" == "on") ? "on" : "off";
      insertVal(form, "sUamServer", (!tmp[18]) ? "<% getCfgGeneral(1, "chilli_uamserver"); %>" : tmp[18], tmp[18]);
      insertVal(form, "sUamHomepage", (!tmp[19]) ? "<% getCfgGeneral(1, "chilli_uamhomepage"); %>" : tmp[19], tmp[19]);
      insertVal(form, "sUamSecret", (!tmp[20]) ? "<% getCfgGeneral(1, "chilli_uamsecret"); %>" : tmp[20], tmp[20]);
      insertVal(form, "sUamAllowed", (!tmp[21]) ? "<% getCfgGeneral(1, "chilli_uamallowed"); %>" : tmp[21], tmp[21]);
      insertVal(form, "sUamDomain", (!tmp[22]) ? "<% getCfgGeneral(1, "chilli_uamdomain"); %>" : tmp[22], tmp[22]);
      insertVal(form, "sUamAnyDNS", (!tmp[23]) ? "<% getCfgGeneral(1, "chilli_uamanydns"); %>" : tmp[23], tmp[23]);
      if(!tmp[23])
        form.sUamAnyDNS.value = ("<% getCfgGeneral(1, "chilli_uamanydns"); %>" == "on") ? "on" : "off";
      insertVal(form, "sMacAllowed", (!tmp[24]) ? "<% getCfgGeneral(1, "chilli_macallowed"); %>" : tmp[24], tmp[24]);
    }
  }
}

function MACMechanismOnChange(form) {
  displayElement("row_AllowedMACList", form.MACMechanism.value == "allow");
  displayElement("row_BlockedMACList", form.MACMechanism.value == "block");
  displayElement("row_Password", (form.PasswordAuthentication.value*1) == 1);
  displayElement("row_Username", (form.UsernameAuthentication.value*1) == 1);
}

function initValue() {
  var form = document.spotCfg;

  var nat_fp = defaultNumber("<% getCfgGeneral(1, "offloadMode"); %>", "1");

  var enabled = form.spotEnable;
  enabled.options.length = 0;
  addOption(enabled, _("button disable"), "0");
  if(chilli_built)
    addOption(enabled, "ChilliSpot", "1");
  if(nodog_built)
    addOption(enabled, "NoDogSplash", "2");

  if(chilli_built && (form.chilliEnable.value == "on"))
    enabled.value = "1";
  else if(nodog_built && (form.nodogEnable.value == "1"))
    enabled.value = "2";
  else
    enabled.value = "0";

  var prof = form.spotProfile;
  prof.options.length = 0;
  for(var i = 0; i < Profiles.length; i++) {
    addOption(prof, Profiles[i][1], Profiles[i][0]);
    if(Profiles[i][0] == old_profile)
      prof.value = old_profile;
  }
  if(old_profile == "0")
    prof.options.selectedIndex == 0;

  form.AuthenticateImmediately.value = ("<% getCfgGeneral(1, "nodog_AuthenticateImmediately"); %>" == "1") ? "1" : "0";
  form.MACMechanism.value = ("<% getCfgGeneral(1, "nodog_MACMechanism"); %>" == "allow") ? "allow" : "block";
  form.PasswordAuthentication.value = ("<% getCfgGeneral(1, "nodog_PasswordAuthentication"); %>" == "1") ? "1" : "0";
  form.UsernameAuthentication.value = ("<% getCfgGeneral(1, "nodog_UsernameAuthentication"); %>" == "1") ? "1" : "0";

  ModeOnChange(form);
  initTranslation();
}

function CheckValue(form) {
  if(form.spotEnable.value == "1")
    for(var i = 0; i < Profiles.length; i++) {
      var tmp = Profiles[i];
      if (tmp[0] == form.spotProfile.value) {
        // Check Network
        if ((tmp[2] == false) && !validateIP(form.sIp, true)) {
          alert(_("ipv6 invalid ipv4"));
          form.sIp.focus();
          form.sIp.select();
          return false;
        }
        // Check Netmask
        if ((tmp[3] == false) && !validateIP(form.sNetmask, true)) {
          alert(_("ipv6 invalid ipv4"));
          form.sNetmask.focus();
          form.sNetmask.select();
          return false;
        }
        // Check start IP
        if ((tmp[4] == false) && !checkDigitRange(form.sStartIP.value, 1, 254)) {
          alert(_("hotspot expects number"));
          form.sStartIP.focus();
          form.sStartIP.select();
          return false;
        }
        // Check end IP
        if ((tmp[5] == false) && !checkDigitRange(form.sEndIP.value, 1, 254)) {
          alert(_("hotspot expects number"));
          form.sEndIP.focus();
          form.sEndIP.select();
          return false;
        }
        // Check Primary DNS
        if ((tmp[6] == false) && !validateIP(form.sPriDns, true)) {
          alert(_("ipv6 invalid ipv4"));
          form.sPriDns.focus();
          form.sPriDns.select();
          return false;
        }
        // Check Secondery DNS
        if ((tmp[7] == false) && !validateIP(form.sSecDns, true)) {
          alert(_("ipv6 invalid ipv4"));
          form.sSecDns.focus();
          form.sSecDns.select();
          return false;
        }
        // Check Domain
        if ((tmp[8] == false) && (form.sDomain.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sDomain.focus();
          form.sDomain.select();
          return false;
        }
        // Check sLease
        if ((tmp[9] == false) && !/^\d+$/.test(form.sLease.value)) {
          alert(_("hotspot expects number"));
          form.sLease.focus();
          form.sLease.select();
          return false;
        }
        // Check sRadServer1
        if ((tmp[10] == false) && (form.sRadServer1.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sRadServer1.focus();
          form.sRadServer1.select();
          return false;
        }
        // Check sRadServer2
        if ((tmp[11] == false) && (form.sRadServer2.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sRadServer2.focus();
          form.sRadServer2.select();
          return false;
        }
        // Check sRadSecret
        if ((tmp[12] == false) && (form.sRadSecret.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sRadSecret.focus();
          form.sRadSecret.select();
          return false;
        }
        // Check sNasId
        if ((tmp[13] == false) && (form.sNasId.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sNasId.focus();
          form.sNasId.select();
          return false;
        }
        // Check sRadLocId
        if ((tmp[14] == false) && (form.sRadLocId.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sRadLocId.focus();
          form.sRadLocId.select();
          return false;
        }
        // Check sRadLocName
        if ((tmp[15] == false) && (form.sRadLocName.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sRadLocName.focus();
          form.sRadLocName.select();
          return false;
        }
        // Check sRadCoaPort
        if ((tmp[16] == false) && !/^\d+$/.test(form.sRadCoaPort.value)) {
          alert(_("hotspot expects number"));
          form.sRadCoaPort.focus();
          form.sRadCoaPort.select();
          return false;
        }
        // Check sUamServer
        if ((tmp[18] == false) && (form.sUamServer.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sUamServer.focus();
          form.sUamServer.select();
          return false;
        }
        // Check sUamHomepage
        if ((tmp[19] == false) && (form.sUamHomepage.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sUamHomepage.focus();
          form.sUamHomepage.select();
          return false;
        }
        // Check sUamSecret
        if ((tmp[20] == false) && (form.sUamSecret.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sUamSecret.focus();
          form.sUamSecret.select();
          return false;
        }
        // Check sUamAllowed
        if ((tmp[21] == false) && (form.sUamAllowed.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sUamAllowed.focus();
          form.sUamAllowed.select();
          return false;
        }
        // Check sUamDomain
        if ((tmp[22] == false) && (form.sUamDomain.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sUamDomain.focus();
          form.sUamDomain.select();
          return false;
        }
        // Check sMacAllowed
        if ((tmp[24] == false) && (form.sMacAllowed.value.indexOf(" ") >= 0)) {
          alert(_("hotspot dont space"));
          form.sMacAllowed.focus();
          form.sMacAllowed.select();
          return false;
        } else if (validateMACList(form.sMacAllowed.value.split(",")), false) {
          alert(_("hotspot mac list"));
          form.sMacAllowed.focus();
          form.sMacAllowed.select();
          return false;
        }
      }
    }
  else if(form.spotEnable.value == "2")
    // Check sMacAllowed
    if (form.RedirectURL.value.indexOf(" ") >= 0) {
      alert(_("hotspot dont space"));
      form.RedirectURL.focus();
      form.RedirectURL.select();
      return false;
    } else if (!/^\d+$/.test(form.MaxClients.value)) {
      alert(_("hotspot expects number"));
      form.MaxClients.focus();
      form.MaxClients.select();
      return false;
    } else if (!/^\d+$/.test(form.ClientIdleTimeout.value)) {
      alert(_("hotspot expects number"));
      form.ClientIdleTimeout.focus();
      form.ClientIdleTimeout.select();
      return false;
    } else if (!/^\d+$/.test(form.ClientForceTimeout.value)) {
      alert(_("hotspot expects number"));
      form.ClientForceTimeout.focus();
      form.ClientForceTimeout.select();
      return false;
    } else if ((form.MACMechanism.value == "block") && (form.BlockedMACList.value.indexOf(" ") >= 0)) {
      alert(_("hotspot dont space"));
      form.BlockedMACList.focus();
      form.BlockedMACList.select();
      return false;
    } else if ((form.MACMechanism.value == "allow") && (form.AllowedMACList.value.indexOf(" ") >= 0)) {
      alert(_("hotspot dont space"));
      form.AllowedMACList.focus();
      form.AllowedMACList.select();
      return false;
    } else if (form.TrustedMACList.value.indexOf(" ") >= 0) {
      alert(_("hotspot dont space"));
      form.TrustedMACList.focus();
      form.TrustedMACList.select();
      return false;
    } else if (form.Password.value.indexOf(" ") >= 0) {
      alert(_("hotspot dont space"));
      form.Password.focus();
      form.Password.select();
      return false;
    } else if (form.Username.value.indexOf(" ") >= 0) {
      alert(_("hotspot dont space"));
      form.Username.focus();
      form.Username.select();
      return false;
    } else if (!/^\d+$/.test(form.PasswordAttempts.value)) {
      alert(_("hotspot expects number"));
      form.PasswordAttempts.focus();
      form.PasswordAttempts.select();
      return false;
    }
  return true;
}

function resetClick(form) {
    form.reset.value = "1";
    form.submit();
}
</script>
</head>
<body onLoad="initValue();">
  <table class="body">
    <tr>
      <td>
        <h1 id="sTitle"></h1>
        <p id="sIntroduction"></p>
        <hr />
        <form method="POST" name="spotCfg" action="/goform/setHotspot" onSubmit="return CheckValue(this);">
          <table class="form">
            <tr onMouseOver="showHint('spot_enable');" onMouseOut="hideHint();">
              <td class="title" id="spotSetup">Hotspot Setup</td>
              <td class="title" style="text-align:right"><select name="spotEnable" id="spotEnable" class="normal" onChange="ModeOnChange(this.form);"></select></td>
            </tr>
            <!-- Coova setup -->
            <tr id="row_chilli" style="display:none;" onMouseOver="showHint('spot_profile');" onMouseOut="hideHint();">
              <td class="head" id="spotProfile">Chilli profile</td>
              <td>
                <select name="spotProfile" class="mid" onChange="ProfileOnChange(this.form);"></select>
              </td>
            </tr>
            <tr id="row_sIp" style="display:none;" onMouseOver="showHint('spot_ip');" onMouseOut="hideHint();">
              <td class="head" id="sIp">Subnet Address</td>
              <td><input name="sIp" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sNetmask" style="display:none;" onMouseOver="showHint('spot_mask');" onMouseOut="hideHint();">
              <td class="head" id="sNetmask">Subnet Mask</td>
              <td><input name="sNetmask" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sStartIP" style="display:none;" onMouseOver="showHint('spot_startip');" onMouseOut="hideHint();">
              <td class="head" id="sStartIP">Start IP</td>
              <td><input name="sStartIP" maxlength="3" value=""></td>
            </tr>
            <tr id="row_sEndIP" style="display:none;" onMouseOver="showHint('spot_endip');" onMouseOut="hideHint();">
              <td class="head" id="sEndIP">End IP</td>
              <td><input name="sEndIP" maxlength="3" value=""></td>
            </tr>
            <tr id="row_sPriDns" style="display:none;" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
              <td class="head" id="sPriDns">Primary DNS Server</td>
              <td><input name="sPriDns" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sSecDns" style="display:none;" onMouseOver="showHint('spot_dns');" onMouseOut="hideHint();">
              <td class="head" id="sSecDns">Secondary DNS Server</td>
              <td><input name="sSecDns" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sDomain" style="display:none;" onMouseOver="showHint('spot_domain');" onMouseOut="hideHint();">
              <td class="head" id="sDomain">Domain name</td>
              <td><input name="sDomain" maxlength="64" value=""></td>
            </tr>
            <tr id="row_sLease" style="display:none;" onMouseOver="showHint('spot_lease');" onMouseOut="hideHint();">
              <td class="head" id="sLease">Lease time (sec)</td>
              <td><input name="sLease" maxlength="8" value=""></td>
            </tr>
            <tr id="row_sRadServer1" style="display:none;" onMouseOver="showHint('spot_auth');" onMouseOut="hideHint();">
              <td class="head" id="sRadServer1">Radius server auth address</td>
              <td><input name="sRadServer1" maxlength="256" value=""></td>
            </tr>
            <tr id="row_sRadServer2" style="display:none;" onMouseOver="showHint('spot_acct');" onMouseOut="hideHint();">
              <td class="head" id="sRadServer2">Radius server acct address</td>
              <td><input name="sRadServer2" maxlength="256" value=""></td>
            </tr>
            <tr id="row_sRadSecret" style="display:none;" onMouseOver="showHint('spot_rad_secret');" onMouseOut="hideHint();">
              <td class="head" id="sRadSecret">Radius server secret</td>
              <td><input name="sRadSecret" maxlength="256" value="" type="password"></td>
            </tr>
            <tr id="row_sNasId" style="display:none;" onMouseOver="showHint('spot_nasid');" onMouseOut="hideHint();">
              <td class="head" id="sNasId">Radius NAS ID</td>
              <td><input name="sNasId" maxlength="256" value=""></td>
            </tr>
            <tr id="row_sRadLocId" style="display:none;" onMouseOver="showHint('spot_locid');" onMouseOut="hideHint();">
              <td class="head" id="sRadLocId">Radius Location ID</td>
              <td><input name="sRadLocId" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sRadLocName" style="display:none;" onMouseOver="showHint('spot_locname');" onMouseOut="hideHint();">
              <td class="head" id="sRadLocName">Radius Location Name</td>
              <td><input name="sRadLocName" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sRadCoaPort" style="display:none;" onMouseOver="showHint('spot_port');" onMouseOut="hideHint();">
              <td class="head" id="sRadCoaPort">Radius CoA/DM port</td>
              <td><input name="sRadCoaPort" maxlength="15" value=""></td>
            </tr>
            <tr id="row_sRadCoaNoIpCheck" style="display:none;" onMouseOver="showHint('spot_ipcheck');" onMouseOut="hideHint();">
              <td class="head" id="sRadCoaNoIpCheck">No IP check for CoA/DM request</td>
              <td>
                <select name="sRadCoaNoIpCheck" class="normal">
                  <option value="off" id="disable">Disable</option>
                  <option value="on" id="enable">Enable</option>
                </select>
              </td>
            </tr>
            <tr id="row_sUamServer" style="display:none;" onMouseOver="showHint('spot_uamserver');" onMouseOut="hideHint();">
              <td class="head" id="sUamServer">UAM server URL</td>
              <td><input name="sUamServer" maxlength="256" value=""></td>
            </tr>
            <tr id="row_sUamHomepage" style="display:none;" onMouseOver="showHint('spot_uamhomepage');" onMouseOut="hideHint();">
              <td class="head" id="sUamHomepage">UAM homepage URL</td>
              <td><input name="sUamHomepage" maxlength="256" value=""></td>
            </tr>
            <tr id="row_sUamSecret" style="display:none;" onMouseOver="showHint('spot_uamsecret');" onMouseOut="hideHint();">
              <td class="head" id="sUamSecret">UAM secret</td>
              <td><input name="sUamSecret" maxlength="256" value="" type="password"></td>
            </tr>
            <tr id="row_sUamAllowed" style="display:none;" onMouseOver="showHint('spot_uamallowed');" onMouseOut="hideHint();">
              <td class="head" id="sUamAllowed">UAM allowed hosts</td>
              <td><input name="sUamAllowed" maxlength="1024" value=""></td>
            </tr>
            <tr id="row_sUamDomain" style="display:none;" onMouseOver="showHint('spot_uamdomain');" onMouseOut="hideHint();">
              <td class="head" id="sUamDomain">UAM allowed domains</td>
              <td><input name="sUamDomain" maxlength="1024" value=""></td>
            </tr>
            <tr id="row_sUamAnyDNS" style="display:none;" onMouseOver="showHint('spot_uamdns');" onMouseOut="hideHint();">
              <td class="head" id="sUamAnyDNS">Allow all DNS requests</td>
              <td>
                <select name="sUamAnyDNS" class="normal">
                  <option value="off" id="disable">Disable</option>
                  <option value="on" id="enable">Enable</option>
                </select>
              </td>
            </tr>
            <tr id="row_sMacAllowed" style="display:none;" onMouseOver="showHint('spot_macallowed');" onMouseOut="hideHint();">
              <td class="head" id="sMacAllowed">Allowed MAC</td>
              <td><input name="sMacAllowed" maxlength="512" value=""></td>
            </tr>
            <!-- Nodog setup -->
            <tr id="row_GatewayIPRange" style="display:none;" onMouseOver="showHint('GatewayIPRange');" onMouseOut="hideHint();">
              <td class="head" id="GatewayIPRange">Range of IP addresses</td>
              <td><input name="GatewayIPRange" maxlength="128" value="<% getCfgGeneral(1, "nodog_GatewayIPRange"); %>"></td>
            </tr>
            <tr id="row_RedirectURL" style="display:none;" onMouseOver="showHint('RedirectURL');" onMouseOut="hideHint();">
              <td class="head" id="RedirectURL">Redirected to URL after authentication</td>
              <td><input name="RedirectURL" maxlength="128" value="<% getCfgGeneral(1, "nodog_RedirectURL"); %>"></td>
            </tr>
            <tr id="row_MaxClients" style="display:none;" onMouseOver="showHint('MaxClients');" onMouseOut="hideHint();">
              <td class="head" id="MaxClients">Maximum number of users</td>
              <td><input name="MaxClients" maxlength="128" value="<% getCfgGeneral(1, "nodog_MaxClients"); %>"></td>
            </tr>
            <tr id="row_ClientIdleTimeout" style="display:none;" onMouseOver="showHint('ClientIdleTimeout');" onMouseOut="hideHint();">
              <td class="head" id="ClientIdleTimeout">Inactivity before a user is 'deauthenticated'</td>
              <td><input name="ClientIdleTimeout" maxlength="128" value="<% getCfgGeneral(1, "nodog_ClientIdleTimeout"); %>"></td>
            </tr>
            <tr id="row_ClientForceTimeout" style="display:none;" onMouseOver="showHint('ClientForceTimeout');" onMouseOut="hideHint();">
              <td class="head" id="ClientForceTimeout">Minutes before a user is 'deauthenticated'</td>
              <td><input name="ClientForceTimeout" maxlength="128" value="<% getCfgGeneral(1, "nodog_ClientForceTimeout"); %>"></td>
            </tr>
            <tr id="row_AuthenticateImmediately" style="display:none;" onMouseOver="showHint('AuthenticateImmediately');" onMouseOut="hideHint();">
              <td class="head" id="AuthenticateImmediately">Authenticate Immediately</td>
              <td>
                <select name="AuthenticateImmediately" class="normal">
                  <option value=0 id="disable">Disable</option>
                  <option value=1 id="enable">Enable</option>
                </select>
              </td>
            </tr>
            <tr id="row_MACMechanism" style="display:none;" onMouseOver="showHint('MACMechanism');" onMouseOut="hideHint();">
              <td class="head" id="MACMechanism">MAC Mechanism</td>
              <td>
                <select name="MACMechanism" class="normal" onChange="MACMechanismOnChange(this.form);">
                  <option value="block" id="block">Block</option>
                  <option value="allow" id="allow">Allow</option>
                </select>
              </td>
            </tr>
            <tr id="row_BlockedMACList" style="display:none;" onMouseOver="showHint('BlockedMACList');" onMouseOut="hideHint();">
              <td class="head" id="BlockedMACList">List Blocked MAC Addresses</td>
              <td><input name="BlockedMACList" maxlength="128" value="<% getCfgGeneral(1, "nodog_BlockedMACList"); %>"></td>
            </tr>
            <tr id="row_AllowedMACList" style="display:none;" onMouseOver="showHint('AllowedMACList');" onMouseOut="hideHint();">
              <td class="head" id="AllowedMACList">List Allowed MAC Addresses</td>
              <td><input name="AllowedMACList" maxlength="128" value="<% getCfgGeneral(1, "nodog_AllowedMACList"); %>"></td>
            </tr>
            <tr id="row_TrustedMACList" style="display:none;" onMouseOver="showHint('TrustedMACList');" onMouseOut="hideHint();">
              <td class="head" id="TrustedMACList">List Trusted MAC Addresses</td>
              <td><input name="TrustedMACList" maxlength="128" value="<% getCfgGeneral(1, "nodog_TrustedMACList"); %>"></td>
            </tr>
            <tr id="row_PasswordAuthentication" style="display:none;" onMouseOver="showHint('PasswordAuthentication');" onMouseOut="hideHint();">
              <td class="head" id="PasswordAuthentication">Password Authentication</td>
              <td>
                <select name="PasswordAuthentication" class="normal" onChange="MACMechanismOnChange(this.form);">
                  <option value=0 id="disable">Disable</option>
                  <option value=1 id="enable">Enable</option>
                </select>
              </td>
            </tr>
            <tr id="row_Password" style="display:none;" onMouseOver="showHint('Password');" onMouseOut="hideHint();">
              <td class="head" id="Password">Password</td>
              <td><input type="password" name="Password" maxlength="128" value="<% getCfgGeneral(1, "nodog_Password"); %>"></td>
            </tr>
            <tr id="row_UsernameAuthentication" style="display:none;" onMouseOver="showHint('UsernameAuthentication');" onMouseOut="hideHint();">
              <td class="head" id="UsernameAuthentication">Username Authentication</td>
              <td>
                <select name="UsernameAuthentication" class="normal" onChange="MACMechanismOnChange(this.form);">
                  <option value=0 id="disable">Disable</option>
                  <option value=1 id="enable">Enable</option>
                </select>
              </td>
            </tr>
            <tr id="row_Username" style="display:none;" onMouseOver="showHint('Username');" onMouseOut="hideHint();">
              <td class="head" id="Username">Username</td>
              <td><input name="Username" maxlength="128" value="<% getCfgGeneral(1, "nodog_Username"); %>"></td>
            </tr>
            <tr id="row_PasswordAttempts" style="display:none;" onMouseOver="showHint('PasswordAttempts');" onMouseOut="hideHint();">
              <td class="head" id="PasswordAttempts">Password Attempts</td>
              <td><input name="PasswordAttempts" maxlength="128" value="<% getCfgGeneral(1, "nodog_PasswordAttempts"); %>"></td>
            </tr>
          </table>
          <table class="button">
            <tr>
              <td>
                <input type="submit" class="normal" value="Apply" id="sApply">&nbsp;&nbsp;
                <input type="reset"  class="normal" value="Cancel" id="sCancel">&nbsp;&nbsp;
                <input type="button" class="normal" value="Reset" id="sReset" onClick="resetClick(this.form);">
                <input type="hidden" name="reset" value="0">
                <input type="hidden" name="submit-url" value="/internet/hotspot.asp" >
                <input type="hidden" name="chilliEnable" value="<% getCfgZero(1, "chilli_enable"); %>" >
                <input type="hidden" name="nodogEnable" value="<% getCfgZero(1, "nodogsplash_enable"); %>" >
              </td>
            </tr>
          </table>
        </form>
        <div id="hint_row">&nbsp;</div>
        <div class="whitespace">&nbsp;</div>
      </td>
    </tr>
  </table>
</body>
</html>
