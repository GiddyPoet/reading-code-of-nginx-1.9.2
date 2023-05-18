
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
ngx_http_file_cache_init(ngx_shm_zone_t *shm_zone, void *data) //ngx_init_cycle中执行
{
    ngx_http_file_cache_t  *ocache = data;

    size_t                  len;
    ngx_uint_t              n;
    ngx_http_file_cache_t  *cache;

    cache = shm_zone->data;

    if (ocache) {
        //如果ocache不是NULL，即有old cache，就比较缓存路径和level等，如果match的话就继承ocache的sh、shpool、bsize等  
        if (ngx_strcmp(cache->path->name.data, ocache->path->name.data) != 0) {
            ngx_log_error(NGX_LOG_EMERG, shm_zone->shm.log, 0,
                          "cache \"%V\" uses the \"%V\" cache path "
                          "while previously it used the \"%V\" cache path",
                          &shm_zone->shm.name, &cache->path->name,
                          &ocache->path->name);

            return NGX_ERROR;
        }

        for (n = 0; n < 3; n++) {
            if (cache->path->level[n] != ocache->path->level[n]) {
                ngx_log_error(NGX_LOG_EMERG, shm_zone->shm.log, 0,
                              "cache \"%V\" had previously different levels",
                              &shm_zone->shm.name);
                return NGX_ERROR;
            }
        }

        cache->sh = ocache->sh;

        cache->shpool = ocache->shpool;
        cache->bsize = ocache->bsize;

        cache->max_size /= cache->bsize;

        if (!cache->sh->cold || cache->sh->loading) {
            cache->path->loader = NULL;
        }

        return NGX_OK;
    }

    cache->shpool = (ngx_slab_pool_t *) shm_zone->shm.addr;

    if (shm_zone->shm.exists) {
        cache->sh = cache->shpool->data;
        cache->bsize = ngx_fs_bsize(cache->path->name.data);

        return NGX_OK;
    }

    cache->sh = ngx_slab_alloc(cache->shpool, sizeof(ngx_http_file_cache_sh_t));
    if (cache->sh == NULL) {
        return NGX_ERROR;
    }

    cache->shpool->data = cache->sh;

    ngx_rbtree_init(&cache->sh->rbtree, &cache->sh->sentinel,
                    ngx_http_file_cache_rbtree_insert_value); //红黑树初始化

    ngx_queue_init(&cache->sh->queue);//队列初始化

    cache->sh->cold = 1;
    cache->sh->loading = 0;
    cache->sh->size = 0;

    cache->bsize = ngx_fs_bsize(cache->path->name.data);

    cache->max_size /= cache->bsize;

    len = sizeof(" in cache keys zone \"\"") + shm_zone->shm.name.len;

    cache->shpool->log_ctx = ngx_slab_alloc(cache->shpool, len);
    if (cache->shpool->log_ctx == NULL) {
        return NGX_ERROR;
    }

    ngx_sprintf(cache->shpool->log_ctx, " in cache keys zone \"%V\"%Z",
                &shm_zone->shm.name);

    cache->shpool->log_nomem = 0;

    return NGX_OK;
}

//这里面的keys数组是为了存储proxy_cache_key $scheme$proxy_host$request_uri各个变量对应的value值
ngx_int_t
ngx_http_file_cache_new(ngx_http_request_t *r)
{
    ngx_http_cache_t  *c;

    c = ngx_pcalloc(r->pool, sizeof(ngx_http_cache_t));
    if (c == NULL) {
        return NGX_ERROR;
    }

    if (ngx_array_init(&c->keys, r->pool, 4, sizeof(ngx_str_t)) != NGX_OK) {
        return NGX_ERROR;
    }

    r->cache = c;
    c->file.log = r->connection->log;
    c->file.fd = NGX_INVALID_FILE;

    return NGX_OK;
}

/*ngx_http_upstream_init_request->ngx_http_upstream_cache 客户端获取缓存 后端应答回来数据后在ngx_http_upstream_send_response->ngx_http_file_cache_create
中创建临时文件，然后在ngx_event_pipe_write_chain_to_temp_file把读取的后端数据写入临时文件，最后在
ngx_http_upstream_send_response->ngx_http_upstream_process_request->ngx_http_file_cache_update中把临时文件内容rename(相当于mv)到proxy_cache_path指定
的cache目录下面
*/
    /*后端数据读取完毕，并且全部写入临时文件后才会执行rename过程，为什么需要临时文件的原因是:例如之前的缓存过期了，现在有个请求正在从后端
    获取数据写入临时文件，如果是直接写入缓存文件，则在获取后端数据过程中，如果在来一个客户端请求，如果允许proxy_cache_use_stale updating，则
    后面的请求可以直接获取之前老旧的过期缓存，从而可以避免冲突(前面的请求写文件，后面的请求获取文件内容) 
    */

//为后端应答的数据创建对应的缓存文件
ngx_int_t
ngx_http_file_cache_create(ngx_http_request_t *r)
{
    ngx_http_cache_t       *c;
    ngx_pool_cleanup_t     *cln;
    ngx_http_file_cache_t  *cache;

    c = r->cache;
    cache = c->file_cache;

    cln = ngx_pool_cleanup_add(r->pool, 0);
    if (cln == NULL) {
        return NGX_ERROR;
    }

    cln->handler = ngx_http_file_cache_cleanup;
    cln->data = c;

    if (ngx_http_file_cache_exists(cache, c) == NGX_ERROR) {
        return NGX_ERROR;
    }

    if (ngx_http_file_cache_name(r, cache->path) != NGX_OK) {
        return NGX_ERROR;
    }

    return NGX_OK;
}

