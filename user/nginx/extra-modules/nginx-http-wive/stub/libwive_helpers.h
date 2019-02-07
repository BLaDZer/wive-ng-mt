#ifndef _WIVE_HELPERS_H
#define _WIVE_HELPERS_H 	1

char *strip_space(char *str);
int strToIntDef(char* value, int def);

int isNumOnly(char *str);
int isAllNumAndSlash(char *str);
int isOnlyOneSlash(char *str);

void STFs(int nvram, int index, char *flash_key, char *value);
int getNums(char *value, char delimit);
int getRuleNums(char *rules);

char *scale(uint64_t size);
const char *normalizeSize(long long *size);

char *racat(char *s, int i);
int checkSemicolon(char *str);

int secondsToTimeIntervalStr(char* timeStr, int strLen, long seconds);

char *getNthValue(int index, char *values);
int getNthValueSafe(int index, char *value, char delimit, char *result, int len);
char *setNthValue(int index, char *old_values, char *new_value);
int deleteNthValueMulti(int index[], int count, char *value, char delimit);

#ifndef LIBBSD_STRING_H
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#endif
