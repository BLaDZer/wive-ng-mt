<table class="form">
  <tr>
    <td class="title" colspan="5" id="dClients">DHCP Clients</td>
  </tr>
  <tr style="background-color: #E8F8FF">
    <th id="dHostname">Hostname</th>
    <th id="dMAC">MAC Address</th>
    <th id="dIP">IP Address</th>
    <th id="dExpires">Expires in</th>
    <th id="dStatic">Static</th>
  </tr>
  <% getDhcpCliList(); %>
</table>
