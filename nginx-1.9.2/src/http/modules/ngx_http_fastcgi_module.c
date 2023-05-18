
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct {
    ngx_array_t                    caches;  /* ngx_http_file_cache_t * */
} ngx_http_fastcgi_main_conf_t;


typedef struct { //创建空间和赋值见ngx_http_fastcgi_init_params
    ngx_array_t                   *flushes;
    ngx_array_t                   *lengths;//fastcgi_param  HTTP_xx  XXX设置的非HTTP_变量的相关变量的长度code添加到这里面
    ngx_array_t                   *values;//fastcgi_param  HTTP_xx  XXX设置的非HTTP_变量的相关变量的value值code添加到这里面
    ngx_uint_t                     number; //fastcgi_param  HTTP_  XXX;环境中通过fastcgi_param设置的HTTP_xx变量个数
    ngx_hash_t                     hash;//fastcgi_param  HTTP_  XXX;环境中通过fastcgi_param设置的HTTP_xx通过hash运算存到该hash表中,实际存到hash中时会把HTTP_这5个字符去掉
} ngx_http_fastcgi_params_t; //ngx_http_fastcgi_loc_conf_t->params(params_source)中存储


typedef struct {
    ngx_http_upstream_conf_t       upstream; //例如fastcgi在ngx_http_fastcgi_pass中创建upstream空间，ngx_http_xxx_pass

    ngx_str_t                      index;
    //在ngx_http_fastcgi_init_params中通过脚本解析引擎把变量code添加到params中
    ngx_http_fastcgi_params_t      params; //Params数据包，用于传递执行页面所需要的参数和环境变量。
#if (NGX_HTTP_CACHE)
    ngx_http_fastcgi_params_t      params_cache;
#endif

    //fastcgi_param设置的传送到FastCGI服务器的相关参数都添加到该数组中，见ngx_http_upstream_param_set_slot
    ngx_array_t                   *params_source;  //最终会在ngx_http_fastcgi_init_params中通过脚本解析引擎把变量code添加到params中

    /*
    Sets a string to search for in the error stream of a response received from a FastCGI server. If the string is found then 
    it is considered that the FastCGI server has returned an invalid response. This allows handling application errors in nginx, for example: 
    
    location /php {
        fastcgi_pass backend:9000;
        ...
        fastcgi_catch_stderr "PHP Fatal error";
        fastcgi_next_upstream error timeout invalid_header;
    }
    */ //如果后端返回的fastcgi ERRSTD信息中的data部分带有fastcgi_catch_stderr配置的字符串，则会请求下一个后端服务器 参考ngx_http_fastcgi_process_header
    ngx_array_t                   *catch_stderr; //fastcgi_catch_stderr xxx_catch_stderr

    //在ngx_http_fastcgi_eval中执行对应的code，从而把相关变量转换为普通字符串   
    //赋值见ngx_http_fastcgi_pass
    ngx_array_t                   *fastcgi_lengths; //fastcgi相关参数的长度code  如果fastcgi_pass xxx中有变量，则该数组为空
    ngx_array_t                   *fastcgi_values; //fastcgi相关参数的值code

    ngx_flag_t                     keep_conn; //fastcgi_keep_conn  on | off  默认off

#if (NGX_HTTP_CACHE)
    ngx_http_complex_value_t       cache_key;
    //fastcgi_cache_key proxy_cache_key指令的时候计算出来的复杂表达式结构，存放在flcf->cache_key中 ngx_http_fastcgi_cache_key ngx_http_proxy_cache_key
#endif

#if (NGX_PCRE)
    ngx_regex_t                   *split_regex;
    ngx_str_t                      split_name;
#endif
} ngx_http_fastcgi_loc_conf_t;

//http://my.oschina.net/goal/blog/196599
typedef enum { //对应ngx_http_fastcgi_header_t的各个字段   参考ngx_http_fastcgi_process_record解析过程 组包过程ngx_http_fastcgi_create_request
    //fastcgi头部
    ngx_http_fastcgi_st_version = 0,
    ngx_http_fastcgi_st_type,
    ngx_http_fastcgi_st_request_id_hi,
    ngx_http_fastcgi_st_request_id_lo,
    ngx_http_fastcgi_st_content_length_hi,
    ngx_http_fastcgi_st_content_length_lo,
    ngx_http_fastcgi_st_padding_length,
    ngx_http_fastcgi_st_reserved,
    
    ngx_http_fastcgi_st_data, //fastcgi内容
    ngx_http_fastcgi_st_padding //8字节对齐填充字段
} ngx_http_fastcgi_state_e; //fastcgi报文格式，头部(8字节)+内容(一般是8内容头部+数据)+填充字段(8字节对齐引起的填充字节数) 
 

typedef struct {
    u_char                        *start;
    u_char                        *end;
} ngx_http_fastcgi_split_part_t; //创建和赋值见ngx_http_fastcgi_process_header  如果一次解析fastcgi头部行信息没完成，需要再次读取后端数据解析

