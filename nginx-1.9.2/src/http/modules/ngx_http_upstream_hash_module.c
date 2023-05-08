
/*
 * Copyright (C) Roman Arutyunyan
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    uint32_t                            hash;
    ngx_str_t                          *server;
} ngx_http_upstream_chash_point_t;


typedef struct {
    ngx_uint_t                          number;
    ngx_http_upstream_chash_point_t     point[1];
} ngx_http_upstream_chash_points_t;


typedef struct {
    ngx_http_complex_value_t            key;
    ngx_http_upstream_chash_points_t   *points;
} ngx_http_upstream_hash_srv_conf_t;


typedef struct {
    /* the round robin data must be first */
    ngx_http_upstream_rr_peer_data_t    rrp;
    ngx_http_upstream_hash_srv_conf_t  *conf;
    ngx_str_t                           key;
    ngx_uint_t                          tries;
    ngx_uint_t                          rehash;
    uint32_t                            hash;
    ngx_event_get_peer_pt               get_rr_peer;
} ngx_http_upstream_hash_peer_data_t;


static ngx_int_t ngx_http_upstream_init_hash(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
static ngx_int_t ngx_http_upstream_init_hash_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
static ngx_int_t ngx_http_upstream_get_hash_peer(ngx_peer_connection_t *pc,
    void *data);

static ngx_int_t ngx_http_upstream_init_chash(ngx_conf_t *cf,
    ngx_http_upstream_srv_conf_t *us);
static int ngx_libc_cdecl
    ngx_http_upstream_chash_cmp_points(const void *one, const void *two);
static ngx_uint_t ngx_http_upstream_find_chash_point(
    ngx_http_upstream_chash_points_t *points, uint32_t hash);
static ngx_int_t ngx_http_upstream_init_chash_peer(ngx_http_request_t *r,
    ngx_http_upstream_srv_conf_t *us);
static ngx_int_t ngx_http_upstream_get_chash_peer(ngx_peer_connection_t *pc,
    void *data);

static void *ngx_http_upstream_hash_create_conf(ngx_conf_t *cf);
static char *ngx_http_upstream_hash(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


static ngx_command_t  ngx_http_upstream_hash_commands[] = {

    { ngx_string("hash"),
      NGX_HTTP_UPS_CONF|NGX_CONF_TAKE12,
      ngx_http_upstream_hash,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_upstream_hash_module_ctx = {
    NULL,                                  /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    ngx_http_upstream_hash_create_conf,    /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/*
璐