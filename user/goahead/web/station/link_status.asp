<!DOCTYPE html>
<html>
<head>
<title>Station Link Status</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0">
<meta http-equiv="Pragma" content="no-cache">
<script type="text/javascript" src="/lang/b28n.js"></script>
<script type="text/javascript" src="/js/controls.js"></script>
<script type="text/javascript" src="/js/ajax.js"></script>
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<script language="JavaScript" type="text/javascript">
Butterlate.setTextDomain("wireless");
Butterlate.setTextDomain("buttons");

function initTranslation()
{
	_TR("linkTitle", "link title");
	_TR("linkIntroduction", "link introduction");

	_TR("linkLinkStatus", "link link status");
	_TR("linkStatus", "link status");
	_TR("linkExtraInfo", "link extra info");
	_TR("linkChannel", "station channel");
	_TR("linkSpeed", "link speed");
	_TR("linkThroughput", "link throughput");
	_TR("linkQuality", "link quality");
	_TR("linkSigStrength1", "link signal strength");
	_TR("linkSigStrength2", "link signal strength");
	_TR("linkSigStrength3", "link signal strength");
	_TR("linkNoiseLevel", "link noise level");

	_TR("linkHT", "link ht");
	_TR("linkSNR", "link snr");
}

function initValues()
{
	var modes = '<% getStaTrModes(); %>' * 1;
	displayElement('rowSignalStrength2', modes>=2);
	displayElement('rowSignalStrength3', modes>=3);
	var el = document.getElementById('colDbmChecked');
	el.rowspan = modes + 1;
}

function PageInit()
{
	initTranslation();
	initValues();
}
</script>
</head>

<body bgcolor="#FFFFFF" onLoad="PageInit()">
<table class="body">
  <tr>
    <td><h1 id="linkTitle">Station Link Status</h1>
      <p id="linkIntroduction">The Status page shows the settings and current operation status of the Station.</p>
      <hr />
      <form method="post" name="sta_link_status" action="/goform/setStaDbm" OnSubmit="ajaxShowTimer(form, 'timerReloader', _('message apply'), 15);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td colspan="3" class="title" id="linkLinkStatus">Link Status</td>
          </tr>
          <tr>
            <td class="head" id="linkStatus">Status</td>
            <td colspan="2"><% getStaLinkStatus(); %></td>
          </tr>
          <tr>
            <td class="head" id="linkExtraInfo">Extra Info</td>
            <td colspan="2"><% getStaExtraInfo(); %></td>
          </tr>
          <tr>
            <td class="head" id="linkChannel">Channel</td>
            <td colspan="2"><% getStaLinkChannel(); %></td>
          </tr>
          <tr>
            <td class="head" id="linkSpeed">Link Speed</td>
            <td>Tx(Mbps)&nbsp;&nbsp;
              <% getStaLinkTxRate(); %></td>
            <td>Rx(Mbps)&nbsp;&nbsp;
              <% getStaLinkRxRate(); %></td>
          </tr>
          <tr>
            <td class="head" id="linkThroughput">Throughput</td>
            <td>Tx(Kbps)&nbsp;&nbsp;
              <% getStaTxThroughput(); %></td>
            <td>Rx(Kbps)&nbsp;&nbsp;
              <% getStaRxThroughput(); %></td>
          </tr>
          <tr>
            <td class="head" id="linkQuality">Link Quality</td>
            <td colspan="2"><% getStaLinkQuality(); %></td>
          </tr>
          <tr>
            <td class="head"><font id="linkSigStrength1">Signal Strength </font>1</td>
            <td><% getStaSignalStrength(); %></td>
            <td id="colDbmChecked" rowspan="4"><input type="checkbox" name="dbmChecked" <% dbm = getStaDbm(); if (dbm == "1") write("checked"); %> onClick="submit();">
              dBm format</td>
          </tr>
          <tr id="rowSignalStrength2">
            <td class="head"><font id="linkSigStrength2">Signal Strength </font>2</td>
            <td><% getStaSignalStrength_1(); %></td>
          </tr>
          <tr id="rowSignalStrength3">
            <td class="head"><font id="linkSigStrength3">Signal Strength </font>3</td>
            <td><% getStaSignalStrength_2(); %></td>
          </tr>
          <tr>
            <td class="head" id="linkNoiseLevel">Noise Level</td>
            <td><% getStaNoiseLevel(); %></td>
          </tr>
        </table>
        <br>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="linkHT">HT</td>
          </tr>
          <% getStaHT(); %>
          <tr>
            <td class="head"><font id="linkSNR">SNR</font></td>
            <td><% getStaSNR(); %></td>
          </tr>
        </table>
        <input type="hidden" name="dummyData" value="1">
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
