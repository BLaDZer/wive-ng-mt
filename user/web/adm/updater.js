var updater_state = '<% getUpdaterState(); %>';
if (updater_state == "" || updater_state == "error") updater_state = "uptodate";

var updater_version = '<% getUpdaterImageName(); %>';

displayElement('fwAutoUptodateDiv', updater_state == "uptodate" );
displayElement('fwAutoAvailableDiv', updater_state == "available" );
displayElement('fwAutoDownloadingDiv', updater_state == "downloading" );
displayElement('fwAutoDownloadedDiv', updater_state == "downloaded" );
displayElement('fwAutoFlashingDiv', updater_state == "flashing" );

var form = document.forms['UploadFirmwareAuto'];
displayElement('manAdmFirmwareAuto2', updater_state == "available" || updater_state == "downloaded" || updater_state == "uptodate" || updater_state == "error");

if (updater_state == "flashing")
{
	var rootWindow = ajaxGetRootWindow();
	if (typeof rootWindow.currentProgressHandler !== 'undefined') {
		var handler = rootWindow.currentProgressHandler;
		if (handler.name == 'indicator') {
			ajaxReloadDelayedPage('<% getFirmwareUpdateTime(1); %>'*1000);
		}
	}
	clearTimeout(updaterTimerId);
} else
if (updater_state == "available")
{
	document.getElementById('fwAutoVersion').innerHTML = updater_version;
	form.cmd.value = "download";
	form.updateFWAuto.value = _("management autoupdate button download");
} else 
if (updater_state == "downloaded")
{
	document.getElementById('fwAutoVersion2').innerHTML = updater_version;
	form.cmd.value = "flash";
	form.updateFWAuto.value = _("management autoupdate button update");
} else
{
	form.cmd.value = "check";
	form.updateFWAuto.value = _("management autoupdate button check");
}

