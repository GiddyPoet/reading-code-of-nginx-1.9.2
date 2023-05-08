
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


/*
 * open file cache caches
 *    open file handles with stat() info;
 *    directories stat() info;
 *    files and directories errors: not found, access denied, etc.
 */


#define NGX_MIN_READ_AHEAD  (128 * 1024)


static void ngx_open_file_cache_cleanup(void *data);
#if (NGX_HAVE_OPENAT)
static ngx_fd_t ngx_openat_file_owner(ngx_fd_t at_fd, const u_char *name,
    ngx_int_t mode, ngx_int_t create, ngx_int_t access, ngx_log_t *log);
#if (NGX_HAVE_O_PATH)
static ngx_int_t ngx_file_o_path_info(ngx_fd_t fd, ngx_file_info_t *fi,
    ngx_log_t *log);
#endif
#endif
static ngx_fd_t ngx_open_file_wrapper(ngx_str_t *name,
    ngx_open_file_info_t *of, ngx_int_t mode, ngx_int_t create,
    ngx_int_t access, ngx_log_t *log);
static ngx_int_t ngx_file_info_wrapper(ngx_str_t *name,
    ngx_open_file_info_t *of, ngx_file_info_t *fi, ngx_log_t *log);
static ngx_int_t ngx_open_and_stat_file(ngx_str_t *name,
    ngx_open_file_info_t *of, ngx_log_t *log);
static void ngx_open_file_add_event(ngx_open_file_cache_t *cache,
    ngx_cached_open_file_t *file, ngx_open_file_info_t *of, ngx_log_t *log);
static void ngx_open_file_cleanup(void *data);
static void ngx_close_cached_file(ngx_open_file_cache_t *cache,
    ngx_cached_open_file_t *file, ngx_uint_t min_uses, ngx_log_t *log);
static void ngx_open_file_del_event(ngx_cached_open_file_t *file);
static void ngx_expire_old_cached_files(ngx_open_file_cache_t *cache,
    ngx_uint_t n, ngx_log_t *log);
static void ngx_open_file_cache_rbtree_insert_value(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
static ngx_cached_open_file_t *
    ngx_open_file_lookup(ngx_open_file_cache_t *cache, ngx_str_t *name,
    uint32_t hash);
static void ngx_open_file_cache_remove(ngx_event_t *ev);


ngx_open_file_cache_t *
ngx_open_file_cache_init(ngx_pool_t *pool, ngx_uint_t max, time_t inactive)
{
    ngx_pool_cleanup_t     *cln;
    ngx_open_file_cache_t  *cache;

    cache = ngx_palloc(pool, sizeof(ngx_open_file_cache_t));
    if (cache == NULL) {
        return NULL;
    }

    ngx_rbtree_init(&cache->rbtree, &cache->sentinel,
                    ngx_open_file_cache_rbtree_insert_value);

    ngx_queue_init(&cache->expire_queue);

    cache->current = 0;
    cache->max = max;
    cache->inactive = inactive;

    cln = ngx_pool_cleanup_add(pool, 0);
    if (cln == NULL) {
        return NULL;
    }

    cln->handler = ngx_open_file_cache_cleanup;
    cln->data = cache;

    return cache;
}

//渚