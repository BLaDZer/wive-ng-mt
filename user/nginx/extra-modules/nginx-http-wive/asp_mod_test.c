#include "asp_mod_test.h"

static ngx_str_t NG(char* line)
{
    ngx_str_t str;
    str.data = (unsigned char*)strdup(line);
    str.len = strlen(line);
    return str;
}

static void assert(webs_t* wp, int val, char* test_name)
{
    ELOG_DEBUG(wp->log, 0, "assert START %s\n", test_name);

    if (val == 0)
    {
        outWrite("<abbr title='%s'><span style='color: #910;'>FAIL</span></abbr> ", test_name);
    }
    else
    {
        outWrite("<abbr title='%s'><span style='color: #091;'>OK</span></abbr> ", test_name);
    }
}

static int test_n_skipchar(webs_t *wp, char** params, int nparams)
{
    outWrite("n_skipchar: <br>");
    ngx_str_t str = NG("ABCD");

    str = n_skipchar(str, 0);
    assert( wp, (memcmp(str.data, "ABCD", 5) == 0)&&(str.len == 4), "test 0");

    str = n_skipchar(str, 1);
    assert( wp, (memcmp(str.data, "BCD", 4) == 0)&&(str.len == 3), "test 1");

    str = n_skipchar(str, 2);
    assert( wp, (memcmp(str.data, "D", 2) == 0)&&(str.len == 1), "test 2");

    str = n_skipchar(str, 1);
    assert( wp, (memcmp(str.data, "", 1) == 0)&&(str.len == 0), "test 3");

    str = n_skipchar(str, 1);
    assert( wp,  (str.data == NULL)&&(str.len == 0) , "test 4" );

    str = n_skipchar(str, 4);
    assert( wp,  (str.data == NULL)&&(str.len == 0) , "test 5" );

    outWrite("DONE<br><br>");

    return 0;
}


static int test_n_strcmp(webs_t *wp, char** params, int nparams)
{
    outWrite("n_strcmp: <br>");
    ngx_str_t ptr;
    ngx_str_t str1, str2;
    int res;

    res = n_strcmp(NG("ABCD"), NG("ABC"));
    assert ( wp, res > 0, "test 1" );

    res = n_strcmp(NG("ABC"),NG("ABC"));
    assert ( wp, res == 0, "test 2" );

    res = n_strcmp(NG("ABC"),NG("ABCD"));
    assert ( wp, res < 0, "test 3" );

    res = n_strcmp(NG("ABCA"),NG("ABCD"));
    assert ( wp, res < 0, "test 4" );

    res = n_strcmp(NG("ABCZ"),NG("ABCD"));
    assert ( wp, res > 0, "test 5" );

    res = n_strcmp(NG("ZBC"), NG("ABCD"));
    assert ( wp, res > 0, "test 6" );

    res = n_strcmp(NG("ZCBE"), NG("AHRR"));
    assert ( wp, res > 0, "test 7" );

    res = n_strcmp(NG(""),NG(""));
    assert ( wp, res == 0, "test 8" );

    res = n_strcmp(NG("A"),NG("B"));
    assert ( wp, res < 0, "test 9" );

    res = n_strcmp(NG("B"),NG("A"));
    assert ( wp, res > 0, "test 10" );

    outWrite("DONE<br><br>");

    return 0;
}

static int test_ngx_split_inplace(webs_t *wp, char** params, int nparams)
{
    outWrite("ngx_split_inplace: <br>");
    
    ngx_str_t token1 = ngx_split_inplace(NG("abc def g  "), NG(" "));
    assert( wp, token1.data != NULL && (memcmp(token1.data, "def", 3) == 0), "test 1");  //def g  

    ngx_str_t token2 = ngx_split_inplace(token1, NG(" "));
    assert( wp, token2.data != NULL && (memcmp(token2.data, "g", 1) == 0), "test 2"); //g  

    ngx_str_t token3 = ngx_split_inplace(token2, NG(" "));
    assert( wp, token3.data != NULL && (memcmp(token3.data, " ", 2) == 0), "test 3"); //  

    ngx_str_t token4 = ngx_split_inplace(token3, NG(" "));
    assert( wp, token3.data != NULL && token4.data[0] == '\0', "test 4"); //

    ngx_str_t token5 = ngx_split_inplace(token4, NG(" "));
    assert( wp, token5.data == NULL, "test 5");

    outWrite("DONE<br><br>");

    return 0;
}

