
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include <zlib.h>


typedef struct {
    ngx_flag_t           enable;
    ngx_flag_t           no_buffer;

    ngx_hash_t           types;

    ngx_bufs_t           bufs;

    size_t               postpone_gzipping;
    ngx_int_t            level;
    size_t               wbits;
    size_t               memlevel;
    ssize_t              min_length;

    ngx_array_t         *types_keys;
} ngx_http_gzip_conf_t;


typedef struct {
    ngx_chain_t         *in;
    ngx_chain_t         *free;
    ngx_chain_t         *busy;
    ngx_chain_t         *out;
    ngx_chain_t        **last_out;

    ngx_chain_t         *copied;
    ngx_chain_t         *copy_buf;

    ngx_buf_t           *in_buf;
    ngx_buf_t           *out_buf;
    ngx_int_t            bufs;

    void                *preallocated;
    char                *free_mem;
    ngx_uint_t           allocated;

    int                  wbits;
    int                  memlevel;

    unsigned             flush:4;
    unsigned             redo:1;
    unsigned             done:1;
    unsigned             nomem:1;
    unsigned             gzheader:1;
    unsigned             buffering:1;

    size_t               zin;
    size_t               zout;

    uint32_t             crc32;
    z_stream             zstream;
    ngx_http_request_t  *request;
} ngx_http_gzip_ctx_t;


#if (NGX_HAVE_LITTLE_ENDIAN && NGX_HAVE_NONALIGNED)

struct gztrailer {
    uint32_t  crc32;
    uint32_t  zlen;
};

#else /* NGX_HAVE_BIG_ENDIAN || !NGX_HAVE_NONALIGNED */

struct gztrailer {
    u_char  crc32[4];
    u_char  zlen[4];
};

#endif


static void ngx_http_gzip_filter_memory(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);
static ngx_int_t ngx_http_gzip_filter_buffer(ngx_http_gzip_ctx_t *ctx,
    ngx_chain_t *in);
static ngx_int_t ngx_http_gzip_filter_deflate_start(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);
static ngx_int_t ngx_http_gzip_filter_gzheader(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);
static ngx_int_t ngx_http_gzip_filter_add_data(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);
static ngx_int_t ngx_http_gzip_filter_get_buf(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);
static ngx_int_t ngx_http_gzip_filter_deflate(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);
static ngx_int_t ngx_http_gzip_filter_deflate_end(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);

static void *ngx_http_gzip_filter_alloc(void *opaque, u_int items,
    u_int size);
static void ngx_http_gzip_filter_free(void *opaque, void *address);
static void ngx_http_gzip_filter_free_copy_buf(ngx_http_request_t *r,
    ngx_http_gzip_ctx_t *ctx);

static ngx_int_t ngx_http_gzip_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_gzip_ratio_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

static ngx_int_t ngx_http_gzip_filter_init(ngx_conf_t *cf);
static void *ngx_http_gzip_create_conf(ngx_conf_t *cf);
static char *ngx_http_gzip_merge_conf(ngx_conf_t *cf,
    void *parent, void *child);
static char *ngx_http_gzip_window(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_gzip_hash(ngx_conf_t *cf, void *post, void *data);


static ngx_conf_num_bounds_t  ngx_http_gzip_comp_level_bounds = {
    ngx_conf_check_num_bounds, 1, 9
};

static ngx_conf_post_handler_pt  ngx_http_gzip_window_p = ngx_http_gzip_window;
static ngx_conf_post_handler_pt  ngx_http_gzip_hash_p = ngx_http_gzip_hash;


static ngx_command_t  ngx_http_gzip_filter_commands[] = {

    { ngx_string("gzip"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, enable),
      NULL },

    { ngx_string("gzip_buffers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE2,
      ngx_conf_set_bufs_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, bufs),
      NULL },

    { ngx_string("gzip_types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, types_keys),
      &ngx_http_html_default_types[0] },

    { ngx_string("gzip_comp_level"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, level),
      &ngx_http_gzip_comp_level_bounds },

    { ngx_string("gzip_window"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, wbits),
      &ngx_http_gzip_window_p },

    { ngx_string("gzip_hash"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, memlevel),
      &ngx_http_gzip_hash_p },

    { ngx_string("postpone_gzipping"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, postpone_gzipping),
      NULL },

    { ngx_string("gzip_no_buffer"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, no_buffer),
      NULL },

    { ngx_string("gzip_min_length"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_gzip_conf_t, min_length),
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_gzip_filter_module_ctx = {
    ngx_http_gzip_add_variables,           /* preconfiguration */
    ngx_http_gzip_filter_init,             /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_gzip_create_conf,             /* create location configuration */
    ngx_http_gzip_merge_conf               /* merge location configuration */
};


/*
琛