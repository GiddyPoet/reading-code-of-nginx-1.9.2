
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#if 0

typedef struct {
    ngx_buf_t     *buf;
    size_t         size;
    ngx_pool_t    *pool;
    size_t         alloc_size;
    ngx_chain_t  **last_out;
} ngx_http_autoindex_ctx_t;

#endif


typedef struct {
    ngx_str_t      name;
    size_t         utf_len;
    size_t         escape;
    size_t         escape_html;

    unsigned       dir:1;
    unsigned       file:1;

    time_t         mtime;
    off_t          size;
} ngx_http_autoindex_entry_t;


typedef struct {
    ngx_flag_t     enable;
    ngx_uint_t     format;
    ngx_flag_t     localtime;
    ngx_flag_t     exact_size;
} ngx_http_autoindex_loc_conf_t;


#define NGX_HTTP_AUTOINDEX_HTML         0
#define NGX_HTTP_AUTOINDEX_JSON         1
#define NGX_HTTP_AUTOINDEX_JSONP        2
#define NGX_HTTP_AUTOINDEX_XML          3

#define NGX_HTTP_AUTOINDEX_PREALLOCATE  50

#define NGX_HTTP_AUTOINDEX_NAME_LEN     50


static ngx_buf_t *ngx_http_autoindex_html(ngx_http_request_t *r,
    ngx_array_t *entries);
static ngx_buf_t *ngx_http_autoindex_json(ngx_http_request_t *r,
    ngx_array_t *entries, ngx_str_t *callback);
static ngx_int_t ngx_http_autoindex_jsonp_callback(ngx_http_request_t *r,
    ngx_str_t *callback);
static ngx_buf_t *ngx_http_autoindex_xml(ngx_http_request_t *r,
    ngx_array_t *entries);

static int ngx_libc_cdecl ngx_http_autoindex_cmp_entries(const void *one,
    const void *two);
static ngx_int_t ngx_http_autoindex_error(ngx_http_request_t *r,
    ngx_dir_t *dir, ngx_str_t *name);

static ngx_int_t ngx_http_autoindex_init(ngx_conf_t *cf);
static void *ngx_http_autoindex_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_autoindex_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);


static ngx_conf_enum_t  ngx_http_autoindex_format[] = {
    { ngx_string("html"), NGX_HTTP_AUTOINDEX_HTML },
    { ngx_string("json"), NGX_HTTP_AUTOINDEX_JSON },
    { ngx_string("jsonp"), NGX_HTTP_AUTOINDEX_JSONP },
    { ngx_string("xml"), NGX_HTTP_AUTOINDEX_XML },
    { ngx_null_string, 0 }
};


/*
location /{
root   /var/www/html;
autoindex on;
} 
杩