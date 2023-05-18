
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <ngx_log.h>

//(proxy_cache_path)等配置走到这里
typedef struct { //赋值见ngx_http_file_cache_set_slot
    //u->caches = &ngx_http_proxy_main_conf_t->caches;
    ngx_array_t                    caches;  /* ngx_http_file_cache_t * */ //ngx_http_file_cache_t成员，最终被赋值给u->caches
} ngx_http_proxy_main_conf_t;


typedef struct ngx_http_proxy_rewrite_s  ngx_http_proxy_rewrite_t;

typedef ngx_int_t (*ngx_http_proxy_rewrite_pt)(ngx_http_request_t *r,
    ngx_table_elt_t *h, size_t prefix, size_t len,
    ngx_http_proxy_rewrite_t *pr);

/*
//如果没有配置proxy_redirect或者配置为proxy_redirect default,则pattern=plcf->url replacement=location(location /xxxx {}中的/xxx),
见ngx_http_proxy_redirect和ngx_http_proxy_merge_loc_conf
*/
struct ngx_http_proxy_rewrite_s { //ngx_http_proxy_redirect中创建该结构空间
    ngx_http_proxy_rewrite_pt      handler; //ngx_http_proxy_rewrite_complex_handler

    //解析proxy_redirect   http://localhost:8000/    http://$host:$server_port/;中的http://localhost:8000/
    union {
        ngx_http_complex_value_t   complex; 
#if (NGX_PCRE)
        ngx_http_regex_t          *regex;
#endif
    } pattern; 
    
    //解析proxy_redirect   http://localhost:8000/    http://$host:$server_port/;中的 http://$host:$server_port/
    ngx_http_complex_value_t       replacement;
};

/*
  proxy_pass  http://10.10.0.103:8080/tttxxsss; 
  浏览器输入:http://10.2.13.167/proxy1111/yangtest
  实际上到后端的uri会变为/tttxxsss/yangtest
  plcf->location:/proxy1111, ctx->vars.uri:/tttxxsss,  r->uri:/proxy1111/yangtest, r->args:, urilen:19
 */
typedef struct {
    //proxy_pass  http://10.10.0.103:8080/tttxx;中的http://10.10.0.103:8080
    ngx_str_t                      key_start; // 初始状态plcf->vars.key_start = plcf->vars.schema;
    /*  "http://" 或者 "https://"  */ //指向的是uri，当时schema->len="http://" 或者 "https://"字符串长度7或者8，参考ngx_http_proxy_pass
    ngx_str_t                      schema; ////proxy_pass  http://10.10.0.103:8080/tttxx;中的http://
    
    ngx_str_t                      host_header;//proxy_pass  http://10.10.0.103:8080/tttxx; 中的10.10.0.103:8080
    ngx_str_t                      port; //"80"或者"443"，见ngx_http_proxy_set_vars
    //proxy_pass  http://10.10.0.103:8080/tttxx; 中的/tttxx         uri不带http://http://10.10.0.103:8080 url带有http://10.10.0.103:8080
    ngx_str_t                      uri; //ngx_http_proxy_set_vars里面赋值   uri是proxy_pass  http://10.10.0.103:8080/xxx中的/xxx，如果
    //proxy_pass  http://10.10.0.103:8080则uri长度为0，没有uri
} ngx_http_proxy_vars_t;


typedef struct {
    ngx_array_t                   *flushes;
     /* 
    把proxy_set_header与ngx_http_proxy_headers合在一起，然后把其中的key:value字符串和长度信息添加到headers->lengths和headers->values中
    把ngx_http_proxy_cache_headers合在一起，然后把其中的key:value字符串和长度信息添加到headers->lengths和headers->values中
     */
    //1. 里面存储的是ngx_http_proxy_headers和proxy_set_header设置的头部信息头添加到该lengths和values 存入ngx_http_proxy_loc_conf_t->headers
    //2. 里面存储的是ngx_http_proxy_cache_headers设置的头部信息头添加到该lengths和values  存入ngx_http_proxy_loc_conf_t->headers_cache
    ngx_array_t                   *lengths; //创建空间和赋值见ngx_http_proxy_init_headers
    ngx_array_t                   *values;  //创建空间和赋值见ngx_http_proxy_init_headers
    ngx_hash_t                     hash;
} ngx_http_proxy_headers_t;