static int test_cstring_split_inplace(webs_t *wp, char** params, int nparams)
{
    outWrite("cstring_split_inplace: <br>");
    char* str_in = strdup("abc def g  ");

    char* token1 = cstring_split_inplace(str_in, " ");
    assert( wp, token1 != NULL && (memcmp(token1, "def", 3) == 0), "test 1");  //def g  

    char* token2 = cstring_split_inplace(token1, " ");
    assert( wp, token2 != NULL && (memcmp(token2, "g", 1) == 0), "test 2"); //g  

    char* token3 = cstring_split_inplace(token2, " ");
    assert( wp, token3 != NULL && (memcmp(token3, " ", 2) == 0), "test 3"); //  

    char* token4 = cstring_split_inplace(token3, " ");
    assert( wp, token3 != NULL && token4[0] == '\0', "test 4"); //

    char* token5 = cstring_split_inplace(token4, " ");
    assert( wp, token5 == NULL, "test 5");

    outWrite("DONE<br><br>");

    return 0;
}

static int test_n_strstr(webs_t *wp, char** params, int nparams)
{
    outWrite("n_strstr: <br>");
    ngx_str_t str1 = NG("abc;defg;hi;j;;");
    ngx_str_t sep = NG(";");

    ngx_str_t str2 = n_strstr(str1, sep);
    assert( wp, str2.data != NULL && (memcmp(str2.data, ";defg;hi;j;;", 13) == 0)&&(str2.len == 12), "test 1");
    str2.data++;
    str2.len--;

    ngx_str_t str3 = n_strstr(str2, sep);
    assert( wp, str3.data != NULL && (memcmp(str3.data, ";hi;j;;", 8) == 0)&&(str3.len == 7), "test 2");
    str3.data++;
    str3.len--;
    ngx_str_t str4 = n_strstr(str3, sep);
    assert( wp, str4.data != NULL && (memcmp(str4.data, ";j;;", 5) == 0)&&(str4.len == 4), "test 3");
    str4.data++;
    str4.len--;

    ngx_str_t str5 = n_strstr(str4, sep);
    assert( wp, str5.data != NULL && (memcmp(str5.data, ";;", 3) == 0)&&(str5.len == 2), "test 4");
    str5.data++;
    str5.len--;

    ngx_str_t str6 = n_strstr(str5, sep);
    assert( wp, str6.data != NULL && (memcmp(str6.data, ";", 2) == 0)&&(str6.len == 1), "test 5");


    ngx_str_t str7 = n_strstr(NG(""), NG(""));
    assert( wp, str7.data != NULL && (memcmp(str7.data, "", 1) == 0)&&(str7.len == 0), "test 6");

    ngx_str_t str8 = n_strstr(NG(""), NG(";"));
    assert( wp, str8.data == NULL && (str8.len == 0), "test 7");

    outWrite("DONE<br><br>");

    return 0;
}


static int test_ngx_array_to_cstring(webs_t *wp, char** params, int nparams)
{
    outWrite("ngx_array_to_cstring: <br>");

    ngx_array_t* arr = ngx_array_create(wp->pool, 9, sizeof(char));
    char* ptr = ngx_array_push_n(arr, 9);

    memcpy(ptr, "test123 a", 9);

    char* str = ngx_array_to_cstring(wp->pool, arr);

    assert( wp, str != NULL && (memcmp(str, "test123 a", 10) == 0), "test 1");

    char* ptr2 = ngx_array_push_n(arr, 2);
    memcpy(ptr2, "bc", 2);

    str = ngx_array_to_cstring(wp->pool, arr);
    assert( wp, str != NULL && (memcmp(str, "test123 abc", 12) == 0), "test 2");

    outWrite("DONE<br><br>");
    return 0;

}

static int test_cstring_trim_left(webs_t *wp, char** params, int nparams)
{
    outWrite("cstring_trim_left: <br>");

    char* str1 = cstring_trim_left(wp->pool, " aaa", ' ');
    assert( wp, str1 != NULL && (memcmp(str1, "aaa", 4) == 0), "test 1");

    char* str2 = cstring_trim_left(wp->pool, "  aaa", ' ');
    assert( wp, str2 != NULL && (memcmp(str2, "aaa", 4) == 0), "test 2");

    char* str3 = cstring_trim_left(wp->pool, "  aaa  ", ' ');
    assert( wp, str3 != NULL && (memcmp(str3, "aaa  ", 6) == 0), "test 3");

    char* str4 = cstring_trim_left(wp->pool, "aaa", ' ');
    assert( wp, str4 != NULL && (memcmp(str4, "aaa", 4) == 0), "test 4");

    char* str5 = cstring_trim_left(wp->pool, "", ' ');
    assert( wp, str5 != NULL && (memcmp(str5, "", 1) == 0), "test 5");

    outWrite("DONE<br><br>");

    return 0;
}