/* 生成 md5sum(key) 和 crc32(key)并计算 `c->header_start` 值 */
void
ngx_http_file_cache_create_key(ngx_http_request_t *r)
{
    size_t             len;
    ngx_str_t         *key;
    ngx_uint_t         i;
    ngx_md5_t          md5;
    ngx_http_cache_t  *c;

    c = r->cache;

    len = 0;

    ngx_crc32_init(c->crc32);
    ngx_md5_init(&md5);

    key = c->keys.elts; 
    for (i = 0; i < c->keys.nelts; i++) { //计算 proxy_cache_key $scheme$proxy_host$request_uri对应的变量value值的md5和crc32值
        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http cache key: \"%V\"", &key[i]);

        len += key[i].len; //xxx_cache_key配置中的字符串长度和

        ngx_crc32_update(&c->crc32, key[i].data, key[i].len); //xxx_cache_key配置中的字符串进行crc32校验值   ・
        ngx_md5_update(&md5, key[i].data, key[i].len); //xxx_cache_key配置中的字符串进行MD5运算 ・
    }

    ////[ngx_http_file_cache_header_t]["\nKEY: "][orig_key]["\n"][header][body] 封包过程见ngx_http_file_cache_set_header
    c->header_start = sizeof(ngx_http_file_cache_header_t)
                      + sizeof(ngx_http_file_cache_key) + len + 1; //+1是因为key后面有有个'\N'

    ngx_crc32_final(c->crc32);//获取所有key字符串的校验结果
    ngx_md5_final(c->key, &md5);//获取xxx_cache_key配置字符串进行MD5运算的值

    ngx_memcpy(c->main, c->key, NGX_HTTP_CACHE_KEY_LEN);
}

/*
 ngx_http_file_cache_open->ngx_http_file_cache_read->ngx_http_file_cache_aio_read这个流程获取文件中前面的头部信息相关内容，并获取整个
 文件stat信息，例如文件大小等。
 头部部分在ngx_http_cache_send->ngx_http_send_header发送，
 缓存文件后面的包体部分在ngx_http_cache_send后半部代码中触发在filter模块中发送
 */

//调用 ngx_http_file_cache_open 函数查找是否有对应的有效缓存数据 ngx_http_file_cache_open 函数负责缓存文件定位、缓存文件打开和校验等操作
ngx_int_t
ngx_http_file_cache_open(ngx_http_request_t *r)
{//读取缓存文件前面的头部信息数据到r->cache->buf，同时获取文件的相关属性到r->cache的相关字段
    ngx_int_t                  rc, rv;
    ngx_uint_t                 test;
    ngx_http_cache_t          *c;
    ngx_pool_cleanup_t        *cln;
    ngx_open_file_info_t       of;
    ngx_http_file_cache_t     *cache;
    ngx_http_core_loc_conf_t  *clcf;

    c = r->cache;

    /* ngx_http_file_cache_open如果返回NGX_AGAIN，则会在函数外执行下面的代码，也就是等待前面的请求后端返回后，再次触发后面的请求执行ngx_http_upstream_init_request过程
        这时候前面从后端获取的数据肯定已经得到缓存
        r->write_event_handler = ngx_http_upstream_init_request;  //这么触发该write handler呢?因为前面的请求获取到后端数据后，在触发epoll_in的同时
        也会触发epoll_out，从而会执行该函数
        return;  
     */
    if (c->waiting) {  //缓存内容己过期，当前请求正等待其它请求更新此缓存节点。 
        return NGX_AGAIN;
    }

    if (c->reading) {
        return ngx_http_file_cache_read(r, c);
    }

    //通过proxy_cache xxx或者fastcgi_cache xxx来设置的共享内存等信息
    cache = c->file_cache;

    /*
     第一次根据请求信息生成的 key 查找对应缓存节点时，先注册一下请求内存池级别的清理函数
     */
    if (c->node == NULL) { //添加缓存对应的cleanup
        cln = ngx_pool_cleanup_add(r->pool, 0);
        if (cln == NULL) {
            return NGX_ERROR;
        }

        cln->handler = ngx_http_file_cache_cleanup;
        cln->data = c;
    }

    rc = ngx_http_file_cache_exists(cache, c);

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http file cache exists, rc: %i if exist:%d", rc, c->exists);

    if (rc == NGX_ERROR) {
        return rc;
    }

    
    if (rc == NGX_AGAIN) { //例如配置Proxy_cache_min_uses 5，则需要客户端请求5才才能从缓存中取，如果现在只有4次，则都需要从后端获取数据
        return NGX_HTTP_CACHE_SCARCE; //函数外层ngx_http_upstream_cache会把 u->cacheable = 0;
    }

    if (rc == NGX_OK) { 
        
        if (c->error) {
            return c->error;
        }

        c->temp_file = 1;
        test = c->exists ? 1 : 0; //是否有达到Proxy_cache_min_uses 5配置的开始缓存文件的请求次数，达到为1，没达到为0
        rv = NGX_DECLINED;//如果返回这个，会把cached置0，返回出去后只有从后端从新获取数据

    } else { /* rc == NGX_DECLINED */ //表示在ngx_http_file_cache_exists中没找到该key对应的node节点，因此按照key重新创建了一个node节点(第一次请求该uri)
        //ngx_http_file_cache_exists没找到对应的ngx_http_file_cache_node_t节点，或者该节点对应缓存过期，返回NGX_DECLINED (第一次请求该uri)
        test = cache->sh->cold ? 1 : 0;//test=0,表示进程起来后缓存文件已经加载完毕，为1表示进程刚起来还没有加载缓存文件，默认值1

        if (c->min_uses > 1) {

            if (!test) { //
                return NGX_HTTP_CACHE_SCARCE;
            }

            rv = NGX_HTTP_CACHE_SCARCE;

        } else {
            c->temp_file = 1;
            rv = NGX_DECLINED; //如果返回这个，会把cached置0，返回出去后只有从后端从新获取数据
        }
    }

    if (ngx_http_file_cache_name(r, cache->path) != NGX_OK) {
        return NGX_ERROR;
    }

    if (!test) {
        //还没达到Proxy_cache_min_uses 5配置的开始缓存文件的请求次数
        //nginx进程起来后，loader进程已经把缓存文件加载完毕，但是在红黑树中没有找到对应的文件node节点(第一次请求该uri)
        goto done;
    }

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    ngx_memzero(&of, sizeof(ngx_open_file_info_t));

    of.uniq = c->uniq;
    of.valid = clcf->open_file_cache_valid;  
    of.min_uses = clcf->open_file_cache_min_uses;
    of.events = clcf->open_file_cache_events;
    of.directio = NGX_OPEN_FILE_DIRECTIO_OFF;
    of.read_ahead = clcf->read_ahead;  /* read_ahead配置，默认0 */

    if (ngx_open_cached_file(clcf->open_file_cache, &c->file.name, &of, r->pool)
        != NGX_OK)
    { //一般没有该文件的时候会走到这里面
        ngx_log_debugall(r->connection->log, 0, "ngx_open_cached_file return:NGX_ERROR");
        switch (of.err) {

        case 0:
            return NGX_ERROR;

        case NGX_ENOENT:
        case NGX_ENOTDIR:
            goto done;

        default:
            ngx_log_error(NGX_LOG_CRIT, r->connection->log, of.err,
                          ngx_open_file_n " \"%s\" failed", c->file.name.data);
            return NGX_ERROR;
        }
    }

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http file cache fd: %d", of.fd);

    c->file.fd = of.fd;
    c->file.log = r->connection->log;
    c->uniq = of.uniq;
    c->length = of.size;
    c->fs_size = (of.fs_size + cache->bsize - 1) / cache->bsize; //bsize对齐

    /*
    root@root:/var/yyz# cat cache_xxx/f/27/46492fbf0d9d35d3753c66851e81627f   封包过程见ngx_http_file_cache_set_header
     3hwhdBw
     KEY: /test2.php
     
     X-Powered-By: PHP/5.2.13
     Content-type: text/html
    //body_start就是上面这一段内存内容长度
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
     <Html> 
     <title>file update</title>
     <body> 
     <form method="post" action="" enctype="multipart/form-data">
     <input type="file" name="file" /> 
     <input type="submit" value="submit" /> 
     </form> 
     </body> 
     </html>
     */ 
    //创建存放缓存文件中前面[ngx_http_file_cache_header_t]["\nKEY: "][fastcgi_cache_key中的KEY]["\n"][header]部分的内容长度空间,也就是
    //@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@前面的内容
    c->buf = ngx_create_temp_buf(r->pool, c->body_start);
    if (c->buf == NULL) {
        return NGX_ERROR;
    }

//注意这里读取缓存文件中的头部部分的时候，只有aio读取或者缓存方式读取，和sendfile没有关系，因为头部读出来需要重新组装发往客户端的头部行信息，必须从文件读到内存中
    //如果返回这个NGX_DECLINED，会把cached置0，返回出去后只有从后端从新获取数据
    return ngx_http_file_cache_read(r, c);  

done:
    //还没达到Proxy_cache_min_uses 5配置的开始缓存文件的请求次数
    //nginx进程起来后，loader进程已经把缓存文件加载完毕，但是在红黑树中没有找到对应的文件node节点(第一次请求该uri)，同时配置的Proxy_cache_min_uses=1
    if (rv == NGX_DECLINED) {
    //说明没有uri对应的缓存文件，通过ngx_http_cache_t->key[](实际上就是由uri进行MD5计算出的值放到key[]中的)在红黑树中找不到该节点
        return ngx_http_file_cache_lock(r, c);
    }

    return rv;
}


