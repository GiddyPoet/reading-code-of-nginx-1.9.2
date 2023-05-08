
/*
 * Copyright (C) Nginx, Inc.
 * Copyright (C) Valentin V. Bartenev
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#define NGX_HTTP_V2_TABLE_SIZE  4096


static ngx_int_t ngx_http_v2_table_account(ngx_http_v2_connection_t *h2c,
    size_t size);

/**/
//header甯у