
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define NGX_HTTP_SSI_ERROR          1

#define NGX_HTTP_SSI_DATE_LEN       2048

#define NGX_HTTP_SSI_ADD_PREFIX     1
#define NGX_HTTP_SSI_ADD_ZERO       2


typedef struct {
    ngx_flag_t    enable;
    ngx_flag_t    silent_errors;
    ngx_flag_t    ignore_recycled_buffers;
    ngx_flag_t    last_modified;

    ngx_hash_t    types;

    size_t        min_file_chunk;
    size_t        value_len;

    ngx_array_t  *types_keys;
} ngx_http_ssi_loc_conf_t;


typedef struct {
    ngx_str_t     name;
    ngx_uint_t    key;
    ngx_str_t     value;
} ngx_http_ssi_var_t;


typedef struct {
    ngx_str_t     name;
    ngx_chain_t  *bufs;
    ngx_uint_t    count;
} ngx_http_ssi_block_t;


typedef enum {
    ssi_start_state = 0,
    ssi_tag_state,
    ssi_comment0_state,
    ssi_comment1_state,
    ssi_sharp_state,
    ssi_precommand_state,
    ssi_command_state,
    ssi_preparam_state,
    ssi_param_state,
    ssi_preequal_state,
    ssi_prevalue_state,
    ssi_double_quoted_value_state,
    ssi_quoted_value_state,
    ssi_quoted_symbol_state,
    ssi_postparam_state,
    ssi_comment_end0_state,
    ssi_comment_end1_state,
    ssi_error_state,
    ssi_error_end0_state,
    ssi_error_end1_state
} ngx_http_ssi_state_e;


static ngx_int_t ngx_http_ssi_output(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx);
static void ngx_http_ssi_buffered(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx);
static ngx_int_t ngx_http_ssi_parse(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx);
static ngx_str_t *ngx_http_ssi_get_variable(ngx_http_request_t *r,
    ngx_str_t *name, ngx_uint_t key);
static ngx_int_t ngx_http_ssi_evaluate_string(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t *text, ngx_uint_t flags);
static ngx_int_t ngx_http_ssi_regex_match(ngx_http_request_t *r,
    ngx_str_t *pattern, ngx_str_t *str);

static ngx_int_t ngx_http_ssi_include(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_stub_output(ngx_http_request_t *r, void *data,
    ngx_int_t rc);
static ngx_int_t ngx_http_ssi_set_variable(ngx_http_request_t *r, void *data,
    ngx_int_t rc);
static ngx_int_t ngx_http_ssi_echo(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_config(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_set(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_if(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_else(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_endif(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_block(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);
static ngx_int_t ngx_http_ssi_endblock(ngx_http_request_t *r,
    ngx_http_ssi_ctx_t *ctx, ngx_str_t **params);

static ngx_int_t ngx_http_ssi_date_gmt_local_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t gmt);

static ngx_int_t ngx_http_ssi_preconfiguration(ngx_conf_t *cf);
static void *ngx_http_ssi_create_main_conf(ngx_conf_t *cf);
static char *ngx_http_ssi_init_main_conf(ngx_conf_t *cf, void *conf);
static void *ngx_http_ssi_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_ssi_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);
static ngx_int_t ngx_http_ssi_filter_init(ngx_conf_t *cf);


static ngx_command_t  ngx_http_ssi_filter_commands[] = {

    { ngx_string("ssi"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, enable),
      NULL },

    { ngx_string("ssi_silent_errors"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, silent_errors),
      NULL },

    { ngx_string("ssi_ignore_recycled_buffers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, ignore_recycled_buffers),
      NULL },

    { ngx_string("ssi_min_file_chunk"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, min_file_chunk),
      NULL },

    { ngx_string("ssi_value_length"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, value_len),
      NULL },

    { ngx_string("ssi_types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_types_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, types_keys),
      &ngx_http_html_default_types[0] },

    { ngx_string("ssi_last_modified"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_ssi_loc_conf_t, last_modified),
      NULL },

      ngx_null_command
};



static ngx_http_module_t  ngx_http_ssi_filter_module_ctx = {
    ngx_http_ssi_preconfiguration,         /* preconfiguration */
    ngx_http_ssi_filter_init,              /* postconfiguration */

    ngx_http_ssi_create_main_conf,         /* create main configuration */
    ngx_http_ssi_init_main_conf,           /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_ssi_create_loc_conf,          /* create location configuration */
    ngx_http_ssi_merge_loc_conf            /* merge location configuration */
};


/*
琛