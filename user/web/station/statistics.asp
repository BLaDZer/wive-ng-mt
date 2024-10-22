<!DOCTYPE html>
<html>
<head>
<title>Wireless Station Statistics</title>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8">
<meta http-equiv="Cache-Control" content="no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0">
<meta http-equiv="Pragma" content="no-cache">
<meta http-equiv="Expires" content="-1">
<script type="text/javascript" src="/js/ajax.js"></script>
<link rel="stylesheet" href="/style/windows.css" type="text/css">
<link rel="stylesheet" href="/style/normal_ws.css" type="text/css">
<link rel="stylesheet" href="/style/controls.css" type="text/css">
<script language="JavaScript" type="text/javascript">
</script>
</head>

<body bgcolor="#FFFFFF">
<table class="body">
  <tr>
    <td><h1 id="statisticTitle">Station Statistics</h1>
      <p id="statisticIntroduction">The Status page shows the settings and current operation status of the Station.</p>
      <hr />
      <form method="post" name="sta_statistics" action="/goform/resetStaCounters" OnSubmit="ajaxShowTimer(this, 'timerReloader', _('message apply'), 15);">
        <iframe name="timerReloader" id="timerReloader" src="" style="width:0;height:0;border:0px solid #fff;"></iframe>
        <table class="form">
          <tr>
            <td class="title" colspan="2" id="statisticTx">Transmit Statistics</td>
          </tr>
          <% getStaStatsTx(); %>
          <tr>
            <td class="head">Frames Received With CRC Error </td>
            <td><% getStaStatsRxCRCErr(); %></td>
          </tr>
          <tr>
            <td class="head">Frames Dropped Due To Out-of-Resource</td>
            <td><% getStaStatsRxNoBuf(); %></td>
          </tr>
          <tr>
            <td class="head">Duplicate Frames Received </td>
            <td><% getStaStatsRxDup(); %></td>
          </tr>
        </table>
        <input type=hidden name=dummyData value="1">
        <br />
        <table class="buttons">
          <tr>
            <td><input type="submit" class="normal" value="Reset Counters" id="statisticResetCounter"></td>
          </tr>
        </table>
      </form>
      <div class="whitespace">&nbsp;</div></td>
  </tr>
</table>
</body>
</html>