static ngx_int_t
ngx_http_file_cache_lock(ngx_http_request_t *r, ngx_http_cache_t *c)
{
    ngx_msec_t                 now, timer;
    ngx_http_file_cache_t     *cache;

    if (!c->lock) {//默认就是0
        return NGX_DECLINED;
    }

    now = ngx_current_msec;

    cache = c->file_cache;

    ngx_shmtx_lock(&cache->shpool->mutex);

    timer = c->node->lock_time - now;

    if (!c->node->updating || (ngx_msec_int_t) timer <= 0) {
        c->node->updating = 1;
        c->node->lock_time = now + c->lock_age;
        c->updating = 1;
        c->lock_time = c->node->lock_time;
    }

    ngx_shmtx_unlock(&cache->shpool->mutex);

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http file cache lock u:%d wt:%M",
                   c->updating, c->wait_time);

    if (c->updating) {
        return NGX_DECLINED;
    }

    if (c->lock_timeout == 0) {
        return NGX_HTTP_CACHE_SCARCE;
    }

    c->waiting = 1;

    if (c->wait_time == 0) {
        c->wait_time = now + c->lock_timeout;

        c->wait_event.handler = ngx_http_file_cache_lock_wait_handler;
        c->wait_event.data = r;
        c->wait_event.log = r->connection->log;
    }

    timer = c->wait_time - now;

    ngx_add_timer(&c->wait_event, (timer > 500) ? 500 : timer, NGX_FUNC_LINE);

    r->main->blocked++;

    return NGX_AGAIN;
}


static void
ngx_http_file_cache_lock_wait_handler(ngx_event_t *ev)
{
    ngx_connection_t    *c;
    ngx_http_request_t  *r;

    r = ev->data;
    c = r->connection;

    ngx_http_set_log_request(c->log, r);

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http file cache wait: \"%V?%V\"", &r->uri, &r->args);

    ngx_http_file_cache_lock_wait(r, r->cache);

    ngx_http_run_posted_requests(c);
}