static int test_cstring_trim_right(webs_t *wp, char** params, int nparams)
{
    outWrite("cstring_trim_right: <br>");

    char* str1 = cstring_trim_right(wp->pool, "bbb ", ' ');
    assert( wp, str1 != NULL && (memcmp(str1, "bbb", 4) == 0), "test 1");

    char* str2 = cstring_trim_right(wp->pool, "bbb  ", ' ');
    assert( wp, str2 != NULL && (memcmp(str2, "bbb", 4) == 0), "test 2");

    char* str3 = cstring_trim_right(wp->pool, "  bbb  ", ' ');
    assert( wp, str3 != NULL && (memcmp(str3, "  bbb", 6) == 0), "test 3");

    char* str4 = cstring_trim_right(wp->pool, "bbb", ' ');
    assert( wp, str4 != NULL && (memcmp(str4, "bbb", 4) == 0), "test 4");

    char* str5 = cstring_trim_right(wp->pool, "", ' ');
    assert( wp, str5 != NULL && (memcmp(str5, "", 1) == 0), "test 5");

    outWrite("DONE<br><br>");

    return 0;
}

static int test_cstring_trim(webs_t *wp, char** params, int nparams)
{
    outWrite("cstring_trim: <br>");

    char* str1 = cstring_trim(wp->pool, "bbb ", ' ');
    assert( wp, str1 != NULL && (memcmp(str1, "bbb", 4) == 0), "test 1");

    char* str2 = cstring_trim(wp->pool, " bbb", ' ');
    assert( wp, str2 != NULL && (memcmp(str2, "bbb", 4) == 0), "test 2");

    char* str3 = cstring_trim(wp->pool, "  bbb  ", ' ');
    assert( wp, str3 != NULL && (memcmp(str3, "bbb", 4) == 0), "test 3");

    char* str4 = cstring_trim(wp->pool, "bbb", ' ');
    assert( wp, str4 != NULL && (memcmp(str4, "bbb", 4) == 0), "test 4");

    char* str5 = cstring_trim(wp->pool, "", ' ');
    assert( wp, str5 != NULL && (memcmp(str5, "", 1) == 0), "test 5");

    outWrite("DONE<br><br>");

    return 0;
}



static int test_ngx_to_cstring(webs_t *wp, char** params, int nparams)
{
    outWrite("ngx_to_cstring: <br>");
    char* str1 = ngx_to_cstring(wp->pool, NG("test string"));
    assert( wp, str1 != NULL && memcmp(str1, "test string", sizeof("test string")) == 0, "test 1");


    char* str2 = ngx_to_cstring(wp->pool, NG(""));
    assert( wp, str2 != NULL && str2[0] == '\0', "test 2");

    ngx_str_t str;
    str.data = NULL;
    str.len = 0;
    char* str3 = ngx_to_cstring(wp->pool, str);
    assert( wp, str3 == NULL, "test 3");

    outWrite("DONE<br><br>");

    return 0;
}

static int test_ngx_string_split(webs_t *wp, char** params, int nparams)
{
    outWrite("ngx_string_split: <br>");

    ngx_str_t str = NG("abc;defg;hi;j;;");
    ngx_array_t* arr = ngx_string_split(wp->pool, str, NG(";"));
    ngx_str_t* tokens = (ngx_str_t*)arr->elts;

    assert( wp, arr != NULL, "test 1");

    assert( wp, n_strcmp(tokens[0], NG("abc")) == 0 , "test 2");
    assert( wp, n_strcmp(tokens[1], NG("defg")) == 0 , "test 3");
    assert( wp, n_strcmp(tokens[2], NG("hi")) == 0 , "test 4");
    assert( wp, n_strcmp(tokens[3], NG("j")) == 0 , "test 5");
    assert( wp, n_strcmp(tokens[4], NG("")) == 0 , "test 6");
    assert( wp, n_strcmp(tokens[5], NG("")) == 0 , "test 7");



    outWrite("DONE<br><br>");

    return 0;

}

