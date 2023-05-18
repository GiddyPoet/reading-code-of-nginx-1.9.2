
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


 
#ifndef _NGX_HTTP_CACHE_H_INCLUDED_
#define _NGX_HTTP_CACHE_H_INCLUDED_

/*
缓存涉及相关:
?http://blog.csdn.net/brainkick/article/details/8535242 
?http://blog.csdn.net/brainkick/article/details/8570698 
?http://blog.csdn.net/brainkick/article/details/8583335 
?http://blog.csdn.net/brainkick/article/details/8592027 
?http://blog.csdn.net/brainkick/article/details/39966271 
*/

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#define NGX_HTTP_CACHE_MISS          1
//xxx_cache_bypass 配置指令可以使满足既定条件的请求绕过缓存数据，但是这些请求的响应数据依然可以被 upstream 模块缓存。 
#define NGX_HTTP_CACHE_BYPASS        2 //说明不能从缓存获取，应该从后端获取 通过xxx_cache_bypass设置判断ngx_http_test_predicates(r, u->conf->cache_bypass)
#define NGX_HTTP_CACHE_EXPIRED       3  //缓存内容过期，当前请求需要向后端请求新的响应数据。
//见ngx_http_file_cache_open->ngx_http_file_cache_read，然后在ngx_http_upstream_cache会把u->cache_status = NGX_HTTP_CACHE_EXPIRED;
#define NGX_HTTP_CACHE_STALE         4 //表示自己是第一个发现该缓存过期的客户端请求，因此自己需要从后端从新获取
#define NGX_HTTP_CACHE_UPDATING      5 //缓存内容过期，同时己有同样使用该缓存节点的其它请求正在向请求新的响应数据。等其他请求获取完后端数据后，自己在读缓存文件向客户端发送
#define NGX_HTTP_CACHE_REVALIDATED   6
#define NGX_HTTP_CACHE_HIT           7 //缓存正常命中
/*
因缓存节点被查询次数还未达 `min_uses`，对此请求禁用缓存机制继续请求处理，但是不再缓存其响应数据 (`u->cacheable = 0`)。
case NGX_HTTP_CACHE_SCARCE: (函数ngx_http_upstream_cache)
        u->cacheable = 0;

*/
#define NGX_HTTP_CACHE_SCARCE        8

#define NGX_HTTP_CACHE_KEY_LEN       16
#define NGX_HTTP_CACHE_ETAG_LEN      42
#define NGX_HTTP_CACHE_VARY_LEN      42

#define NGX_HTTP_CACHE_VERSION       3


typedef struct { //创建空间和赋值见ngx_http_file_cache_valid_set_slot
    ngx_uint_t                       status; //2XX 3XX 4XX 5XX等，如果为0表示proxy_cache_valid any 3m;
    time_t                           valid; //proxy_cache_valid xxx 4m;中的4m
} ngx_http_cache_valid_t;

//结构体 ngx_http_file_cache_node_t 保存磁盘缓存文件在内存中的描述信息 
//一个cache文件对应一个node，这个node中主要保存了cache 的key和uniq， uniq主要是关联文件，而key是用于红黑树。

/*
为后端应答回来的数据创建缓存文件用该函数获取缓存文件名，客户端请求过来后，也是采用该函数获取缓存文件名，只要
proxy_cache_key $scheme$proxy_host$request_uri配置中的变量对应的值一样，则获取到的文件名肯定是一样的，即使是不同的客户端r，参考ngx_http_file_cache_name
因为不同客户端的proxy_cache_key配置的对应变量value一样，则他们计算出来的ngx_http_cache_s->key[]也会一样，他们的在红黑树和queue队列中的
node节点也会是同一个，参考ngx_http_file_cache_lookup  
*/

/*
   同一个客户端请求r只拥有一个r->ngx_http_cache_t和r->ngx_http_cache_t->ngx_http_file_cache_t结构，同一个客户端可能会请求后端的多个uri，
   则在向后端发起请求前，在ngx_http_file_cache_open->ngx_http_file_cache_exists中会按照proxy_cache_key $scheme$proxy_host$request_uri计算出来的
   MD5来创建对应的红黑树节点，然后添加到ngx_http_file_cache_t->sh->rbtree红黑树中。
*/

/*
缓存文件stat状态信息ngx_cached_open_file_s(ngx_open_file_cache_t->rbtree(expire_queue)的成员   )在ngx_expire_old_cached_files进行失效判断, 
缓存文件内容信息(实实在在的文件信息)ngx_http_file_cache_node_t(ngx_http_file_cache_s->sh中的成员)在ngx_http_file_cache_expire进行失效判断。
*/

//该结构为什么能代表一个缓存文件? 因为ngx_http_file_cache_node_t中的node+key[]就是一个对应的缓存文件的目录f/27/46492fbf0d9d35d3753c66851e81627f中的46492fbf0d9d35d3753c66851e81627f，注意f/27就是最尾部的字节
//该结构式红黑树节点，被添加到ngx_http_file_cache_t->sh->rbtree红黑树中以及ngx_http_file_cache_t->sh->queue队列中
typedef struct { //ngx_http_file_cache_add中创建 //ngx_http_file_cache_exists中创建空间和赋值    
    ngx_rbtree_node_t                node; /* 缓存查询树的节点 */ //node就是本ngx_http_file_cache_node_t结构的前面ngx_rbtree_node_t个字节
    ngx_queue_t                      queue; /* LRU页面置换算法 队列中的节点 */
    
    //参考ngx_http_file_cache_exists，存储的是ngx_http_cache_t->key中的后面一些字节
    u_char                           key[NGX_HTTP_CACHE_KEY_LEN
                                         - sizeof(ngx_rbtree_key_t)]; 

    //ngx_http_file_cache_exists中第一次创建的时候默认为1  ngx_http_file_cache_update会剪1，
    //ngx_http_upstream_finalize_request->ngx_http_file_cache_free也会减1  ngx_http_file_cache_exists中加1，表示有多少个客户端连接在获取该缓存
    unsigned                         count:20;    /* 引用计数 */ //参考
    //只会做自增操作，见ngx_http_file_cache_exists中加1，表示总共有多少个客户端请求该缓存，即使和该客户端连接断开也不会做减1操作
    unsigned                         uses:10;    /* 被请求查询到的次数 */     //多少请求在使用  ngx_http_file_cache_exists没查找到一次自增一次
/*
valid_sec , valid_msec 