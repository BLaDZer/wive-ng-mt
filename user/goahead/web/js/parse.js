function parseAtoi(str, num)
{
	i=1;
	if (num != 1 )
	{
		while ((i != num) && (str.length != 0))
		{
			if (str.charAt(0) == '.')
				i++;
			str = str.substring(1);
		}
		if(i != num )
			return -1;
	}

	for (i=0; i<str.length; i++)
	{
		if(str.charAt(i) == '.')
		{
			str = str.substring(0, i);
			break;
		}
	}

	if (str.length == 0)
		return -1;
	return parseInt(str, 10);
}
