
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_http_variable_value_t       *value;
    u_short                          start;
    u_short                          end;
} ngx_http_geo_range_t;


typedef struct {
    ngx_radix_tree_t                *tree;
#if (NGX_HAVE_INET6)
    ngx_radix_tree_t                *tree6;
#endif
} ngx_http_geo_trees_t;


typedef struct {
    ngx_http_geo_range_t           **low;
    ngx_http_variable_value_t       *default_value;
} ngx_http_geo_high_ranges_t;


typedef struct {
    ngx_str_node_t                   sn;
    ngx_http_variable_value_t       *value;
    size_t                           offset;
} ngx_http_geo_variable_value_node_t;


typedef struct {
    ngx_http_variable_value_t       *value;
    ngx_str_t                       *net;
    ngx_http_geo_high_ranges_t       high;
    ngx_radix_tree_t                *tree;
#if (NGX_HAVE_INET6)
    ngx_radix_tree_t                *tree6;
#endif
    ngx_rbtree_t                     rbtree;
    ngx_rbtree_node_t                sentinel;
    ngx_array_t                     *proxies;
    ngx_pool_t                      *pool;
    ngx_pool_t                      *temp_pool;

    size_t                           data_size;

    ngx_str_t                        include_name;
    ngx_uint_t                       includes;
    ngx_uint_t                       entries;

    unsigned                         ranges:1;
    unsigned                         outside_entries:1;
    unsigned                         allow_binary_include:1;
    unsigned                         binary_include:1;
    unsigned                         proxy_recursive:1;
} ngx_http_geo_conf_ctx_t;


typedef struct {
    union {
        ngx_http_geo_trees_t         trees;
        ngx_http_geo_high_ranges_t   high;
    } u;

    ngx_array_t                     *proxies;
    unsigned                         proxy_recursive:1;

    ngx_int_t                        index;
} ngx_http_geo_ctx_t;


static ngx_int_t ngx_http_geo_addr(ngx_http_request_t *r,
    ngx_http_geo_ctx_t *ctx, ngx_addr_t *addr);
static ngx_int_t ngx_http_geo_real_addr(ngx_http_request_t *r,
    ngx_http_geo_ctx_t *ctx, ngx_addr_t *addr);
static char *ngx_http_geo_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_geo(ngx_conf_t *cf, ngx_command_t *dummy, void *conf);
static char *ngx_http_geo_range(ngx_conf_t *cf, ngx_http_geo_conf_ctx_t *ctx,
    ngx_str_t *value);
static char *ngx_http_geo_add_range(ngx_conf_t *cf,
    ngx_http_geo_conf_ctx_t *ctx, in_addr_t start, in_addr_t end);
static ngx_uint_t ngx_http_geo_delete_range(ngx_conf_t *cf,
    ngx_http_geo_conf_ctx_t *ctx, in_addr_t start, in_addr_t end);
static char *ngx_http_geo_cidr(ngx_conf_t *cf, ngx_http_geo_conf_ctx_t *ctx,
    ngx_str_t *value);
static char *ngx_http_geo_cidr_add(ngx_conf_t *cf, ngx_http_geo_conf_ctx_t *ctx,
    ngx_cidr_t *cidr, ngx_str_t *value, ngx_str_t *net);
static ngx_http_variable_value_t *ngx_http_geo_value(ngx_conf_t *cf,
    ngx_http_geo_conf_ctx_t *ctx, ngx_str_t *value);
static char *ngx_http_geo_add_proxy(ngx_conf_t *cf,
    ngx_http_geo_conf_ctx_t *ctx, ngx_cidr_t *cidr);
static ngx_int_t ngx_http_geo_cidr_value(ngx_conf_t *cf, ngx_str_t *net,
    ngx_cidr_t *cidr);
static char *ngx_http_geo_include(ngx_conf_t *cf, ngx_http_geo_conf_ctx_t *ctx,
    ngx_str_t *name);
static ngx_int_t ngx_http_geo_include_binary_base(ngx_conf_t *cf,
    ngx_http_geo_conf_ctx_t *ctx, ngx_str_t *name);
static void ngx_http_geo_create_binary_base(ngx_http_geo_conf_ctx_t *ctx);
static u_char *ngx_http_geo_copy_values(u_char *base, u_char *p,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);


static ngx_command_t  ngx_http_geo_commands[] = {

    { ngx_string("geo"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_TAKE12,
      ngx_http_geo_block,
      NGX_HTTP_MAIN_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_geo_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/*
worker_processes 1;
events {
worker_connections 1024;
 
}
 
http {
include         mime.types;
default_type    application/octet-stream;
sendfile        on;
keepalive_timeout 65;
geo $cmccip {
    default 1;