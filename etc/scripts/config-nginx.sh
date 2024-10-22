#!/bin/sh

# include global config
. /etc/scripts/global.sh

eval `nvram_buf_get 2860 RemoteManagementPort RemoteManagementPortHTTPS nginx_nolog nginx_autoindex`



if [ "$RemoteManagementPort" = "" ];then
    RemoteManagementPort=80
fi

if [ "$RemoteManagementPortHTTPS" = "" ];then
    RemoteManagementPortHTTPS=443
fi


NGINX_CONFIG_FILE="/etc/nginx/nginx.conf"

cat <<EOT > $NGINX_CONFIG_FILE

worker_processes  1;

events {
    worker_connections  128;
}

http {
    include       mime.types;
    default_type  application/octet-stream;

    client_max_body_size 32m;
    client_body_buffer_size 1m;


    sendfile        off;

    keepalive_timeout  120;
    keepalive_requests 300;

#    error_log /var/log/nginx-debug.log;
#    access_log syslog:server=unix:/dev/log,facility=local7,tag=nginx,severity=info,nohostname;
EOT

if [ "$nginx_nolog" != "1" ];then
cat <<EOT >> $NGINX_CONFIG_FILE
    error_log syslog:server=unix:/dev/log,facility=local7,tag=nginx,severity=info,nohostname debug;
EOT
fi

cat <<EOT >> $NGINX_CONFIG_FILE
    server {
EOT

if [ -e "/lib/libssl.so" ]; then
if [ "$RemoteManagementPortHTTPS" != "0" ];then
    cat <<EOT >> $NGINX_CONFIG_FILE
        listen       $RemoteManagementPortHTTPS ssl;

        ssl_protocols       TLSv1 TLSv1.1 TLSv1.2;
        ssl_ciphers         AES128-SHA:AES256-SHA:RC4-SHA:DES-CBC3-SHA:RC4-MD5;
        ssl_certificate     /etc/certs/server.pem;
        ssl_certificate_key /etc/certs/server.key;
        ssl_session_cache   shared:SSL:10m;
        ssl_session_timeout 10m;
        ssl_prefer_server_ciphers  on;
EOT
fi
fi

if [ "$RemoteManagementPort" != "0" ];then
cat <<EOT >> $NGINX_CONFIG_FILE
        listen       $RemoteManagementPort;
EOT
fi

cat <<EOT >> $NGINX_CONFIG_FILE
        server_name  localhost;
        keepalive_timeout   120;
	keepalive_requests 300;

        location / {
            root   /tmp/web;
            index  index.html index.htm home.asp;
            wive " ";
            wive_types application/javascript;

    	    # no cache (disable caches and send current time as modifided time)
	    add_header Cache-Control 'no-store, no-cache, must-revalidate, post-check=0, pre-check=0, proxy-revalidate, max-age=0';
	    if_modified_since off;
	    etag off;
	    expires -1;
	    add_header Last-Modified \$sent_http_Expires;

            location = /login.asp {
                disable_auth;
            }

            location = /robots.txt {
                disable_auth;
            }
        }

EOT

if [ "$nginx_autoindex" == "1" ];then
cat <<EOT >> $NGINX_CONFIG_FILE
        location /media {
            root   /;
            autoindex on;
        }
EOT
fi

cat <<EOT >> $NGINX_CONFIG_FILE
        location /goform/ {
            asp_post_form;
        }

        location = /goform/auth {
            asp_post_form;
            disable_auth;
        }


        location = /cgi-bin/upload.cgi {
            upload_pass /goform/firmwareUploadForm;
            upload_pass_args on;

            upload_pass_form_field "reset_rwfs";

            upload_store /tmp;
            upload_set_form_field \$upload_field_name.name "\$upload_file_name";
            upload_set_form_field \$upload_field_name.content_type "\$upload_content_type";
            upload_set_form_field \$upload_field_name.path "\$upload_tmp_path";
            upload_aggregate_form_field "\$upload_field_name.size" "\$upload_file_size";

            upload_cleanup 400 404 499 500-505;
        }

        location = /cgi-bin/upload_rwfs.cgi {
            upload_pass /goform/rwfsUploadForm;
            upload_pass_args on;

            upload_store /tmp;
            upload_set_form_field \$upload_field_name.name "\$upload_file_name";
            upload_set_form_field \$upload_field_name.content_type "\$upload_content_type";
            upload_set_form_field \$upload_field_name.path "\$upload_tmp_path";
            upload_aggregate_form_field "\$upload_field_name.size" "\$upload_file_size";

            upload_cleanup 400 404 499 500-505;
        }

        location = /cgi-bin/upload_settings.cgi {
            upload_pass /goform/settingsUploadForm;
            upload_pass_args on;

            upload_store /tmp;
            upload_set_form_field \$upload_field_name.name "\$upload_file_name";
            upload_set_form_field \$upload_field_name.content_type "\$upload_content_type";
            upload_set_form_field \$upload_field_name.path "\$upload_tmp_path";
            upload_aggregate_form_field "\$upload_field_name.size" "\$upload_file_size";

            upload_cleanup 400 404 499 500-505;
        }

        #error_page  404              /404.html;

        # redirect server error pages to the static page /50x.html
        #
        error_page   500 502 503 504  /50x.html;
        location = /50x.html {
            root   html;
        }

    }
}

EOT
