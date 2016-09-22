<input type="hidden" id="vlanPort_status"         value='<% getPortStatusJSON(); %>'>
<input type="hidden" id="vlanMode_WifiLan"        value='<% getCfgGeneral(1, "VlanWifiLan"); %>'>
<input type="hidden" id="vlanMode_WifiLanINIC"    value='<% getCfgGeneral(1, "VlanWifiLanINIC"); %>'>
<input type="hidden" id="vlanMode_WifiWan"        value='<% getCfgGeneral(1, "VlanWifiWan"); %>'>
<input type="hidden" id="vlanMode_WifiWanINIC"    value='<% getCfgGeneral(1, "VlanWifiWanINIC"); %>'>
<input type="hidden" id="vlanMode_MBSSID_mode"    value='<% getCfgGeneral(1, "BssidIfName"); %>'>
<input type="hidden" id="vlanMode_SSID_num"       value='<% getCfgGeneral(1, "BssidNum"); %>'>
<input type="hidden" id="vlanMode_SsidList"       value='<% getSSIDsList(); %>'>
<input type="hidden" id="vlanMode_VLANlan"        value='<% getCfgGeneral(1, "VlanLan"); %>'>
<input type="hidden" id="vlanMode_VLANlanIsolate" value='<% getCfgGeneral(1, "VlanLanIsolate"); %>'>