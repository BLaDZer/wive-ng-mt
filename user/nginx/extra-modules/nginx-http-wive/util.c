#include "util.h"
#include "sessionid.h"

enum UserRole get_user_role(char* username)
{
    if (username == NULL) return DENY;

    char *adm_login = nvram_get(RT2860_NVRAM, "Login");  // nvram_get usage is not an error, we shouldn't use ACLs during authorization
    char *ord_login = nvram_get(RT2860_NVRAM, "UserLogin");
    char *mng_login = nvram_get(RT2860_NVRAM, "MngmtLogin");

    if (adm_login == NULL) adm_login = "Admin";

    if (strcmp(username,adm_login) == 0) return ADMIN;

//    ELOG_DEBUG(wp->log, 0, "ROLE: ord_login = %s\n, username = %s\n", ord_login, username);
    if (ord_login != NULL && strcmp(username, ord_login) == 0) return USER;
    if (mng_login != NULL && strcmp(username, mng_login) == 0) return ADMIN;

    return DENY;
}


int nvram_acl_check(webs_t* wp, char* key, int write)
{
    enum UserRole role = DENY;

    if (wp->auth_session != NULL)
    {
        role = wp->auth_session->role;
    }

    if (role == DENY)
    {
        goto deny;
    }

    asp_nvram_acl_t* acl_arr = (asp_nvram_acl_t*)asp_nvram_acl_array->elts;
    unsigned int flags = 0;
    unsigned int n;
    for (n=0;n<asp_nvram_acl_array->nelts;n++)
    {
        asp_nvram_acl_t acl = acl_arr[n];
        if (strcmp(acl.name, key)==0)
        {
            flags = acl.flags;
            break;
        }
//        ELOG_DEBUG(r->connection->log, 0, "ACL %s=%i\n", acl.name, acl.flags);
    }

    if (role == USER)
    {
        if (write && (flags&USER_W) )
        {
            return 0;
        }
        else
        if (!write && (flags&USER_R) )
        {
            return 0;
        }
        else
        {
            goto deny;
        }
    }
    else
    if (role == ADMIN)
    {
        if (write && (flags&ADMIN_W) )
        {
            return 0;
        }
        else
        if (!write && (flags&ADMIN_R) )
        {
            return 0;
        }
        else
        {
            goto deny;
        }
    }
    else
    {
        goto deny;
    }

deny:
    return 1;

}

#ifdef NGX_HTTP_SSL
void sha256(char *string, char outputBuffer[65])
{
    int i;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX ctx;

    SHA256_Init(&ctx);
    SHA256_Update(&ctx, string, strlen(string));
    SHA256_Final(hash, &ctx);

    for(i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }

    outputBuffer[64] = 0;
}
#endif

void md5(char *string, char outputBuffer[65])
{
    int i;
    unsigned char hash[16];
    ngx_md5_t ctx;

    ngx_md5_init(&ctx);
    ngx_md5_update(&ctx, string, strlen(string));
    ngx_md5_final(hash, &ctx);


    for(i = 0; i < 16; i++)
    {
        sprintf(outputBuffer + (i * 2), "%02x", hash[i]);
    }

    outputBuffer[32] = 0;
}

/*
ngx_array_t* get_passwd_users(ngx_pool_t* pool)
{
    FILE * fp;
    char * line = NULL;
    char** token;
    size_t len = 0;
    ssize_t read;

    ngx_array_t* out = ngx_array_create(pool, 16, sizeof(char*));
    if (out == NULL) return NULL;

    fp = fopen("/etc/passwd", "r");
    if (fp == NULL) return NULL;

    while ((read = getline(&line, &len, fp)) != -1) 
    {
        char* ptr = strchr(line, ':');
        if (ptr == NULL) continue;
        *ptr = '\0';

        token = ngx_array_push(out);
        if (token == NULL) continue;
        char* line_copy = cstring_copy(pool, line);
        if (line_copy == NULL) continue;

        *token = line_copy;
    }

    fclose(fp);
    free(line);

    return out;
}
*/

