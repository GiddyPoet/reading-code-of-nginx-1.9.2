
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


ssize_t
ngx_unix_send(ngx_connection_t *c, u_char *buf, size_t size)
{
    ssize_t       n;
    ngx_err_t     err;
    ngx_event_t  *wev;
    int ready = 0;

    wev = c->write;

#if (NGX_HAVE_KQUEUE)

    if ((ngx_event_flags & NGX_USE_KQUEUE_EVENT) && wev->pending_eof) {
        (void) ngx_connection_error(c, wev->kq_errno,
                               "kevent() reported about an closed connection");
        wev->error = 1;
        return NGX_ERROR;
    }

#endif

    for ( ;; ) {
        n = send(c->fd, buf, size, 0);

        ngx_log_debug3(NGX_LOG_DEBUG_EVENT, c->log, 0,
                       "send: fd:%d %d of %d", c->fd, n, size);

        if (n > 0) {
            //