static void
ngx_http_file_cache_lock_wait(ngx_http_request_t *r, ngx_http_cache_t *c)
{
    ngx_uint_t              wait;
    ngx_msec_t              now, timer;
    ngx_http_file_cache_t  *cache;

    now = ngx_current_msec;

    timer = c->wait_time - now;

    if ((ngx_msec_int_t) timer <= 0) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                      "cache lock timeout");
        c->lock_timeout = 0;
        goto wakeup;
    }

    cache = c->file_cache;
    wait = 0;

    ngx_shmtx_lock(&cache->shpool->mutex);

    timer = c->node->lock_time - now;

    if (c->node->updating && (ngx_msec_int_t) timer > 0) {
        wait = 1;
    }

    ngx_shmtx_unlock(&cache->shpool->mutex);

    if (wait) {
        ngx_add_timer(&c->wait_event, (timer > 500) ? 500 : timer, NGX_FUNC_LINE);
        return;
    }

wakeup:

    c->waiting = 0;
    r->main->blocked--;
    r->write_event_handler(r);
}


/*
    root@root:/var/yyz# cat cache_xxx/f/27/46492fbf0d9d35d3753c66851e81627f   封包过程见ngx_http_file_cache_set_header
     3hwhdBw
     KEY: /test2.php
     
     X-Powered-By: PHP/5.2.13
     Content-type: text/html
    //body_start就是上面这一段内存内容长度
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
     <Html> 
     <title>file update</title>
     <body> 
     <form method="post" action="" enctype="multipart/form-data">
     <input type="file" name="file" /> 
     <input type="submit" value="submit" /> 
     </form> 
     </body> 
     </html>
*/ 

/*
     ngx_http_file_cache_open->ngx_http_file_cache_read->ngx_http_file_cache_aio_read这个流程获取文件中前面的头部信息相关内容，并获取整个
     文件stat信息，例如文件大小等。
     头部部分在ngx_http_cache_send->ngx_http_send_header发送，
     缓存文件后面的包体部分在ngx_http_cache_send后半部代码中触发在filter模块中发送
 */

//读取缓存文件中前面[ngx_http_file_cache_header_t]["\nKEY: "][fastcgi_cache_key中的KEY]["\n"][header]部分的内容长度空间,也就是
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@前面的内容
static ngx_int_t
ngx_http_file_cache_read(ngx_http_request_t *r, ngx_http_cache_t *c)
{
//注意这里读取缓存文件中的头部部分的时候，只有aio读取或者缓存方式读取，和sendfile没有关系，因为头部读出来需要重新组装发往客户端的头部行信息，必须从文件读到内存中
    time_t                         now;
    ssize_t                        n;
    ngx_int_t                      rc;
    ngx_http_file_cache_t         *cache;
    ngx_http_file_cache_header_t  *h;

    /*
     ngx_http_file_cache_open->ngx_http_file_cache_read->ngx_http_file_cache_aio_read这个流程获取文件中前面的头部信息相关内容，并获取整个
     文件stat信息，例如文件大小等。
     头部部分在ngx_http_cache_send->ngx_http_send_header发送，
     缓存文件后面的包体部分在ngx_http_cache_send后半部代码中触发在filter模块中发送
     */
    n = ngx_http_file_cache_aio_read(r, c);//读取缓存文件中的前面头部相关信息部分数据

    if (n < 0) {
        return n;
    }

    //写缓冲区封装过程参考:ngx_http_upstream_process_header
    //缓存文件中前面部分格式:[ngx_http_file_cache_header_t]["\nKEY: "][orig_key]["\n"][header]

    //头部部分读取错误
    if ((size_t) n < c->header_start) {
        ngx_log_error(NGX_LOG_CRIT, r->connection->log, 0,
                      "cache file \"%s\" is too small", c->file.name.data);
        return NGX_DECLINED; //如果返回这个NGX_DECLINED，会把cached置0，返回出去后只有从后端从新获取数据
    }

    //[ngx_http_file_cache_header_t]["\nKEY: "][orig_key]["\n"][header]
    h = (ngx_http_file_cache_header_t *) c->buf->pos;

    if (h->version != NGX_HTTP_CACHE_VERSION) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                      "cache file \"%s\" version mismatch", c->file.name.data);
        return NGX_DECLINED; //如果返回这个NGX_DECLINED，会把cached置0，返回出去后只有从后端从新获取数据
    }

    if (h->crc32 != c->crc32) {
        ngx_log_error(NGX_LOG_CRIT, r->connection->log, 0,
                      "cache file \"%s\" has md5 collision", c->file.name.data);
        return NGX_DECLINED; //如果返回这个NGX_DECLINED，会把cached置0，返回出去后只有从后端从新获取数据
    }

    if ((size_t) h->body_start > c->body_start) {
        ngx_log_error(NGX_LOG_CRIT, r->connection->log, 0,
                      "cache file \"%s\" has too long header",
                      c->file.name.data);
        return NGX_DECLINED; //如果返回这个NGX_DECLINED，会把cached置0，返回出去后只有从后端从新获取数据
    }

    if (h->vary_len > NGX_HTTP_CACHE_VARY_LEN) {
        ngx_log_error(NGX_LOG_CRIT, r->connection->log, 0,
                      "cache file \"%s\" has incorrect vary length",
                      c->file.name.data);
        return NGX_DECLINED; //如果返回这个NGX_DECLINED，会把cached置0，返回出去后只有从后端从新获取数据
    }

    if (h->vary_len) {
        ngx_http_file_cache_vary(r, h->vary, h->vary_len, c->variant);

        if (ngx_memcmp(c->variant, h->variant, NGX_HTTP_CACHE_KEY_LEN) != 0) {
            ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                           "http file cache vary mismatch");
            return ngx_http_file_cache_reopen(r, c);
        }
    }

    c->buf->last += n; //移动last指针

    c->valid_sec = h->valid_sec;
    c->last_modified = h->last_modified;
    c->date = h->date;
    c->valid_msec = h->valid_msec;
    c->header_start = h->header_start;
    c->body_start = h->body_start;
    c->etag.len = h->etag_len;
    c->etag.data = h->etag;

    r->cached = 1;

    cache = c->file_cache;

    if (cache->sh->cold) {

        ngx_shmtx_lock(&cache->shpool->mutex);

        if (!c->node->exists) {
            c->node->uses = 1;
            c->node->body_start = c->body_start;
            c->node->exists = 1;
            c->node->uniq = c->uniq;
            c->node->fs_size = c->fs_size;

            cache->sh->size += c->fs_size;
        }

        ngx_shmtx_unlock(&cache->shpool->mutex);
    }

    now = ngx_time();

    if (c->valid_sec < now) { //判断该缓存是否过期

        ngx_shmtx_lock(&cache->shpool->mutex);

        if (c->node->updating) {
            rc = NGX_HTTP_CACHE_UPDATING;

        } else { //表示自己是第一个发现该缓存过期的客户端请求，因此自己需要从后端从新获取
            c->node->updating = 1;//客户端请求到nginx后，发现缓存过期，则会重新从后端获取数据，updating置1，见ngx_http_file_cache_read
            c->updating = 1;
            c->lock_time = c->node->lock_time;
            rc = NGX_HTTP_CACHE_STALE;
        }

        ngx_shmtx_unlock(&cache->shpool->mutex);

        ngx_log_debug3(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http file cache expired: %i %T %T",
                       rc, c->valid_sec, now);

        return rc;
    }

    return NGX_OK;
}