/*
    0 = password is OK
    1 = password is incorrect
    < 0 = error
*/
int check_shadow_pass(char* username, char* password)
{
    struct passwd* pwd = getShadowPassword(username);
    if (pwd == NULL) return -1; // user not found, abort

    char* enc_pass = cryptShadowPassword(username, password);
    if (enc_pass == NULL) return -2; // encryption error, abort

    char enc_pass1[65] = {0};
    char enc_pass2[65] = {0};

#ifdef NGX_HTTP_SSL
    sha256(enc_pass, enc_pass1);
    sha256(pwd->pw_passwd, enc_pass2);
#else
    md5(enc_pass, enc_pass1);
    md5(pwd->pw_passwd, enc_pass2);
#endif

    if (strcmp(enc_pass1, enc_pass2) == 0) // plain input
    {
        return 0;
    }

    if (strcmp(password, enc_pass2) == 0) // pre-hashed input
    {
        return 0;
    }

    return 1;
}

int doSystem(char *fmt, ...)
{
    char buf[1024] = {0};
    char buf2[1060] = {0};

    va_list vl;
    va_start(vl, fmt);

    int errcode = vsnprintf( buf, sizeof(buf), fmt, vl);
    va_end( vl);

    if (errcode < 0 || errcode >= 1024)
    {
        return -1;
    }

    int len = strlen(buf);
    if (len == 0) return 0;

    snprintf(buf2, sizeof(buf2)-1, "(%s) > /dev/console 2>&1", buf);

    errcode = system(buf2);
    syslog(LOG_INFO, "doSystem: %s (%i)", buf2, errcode);

    return errcode;
}


ngx_str_t n_escape_html(ngx_pool_t* pool, ngx_str_t str)
{
    size_t len = ngx_escape_html(NULL, str.data, str.len);
    ngx_str_t out;
    out.len = str.len + len;

//    if (len) {
    char* p = (char*)ngx_pcalloc(pool, out.len+1);
    if (p == NULL) {
        out.data = NULL;
        out.len = 0;
        return out;
    }

    ngx_escape_html((unsigned char*)p, str.data, str.len);
    out.data = (unsigned char*)p;
//    }
    return out;
}


ngx_str_t n_unescape_uri(ngx_pool_t* pool, ngx_str_t str, ngx_uint_t type)
{
    ngx_str_t out;

    unsigned char* p = (unsigned char*)ngx_pcalloc(pool, str.len+1);
    if (p == NULL) {
        out.data = NULL;
        out.len = 0;
        return out;
    }

    out.data = p;

    ngx_unescape_uri(&p, &str.data, str.len, type);
    out.len = p - out.data;

    return out;
}

ngx_str_t n_base64_decode(ngx_pool_t* pool, ngx_str_t str)
{
    ngx_str_t plain;

    plain.data = ngx_pnalloc(pool, ngx_base64_decoded_length(str.len));
    if (plain.data == NULL) {
        plain.len = 0;
        return plain;
    }

    if (ngx_decode_base64(&plain, &str) != NGX_OK) {
        plain.data = NULL;
        plain.len = 0;
        return plain;
    }

    return plain;
}

/*
    skip N string characters, shrink length by N
*/
ngx_str_t n_skipchar(ngx_str_t str, int n)
{
    ngx_str_t out;

    if (n > 0 && (unsigned int)n > str.len)
    {
        out.data = NULL;
        out.len = 0;
        return out;
    }

    out.data = str.data + n;
    out.len = str.len - n;
    return out;
}

/*
   -1 -- str1 < str2
    0 -- str1 = str2
    1 -- str1 > str2
*/
int n_strcmp(ngx_str_t str1, ngx_str_t str2)
{
    unsigned int i;

    unsigned int len = (str1.len<str2.len)?str1.len:str2.len;

    for (i = 0; i < len; i++)
    {
        if (str1.data[i] < str2.data[i]) return -1;
        else
        if (str1.data[i] > str2.data[i]) return 1;
    }

    if (str1.len > str2.len) return 1;
    if (str1.len < str2.len) return -1;


    return 0;
}


/*
    strstr or out.data == NULL, out.len = 0
*/
ngx_str_t n_strstr(ngx_str_t str1, ngx_str_t str2)
{
    if (str2.len == 0)
    {
        return str1;
    }

    ngx_str_t out;
    out.data = NULL;
    out.len = 0;

    if (str2.len > str1.len)
    {
        return out;
    }

    unsigned int i;
    for (i = 0; i<(str1.len - str2.len + 1);i++)
    {
        if (str1.data[i] == str2.data[0])
        {
            if (memcmp(str1.data+i, str2.data, str2.len) == 0)
            {
                out.data = str1.data + i;
                out.len = str1.len - i;
                return out;
            }
        }
//              str1.data[i]
    }

    return out;
}

