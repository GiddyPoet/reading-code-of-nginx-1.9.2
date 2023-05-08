
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_str_t                name;
    ngx_array_t             *lengths;
    ngx_array_t             *values;
} ngx_http_index_t;


typedef struct { //绌洪