
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


ssize_t
ngx_readv_chain(ngx_connection_t *c, ngx_chain_t *chain, off_t limit)
{//杩