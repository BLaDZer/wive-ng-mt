var overview_dictionary = {
	// Overview page
	'overview select language'	: 'Выбор языка',
	'overview status'		: 'Статус',
	'overview statistics'		: 'Статистика',
	'overview management'		: 'Управление',

	// Opmode page
	'opmode title'			: 'Настройка режима работы',
	'opmode firmware version'	: 'Текущая версия прошивки: ',
	'opmode introduction'		: 'Здесь вы можете выбрать режим работы, подходящий для ваших задач.',
	'opmode mode bridge'		: 'Мост',
	'opmode mode bridge intro'	: 'Все проводные и беспроводные интерфейсы объединены в один мост.',
	'opmode mode gateway'		: 'Шлюз',
	'opmode mode gateway intro'	: 'Один проводной интерфейс используется для доступа в интернет(WAN), остальные проводные и беспроводные интерфейсы используются как локальные(LAN).',
	'opmode mode e'			: 'Ethernet конвертер',
	'opmode mode e intro'		: 'Беспроводной интерфейс рассматривается как WAN-порт, а проводные интерфейсы используются в качестве портов LAN.',
	'opmode mode a'			: 'Клиент + AП + Шлюз(WISP) / Клиент + AП + Мост(Repeater)',
	'opmode mode a intro'		: 'Wi-Fi apcli интерфейс назначается как WAN, ethernet порты и Wi-Fi AP порт - как LAN. Либо все порты объединяются в единый мост.',
	'opmode mode unknown'		: 'Неизвестно',
	'opmode confirm'		: 'Для изменение режима работы необходимо выполнить перезагрузку маршрутизатора. Выполнить перезагрузку?'
}

lang_dictionary = Object.assign(lang_dictionary, overview_dictionary);

delete overview_dictionary;
