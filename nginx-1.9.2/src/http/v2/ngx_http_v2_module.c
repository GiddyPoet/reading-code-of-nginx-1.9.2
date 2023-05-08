
/*
 * Copyright (C) Nginx, Inc.
 * Copyright (C) Valentin V. Bartenev
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_http_v2_module.h>


static ngx_int_t ngx_http_v2_add_variables(ngx_conf_t *cf);

static ngx_int_t ngx_http_v2_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);

static ngx_int_t ngx_http_v2_module_init(ngx_cycle_t *cycle);

static void *ngx_http_v2_create_main_conf(ngx_conf_t *cf);
static char *ngx_http_v2_init_main_conf(ngx_conf_t *cf, void *conf);
static void *ngx_http_v2_create_srv_conf(ngx_conf_t *cf);
static char *ngx_http_v2_merge_srv_conf(ngx_conf_t *cf, void *parent,
    void *child);
static void *ngx_http_v2_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_v2_merge_loc_conf(ngx_conf_t *cf, void *parent,
    void *child);

static char *ngx_http_v2_recv_buffer_size(ngx_conf_t *cf, void *post,
    void *data);
static char *ngx_http_v2_pool_size(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_v2_streams_index_mask(ngx_conf_t *cf, void *post,
    void *data);
static char *ngx_http_v2_chunk_size(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_v2_spdy_deprecated(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


static ngx_conf_post_t  ngx_http_v2_recv_buffer_size_post =
    { ngx_http_v2_recv_buffer_size };
static ngx_conf_post_t  ngx_http_v2_pool_size_post =
    { ngx_http_v2_pool_size };
static ngx_conf_post_t  ngx_http_v2_streams_index_mask_post =
    { ngx_http_v2_streams_index_mask };
static ngx_conf_post_t  ngx_http_v2_chunk_size_post =
    { ngx_http_v2_chunk_size };


static ngx_command_t  ngx_http_v2_commands[] = {
    //璁剧疆姣