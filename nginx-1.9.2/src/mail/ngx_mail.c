
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <ngx_mail.h>


static char *ngx_mail_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_mail_add_ports(ngx_conf_t *cf, ngx_array_t *ports,
    ngx_mail_listen_t *listen);
static char *ngx_mail_optimize_servers(ngx_conf_t *cf, ngx_array_t *ports);
static ngx_int_t ngx_mail_add_addrs(ngx_conf_t *cf, ngx_mail_port_t *mport,
    ngx_mail_conf_addr_t *addr);
#if (NGX_HAVE_INET6)
static ngx_int_t ngx_mail_add_addrs6(ngx_conf_t *cf, ngx_mail_port_t *mport,
    ngx_mail_conf_addr_t *addr);
#endif
static ngx_int_t ngx_mail_cmp_conf_addrs(const void *one, const void *two);


ngx_uint_t  ngx_mail_max_module;

//