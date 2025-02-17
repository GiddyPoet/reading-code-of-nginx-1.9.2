
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    u_char    *name;
    uint32_t   method;
} ngx_http_method_name_t;

//client_body_in_file_only  on | off | clean
#define NGX_HTTP_REQUEST_BODY_FILE_OFF    0
#define NGX_HTTP_REQUEST_BODY_FILE_ON     1
#define NGX_HTTP_REQUEST_BODY_FILE_CLEAN  2


static ngx_int_t ngx_http_core_find_location(ngx_http_request_t *r);
static ngx_int_t ngx_http_core_find_static_location(ngx_http_request_t *r,
    ngx_http_location_tree_node_t *node);

static ngx_int_t ngx_http_core_preconfiguration(ngx_conf_t *cf);
static ngx_int_t ngx_http_core_postconfiguration(ngx_conf_t *cf);
static void *ngx_http_core_create_main_conf(ngx_conf_t *cf);
static char *ngx_http_core_init_main_conf(ngx_conf_t *cf, void *conf);
static void *ngx_http_core_create_srv_conf(ngx_conf_t *cf);
static char *ngx_http_core_merge_srv_conf(ngx_conf_t *cf,
    void *parent, void *child);
static void *ngx_http_core_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_core_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);

static char *ngx_http_core_server(ngx_conf_t *cf, ngx_command_t *cmd,
    void *dummy);
static char *ngx_http_core_location(ngx_conf_t *cf, ngx_command_t *cmd,
    void *dummy);
static ngx_int_t ngx_http_core_regex_location(ngx_conf_t *cf,
    ngx_http_core_loc_conf_t *clcf, ngx_str_t *regex, ngx_uint_t caseless);

