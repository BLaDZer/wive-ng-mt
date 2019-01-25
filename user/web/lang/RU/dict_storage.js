var storage_dictionary = {
	'storage disk title'			: 'Управление дисками',
	'storage disk introduction'		: '',
	'storage disk no devices'		: 'Внешние устройства не обнаружены.',
	'storage disk unmount'			: 'Отключить',
	'storage disk unmount confirm'		: 'Вы действительно желаете отключить "{path}" ?',
	'storage disk unmount error'		: 'Невозможно отключить устройство. Проверьте, не используется ли устройство в данный момент.',
	'storage disk remove'			: 'Удалить',
	'storage disk remove confirm'		: 'Вы действительно желаете удалить "{path}" ?',
	'storage disk info'			: 'Подробнее',
	'storage ftp title'			: 'Настройки FTP',
	'storage ftp introduction'		: '',
	'storage ftp server setup'		: 'Настройка FTP-сервера',
	'storage ftp server access'		: 'Доступ к FTP-серверу',
	'storage ftp server port'		: 'Порт FTP-сервера',
	'storage ftp server idle timeout'	: 'Таймаут простоя FTP-сервера',
	'storage ftp invalid port'		: 'Пожалуйста, укажите корректный порт FTP-сервера'
}

lang_dictionary = Object.assign(lang_dictionary, storage_dictionary);

delete storage_dictionary;