static int test_cstring_split(webs_t *wp, char** params, int nparams)
{
    outWrite("cstring_split: <br>");

    char* str = strdup("abc;defg;hi;j;;");
    ngx_array_t* arr = cstring_split(wp->pool, str, ";");
    char** tokens = (char**)(arr->elts);

    assert( wp, arr != NULL, "test 1");

    assert( wp, strcmp(tokens[0], "abc") == 0 , "test 2");
    assert( wp, strcmp(tokens[1], "defg") == 0 , "test 3");
    assert( wp, strcmp(tokens[2], "hi") == 0 , "test 4");
    assert( wp, strcmp(tokens[3], "j") == 0 , "test 5");
    assert( wp, strcmp(tokens[4], "") == 0 , "test 6");
    assert( wp, strcmp(tokens[5], "") == 0 , "test 7");

    outWrite("DONE<br><br>");

    return 0;

}


static void test1(webs_t* wp, char_t *path, char_t *query)
{
//    ngx_str_t str = NG("a+b+c%20_e");
//    ngx_str_t out = n_unescape_uri(wp->pool, str, NGX_UNESCAPE_URI);
/*    unsigned int i;
    ngx_array_t* users = get_passwd_users(wp->pool);
    for (i=0;i<users->nelts;i++)
    {
        char* username = ((char**)users->elts)[i];

        outWrite("user %i: %s <br>\n", i, username);
    }
*/
//    outWrite("user: %s <br>", wp->auth_username);
//    outWrite("role: %d <br>", wp->auth_role);

/*
    unsigned char* p = (unsigned char*)ngx_pcalloc(wp->pool, str.len+1);
    if (p == NULL) {
        return;
    }
    ngx_str_t out;
    out.data = p;

    ngx_unescape_uri(&p, &str.data, str.len, NGX_UNESCAPE_URI);

    out.len = p - out.data;*/

//    outWrite("out: %.*s", out.len, out.data);

//    out.data = p;
//    out.len = strlen((char*)out.data);

/*    char_t *str = websGetVar(wp, "str", "0");
    if (str == NULL)
    {
        outWrite("str is NULL <br>");
        websDone(wp, 200);
        return;
    }

    outWrite("str: |%s|\n", str);
*/

//    ngx_strstr(str, "2");

/*    unsigned int i = 0;
    char_t *str = websGetVar(wp, T("str"), T("0"));
    char_t *sep = websGetVar(wp, T("sep"), T("0"));
    ngx_array_t* args = cstring_split(wp->pool, str, sep);

    outWrite("str: %s \n", str);
    outWrite("str: %s \n", sep);

    for (i=0;i<args->nelts;i++)
    {
        char* token = ((char**) args->elts)[i];
        outWrite("%i: %s \n", i, token);
    }

//    outWrite("test %s", arg1);
//	doSystem("echo done > /opt/nginx/debug.log");
*/

//    outWrite("test2 %s", str);
//    websDone(wp, 200);
    websError(wp, 500, "Test1 OK!");
}


void asp_mod_test_init()
{
/*
    aspDefineFunc("test_n_skipchar", test_n_skipchar, EVERYONE);
    aspDefineFunc("test_n_strcmp", test_n_strcmp, EVERYONE);
    aspDefineFunc("test_n_strstr", test_n_strstr, EVERYONE);

    aspDefineFunc("test_ngx_array_to_cstring", test_ngx_array_to_cstring, EVERYONE);

    aspDefineFunc("test_ngx_to_cstring", test_ngx_to_cstring, EVERYONE);
    aspDefineFunc("test_cstring_trim_left", test_cstring_trim_left, EVERYONE);
    aspDefineFunc("test_cstring_trim_right", test_cstring_trim_right, EVERYONE);
    aspDefineFunc("test_cstring_trim", test_cstring_trim, EVERYONE);

    aspDefineFunc("test_cstring_split", test_cstring_split, EVERYONE);
    aspDefineFunc("test_ngx_string_split", test_ngx_string_split, EVERYONE);

    aspDefineFunc("test_cstring_split_inplace", test_cstring_split_inplace, EVERYONE);
    aspDefineFunc("test_ngx_split_inplace", test_ngx_split_inplace, EVERYONE);

    websFormDefine("test1", test1, EVERYONE);
*/
}
