
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>


static ngx_int_t ngx_http_header_filter_init(ngx_conf_t *cf);
static ngx_int_t ngx_http_header_filter(ngx_http_request_t *r);


static ngx_http_module_t  ngx_http_header_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_header_filter_init,           /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL,                                  /* merge location configuration */
};


/*
琛