#include "sessionid.h"


int sessionid_init(ngx_pool_t* pool)
{
    if (auth_sessions == NULL)
    {
        sessionid_pool = pool;
        auth_sessions = ngx_array_create(pool, 16, sizeof(auth_session_t));
//        loadSessions();
        return 0;
    }

    return 1;
}

auth_session_t* getSessionByAddress(char* address)
{
    unsigned int i;
    struct auth_session_t* sessions = (auth_session_t*) auth_sessions->elts;

    for (i=0;i<auth_sessions->nelts;i++)
    {

        if (strcmp(sessions[i].address, address) == 0)
        {
            return &(sessions[i]);
        }
    }

    return NULL;
}

auth_session_t* getSessionById(char* sessionid)
{
    unsigned int i;
    struct auth_session_t* sessions = (auth_session_t*) auth_sessions->elts;

    for (i=0;i<auth_sessions->nelts;i++)
    {

        if (strcmp(sessions[i].sessionid, sessionid) == 0)
        {
            return &(sessions[i]);
        }
    }

    return NULL;
}


int closeSessionsByUser(char* username)
{
    unsigned int i;
    struct auth_session_t* sessions = (auth_session_t*) auth_sessions->elts;
    int c;

    for (i=0;i<auth_sessions->nelts;i++)
    {
        if (strcmp(sessions[i].username, username) == 0)
        {
            sessions[i].start_time = 0;
            c++;
        }
    }

    return c;
}


auth_session_t* getSessionOlderThan(unsigned int seconds)
{
    time_t current_time = time(NULL);
    unsigned int i;
    struct auth_session_t* sessions = (auth_session_t*) auth_sessions->elts;

    for (i=0;i<auth_sessions->nelts;i++)
    {

        if ((current_time - sessions[i].start_time) > seconds)
        {
            return &(sessions[i]);
        }
    }

    return NULL;
}


int closeSessionByAddress(char* address)
{
    auth_session_t* session = getSessionByAddress(address);
    if (address == NULL)
    {
        return 1;
    }

    session->start_time = 0;

    return 0;
}


auth_session_t* createAuthSession(char* address, char* username)
{
    int i;
    enum UserRole role = get_user_role(username);
    struct auth_session_t* auth_session = NULL;

    // commented out to not replace alive sessions
    //auth_session = getSessionByAddress(address);

    if (auth_session == NULL)
    {
        auth_session = getSessionOlderThan(AUTH_SESSION_MAX_LENGTH); // 1 hour
    }

    if (auth_session == NULL)
    {
        auth_session = ngx_array_push(auth_sessions);

        if (auth_session == NULL)
        {
            return NULL;
        }
    }

    unsigned char bts[65] = {0};
#ifdef NGX_HTTP_SSL
    RAND_bytes(bts, 64);
#else
    for (i=0;i<65;i++)
    {
        bts[i] = rand() % 256;
    }
#endif

    char alpha[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

    for (i=0;i<64;i++)
    {
        auth_session->sessionid[i] = alpha[ bts[i] % (sizeof(alpha)-1) ];
    }
    auth_session->sessionid[64] = '\0';

    strncpy(auth_session->address, address, sizeof(auth_session->address));
    strncpy(auth_session->username, username, sizeof(auth_session->username));

    auth_session->role = role;
    auth_session->start_time = time(NULL);

//    saveSessions();

    return auth_session;
}

/* save sessions to rwfs in case we need to */
void saveSessions()
{
    FILE* f = fopen("/etc/nginx/sessions.conf", "wt");
    if (f == NULL) return;

    time_t current_time = time(NULL);
    unsigned int i;
    struct auth_session_t* sessions = (auth_session_t*) auth_sessions->elts;

    for (i=0;i<auth_sessions->nelts && i < 100;i++)
    {

        if (current_time >= sessions[i].start_time && (current_time - sessions[i].start_time) < AUTH_SESSION_MAX_LENGTH)
        {
            auth_session_t session = sessions[i];
            fprintf(f, "%s\t%s\t%s\t%lu\n", session.address, session.username, session.sessionid, session.start_time);
        }
    }

    fclose(f);
    
}

/* load sessions from rwfs in case we need to */
void loadSessions()
{
//    syslog(LOG_WARNING, "loadSessions(): start");

    FILE* f = fopen("/etc/nginx/sessions.conf", "rt");
    if (f == NULL) return;

    time_t current_time = time(NULL);
    unsigned int i;
    struct auth_session_t* sessions = (auth_session_t*) auth_sessions->elts;

    auth_session_t session;

    while (!feof(f))
    {
        if (fscanf(f,"%s\t%s\t%s\t%lu", session.address, session.username, session.sessionid, &session.start_time) != 4)
        {
            break;
        }


        if (current_time >= session.start_time && (current_time - session.start_time) < AUTH_SESSION_MAX_LENGTH)
        {
            auth_session_t* new_sess = createAuthSession(session.address, session.username);
            if (new_sess != NULL)
            {
                enum UserRole role = get_user_role(session.username);
                new_sess->role = role;
                strncpy(new_sess->sessionid, session.sessionid, sizeof(new_sess->sessionid));
                new_sess->start_time = session.start_time;

            }
        }
    }

    fclose(f);
}