static char *ngx_http_core_types(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_type(ngx_conf_t *cf, ngx_command_t *dummy,
    void *conf);

static char *ngx_http_core_listen(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_server_name(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_root(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_http_core_limit_except(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_set_aio(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_directio(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_error_page(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_try_files(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_open_file_cache(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_error_log(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_keepalive(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_internal(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_core_resolver(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
#if (NGX_HTTP_GZIP)
static ngx_int_t ngx_http_gzip_accept_encoding(ngx_str_t *ae);
static ngx_uint_t ngx_http_gzip_quantity(u_char *p, u_char *last);
static char *ngx_http_gzip_disable(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
#endif
static ngx_int_t ngx_http_get_forwarded_addr_internal(ngx_http_request_t *r,
    ngx_addr_t *addr, u_char *xff, size_t xfflen, ngx_array_t *proxies,
    int recursive);
#if (NGX_HAVE_OPENAT)
static char *ngx_http_disable_symlinks(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
#endif

static char *ngx_http_core_lowat_check(ngx_conf_t *cf, void *post, void *data);
static char *ngx_http_core_pool_size(ngx_conf_t *cf, void *post, void *data);

static ngx_conf_post_t  ngx_http_core_lowat_post =
    { ngx_http_core_lowat_check };

static ngx_conf_post_handler_pt  ngx_http_core_pool_size_p =
    ngx_http_core_pool_size;


static ngx_conf_enum_t  ngx_http_core_request_body_in_file[] = {
    { ngx_string("off"), NGX_HTTP_REQUEST_BODY_FILE_OFF },
    { ngx_string("on"), NGX_HTTP_REQUEST_BODY_FILE_ON },
    { ngx_string("clean"), NGX_HTTP_REQUEST_BODY_FILE_CLEAN },
    { ngx_null_string, 0 }
};


static ngx_conf_enum_t  ngx_http_core_satisfy[] = {
    { ngx_string("all"), NGX_HTTP_SATISFY_ALL },
    { ngx_string("any"), NGX_HTTP_SATISFY_ANY },
    { ngx_null_string, 0 }
};


static ngx_conf_enum_t  ngx_http_core_lingering_close[] = {
    { ngx_string("off"), NGX_HTTP_LINGERING_OFF },
    { ngx_string("on"), NGX_HTTP_LINGERING_ON },
    { ngx_string("always"), NGX_HTTP_LINGERING_ALWAYS },
    { ngx_null_string, 0 }
};


static ngx_conf_enum_t  ngx_http_core_if_modified_since[] = {
    { ngx_string("off"), NGX_HTTP_IMS_OFF },
    { ngx_string("exact"), NGX_HTTP_IMS_EXACT },
    { ngx_string("before"), NGX_HTTP_IMS_BEFORE },
    { ngx_null_string, 0 }
};


static ngx_conf_bitmask_t  ngx_http_core_keepalive_disable[] = {
    { ngx_string("none"), NGX_HTTP_KEEPALIVE_DISABLE_NONE },
    { ngx_string("msie6"), NGX_HTTP_KEEPALIVE_DISABLE_MSIE6 },
    { ngx_string("safari"), NGX_HTTP_KEEPALIVE_DISABLE_SAFARI },
    { ngx_null_string, 0 }
};


static ngx_path_init_t  ngx_http_client_temp_path = {
    ngx_string(NGX_HTTP_CLIENT_TEMP_PATH), { 0, 0, 0 }
};


#if (NGX_HTTP_GZIP)

static ngx_conf_enum_t  ngx_http_gzip_http_version[] = {
    { ngx_string("1.0"), NGX_HTTP_VERSION_10 },
    { ngx_string("1.1"), NGX_HTTP_VERSION_11 },
    { ngx_null_string, 0 }
};


static ngx_conf_bitmask_t  ngx_http_gzip_proxied_mask[] = {
    { ngx_string("off"), NGX_HTTP_GZIP_PROXIED_OFF },
    { ngx_string("expired"), NGX_HTTP_GZIP_PROXIED_EXPIRED },
    { ngx_string("no-cache"), NGX_HTTP_GZIP_PROXIED_NO_CACHE },
    { ngx_string("no-store"), NGX_HTTP_GZIP_PROXIED_NO_STORE },
    { ngx_string("private"), NGX_HTTP_GZIP_PROXIED_PRIVATE },
    { ngx_string("no_last_modified"), NGX_HTTP_GZIP_PROXIED_NO_LM },
    { ngx_string("no_etag"), NGX_HTTP_GZIP_PROXIED_NO_ETAG },
    { ngx_string("auth"), NGX_HTTP_GZIP_PROXIED_AUTH },
    { ngx_string("any"), NGX_HTTP_GZIP_PROXIED_ANY },
    { ngx_null_string, 0 }
};


static ngx_str_t  ngx_http_gzip_no_cache = ngx_string("no-cache");
static ngx_str_t  ngx_http_gzip_no_store = ngx_string("no-store");
static ngx_str_t  ngx_http_gzip_private = ngx_string("private");

#endif

//相关配置见ngx_event_core_commands ngx_http_core_commands ngx_stream_commands ngx_http_core_commands ngx_core_commands  ngx_mail_commands
static ngx_command_t  ngx_http_core_commands[] = {
    /* 确定桶的个数，越大冲突概率越小。variables_hash_max_size是每个桶中对应的散列信息个数 
        配置块:http server location
     */
    { ngx_string("variables_hash_max_size"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_core_main_conf_t, variables_hash_max_size),
      NULL },

    /* server_names_hash_max_size 32 | 64 |128 ，为了提个寻找server_name的能力，nginx使用散列表来存储server name。
        这个是设置每个散列桶咋弄的内存大小，注意和variables_hash_max_size区别
        配置块:http server location
    */
    { ngx_string("variables_hash_bucket_size"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_core_main_conf_t, variables_hash_bucket_size),
      NULL },
    /* server_names_hash_max_size 32 | 64 |128 ，为了提个寻找server_name的能力，nginx使用散列表来存储server name。
    */
    { ngx_string("server_names_hash_max_size"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_core_main_conf_t, server_names_hash_max_size),
      NULL },

    { ngx_string("server_names_hash_bucket_size"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_MAIN_CONF_OFFSET,
      offsetof(ngx_http_core_main_conf_t, server_names_hash_bucket_size),
      NULL },

    { ngx_string("server"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_NOARGS,
      ngx_http_core_server,
      0,
      0,
      NULL },

    /*
    connection_pool_size
    语法：connection_pool_size size;
    默认：connection_pool_size 256;
    配置块：http、server
    Nginx对于每个建立成功的TCP连接会预先分配一个内存池，上面的size配置项将指定这个内存池的初始大小（即ngx_connection_t结构体中的pool内存池初始大小，
    9.8.1节将介绍这个内存池是何时分配的），用于减少内核对于小块内存的分配次数。需慎重设置，因为更大的size会使服务器消耗的内存增多，而更小的size则会引发更多的内存分配次数。
    */
    { ngx_string("connection_pool_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, connection_pool_size),
      &ngx_http_core_pool_size_p },

/*
语法：request_pool_size size;
默认：request_pool_size 4k;
配置块：http、server
Nginx开始处理HTTP请求时，将会为每个请求都分配一个内存池，size配置项将指定这个内存池的初始大小（即ngx_http_request_t结构体中的pool内存池初始大小，
11.3节将介绍这个内存池是何时分配的），用于减少内核对于小块内存的分配次数。TCP连接关闭时会销毁connection_pool_size指定的连接内存池，HTTP请求结束
时会销毁request_pool_size指定的HTTP请求内存池，但它们的创建、销毁时间并不一致，因为一个TCP连接可能被复用于多个HTTP请求。
*/
    { ngx_string("request_pool_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, request_pool_size),
      &ngx_http_core_pool_size_p },
    /*
    读取HTTP头部的超时时间
    语法：client_header_timeout time（默认单位：秒）;
    默认：client_header_timeout 60;
    配置块：http、server、location
    客户端与服务器建立连接后将开始接收HTTP头部，在这个过程中，如果在一个时间间隔（超时时间）内没有读取到客户端发来的字节，则认为超时，
    并向客户端返回408 ("Request timed out")响应。
    */
    { ngx_string("client_header_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, client_header_timeout),
      NULL },

/*
语法:  client_header_buffer_size size;
默认值:  client_header_buffer_size 1k;
上下文:  http, server

设置读取客户端请求头部的缓冲容量。 对于大多数请求，1K的缓冲足矣。 但如果请求中含有的cookie很长，或者请求来自WAP的客户端，可能
请求头不能放在1K的缓冲中。 如果从请求行，或者某个请求头开始不能完整的放在这块空间中，那么nginx将按照 large_client_header_buffers
指令的配置分配更多更大的缓冲来存放。
*/
    { ngx_string("client_header_buffer_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, client_header_buffer_size),
      NULL },

/*
存储超大HTTP头部的内存buffer大小
语法：large_client_header_buffers number size;
默认：large_client_header_buffers 4 8k;
配置块：http、server
large_client_header_buffers定义了Nginx接收一个超大HTTP头部请求的buffer个数和每个buffer的大小。如果HTTP请求行（如GET /index HTTP/1.1）
的大小超过上面的单个buffer，则返回"Request URI too large" (414)。请求中一般会有许多header，每一个header的大小也不能超过单个buffer的大小，
否则会返回"Bad request" (400)。当然，请求行和请求头部的总和也不可以超过buffer个数*buffer大小。

设置读取客户端请求超大请求的缓冲最大number(数量)和每块缓冲的size(容量)。 HTTP请求行的长度不能超过一块缓冲的容量，否则nginx返回错误414
(Request-URI Too Large)到客户端。 每个请求头的长度也不能超过一块缓冲的容量，否则nginx返回错误400 (Bad Request)到客户端。 缓冲仅在必需
是才分配，默认每块的容量是8K字节。 即使nginx处理完请求后与客户端保持入长连接，nginx也会释放这些缓冲。 
*/ //当client_header_buffer_size不够存储头部行的时候，用large_client_header_buffers再次分配空间存储
    { ngx_string("large_client_header_buffers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_TAKE2,
      ngx_conf_set_bufs_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, large_client_header_buffers),
      NULL },

/*
忽略不合法的HTTP头部
语法：ignore_invalid_headers on | off;
默认：ignore_invalid_headers on;
配置块：http、server
如果将其设置为off，那么当出现不合法的HTTP头部时，Nginx会拒绝服务，并直接向用户发送400（Bad Request）错误。如果将其设置为on，则会忽略此HTTP头部。
*/
    { ngx_string("ignore_invalid_headers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, ignore_invalid_headers),
      NULL },

/*
merge_slashes
语法：merge_slashes on | off;
默认：merge_slashes on;
配置块：http、server、location
此配置项表示是否合并相邻的“/”，例如，//test///a.txt，在配置为on时，会将其匹配为location /test/a.txt；如果配置为off，则不会匹配，URI将仍然是//test///a.txt。
*/
    { ngx_string("merge_slashes"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, merge_slashes),
      NULL },

/*
HTTP头部是否允许下画线
语法：underscores_in_headers on | off;
默认：underscores_in_headers off;
配置块：http、server
默认为off，表示HTTP头部的名称中不允许带“_”（下画线）。
*/
    { ngx_string("underscores_in_headers"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_SRV_CONF_OFFSET,
      offsetof(ngx_http_core_srv_conf_t, underscores_in_headers),
      NULL },
    /*
        location [= ~ ~* ^~ @ ] /uri/ {....} location会尝试根据用户请求中url来匹配上面的/url表达式，如果可以匹配
        就选择location{}块中的配置来处理用户请求。当然，匹配方式是多样的，如下:
        1) = 表示把url当做字符串，以便于参数中的url做完全匹配。例如
            localtion = / {
                #只有当用户请求是/时，才会使用该location下的配置。
            }
        2) ~表示匹配url时是字母大小写敏感的。
        3) ~*表示匹配url时忽略字母大小写问题
        4) ^~表示匹配url时指需要其前半部分与url参数匹配即可，例如:
            location ^~ /images/ {
                #以/images/开通的请求都会被匹配上
            }
        5) @表示仅用于nginx服务器内部请求之间的重定向，带有@的location不直接处理用户请求。当然，在url参数里是可以用
            正则表达式的，例如:
            location ~* \.(gif|jpg|jpeg)$ {
                #匹配以.gif .jpg .jpeg结尾的请求。
            }

        上面这些方式表达为"如果匹配，则..."，如果要实现"如果不匹配，则...."，可以在最后一个location中使用/作为参数，它会匹配所有
        的HTTP请求，这样就可以表示如果不能匹配前面的所有location，则由"/"这个location处理。例如:
            location / {
                # /可以匹配所有请求。
            }

         完全匹配 > 前缀匹配 > 正则表达式 > /
    */ //location {}配置查找可以参考ngx_http_core_find_config_phase->ngx_http_core_find_location
    { ngx_string("location"), 
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_BLOCK|NGX_CONF_TAKE12,
      ngx_http_core_location,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },
    //lisen [ip:port | ip(不知道端口默认为80) | 端口 | *:端口 | localhost:端口] [ngx_http_core_listen函数中的参数]
    { ngx_string("listen"),  //
      NGX_HTTP_SRV_CONF|NGX_CONF_1MORE,
      ngx_http_core_listen,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },
      
    /* server_name www.a.com www.b.com，可以跟多个服务器域名
    在开始处理一个HTTP请求是，nginx会取出header头中的Host,与每个server的server_name进行匹配，一次决定到底由那一个server
    块来处理这个请求。有可能一个host与多个server块中的多个server_name匹配，这是就会根据匹配优先级来选择实际处理的server块。
    server_name与HOst的匹配优先级如下:
    1 首先匹配字符串完全匹配的servername,如www.aaa.com
    2 其次选择通配符在前面的servername,如*.aaa.com
    3 再次选择通配符在后面的servername,如www.aaa.*
    4 最新选择使用正则表达式才匹配的servername.

    如果都不匹配，按照下面顺序选择处理server块:
    1 优先选择在listen配置项后加入[default|default_server]的server块。
    2 找到匹配listen端口的第一个server块

    如果server_name后面跟着空字符串，如server_name ""表示匹配没有host这个HTTP头部的请求
    该参数默认为server_name ""
    server_name_in_redirect on | off 该配置需要配合server_name使用。在使用on打开后,表示在重定向请求时会使用
    server_name里的第一个主机名代替原先请求中的Host头部，而使用off关闭时，表示在重定向请求时使用请求本身的HOST头部
    */ //官方详细文档参考http://nginx.org/en/docs/http/server_names.html
    { ngx_string("server_name"),
      NGX_HTTP_SRV_CONF|NGX_CONF_1MORE,
      ngx_http_core_server_name,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

/*
types_hash_max_size
语法：types_hash_max_size size;
默认：types_hash_max_size 1024;
配置块：http、server、location
types_hash_max_size影响散列表的冲突率。types_hash_max_size越大，就会消耗更多的内存，但散列key的冲突率会降低，检索速度就更快。types_hash_max_size越小，消耗的内存就越小，但散列key的冲突率可能上升。
*/
    { ngx_string("types_hash_max_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, types_hash_max_size),
      NULL },

/*
types_hash_bucket_size
语法：types_hash_bucket_size size;
默认：types_hash_bucket_size 32|64|128;
配置块：http、server、location
为了快速寻找到相应MIME type，Nginx使用散列表来存储MIME type与文件扩展名。types_hash_bucket_size 设置了每个散列桶占用的内存大小。
*/
    { ngx_string("types_hash_bucket_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, types_hash_bucket_size),
      NULL },

    /*
    下面是MIME类型的设置配置项。
    MIME type与文件扩展的映射
    语法：type {...};
    配置块：http、server、location
    定义MIME type到文件扩展名的映射。多个扩展名可以映射到同一个MIME type。例如：
    types {
     text/html    html;
     text/html    conf;
     image/gif    gif;
     image/jpeg   jpg;
    }
    */ //types和default_type对应
//types {}配置ngx_http_core_type首先存在与该数组中，然后在ngx_http_core_merge_loc_conf存入types_hash中，真正生效见ngx_http_set_content_type
    { ngx_string("types"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF
                                          |NGX_CONF_BLOCK|NGX_CONF_NOARGS,
      ngx_http_core_types,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
/*
默认MIME type
语法：default_type MIME-type;
默认：default_type text/plain;
配置块：http、server、location
当找不到相应的MIME type与文件扩展名之间的映射时，使用默认的MIME type作为HTTP header中的Content-Type。
*/ //types和default_type对应
    { ngx_string("default_type"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, default_type),
      NULL },

    /* 
       nginx指定文件路径有两种方式root和alias，这两者的用法区别，使用方法总结了下，方便大家在应用过程中，快速响应。root与alias主要区别在于nginx如何解释location后面的uri，这会使两者分别以不同的方式将请求映射到服务器文件上。
       [root]
       语法：root path
       默认值：root html
       配置段：http、server、location、if
       [alias]
       语法：alias path
       配置段：location
       实例：
       
       location ~ ^/weblogs/ {
        root /data/weblogs/www.ttlsa.com;
        autoindex on;
        auth_basic            "Restricted";
        auth_basic_user_file  passwd/weblogs;
       }
       如果一个请求的URI是/weblogs/httplogs/www.ttlsa.com-access.log时，web服务器将会返回服务器上的/data/weblogs/www.ttlsa.com/weblogs/httplogs/www.ttlsa.com-access.log的文件。
       [info]root会根据完整的URI请求来映射，也就是/path/uri。[/info]
       因此，前面的请求映射为path/weblogs/httplogs/www.ttlsa.com-access.log。
       
       
       location ^~ /binapp/ {  
        limit_conn limit 4;
        limit_rate 200k;
        internal;  
        alias /data/statics/bin/apps/;
       }
       alias会把location后面配置的路径丢弃掉，把当前匹配到的目录指向到指定的目录。如果一个请求的URI是/binapp/a.ttlsa.com/favicon时，web服务器将会返回服务器上的/data/statics/bin/apps/a.ttlsa.com/favicon.jgp的文件。
       [warning]1. 使用alias时，目录名后面一定要加"/"。
       2. alias可以指定任何名称。
       3. alias在使用正则匹配时，必须捕捉要匹配的内容并在指定的内容处使用。
       4. alias只能位于location块中。[/warning]
       如需转载请注明出处：  http://www.ttlsa.com/html/2907.html


       定义资源文件路径。默认root html.配置块:http  server location  if， 如:
        location /download/ {
            root /opt/web/html/;
        } 
        如果有一个请求的url是/download/index/aa.html,那么WEB将会返回服务器上/opt/web/html/download/index/aa.html文件的内容
    */
    { ngx_string("root"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_http_core_root,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    /*
    以alias方式设置资源路径
    语法：alias path;
    配置块：location
    
    alias也是用来设置文件资源路径的，它与root的不同点主要在于如何解读紧跟location后面的uri参数，这将会致使alias与root以不同的方式将用户请求映射到真正的磁盘文件上。例如，如果有一个请求的URI是/conf/nginx.conf，而用户实际想访问的文件在/usr/local/nginx/conf/nginx.conf，那么想要使用alias来进行设置的话，可以采用如下方式：
    location /conf {
       alias /usr/local/nginx/conf/;   
    }
    
    如果用root设置，那么语句如下所示：
    location /conf {
       root /usr/local/nginx/;       
    }
    
    使用alias时，在URI向实际文件路径的映射过程中，已经把location后配置的/conf这部分字符串丢弃掉，
    因此，/conf/nginx.conf请求将根据alias path映射为path/nginx.conf。root则不然，它会根据完整的URI
    请求来映射，因此，/conf/nginx.conf请求会根据root path映射为path/conf/nginx.conf。这也是root
    可以放置到http、server、location或if块中，而alias只能放置到location块中的原因。
    
    alias后面还可以添加正则表达式，例如：
    location ~ ^/test/(\w+)\.(\w+)$ {
        alias /usr/local/nginx/$2/$1.$2;
    }
    
    这样，请求在访问/test/nginx.conf时，Nginx会返回/usr/local/nginx/conf/nginx.conf文件中的内容。

    nginx指定文件路径有两种方式root和alias，这两者的用法区别，使用方法总结了下，方便大家在应用过程中，快速响应。root与alias主要区别在于nginx如何解释location后面的uri，这会使两者分别以不同的方式将请求映射到服务器文件上。
[root]
语法：root path
默认值：root html
配置段：http、server、location、if
[alias]
语法：alias path
配置段：location
实例：

location ~ ^/weblogs/ {
 root /data/weblogs/www.ttlsa.com;
 autoindex on;
 auth_basic            "Restricted";
 auth_basic_user_file  passwd/weblogs;
}
如果一个请求的URI是/weblogs/httplogs/www.ttlsa.com-access.log时，web服务器将会返回服务器上的/data/weblogs/www.ttlsa.com/weblogs/httplogs/www.ttlsa.com-access.log的文件。
[info]root会根据完整的URI请求来映射，也就是/path/uri。[/info]
因此，前面的请求映射为path/weblogs/httplogs/www.ttlsa.com-access.log。


location ^~ /binapp/ {  
 limit_conn limit 4;
 limit_rate 200k;
 internal;  
 alias /data/statics/bin/apps/;
}
alias会把location后面配置的路径丢弃掉，把当前匹配到的目录指向到指定的目录。如果一个请求的URI是/binapp/a.ttlsa.com/favicon时，web服务器将会返回服务器上的/data/statics/bin/apps/a.ttlsa.com/favicon.jgp的文件。
[warning]1. 使用alias时，目录名后面一定要加"/"。
2. alias可以指定任何名称。
3. alias在使用正则匹配时，必须捕捉要匹配的内容并在指定的内容处使用。
4. alias只能位于location块中。[/warning]
如需转载请注明出处：  http://www.ttlsa.com/html/2907.html
    */
    { ngx_string("alias"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_core_root,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    /*
按HTTP方法名限制用户请求
语法:  limit_except method ... { ... }
默认值:  ―  
上下文:  location
 
允许按请求的HTTP方法限制对某路径的请求。method用于指定不由这些限制条件进行过滤的HTTP方法，可选值有 GET、 HEAD、 POST、 PUT、 
DELETE、 MKCOL、 COPY、 MOVE、 OPTIONS、 PROPFIND、 PROPPATCH、 LOCK、 UNLOCK 或者 PATCH。 指定method为GET方法的同时，
nginx会自动添加HEAD方法。 那么其他HTTP方法的请求就会由指令引导的配置块中的ngx_http_access_module 模块和ngx_http_auth_basic_module
模块的指令来限制访问。如： 

limit_except GET {
    allow 192.168.1.0/32;
    deny  all;
}

请留意上面的例子将对除GET和HEAD方法以外的所有HTTP方法的请求进行访问限制。 
    */
    { ngx_string("limit_except"),
      NGX_HTTP_LOC_CONF|NGX_CONF_BLOCK|NGX_CONF_1MORE,
      ngx_http_core_limit_except,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
/*
HTTP请求包体的最大值
语法：client_max_body_size size;
默认：client_max_body_size 1m;
配置块：http、server、location
浏览器在发送含有较大HTTP包体的请求时，其头部会有一个Content-Length字段，client_max_body_size是用来限制Content-Length所示值的大小的。因此，
这个限制包体的配置非常有用处，因为不用等Nginx接收完所有的HTTP包体―这有可能消耗很长时间―就可以告诉用户请求过大不被接受。例如，用户试图
上传一个10GB的文件，Nginx在收完包头后，发现Content-Length超过client_max_body_size定义的值，就直接发送413 ("Request Entity Too Large")响应给客户端。
*/
    { ngx_string("client_max_body_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_off_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, client_max_body_size),
      NULL },

/*
存储HTTP头部的内存buffer大小
语法：client_header_buffer_size size;
默认：client_header_buffer_size 1k;
配置块：http、server
上面配置项定义了正常情况下Nginx接收用户请求中HTTP header部分（包括HTTP行和HTTP头部）时分配的内存buffer大小。有时，
请求中的HTTP header部分可能会超过这个大小，这时large_client_header_buffers定义的buffer将会生效。
*/
    { ngx_string("client_body_buffer_size"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, client_body_buffer_size),
      NULL },

    /*
    读取HTTP包体的超时时间
    语法：client_body_timeout time（默认单位：秒）；
    默认：client_body_timeout 60;
    配置块：http、server、location
    此配置项与client_header_timeout相似，只是这个超时时间只在读取HTTP包体时才有效。
    */
    { ngx_string("client_body_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, client_body_timeout),
      NULL },

/*
HTTP包体的临时存放目录
语法：client_body_temp_path dir-path [ level1 [ level2 [ level3 ]]]
默认：client_body_temp_path client_body_temp;
配置块：http、server、location
上面配置项定义HTTP包体存放的临时目录。在接收HTTP包体时，如果包体的大小大于client_body_buffer_size，则会以一个递增的整数命名并存放到
client_body_temp_path指定的目录中。后面跟着的level1、level2、level3，是为了防止一个目录下的文件数量太多，从而导致性能下降，
因此使用了level参数，这样可以按照临时文件名最多再加三层目录。例如：
client_body_temp_path  /opt/nginx/client_temp 1 2;
如果新上传的HTTP 包体使用00000123456作为临时文件名，就会被存放在这个目录中。
/opt/nginx/client_temp/6/45/00000123456
*/
    { ngx_string("client_body_temp_path"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1234,
      ngx_conf_set_path_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, client_body_temp_path),
      NULL },

    /*
    HTTP包体只存储到磁盘文件中
    语法：client_body_in_file_only on | clean | off;
    默认：client_body_in_file_only off;
    配置块：http、server、location
    当值为非off时，用户请求中的HTTP包体一律存储到磁盘文件中，即使只有0字节也会存储为文件。当请求结束时，如果配置为on，则这个文件不会
    被删除（该配置一般用于调试、定位问题），但如果配置为clean，则会删除该文件。
   */
    { ngx_string("client_body_in_file_only"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_enum_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, client_body_in_file_only),
      &ngx_http_core_request_body_in_file },

/*
HTTP包体尽量写入到一个内存buffer中
语法：client_body_in_single_buffer on | off;
默认：client_body_in_single_buffer off;
配置块：http、server、location
用户请求中的HTTP包体一律存储到内存唯一同一个buffer中。当然，如果HTTP包体的大小超过了下面client_body_buffer_size设置的值，包体还是会写入到磁盘文件中。
*/
    { ngx_string("client_body_in_single_buffer"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, client_body_in_single_buffer),
      NULL },

/*
sendfile系统调用
语法：sendfile on | off;
默认：sendfile off;
配置块：http、server、location
可以启用Linux上的sendfile系统调用来发送文件，它减少了内核态与用户态之间的两次内存复制，这样就会从磁盘中读取文件后直接在内核态发送到网卡设备，
提高了发送文件的效率。
*/ 
    /*
    When both AIO and sendfile are enabled on Linux, AIO is used for files that are larger than or equal to the size specified in the 
    directio directive, while sendfile is used for files of smaller sizes or when directio is disabled. 
    如果aio on; sendfile都配置了，并且执行了b->file->directio = of.is_directio(并且of.is_directio要为1)这几个模块，
    则当文件大小大于等于directio指定size(默认512)的时候使用aio,当小于size或者directio off的时候使用sendfile
    生效见ngx_open_and_stat_file  if (of->directio <= ngx_file_size(&fi)) { ngx_directio_on } 以及ngx_output_chain_copy_buf

    不过不满足上面的条件，如果aio on; sendfile都配置了，则还是以sendfile为准
    */ //ngx_output_chain_as_is  ngx_output_chain_copy_buf是aio和sendfile和普通文件读写的分支点  ngx_linux_sendfile_chain是sendfile发送和普通write发送的分界点
    { ngx_string("sendfile"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_FLAG,
//一般大缓存文件用aio发送，小文件用sendfile，因为aio是异步的，不影响其他流程，但是sendfile是同步的，太大的话可能需要多次sendfile才能发送完，有种阻塞感觉
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, sendfile),
      NULL },

    { ngx_string("sendfile_max_chunk"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, sendfile_max_chunk),
      NULL },

/*
AIO系统调用
语法：aio on | off;
默认：aio off;
配置块：http、server、location
此配置项表示是否在FreeBSD或Linux系统上启用内核级别的异步文件I/O功能。注意，它与sendfile功能是互斥的。

Syntax:  aio on | off | threads[=pool];
 
Default:  aio off; 
Context:  http, server, location
 
Enables or disables the use of asynchronous file I/O (AIO) on FreeBSD and Linux: 

location /video/ {
    aio            on;
    output_buffers 1 64k;
}

On FreeBSD, AIO can be used starting from FreeBSD 4.3. AIO can either be linked statically into a kernel: 

options VFS_AIO
or loaded dynamically as a kernel loadable module: 

kldload aio

On Linux, AIO can be used starting from kernel version 2.6.22. Also, it is necessary to enable directio, or otherwise reading will be blocking: 

location /video/ {
    aio            on;
    directio       512;
    output_buffers 1 128k;
}

On Linux, directio can only be used for reading blocks that are aligned on 512-byte boundaries (or 4K for XFS). File’s unaligned end is 
read in blocking mode. The same holds true for byte range requests and for FLV requests not from the beginning of a file: reading of 
unaligned data at the beginning and end of a file will be blocking. 

When both AIO and sendfile are enabled on Linux, AIO is used for files that are larger than or equal to the size specified in the directio 
directive, while sendfile is used for files of smaller sizes or when directio is disabled. 

location /video/ {
    sendfile       on;
    aio            on;
    directio       8m;
}

Finally, files can be read and sent using multi-threading (1.7.11), without blocking a worker process: 

location /video/ {
    sendfile       on;
    aio            threads;
}
Read and send file operations are offloaded to threads of the specified pool. If the pool name is omitted, the pool with the name “default” 
is used. The pool name can also be set with variables: 

aio threads=pool$disk;
By default, multi-threading is disabled, it should be enabled with the --with-threads configuration parameter. Currently, multi-threading is 
compatible only with the epoll, kqueue, and eventport methods. Multi-threaded sending of files is only supported on Linux. 
*/
/*
When both AIO and sendfile are enabled on Linux, AIO is used for files that are larger than or equal to the size specified in the 
directio directive, while sendfile is used for files of smaller sizes or when directio is disabled. 
如果aio on; sendfile都配置了，并且执行了b->file->directio = of.is_directio(并且of.is_directio要为1)这几个模块，
则当文件大小大于等于directio指定size(默认512)的时候使用aio,当小于size或者directio off的时候使用sendfile
生效见ngx_open_and_stat_file  if (of->directio <= ngx_file_size(&fi)) { ngx_directio_on } 以及ngx_output_chain_copy_buf

不过不满足上面的条件，如果aio on; sendfile都配置了，则还是以sendfile为准
*/ //ngx_output_chain_as_is  ngx_output_chain_align_file_buf  ngx_output_chain_copy_buf是aio和sendfile和普通文件读写的分支点  ngx_linux_sendfile_chain是sendfile发送和普通write发送的分界点
    { ngx_string("aio"),  
//一般大缓存文件用aio发送，小文件用sendfile，因为aio是异步的，不影响其他流程，但是sendfile是同步的，太大的话可能需要多次sendfile才能发送完，有种阻塞感觉
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_core_set_aio,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("read_ahead"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, read_ahead),
      NULL },

/*
语法：directio size | off;
默认：directio off;
配置块：http、server、location
当文件大小大于该值的时候，可以此配置项在FreeBSD和Linux系统上使用O_DIRECT选项去读取文件，通常对大文件的读取速度有优化作用。注意，它与sendfile功能是互斥的。
*/
/*
When both AIO and sendfile are enabled on Linux, AIO is used for files that are larger than or equal to the size specified in the 
directio directive, while sendfile is used for files of smaller sizes or when directio is disabled. 
如果aio on; sendfile都配置了，并且执行了b->file->directio = of.is_directio(并且of.is_directio要为1)这几个模块，
则当文件大小大于等于directio指定size(默认512)的时候使用aio,当小于size或者directio off的时候使用sendfile
生效见ngx_open_and_stat_file  if (of->directio <= ngx_file_size(&fi)) { ngx_directio_on } 以及ngx_output_chain_copy_buf

不过不满足上面的条件，如果aio on; sendfile都配置了，则还是以sendfile为准


当读入长度大于等于指定size的文件时，开启DirectIO功能。具体的做法是，在FreeBSD或Linux系统开启使用O_DIRECT标志，在MacOS X系统开启
使用F_NOCACHE标志，在Solaris系统开启使用directio()功能。这条指令自动关闭sendfile(0.7.15版)。它在处理大文件时 
*/ //ngx_output_chain_as_is  ngx_output_chain_align_file_buf  ngx_output_chain_copy_buf是aio和sendfile和普通文件读写的分支点  ngx_linux_sendfile_chain是sendfile发送和普通write发送的分界点
  //生效见ngx_open_and_stat_file  if (of->directio <= ngx_file_size(&fi)) { ngx_directio_on }

    /* 数据在文件里面，并且程序有走到了 b->file->directio = of.is_directio(并且of.is_directio要为1)这几个模块，
        并且文件大小大于directio xxx中的大小才才会生效，见ngx_output_chain_align_file_buf  ngx_output_chain_as_is */
    { ngx_string("directio"), //在获取缓存文件内容的时候，只有文件大小大与等于directio的时候才会生效ngx_directio_on
//一般大缓存文件用aio发送，小文件用sendfile，因为aio是异步的，不影响其他流程，太大的话可能需要多次sendfile才能发送完，有种阻塞感觉
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_http_core_directio,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

/*
directio_alignment
语法：directio_alignment size;
默认：directio_alignment 512;  它与directio配合使用，指定以directio方式读取文件时的对齐方式
配置块：http、server、location
它与directio配合使用，指定以directio方式读取文件时的对齐方式。一般情况下，512B已经足够了，但针对一些高性能文件系统，如Linux下的XFS文件系统，
可能需要设置到4KB作为对齐方式。
*/ // 默认512   在ngx_output_chain_get_buf生效，表示分配内存空间的时候，空间起始地址需要按照这个值对齐
    { ngx_string("directio_alignment"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_off_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, directio_alignment),
      NULL },
/*
tcp_nopush
官方:

tcp_nopush
Syntax: tcp_nopush on | off
Default: off
Context: http
server
location
Reference: tcp_nopush
This directive permits or forbids the use of thesocket options TCP_NOPUSH on FreeBSD or TCP_CORK on Linux. This option is 
onlyavailable when using sendfile.
Setting this option causes nginx to attempt to sendit’s HTTP response headers in one packet on Linux and FreeBSD 4.x
You can read more about the TCP_NOPUSH and TCP_CORKsocket options here.

 
linux 下是tcp_cork,上面的意思就是说，当使用sendfile函数时，tcp_nopush才起作用，它和指令tcp_nodelay是互斥的。tcp_cork是linux下
tcp/ip传输的一个标准了，这个标准的大概的意思是，一般情况下，在tcp交互的过程中，当应用程序接收到数据包后马上传送出去，不等待，
而tcp_cork选项是数据包不会马上传送出去，等到数据包最大时，一次性的传输出去，这样有助于解决网络堵塞，已经是默认了。

也就是说tcp_nopush = on 会设置调用tcp_cork方法，这个也是默认的，结果就是数据包不会马上传送出去，等到数据包最大时，一次性的传输出去，
这样有助于解决网络堵塞。

以快递投递举例说明一下（以下是我的理解，也许是不正确的），当快递东西时，快递员收到一个包裹，马上投递，这样保证了即时性，但是会
耗费大量的人力物力，在网络上表现就是会引起网络堵塞，而当快递收到一个包裹，把包裹放到集散地，等一定数量后统一投递，这样就是tcp_cork的
选项干的事情，这样的话，会最大化的利用网络资源，虽然有一点点延迟。

对于nginx配置文件中的tcp_nopush，默认就是tcp_nopush,不需要特别指定，这个选项对于www，ftp等大文件很有帮助

tcp_nodelay
        TCP_NODELAY和TCP_CORK基本上控制了包的“Nagle化”，Nagle化在这里的含义是采用Nagle算法把较小的包组装为更大的帧。 John Nagle是Nagle算法的发明人，后者就是用他的名字来命名的，他在1984年首次用这种方法来尝试解决福特汽车公司的网络拥塞问题（欲了解详情请参看IETF RFC 896）。他解决的问题就是所谓的silly window syndrome，中文称“愚蠢窗口症候群”，具体含义是，因为普遍终端应用程序每产生一次击键操作就会发送一个包，而典型情况下一个包会拥有一个字节的数据载荷以及40个字节长的包头，于是产生4000%的过载，很轻易地就能令网络发生拥塞,。 Nagle化后来成了一种标准并且立即在因特网上得以实现。它现在已经成为缺省配置了，但在我们看来，有些场合下把这一选项关掉也是合乎需要的。

       现在让我们假设某个应用程序发出了一个请求，希望发送小块数据。我们可以选择立即发送数据或者等待产生更多的数据然后再一次发送两种策略。如果我们马上发送数据，那么交互性的以及客户/服务器型的应用程序将极大地受益。如果请求立即发出那么响应时间也会快一些。以上操作可以通过设置套接字的TCP_NODELAY = on 选项来完成，这样就禁用了Nagle 算法。

       另外一种情况则需要我们等到数据量达到最大时才通过网络一次发送全部数据，这种数据传输方式有益于大量数据的通信性能，典型的应用就是文件服务器。应用 Nagle算法在这种情况下就会产生问题。但是，如果你正在发送大量数据，你可以设置TCP_CORK选项禁用Nagle化，其方式正好同 TCP_NODELAY相反（TCP_CORK和 TCP_NODELAY是互相排斥的）。



tcp_nopush
语法：tcp_nopush on | off;
默认：tcp_nopush off;
配置块：http、server、location
在打开sendfile选项时，确定是否开启FreeBSD系统上的TCP_NOPUSH或Linux系统上的TCP_CORK功能。打开tcp_nopush后，将会在发送响应时把整个响应包头放到一个TCP包中发送。
*/ // tcp_nopush on | off;只有开启sendfile，nopush才生效，通过设置TCP_CORK实现
    { ngx_string("tcp_nopush"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, tcp_nopush),
      NULL },

      /*
      在元数据操作等小包传送时，发现性能不好，通过调试发现跟socket的TCP_NODELAY有很大关系。
      TCP_NODELAY 和 TCP_CORK， 
      这两个选项都对网络连接的行为具有重要的作用。许多UNIX系统都实现了TCP_NODELAY选项，但是，TCP_CORK则是Linux系统所独有的而且相对较新；它首先在内核版本2.4上得以实现。
      此外，其他UNIX系统版本也有功能类似的选项，值得注意的是，在某种由BSD派生的系统上的 TCP_NOPUSH选项其实就是TCP_CORK的一部分具体实现。 
      TCP_NODELAY和TCP_CORK基本上控制了包的“Nagle化”，Nagle化在这里的含义是采用Nagle算法把较小的包组装为更大的帧。 John Nagle是Nagle算法的发明人，
      后者就是用他的名字来命名的，他在1984年首次用这种方法来尝试解决福特汽车公司的网络拥塞问题（欲了解详情请参看IETF RFC 896）。他解决的问题就是所谓的silly 
      window syndrome ，中文称“愚蠢窗口症候群”，具体含义是，因为普遍终端应用程序每产生一次击键操作就会发送一个包，而典型情况下一个包会拥有
      一个字节的数据载荷以及40 个字节长的包头，于是产生4000%的过载，很轻易地就能令网络发生拥塞,。 Nagle化后来成了一种标准并且立即在因特网上得以实现。
      它现在已经成为缺省配置了，但在我们看来，有些场合下把这一选项关掉也是合乎需要的。 
      现在让我们假设某个应用程序发出了一个请求，希望发送小块数据。我们可以选择立即发送数据或者等待产生更多的数据然后再一次发送两种策略。如果我们马上发送数据，
      那么交互性的以及客户/服务器型的应用程序将极大地受益。例如，当我们正在发送一个较短的请求并且等候较大的响应时，相关过载与传输的数据总量相比就会比较低，
      而且，如果请求立即发出那么响应时间也会快一些。以上操作可以通过设置套接字的TCP_NODELAY选项来完成，这样就禁用了Nagle 算法。 
      另外一种情况则需要我们等到数据量达到最大时才通过网络一次发送全部数据，这种数据传输方式有益于大量数据的通信性能，典型的应用就是文件服务器。
      应用 Nagle算法在这种情况下就会产生问题。但是，如果你正在发送大量数据，你可以设置TCP_CORK选项禁用Nagle化，其方式正好同 TCP_NODELAY相反
      （TCP_CORK 和 TCP_NODELAY 是互相排斥的）。下面就让我们仔细分析下其工作原理。 
      假设应用程序使用sendfile()函数来转移大量数据。应用协议通常要求发送某些信息来预先解释数据，这些信息其实就是报头内容。典型情况下报头很小，
      而且套接字上设置了TCP_NODELAY。有报头的包将被立即传输，在某些情况下（取决于内部的包计数器），因为这个包成功地被对方收到后需要请求对方确认。
      这样，大量数据的传输就会被推迟而且产生了不必要的网络流量交换。 
      但是，如果我们在套接字上设置了TCP_CORK（可以比喻为在管道上插入“塞子”）选项，具有报头的包就会填补大量的数据，所有的数据都根据大小自动地通过包传输出去。
      当数据传输完成时，最好取消TCP_CORK 选项设置给连接“拔去塞子”以便任一部分的帧都能发送出去。这同“塞住”网络连接同等重要。 
      总而言之，如果你肯定能一起发送多个数据集合（例如HTTP响应的头和正文），那么我们建议你设置TCP_CORK选项，这样在这些数据之间不存在延迟。
      能极大地有益于WWW、FTP以及文件服务器的性能，同时也简化了你的工作。示例代码如下： 
      
      intfd, on = 1; 
      … 
      此处是创建套接字等操作，出于篇幅的考虑省略
      … 
      setsockopt (fd, SOL_TCP, TCP_CORK, &on, sizeof (on));  cork 
      write (fd, …); 
      fprintf (fd, …); 
      sendfile (fd, …); 
      write (fd, …); 
      sendfile (fd, …); 
      … 
      on = 0; 
      setsockopt (fd, SOL_TCP, TCP_CORK, &on, sizeof (on));  拔去塞子 
      或者
      setsockopt(s,IPPROTO_TCP,TCP_NODELAY,(char*)&yes,sizeof(int));
      */
    /*
    语法：tcp_nodelay on | off;
    默认：tcp_nodelay on;
    配置块：http、server、location
    确定对keepalive连接是否使用TCP_NODELAY选项。 TCP_NODEALY其实就是禁用naggle算法，即使是小包也立即发送，TCP_CORK则和他相反，只有填充满后才发送
    */
    { ngx_string("tcp_nodelay"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, tcp_nodelay),
      NULL },

/*    reset_timeout_connection (感觉很多nginx源码没这个参数)
     
    语法：reset_timeout_connection on | off;
    
    默认：reset_timeout_connection off;
    
    配置块：http、server、location
    
    连接超时后将通过向客户端发送RST包来直接重置连接。这个选项打开后，Nginx会在某个连接超时后，不是使用正常情形下的四次握手关闭TCP连接，
    而是直接向用户发送RST重置包，不再等待用户的应答，直接释放Nginx服务器上关于这个套接字使用的所有缓存（如TCP滑动窗口）。相比正常的关闭方式，
    它使得服务器避免产生许多处于FIN_WAIT_1、FIN_WAIT_2、TIME_WAIT状态的TCP连接。
    
    注意，使用RST重置包关闭连接会带来一些问题，默认情况下不会开启。
*/         
    /*
    发送响应的超时时间
    语法：send_timeout time;
    默认：send_timeout 60;
    配置块：http、server、location
    这个超时时间是发送响应的超时时间，即Nginx服务器向客户端发送了数据包，但客户端一直没有去接收这个数据包。如果某个连接超过
    send_timeout定义的超时时间，那么Nginx将会关闭这个连接
    */
    { ngx_string("send_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, send_timeout),
      NULL },

    { ngx_string("send_lowat"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, send_lowat),
      &ngx_http_core_lowat_post },
/* 
clcf->postpone_output：由于处理postpone_output指令，用于设置延时输出的阈值。比如指令“postpone s”，当输出内容的size小于s， 默认1460
并且不是最后一个buffer，也不需要flush，那么就延时输出。见ngx_http_write_filter  
*/
    { ngx_string("postpone_output"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, postpone_output),
      NULL },

/* 
语法:  limit_rate rate;
默认值:  limit_rate 0;
上下文:  http, server, location, if in location

限制向客户端传送响应的速率限制。参数rate的单位是字节/秒，设置为0将关闭限速。 nginx按连接限速，所以如果某个客户端同时开启了两个连接，
那么客户端的整体速率是这条指令设置值的2倍。 

也可以利用$limit_rate变量设置流量限制。如果想在特定条件下限制响应传输速率，可以使用这个功能： 
server {

    if ($slow) {
        set $limit_rate 4k;
    }
    ...
}

此外，也可以通过“X-Accel-Limit-Rate”响应头来完成速率限制。 这种机制可以用proxy_ignore_headers指令和 fastcgi_ignore_headers指令关闭。 
*/
    { ngx_string("limit_rate"), //limit_rate限制包体的发送速度，limit_req限制连接请求连理速度
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, limit_rate),
      NULL },

/*

语法:  limit_rate_after size;
默认值:  limit_rate_after 0;
上下文:  http, server, location, if in location
 

设置不限速传输的响应大小。当传输量大于此值时，超出部分将限速传送。 
比如: 
location /flv/ {
    flv;
    limit_rate_after 500k;
    limit_rate       50k;
}
*/
    { ngx_string("limit_rate_after"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_conf_set_size_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, limit_rate_after),
      NULL },
        /*
        keepalive超时时间
        语法：keepalive_timeout time（默认单位：秒）;
        默认：keepalive_timeout 75;
        配置块：http、server、location
        一个keepalive 连接在闲置超过一定时间后（默认的是75秒），服务器和浏览器都会去关闭这个连接。当然，keepalive_timeout配置项是用
        来约束Nginx服务器的，Nginx也会按照规范把这个时间传给浏览器，但每个浏览器对待keepalive的策略有可能是不同的。
        */ //注意和ngx_http_upstream_keepalive_commands中keepalive的区别
    { ngx_string("keepalive_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE12,
      ngx_http_core_keepalive,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
/*
一个keepalive长连接上允许承载的请求最大数
语法：keepalive_requests n;
默认：keepalive_requests 100;
配置块：http、server、location
一个keepalive连接上默认最多只能发送100个请求。 设置通过一个长连接可以处理的最大请求数。 请求数超过此值，长连接将关闭。 
*/
    { ngx_string("keepalive_requests"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, keepalive_requests),
      NULL },
/*
对某些浏览器禁用keepalive功能
语法：keepalive_disable [ msie6 | safari | none ]...
默认：keepalive_disable  msie6 safari
配置块：http、server、location
HTTP请求中的keepalive功能是为了让多个请求复用一个HTTP长连接，这个功能对服务器的性能提高是很有帮助的。但有些浏览器，如IE 6和Safari，
它们对于使用keepalive功能的POST请求处理有功能性问题。因此，针对IE 6及其早期版本、Safari浏览器默认是禁用keepalive功能的。
*/
    { ngx_string("keepalive_disable"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE12,
      ngx_conf_set_bitmask_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, keepalive_disable),
      &ngx_http_core_keepalive_disable },

       /*
        相对于NGX HTTP ACCESS PHASE阶段处理方法，satisfy配置项参数的意义
        ┏━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
        ┃satisfy的参数 ┃    意义                                                                              ┃
        ┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
        ┃              ┃    NGX HTTP ACCESS PHASE阶段可能有很多HTTP模块都对控制请求的访问权限感兴趣，         ┃
        ┃              ┃那么以哪一个为准呢？当satisfy的参数为all时，这些HTTP模块必须同时发生作用，即以该阶    ┃
        ┃all           ┃                                                                                      ┃
        ┃              ┃段中全部的handler方法共同决定请求的访问权限，换句话说，这一阶段的所有handler方法必    ┃
        ┃              ┃须全部返回NGX OK才能认为请求具有访问权限                                              ┃
        ┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
        ┃              ┃  与all相反，参数为any时意味着在NGX―HTTP__ ACCESS―PHASE阶段只要有任意一个           ┃
        ┃              ┃HTTP模块认为请求合法，就不用再调用其他HTTP模块继续检查了，可以认为请求是具有访问      ┃
        ┃              ┃权限的。实际上，这时的情况有些复杂：如果其中任何一个handler方法返回NGX二OK，则认为    ┃
        ┃              ┃请求具有访问权限；如果某一个handler方法返回403戎者401，则认为请求没有访问权限，还     ┃
        ┃any           ┃                                                                                      ┃
        ┃              ┃需要检查NGX―HTTP―ACCESS―PHASE阶段的其他handler方法。也就是说，any配置项下任        ┃
        ┃              ┃何一个handler方法一旦认为请求具有访问权限，就认为这一阶段执行成功，继续向下执行；如   ┃
        ┃              ┃果其中一个handler方法认为没有访问权限，则未必以此为准，还需要检测其他的hanlder方法。  ┃
        ┃              ┃all和any有点像“&&”和“¨”的关系                                                    ┃
        ┗━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
        */
    { ngx_string("satisfy"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_enum_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, satisfy),
      &ngx_http_core_satisfy },

    /*
     internal 
     语法：internal 
     默认值：no 
     使用字段： location 
     internal指令指定某个location只能被“内部的”请求调用，外部的调用请求会返回"Not found" (404)
     “内部的”是指下列类型：
     
     ・指令error_page重定向的请求。
     ・ngx_http_ssi_module模块中使用include virtual指令创建的某些子请求。
     ・ngx_http_rewrite_module模块中使用rewrite指令修改的请求。
     
     一个防止错误页面被用户直接访问的例子：
     
     error_page 404 /404.html;
     location  /404.html {  //表示匹配/404.html的location必须uri是重定向后的uri
       internal;
     }
     */ 
     /* 该location{}必须是内部重定向(index重定向 、error_pages等重定向调用ngx_http_internal_redirect)后匹配的location{}，否则不让访问该location */
     //在location{}中配置了internal，表示匹配该uri的location{}必须是进行重定向后匹配的该location,如果不满足条件直接返回NGX_HTTP_NOT_FOUND，
     //生效地方见ngx_http_core_find_config_phase   
    { ngx_string("internal"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_core_internal,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
/*
lingering_close
语法：lingering_close off | on | always;
默认：lingering_close on;
配置块：http、server、location
该配置控制Nginx关闭用户连接的方式。always表示关闭用户连接前必须无条件地处理连接上所有用户发送的数据。off表示关闭连接时完全不管连接
上是否已经有准备就绪的来自用户的数据。on是中间值，一般情况下在关闭连接前都会处理连接上的用户发送的数据，除了有些情况下在业务上认定这之后的数据是不必要的。
*/
    { ngx_string("lingering_close"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_enum_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, lingering_close),
      &ngx_http_core_lingering_close },

    /*
    lingering_time
    语法：lingering_time time;
    默认：lingering_time 30s;
    配置块：http、server、location
    lingering_close启用后，这个配置项对于上传大文件很有用。上文讲过，当用户请求的Content-Length大于max_client_body_size配置时，
    Nginx服务会立刻向用户发送413（Request entity too large）响应。但是，很多客户端可能不管413返回值，仍然持续不断地上传HTTP body，
    这时，经过了lingering_time设置的时间后，Nginx将不管用户是否仍在上传，都会把连接关闭掉。
    */
    { ngx_string("lingering_time"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, lingering_time),
      NULL },
/*
lingering_timeout
语法：lingering_timeout time;
默认：lingering_timeout 5s;
配置块：http、server、location
lingering_close生效后，在关闭连接前，会检测是否有用户发送的数据到达服务器，如果超过lingering_timeout时间后还没有数据可读，
就直接关闭连接；否则，必须在读取完连接缓冲区上的数据并丢弃掉后才会关闭连接。
*/
    { ngx_string("lingering_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, lingering_timeout),
      NULL },

    { ngx_string("reset_timedout_connection"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, reset_timedout_connection),
      NULL },
    /*  server_name_in_redirect on | off 该配置需要配合server_name使用。在使用on打开后,表示在重定向请求时会使用
    server_name里的第一个主机名代替原先请求中的Host头部，而使用off关闭时，表示在重定向请求时使用请求本身的HOST头部 */
    { ngx_string("server_name_in_redirect"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, server_name_in_redirect),
      NULL },

    { ngx_string("port_in_redirect"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, port_in_redirect),
      NULL },

    { ngx_string("msie_padding"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, msie_padding),
      NULL },

    { ngx_string("msie_refresh"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, msie_refresh),
      NULL },

/*
文件未找到时是否记录到error日志
语法：log_not_found on | off;
默认：log_not_found on;
配置块：http、server、location
此配置项表示当处理用户请求且需要访问文件时，如果没有找到文件，是否将错误日志记录到error.log文件中。这仅用于定位问题。
*/
    { ngx_string("log_not_found"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, log_not_found),
      NULL },

    { ngx_string("log_subrequest"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, log_subrequest),
      NULL },

    /* 
    是否允许递归使用error_page
    语法：recursive_error_pages [on | off];
    默认：recursive_error_pages off;
    配置块：http、server、location
    确定是否允许递归地定义error_page。
    */
    { ngx_string("recursive_error_pages"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, recursive_error_pages),
      NULL },

/*
返回错误页面时是否在Server中注明Nginx版本
语法：server_tokens on | off;
默认：server_tokens on;
配置块：http、server、location
表示处理请求出错时是否在响应的Server头部中标明Nginx版本，这是为了方便定位问题。
*/
    { ngx_string("server_tokens"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, server_tokens),
      NULL },

/*
对If-Modified-Since头部的处理策略
语法：if_modified_since [off|exact|before];
默认：if_modified_since exact;
配置块：http、server、location
出于性能考虑，Web浏览器一般会在客户端本地缓存一些文件，并存储当时获取的时间。这样，下次向Web服务器获取缓存过的资源时，
就可以用If-Modified-Since头部把上次获取的时间捎带上，而if_modified_since将根据后面的参数决定如何处理If-Modified-Since头部。

相关参数说明如下。
off：表示忽略用户请求中的If-Modified-Since头部。这时，如果获取一个文件，那么会正常地返回文件内容。HTTP响应码通常是200。
exact：将If-Modified-Since头部包含的时间与将要返回的文件上次修改的时间做精确比较，如果没有匹配上，则返回200和文件的实际内容，如果匹配上，
则表示浏览器缓存的文件内容已经是最新的了，没有必要再返回文件从而浪费时间与带宽了，这时会返回304 Not Modified，浏览器收到后会直接读取自己的本地缓存。
before：是比exact更宽松的比较。只要文件的上次修改时间等于或者早于用户请求中的If-Modified-Since头部的时间，就会向客户端返回304 Not Modified。
*/ //生效见ngx_http_test_if_modified
    { ngx_string("if_modified_since"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_enum_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, if_modified_since),
      &ngx_http_core_if_modified_since },

    { ngx_string("max_ranges"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, max_ranges),
      NULL },

    { ngx_string("chunked_transfer_encoding"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, chunked_transfer_encoding),
      NULL },

    //生效见ngx_http_set_etag
    { ngx_string("etag"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, etag),
      NULL },

    /*
    根据HTTP返回码重定向页面
    语法：error_page code [ code... ] [ = | =answer-code ] uri | @named_location
    配置块：http、server、location、if 
    
    当对于某个请求返回错误码时，如果匹配上了error_page中设置的code，则重定向到新的URI中。例如：
    error_page   404          /404.html;
    error_page   502 503 504  /50x.html;
    error_page   403          http://example.com/forbidden.html;
    error_page   404          = @fetch;
    
    注意，虽然重定向了URI，但返回的HTTP错误码还是与原来的相同。用户可以通过“=”来更改返回的错误码，例如：
    error_page 404 =200 /empty.gif;
    error_page 404 =403 /forbidden.gif;
    
    也可以不指定确切的返回错误码，而是由重定向后实际处理的真实结果来决定，这时，只要把“=”后面的错误码去掉即可，例如：
    error_page 404 = /empty.gif;
    
    如果不想修改URI，只是想让这样的请求重定向到另一个location中进行处理，那么可以这样设置：
    location / (
        error_page 404 @fallback;
    )
     
    location @fallback (
        proxy_pass http://backend;
    )
    
    这样，返回404的请求会被反向代理到http://backend上游服务器中处理
    */ //try_files和error_page都有重定向功能  //error_page错误码必须must be between 300 and 599，并且不能为499，见ngx_http_core_error_page
    { ngx_string("error_page"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_2MORE,
      ngx_http_core_error_page,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    /*
    
    语法：try_files path1 [path2] uri;
    配置块：server、location   

    try_files后要跟若干路径，如path1 path2...，而且最后必须要有uri参数，意义如下：尝试按照顺序访问每一个path，如果可以有效地读取，
    就直接向用户返回这个path对应的文件结束请求，否则继续向下访问。如果所有的path都找不到有效的文件，就重定向到最后的参数uri上。因此，
    最后这个参数uri必须存在，而且它应该是可以有效重定向的。例如：
    try_files /system/maintenance.html $uri $uri/index.html $uri.html @other;
    location @other {
      proxy_pass http://backend;
    }
    
    上面这段代码表示如果前面的路径，如/system/maintenance.html等，都找不到，就会反向代理到http://backend服务上。还可以用指定错误码的方式与error_page配合使用，例如：
    location / {
      try_files $uri $uri/ /error.php?c=404 =404;
    }
    */ //try_files和error_page都有重定向功能
    { ngx_string("try_files"),  //注意try_files至少包含两个参数，否则解析配置文件会出错
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_2MORE,
      ngx_http_core_try_files,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("post_action"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, post_action),
      NULL },
    
//    error_log file [ debug | info | notice | warn | error | crit ] 
    { ngx_string("error_log"), //ngx_errlog_module中的error_log配置只能全局配置，ngx_http_core_module在http{} server{} local{}中配置
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_core_error_log,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

/*
打开文件缓存

语法：open_file_cache max = N [inactive = time] | off;

默认：open_file_cache off;

配置块：http、server、location

文件缓存会在内存中存储以下3种信息：

文件句柄、文件大小和上次修改时间。

已经打开过的目录结构。

没有找到的或者没有权限操作的文件信息。

这样，通过读取缓存就减少了对磁盘的操作。

该配置项后面跟3种参数。
max：表示在内存中存储元素的最大个数。当达到最大限制数量后，将采用LRU（Least Recently Used）算法从缓存中淘汰最近最少使用的元素。
inactive：表示在inactive指定的时间段内没有被访问过的元素将会被淘汰。默认时间为60秒。
off：关闭缓存功能。
例如：
open_file_cache max=1000 inactive=20s; //如果20s内有请求到该缓存，则该缓存继续生效，如果20s内都没有请求该缓存，则20s外请求，会重新获取原文件并生成缓存
*/

/*
   注意open_file_cache inactive=20s和fastcgi_cache_valid 20s的区别，前者指的是如果客户端在20s内没有请求到来，则会把该缓存文件对应的fd stat属性信息
   从ngx_open_file_cache_t->rbtree(expire_queue)中删除(客户端第一次请求该uri对应的缓存文件的时候会把该文件对应的stat信息节点ngx_cached_open_file_s添加到
   ngx_open_file_cache_t->rbtree(expire_queue)中)，从而提高获取缓存文件的效率
   fastcgi_cache_valid指的是何时缓存文件过期，过期则删除，定时执行ngx_cache_manager_process_handler->ngx_http_file_cache_manager
*/

/* 
   如果没有配置open_file_cache max=1000 inactive=20s;，也就是说没有缓存cache缓存文件对应的文件stat信息，则每次都要从新打开文件获取文件stat信息，
   如果有配置open_file_cache，则会把打开的cache缓存文件stat信息按照ngx_crc32_long做hash后添加到ngx_cached_open_file_t->rbtree中，这样下次在请求该
   uri，则就不用再次open文件后在stat获取文件属性了，这样可以提高效率,参考ngx_open_cached_file 

   创建缓存文件stat节点node后，每次新来请求的时候都会更新accessed时间，因此只要inactive时间内有请求，就不会删除缓存stat节点，见ngx_expire_old_cached_files
   inactive时间内没有新的请求则会从红黑树中删除该节点，同时关闭该文件见ngx_open_file_cleanup  ngx_close_cached_file  ngx_expire_old_cached_files
   */ //可以参考ngx_open_file_cache_t  参考ngx_open_cached_file 
   
    { ngx_string("open_file_cache"), 
//open_file_cache inactive 30主要用于是否在30s内有新请求，没有则删除缓存，而open_file_cache_min_uses表示只要缓存在红黑树中，并且遍历该文件次数达到指定次数，则不会close文件，也就不会从新获取stat信息
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE12,
      ngx_http_core_open_file_cache,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, open_file_cache),
      NULL },

/*
检验缓存中元素有效性的频率
语法：open_file_cache_valid time;
默认：open_file_cache_valid 60s;
配置块：http、server、location
设置检查open_file_cache缓存stat信息的元素的时间间隔。 
*/ 
//表示60s后来的第一个请求要对文件stat信息做一次检查，检查是否发送变化，如果发送变化则从新获取文件stat信息或者从新创建该阶段，
    //生效在ngx_open_cached_file中的(&& now - file->created < of->valid ) 
    { ngx_string("open_file_cache_valid"), 
    //open_file_cache_min_uses后者是判断是否需要close描述符，然后重新打开获取fd和stat信息，open_file_cache_valid只是定期对stat(超过该配置时间后，在来一个的时候会进行判断)进行更新
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_sec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, open_file_cache_valid),
      NULL },

/*
不被淘汰的最小访问次数
语法：open_file_cache_min_uses number;
默认：open_file_cache_min_uses 1;
配置块：http、server、location
 
设置在由open_file_cache指令的inactive参数配置的超时时间内，文件应该被访问的最小number(次数)。如果访问次数大于等于此值，文件描
述符会保留在缓存中，否则从缓存中删除。 
*/  //例如open_file_cache max=102400 inactive=20s; 只要该缓存文件被遍历次数超过open_file_cache_min_uses次请求，则缓存中的文件更改信息不变,不会close文件
    //这时候的情况是:请求带有If-Modified-Since，得到的是304且Last-Modified时间没变
/*
file->uses >= min_uses表示只要在inactive时间内该ngx_cached_open_file_s file节点被遍历到的次数达到min_uses次，则永远不会关闭文件(也就是不用重新获取文件stat信息)，
除非该cache node失效，缓存超时inactive后会从红黑树中删除该file node节点，同时关闭文件等见ngx_open_file_cleanup  ngx_close_cached_file  ngx_expire_old_cached_files
*/    { ngx_string("open_file_cache_min_uses"), 
//只要缓存匹配次数达到这么多次，就不会重新关闭close该文件缓存，下次也就不会从新打开文件获取文件描述符，除非缓存时间inactive内都没有新请求，则会删除节点并关闭文件
//open_file_cache inactive 30主要用于是否在30s内有新请求，没有则删除缓存，而open_file_cache_min_uses表示只要缓存在红黑树中，并且遍历该文件次数达到指定次数，则不会close文件，也就不会从新获取stat信息

//open_file_cache_min_uses后者是判断是否需要close描述符，然后重新打开获取fd和stat信息，open_file_cache_valid只是定期对stat(超过该配置时间后，在来一个的时候会进行判断)进行更新

      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, open_file_cache_min_uses),
      NULL },

/*
是否缓存打开文件错误的信息
语法：open_file_cache_errors on | off;
默认：open_file_cache_errors off;
配置块：http、server、location
此配置项表示是否在文件缓存中缓存打开文件时出现的找不到路径、没有权限等错误信息。
*/
    { ngx_string("open_file_cache_errors"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, open_file_cache_errors),
      NULL },

    { ngx_string("open_file_cache_events"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, open_file_cache_events),
      NULL },

/*
DNS解析地址
语法：resolver address ...;
配置块：http、server、location
设置DNS名字解析服务器的地址，例如：
resolver 127.0.0.1 192.0.2.1;
*/
    { ngx_string("resolver"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_core_resolver,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

/*
DNS解析的超时时间
语法：resolver_timeout time;
默认：resolver_timeout 30s;
配置块：http、server、location
此配置项表示DNS解析的超时时间。
*/ //产考:http://theantway.com/2013/09/understanding_the_dns_resolving_in_nginx/         Nginx的DNS解析过程分析 
    { ngx_string("resolver_timeout"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_msec_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, resolver_timeout),
      NULL },

#if (NGX_HTTP_GZIP)

    { ngx_string("gzip_vary"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, gzip_vary),
      NULL },

    { ngx_string("gzip_http_version"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_enum_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, gzip_http_version),
      &ngx_http_gzip_http_version },

    { ngx_string("gzip_proxied"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_conf_set_bitmask_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_core_loc_conf_t, gzip_proxied),
      &ngx_http_gzip_proxied_mask },

    { ngx_string("gzip_disable"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_1MORE,
      ngx_http_gzip_disable,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

#endif

#if (NGX_HAVE_OPENAT)

/*
语法:  disable_symlinks off;
disable_symlinks on | if_not_owner [from=part];
 
默认值:  disable_symlinks off; 
上下文:  http, server, location
 
决定nginx打开文件时如何处理符号链接： 

off
默认行为，允许路径中出现符号链接，不做检查。 
on
如果文件路径中任何组成部分中含有符号链接，拒绝访问该文件。 
if_not_owner
如果文件路径中任何组成部分中含有符号链接，且符号链接和链接目标的所有者不同，拒绝访问该文件。 
from=part
当nginx进行符号链接检查时(参数on和参数if_not_owner)，路径中所有部分默认都会被检查。而使用from=part参数可以避免对路径开始部分进行符号链接检查，
而只检查后面的部分路径。如果某路径不是以指定值开始，整个路径将被检查，就如同没有指定这个参数一样。如果某路径与指定值完全匹配，将不做检查。这
个参数的值可以包含变量。 

比如： 
disable_symlinks on from=$document_root;
这条指令只在有openat()和fstatat()接口的系统上可用。当然，现在的FreeBSD、Linux和Solaris都支持这些接口。 
参数on和if_not_owner会带来处理开销。 
只在那些不支持打开目录查找文件的系统中，使用这些参数需要工作进程有这些被检查目录的读权限。 
*/
    { ngx_string("disable_symlinks"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE12,
      ngx_http_disable_symlinks,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

#endif

      ngx_null_command
};

/*
任何一个HTTP模块的server相关的配置项都是可能出现在main级别中，而location相关的配置项可能出现在main、srv级别中


main server location配置定义如下:
1.main配置项:只能在http{}内 server外的配置。例如 http{aaa; server{ location{} }}   aaa为main配置项
2.server配置项:可以在http内，server外配置，也可以在server内配置。 例如 http{bbb; server{bbb; location{} }}   bbb为server配置项
2.server配置项:可以在http内，server外配置，也可以在server内配置,也可以出现在location中。 例如 http{ccc; server{ccc; location{ccc} }}   ccc为location配置项 

这样区分main srv local_create的原因是:
例如
http {
    sss;
    xxx; 
    server {
        sss;
        xxx; 
        location {
            xxx;
        } 
    }
},
其中的xxx可以同时出现在http server 和location中，所以在解析到http{}行的时候需要创建main来存储NGX_HTTP_MAIN_CONF配置。
那么为什么还需要调用sev和loc对应的create呢?
因为server类配置可能同时出现在main中，所以需要存储这写配置，所以要创建srv来存储他们，就是上面的sss配置。
同理location类配置可能同时出现在main中，所以需要存储这写配置，所以要创建loc来存储他们，就是上面的sss配置。

在解析到server{}的时候，由于location配置也可能出现在server{}内，就是上面server{}中的xxx;所以解析到server{}的时候
需要调动srv和loc create;

所以最终要决定使用那个sss配置和xxx配置，这就需要把http和server的sss合并， http、server和location中的xxx合并
*/
static ngx_http_module_t  ngx_http_core_module_ctx = {
    ngx_http_core_preconfiguration,        /* preconfiguration */ //在解析http{}内的配置项前回调
    ngx_http_core_postconfiguration,       /* postconfiguration */ //解析完http{}内的所有配置项后回调

    ////解析到http{}行时，在ngx_http_block执行。该函数创建的结构体成员只能出现在http中，不会出现在server和location中
    ngx_http_core_create_main_conf,        /* create main configuration */
    //http{}的所有项解析完后执行
    ngx_http_core_init_main_conf,          /* init main configuration */ //解析完main配置项后回调

    //解析server{}   local{}项时，会执行
    //创建用于存储可同时出现在main、srv级别配置项的结构体，该结构体中的成员与server配置是相关联的
    ngx_http_core_create_srv_conf,         /* create server configuration */
    /* merge_srv_conf方法可以把出现在main级别中的配置项值合并到srv级别配置项中 */
    ngx_http_core_merge_srv_conf,          /* merge server configuration */

    //解析到http{}  server{}  local{}行时，会执行
    //创建用于存储可同时出现在main、srv、loc级别配置项的结构体，该结构体中的成员与location配置是相关联的
    ngx_http_core_create_loc_conf,         /* create location configuration */
    //把出现在main、srv级别的配置项值合并到loc级别的配置项中
    ngx_http_core_merge_loc_conf           /* merge location configuration */
};

//在解析到http{}行的时候，会根据ngx_http_block来执行ngx_http_core_module_ctx中的相关create来创建存储配置项目的空间
ngx_module_t  ngx_http_core_module = { //http{}内部 和server location都属于这个模块，他们的main_create  srv_create loc_ctreate都是一样的
//http{}相关配置结构创建首先需要执行ngx_http_core_module，而后才能执行对应的http子模块，这里有个顺序关系在里面。因为
//ngx_http_core_loc_conf_t ngx_http_core_srv_conf_t ngx_http_core_main_conf_t的相关
    NGX_MODULE_V1,
    &ngx_http_core_module_ctx,               /* module context */
    ngx_http_core_commands,                  /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


ngx_str_t  ngx_http_core_get_method = { 3, (u_char *) "GET " };

//ngx_http_process_request->ngx_http_handler->ngx_http_core_run_phases
//ngx_http_run_posted_requests->ngx_http_handler
void
ngx_http_handler(ngx_http_request_t *r) /* 执行11个阶段的指定阶段 */
{
    ngx_http_core_main_conf_t  *cmcf;

    r->connection->log->action = NULL;

    r->connection->unexpected_eof = 0;

/*
    检查ngx_http_request_t结构体的internal标志位，如果internal为0，则从头部phase_handler执行；如果internal标志位为1，则表示请求当前需要做内部跳转，
将要把结构体中的phase_handler序号置为server_rewrite_index。注意ngx_http_phase_engine_t结构体中的handlers动态数组中保存了请求需要经历的所有
回调方法，而server_rewrite_index则是handlers数组中NGX_HTTP_SERVER_REWRITE_PHASE处理阶段的第一个ngx_http_phase_handler_t回调方法所处的位置。
    究竟handlers数组是怎么使用的呢？事实上，它要配合着ngx_http_request_t结构体的phase_handler序号使用，由phase_handler指定着请求将要执行
的handlers数组中的方法位置。注意，handlers数组中的方法都是由各个HTTP模块实现的，这就是所有HTTP模块能够共同处理请求的原因。 
 */
    if (!r->internal) {
        
        switch (r->headers_in.connection_type) {
        case 0:
            r->keepalive = (r->http_version > NGX_HTTP_VERSION_10); //指明在1.0以上版本默认是长连接
            break;

        case NGX_HTTP_CONNECTION_CLOSE:
            r->keepalive = 0;
            break;

        case NGX_HTTP_CONNECTION_KEEP_ALIVE:
            r->keepalive = 1;
            break;
        }
    
        r->lingering_close = (r->headers_in.content_length_n > 0
                              || r->headers_in.chunked); 
        /*
       当internal标志位为0时，表示不需要重定向（如刚开始处理请求时），将phase_handler序号置为0，意味着从ngx_http_phase_engine_t指定数组
       的第一个回调方法开始执行（了解ngx_http_phase_engine_t是如何将各HTTP模块的回调方法构造成handlers数组的）。
          */
        r->phase_handler = 0;

    } else {
/* 
在这一步骤中，把phase_handler序号设为server_rewrite_index，这意味着无论之前执行到哪一个阶段，马上都要重新从NGX_HTTP_SERVER_REWRITE_PHASE
阶段开始再次执行，这是Nginx的请求可以反复rewrite重定向的基础。
*/
        cmcf = ngx_http_get_module_main_conf(r, ngx_http_core_module);
        r->phase_handler = cmcf->phase_engine.server_rewrite_index;
    }

    r->valid_location = 1;
#if (NGX_HTTP_GZIP)
    r->gzip_tested = 0;
    r->gzip_ok = 0;
    r->gzip_vary = 0;
#endif

    r->write_event_handler = ngx_http_core_run_phases;
    ngx_http_core_run_phases(r);  
}

/*  
    每个ngx_http_phases阶段对应的checker函数，处于同一个阶段的checker函数相同，见ngx_http_init_phase_handlers
    NGX_HTTP_SERVER_REWRITE_PHASE  -------  ngx_http_core_rewrite_phase
    NGX_HTTP_FIND_CONFIG_PHASE     -------  ngx_http_core_find_config_phase
    NGX_HTTP_REWRITE_PHASE         -------  ngx_http_core_rewrite_phase
    NGX_HTTP_POST_REWRITE_PHASE    -------  ngx_http_core_post_rewrite_phase
    NGX_HTTP_ACCESS_PHASE          -------  ngx_http_core_access_phase
    NGX_HTTP_POST_ACCESS_PHASE     -------  ngx_http_core_post_access_phase
    NGX_HTTP_TRY_FILES_PHASE       -------  NGX_HTTP_TRY_FILES_PHASE
    NGX_HTTP_CONTENT_PHASE         -------  ngx_http_core_content_phase
    其他几个阶段                   -------  ngx_http_core_generic_phase

    HTTP框架为11个阶段实现的checker方法  赋值见ngx_http_init_phase_handlers
┏━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━┓
┃    阶段名称                  ┃    checker方法                   ┃
┏━━━━━━━━━━━━━━━┳━━━━━━━━━━━━━━━━━┓
┃   NGX_HTTP_POST_READ_PHASE   ┃    ngx_http_core_generic_phase   ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP SERVER REWRITE PHASE ┃ngx_http_core_rewrite_phase       ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP FIND CONFIG PHASE    ┃ngx_http_core find config_phase   ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP REWRITE PHASE        ┃ngx_http_core_rewrite_phase       ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP POST REWRITE PHASE   ┃ngx_http_core_post_rewrite_phase  ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP PREACCESS PHASE      ┃ngx_http_core_generic_phase       ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP ACCESS PHASE         ┃ngx_http_core_access_phase        ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP POST ACCESS PHASE    ┃ngx_http_core_post_access_phase   ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP TRY FILES PHASE      ┃ngx_http_core_try_files_phase     ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP CONTENT PHASE        ┃ngx_http_core_content_phase       ┃
┣━━━━━━━━━━━━━━━╋━━━━━━━━━━━━━━━━━┫
┃NGX HTTP LOG PHASE            ┃ngx_http_core_generic_phase       ┃
┗━━━━━━━━━━━━━━━┻━━━━━━━━━━━━━━━━━┛
*/
/*
通常来说，在接收完HTTP头部后，是无法在一次Nginx框架的调度中处理完一个请求的。在第一次接收完HTTP头部后，HTTP框架将调度
ngx_http_process_request方法开始处理请求，如果某个checker方法返回了NGX_OK，则将会把控制权交还给Nginx框架。当这个请求
上对应的事件再次触发时，HTTP框架将不会再调度ngx_http_process_request方法处理请求，而是由ngx_http_request_handler方法
开始处理请求。例如recv虽然把头部行内容读取完毕，并能解析完成，但是可能有携带请求内容，内容可能没有读完
*/
//通过执行当前r->phase_handler所指向的阶段的checker函数
//ngx_http_process_request->ngx_http_handler->ngx_http_core_run_phases
void
ngx_http_core_run_phases(ngx_http_request_t *r) //执行该请求对于的阶段的checker(),并获取返回值
{
    ngx_int_t                   rc;
    ngx_http_phase_handler_t   *ph;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_get_module_main_conf(r, ngx_http_core_module);

    ph = cmcf->phase_engine.handlers;

    while (ph[r->phase_handler].checker) { //处于同一ngx_http_phases阶段的所有ngx_http_phase_handler_t的checker指向相同的函数，见ngx_http_init_phase_handlers
/*
handler方法其实仅能在checker方法中被调用，而且checker方法由HTTP框架实现，所以可以控制各HTTP模块实现的处理方法在不同的阶段中采用不同的调用行为

ngx_http_request_t结构体中的phase_handler成员将决定执行到哪一阶段，以及下一阶段应当执行哪个HTTP模块实现的内容。可以看到请求的phase_handler成员
会被重置，而HTTP框架实现的checker穷法也会修改phase_handler成员的值

当checker方法的返回值非NGX_OK时，意味着向下执行phase_engine中的各处理方法；反之，当任何一个checker方法返回NGX_OK时，意味着把控制权交还
给Nginx的事件模块，由它根据事件（网络事件、定时器事件、异步I/O事件等）再次调度请求。然而，一个请求多半需要Nginx事件模块多次地调度HTTP模
块处理，也就是在该函数外设置的读/写事件的回调方法ngx_http_request_handler
*/
        
        rc = ph[r->phase_handler].checker(r, &ph[r->phase_handler]);

 /* 直接返回NGX OK会使待HTTP框架立刻把控制权交还给epoll事件框架，不再处理当前请求，唯有这个请求上的事件再次被触发才会继续执行。*/
        if (rc == NGX_OK) { //执行phase_handler阶段的hecker  handler方法后，返回值为NGX_OK，则直接退出，否则继续循环执行checker handler
            return;
        }
    }
}

const char* ngx_http_phase_2str(ngx_uint_t phase)  
{
    static char buf[56];
    
    switch(phase)
    {
        case NGX_HTTP_POST_READ_PHASE:
            return "NGX_HTTP_POST_READ_PHASE";

        case NGX_HTTP_SERVER_REWRITE_PHASE:
            return "NGX_HTTP_SERVER_REWRITE_PHASE"; 

        case NGX_HTTP_FIND_CONFIG_PHASE:
            return "NGX_HTTP_FIND_CONFIG_PHASE";

        case NGX_HTTP_REWRITE_PHASE:
            return "NGX_HTTP_REWRITE_PHASE";

        case NGX_HTTP_POST_REWRITE_PHASE:
            return "NGX_HTTP_POST_REWRITE_PHASE";

        case NGX_HTTP_PREACCESS_PHASE:
            return "NGX_HTTP_PREACCESS_PHASE"; 

        case NGX_HTTP_ACCESS_PHASE:
            return "NGX_HTTP_ACCESS_PHASE";

        case NGX_HTTP_POST_ACCESS_PHASE:
            return "NGX_HTTP_POST_ACCESS_PHASE";

        case NGX_HTTP_TRY_FILES_PHASE:
            return "NGX_HTTP_TRY_FILES_PHASE";

        case NGX_HTTP_CONTENT_PHASE:
            return "NGX_HTTP_CONTENT_PHASE"; 

        case NGX_HTTP_LOG_PHASE:
            return "NGX_HTTP_LOG_PHASE";
    }

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "error phase:%u", (unsigned int)phase);
    return buf;
}


/*
//NGX_HTTP_POST_READ_PHASE   NGX_HTTP_PREACCESS_PHASE  NGX_HTTP_LOG_PHASE默认都是该函数盼段下HTTP模块的ngx_http_handler_pt方法返回值意义
┏━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃    返回值    ┃    意义                                                                          ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃              ┃  执行下一个ngx_http_phases阶段中的第一个ngx_http_handler_pt处理方法。这意味着两  ┃
┃              ┃点：①即使当前阶段中后续还有一曲HTTP模块设置了ngx_http_handler_pt处理方法，返回   ┃
┃NGX_OK        ┃                                                                                  ┃
┃              ┃NGX_OK之后它们也是得不到执行机会的；②如果下一个ngx_http_phases阶段中没有任何     ┃
┃              ┃HTTP模块设置了ngx_http_handler_pt处理方法，将再次寻找之后的阶段，如此循环下去     ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃NGX_DECLINED  ┃  按照顺序执行下一个ngx_http_handler_pt处理方法。这个顺序就是ngx_http_phase_      ┃
┃              ┃engine_t中所有ngx_http_phase_handler_t结构体组成的数组的顺序                      ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃NGX_AGAIN     ┃  当前的ngx_http_handler_pt处理方法尚未结束，这意味着该处理方法在当前阶段有机会   ┃
┃              ┃再次被调用。这时一般会把控制权交还给事件模块，当下次可写事件发生时会再次执行到该  ┃
┣━━━━━━━┫                                                                                  ┃
┃NGX_DONE      ┃ngx_http_handler_pt处理方法                                                       ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃ NGX_ERROR    ┃                                                                                  ┃
┃              ┃  需要调用ngx_http_finalize_request结束请求                                       ┃
┣━━━━━━━┫                                                                                  ┃
┃其他          ┃                                                                                  ┃
┗━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛
*/ 
/* 
有3个HTTP阶段都使用了ngx_http_core_generic_phase作为它们的checker方法，这意味着任何试图在NGX_HTTP_POST_READ_PHASE   NGX_HTTP_PREACCESS_PHASE  
NGX_HTTP_LOG_PHASE这3个阶段处理请求的HTTP模块都需要了解ngx_http_core_generic_phase方法 
*/ //所有阶段的checker在ngx_http_core_run_phases中调用
//NGX_HTTP_POST_READ_PHASE   NGX_HTTP_PREACCESS_PHASE  NGX_HTTP_LOG_PHASE默认都是该函数
//当HTTP框架在建立的TCP连接上接收到客户发送的完整HTTP请求头部时，开始执行NGX_HTTP_POST_READ_PHASE阶段的checker方法
ngx_int_t
ngx_http_core_generic_phase(ngx_http_request_t *r, ngx_http_phase_handler_t *ph)
{
    ngx_int_t  rc;

    /*
     * generic phase checker,
     * used by the post read and pre-access phases
     */

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "generic phase: %ui (%s)", r->phase_handler, ngx_http_phase_2str(ph->phase));

    rc = ph->handler(r); //调用这一阶段中各HTTP模块添加的handler处理方法

    if (rc == NGX_OK) {//如果handler方法返回NGX OK，之后将进入下一个阶段处理，而不佥理会当前阶段中是否还有其他的处理方法
        r->phase_handler = ph->next; //直接指向下一个处理阶段的第一个方法
        return NGX_AGAIN;
    }

//如果handler方法返回NGX_DECLINED，那么将进入下一个处理方法，这个处理方法既可能属于当前阶段，也可能属于下一个阶段。注意返回
//NGX_OK与NGX_DECLINED之间的区别
    if (rc == NGX_DECLINED) {
        r->phase_handler++;//紧接着的下一个处理方法
        return NGX_AGAIN;
    }
/*
如果handler方法返回NGX_AGAIN或者NGX_DONE，则意味着刚才的handler方法无法在这一次调度中处理完这一个阶段，它需要多次调度才能完成，
也就是说，刚刚执行过的handler方法希望：如果请求对应的事件再次被触发时，将由ngx_http_request_handler通过ngx_http_core_ run_phases再次
调用这个handler方法。直接返回NGX_OK会使待HTTP框架立刻把控制权交还给epoll事件框架，不再处理当前请求，唯有这个请求上的事件再次被触发才会继续执行。
*/
//如果handler方法返回NGX_AGAIN或者NGX_DONE，那么当前请求将仍然停留在这一个处理阶段中
    if (rc == NGX_AGAIN || rc == NGX_DONE) { //phase_handler没有发生变化，当这个请求上的事件再次触发的时候继续在该阶段执行
        return NGX_OK;
    }

    /* rc == NGX_ERROR || rc == NGX_HTTP_...  */
    //如果handler方法返回NGX_ERROR或者类似NGX_HTTP开头的返回码，则调用ngx_http_finalize_request结束请求
    ngx_http_finalize_request(r, rc);

    return NGX_OK;
}

/*
NGX_HTTP_SERVER_REWRITE_PHASE  NGX_HTTP_REWRITE_PHASE阶段的checker方法是ngx_http_core_rewrite_phase。表10-2总结了该阶段
下ngx_http_handler_pt处理方法的返回值是如何影响HTTP框架执行的，注意，这个阶段中不存在返回值可以使请求直接跳到下一个阶段执行。
NGX_HTTP_REWRITE_PHASE  NGX_HTTP_POST_REWRITE_PHASE阶段HTTP模块的ngx_http_handler_pt方法返回值意义
┏━━━━━━━┳━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓
┃    返回值    ┃    意义                                                                            ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃              ┃  当前的ngx_http_handler_pt处理方法尚未结束，这意味着该处理方法在当前阶段中有机会   ┃
┃NGX DONE      ┃                                                                                    ┃
┃              ┃再次被调用                                                                          ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃              ┃  当前ngx_http_handler_pt处理方法执行完毕，按照顺序执行下一个ngx_http_handler_pt处  ┃
┃NGX DECLINED  ┃                                                                                    ┃
┃              ┃理方法                                                                              ┃
┣━━━━━━━╋━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┫
┃NGX AGAIN     ┃                                                                                    ┃
┣━━━━━━━┫                                                                                    ┃
┃NGX DONE      ┃                                                                                    ┃
┃              ┃  需要调用ngx_http_finalize_request结束请求                                         ┃
┣━━━━━━━┫                                                                                    ┃
┃ NGX ERROR    ┃                                                                                    ┃
┣━━━━━━━┫                                                                                    ┃
┃其他          ┃                                                                                    ┃
┗━━━━━━━┻━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛

*/ //所有阶段的checker在ngx_http_core_run_phases中调用
ngx_int_t
ngx_http_core_rewrite_phase(ngx_http_request_t *r, ngx_http_phase_handler_t *ph) 
{
    ngx_int_t  rc;

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "rewrite phase: %ui (%s)", r->phase_handler, ngx_http_phase_2str(ph->phase));

    rc = ph->handler(r);//ngx_http_rewrite_handler

/* 将phase_handler加1表示将要执行下一个回调方法。注意，此时返回的是NGX AGAIN，HTTP框架不会把进程控制权交还给epoll事件框架，而
是继续立刻执行请求的下一个回调方法。 */
    if (rc == NGX_DECLINED) {
        r->phase_handler++;
        return NGX_AGAIN;
    }

    /*
     如果handler方法返回NGX_DONE，则意味着刚才的handler方法无法在这一次调度中处理完这一个阶段，它需要多次的调度才能完成。注意，此
     时返回NGX_OK，它会使得HTTP框架立刻把控制权交还给epoll等事件模块，不再处理当前请求，唯有这个请求上的事件再次被触发时才会继续执行。
     */
    if (rc == NGX_DONE) { //phase_handler没有发生变化，因此如果该请求的事件再次触发，还会接着上次的handler执行
        return NGX_OK;
    }

    /*
    为什么该checker执行handler没有NGX_DECLINED(r- >phase_handler  =  ph- >next) ?????
    答:ngx_http_core_rewrite_phase方法与ngx_http_core_generic_phase方法有一个显著的不同点：前者永远不会导致跨过同
一个HTTP阶段的其他处理方法，就直接跳到下一个阶段来处理请求。原因其实很简单，可能有许多HTTP模块在NGX_HTTP_REWRITE_PHASE和
NGX_HTTP_POST_REWRITE_PHASE阶段同时处理重写URL这样的业务，HTTP框架认为这两个盼段的HTTP模块是完全平等的，序号靠前的HTTP模块优先
级并不会更高，它不能决定序号靠后的HTTP模块是否可以再次重写URL。因此，ngx_http_core_rewrite_phase方法绝对不会把phase_handler直接
设置到下一个阶段处理方法的流程中，即不可能存在类似下面的代码: r- >phase_handler  =  ph- >next ;
     */
    

    /* NGX_OK, NGX_AGAIN, NGX_ERROR, NGX_HTTP_...  */

    ngx_http_finalize_request(r, rc);

    return NGX_OK;
}


/*
NGXHTTP―FIND―CONFIG―PHASE阶段上不能挂载任何回调函数，因为它们永远也不会被执行，该阶段完成的是Nginx的特定任务，即进行Location定位
*/
//所有阶段的checker在ngx_http_core_run_phases中调用
ngx_int_t
ngx_http_core_find_config_phase(ngx_http_request_t *r,
    ngx_http_phase_handler_t *ph)
{
    u_char                    *p;
    size_t                     len;
    ngx_int_t                  rc;
    ngx_http_core_loc_conf_t  *clcf;
    
    
    r->content_handler = NULL; //首先初始化content_handler，这个会在ngx_http_core_content_phase里面使用
    r->uri_changed = 0;

    char buf[NGX_STR2BUF_LEN];
    ngx_str_t_2buf(buf, &r->uri);    
    
    ngx_log_debug3(NGX_LOG_DEBUG_HTTP, r->connection->log, 0, "find config phase: %ui (%s), uri:%s", r->phase_handler, 
        (char*)ngx_http_phase_2str(ph->phase), buf);

    rc = ngx_http_core_find_location(r);//解析完HTTP{}块后，ngx_http_init_static_location_trees函数会创建一颗三叉树，以加速配置查找。
	//找到所属的location，并且loc_conf也已经更新了r->loc_conf了。

    if (rc == NGX_ERROR) {
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return NGX_OK;
    }

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);//用刚找到的loc_conf，得到其http_core模块的位置配置。

    /* 该location{}必须是内部重定向(index重定向 、error_pages等重定向调用ngx_http_internal_redirect)后匹配的location{}，否则不让访问该location */
    if (!r->internal && clcf->internal) { //是否是i在内部重定向，如果是，中断吗 