typedef struct {
    ngx_http_upstream_conf_t       upstream;

    /* 下面这几个和proxy_set_body XXX配置相关 proxy_set_body设置包体后，就不会传送客户端发送来的数据到后端服务器(只传送proxy_set_body设置的包体)，
    如果没有设置，则会发送客户端包体数据到后端， 参考ngx_http_proxy_create_request */
    ngx_array_t                   *body_flushes; //从proxy_set_body XXX配置中获取，见ngx_http_proxy_merge_loc_conf
    ngx_array_t                   *body_lengths; //从proxy_set_body XXX配置中获取，见ngx_http_proxy_merge_loc_conf
    ngx_array_t                   *body_values;  //从proxy_set_body XXX配置中获取，见ngx_http_proxy_merge_loc_conf
    ngx_str_t                      body_source; //proxy_set_body XXX配置中的xxx

    //数据来源是ngx_http_proxy_headers  proxy_set_header，见ngx_http_proxy_init_headers
    ngx_http_proxy_headers_t       headers;//创建空间和赋值见ngx_http_proxy_merge_loc_conf
#if (NGX_HTTP_CACHE)
    //数据来源是ngx_http_proxy_cache_headers，见ngx_http_proxy_init_headers
    ngx_http_proxy_headers_t       headers_cache;//创建空间和赋值见ngx_http_proxy_merge_loc_conf
#endif
    //通过proxy_set_header Host $proxy_host;设置并添加到该数组中
    ngx_array_t                   *headers_source; //创建空间和赋值见ngx_http_proxy_init_headers

    //解析uri中的变量的时候会用到下面两个，见ngx_http_proxy_pass  proxy_pass xxx中如果有变量，下面两个就不为空
    ngx_array_t                   *proxy_lengths;
    ngx_array_t                   *proxy_values;
    //proxy_redirect [ default|off|redirect replacement ]; 
    //成员类型ngx_http_proxy_rewrite_t
    ngx_array_t                   *redirects; //和配置proxy_redirect相关，见ngx_http_proxy_redirect创建空间
    ngx_array_t                   *cookie_domains;
    ngx_array_t                   *cookie_paths;

    /* 此配置项表示转发时的协议方法名。例如设置为：proxy_method POST;那么客户端发来的GET请求在转发时方法名也会改为POST */
    ngx_str_t                      method;
    ngx_str_t                      location; //当前location的名字 location xxx {} 中的xxx

    //proxy_pass  http://10.10.0.103:8080/tttxx; 中的http://10.10.0.103:8080/tttxx
    ngx_str_t                      url; //proxy_pass url名字，见ngx_http_proxy_pass

#if (NGX_HTTP_CACHE)
    ngx_http_complex_value_t       cache_key;//proxy_cache_key为缓存建立索引时使用的关键字；见ngx_http_proxy_cache_key
#endif

    ngx_http_proxy_vars_t          vars;//里面保存proxy_pass uri中的uri信息
    //默认1
    ngx_flag_t                     redirect;//和配置proxy_redirect相关，见ngx_http_proxy_redirect
    //proxy_http_version设置，
    ngx_uint_t                     http_version; //到后端服务器采用的http协议版本，默认NGX_HTTP_VERSION_10

    ngx_uint_t                     headers_hash_max_size;
    ngx_uint_t                     headers_hash_bucket_size;

#if (NGX_HTTP_SSL)
    ngx_uint_t                     ssl;
    ngx_uint_t                     ssl_protocols;
    ngx_str_t                      ssl_ciphers;
    ngx_uint_t                     ssl_verify_depth;
    ngx_str_t                      ssl_trusted_certificate;
    ngx_str_t                      ssl_crl;
    ngx_str_t                      ssl_certificate;
    ngx_str_t                      ssl_certificate_key;
    ngx_array_t                   *ssl_passwords;
#endif
} ngx_http_proxy_loc_conf_t;


typedef struct { //ngx_http_proxy_handler中创建空间
    ngx_http_status_t              status; //HTTP/1.1 200 OK 赋值见ngx_http_proxy_process_status_line
    ngx_http_chunked_t             chunked;
    ngx_http_proxy_vars_t          vars;
    off_t                          internal_body_length; //长度为客户端发送过来的包体长度+proxy_set_body设置的字符包体长度和

    ngx_chain_t                   *free;
    ngx_chain_t                   *busy;

    unsigned                       head:1; //到后端的请求行请求时"HEAD"
    unsigned                       internal_chunked:1;
    unsigned                       header_sent:1;
} ngx_http_proxy_ctx_t; //proxy模块的各种上下文信息，例如读取后端数据的时候会有多次交换，这里面会记录状态信息ctx = ngx_http_get_module_ctx(r, ngx_http_proxy_module);


