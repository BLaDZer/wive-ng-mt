/*
 EULA:

 WIVE FIRMWARE IS FREE FOR THE NON-COMMERCIAL USE ONLY.

 Conditions of commercial use non GPL (or other not viral license) components are discussed individually.
 For a more detailed discussion please mail me.
 News and changes in Russian: http://wive-ng.sf.net
 Bug report please send to support[at]nag.ru or sfstudio[at]mail.ru
*/

#include "libwive.h"

#define DIM(x) (sizeof(x)/sizeof(*(x)))

static const char *sizes[]   = { "EiB", "PiB", "TiB", "GiB", "MiB", "KiB", "B" };
static const uint64_t  exbibytes = 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL * 1024ULL;

char *scale(uint64_t size)
{
    char *result = (char *)malloc(sizeof(char) * 20);
    uint64_t  multiplier = exbibytes;
    unsigned int i;

    for (i = 0; i < DIM(sizes); i++, multiplier /= 1024)
    {
        if (size < multiplier)
            continue;
        if (size % multiplier == 0)
            sprintf(result, "%" PRIu64 " %s", size / multiplier, sizes[i]);
        else
            sprintf(result, "%.1f %s", (float) size / multiplier, sizes[i]);
        return result;
    }
    strcpy(result, "0");
    return result;
}

// Sizes table
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

// Normalize size
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

/*
 * concatenate a string with an integer
 * ex: racat("SSID", 1) will return "SSID1"
 */
char *racat(char *s, int i)
{
	static char str[32];
	snprintf(str, 32, "%s%1d", s, i);
	return str;
}

/*
 * check the existence of semicolon in str
 */
int checkSemicolon(char *str)
{
	char *c = strchr(str, ';');
	if (c)
		return 1;
	return 0;
}

/* secondsToTimeIntervalStr - Convert sec counter to human-readable time interval string format
 *
 * arg: (out)timeStr - time interval string
 * arg: strLen - time interval string length
 * arg: seconds - counter
 * return: 0 = OK
 */
int secondsToTimeIntervalStr(char* timeStr, int strLen, long seconds)
{
    snprintf(timeStr, strLen, "%lu day%s, %lu hour%s, %lu min%s, %lu sec%s",
        seconds/(unsigned)86400, ((seconds/(unsigned)86400) == 1)? "" : "s",
        (seconds%(unsigned)86400)/(unsigned)3600, (((seconds%(unsigned)86400)/(unsigned)3600) == 1)? "" : "s",
        (seconds%(unsigned)3600)/(unsigned)60, (((seconds%(unsigned)3600)/(unsigned)60) == 1)? "" : "s",
        seconds%(unsigned)60, ((seconds%(unsigned)60) == 1)? "" : "s");

    timeStr[strLen-1] = '\0';

    return 0;

}

#ifndef LIBBSD_STRING_H
/*
 * Copy src to string dst of size siz.  At most siz-1 characters
 * will be copied.  Always NUL terminates (unless siz == 0).
 * Returns strlen(src); if retval >= siz, truncation occurred.
 */
size_t
strlcpy(char *dst, const char *src, size_t siz)
{
    char *d = dst;
    const char *s = src;
    size_t n = siz;

    /* Copy as many bytes as will fit */
    if (n != 0) {
        while (--n != 0) {
            if ((*d++ = *s++) == '\0')
                break;
        }
    }

    /* Not enough room in dst, add NUL and traverse rest of src */
    if (n == 0) {
        if (siz != 0)
            *d = '\0';          /* NUL-terminate dst */
        while (*s++)
            ;
    }

    return(s - src - 1);        /* count does not include NUL */
}
#endif