/*
    root@root:/var/yyz# cat cache_xxx/f/27/46492fbf0d9d35d3753c66851e81627f   封包过程见ngx_http_file_cache_set_header
     3hwhdBw
     KEY: /test2.php
     
     X-Powered-By: PHP/5.2.13
     Content-type: text/html
    //body_start就是上面这一段内存内容长度
    @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
    
     <Html> 
     <title>file update</title>
     <body> 
     <form method="post" action="" enctype="multipart/form-data">
     <input type="file" name="file" /> 
     <input type="submit" value="submit" /> 
     </form> 
     </body> 
     </html>
*/ 
//读取缓存文件中前面[ngx_http_file_cache_header_t]["\nKEY: "][fastcgi_cache_key中的KEY]["\n"][header]部分的内容长度空间,也就是
//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@前面的内容

/*
发送缓存文件中内容到客户端过程:
 ngx_http_file_cache_open->ngx_http_file_cache_read->ngx_http_file_cache_aio_read这个流程获取文件中前面的头部信息相关内容，并获取整个
 文件stat信息，例如文件大小等。
 头部部分在ngx_http_cache_send->ngx_http_send_header发送，
 缓存文件后面的包体部分在ngx_http_cache_send后半部代码中触发在filter模块中发送

 接收后端数据并转发到客户端触发数据发送过程:
 ngx_event_pipe_write_to_downstream中的
 if (p->upstream_eof || p->upstream_error || p->upstream_done) {
    遍历p->in 或者遍历p->out，然后执行输出
    p->output_filter(p->output_ctx, p->out);
 }
 */

/* 读取缓存文件中前面的[ngx_http_file_cache_header_t]["\nKEY: "][fastcgi_cache_key中的KEY]["\n"][header] */
//注意置读取前面的头部信息，紧跟后面的后端应答回来的缓存包体是没有读取的
static ssize_t
ngx_http_file_cache_aio_read(ngx_http_request_t *r, ngx_http_cache_t *c)
{
//注意这里读取缓存文件中的头部部分的时候，只有aio读取或者缓存方式读取，和sendfile没有关系，因为头部读出来需要重新组装发往客户端的头部行信息，必须从文件读到内存中
#if (NGX_HAVE_FILE_AIO || NGX_THREADS)
    ssize_t                    n;
    ngx_http_core_loc_conf_t  *clcf;

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
#endif

#if (NGX_HAVE_FILE_AIO)

    if (clcf->aio == NGX_HTTP_AIO_ON && ngx_file_aio) { //aio on这这里  aio on | off | threads[=pool]; 
        n = ngx_file_aio_read(&c->file, c->buf->pos, c->body_start, 0, r->pool);

        if (n != NGX_AGAIN) {
            c->reading = 0;
            return n;
        }

        c->reading = 1;

        c->file.aio->data = r;
        c->file.aio->handler = ngx_http_cache_aio_event_handler;

        r->main->blocked++;
        r->aio = 1;

        return NGX_AGAIN;
    }

#endif

#if (NGX_THREADS)

    if (clcf->aio == NGX_HTTP_AIO_THREADS) { //aio thread配置的时候走这里  aio on | off | threads[=pool]; 
        c->file.thread_handler = ngx_http_cache_thread_handler;
        c->file.thread_ctx = r;

        n = ngx_thread_read(&c->thread_task, &c->file, c->buf->pos,
                            c->body_start, 0, r->pool); //只是读取缓冲区文件中前面的头部信息部分

        c->reading = (n == NGX_AGAIN);

        return n;
    }

#endif

    /*
     ngx_http_file_cache_open->ngx_http_file_cache_read->ngx_http_file_cache_aio_read这个流程获取文件中前面的头部信息相关内容，并获取整个
     文件stat信息，例如文件大小等。
     头部部分在ngx_http_cache_send->ngx_http_send_header发送，
     缓存文件后面的包体部分在ngx_http_cache_send后半部代码中触发在filter模块中发送
     */


    /* 读取缓存文件中前面的[ngx_http_file_cache_header_t]["\nKEY: "][fastcgi_cache_key中的KEY]["\n"][header] */
    return  ngx_read_file(&c->file, c->buf->pos, c->body_start, 0);
    /*c->buf->last += ret;
    ngx_log_debugall(r->connection->log, 0, "YANG TEST ......@@@@@@@@@......%d, ret:%uz", 
        (int)(c->buf->last - c->buf->pos), ret);
    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "%*s", (size_t) (c->buf->last - c->buf->pos), c->buf->pos);

    c->buf->last -= ret;
    return ret;*/
}


#if (NGX_HAVE_FILE_AIO)

