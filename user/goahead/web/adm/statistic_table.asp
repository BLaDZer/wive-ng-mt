<table class="form">
  <tbody>
    <tr id="wirelessAbout" style="display:none;">
      <td class="title" colspan="6" id="statWiFiAbout">Wireless About</td>
    </tr>
    <tr id="wirelessDriverVersion" style="display:none;">
      <td class="head" colspan="3" id="statWiFiVer">Driver Version</td>
      <td colspan="3"><% getStaDriverVer(); %></td>
    </tr>
    <tr id="wirelessMacAddr" style="display:none;">
      <td class="head" colspan="3" id="statWiFiMAC">Mac Address</td>
      <td colspan="3"><% getStaMacAddrw(); %></td>
    </tr>
    <!-- ==============  MEMORY and CPU  =============== -->
    <tr>
      <td class="title" colspan="6" id="statisticMMCPU">Memory and CPU</td>
    </tr>
    <tr>
      <td class="head" id="statisticMMTotal" colspan="3">Memory total: </td>
      <td colspan="3"><% getMemTotalASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticMMLeft" colspan="3">Memory left: </td>
      <td colspan="3"><% getMemLeftASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticCpuUse" colspan="3">CPU usage: </td>
      <td colspan="3"><% getCpuUsageASP(); %></td>
    </tr>
    <!-- =================  WAN/LAN  ================== -->
    <tr>
      <td class="title" colspan="6" id="statisticWANLAN">WAN/LAN</td>
    </tr>
    <tr>
      <td class="head" id="statisticWANRxPkt" colspan="3">WAN Rx packets:</td>
      <td colspan="3"><% getWANRxPacketASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticWANRxBytes" colspan="3">WAN Rx bytes:</td>
      <td colspan="3"><% getWANRxByteASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticWANTxPkt" colspan="3">WAN Tx packets:</td>
      <td colspan="3"><% getWANTxPacketASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticWANTxBytes" colspan="3">WAN Tx bytes:</td>
      <td colspan="3"><% getWANTxByteASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticLANRxPkt" colspan="3">LAN Rx packets:</td>
      <td colspan="3"><% getLANRxPacketASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticLANRxBytes" colspan="3">LAN Rx bytes:</td>
      <td colspan="3"><% getLANRxByteASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticLANTxPkt" colspan="3">LAN Tx packets:</td>
      <td colspan="3"><% getLANTxPacketASP(); %></td>
    </tr>
    <tr>
      <td class="head" id="statisticLANTxBytes" colspan="3">LAN Tx bytes:</td>
      <td colspan="3"><% getLANTxByteASP(); %></td>
    </tr>
    <!-- =================  ALL  ================= -->
    <tr>
      <td class="title" colspan="6" id="statisticAllIF">All interfaces</td>
    <tr>
    <tr>
      <th colspan="2" id="statisticIfName">Name</th>
      <th id="statisticRxPkt">Rx Packets</th>
      <th id="statisticRxBytes">Rx Bytes</th>
      <th id="statisticTxPkt">Tx Packets</th>
      <th id="statisticTxBytes">Tx Bytes</th>
    </tr>
    <% getAllNICStatisticASP(); %>
  </tbody>
</table>
<table class="form" id="statisticHWStats">
  <tr>
    <td class="title" colspan="6" id="statisPorts">Physical ports</td>
  </tr>
  <tr id="statisticPorts">
    <td class="head"></td>
    <td class="head"><span id="Port">Port</span> 1</td>
    <td class="head"><span id="Port">Port</span> 2</td>
    <td class="head"><span id="Port">Port</span> 3</td>
    <td class="head"><span id="Port">Port</span> 4</td>
    <td class="head"><span id="Port">Port</span> 5</td>
  </tr>
  <% getHWStatistic(); %>
</table>
