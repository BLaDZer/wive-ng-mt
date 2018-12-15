var lang_dictionary = {
	// Warinig table
	'warning header'		: 'Внимание!',
	'warning access password'	: 'Рекомендуется изменить пароль доступа к маршрутизатору.',
	'warning wireless security'	: 'Рекомендуется выставить шифрование и/или изменить пароль беспроводной сети.',
	'warning cwmpd'			: 'Активирован режим удалённого управления маршрутизатором специалистами провайдера.',
	'warning cwmpd auto available'	: 'Ваш провайдер предоставляет возможность автоматической конфигурации подключения к сети интернет. Включить удалённое управление маршрутизатором?',
	'warning cwmpd auto alert'	: 'Вы уверены, что желаете включить удалённое управление маршрутизатором? Перед использованием данной функции уточните у вашего провайдера, поддерживается ли конфигурация данной модели маршрутизатора.',
	'button warning'		: 'Перейти',

	// Messages
	'message apply'			: 'Пожалуйста, подождите пока будут применены новые настройки...',
	'message reboot'		: 'Пожалуйста, подождите пока маршрутизатор перезагрузится...',
	'message chmode'		: 'Пожалуйста, подождите пока применяются изменения режима работы...',
	'message config'		: 'Пожалуйста, подождите пока применяются новые параметры...',
	'message upgrade'		: 'Обновление прошивки... Пожалуйста, не отключайте и не перезагружайте маршрутизатор.',
	'message rwfs'			: 'Пожалуйста, подождите пока обновляется файловая система...',
	'message reset confirm'		: 'Настройки будут сброшены к заводским значениям. Продолжить?',

	// Buttons
	'button apply'			: 'Применить',
	'button cancel'			: 'Отменить',
	'button reset'			: 'Сбросить',
	'button enable'			: 'Включить',
	'button disable'		: 'Отключить',
	'button add'			: 'Добавить',
	'button add rule'		: 'Добавить правило',
	'button add edit'		: 'Добавить / Изменить',
	'button remove'			: 'Удалить',
	'button clear'			: 'Очистить',
	'button restore factory'	: 'Восстановить заводской',
	'button apply connect'		: 'Применить и подключить',
	'button update'			: 'Обновить',
	'button load'			: 'Загрузить',
	'button backup'			: 'Сохранить',
	'button refresh'		: 'Обновить',
	'button reset statistics'	: 'Сбросить статистику',
	'button disconnect'		: 'Отключить',
	'button disconnect all'		: 'Отключить всех',

	// Services status
	'services status about'		: 'О сервисе...',
	'services status off'		: 'отключено',
	'services status work'		: 'работает',
	'services status starting'	: 'запускается',
	'services status details'	: 'Информация',
	'services status configure'	: 'Настройка'
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