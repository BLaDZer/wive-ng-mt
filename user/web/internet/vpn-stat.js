var status = '<% vpnShowVPNStatus(); %>';
var message;
switch(status) {
	case 'disabled':		message = '<b><font color="#808080">' + _('vpn status disabled') + '</font></b>';		break;
	case 'not started':		message = '<b><font color="#808080">' + _('vpn status not started') + '</font></b>';		break;
	case 'offline':			message = '<b><font color="#FF0000">' + _('vpn status offline') + '</font></b>';		break;
	case 'connecting':		message = '<b><font color="#FF8000">' + _('vpn status connecting') + '</font></b>';		break;
	case 'online':			message = '<b><font color="#00FF00">' + _('vpn status online') + '</font></b>';			break;
	case 'kabinet networks':	message = '<b><font color="#33BB33">' + _('vpn status kabinet networks') + '</font></b>';	break;
	case 'full access':		message = '<b><font color="#00FF00">' + _('vpn status full access') + '</font></b>';		break;
}
ajaxModifyElementHTML('vpn_status_col', message);