//在解析从后端发送过来的fastcgi头部信息的时候用到，见ngx_http_fastcgi_process_header
typedef struct { //ngx_http_fastcgi_handler分配空间
//用他来记录每次读取解析过程中的各个状态(如果需要多次epoll触发读取，就需要记录前面读取解析时候的状态)f = ngx_http_get_module_ctx(r, ngx_http_fastcgi_module);
    ngx_http_fastcgi_state_e       state; //标识解析到了fastcgi 8字节头部中的那个地方
    u_char                        *pos; //指向要解析内容的头
    u_char                        *last;//指向要解析内容的尾部
    ngx_uint_t                     type; //交互标识，例如NGX_HTTP_FASTCGI_STDOUT等
    size_t                         length; //该条fastcgi信息的包体内容长度 不包括padding填充
    size_t                         padding; //填充了多少个字节，从而8字节对齐

    ngx_chain_t                   *free;
    ngx_chain_t                   *busy;

    unsigned                       fastcgi_stdout:1; //标识有收到fastcgi stdout标识信息
    unsigned                       large_stderr:1; //标识有收到fastcgi stderr标识信息
    unsigned                       header_sent:1;
    //创建和赋值见ngx_http_fastcgi_process_header  如果一次解析fastcgi头部行信息没完成，需要再次读取后端数据解析
    ngx_array_t                   *split_parts;

    ngx_str_t                      script_name;
    ngx_str_t                      path_info;
} ngx_http_fastcgi_ctx_t; 
//用他来记录每次读取解析过程中的各个状态(如果需要多次epoll触发读取，就需要记录前面读取解析时候的状态)f = ngx_http_get_module_ctx(r, ngx_http_fastcgi_module);

#define NGX_HTTP_FASTCGI_KEEP_CONN      1  //NGX_HTTP_FASTCGI_RESPONDER标识的fastcgi header中的flag为该值表示和后端使用长连接

//FASTCGI交互流程标识，可以参考http://my.oschina.net/goal/blog/196599
#define NGX_HTTP_FASTCGI_RESPONDER      1 //到后端服务器的标识信息 参考ngx_http_fastcgi_create_request  这个标识携带长连接还是短连接ngx_http_fastcgi_request_start

#define NGX_HTTP_FASTCGI_BEGIN_REQUEST  1 //到后端服务器的标识信息 参考ngx_http_fastcgi_create_request  请求开始 ngx_http_fastcgi_request_start
#define NGX_HTTP_FASTCGI_ABORT_REQUEST  2 
#define NGX_HTTP_FASTCGI_END_REQUEST    3 //后端到nginx 参考ngx_http_fastcgi_process_record
#define NGX_HTTP_FASTCGI_PARAMS         4 //到后端服务器的标识信息 参考ngx_http_fastcgi_create_request 客户端请求行中的HTTP_xx信息和fastcgi_params参数通过他发送
#define NGX_HTTP_FASTCGI_STDIN          5 //到后端服务器的标识信息 参考ngx_http_fastcgi_create_request  客户端发送到服务端的包体用这个标识

#define NGX_HTTP_FASTCGI_STDOUT         6 //后端到nginx 参考ngx_http_fastcgi_process_record  该标识一般会携带数据，通过解析到的ngx_http_fastcgi_ctx_t->length表示数据长度
#define NGX_HTTP_FASTCGI_STDERR         7 //后端到nginx 参考ngx_http_fastcgi_process_record
#define NGX_HTTP_FASTCGI_DATA           8  


/*
typedef struct {     
unsigned char version;     
unsigned char type;     
unsigned char requestIdB1;     
unsigned char requestIdB0;     
unsigned char contentLengthB1;     
unsigned char contentLengthB0;     
unsigned char paddingLength;     //填充字节数
unsigned char reserved;    

unsigned char contentData[contentLength]; //内容不符
unsigned char paddingData[paddingLength];  //填充字符
} FCGI_Record; 

*/
//fastcgi报文格式，头部(8字节)+内容(一般是8内容头部+数据)+填充字段(8字节对齐引起的填充字节数)  可以参考http://my.oschina.net/goal/blog/196599
typedef struct { //解析的时候对应前面的ngx_http_fastcgi_state_e
    u_char  version;
    u_char  type; //NGX_HTTP_FASTCGI_BEGIN_REQUEST  等
    u_char  request_id_hi;//序列号，请求应答一般一致
    u_char  request_id_lo;
    u_char  content_length_hi; //内容字节数
    u_char  content_length_lo;
    u_char  padding_length; //填充字节数
    u_char  reserved;//保留字段
} ngx_http_fastcgi_header_t; //   参考ngx_http_fastcgi_process_record解析过程 组包过程ngx_http_fastcgi_create_request


typedef struct {
    u_char  role_hi;
    u_char  role_lo; //NGX_HTTP_FASTCGI_RESPONDER或者0
    u_char  flags;//NGX_HTTP_FASTCGI_KEEP_CONN或者0  如果设置了和后端长连接flcf->keep_conn则为NGX_HTTP_FASTCGI_KEEP_CONN否则为0，见ngx_http_fastcgi_create_request
    u_char  reserved[5];
} ngx_http_fastcgi_begin_request_t;//包含在ngx_http_fastcgi_request_start_t


typedef struct {
    u_char  version;
    u_char  type;
    u_char  request_id_hi;
    u_char  request_id_lo;
} ngx_http_fastcgi_header_small_t; //包含在ngx_http_fastcgi_request_start_t


typedef struct {
    ngx_http_fastcgi_header_t         h0;//请求开始头包括正常头，加上开始请求的头部，
    ngx_http_fastcgi_begin_request_t  br;
    
    //这是什么