static ngx_int_t ngx_http_proxy_eval(ngx_http_request_t *r,
    ngx_http_proxy_ctx_t *ctx, ngx_http_proxy_loc_conf_t *plcf);
#if (NGX_HTTP_CACHE)
static ngx_int_t ngx_http_proxy_create_key(ngx_http_request_t *r);
#endif
static ngx_int_t ngx_http_proxy_create_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_proxy_reinit_request(ngx_http_request_t *r);
static ngx_int_t ngx_http_proxy_body_output_filter(void *data, ngx_chain_t *in);
static ngx_int_t ngx_http_proxy_process_status_line(ngx_http_request_t *r);
static ngx_int_t ngx_http_proxy_process_header(ngx_http_request_t *r);
static ngx_int_t ngx_http_proxy_input_filter_init(void *data);
static ngx_int_t ngx_http_proxy_copy_filter(ngx_event_pipe_t *p,
    ngx_buf_t *buf);
static ngx_int_t ngx_http_proxy_chunked_filter(ngx_event_pipe_t *p,
    ngx_buf_t *buf);
static ngx_int_t ngx_http_proxy_non_buffered_copy_filter(void *data,
    ssize_t bytes);
static ngx_int_t ngx_http_proxy_non_buffered_chunked_filter(void *data,
    ssize_t bytes);
static void ngx_http_proxy_abort_request(ngx_http_request_t *r);
static void ngx_http_proxy_finalize_request(ngx_http_request_t *r,
    ngx_int_t rc);

static ngx_int_t ngx_http_proxy_host_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_proxy_port_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t
    ngx_http_proxy_add_x_forwarded_for_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t
    ngx_http_proxy_internal_body_length_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_proxy_internal_chunked_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_proxy_rewrite_redirect(ngx_http_request_t *r,
    ngx_table_elt_t *h, size_t prefix);
static ngx_int_t ngx_http_proxy_rewrite_cookie(ngx_http_request_t *r,
    ngx_table_elt_t *h);
static ngx_int_t ngx_http_proxy_rewrite_cookie_value(ngx_http_request_t *r,
    ngx_table_elt_t *h, u_char *value, ngx_array_t *rewrites);
static ngx_int_t ngx_http_proxy_rewrite(ngx_http_request_t *r,
    ngx_table_elt_t *h, size_t prefix, size_t len, ngx_str_t *replacement);

static ngx_int_t ngx_http_proxy_add_variables(ngx_conf_t *cf);
static void *ngx_http_proxy_create_main_conf(ngx_conf_t *cf);
static void *ngx_http_proxy_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_proxy_merge_loc_conf(ngx_conf_t *cf,
    void *parent, void *child);
static ngx_int_t ngx_http_proxy_init_headers(ngx_conf_t *cf,
    ngx_http_proxy_loc_conf_t *conf, ngx_http_proxy_headers_t *headers,
    ngx_keyval_t *default_headers);

