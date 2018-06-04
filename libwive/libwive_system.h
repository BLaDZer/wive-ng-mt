#ifndef _WIVE_SYSTEM_H
#define _WIVE_SYSTEM_H	1


void reboot_now(void);
const char* getSystemPlatform();
long getSystemUptime();
struct passwd* getShadowPassword(char* username);
char* cryptShadowPassword(char* username, char* password);

#endif
