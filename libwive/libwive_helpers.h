#ifndef _WIVE_HELPERS_H
#define _WIVE_HELPERS_H 	1

char *scale(uint64_t size);
const char *normalizeSize(long long *size);

char *racat(char *s, int i);
int checkSemicolon(char *str);

int secondsToTimeIntervalStr(char* timeStr, int strLen, long seconds);

#ifndef LIBBSD_STRING_H
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#endif
