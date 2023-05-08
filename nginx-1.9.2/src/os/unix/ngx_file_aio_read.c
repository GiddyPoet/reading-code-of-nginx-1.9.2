
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


/*
 * FreeBSD file AIO features and quirks:
 *
 *    if an asked data are already in VM cache, then aio_error() returns 0,
 *    and the data are already copied in buffer;
 *
 *    aio_read() preread in VM cache as minimum 16K (probably BKVASIZE);
 *    the first AIO preload may be up to 128K;
 *
 *    aio_read/aio_error() may return EINPROGRESS for just written data;
 *
 *    kqueue EVFILT_AIO filter is level triggered only: an event repeats
 *    until aio_return() will be called;
 *
 *    aio_cancel() cannot cancel file AIO: it returns AIO_NOTCANCELED always.
 */


extern int  ngx_kqueue;


static ssize_t ngx_file_aio_result(ngx_file_t *file, ngx_event_aio_t *aio,
    ngx_event_t *ev);
static void ngx_file_aio_event_handler(ngx_event_t *ev);


ngx_int_t
ngx_file_aio_init(ngx_file_t *file, ngx_pool_t *pool)
{
    ngx_event_aio_t  *aio;

    aio = ngx_pcalloc(pool, sizeof(ngx_event_aio_t));
    if (aio == NULL) {
        return NGX_ERROR;
    }

    aio->file = file;
    aio->fd = file->fd;
    aio->event.data = aio;
    aio->event.ready = 1;
    aio->event.log = file->log;

    file->aio = aio;

    return NGX_OK;
}

/*
 