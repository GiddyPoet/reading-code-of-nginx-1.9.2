
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#define NGX_HTTP_CHARSET_OFF    -2
#define NGX_HTTP_NO_CHARSET     -3
#define NGX_HTTP_CHARSET_VAR    0x10000

/* 1 byte length and up to 3 bytes for the UTF-8 encoding of the UCS-2 */
#define NGX_UTF_LEN             4

#define NGX_HTML_ENTITY_LEN     (sizeof("&#1114111;") - 1)


typedef struct {
    u_char                    **tables;
    ngx_str_t                   name;

    unsigned                    length:16;
    unsigned                    utf8:1;
} ngx_http_charset_t;


typedef struct {
    ngx_int_t                   src;
    ngx_int_t                   dst;
} ngx_http_charset_recode_t;


typedef struct {
    ngx_int_t                   src;
    ngx_int_t                   dst;
    u_char                     *src2dst;
    u_char                     *dst2src;
} ngx_http_charset_tables_t;


typedef struct {
    ngx_array_t                 charsets;       /* ngx_http_charset_t */
    ngx_array_t                 tables;         /* ngx_http_charset_tables_t */
    ngx_array_t                 recodes;        /* ngx_http_charset_recode_t */
} ngx_http_charset_main_conf_t;


typedef struct {
    ngx_int_t                   charset;
    ngx_int_t                   source_charset;
    ngx_flag_t                  override_charset;

    ngx_hash_t                  types;
    ngx_array_t                *types_keys;
} ngx_http_charset_loc_conf_t;


typedef struct {
    u_char                     *table;
    ngx_int_t                   charset;
    ngx_str_t                   charset_name;

    ngx_chain_t                *busy;
    ngx_chain_t                *free_bufs;
    ngx_chain_t                *free_buffers;

    size_t                      saved_len;
    u_char                      saved[NGX_UTF_LEN];

    unsigned                    length:16;
    unsigned                    from_utf8:1;
    unsigned                    to_utf8:1;
} ngx_http_charset_ctx_t;


typedef struct {
    ngx_http_charset_tables_t  *table;
    ngx_http_charset_t         *charset;
    ngx_uint_t                  characters;
} ngx_http_charset_conf_ctx_t;


static ngx_int_t ngx_http_destination_charset(ngx_http_request_t *r,
    ngx_str_t *name);
static ngx_int_t ngx_http_main_request_charset(ngx_http_request_t *r,
    ngx_str_t *name);
static ngx_int_t ngx_http_source_charset(ngx_http_request_t *r,
    ngx_str_t *name);
static ngx_int_t ngx_http_get_charset(ngx_http_request_t *r, ngx_str_t *name);
static ngx_inline void ngx_http_set_charset(ngx_http_request_t *r,
    ngx_str_t *charset);
static ngx_int_t ngx_http_charset_ctx(ngx_http_request_t *r,
    ngx_http_charset_t *charsets, ngx_int_t charset, ngx_int_t source_charset);
static ngx_uint_t ngx_http_charset_recode(ngx_buf_t *b, u_char *table);
static ngx_chain_t *ngx_http_charset_recode_from_utf8(ngx_pool_t *pool,
    ngx_buf_t *buf, ngx_http_charset_ctx_t *ctx);
static ngx_chain_t *ngx_http_charset_recode_to_utf8(ngx_pool_t *pool,
    ngx_buf_t *buf, ngx_http_charset_ctx_t *ctx);

static ngx_chain_t *ngx_http_charset_get_buf(ngx_pool_t *pool,
    ngx_http_charset_ctx_t *ctx);
static ngx_chain_t *ngx_http_charset_get_buffer(ngx_pool_t *pool,
    ngx_http_charset_ctx_t *ctx, size_t size);

static char *ngx_http_charset_map_block(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_charset_map(ngx_conf_t *cf, ngx_command_t *dummy,
    void *conf);

static char *ngx_http_set_charset_slot(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static ngx_int_t ngx_http_add_charset(ngx_array_t *charsets, ngx_str_t *name);

static void *ngx_http_charset_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_charset_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_charset_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);
static ngx_int_t ngx_http_charset_postconfiguration(ngx_conf_t *cf);


ngx_str_t  ngx_http_charset_default_types[] = {
    ngx_string("text/html"),
    ngx_string("text/xml"),
    ngx_string("text/plain"),
    ngx_string("text/vnd.wap.wml"),
    ngx_string("application/javascript"),
    ngx_string("application/rss+xml"),
    ngx_null_string
};


static ngx_command_t  ngx_http_charset_filter_commands[] = {

    { ngx_string("charset"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF
                        |NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
      ngx_http_set_charset_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_charset_loc_conf_t, charset),
      NULL },

    { ngx_string("source_charset"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF
                        |NGX_HTTP_LIF_CONF|NGX_CONF_TAKE1,
      ngx_http_set_charset_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_charset_loc_conf_t, source_charset),
      NULL },

    { ngx_string("override_charset"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF
                        |NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_charset_loc_conf_t, override_charset),
      NULL },

    { ngx_string("charset_types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_charset_loc_conf_t, types_keys),
      &ngx_http_charset_default_types[0] },

    { ngx_string("charset_map"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_TAKE2,
      ngx_http_charset_map_block,
      NGX_HTTP_MAIN_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_charset_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_charset_postconfiguration,    /* postconfiguration */

    ngx_http_charset_create_main_conf,     /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_charset_create_loc_conf,      /* create location configuration */
    ngx_http_charset_merge_loc_conf        /* merge location configuration */
};


/*
琛