ngx_str_t n_strstr2(ngx_http_request_t *r, ngx_str_t str1, ngx_str_t str2)
{
    if (str2.len == 0)
    {
        return str1;
    }

    ngx_str_t out;
    out.data = NULL;
    out.len = 0;

    unsigned int i;

    if (str2.len > str1.len)
    {
        ELOG_DEBUG(r->connection->log, 0, "n_strstr str2 len > str1 len");
        return out;
    }

    for (i = 0; i<(str1.len - str2.len + 1);i++)
    {
//        ELOG_DEBUG(r->connection->log, 0, "n_strstr D: %d/%d \n", i, (str1.len - str2.len + 1));
        if (str1.data[i] == str2.data[0])
        {
            if (memcmp(str1.data+i, str2.data, str2.len) == 0)
            {
                out.data = str1.data + i;
                out.len = str1.len - i;
                return out;
            }
        }
//              str1.data[i]
    }

    return out;
}

int ngx_nvram_set(webs_t* wp, char *key, char *value)
{
    if (nvram_acl_check(wp, key, 1))
    {
        ELOG_DEBUG(wp->log, 0, "ngx_nvram_set: ACCESS DENIED (key=%s)!\n", key);
        return 0;
    }

    return nvram_set(RT2860_NVRAM, key, value);

}

int ngx_nvram_bufset(webs_t* wp, char *key, char *value)
{
    if (nvram_acl_check(wp, key, 1))
    {
        ELOG_DEBUG(wp->log, 0, "ngx_nvram_bufset: ACCESS DENIED (key=%s)!\n", key);
        return 0;
    }

    return nvram_bufset(RT2860_NVRAM, key, value);
}


char* ngx_nvram_get(webs_t* wp, char* key)
{
    if (nvram_acl_check(wp, key, 0))
    {
        ELOG_DEBUG(wp->log, 0, "ngx_nvram_get: ACCESS DENIED (key=%s)!\n", key);
        return "";
    }

    char* val = nvram_get(RT2860_NVRAM, key);
    ELOG_DEBUG(wp->log, 0, "ngx_nvram_get: |%s| = |%s|\n", key, val);

    return val;
}

int ngx_nvram_get_int(webs_t* wp, char* key, int def)
{
    if (nvram_acl_check(wp, key, 0))
    {
        ELOG_DEBUG(wp->log, 0, "ngx_nvram_get_int: ACCESS DENIED (key=%s)!\n", key);
        return def;
    }

    int val = nvram_get_int(RT2860_NVRAM, key, def);
    ELOG_DEBUG(wp->log, 0, "ngx_nvram_get_int: |%s| = |%i|\n", key, val);

    return val;
}

char *ngx_array_to_cstring(ngx_pool_t* pool, ngx_array_t *arr)
{
    int len = arr->nelts;
    char* str_inner = ngx_pcalloc(pool, len+1);
    memcpy(str_inner, arr->elts, len);
    return str_inner;
}



char* cstring_trim_left(ngx_pool_t* pool, char* str, char symbol)
{
    int len = strlen(str);
    char* str_out = (char*)ngx_pcalloc(pool,len+1);
    char* ptr = str;
    while (*ptr && *ptr == symbol)
    {
        ptr++;
    }

    if (str_out != NULL)
    {
        strcpy(str_out, ptr);
        return str_out;
    }

    return NULL;
}

char* cstring_trim_right(ngx_pool_t* pool, char* str, char symbol)
{
    int len = strlen(str);
    char* ptr = str+len;

    while (ptr >= str)
    {
        if (*ptr != symbol && *ptr != '\0') break;
        ptr--;
    }

    int newlen = ptr-str+1;
    char* str_out = (char*)ngx_pcalloc(pool, newlen+1);

    if (str_out != NULL)
    {
        memcpy(str_out, str, newlen);
        return str_out;
    }

    return NULL;
}

