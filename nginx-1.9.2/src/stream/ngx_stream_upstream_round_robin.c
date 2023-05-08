
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>


#define ngx_stream_upstream_tries(p) ((p)->number                             \
                                      + ((p)->next ? (p)->next->number : 0))


static ngx_stream_upstream_rr_peer_t *ngx_stream_upstream_get_peer(
    ngx_stream_upstream_rr_peer_data_t *rrp);

#if (NGX_STREAM_SSL)

static ngx_int_t ngx_stream_upstream_set_round_robin_peer_session(
    ngx_peer_connection_t *pc, void *data);
static void ngx_stream_upstream_save_round_robin_peer_session(
    ngx_peer_connection_t *pc, void *data);

#endif

/*
涓昏