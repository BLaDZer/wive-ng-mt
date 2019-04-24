var lang_dictionary = {
	'warning header'		: 'Warning!',
	'warning access password'	: 'Recommended change your password to access the router.',
	'warning wireless security'	: 'Recommended set the encryption and/or change your password the wireless network.',
	'warning cwmpd'			: 'The remote control of the router by ISP experts is active.',
	'warning cwmpd auto available'	: 'Your ISP can automatically configure your internet connection. Enable remote control of the router?',
	'warning cwmpd auto alert'	: 'Are you sure you want to enable remote router control? Before using this feature, check with your provider if the configuration of this router model is supported.',
	'warning update available'	: 'A newer version of the device firmware is available. Go to the management page to install the new firmware.',
	'button warning'		: 'Go to settings',

	'message apply'			: 'Please wait while settings are applied...',
	'message reboot'		: 'Please wait while the router is rebooting...',
	'message chmode'		: 'Please wait while the new operating mode is applied...',
	'message config'		: 'Please wait while parameters are applied...',
	'message upgrade'		: 'Upgrading firmware... Please do not turn off power or reboot your router.',
	'message rwfs'			: 'Please wait while new RWFS are applied...',
	'message reset confirm'		: 'Settings will be reset to the factory defaults. Continue?',
	'message reboot confirm'	: 'Applying some settings will require rebooting the router. Reboot now?',

	'button enable'			: 'Enable',
	'button disable'		: 'Disable',
	'button apply'			: 'Apply',
	'button cancel'			: 'Cancel',
	'button add'			: 'Add',
	'button add rule'		: 'Add rule',
	'button openall'		: 'Open All',
	'button closeall'		: 'Close All',
	'button reset'			: 'Reset',
	'button restore factory'	: 'Restore Factory',
	'button apply connect'		: 'Apply and connect',
	'button update'			: 'Update',
	'button load'			: 'Load',
	'button backup'			: 'Backup',
	'button refresh'		: 'Refresh',
	'button reset statistics'	: 'Reset Statistics',
	'button disconnect'		: 'Disconnect',
	'button disconnect all'		: 'Disconnect All',
	'button clear'			: 'Clear',
	'button remove'			: 'Remove',
	'button add edit'		: 'Add / Edit',

	'services status off'		: 'off',
	'services status work'		: 'work',
	'services status starting'	: 'starting',
	'services status details'	: 'Details',
	'services status about'		: 'Learn more...',
	'services status configure'	: 'Configure'
}

if (typeof Object.assign != 'function') {
	Object.assign = function(target) {
		'use strict';
		if (target == null) {
			throw new TypeError('Cannot convert undefined or null to object');
		}

		target = Object(target);
		for (var index = 1; index < arguments.length; index++) {
			var source = arguments[index];
			if (source != null) {
				for (var key in source) {
					if (Object.prototype.hasOwnProperty.call(source, key)) {
						target[key] = source[key];
					}
				}
			}
		}
		return target;
	};
}