static char *ngx_http_proxy_pass(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_proxy_redirect(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_proxy_cookie_domain(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_proxy_cookie_path(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_proxy_store(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
#if (NGX_HTTP_CACHE)
static char *ngx_http_proxy_cache(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_http_proxy_cache_key(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
#endif
#if (NGX_HTTP_SSL)
static char *ngx_http_proxy_ssl_password_file(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
#endif

static char *ngx_http_proxy_lowat_check(ngx_conf_t *cf, void *post, void *data);

static ngx_int_t ngx_http_proxy_rewrite_regex(ngx_conf_t *cf,
    ngx_http_proxy_rewrite_t *pr, ngx_str_t *regex, ngx_uint_t caseless);

#if (NGX_HTTP_SSL)
static ngx_int_t ngx_http_proxy_set_ssl(ngx_conf_t *cf,
    ngx_http_proxy_loc_conf_t *plcf);
#endif
static void ngx_http_proxy_set_vars(ngx_url_t *u, ngx_http_proxy_vars_t *v);


static ngx_conf_post_t  ngx_http_proxy_lowat_post =
    { ngx_http_proxy_lowat_check };

//这个决定后端返回的status来决定是否寻找下一个服务器进行连接
static ngx_conf_bitmask_t  ngx_http_proxy_next_upstream_masks[] = {
    { ngx_string("error"), NGX_HTTP_UPSTREAM_FT_ERROR },
    { ngx_string("timeout"), NGX_HTTP_UPSTREAM_FT_TIMEOUT },
    { ngx_string("invalid_header"), NGX_HTTP_UPSTREAM_FT_INVALID_HEADER },
    { ngx_string("http_500"), NGX_HTTP_UPSTREAM_FT_HTTP_500 },
    { ngx_string("http_502"), NGX_HTTP_UPSTREAM_FT_HTTP_502 },
    { ngx_string("http_503"), NGX_HTTP_UPSTREAM_FT_HTTP_503 },
    { ngx_string("http_504"), NGX_HTTP_UPSTREAM_FT_HTTP_504 },
    { ngx_string("http_403"), NGX_HTTP_UPSTREAM_FT_HTTP_403 },
    { ngx_string("http_404"), NGX_HTTP_UPSTREAM_FT_HTTP_404 },
    { ngx_string("updating"), NGX_HTTP_UPSTREAM_FT_UPDATING },
    { ngx_string("off"), NGX_HTTP_UPSTREAM_FT_OFF },
    { ngx_null_string, 0 }
};


#if (NGX_HTTP_SSL)

static ngx_conf_bitmask_t  ngx_http_proxy_ssl_protocols[] = {
    { ngx_string("SSLv2"), NGX_SSL_SSLv2 },
    { ngx_string("SSLv3"), NGX_SSL_SSLv3 },
    { ngx_string("TLSv1"), NGX_SSL_TLSv1 },
    { ngx_string("TLSv1.1"), NGX_SSL_TLSv1_1 },
    { ngx_string("TLSv1.2"), NGX_SSL_TLSv1_2 },
    { ngx_null_string, 0 }
};

#endif


static ngx_conf_enum_t  ngx_http_proxy_http_version[] = {
    { ngx_string("1.0"), NGX_HTTP_VERSION_10 },
    { ngx_string("1.1"), NGX_HTTP_VERSION_11 },
    { ngx_null_string, 0 }
};

ngx_module_t  ngx_http_proxy_module;

/*
Nginx的反向代理模块还提供了很多种配置，如设置连接的超时时间、临时文件如何存储，以及最重要的如何缓存上游服务器响应等功能。这些配置可以通过阅读
ngx_http_proxy_module模块的说明了解，只有深入地理解，才能实现一个高性能的反向代理服务器。本节只是介绍反向代理服务器的基本功能，在第12章中我们
将会深入地探索upstream机制，到那时，读者也许会发现ngx_http_proxy_module模块只是使用upstream机制实现了反向代理功能而已。
*/
static ngx_command_t  ngx_http_proxy_commands[] = {
/*
upstream块
语法：upstream name {...}
配置块：http
upstream块定义了一个上游服务器的集群，便于反向代理中的proxy_pass使用。例如：
upstream backend {
  server backend1.example.com;
  server backend2.example.com;
    server backend3.example.com;
}

server {
  location / {
    proxy_pass  http://backend;
  }
}

语法：proxy_pass URL 
默认值：no 
使用字段：location, location中的if字段 
这个指令设置被代理服务器的地址和被映射的URI，地址可以使用主机名或IP加端口号的形式，例如：


proxy_pass http://localhost:8000/uri/;或者一个unix socket：


proxy_pass http://unix:/path/to/backend.socket:/uri/;路径在unix关键字的后面指定，位于两个冒号之间。
注意：HTTP Host头没有转发，它将设置为基于proxy_pass声明，例如，如果你移动虚拟主机example.com到另外一台机器，然后重新配置正常（监听example.com到一个新的IP），同时在旧机器上手动将新的example.comIP写入/etc/hosts，同时使用proxy_pass重定向到http://example.com, 然后修改DNS到新的IP。
当传递请求时，Nginx将location对应的URI部分替换成proxy_pass指令中所指定的部分，但是有两个例外会使其无法确定如何去替换：


■location通过正则表达式指定；

■在使用代理的location中利用rewrite指令改变URI，使用这个配置可以更加精确的处理请求（break）：

location  /name/ {
  rewrite      /name/([^/] +)  /users?name=$1  break;
  proxy_pass   http://127.0.0.1;
}这些情况下URI并没有被映射传递。
此外，需要标明一些标记以便URI将以和客户端相同的发送形式转发，而不是处理过的形式，在其处理期间：


■两个以上的斜杠将被替换为一个： ”//” 