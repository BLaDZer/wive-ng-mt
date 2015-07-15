<table class="form" >
  <!-- ================= System Info ================= -->
  <tr>
    <td class="title" colspan="2" id="statusSysInfo">System Info</td>
  </tr>
  <tr>
    <td class="head" id="statusSDKVersion">Firmware Version</td>
    <td><% getSdkVersion(); %></td>
  </tr>
  <tr>
    <td class="head" id="statusSysDateTime">System Time</td>
    <td><% getSysDateTime(); %></td>
  </tr>
  <tr>
    <td class="head" id="statusSysUpTime">System Uptime</td>
    <td><% getSysUptime(); %></td>
  </tr>
  <tr>
    <td class="head" id="statusSysPlatform">System Platform</td>
    <td><% getPlatform(); %></td>
  </tr>
  <tr>
    <td class="head"><input type="hidden" id="inpOperationMode" value='<% getCfgZero(1, "OperationMode"); %>'><span id="statusOPMode">Operation Mode</span></td>
    <td id="tdOperationMode">&nbsp;</td>
  </tr>
  <!-- ================= Internet Configurations ================= -->
  <tr>
    <td class="title" colspan="2" id="statusInternetConfig">Internet Configuration</td>
  </tr>
  <tr>
    <td class="head" id="statusConnectedType">Connection Mode</td>
    <td><% getCfgGeneral(1, "wanConnectionMode"); %></td>
  </tr>
  <tr>
    <td class="head" id="statusINTIPAddr">Internet/VPN IP Address</td>
    <td><% getIntIp(); %>
      &nbsp;</td>
  </tr>
  <tr>
    <td class="head" id="statusWANIPAddr">Physical WAN IP Address</td>
    <td><% getWanIp(); %>
      &nbsp;</td>
  </tr>
  <tr>
    <td class="head" id="statusSubnetMask">Subnet Mask</td>
    <td><% getWanNetmask(); %>
      &nbsp;</td>
  </tr>
  <tr id="defaultGW">
    <td class="head" id="statusDefaultGW">Default Gateway</td>
    <td id="statusDGW"></td>
  </tr>
  <tr>
    <td class="head" id="statusPrimaryDNS">Primary Domain Name Server</td>
    <td><% getDns(1); %></td>
  </tr>
  <tr>
    <td class="head" id="statusSecondaryDNS">Secondary Domain Name Server</td>
    <td><% getDns(2); %></td>
  </tr>
  <tr id="statusIPv6Int">
    <td class="head" id="statusIPv6IntAddr">Internal IPv6 Address</td>
    <td id="IPv6IntAddr"></td>
  </tr>
  <tr id="statusIPv6Ext">
    <td class="head" id="statusIPv6ExtAddr">External IPv6 Address</td>
    <td id="IPv6ExtAddr"></td>
  </tr>
  <tr>
    <td class="head" id="statusWANMAC">MAC Address</td>
    <td><% getWanMac(); %></td>
  </tr>
  <!-- ================= Local Network ================= -->
  <tr>
    <td class="title" colspan="2" id="statusLocalNet">Local Network</td>
  </tr>
  <tr>
    <td class="head" id="statusLANIPAddr">Local IP Address</td>
    <td><% getLanIp(); %></td>
  </tr>
  <tr>
    <td class="head" id="statusLocalNetmask">Local Netmask</td>
    <td><% getLanNetmask(); %></td>
  </tr>
  <tr>
    <td class="head" id="statusLANMAC">MAC Address</td>
    <td><% getLanMac(); %></td>
  </tr>
</table>
