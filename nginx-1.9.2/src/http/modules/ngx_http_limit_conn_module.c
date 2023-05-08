
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    u_char                     color;
    u_char                     len;
    u_short                    conn;
    u_char                     data[1];
} ngx_http_limit_conn_node_t;


typedef struct {
    ngx_shm_zone_t            *shm_zone;
    ngx_rbtree_node_t         *node;
} ngx_http_limit_conn_cleanup_t;


typedef struct {
    ngx_rbtree_t              *rbtree;
    ngx_http_complex_value_t   key;
} ngx_http_limit_conn_ctx_t;


typedef struct {
    ngx_shm_zone_t            *shm_zone;
    ngx_uint_t                 conn;
} ngx_http_limit_conn_limit_t;


typedef struct {
    ngx_array_t                limits;
    ngx_uint_t                 log_level;
    ngx_uint_t                 status_code;
} ngx_http_limit_conn_conf_t;


static ngx_rbtree_node_t *ngx_http_limit_conn_lookup(ngx_rbtree_t *rbtree,
    ngx_str_t *key, uint32_t hash);
static void ngx_http_limit_conn_cleanup(void *data);
static ngx_inline void ngx_http_limit_conn_cleanup_all(ngx_pool_t *pool);

static void *ngx_http_limit_conn_create_conf(ngx_conf_t *cf);
static char *ngx_http_limit_conn_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);
static char *ngx_http_limit_conn_zone(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_limit_conn(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static ngx_int_t ngx_http_limit_conn_init(ngx_conf_t *cf);


static ngx_conf_enum_t  ngx_http_limit_conn_log_levels[] = {
    { ngx_string("info"), NGX_LOG_INFO },
    { ngx_string("notice"), NGX_LOG_NOTICE },
    { ngx_string("warn"), NGX_LOG_WARN },
    { ngx_string("error"), NGX_LOG_ERR },
    { ngx_null_string, 0 }
};


static ngx_conf_num_bounds_t  ngx_http_limit_conn_status_bounds = {
    ngx_conf_check_num_bounds, 400, 599
};


static ngx_command_t  ngx_http_limit_conn_commands[] = {
/*

璇