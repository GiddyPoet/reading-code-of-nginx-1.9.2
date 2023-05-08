
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_array_t                    caches;  /* ngx_http_file_cache_t * */
} ngx_http_fastcgi_main_conf_t;


typedef struct { //