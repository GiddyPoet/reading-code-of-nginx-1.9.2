
/*
 * Copyright (C) Ruslan Ermilov
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_upstream_zone(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static ngx_int_t ngx_http_upstream_init_zone(ngx_shm_zone_t *shm_zone,
    void *data);
static ngx_int_t ngx_http_upstream_zone_copy_peers(ngx_slab_pool_t *shpool,
    ngx_http_upstream_srv_conf_t *uscf);


static ngx_command_t  ngx_http_upstream_zone_commands[] = {

    { ngx_string("zone"),
      NGX_HTTP_UPS_CONF|NGX_CONF_TAKE12,
      ngx_http_upstream_zone,
      0,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_upstream_zone_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};


ngx_module_t  ngx_http_upstream_zone_module = {
    NGX_MODULE_V1,
    &ngx_http_upstream_zone_module_ctx,    /* module context */
    ngx_http_upstream_zone_commands,       /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

//