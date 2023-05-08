
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


char    ngx_darwin_kern_ostype[16];
char    ngx_darwin_kern_osrelease[128];
int     ngx_darwin_hw_ncpu;
int     ngx_darwin_kern_ipc_somaxconn;
u_long  ngx_darwin_net_inet_tcp_sendspace;

ngx_uint_t  ngx_debug_malloc; //configure