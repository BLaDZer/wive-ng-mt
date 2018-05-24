var login_dictionary = {
	'login title'			: 'Авторизация',
	'login username'		: 'Логин',
	'login password'		: 'Пароль',
	'login enter'			: 'Войти',
	'login incorrect'		: 'Введённые имя пользователя или пароль неверны. Пожалуйста проверьте введённые данные и повторите попытку.'
}

lang_dictionary = Object.assign(lang_dictionary, login_dictionary);

delete login_dictionary;