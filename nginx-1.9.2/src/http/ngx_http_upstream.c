
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


#if (NGX_HTTP_CACHE)
static ngx_int_t ngx_http_upstream_cache(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_cache_get(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_http_file_cache_t **cache);
static ngx_int_t ngx_http_upstream_cache_send(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_cache_status(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_cache_last_modified(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_cache_etag(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
#endif

static void ngx_http_upstream_init_request(ngx_http_request_t *r);
static void ngx_http_upstream_resolve_handler(ngx_resolver_ctx_t *ctx);
static void ngx_http_upstream_rd_check_broken_connection(ngx_http_request_t *r);
static void ngx_http_upstream_wr_check_broken_connection(ngx_http_request_t *r);
static void ngx_http_upstream_check_broken_connection(ngx_http_request_t *r,
    ngx_event_t *ev);
static void ngx_http_upstream_connect(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_reinit(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_send_request(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_uint_t do_write);
static ngx_int_t ngx_http_upstream_send_request_body(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_uint_t do_write);
static void ngx_http_upstream_send_request_handler(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_read_request_handler(ngx_http_request_t *r);
static void ngx_http_upstream_process_header(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_test_next(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_intercept_errors(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static ngx_int_t ngx_http_upstream_test_connect(ngx_connection_t *c);
static ngx_int_t ngx_http_upstream_process_headers(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_process_body_in_memory(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_send_response(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_upgrade(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_upgraded_read_downstream(ngx_http_request_t *r);
static void ngx_http_upstream_upgraded_write_downstream(ngx_http_request_t *r);
static void ngx_http_upstream_upgraded_read_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_upgraded_write_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_process_upgraded(ngx_http_request_t *r,
    ngx_uint_t from_upstream, ngx_uint_t do_write);
static void
    ngx_http_upstream_process_non_buffered_downstream(ngx_http_request_t *r);
static void
    ngx_http_upstream_process_non_buffered_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void
    ngx_http_upstream_process_non_buffered_request(ngx_http_request_t *r,
    ngx_uint_t do_write);
static ngx_int_t ngx_http_upstream_non_buffered_filter_init(void *data);
static ngx_int_t ngx_http_upstream_non_buffered_filter(void *data,
    ssize_t bytes);
static void ngx_http_upstream_process_downstream(ngx_http_request_t *r);
static void ngx_http_upstream_process_upstream(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_process_request(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_store(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_dummy_handler(ngx_http_request_t *r,
    ngx_http_upstream_t *u);
static void ngx_http_upstream_next(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_uint_t ft_type);
static void ngx_http_upstream_cleanup(void *data);
static void ngx_http_upstream_finalize_request(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_int_t rc);

static ngx_int_t ngx_http_upstream_process_header_line(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_content_length(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_last_modified(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_set_cookie(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t
    ngx_http_upstream_process_cache_control(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_ignore_header_line(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_expires(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_accel_expires(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_limit_rate(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_buffering(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_charset(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_connection(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t
    ngx_http_upstream_process_transfer_encoding(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_process_vary(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_header_line(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t
    ngx_http_upstream_copy_multi_header_lines(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_content_type(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_last_modified(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_rewrite_location(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_rewrite_refresh(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_rewrite_set_cookie(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
static ngx_int_t ngx_http_upstream_copy_allow_ranges(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);

#if (NGX_HTTP_GZIP)
static ngx_int_t ngx_http_upstream_copy_content_encoding(ngx_http_request_t *r,
    ngx_table_elt_t *h, ngx_uint_t offset);
#endif

static ngx_int_t ngx_http_upstream_add_variables(ngx_conf_t *cf);
static ngx_int_t ngx_http_upstream_addr_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_status_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_response_time_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_upstream_response_length_variable(
    ngx_http_request_t *r, ngx_http_variable_value_t *v, uintptr_t data);

static char *ngx_http_upstream(ngx_conf_t *cf, ngx_command_t *cmd, void *dummy);
static char *ngx_http_upstream_server(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);

static ngx_addr_t *ngx_http_upstream_get_local(ngx_http_request_t *r,
    ngx_http_upstream_local_t *local);

static void *ngx_http_upstream_create_main_conf(ngx_conf_t *cf);
static char *ngx_http_upstream_init_main_conf(ngx_conf_t *cf, void *conf);

#if (NGX_HTTP_SSL)
static void ngx_http_upstream_ssl_init_connection(ngx_http_request_t *,
    ngx_http_upstream_t *u, ngx_connection_t *c);
static void ngx_http_upstream_ssl_handshake(ngx_connection_t *c);
static ngx_int_t ngx_http_upstream_ssl_name(ngx_http_request_t *r,
    ngx_http_upstream_t *u, ngx_connection_t *c);
#endif

//通过ngx_http_upstream_init_main_conf把所有ngx_http_upstream_headers_in成员做hash运算，放入ngx_http_upstream_main_conf_t的headers_in_hash中
//这些成员最终会赋值给ngx_http_request_t->upstream->headers_in
ngx_http_upstream_header_t  ngx_http_upstream_headers_in[] = { //后端应答的头部行匹配这里面的字段后，最终由ngx_http_upstream_headers_in_t里面的成员指向
//该数组生效地方见ngx_http_proxy_process_header，通过handler(如ngx_http_upstream_copy_header_line)把后端头部行的相关信息赋值给ngx_http_request_t->upstream->headers_in相关成员
    { ngx_string("Status"),
                 ngx_http_upstream_process_header_line, //通过该handler函数把从后端服务器解析到的头部行字段赋值给ngx_http_upstream_headers_in_t->status
                 offsetof(ngx_http_upstream_headers_in_t, status),
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("Content-Type"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, content_type),
                 ngx_http_upstream_copy_content_type, 0, 1 },

    { ngx_string("Content-Length"),
                 ngx_http_upstream_process_content_length, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },

    { ngx_string("Date"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, date),
                 ngx_http_upstream_copy_header_line,
                 offsetof(ngx_http_headers_out_t, date), 0 },

    { ngx_string("Last-Modified"),
                 ngx_http_upstream_process_last_modified, 0,
                 ngx_http_upstream_copy_last_modified, 0, 0 },

    { ngx_string("ETag"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, etag),
                 ngx_http_upstream_copy_header_line,
                 offsetof(ngx_http_headers_out_t, etag), 0 },

    { ngx_string("Server"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, server),
                 ngx_http_upstream_copy_header_line,
                 offsetof(ngx_http_headers_out_t, server), 0 },

    { ngx_string("WWW-Authenticate"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, www_authenticate),
                 ngx_http_upstream_copy_header_line, 0, 0 },

//只有在配置了location /uri {mytest;}后，HTTP框架才会在某个请求匹配了/uri后调用它处理请求
    { ngx_string("Location"),  //后端应答这个，表示需要重定向
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, location),
                 ngx_http_upstream_rewrite_location, 0, 0 }, //ngx_http_upstream_process_headers中执行

    { ngx_string("Refresh"),
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_rewrite_refresh, 0, 0 },

    { ngx_string("Set-Cookie"),
                 ngx_http_upstream_process_set_cookie,
                 offsetof(ngx_http_upstream_headers_in_t, cookies),
                 ngx_http_upstream_rewrite_set_cookie, 0, 1 },

    { ngx_string("Content-Disposition"),
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_copy_header_line, 0, 1 },

    { ngx_string("Cache-Control"),
                 ngx_http_upstream_process_cache_control, 0,
                 ngx_http_upstream_copy_multi_header_lines,
                 offsetof(ngx_http_headers_out_t, cache_control), 1 },

    { ngx_string("Expires"),
                 ngx_http_upstream_process_expires, 0,
                 ngx_http_upstream_copy_header_line,
                 offsetof(ngx_http_headers_out_t, expires), 1 },

    { ngx_string("Accept-Ranges"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, accept_ranges),
                 ngx_http_upstream_copy_allow_ranges,
                 offsetof(ngx_http_headers_out_t, accept_ranges), 1 },

    { ngx_string("Connection"),
                 ngx_http_upstream_process_connection, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },

    { ngx_string("Keep-Alive"),
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },

    { ngx_string("Vary"), //nginx在缓存过程中不会处理"Vary"头，为了确保一些私有数据不被所有的用户看到，
                 ngx_http_upstream_process_vary, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("X-Powered-By"),
                 ngx_http_upstream_ignore_header_line, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("X-Accel-Expires"),
                 ngx_http_upstream_process_accel_expires, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("X-Accel-Redirect"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, x_accel_redirect),
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("X-Accel-Limit-Rate"),
                 ngx_http_upstream_process_limit_rate, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("X-Accel-Buffering"),
                 ngx_http_upstream_process_buffering, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("X-Accel-Charset"),
                 ngx_http_upstream_process_charset, 0,
                 ngx_http_upstream_copy_header_line, 0, 0 },

    { ngx_string("Transfer-Encoding"),
                 ngx_http_upstream_process_transfer_encoding, 0,
                 ngx_http_upstream_ignore_header_line, 0, 0 },

#if (NGX_HTTP_GZIP)
    { ngx_string("Content-Encoding"),
                 ngx_http_upstream_process_header_line,
                 offsetof(ngx_http_upstream_headers_in_t, content_encoding),
                 ngx_http_upstream_copy_content_encoding, 0, 0 },
#endif

    { ngx_null_string, NULL, 0, NULL, 0, 0 }
};

/*
关于nginx upstream的几种配置方式
发表于2011 年 06 月 16 日由edwin 
平时一直依赖硬件来作load blance，最近研究Nginx来做负载设备，记录下upstream的几种配置方式。

第一种：轮询

upstream test{
    server 192.168.0.1:3000;
    server 192.168.0.1:3001;
}第二种：权重

upstream test{
    server 192.168.0.1 weight=2;
    server 192.168.0.2 weight=3;
}这种模式可解决服务器性能不等的情况下轮询比率的调配

第三种：ip_hash

upstream test{
    ip_hash;
    server 192.168.0.1;
    server 192.168.0.2;
}这种模式会根据来源IP和后端配置来做hash分配，确保固定IP只访问一个后端

第四种：fair

需要安装Upstream Fair Balancer Module

upstream test{
    server 192.168.0.1;
    server 192.168.0.2;
    fair;
}这种模式会根据后端服务的响应时间来分配，响应时间短的后端优先分配

第五种：自定义hash

需要安装Upstream Hash Module

upstream test{
    server 192.168.0.1;
    server 192.168.0.2;
    hash $request_uri;
}这种模式可以根据给定的字符串进行Hash分配

具体应用：

server{
    listen 80;
    server_name .test.com;
    charset utf-8;
    
    location / {
        proxy_pass http://test/;
    } 
}此外upstream每个后端的可设置参数为：

1.down: 表示此台server暂时不参与负载。
2.weight: 默认为1，weight越大，负载的权重就越大。
3.max_fails: 允许请求失败的次数默认为1.当超过最大次数时，返回proxy_next_upstream模块定义的错误。
4.fail_timeout: max_fails次失败后，暂停的时间。
5.backup: 其它所有的非backup机器down或者忙的时候，请求backup机器，应急措施。
*/
static ngx_command_t  ngx_http_upstream_commands[] = {
/*
语法：upstream name { ... } 
默认值：none 
使用字段：http 
这个字段设置一群服务器，可以将这个字段放在proxy_pass和fastcgi_pass指令中作为一个单独的实体，它们可以可以是监听不同端口的服务器，
并且也可以是同时监听TCP和Unix socket的服务器。
服务器可以指定不同的权重，默认为1。
示例配置

upstream backend {
  server backend1.example.com weight=5;
  server 127.0.0.1:8080       max_fails=3  fail_timeout=30s;
  server unix:/tmp/backend3;

  server backup1.example.com:8080 backup; 
}请求将按照轮询的方式分发到后端服务器，但同时也会考虑权重。
在上面的例子中如果每次发生7个请求，5个请求将被发送到backend1.example.com，其他两台将分别得到一个请求，如果有一台服务器不可用，那么
请求将被转发到下一台服务器，直到所有的服务器检查都通过。如果所有的服务器都无法通过检查，那么将返回给客户端最后一台工作的服务器产生的结果。

max_fails=number

  设置在fail_timeout参数设置的时间内最大失败次数，如果在这个时间内，所有针对该服务器的请求
  都失败了，那么认为该服务器会被认为是停机了，停机时间是fail_timeout设置的时间。默认情况下，
  不成功连接数被设置为1。被设置为零则表示不进行链接数统计。那些连接被认为是不成功的可以通过
  proxy_next_upstream, fastcgi_next_upstream，和memcached_next_upstream指令配置。http_404
  状态不会被认为是不成功的尝试。

fail_time=time
  设置 多长时间内失败次数达到最大失败次数会被认为服务器停机了服务器会被认为停机的时间长度 默认情况下，超时时间被设置为10S
*/
    { ngx_string("upstream"),
      NGX_HTTP_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_TAKE1,
      ngx_http_upstream,
      0,
      0,
      NULL },

    /*
    语法：server name [parameters];
    配置块：upstream
    server配置项指定了一台上游服务器的名字，这个名字可以是域名、IP地址端口、UNIX句柄等，在其后还可以跟下列参数。
    weight=number：设置向这台上游服务器转发的权重，默认为1。 weigth参数表示权值，权值越高被分配到的几率越大 
    max_fails=number：该选项与fail_timeout配合使用，指在fail_timeout时间段内，如果向当前的上游服务器转发失败次数超过number，则认为在当前的fail_timeout时间段内这台上游服务器不可用。max_fails默认为1，如果设置为0，则表示不检查失败次数。
    fail_timeout=time：fail_timeout表示该时间段内转发失败多少次后就认为上游服务器暂时不可用，用于优化反向代理功能。它与向上游服务器建立连接的超时时间、读取上游服务器的响应超时时间等完全无关。fail_timeout默认为10秒。
    down：表示所在的上游服务器永久下线，只在使用ip_hash配置项时才有用。
    backup：在使用ip_hash配置项时它是无效的。它表示所在的上游服务器只是备份服务器，只有在所有的非备份上游服务器都失效后，才会向所在的上游服务器转发请求。
    例如：
    upstream  backend  {
      server   backend1.example.com    weight=5;
      server   127.0.0.1:8080          max_fails=3  fail_timeout=30s;
      server   unix:/tmp/backend3;
    }

    
    语法：server name [parameters] 
    默认值：none 
    使用字段：upstream 
    指定后端服务器的名称和一些参数，可以使用域名，IP，端口，或者unix socket。如果指定为域名，则首先将其解析为IP。
    ・weight = NUMBER - 设置服务器权重，默认为1。
    ・max_fails = NUMBER - 在一定时间内（这个时间在fail_timeout参数中设置）检查这个服务器是否可用时产生的最多失败请求数，默认为1，将其设置为0可以关闭检查，这些错误在proxy_next_upstream或fastcgi_next_upstream（404错误不会使max_fails增加）中定义。
    ・fail_timeout = TIME - 在这个时间内产生了max_fails所设置大小的失败尝试连接请求后这个服务器可能不可用，同样它指定了服务器不可用的时间（在下一次尝试连接请求发起之前），默认为10秒，fail_timeout与前端响应时间没有直接关系，不过可以使用proxy_connect_timeout和proxy_read_timeout来控制。
    ・down - 标记服务器处于离线状态，通常和ip_hash一起使用。
    ・backup - (0.6.7或更高)如果所有的非备份服务器都宕机或繁忙，则使用本服务器（无法和ip_hash指令搭配使用）。
    示例配置
    
    upstream  backend  {
      server   backend1.example.com    weight=5;
      server   127.0.0.1:8080          max_fails=3  fail_timeout=30s;
      server   unix:/tmp/backend3;
    }注意：如果你只使用一台上游服务器，nginx将设置一个内置变量为1，即max_fails和fail_timeout参数不会被处理。
    结果：如果nginx不能连接到上游，请求将丢失。
    解决：使用多台上游服务器。
    */
    { ngx_string("server"),
      NGX_HTTP_UPS_CONF|NGX_CONF_1MORE,
      ngx_http_upstream_server,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_upstream_module_ctx = {
    ngx_http_upstream_add_variables,       /* preconfiguration */
    NULL,                                  /* postconfiguration */

    ngx_http_upstream_create_main_conf,    /* create main configuration */
    ngx_http_upstream_init_main_conf,      /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/*
负载均衡相关配置:
upstream
server
ip_hash:根据客户端的IP来做hash,不过如果squid -- nginx -- server(s)则，ip永远是squid服务器ip,因此不管用,需要ngx_http_realip_module或者第三方模块
keepalive:配置到后端的最大连接数，保持长连接，不必为每一个客户端都重新建立nginx到后端PHP等服务器的连接，需要保持和后端
    长连接，例如fastcgi:fastcgi_keep_conn on;       proxy:  proxy_http_version 1.1;  proxy_set_header Connection "";
least_conn:根据其权重值，将请求发送到活跃连接数最少的那台服务器
hash:可以按照uri  ip 等参数进行做hash

参考http://tengine.taobao.org/nginx_docs/cn/docs/http/ngx_http_upstream_module.html#ip_hash
*/


/*
Nginx不仅仅可以用做Web服务器。upstream机制其实是由ngx_http_upstream_module模块实现的，它是一个HTTP模块，使用upstream机制时客
户端的请求必须基于HTTP。

既然upstream是用于访问“上游”服务器的，那么，Nginx需要访问什么类型的“上游”服务器呢？是Apache、Tomcat这样的Web服务器，还
是memcached、cassandra这样的Key-Value存储系统，又或是mongoDB、MySQL这样的数据库？这就涉及upstream机制的范围了。基于事件驱动
架构的upstream机制所要访问的就是所有支持TCP的上游服务器。因此，既有ngx_http_proxy_module模块基于upstream机制实现了HTTP的反向
代理功能，也有类似ngx_http_memcached_module的模块基于upstream机制使得请求可以访问memcached服务器。

当nginx接收到一个连接后，读取完客户端发送出来的Header，然后就会进行各个处理过程的调用。之后就是upstream发挥作用的时候了，
upstream在客户端跟后端比如FCGI/PHP之间，接收客户端的HTTP body，发送给FCGI，然后接收FCGI的结果，发送给客户端。作为一个桥梁的作用。
同时，upstream为了充分显示其灵活性，至于后端具体是什么协议，什么系统他都不care，我只实现主体的框架，具体到FCGI协议的发送，接收，
解析，这些都交给后面的插件来处理，比如有fastcgi,memcached,proxy等插件

http://chenzhenianqing.cn/articles/category/%e5%90%84%e7%a7%8dserver/nginx
upstream和FastCGI memcached  uwsgi  scgi proxy的关系参考:http://chenzhenianqing.cn/articles/category/%e5%90%84%e7%a7%8dserver/nginx
*/
ngx_module_t  ngx_http_upstream_module = { //该模块是访问上游服务器相关模块的基础(例如 FastCGI memcached  uwsgi  scgi proxy都会用到upstream模块  ngx_http_proxy_module  ngx_http_memcached_module)
    NGX_MODULE_V1,
    &ngx_http_upstream_module_ctx,            /* module context */
    ngx_http_upstream_commands,               /* module directives */
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


static ngx_http_variable_t  ngx_http_upstream_vars[] = {

    { ngx_string("upstream_addr"), NULL,
      ngx_http_upstream_addr_variable, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 }, //前端服务器处理请求的服务器地址

    { ngx_string("upstream_status"), NULL,
      ngx_http_upstream_status_variable, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 }, //前端服务器的响应状态。

    { ngx_string("upstream_connect_time"), NULL,
      ngx_http_upstream_response_time_variable, 2,
      NGX_HTTP_VAR_NOCACHEABLE, 0 }, 

    { ngx_string("upstream_header_time"), NULL,
      ngx_http_upstream_response_time_variable, 1,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_response_time"), NULL,
      ngx_http_upstream_response_time_variable, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },//前端服务器的应答时间，精确到毫秒，不同的应答以逗号和冒号分开。

    { ngx_string("upstream_response_length"), NULL,
      ngx_http_upstream_response_length_variable, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

#if (NGX_HTTP_CACHE)

    { ngx_string("upstream_cache_status"), NULL,
      ngx_http_upstream_cache_status, 0,
      NGX_HTTP_VAR_NOCACHEABLE, 0 },

    { ngx_string("upstream_cache_last_modified"), NULL,
      ngx_http_upstream_cache_last_modified, 0,
      NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH, 0 },

    { ngx_string("upstream_cache_etag"), NULL,
      ngx_http_upstream_cache_etag, 0,
      NGX_HTTP_VAR_NOCACHEABLE|NGX_HTTP_VAR_NOHASH, 0 },

#endif

    { ngx_null_string, NULL, NULL, 0, 0, 0 }
};


static ngx_http_upstream_next_t  ngx_http_upstream_next_errors[] = {
    { 500, NGX_HTTP_UPSTREAM_FT_HTTP_500 },
    { 502, NGX_HTTP_UPSTREAM_FT_HTTP_502 },
    { 503, NGX_HTTP_UPSTREAM_FT_HTTP_503 },
    { 504, NGX_HTTP_UPSTREAM_FT_HTTP_504 },
    { 403, NGX_HTTP_UPSTREAM_FT_HTTP_403 },
    { 404, NGX_HTTP_UPSTREAM_FT_HTTP_404 },
    { 0, 0 }
};


ngx_conf_bitmask_t  ngx_http_upstream_cache_method_mask[] = {
   { ngx_string("GET"),  NGX_HTTP_GET},
   { ngx_string("HEAD"), NGX_HTTP_HEAD },
   { ngx_string("POST"), NGX_HTTP_POST },
   { ngx_null_string, 0 }
};


ngx_conf_bitmask_t  ngx_http_upstream_ignore_headers_masks[] = {
    { ngx_string("X-Accel-Redirect"), NGX_HTTP_UPSTREAM_IGN_XA_REDIRECT },
    { ngx_string("X-Accel-Expires"), NGX_HTTP_UPSTREAM_IGN_XA_EXPIRES },
    { ngx_string("X-Accel-Limit-Rate"), NGX_HTTP_UPSTREAM_IGN_XA_LIMIT_RATE },
    { ngx_string("X-Accel-Buffering"), NGX_HTTP_UPSTREAM_IGN_XA_BUFFERING },
    { ngx_string("X-Accel-Charset"), NGX_HTTP_UPSTREAM_IGN_XA_CHARSET },
    { ngx_string("Expires"), NGX_HTTP_UPSTREAM_IGN_EXPIRES },
    { ngx_string("Cache-Control"), NGX_HTTP_UPSTREAM_IGN_CACHE_CONTROL },
    { ngx_string("Set-Cookie"), NGX_HTTP_UPSTREAM_IGN_SET_COOKIE },
    { ngx_string("Vary"), NGX_HTTP_UPSTREAM_IGN_VARY },
    { ngx_null_string, 0 }
};


//ngx_http_upstream_create创建ngx_http_upstream_t，资源回收用ngx_http_upstream_finalize_request
//upstream资源回收在ngx_http_upstream_finalize_request   ngx_http_XXX_handler(ngx_http_proxy_handler)中执行
ngx_int_t
ngx_http_upstream_create(ngx_http_request_t *r)//创建一个ngx_http_upstream_t结构，放到r->upstream里面去。
{
    ngx_http_upstream_t  *u;

    u = r->upstream;

    if (u && u->cleanup) {
        r->main->count++;
        ngx_http_upstream_cleanup(r);
    }

    u = ngx_pcalloc(r->pool, sizeof(ngx_http_upstream_t));
    if (u == NULL) {
        return NGX_ERROR;
    }

    r->upstream = u;

    u->peer.log = r->connection->log;
    u->peer.log_error = NGX_ERROR_ERR;

#if (NGX_HTTP_CACHE)
    r->cache = NULL;
#endif

    u->headers_in.content_length_n = -1;
    u->headers_in.last_modified_time = -1;

    return NGX_OK;
}

/*
    1)调用ngx_http_up stream_init方法启动upstream。
    2) upstream模块会去检查文件缓存，如果缓存中已经有合适的响应包，则会直接返回缓存（当然必须是在使用反向代理文件缓存的前提下）。
    为了让读者方便地理解upstream机制，本章将不再提及文件缓存。
    3)回调mytest模块已经实现的create_request回调方法。
    4) mytest模块通过设置r->upstream->request_bufs已经决定好发送什么样的请求到上游服务器。
    5) upstream模块将会检查resolved成员，如果有resolved成员的话，就根据它设置好上游服务器的地址r->upstream->peer成员。
    6)用无阻塞的TCP套接字建立连接。
    7)无论连接是否建立成功，负责建立连接的connect方法都会立刻返回。
*/
//ngx_http_upstream_init方法将会根据ngx_http_upstream_conf_t中的成员初始化upstream，同时会开始连接上游服务器，以此展开整个upstream处理流程
void ngx_http_upstream_init(ngx_http_request_t *r) 
//在读取完浏览器发送来的请求头部字段后，会通过proxy fastcgi等模块读取包体，读取完后执行该函数，例如ngx_http_read_client_request_body(r, ngx_http_upstream_init);
{
    ngx_connection_t     *c;

    c = r->connection;//得到客户端连接结构。

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http init upstream, client timer: %d", c->read->timer_set);

#if (NGX_HTTP_V2)
    if (r->stream) {
        ngx_http_upstream_init_request(r);
        return;
    }
#endif

    /*
        首先检查请求对应于客户端的连接，这个连接上的读事件如果在定时器中，也就是说，读事件的timer_ set标志位为1，那么调用ngx_del_timer
    方法把这个读事件从定时器中移除。为什么要做这件事呢？因为一旦启动upstream机制，就不应该对客户端的读操作带有超时时间的处理(超时会关闭客户端连接)，
    请求的主要触发事件将以与上游服务器的连接为主。
     */
    if (c->read->timer_set) {
        ngx_del_timer(c->read, NGX_FUNC_LINE);
    }

    if (ngx_event_flags & NGX_USE_CLEAR_EVENT) { //如果epoll使用边缘触发

/*
2025/04/24 05:31:47[             ngx_http_upstream_init,   654]  [debug] 15507#15507: *1 <   ngx_http_upstream_init,   653> epoll NGX_WRITE_EVENT(et) read add
2025/04/24 05:31:47[                ngx_epoll_add_event,  1400]  [debug] 15507#15507: *1 epoll modify read and write event: fd:11 op:3 ev:80002005
025/04/24 05:31:47[           ngx_epoll_process_events,  1624]  [debug] 15507#15507: begin to epoll_wait, epoll timer: 60000 
2025/04/24 05:31:47[           ngx_epoll_process_events,  1709]  [debug] 15507#15507: epoll: fd:11 epoll-out(ev:0004) d:B26A00E8
实际上是通过ngx_http_upstream_init中的mod epoll_ctl添加读写事件触发的，当本次循环退回到ngx_worker_process_cycle ..->ngx_epoll_process_events
的时候，就会触发epoll_out,从而执行ngx_http_upstream_wr_check_broken_connection
*/
        //这里实际上是触发执行ngx_http_upstream_check_broken_connection
        if (!c->write->active) {//要增加可写事件通知，为啥?因为待会可能就能写了,可能会转发上游服务器的内容给浏览器等客户端
            //实际上是检查和客户端的连接是否异常了
            char tmpbuf[256];
            
            snprintf(tmpbuf, sizeof(tmpbuf), "<%25s, %5d> epoll NGX_WRITE_EVENT(et) write add", NGX_FUNC_LINE);
            ngx_log_debug0(NGX_LOG_DEBUG_EVENT, c->log, 0, tmpbuf);
            if (ngx_add_event(c->write, NGX_WRITE_EVENT, NGX_CLEAR_EVENT)
                == NGX_ERROR)
            {
                ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
                return;
            }
        }
    }

    ngx_http_upstream_init_request(r);
}

//ngx_http_upstream_init调用这里，此时客户端发送的数据都已经接收完毕了。
/*
1. 调用create_request创建fcgi或者proxy的数据结构。
2. 调用ngx_http_upstream_connect连接下游服务器。
*/ 
static void
ngx_http_upstream_init_request(ngx_http_request_t *r)
{
    ngx_str_t                      *host;
    ngx_uint_t                      i;
    ngx_resolver_ctx_t             *ctx, temp;
    ngx_http_cleanup_t             *cln;
    ngx_http_upstream_t            *u;
    ngx_http_core_loc_conf_t       *clcf;
    ngx_http_upstream_srv_conf_t   *uscf, **uscfp;
    ngx_http_upstream_main_conf_t  *umcf;
    

    if (r->aio) {
        return;
    }

    u = r->upstream;//ngx_http_upstream_create里面设置的  ngx_http_XXX_handler(ngx_http_proxy_handler)中执行

#if (NGX_HTTP_CACHE)

    if (u->conf->cache) {
        ngx_int_t  rc;

        int cache = u->conf->cache;
        rc = ngx_http_upstream_cache(r, u);
        ngx_log_debugall(r->connection->log, 0, "ngx http cache, conf->cache:%d, rc:%d", cache, rc);
        
        if (rc == NGX_BUSY) {
            r->write_event_handler = ngx_http_upstream_init_request;
            return;
        }

        r->write_event_handler = ngx_http_request_empty_handler;

        if (rc == NGX_DONE) {
            return;
        }

        if (rc == NGX_ERROR) {
            ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }

        if (rc != NGX_DECLINED) {
            ngx_http_finalize_request(r, rc);
            return;
        }
    }

#endif

    u->store = u->conf->store;

    /*
    设置Nginx与下游客户端之间TCP连接的检查方法
    实际上，这两个方法都会通过ngx_http_upstream_check_broken_connection方法检查Nginx与下游的连接是否正常，如果出现错误，就会立即终止连接。
     */
/*
2025/04/24 05:31:47[             ngx_http_upstream_init,   654]  [debug] 15507#15507: *1 <   ngx_http_upstream_init,   653> epoll NGX_WRITE_EVENT(et) read add
2025/04/24 05:31:47[                ngx_epoll_add_event,  1400]  [debug] 15507#15507: *1 epoll modify read and write event: fd:11 op:3 ev:80002005
2025/04/24 05:31:47[           ngx_epoll_process_events,  1624]  [debug] 15507#15507: begin to epoll_wait, epoll timer: 60000 
2025/04/24 05:31:47[           ngx_epoll_process_events,  1709]  [debug] 15507#15507: epoll: fd:11 epoll-out(ev:0004) d:B26A00E8
实际上是通过ngx_http_upstream_init中的mod epoll_ctl添加读写事件触发的，当本次循环退回到ngx_worker_process_cycle ..->ngx_epoll_process_events
的时候，就会触发epoll_out,从而执行ngx_http_upstream_wr_check_broken_connection
*/
    if (!u->store && !r->post_action && !u->conf->ignore_client_abort) {
        //注意这时候的r还是客户端的连接，与上游服务器的连接r还没有建立
        r->read_event_handler = ngx_http_upstream_rd_check_broken_connection; //设置回调需要检测连接是否有问题。
        r->write_event_handler = ngx_http_upstream_wr_check_broken_connection;
    }
    

    //有接收到客户端包体，则把包体结构赋值给u->request_bufs，在后面的if (u->create_request(r) != NGX_OK) {会用到
    if (r->request_body) {//客户端发送过来的POST数据存放在此,ngx_http_read_client_request_body放的
        u->request_bufs = r->request_body->bufs; //记录客户端发送的数据，下面在create_request的时候拷贝到发送缓冲链接表里面的。
    }

    /*
     调用请求中ngx_http_upstream_t结构体里由某个HTTP模块实现的create_request方法，构造发往上游服务器的请求
     （请求中的内容是设置到request_bufs缓冲区链表中的）。如果create_request方法没有返回NGX_OK，则upstream结束

     如果是FCGI。下面组建好FCGI的各种头部，包括请求开始头，请求参数头，请求STDIN头。存放在u->request_bufs链接表里面。
	如果是Proxy模块，ngx_http_proxy_create_request组件反向代理的头部啥的,放到u->request_bufs里面
	FastCGI memcached  uwsgi  scgi proxy都会用到upstream模块
     */
    if (u->create_request(r) != NGX_OK) { //ngx_http_XXX_create_request   ngx_http_proxy_create_request等
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }

    
    /* 获取ngx_http_upstream_t结构中主动连接结构peer的local本地地址信息 */
    u->peer.local = ngx_http_upstream_get_local(r, u->conf->local);

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
    
    /* 初始化ngx_http_upstream_t结构中成员output向下游发送响应的方式 */
    u->output.alignment = clcf->directio_alignment; //
    u->output.pool = r->pool;
    u->output.bufs.num = 1;
    u->output.bufs.size = clcf->client_body_buffer_size;

    if (u->output.output_filter == NULL) {
        //设置过滤模块的开始过滤函数为writer。也就是output_filter。在ngx_output_chain被调用已进行数据的过滤
        u->output.output_filter = ngx_chain_writer; 
        u->output.filter_ctx = &u->writer; //参考ngx_chain_writer，里面会将输出buf一个个连接到这里。
    }

    u->writer.pool = r->pool;
    
    /* 添加用于表示上游响应的状态，例如：错误编码、包体长度等 */
    if (r->upstream_states == NULL) {//数组upstream_states，保留upstream的状态信息。

        r->upstream_states = ngx_array_create(r->pool, 1,
                                            sizeof(ngx_http_upstream_state_t));
        if (r->upstream_states == NULL) {
            ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }

    } else {

        u->state = ngx_array_push(r->upstream_states);
        if (u->state == NULL) {
            ngx_http_upstream_finalize_request(r, u,
                                               NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }

        ngx_memzero(u->state, sizeof(ngx_http_upstream_state_t));
    }

    cln = ngx_http_cleanup_add(r, 0);//环形链表，申请一个新的元素。
    if (cln == NULL) {
        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }

    cln->handler = ngx_http_upstream_cleanup; //当请求结束时，一定会调用ngx_http_upstream_cleanup方法
    cln->data = r;//指向所指的请求结构体。
    u->cleanup = &cln->handler;

    /*
    http://www.pagefault.info/?p=251
    然后就是这个函数最核心的处理部分，那就是根据upstream的类型来进行不同的操作，这里的upstream就是我们通过XXX_pass传递进来的值，
    这里的upstream有可能下面几种情况。 
    Ngx_http_fastcgi_module.c (src\http\modules):    { ngx_string("fastcgi_pass"),
    Ngx_http_memcached_module.c (src\http\modules):    { ngx_string("memcached_pass"),
    Ngx_http_proxy_module.c (src\http\modules):    { ngx_string("proxy_pass"),
    Ngx_http_scgi_module.c (src\http\modules):    { ngx_string("scgi_pass"),
    Ngx_http_uwsgi_module.c (src\http\modules):    { ngx_string("uwsgi_pass"),
    Ngx_stream_proxy_module.c (src\stream):    { ngx_string("proxy_pass"),
    1 XXX_pass中不包含变量。
    2 XXX_pass传递的值包含了一个变量($开始).这种情况也就是说upstream的url是动态变化的，因此需要每次都解析一遍.
    而第二种情况又分为2种，一种是在进入upstream之前，也就是 upstream模块的handler之中已经被resolve的地址(请看ngx_http_XXX_eval函数)，
    一种是没有被resolve，此时就需要upstream模块来进行resolve。接下来的代码就是处理这部分的东西。
    */
    if (u->resolved == NULL) {//上游的IP地址是否被解析过，ngx_http_fastcgi_handler调用ngx_http_fastcgi_eval会解析。 为NULL说明没有解析过，也就是fastcgi_pas xxx中的xxx参数没有变量
        uscf = u->conf->upstream; //upstream赋值在ngx_http_fastcgi_pass
    } else { //fastcgi_pass xxx的xxx中有变量，说明后端服务器是会根据请求动态变化的，参考ngx_http_fastcgi_handler

#if (NGX_HTTP_SSL)
        u->ssl_name = u->resolved->host;
#endif
    //ngx_http_fastcgi_handler 会调用 ngx_http_fastcgi_eval函数，进行fastcgi_pass 后面的URL的简析，解析出unix域，或者socket.
        // 如果已经是ip地址格式了，就不需要再进行解析
        if (u->resolved->sockaddr) {//如果地址已经被resolve过了，我IP地址，此时创建round robin peer就行

            if (ngx_http_upstream_create_round_robin_peer(r, u->resolved)
                != NGX_OK)
            {
                ngx_http_upstream_finalize_request(r, u,
                                               NGX_HTTP_INTERNAL_SERVER_ERROR);
                return;
            }

            ngx_http_upstream_connect(r, u);//连接 

            return;
        }

        //下面开始查找域名，因为fcgi_pass后面不是ip:port，而是url；
        host = &u->resolved->host;//获取host信息。 
        // 接下来就要开始查找域名

        umcf = ngx_http_get_module_main_conf(r, ngx_http_upstream_module);

        uscfp = umcf->upstreams.elts;

        for (i = 0; i < umcf->upstreams.nelts; i++) {//遍历所有的上游模块，根据其host进行查找，找到host,port相同的。

            uscf = uscfp[i];//找一个IP一样的上流模块

            if (uscf->host.len == host->len
                && ((uscf->port == 0 && u->resolved->no_port)
                     || uscf->port == u->resolved->port)
                && ngx_strncasecmp(uscf->host.data, host->data, host->len) == 0)
            {
                goto found;//这个host正好相等
            }
        }

        if (u->resolved->port == 0) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "no port in upstream \"%V\"", host);
            ngx_http_upstream_finalize_request(r, u,
                                               NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }

        //没办法了，url不在upstreams数组里面，也就是不是我们配置的，那么初始化域名解析器
        temp.name = *host;
        
        // 初始化域名解析器
        ctx = ngx_resolve_start(clcf->resolver, &temp);//进行域名解析，带缓存的。申请相关的结构，返回上下文地址。
        if (ctx == NULL) {
            ngx_http_upstream_finalize_request(r, u,
                                               NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }

        if (ctx == NGX_NO_RESOLVER) {//无法进行域名解析。 
            // 返回NGX_NO_RESOLVER表示无法进行域名解析
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "no resolver defined to resolve %V", host);

            ngx_http_upstream_finalize_request(r, u, NGX_HTTP_BAD_GATEWAY);
            return;
        }
        
        // 设置需要解析的域名的类型与信息
        ctx->name = *host;
        ctx->handler = ngx_http_upstream_resolve_handler;//设置域名解析完成后的回调函数。
        ctx->data = r;
        ctx->timeout = clcf->resolver_timeout;

        u->resolved->ctx = ctx;

        //开始域名解析，没有完成也会返回的。
        if (ngx_resolve_name(ctx) != NGX_OK) {
            u->resolved->ctx = NULL;
            ngx_http_upstream_finalize_request(r, u,
                                               NGX_HTTP_INTERNAL_SERVER_ERROR);
            return;
        }

        return;
        // 域名还没有解析完成，则直接返回
    }

found:

    if (uscf == NULL) {
        ngx_log_error(NGX_LOG_ALERT, r->connection->log, 0,
                      "no upstream configuration");
        ngx_http_upstream_finalize_request(r, u,
                                           NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }

#if (NGX_HTTP_SSL)
    u->ssl_name = uscf->host;
#endif

    if (uscf->peer.init(r, uscf) != NGX_OK) {//ngx_http_upstream_init_round_XX_peer(ngx_http_upstream_init_round_robin_peer)
        ngx_http_upstream_finalize_request(r, u,
                                           NGX_HTTP_INTERNAL_SERVER_ERROR);
        return;
    }

    u->peer.start_time = ngx_current_msec;

    if (u->conf->next_upstream_tries
        && u->peer.tries > u->conf->next_upstream_tries)
    {
        u->peer.tries = u->conf->next_upstream_tries;
    }

    ngx_http_upstream_connect(r, u);//调用ngx_http_upstream_connect方法向上游服务器发起连接
}


#if (NGX_HTTP_CACHE)
/*ngx_http_upstream_init_request->ngx_http_upstream_cache 客户端获取缓存 后端应答回来数据后在ngx_http_upstream_send_response->ngx_http_file_cache_create
中创建临时文件，然后在ngx_event_pipe_write_chain_to_temp_file把读取的后端数据写入临时文件，最后在
ngx_http_upstream_send_response->ngx_http_upstream_process_request->ngx_http_file_cache_update中把临时文件内容rename(相当于mv)到proxy_cache_path指定
的cache目录下面
*/
static ngx_int_t
ngx_http_upstream_cache(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_int_t               rc;
    ngx_http_cache_t       *c; 
    ngx_http_file_cache_t  *cache;

    c = r->cache;

    if (c == NULL) { /* 如果还未给当前请求分配缓存相关结构体( ngx_http_cache_t ) 时，创建此类型字段( r->cache ) 并初始化： */
        //例如proxy |fastcgi _cache_methods  POST设置值缓存POST请求，但是客户端请求方法是GET，则直接返回
        if (!(r->method & u->conf->cache_methods)) {
            return NGX_DECLINED;
        }

        rc = ngx_http_upstream_cache_get(r, u, &cache);

        if (rc != NGX_OK) {
            return rc;
        }

        if (r->method & NGX_HTTP_HEAD) {
            u->method = ngx_http_core_get_method;
        }

        if (ngx_http_file_cache_new(r) != NGX_OK) {
            return NGX_ERROR;
        }

        if (u->create_key(r) != NGX_OK) {////解析xx_cache_key adfaxx 参数值到r->cache->keys
            return NGX_ERROR;
        }

        /* TODO: add keys */

        ngx_http_file_cache_create_key(r); /* 生成 md5sum(key) 和 crc32(key)并计算 `c->header_start` 值 */

        if (r->cache->header_start + 256 >= u->conf->buffer_size) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "%V_buffer_size %uz is not enough for cache key, "
                          "it should be increased to at least %uz",
                          &u->conf->module, u->conf->buffer_size,
                          ngx_align(r->cache->header_start + 256, 1024));

            r->cache = NULL;
            return NGX_DECLINED;
        }

        u->cacheable = 1;/* 默认所有请求的响应结果都是可被缓存的 */

        c = r->cache;

        
        /* 后续会进行调整 */
        c->body_start = u->conf->buffer_size; //xxx_buffer_size(fastcgi_buffer_size proxy_buffer_size memcached_buffer_size)
        c->min_uses = u->conf->cache_min_uses; //Proxy_cache_min_uses number 默认为1，当客户端发送相同请求达到规定次数后，nginx才对响应数据进行缓存；
        c->file_cache = cache;

        /*
          根据配置文件中 ( fastcgi_cache_bypass ) 缓存绕过条件和请求信息，判断是否应该 
          继续尝试使用缓存数据响应该请求： 
          */
        switch (ngx_http_test_predicates(r, u->conf->cache_bypass)) {//判断是否应该冲缓存中取，还是从后端服务器取

        case NGX_ERROR:
            return NGX_ERROR;

        case NGX_DECLINED:
            u->cache_status = NGX_HTTP_CACHE_BYPASS;
            return NGX_DECLINED;

        default: /* NGX_OK */ //应该从后端服务器重新获取
            break;
        }

        c->lock = u->conf->cache_lock;
        c->lock_timeout = u->conf->cache_lock_timeout;
        c->lock_age = u->conf->cache_lock_age;

        u->cache_status = NGX_HTTP_CACHE_MISS;
    }

    rc = ngx_http_file_cache_open(r);

    ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "http upstream cache: %i", rc);

    switch (rc) {

    case NGX_HTTP_CACHE_UPDATING:
        
        if (u->conf->cache_use_stale & NGX_HTTP_UPSTREAM_FT_UPDATING) {
            //如果设置了fastcgi_cache_use_stale updating，表示说虽然该缓存文件失效了，已经有其他客户端请求在获取后端数据，但是现在还没有获取完整，
            //这时候就可以把以前过期的缓存发送给当前请求的客户端
            u->cache_status = rc;
            rc = NGX_OK;

        } else {
            rc = NGX_HTTP_CACHE_STALE;
        }

        break;

    case NGX_OK: //缓存正常命中
        u->cache_status = NGX_HTTP_CACHE_HIT;
    }

    switch (rc) {

    case NGX_OK:

        rc = ngx_http_upstream_cache_send(r, u);

        if (rc != NGX_HTTP_UPSTREAM_INVALID_HEADER) {
            return rc;
        }

        break;

    case NGX_HTTP_CACHE_STALE: //表示缓存过期，见上面的ngx_http_file_cache_open->ngx_http_file_cache_read

        c->valid_sec = 0;
        u->buffer.start = NULL;
        u->cache_status = NGX_HTTP_CACHE_EXPIRED;

        break;

    //如果返回这个，会把cached置0，返回出去后只有从后端从新获取数据
    case NGX_DECLINED: //表示缓存文件存在，获取缓存文件中前面的头部部分检查有问题，没有通过检查。或者缓存文件不存在(第一次请求该uri或者没有达到开始缓存的请求次数)

        if ((size_t) (u->buffer.end - u->buffer.start) < u->conf->buffer_size) {
            u->buffer.start = NULL;

        } else {
            u->buffer.pos = u->buffer.start + c->header_start;
            u->buffer.last = u->buffer.pos;
        }

        break;

    case NGX_HTTP_CACHE_SCARCE: //没有达到请求次数，只有达到请求次数才会缓存

        u->cacheable = 0;//这里置0，就是说如果配置5次开始缓存，则前面4次都不会缓存，把cacheable置0就不会缓存了

        break;

    case NGX_AGAIN:

        return NGX_BUSY;

    case NGX_ERROR:

        return NGX_ERROR;

    default:

        /* cached NGX_HTTP_BAD_GATEWAY, NGX_HTTP_GATEWAY_TIME_OUT, etc. */

        u->cache_status = NGX_HTTP_CACHE_HIT;

        return rc;
    }

    r->cached = 0;

    return NGX_DECLINED;
}


static ngx_int_t
ngx_http_upstream_cache_get(ngx_http_request_t *r, ngx_http_upstream_t *u,
    ngx_http_file_cache_t **cache)
{
    ngx_str_t               *name, val;
    ngx_uint_t               i;
    ngx_http_file_cache_t  **caches;

    if (u->conf->cache_zone) { 
    //获取proxy_cache设置的共享内存块名，直接返回u->conf->cache_zone->data(这个是在proxy_cache_path fastcgi_cache_path设置的)，因此必须同时设置
    //proxy_cache和proxy_cache_path
        *cache = u->conf->cache_zone->data;
        ngx_log_debugall(r->connection->log, 0, "ngx http upstream cache get use keys_zone:%V", &u->conf->cache_zone->shm.name);
        return NGX_OK;
    }

    //说明proxy_cache xxx$ss中带有参数
    if (ngx_http_complex_value(r, u->conf->cache_value, &val) != NGX_OK) {
        return NGX_ERROR;
    }

    if (val.len == 0
        || (val.len == 3 && ngx_strncmp(val.data, "off", 3) == 0))
    {
        return NGX_DECLINED;
    }

    caches = u->caches->elts; //在proxy_cache_path设置的zone_key中查找有没有对应的共享内存名//keys_zone=fcgi:10m中的fcgi

    for (i = 0; i < u->caches->nelts; i++) {//在u->caches中查找proxy_cache或者fastcgi_cache xxx$ss解析出的xxx$ss字符串，是否有相同的
        name = &caches[i]->shm_zone->shm.name;

        if (name->len == val.len
            && ngx_strncmp(name->data, val.data, val.len) == 0)
        {
            *cache = caches[i];
            return NGX_OK;
        }
    }

    ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                  "cache \"%V\" not found", &val);

    return NGX_ERROR;
}


static ngx_int_t
ngx_http_upstream_cache_send(ngx_http_request_t *r, ngx_http_upstream_t *u)
{
    ngx_int_t          rc;
    ngx_http_cache_t  *c;

    r->cached = 1;
    c = r->cache;

    /*
    root@root:/var/yyz# cat cache_xxx/f/27/46492fbf0d9d35d3753c66851e81627f   封包过程见ngx_http_file_cache_set_header
     3hwhdBw
     KEY: /test2.php
     
     X-Powered-By: PHP/5.2.13
     Content-type: text/html
    //body_start就是上面这一段内存内容长度
    
     <Html> 
     <title>file update</title>
     <body> 
     <form method="post" action="" enctype="multipart/form-data">
     <input type="file" name="file" /> 
     <input type="submit" value="submit" /> 
     </form> 
     </body> 
     </html>
    
     注意第三行哪里其实有8字节的fastcgi表示头部结构ngx_http_fastcgi_header_t，通过vi cache_xxx/f/27/46492fbf0d9d35d3753c66851e81627f可以看出
    
     offset    0  1  2  3   4  5  6  7   8  9  a  b   c  d  e  f  0123456789abcdef
    00000000 <03>00 00 00  ab 53 83 56  ff ff ff ff  2b 02 82 56  ....