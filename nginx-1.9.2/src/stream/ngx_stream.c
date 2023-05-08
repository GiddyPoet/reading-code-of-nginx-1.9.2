
/*
 * Copyright (C) Roman Arutyunyan
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_stream.h>


static char *ngx_stream_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_stream_add_ports(ngx_conf_t *cf, ngx_array_t *ports,
    ngx_stream_listen_t *listen);
static char *ngx_stream_optimize_servers(ngx_conf_t *cf, ngx_array_t *ports);
static ngx_int_t ngx_stream_add_addrs(ngx_conf_t *cf, ngx_stream_port_t *stport,
    ngx_stream_conf_addr_t *addr);
#if (NGX_HAVE_INET6)
static ngx_int_t ngx_stream_add_addrs6(ngx_conf_t *cf,
    ngx_stream_port_t *stport, ngx_stream_conf_addr_t *addr);
#endif
static ngx_int_t ngx_stream_cmp_conf_addrs(const void *one, const void *two);


ngx_uint_t  ngx_stream_max_module;

//