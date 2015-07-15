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
  </tbody>
</table>
<table class="form" id="statisticHWStats">
  <tr>
    <td class="title" colspan="6" id="statisPorts">Physical ports</td>
  </tr>
  <tr id="statisticPorts">
  </tr>
  <% getHWStatistic(); %>
</table>
<input type="hidden" id="inpWanPort" value='<% getPortStatus(); %>'>
    <!-- =================  ALL  ================= -->
<table class="form">
  <tbody>
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
