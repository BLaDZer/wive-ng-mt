var storage_dictionary = {
	'storage disk title'			: 'Disk Management',
	'storage disk introduction'		: '',
	'storage disk no devices'		: 'No external devices connected.',
	'storage disk unmount'			: 'Unmount',
	'storage disk unmount confirm'		: 'Do you really want to unmount "{path}" ?',
	'storage disk unmount error'		: 'Unable to unmount external drive. Check whether the device is currently in use.',
	'storage disk remove'			: 'Remove',
	'storage disk remove confirm'		: 'Do you really want to remove "{path}" ?',
	'storage disk info'			: 'Details',
	'storage ftp title'			: 'FTP Settings',
	'storage ftp introduction'		: '',
	'storage ftp server setup'		: 'FTP Server Setup',
	'storage ftp server access'		: 'FTP Server Access',
	'storage ftp server port'		: 'FTP Server Port',
	'storage ftp server idle timeout'	: 'FTP Server Idle Timeout',
	'storage ftp invalid port'		: 'Please specify valid FTP port number'
}

lang_dictionary = Object.assign(lang_dictionary, storage_dictionary);

delete storage_dictionary;