char* cstring_trim(ngx_pool_t* pool, char* str, char symbol)
{
    char* tstr1 = cstring_trim_left(pool, str, symbol);
    if (tstr1 == NULL) return NULL;

    char* tstr2 = cstring_trim_right(pool, tstr1, symbol);
    if (tstr2 == NULL) return NULL;

    return tstr2;
}

char* cstring_copy(ngx_pool_t* pool, char* str)
{
    int len = strlen(str);
    char* outstr = ngx_pcalloc(pool, sizeof(char)*(len+1));
    if (outstr == NULL) return NULL;

    memcpy(outstr, str, len);
    return outstr;
}

ngx_str_t cstring_to_ngx(ngx_pool_t* pool, char* str)
{
    int len = strlen(str);
    ngx_str_t* outstr = ngx_pcalloc(pool, sizeof(ngx_str_t));
    outstr->len = len;
    outstr->data = ngx_pcalloc(pool, (len+1)*sizeof(char));
    memcpy(outstr->data, str, len+1);
    return *outstr;
}

char* ngx_to_cstring(ngx_pool_t* pool, ngx_str_t str)
{
    if (str.data == NULL)
    {
        return NULL;
    }

    char* outstr = ngx_pcalloc(pool, (str.len+1)*sizeof(char));
    memcpy(outstr, str.data, str.len);
    return outstr;
}

