#ifndef __SESSIONID_H__
#define __SESSIONID_H__

#define AUTH_SESSION_MAX_LENGTH 3600

#include "util.h"
#include "asp_mod_utils.h"

ngx_array_t* auth_sessions;
ngx_pool_t* sessionid_pool;

typedef struct auth_session_t {
    char address[65];
    char sessionid[65];
    char username[65];
    enum UserRole role; // default is Deny
    time_t start_time;
} auth_session_t;

int sessionid_init(ngx_pool_t* pool);
auth_session_t* getSessionByAddress(char* address);
auth_session_t* createAuthSession(char* address, char* username);
auth_session_t* getSessionOlderThan(unsigned int seconds);
int closeSessionByAddress(char* address);
auth_session_t* getLastSession();
void saveSessions();
void loadSessions();


#endif//__SESSIONID_H__