static void
ngx_http_cache_aio_event_handler(ngx_event_t *ev)
{
    ngx_event_aio_t     *aio;
    ngx_connection_t    *c;
    ngx_http_request_t  *r;

    aio = ev->data;
    r = aio->data;
    c = r->connection;

    ngx_http_set_log_request(c->log, r);

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http file cache aio on: \"%V?%V\"", &r->uri, &r->args);

    r->main->blocked--;
    r->aio = 0;

    r->write_event_handler(r);

    ngx_http_run_posted_requests(c);
}

#endif


#if (NGX_THREADS)

////aio thread配置的时候走这里  aio on | off | threads[=pool]; 
//这里添加task->event信息到task中，当task->handler指向完后，通过nginx_notify可以继续通过epoll_wait返回执行task->event
static ngx_int_t
ngx_http_cache_thread_handler(ngx_thread_task_t *task, ngx_file_t *file)
{ //由ngx_thread_read触发执行
    ngx_str_t                  name;
    ngx_thread_pool_t         *tp;
    ngx_http_request_t        *r;
    ngx_http_core_loc_conf_t  *clcf;

    r = file->thread_ctx;

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
    tp = clcf->thread_pool;

    if (tp == NULL) {
        if (ngx_http_complex_value(r, clcf->thread_pool_value, &name)
            != NGX_OK)
        {
            return NGX_ERROR;
        }

        tp = ngx_thread_pool_get((ngx_cycle_t *) ngx_cycle, &name);

        if (tp == NULL) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "thread pool \"%V\" not found", &name);
            return NGX_ERROR;
        }
    }

    task->event.data = r;
    task->event.handler = ngx_http_cache_thread_event_handler;

    if (ngx_thread_task_post(tp, task) != NGX_OK) { //该任务的handler函数式task->handler = ngx_thread_read_handler;
        return NGX_ERROR;
    }

    r->main->blocked++;
    r->aio = 1;

    return NGX_OK;
}

static void
ngx_http_cache_thread_event_handler(ngx_event_t *ev)
{//在ngx_notify(ngx_thread_pool_handler); 中的ngx_thread_pool_handler执行该函数，表示线程读文件完成，通过ngx_notify epoll方式触发
    ngx_connection_t    *c;
    ngx_http_request_t  *r;

    r = ev->data;
    c = r->connection;

    ngx_http_set_log_request(c->log, r);

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http file cache aio thread: \"%V?%V\"", &r->uri, &r->args);

    r->main->blocked--;
    r->aio = 0;

    r->write_event_handler(r);

    ngx_http_run_posted_requests(c);  
}

#endif

/*
  同一个客户端请求r只拥有一个r->ngx_http_cache_t和r->ngx_http_cache_t->ngx_http_file_cache_t结构，同一个客户端可能会请求后端的多个uri，
  则在向后端发起请求前，在ngx_http_file_cache_open->ngx_http_file_cache_exists中会按照proxy_cache_key $scheme$proxy_host$request_uri计算出来的
  MD5来创建对应的红黑树节点，然后添加到ngx_http_file_cache_t->sh->rbtree红黑树中。所以不同的客户端uri会有不同的node节点存在于红黑树中
*/

//http://www.tuicool.com/articles/QnMNr23
//查找红黑树cache->sh->rbtree中的节点ngx_http_file_cache_node_t，没找到则创建响应的ngx_http_file_cache_node_t节点添加到红黑树中
static ngx_int_t
ngx_http_file_cache_exists(ngx_http_file_cache_t *cache, ngx_http_cache_t *c)
{
    ngx_int_t                    rc;
    ngx_http_file_cache_node_t  *fcn;

    ngx_shmtx_lock(&cache->shpool->mutex);

    fcn = c->node;//后面没找到则会创建node节点
   
    if (fcn == NULL) {
        fcn = ngx_http_file_cache_lookup(cache, c->key); //以 c->key 为查找条件从缓存中查找缓存节点： 
    }

    if (fcn) { //cache中存在该key
        ngx_queue_remove(&fcn->queue);

        //该客户端在新建连接后，如果之前有缓存该文件，则c->node为NULL，表示这个连接请求第一次走到这里，有一个客户端在获取数据，如果在
        //连接范围内(还没有断开连接)多次获取该缓存文件，则也只会加1，表示当前有多少个客户端连接在获取该缓存
        if (c->node == NULL) { //如果该请求第一次使用此缓存节点，则增加相关引用和使用次数
            fcn->uses++;
            fcn->count++;
        }

        if (fcn->error) {

            if (fcn->valid_sec < ngx_time()) {
                goto renew; //缓存已过期
            }

            rc = NGX_OK;

            goto done;
        }

        if (fcn->exists || fcn->uses >= c->min_uses) { //该请求的缓存已经存在，并且对该缓存的请求次数达到了最低要求次数min_uses
            //表示该缓存文件是否存在，Proxy_cache_min_uses 3，则第3次后开始获取后端数据，获取完毕后在ngx_http_file_cache_update中置1，但是只有在地4次请求的时候才会在ngx_http_file_cache_exists赋值为1
            c->exists = fcn->exists;
            if (fcn->body_start) {
                c->body_start = fcn->body_start;
            }

            rc = NGX_OK;

            goto done;
        }

        //例如配置Proxy_cache_min_uses 5，则需要客户端请求5才才能从缓存中取，如果现在只有4次，则都需要从后端获取数据
        rc = NGX_AGAIN;

        goto done;
    }

    //没找到，则在下面创建node节点，添加到ngx_http_file_cache_t->sh->rbtree红黑树中
    fcn = ngx_slab_calloc_locked(cache->shpool,
                                 sizeof(ngx_http_file_cache_node_t));
    if (fcn == NULL) {
        ngx_shmtx_unlock(&cache->shpool->mutex);

        (void) ngx_http_file_cache_forced_expire(cache);

        ngx_shmtx_lock(&cache->shpool->mutex);

        fcn = ngx_slab_calloc_locked(cache->shpool,
                                     sizeof(ngx_http_file_cache_node_t));
        if (fcn == NULL) {
            ngx_log_error(NGX_LOG_ALERT, ngx_cycle->log, 0,
                          "could not allocate node%s", cache->shpool->log_ctx);
            rc = NGX_ERROR;
            goto failed;
        }
    }

    ngx_memcpy((u_char *) &fcn->node.key, c->key, sizeof(ngx_rbtree_key_t));

    ngx_memcpy(fcn->key, &c->key[sizeof(ngx_rbtree_key_t)],
               NGX_HTTP_CACHE_KEY_LEN - sizeof(ngx_rbtree_key_t));

    ngx_rbtree_insert(&cache->sh->rbtree, &fcn->node); //把该节点添加到红黑树中

    fcn->uses = 1;
    fcn->count = 1;

renew:

    rc = NGX_DECLINED; //uri第一次请求的时候创建node节点，同时返回NGX_DECLINED。或者缓存过期需要把该节点相关信息恢复为默认值

    fcn->valid_msec = 0;
    fcn->error = 0;
    fcn->exists = 0;
    fcn->valid_sec = 0;
    fcn->uniq = 0;
    fcn->body_start = 0;
    fcn->fs_size = 0;

done:

    fcn->expire = ngx_time() + cache->inactive;

    ngx_queue_insert_head(&cache->sh->queue, &fcn->queue); //新创建的node节点添加到cache->sh->queue头部

    c->uniq = fcn->uniq;//文件的uniq  赋值见ngx_http_file_cache_update
    c->error = fcn->error;
    c->node = fcn; //把新创建的fcn赋值给c->node

failed:

    ngx_shmtx_unlock(&cache->shpool->mutex);

    return rc;
}

