
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#define NGX_HTTP_REFERER_NO_URI_PART  ((void *) 4)

/*
The ngx_http_referer_module module is used to block access to a site for requests with invalid values in the 