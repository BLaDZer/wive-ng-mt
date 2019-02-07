#include "libwive.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

char *strip_space(char *str)
{
	unsigned int i;
	for (i=0;i<strlen(str);i++) {
		if (!isspace(str[i])) return str+i;
	}
	return str+strlen(str);
}

int strToIntDef(char* value, int def)
{
    char* endptr = value;

    if (value == NULL) 
        return def;

    long val = strtol(value, &endptr, 10);

    if (endptr[0] != '\0')
        return def;

    if (val > INT_MAX || val < INT_MIN)
        return def;

    return (int)val;
}

void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *result;
	char *tmp = nvram_get(nvram, flash_key);
	if(!tmp)
		tmp = "";
	result = setNthValue(index, tmp, value);
	nvram_bufset(nvram, flash_key, result);
	return ;
}

int getRuleNums(char *rules)
{
	return getNums(rules, ';');
}

int getNums(char *value, char delimit)
{
    char *pos = value;
    int count = 1;
    if (!pos || !(*pos))
        return 0;

    while ((pos = strchr(pos, delimit)) != NULL) {
        ++pos;
        ++count;
    }
    return count;
}

const char *normalSizes[] =
{
	"B",
	"kB",
	"MB",
	"GB",
	"TB",
	"PB",
	"HB",
	"ZB"
};

const char *normalizeSize(long long *size)
{
	const char **result = normalSizes;
	while ((*size)>(128*1024))
	{
		*size /= 1024;
		result++;
	}
	return *result;
}

char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

int getNthValueSafe(int index, char *value, char delimit, char *result, int len)
{
    int i = 0, result_len = 0;
    char *begin, *end;

    if(!value || !result || !len)
        return -1;

    begin = value;
    end = strchr(begin, delimit);
    while(i < index && end != NULL){
        begin = end + 1;
        end = strchr(begin, delimit);
        i++;
    }

    /* no delimit */
    if(!end){
	if(i == index){
	    end = begin + strlen(begin);
	    result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);
	} else
		return -1;
    } else
	result_len = (len-1) < (end-begin) ? (len-1) : (end-begin);

    memcpy(result, begin, result_len);
    *(result + result_len ) = '\0';

	return 0;
}

char *setNthValue(int index, char *old_values, char *new_value)
{
	int i;
	int retlen = 0;
	char *p, *q;
	static char ret[2048];
	char buf[8][256];

	memset(ret, 0, 2048);
	for (i = 0; i < 8; i++)
	    memset(buf[i], 0, 256);

	/* copy original values */
	for ( i = 0, p = old_values, q = strchr(p, ';')  ;
		i < 8 && q != NULL                         ;
		i++, p = q + 1, q = strchr(p, ';')         )
		strncpy(buf[i], p, q - p);

	if (i > 7) /* limit of buf size = 8 */
		i=7;

	strcpy(buf[i], p); /* the last one */

	/* replace buf[index] with new_value */
	strncpy(buf[index], new_value, 256);

	/* calculate maximum index */
	index = (i > index)? i : index;

	/* strip last unneded divider */
	for (i = index; i >= 0; i--) {
		if (strlen(buf[i]) == 0)
			index--;
		else
		    break;
	}

	/* concatenate into a single string delimited by semicolons */
	strcat(ret, buf[0]);
	retlen += strlen(buf[0]);
	for (i = 1; i <= index; i++) {

		if (retlen+strlen(buf[i])+1 < sizeof(ret)) {
			strcat(ret, ";");
			strcat(ret, buf[i]);
			retlen += strlen(buf[i]) + 1;
		}

//		snprintf(ret, sizeof(ret), "%s%s", ret,  buf[i]);
	}

	p = ret;
	return p;
}

int checkSemicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

int isNumOnly(char *str)
{
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if((str[i] >= '0' && str[i] <= '9'))
			continue;
		return 0;
	}
	return 1;
}

int isAllNumAndSlash(char *str)
{
	int i, len = strlen(str);
	for(i=0; i<len; i++){
		if( (str[i] >= '0' && str[i] <= '9') || str[i] == '.' || str[i] == '/' )
			continue;
		return 0;
	}
	return 1;
}

int isOnlyOneSlash(char *str)
{
	int i, count=0;
	int len = strlen(str);
	for(i=0; i<len; i++)
		if( str[i] == '/')
			count++;
	return count <= 1 ? 1 : 0;
}