//为后端应答回来的数据创建缓存文件用该函数获取缓存文件名，客户端请求过来后，也是采用该函数获取缓存文件名，只要
//proxy_cache_key $scheme$proxy_host$request_uri配置中的变量对应的值一样，则获取到的文件名肯定是一样的，即使是不同的客户端r，参考ngx_http_file_cache_name
//因为不同客户端的proxy_cache_key配置的对应变量value一样，则他们计算出来的ngx_http_cache_s->key[]也会一样，他们的在红黑树和queue队列中的
//node节点也会是同一个，参考ngx_http_file_cache_lookup
static ngx_int_t
ngx_http_file_cache_name(ngx_http_request_t *r, ngx_path_t *path) //获取缓存名
{
    u_char            *p;
    ngx_http_cache_t  *c;

    c = r->cache;

    if (c->file.name.len) {
        return NGX_OK;
    }

    c->file.name.len = path->name.len + 1 + path->len
                       + 2 * NGX_HTTP_CACHE_KEY_LEN;

    c->file.name.data = ngx_pnalloc(r->pool, c->file.name.len + 1);
    if (c->file.name.data == NULL) {
        return NGX_ERROR;
    }

    ngx_memcpy(c->file.name.data, path->name.data, path->name.len); //XXX_cache_path 指定的路径

    //跳过level，在后面的ngx_create_hashed_filename添加到内存中
    p = c->file.name.data + path->name.len + 1 + path->len; //   /cache/0/8d/
    p = ngx_hex_dump(p, c->key, NGX_HTTP_CACHE_KEY_LEN); //16进制key转换为字符串拷贝到cache缓存目录file中
    *p = '\0';

    //通过从配置文件中的path，得到完整路径，ngx_create_hashed_filename是填充level路径
    ngx_create_hashed_filename(path, c->file.name.data, c->file.name.len);

    //cache file: "/var/yyz/cache_xxx/c/c1/13cc494353644acaed96a080cac13c1c"
    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "cache file: \"%s\"", c->file.name.data);

    return NGX_OK;
}

/*
为后端应答回来的数据创建缓存文件用该函数获取缓存文件名，客户端请求过来后，也是采用该函数获取缓存文件名，只要
proxy_cache_key $scheme$proxy_host$request_uri配置中的变量对应的值一样，则获取到的文件名肯定是一样的，即使是不同的客户端r，参考ngx_http_file_cache_name
因为不同客户端的proxy_cache_key配置的对应变量value一样，则他们计算出来的ngx_http_cache_s->key[]也会一样，他们的在红黑树和queue队列中的
node节点也会是同一个，参考ngx_http_file_cache_lookup  
*/

//参考nginx proxy cache分析 http://blog.csdn.net/xiaolang85/article/details/38260041 图解
static ngx_http_file_cache_node_t *
ngx_http_file_cache_lookup(ngx_http_file_cache_t *cache, u_char *key)
{
    ngx_int_t                    rc;
    ngx_rbtree_key_t             node_key;
    ngx_rbtree_node_t           *node, *sentinel;
    ngx_http_file_cache_node_t  *fcn;

    ngx_memcpy((u_char *) &node_key, key, sizeof(ngx_rbtree_key_t)); //拷贝key的前面4个字符

    node = cache->sh->rbtree.root; //红黑树跟节点
    sentinel = cache->sh->rbtree.sentinel; //哨兵节点

    while (node != sentinel) {

        if (node_key < node->key) {
            node = node->left;
            continue;
        }

        if (node_key > node->key) {
            node = node->right;
            continue;
        }

        /* node_key == node->key */

        fcn = (ngx_http_file_cache_node_t *) node;

        rc = ngx_memcmp(&key[sizeof(ngx_rbtree_key_t)], fcn->key,
                        NGX_HTTP_CACHE_KEY_LEN - sizeof(ngx_rbtree_key_t)); 
                        //比较内容是从key的NGX_HTTP_CACHE_KEY_LEN - sizeof(ngx_rbtree_key_t)开始比较

        if (rc == 0) {
            return fcn;
        }

        node = (rc < 0) ? node->left : node->right;
    }

    /* not found */

    return NULL;
}


