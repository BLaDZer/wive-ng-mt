#!/bin/bash

echo "==================CONFIGURE-NGINX=============================="
APROOTDIR=`pwd`

HBUILD=`uname -m`-pc-linux-gnu
HTARGET=mipsel-linux

CONFOPTS="--build=$HTARGET --crossbuild=$HTARGET"

CONFOPTS="$CONFOPTS --with-cc-opt=-I$ROOTDIR/lib/shared/include,-I$ROOTDIR/libnvram,-ffunction-sections,-fdata-sections,-Os"
CONFOPTS="$CONFOPTS --with-ld-opt=-L$ROOTDIR/lib/shared/lib"

if [ -n "$CONFIG_RT2880_FLASH_16M$CONFIG_RT2880_FLASH_32M" ]; then
    CONFOPTS="$CONFOPTS --with-debug"
fi

CONFOPTS="$CONFOPTS --with-libatomic=NO"
CONFOPTS="$CONFOPTS --without-http_rewrite_module --without-http_gzip_module --without-http_upstream_zone_module"

CONFOPTS="$CONFOPTS --sbin-path=/bin/nginx"
CONFOPTS="$CONFOPTS --error-log-path=/var/log/nginx-error.log"
CONFOPTS="$CONFOPTS --http-log-path=/var/log/nginx-access.log"
CONFOPTS="$CONFOPTS --conf-path=/etc/nginx/nginx.conf"
CONFOPTS="$CONFOPTS --lock-path=/tmp/nginx.lock"
CONFOPTS="$CONFOPTS --pid-path=/tmp/nginx.pid"
CONFOPTS="$CONFOPTS --modules-path=/share/nginx/modules"
CONFOPTS="$CONFOPTS --http-client-body-temp-path=/tmp/client_body_temp"
CONFOPTS="$CONFOPTS --http-proxy-temp-path=/tmp/proxy_temp"
CONFOPTS="$CONFOPTS --http-fastcgi-temp-path=/tmp/fastcgi"
CONFOPTS="$CONFOPTS --http-uwsgi-temp-path=/tmp/uwsgi"
CONFOPTS="$CONFOPTS --http-scgi-temp-path=/tmp/scgi"

CONFOPTS="$CONFOPTS --add-module=$APROOTDIR/extra-modules/nginx-http-wive"
CONFOPTS="$CONFOPTS --add-module=$APROOTDIR/extra-modules/nginx-upload-module"

CONFOPTS="$CONFOPTS \
  --with-http_stub_status_module \
  --without-http_charset_module \
  --without-http_gzip_module \
  --without-http_ssi_module \
  --without-http_userid_module \
  --without-http_access_module \
  --without-http_auth_basic_module \
  --without-http_autoindex_module \
  --without-http_geo_module \
  --without-http_map_module \
  --without-http_split_clients_module \
  --without-http_referer_module \
  --without-http_rewrite_module \
  --without-http_proxy_module \
  --without-http_fastcgi_module \
  --without-http_uwsgi_module \
  --without-http_scgi_module \
  --without-http_memcached_module \
  --without-http_limit_conn_module \
  --without-http_limit_req_module \
  --without-http_empty_gif_module \
  --without-http_browser_module \
  --without-http_upstream_ip_hash_module \
  --without-http_upstream_least_conn_module \
  --without-http_upstream_keepalive_module \
"

# build with support ssl only if project configured and builded with openssl
if [ -e ../../lib/shared/lib/libssl.so ]; then
    CONFOPTS="$CONFOPTS --with-http_ssl_module"
fi

./auto/configure $CONFOPTS
