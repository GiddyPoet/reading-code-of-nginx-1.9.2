
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_uint_t                  hash_max_size;
    ngx_uint_t                  hash_bucket_size;
} ngx_http_map_conf_t;


typedef struct {
    ngx_hash_keys_arrays_t      keys;

    ngx_array_t                *values_hash;
    ngx_array_t                 var_values;
#if (NGX_PCRE)
    ngx_array_t                 regexes;
#endif

    ngx_http_variable_value_t  *default_value;
    ngx_conf_t                 *cf;
    ngx_uint_t                  hostnames;      /* unsigned  hostnames:1 */
} ngx_http_map_conf_ctx_t;


typedef struct {
    ngx_http_map_t              map;
    ngx_http_complex_value_t    value;
    ngx_http_variable_value_t  *default_value;
    ngx_uint_t                  hostnames;      /* unsigned  hostnames:1 */
} ngx_http_map_ctx_t;


static int ngx_libc_cdecl ngx_http_map_cmp_dns_wildcards(const void *one,
    const void *two);
static void *ngx_http_map_create_conf(ngx_conf_t *cf);
static char *ngx_http_map_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_map(ngx_conf_t *cf, ngx_command_t *dummy, void *conf);


static ngx_command_t  ngx_http_map_commands[] = {
/*
璇