static void
ngx_http_file_cache_rbtree_insert_value(ngx_rbtree_node_t *temp,
    ngx_rbtree_node_t *node, ngx_rbtree_node_t *sentinel)
{
    ngx_rbtree_node_t           **p;
    ngx_http_file_cache_node_t   *cn, *cnt;

    for ( ;; ) {

        if (node->key < temp->key) {

            p = &temp->left;

        } else if (node->key > temp->key) {

            p = &temp->right;

        } else { /* node->key == temp->key */

            cn = (ngx_http_file_cache_node_t *) node;
            cnt = (ngx_http_file_cache_node_t *) temp;

            p = (ngx_memcmp(cn->key, cnt->key,
                            NGX_HTTP_CACHE_KEY_LEN - sizeof(ngx_rbtree_key_t))
                 < 0)
                    ? &temp->left : &temp->right;
        }

        if (*p == sentinel) {
            break;
        }

        temp = *p;
    }

    *p = node;
    node->parent = temp;
    node->left = sentinel;
    node->right = sentinel;
    ngx_rbt_red(node);
}


static void
ngx_http_file_cache_vary(ngx_http_request_t *r, u_char *vary, size_t len,
    u_char *hash)
{
    u_char     *p, *last;
    ngx_str_t   name;
    ngx_md5_t   md5;
    u_char      buf[NGX_HTTP_CACHE_VARY_LEN];

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http file cache vary: \"%*s\"", len, vary);

    ngx_md5_init(&md5);
    ngx_md5_update(&md5, r->cache->main, NGX_HTTP_CACHE_KEY_LEN);

    ngx_strlow(buf, vary, len);

    p = buf;
    last = buf + len;

    while (p < last) {

        while (p < last && (*p == ' ' || *p == ',')) { p++; }

        name.data = p;

        while (p < last && *p != ',' && *p != ' ') { p++; }

        name.len = p - name.data;

        if (name.len == 0) {
            break;
        }

        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http file cache vary: %V", &name);

        ngx_md5_update(&md5, name.data, name.len);
        ngx_md5_update(&md5, (u_char *) ":", sizeof(":") - 1);

        ngx_http_file_cache_vary_header(r, &md5, &name);

        ngx_md5_update(&md5, (u_char *) CRLF, sizeof(CRLF) - 1);
    }

    ngx_md5_final(hash, &md5);
}


static void
ngx_http_file_cache_vary_header(ngx_http_request_t *r, ngx_md5_t *md5,
    ngx_str_t *name)
{
    size_t            len;
    u_char           *p, *start, *last;
    ngx_uint_t        i, multiple, normalize;
    ngx_list_part_t  *part;
    ngx_table_elt_t  *header;

    multiple = 0;
    normalize = 0;

    if (name->len == sizeof("Accept-Charset") - 1
        && ngx_strncasecmp(name->data, (u_char *) "Accept-Charset",
                           sizeof("Accept-Charset") - 1) == 0)
    {
        normalize = 1;

    } else if (name->len == sizeof("Accept-Encoding") - 1
        && ngx_strncasecmp(name->data, (u_char *) "Accept-Encoding",
                           sizeof("Accept-Encoding") - 1) == 0)
    {
        normalize = 1;

    } else if (name->len == sizeof("Accept-Language") - 1
        && ngx_strncasecmp(name->data, (u_char *) "Accept-Language",
                           sizeof("Accept-Language") - 1) == 0)
    {
        normalize = 1;
    }

    part = &r->headers_in.headers.part;
    header = part->elts;

    for (i = 0; /* void */ ; i++) {

        if (i >= part->nelts) {
            if (part->next == NULL) {
                break;
            }

            part = part->next;
            header = part->elts;
            i = 0;
        }

        if (header[i].hash == 0) {
            continue;
        }

        if (header[i].key.len != name->len) {
            continue;
        }

        if (ngx_strncasecmp(header[i].key.data, name->data, name->len) != 0) {
            continue;
        }

        if (!normalize) {

            if (multiple) {
                ngx_md5_update(md5, (u_char *) ",", sizeof(",") - 1);
            }

            ngx_md5_update(md5, header[i].value.data, header[i].value.len);

            multiple = 1;

            continue;
        }

        /* normalize spaces */

        p = header[i].value.data;
        last = p + header[i].value.len;

        while (p < last) {

            while (p < last && (*p == ' ' || *p == ',')) { p++; }

            start = p;

            while (p < last && *p != ',' && *p != ' ') { p++; }

            len = p - start;

            if (len == 0) {
                break;
            }

            if (multiple) {
                ngx_md5_update(md5, (u_char *) ",", sizeof(",") - 1);
            }

            ngx_md5_update(md5, start, len);

            multiple = 1;
        }
    }
}


static ngx_int_t
ngx_http_file_cache_reopen(ngx_http_request_t *r, ngx_http_cache_t *c)
{
    ngx_http_file_cache_t  *cache;

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->file.log, 0,
                   "http file cache reopen");

    if (c->secondary) {
        ngx_log_error(NGX_LOG_CRIT, r->connection->log, 0,
                      "cache file \"%s\" has incorrect vary hash",
                      c->file.name.data);
        return NGX_DECLINED;
    }

    cache = c->file_cache;

    ngx_shmtx_lock(&cache->shpool->mutex);

    c->node->count--;
    c->node = NULL;

    ngx_shmtx_unlock(&cache->shpool->mutex);

    c->secondary = 1;
    c->file.name.len = 0;
    c->body_start = c->buf->end - c->buf->start;

    ngx_memcpy(c->key, c->variant, NGX_HTTP_CACHE_KEY_LEN);

    return ngx_http_file_cache_open(r);
}

/*
root@root:/var/yyz/cache_xxx# cat b/7d/bf6813c2bc0becb369a8d8367b6b77db 
