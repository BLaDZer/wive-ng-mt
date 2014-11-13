grep 'sizeof("CHAR")' tmp.h | sed -e 's|^.*\(stringpool_str[0-9]*\).*$|  (int)(long)\&((struct stringpool_t *)0)->\1,|'
grep 'sizeof("WCHAR_T")' tmp.h | sed -e 's|^.*\(stringpool_str[0-9]*\).*$|  (int)(long)\&((struct stringpool_t *)0)->\1,|'
