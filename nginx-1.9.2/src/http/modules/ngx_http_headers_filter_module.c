
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct ngx_http_header_val_s  ngx_http_header_val_t;

typedef ngx_int_t (*ngx_http_set_header_pt)(ngx_http_request_t *r,
    ngx_http_header_val_t *hv, ngx_str_t *value);


typedef struct { //ngx_http_set_headers涓