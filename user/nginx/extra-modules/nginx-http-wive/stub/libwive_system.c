#include "libwive.h"

void reboot_now(void)
{
}

const char* getSystemPlatform()
{
    return "DEMO";
}

long getSystemUptime()
{
    return 0;
}

struct passwd* getShadowPassword(char* username)
{
    struct passwd* res = malloc(sizeof(struct passwd));
    res->pw_name = username;
    res->pw_passwd = username; // fixed password for the demo
    return res;
}

char* cryptShadowPassword(char* username, char* password)
{
    return password; // do not encrypt
}

////////////////////////////////////////////////////////////////////////////////////////////////////
