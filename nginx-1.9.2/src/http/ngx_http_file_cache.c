
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_md5.h>


static ngx_int_t ngx_http_file_cache_lock(ngx_http_request_t *r,
    ngx_http_cache_t *c);
static void ngx_http_file_cache_lock_wait_handler(ngx_event_t *ev);
static void ngx_http_file_cache_lock_wait(ngx_http_request_t *r,
    ngx_http_cache_t *c);
static ngx_int_t ngx_http_file_cache_read(ngx_http_request_t *r,
    ngx_http_cache_t *c);
static ssize_t ngx_http_file_cache_aio_read(ngx_http_request_t *r,
    ngx_http_cache_t *c);
#if (NGX_HAVE_FILE_AIO)
static void ngx_http_cache_aio_event_handler(ngx_event_t *ev);
#endif
#if (NGX_THREADS)
static ngx_int_t ngx_http_cache_thread_handler(ngx_thread_task_t *task,
    ngx_file_t *file);
static void ngx_http_cache_thread_event_handler(ngx_event_t *ev);
#endif
static ngx_int_t ngx_http_file_cache_exists(ngx_http_file_cache_t *cache,
    ngx_http_cache_t *c);
static ngx_int_t ngx_http_file_cache_name(ngx_http_request_t *r,
    ngx_path_t *path);
static ngx_http_file_cache_node_t *
    ngx_http_file_cache_lookup(ngx_http_file_cache_t *cache, u_char *key);
static void ngx_http_file_cache_rbtree_insert_value(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel);
static void ngx_http_file_cache_vary(ngx_http_request_t *r, u_char *vary,
    size_t len, u_char *hash);
static void ngx_http_file_cache_vary_header(ngx_http_request_t *r,
    ngx_md5_t *md5, ngx_str_t *name);
static ngx_int_t ngx_http_file_cache_reopen(ngx_http_request_t *r,
    ngx_http_cache_t *c);
static ngx_int_t ngx_http_file_cache_update_variant(ngx_http_request_t *r,
    ngx_http_cache_t *c);
static void ngx_http_file_cache_cleanup(void *data);
static time_t ngx_http_file_cache_forced_expire(ngx_http_file_cache_t *cache);
static time_t ngx_http_file_cache_expire(ngx_http_file_cache_t *cache);
static void ngx_http_file_cache_delete(ngx_http_file_cache_t *cache,
    ngx_queue_t *q, u_char *name);
static void ngx_http_file_cache_loader_sleep(ngx_http_file_cache_t *cache);
static ngx_int_t ngx_http_file_cache_noop(ngx_tree_ctx_t *ctx,
    ngx_str_t *path);
static ngx_int_t ngx_http_file_cache_manage_file(ngx_tree_ctx_t *ctx,
    ngx_str_t *path);
static ngx_int_t ngx_http_file_cache_manage_directory(ngx_tree_ctx_t *ctx,
    ngx_str_t *path);
static ngx_int_t ngx_http_file_cache_add_file(ngx_tree_ctx_t *ctx,
    ngx_str_t *path);
static ngx_int_t ngx_http_file_cache_add(ngx_http_file_cache_t *cache,
    ngx_http_cache_t *c);
static ngx_int_t ngx_http_file_cache_delete_file(ngx_tree_ctx_t *ctx,
    ngx_str_t *path);


ngx_str_t  ngx_http_cache_status[] = {
    ngx_string("MISS"),
    ngx_string("BYPASS"),
    ngx_string("EXPIRED"),
    ngx_string("STALE"),
    ngx_string("UPDATING"),
    ngx_string("REVALIDATED"),
    ngx_string("HIT")
};


static u_char  ngx_http_file_cache_key[] = { LF, 'K', 'E', 'Y', ':', ' ' };


static ngx_int_t
ngx_http_file_cache_init(ngx_shm_zone_t *shm_zone, void *data) //ngx_init_cycle涓