ngx_array_t* cstring_split(ngx_pool_t* pool, char* str, char* sep)
{
    int sep_len = strlen(sep);
    char* ptr = str;
    char* sep_ptr = strstr(ptr, sep);
    char** token;

    ngx_array_t* out = ngx_array_create(pool, 16, sizeof(char*));

    while (sep_ptr != NULL)
    {
        int token_len = sep_ptr-ptr;
        token = ngx_array_push(out);

        if (token != NULL)
        {
            *token = ngx_pcalloc(pool, token_len+1);

            if (*token != NULL)
            {
                memcpy(*token, ptr, token_len);
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }

        ptr = sep_ptr + sep_len;
        sep_ptr = strstr(ptr, sep);
    }

    token = ngx_array_push(out);
    *token = ngx_pcalloc(pool, strlen(ptr)+1);
    if (* token != NULL)
    {
        memcpy(*token, ptr, strlen(ptr));
    }
    else
    {
        return NULL;
    }

    return out;
}

ngx_array_t* ngx_string_split(ngx_pool_t* pool, ngx_str_t str, ngx_str_t sep)
{
    ngx_str_t ptr = str;

    ngx_str_t sep_ptr = n_strstr(ptr, sep);
    ngx_str_t* token;

    ngx_array_t* out = ngx_array_create(pool, 16, sizeof(ngx_str_t));

    while (sep_ptr.data != NULL)
    {
        int token_len = sep_ptr.data-ptr.data;
        token = ngx_array_push(out);

        if (token != NULL)
        {
            token->data = ngx_pcalloc(pool, token_len+1);
            token->len = token_len;

            if (token->data != NULL)
            {
                memcpy(token->data, ptr.data, token_len);
            }
            else
            {
                return NULL;
            }
        }
        else
        {
            return NULL;
        }

        ptr = n_skipchar(sep_ptr, sep.len);
        sep_ptr = n_strstr(ptr, sep);
    }

    token = ngx_array_push(out);
    token->data = ngx_pcalloc(pool, ptr.len+1);
    token->len = ptr.len;
    if (token->data != NULL)
    {
        memcpy(token->data, ptr.data, ptr.len);
    }
    else
    {
        return NULL;
    }

    return out;
}

ngx_str_t ngx_split_inplace2(ngx_http_request_t *r, ngx_str_t str, ngx_str_t sep)
{

    ELOG_DEBUG(r->connection->log, 0, "ngx_split_inplace2 A");
    ngx_str_t sep_ptr = n_strstr2(r, str, sep);
    ELOG_DEBUG(r->connection->log, 0, "ngx_split_inplace2 B");

    if (sep_ptr.data == NULL)
    {
        ELOG_DEBUG(r->connection->log, 0, "ngx_split_inplace2 C");
        return sep_ptr;
    }

    ELOG_DEBUG(r->connection->log, 0, "ngx_split_inplace2 D");
    sep_ptr.data[0] = '\0';

    ELOG_DEBUG(r->connection->log, 0, "ngx_split_inplace2 E");

    return n_skipchar(sep_ptr, sep.len);
}

ngx_str_t ngx_split_inplace(ngx_str_t str, ngx_str_t sep)
{
/*
USAGE:
    ngx_str_t token = LINE;
    ngx_str_t next_token = ngx_null_string();

    while (token.data != NULL)
    {
        next_token = ngx_split_inplace(token, SEP);

        printf("|%s|\n", token);

        token = next_token;
    }
*/

    ngx_str_t sep_ptr = n_strstr(str, sep);

    if (sep_ptr.data == NULL)
    {
        return sep_ptr;
    }

    sep_ptr.data[0] = '\0';

    return n_skipchar(sep_ptr, sep.len);
}



char* cstring_split_inplace(char* str, char* sep)
{
/*
USAGE:
    char* token = line;
    char* next_token = NULL;

    while (token != NULL)
    {
        next_token = cstring_split_inplace(token, SEP);

        printf("|%s|\n", token);

        token = next_token;
    }
*/

    int sep_len = strlen(sep);
    char* sep_ptr = strstr(str, sep);

    if (sep_ptr == NULL)
    {
        return NULL;
    }

    *sep_ptr = '\0';

    return sep_ptr + sep_len;
}

int arrsWrite(ngx_array_t *arr, char* fmt, ...)
{
    va_list vl;

    va_start(vl, fmt);
    int sz = vsnprintf( NULL, 0, fmt, vl);
    va_end( vl);

    if (sz < 0) return NGX_ERROR;
    if (sz == 0) return NGX_OK;

    char* ptr = (char*)ngx_array_push_n(arr, sz);

    va_start(vl, fmt);
    int written = vsnprintf( ptr, sz+1, fmt, vl);
    va_end( vl);

    if (written < 0 || written > sz)
    {
        return NGX_ERROR;
    }

    return NGX_OK;
}

char* getCookie(ngx_http_request_t *r, char* name)
{
    ngx_str_t  cookie, s;

    s = cstring_to_ngx(r->pool, name);

    if (ngx_http_parse_multi_header_lines(&(r->headers_in.cookies), &s, &cookie) == NGX_DECLINED)
    {
        return NULL;
    }

    if (cookie.data == NULL)
    {
        return NULL;
    }

    return ngx_to_cstring(r->pool, cookie);
}

int import_rwfs(char *filename, int offset, int len)
{
	char cmd[4096];
	char data;
	FILE *fp, *src;
	char *pname = "/tmp/rwfstmp.tar.bz2";

	snprintf(cmd, 4096, "cp %s /var/tmpcgi", filename);
	system(cmd);

	if (!( fp = fopen(pname, "w+")))
		return 1;

	if(!( src = fopen(filename, "r")))
	{
		fclose(fp);
		return 2;
	}

	if (fseek(src, offset, SEEK_SET) == -1)
		printf("goahead: fseek error,%s\n", __FUNCTION__);

	while (len > 0)
	{
		if (!fread(&data, 1, 1, src))
		{
			fclose(src);
			fclose(fp);
			return 3;
		}

		fwrite(&data, 1, 1, fp);
		len--;
	}

	fclose(src);
	fclose(fp);

	snprintf(cmd, 4096, "/etc/scripts/unpackrwfs.sh %s", pname);
	system(cmd);

        return 0;
}

int import_settings(char *filename, int offset, int len)
{
	char cmd[4096];
	char data;
	char *pname;
	FILE *fp, *src;

	snprintf(cmd, 4096, "cp %s /var/tmpcgi", filename);
	system(cmd);

	pname = tempnam("/var", "set");
	if (!( fp = fopen(pname, "w+")))
		return 1;

	if(!(src = fopen(filename, "r")))
	{
		fclose(fp);
		return 2;
	}

	if (fseek(src, offset, SEEK_SET) == -1)
		printf("goahead: fseek error, %s\n", __FUNCTION__);

	while (len > 0)
	{
		if (!fread(&data, 1, 1, src))
		{
			fclose(src);
			fclose(fp);
			return 3;
		}

		fwrite(&data, 1, 1, fp);
		len--;
	}

	fclose(src);
	fclose(fp);

	system("nvram_clear 2860");
	snprintf(cmd, 4096, "nvram_renew 2860 %s", pname);
	system(cmd);

        return 0;
}
