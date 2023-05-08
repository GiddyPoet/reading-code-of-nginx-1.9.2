
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static ngx_int_t ngx_http_static_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_static_init(ngx_conf_t *cf);


ngx_http_module_t  ngx_http_static_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_static_init,                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/*
location / {			
    index index11.html	#蹇