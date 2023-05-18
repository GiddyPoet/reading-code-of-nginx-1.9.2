
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *ngx_http_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static ngx_int_t ngx_http_init_phases(ngx_conf_t *cf,
    ngx_http_core_main_conf_t *cmcf);
static ngx_int_t ngx_http_init_headers_in_hash(ngx_conf_t *cf,
    ngx_http_core_main_conf_t *cmcf);
static ngx_int_t ngx_http_init_phase_handlers(ngx_conf_t *cf,
    ngx_http_core_main_conf_t *cmcf);

static ngx_int_t ngx_http_add_addresses(ngx_conf_t *cf,
    ngx_http_core_srv_conf_t *cscf, ngx_http_conf_port_t *port,
    ngx_http_listen_opt_t *lsopt);
static ngx_int_t ngx_http_add_address(ngx_conf_t *cf,
    ngx_http_core_srv_conf_t *cscf, ngx_http_conf_port_t *port,
    ngx_http_listen_opt_t *lsopt);
static ngx_int_t ngx_http_add_server(ngx_conf_t *cf,
    ngx_http_core_srv_conf_t *cscf, ngx_http_conf_addr_t *addr);

static char *ngx_http_merge_servers(ngx_conf_t *cf,
    ngx_http_core_main_conf_t *cmcf, ngx_http_module_t *module,
    ngx_uint_t ctx_index);
static char *ngx_http_merge_locations(ngx_conf_t *cf,
    ngx_queue_t *locations, void **loc_conf, ngx_http_module_t *module,
    ngx_uint_t ctx_index);
static ngx_int_t ngx_http_init_locations(ngx_conf_t *cf,
    ngx_http_core_srv_conf_t *cscf, ngx_http_core_loc_conf_t *pclcf);
static ngx_int_t ngx_http_init_static_location_trees(ngx_conf_t *cf,
    ngx_http_core_loc_conf_t *pclcf);
static ngx_int_t ngx_http_cmp_locations(const ngx_queue_t *one,
    const ngx_queue_t *two);
static ngx_int_t ngx_http_join_exact_locations(ngx_conf_t *cf,
    ngx_queue_t *locations);
static void ngx_http_create_locations_list(ngx_queue_t *locations,
    ngx_queue_t *q);
static ngx_http_location_tree_node_t *
    ngx_http_create_locations_tree(ngx_conf_t *cf, ngx_queue_t *locations,
    size_t prefix);

static ngx_int_t ngx_http_optimize_servers(ngx_conf_t *cf,
    ngx_http_core_main_conf_t *cmcf, ngx_array_t *ports);
static ngx_int_t ngx_http_server_names(ngx_conf_t *cf,
    ngx_http_core_main_conf_t *cmcf, ngx_http_conf_addr_t *addr);
static ngx_int_t ngx_http_cmp_conf_addrs(const void *one, const void *two);
static int ngx_libc_cdecl ngx_http_cmp_dns_wildcards(const void *one,
    const void *two);

static ngx_int_t ngx_http_init_listening(ngx_conf_t *cf,
    ngx_http_conf_port_t *port);
static ngx_listening_t *ngx_http_add_listening(ngx_conf_t *cf,
    ngx_http_conf_addr_t *addr);
static ngx_int_t ngx_http_add_addrs(ngx_conf_t *cf, ngx_http_port_t *hport,
    ngx_http_conf_addr_t *addr);
#if (NGX_HAVE_INET6)
static ngx_int_t ngx_http_add_addrs6(ngx_conf_t *cf, ngx_http_port_t *hport,
    ngx_http_conf_addr_t *addr);
#endif

ngx_uint_t   ngx_http_max_module; //二级模块类型http模块个数，见ngx_http_block  ngx_max_module为NGX_CORE_MODULE(一级模块类型)类型的模块数

/*
当执行ngx_http_send_header发送HTTP头部时，就从ngx_http_top_header_filter指针开始遍历所有的HTTP头部过滤模块，
而在执行ngx_http_output_filter发送HTTP包体时，就从ngx_http_top_body_filter指针开始遍历所有的HTTP包体过滤模块
*/

//包体通过ngx_http_output_filter循环发送

/*
 注意对于HTTP过滤模块来说，在ngx_modules数组中的位置越靠后，在实陈执行请求时就越优先执行。因为在初始化HTTP过滤模块时，每一个http
 过滤模块都是将自己插入到整个单链表的首部的。
*/

//ngx_http_header_filter_module是最后一个header filter模块(ngx_http_top_header_filter = ngx_http_header_filter;他是最后发送头部的地方)，
//ngx_http_write_filter_module是最后一个包体writer模块(ngx_http_top_body_filter = ngx_http_write_filter;),他是最后放包体的地方
//调用ngx_http_output_filter方法即可向客户端发送HTTP响应包体，ngx_http_send_header发送响应行和响应头部
ngx_http_output_header_filter_pt  ngx_http_top_header_filter;//所有的HTTP头部过滤模块都添加到该指针上 ngx_http_send_header中调用链表中所有处理方法
//该函数中的所有filter通过ngx_http_output_filter开始执行
ngx_http_output_body_filter_pt    ngx_http_top_body_filter;//所有的HTTP包体部分都是添加到该指针中,在ngx_http_output_filter中一次调用链表中的各个函数

ngx_http_request_body_filter_pt   ngx_http_top_request_body_filter; //赋值未ngx_http_request_body_save_filter

ngx_str_t  ngx_http_html_default_types[] = {
    ngx_string("text/html"),
    ngx_null_string
};

static ngx_command_t  ngx_http_commands[] = {

    { ngx_string("http"),
      NGX_MAIN_CONF|NGX_CONF_BLOCK|NGX_CONF_NOARGS,
      ngx_http_block,
      0,
      0,
      NULL },

      ngx_null_command
};


static ngx_core_module_t  ngx_http_module_ctx = {
    ngx_string("http"),
    NULL,
    NULL
};

//http{}外的配置见ngx_core_module_ctx， http{}内的配置见ngx_http_module
//NGX_CORE_MODULE的模块在ngx_init_cycle中执行
////一旦在nginx.conf配置文件中找到ngx_http_module感兴趣的“http{}，ngx_http_module模块就开始工作了
ngx_module_t  ngx_http_module = {
    NGX_MODULE_V1,
    &ngx_http_module_ctx,                  /* module context */
    ngx_http_commands,                     /* module directives */
    NGX_CORE_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

/*
4.3.1  解析HTTP配置的流程
    图4-1是HTTP框架解析配置项的示意流程图（图中出现了ngx_http_module和ngx_
http_core_module模块，所谓的HTTP框架主要由这两个模块组成），下面解释图中每个流程
的意义。
    1)图4-1中的主循环是指Nginx进程的主循环，主循环只有调用配置文件解析器才能
解析nginx.conf文件（这里的“主循环”是指解析全部配置文件的循环代码，图8-6的第4
步，为了便于理解，可以认为是Nginx框架代码在循环解析配置项）。
    2)当发现配置文件中含有http{）关键字时，HTTP框架开始启动，这一过程详见10.7
节描述的ngx_http_block方法。
    3) HTTP框架会初始化所有HTTP模块的序列号，并创建3个数组用于存储所有HTTP
模块的create―main- conf、create―srv―conf、create―loc―conf方法返回的指针地址，并把这3
个教组的地址保存到ngx_http_conf_ ctx-t结构中。
    4)调用每个HTTP模块（当然也包括例子中的mytest模块）的create main conf.
create―srv_conf. create一loc―conf（如果实现的话）方法。
    5)把各HTTP模块上述3个方法返回的地址依次保存到ngx_http_conf ctx_t结构体的
3个数组中。
    6)调用每个HTTP模块的preconfiguration方法（如果实现的话）。
    7)注意，如果preconfiguration返回失败，那么Nginx进程将会停止。
    8) HTTP框架开始循环解析nginx.conf文件中http{．．．}里面的所有配置项，
过程到第19步才会返回。
    9)配置文件解析器在检测到1个配置项后，会遍历所有的HTTP模块，
ngx_command_t数组中的name项是否与配置项名相同。
注意，这个
检查它们的
    10)如果找到有1个HTTP模块（如mytest模块）对这个配置项感兴趣（如test- myconfig
配置项），就调用ngx_command_t结构中的set方法来处理。
    11) set方法返回是否处理成功。如果处理失败，那么Nginx进程会停止。
    12)配置文件解析器继续检测配置项。如果发现server{．．．）配置项，就会调用ngx_http_
core__ module模块来处理。因为ngx_http_core__ module模块明确表示希望处理server{}块下
的配置项。注意，这次调用到第18步才会返回。
    13) ngx_http_core_module棋块在解析server{...}之前，也会如第3步一样建立ngx_
http_conf_ctx_t结构，并建立数组保存所有HTTP模块返回的指针地址。然后，它会调用每
个HTTP模块的create―srv_ conf、create- loc―conf方法（如果实现的话）。
    14)将上一步各HTTP模块返回的指针地址保存到ngx_http_conf_ ctx-t对应的数组中。
    15)开始调用配置文件解析器来处理server{．．．}里面的配置项，注意，这个过程在第17
步返回。
    16)继续重复第9步的过程，遍历nginx.conf中当前server{．．．）内的所有配置项。
    17)配置文件解析器继续解析配置项，发现当前server块已经遍历到尾部，说明server
块内的配置项处理完毕，返回ngx_http_core__ module模块。
    18) http core模块也处理完server配置项了，返回至配置文件解析器继续解析后面的配
置项。
    19)配置文件解析器继续解析配置项，这时发现处理到了http{．．．）的尾部，返回给
HTTP框架继续处理。
    20)在第3步和第13步，以及我们没有列幽来的某些步骤中（如发现其他server块
或者location块），都创建了ngx_http_conf_ ctx_t结构，这时将开始调用merge_srv_conf、
merge_loc_conf等方法合并这些不同块(http、server、location)中每个HTTP模块分配的数
据结构。
    21) HTTP框架处理完毕http配置项（也就是ngx_command_t结构中的set回调方法处
理完毕），返回给配置文件解析器继续处理其他http{．．．}外的配置项。
    22)配置文件解析器处理完所有配置项后会告诉Nginx主循环配置项解析完毕，这时
Nginx才会启动Web服务器。

    注意  图4-1并没有列出解析location{...）块的流程，实际上，解析location与解析
server并没有本质上的区别，为了简化起见，没有把它画到图中。

图形化参考:4.3.1  解析HTTP配置的流程图4-1
*/
//从ngx_http_module模块里面的http命令解析走到这里
/*
cf空间始终在一个地方，就是ngx_init_cycle中的conf，使用中只是简单的修改conf中的ctx指向已经cmd_type类型，然后在解析当前{}后，重新恢复解析当前{}前的配置
参考"http" "server" "location"ngx_http_block  ngx_http_core_server  ngx_http_core_location  ngx_http_core_location
*/
static char *
ngx_http_block(ngx_conf_t *cf, ngx_command_t *cmd, void *conf) 
//这里的cf是从ngx_conf_handler里面的if (cmd->type & NGX_DIRECT_CONF)判断里面确定了该cf为
{//图形化参考:深入理解NGINX中的图9-2  图10-1  图4-2，结合图看,并可以配合http://tech.uc.cn/?p=300看
    char                        *rv;
    ngx_uint_t                   mi, m, s;
    ngx_conf_t                   pcf;
    ngx_http_module_t           *module;
    ngx_http_conf_ctx_t         *ctx;
    ngx_http_core_loc_conf_t    *clcf;
    ngx_http_core_srv_conf_t   **cscfp;
    ngx_http_core_main_conf_t   *cmcf;

    /* the main http context */
    ctx = ngx_pcalloc(cf->pool, sizeof(ngx_http_conf_ctx_t));
    if (ctx == NULL) {
        return NGX_CONF_ERROR;
    }

//conf为ngx_conf_handler中的conf = confp[ngx_modules[i]->ctx_index];也就是conf指向的是ngx_cycle_s->conf_ctx[]，
//所以对conf赋值就是对ngx_cycle_s中的conf_ctx赋值
    *(ngx_http_conf_ctx_t **) conf = ctx; //图形化参考:深入理解NGINX中的图9-2  图10-1  图4-2，结合图看,并可以配合http://tech.uc.cn/?p=300看

    /* count the number of the http modules and set up their indices */

    ngx_http_max_module = 0;
    for (m = 0; ngx_modules[m]; m++) {
        if (ngx_modules[m]->type != NGX_HTTP_MODULE) {
            continue;
        }

        ngx_modules[m]->ctx_index = ngx_http_max_module++;  //二级类型按照在ngx_modules中的顺序排序
    }

    /* the http main_conf context, it is the same in the all http contexts */

    ctx->main_conf = ngx_pcalloc(cf->pool,
                                 sizeof(void *) * ngx_http_max_module);
    if (ctx->main_conf == NULL) {
        return NGX_CONF_ERROR;
    }

    /*
     * the http null srv_conf context, it is used to merge
     * the server{}s' srv_conf's
     */

    ctx->srv_conf = ngx_pcalloc(cf->pool, sizeof(void *) * ngx_http_max_module);
    if (ctx->srv_conf == NULL) {
        return NGX_CONF_ERROR;
    }

    /*
     * the http null loc_conf context, it is used to merge
     * the server{}s' loc_conf's
     */

    ctx->loc_conf = ngx_pcalloc(cf->pool, sizeof(void *) * ngx_http_max_module);
    if (ctx->loc_conf == NULL) {
        return NGX_CONF_ERROR;
    }

    /*
     * create the main_conf's, the null srv_conf's, and the null loc_conf's
     * of the all http modules
     */
    //执行所有ngx_modules[m]->type = NGX_HTTP_MODULE的http模块的crate函数来创建对应模块的conf参数，用于后面保存从配置文件中解析出的参数信息
    //http{}下为所有的NGX_HTTP_MODULES模块开辟了main srv loc空间
    //按照模块类型进行合并  http{} server{} location{}都属于同一个ngx_http_core_module模块，他们的init_main_conf都是一样的
    /*
      http {
           xxxx
           server {
                location /xxx {
                }
           }
      }
      这种情况的配置文件，在执行到http的时候开辟ngx_http_conf_ctx_t会分别调用一次main crv loc_creat，执行到server时开辟ngx_http_conf_ctx_t会调用srv_creat loc_creat, 执行到location时开辟ngx_http_conf_ctx_t会调用一次loc_creat
      所以这种情况会调用1次main_creat 2才srv_creat 3次loc_creat。

      http {
           xxxx
           server {
                location /xxx {
                }
           }

           server {
                location /yyy {
                }
           }
      }
      这种情况的配置文件，在执行到http的时候开辟ngx_http_conf_ctx_t会分别调用一次main crv loc_creat，执行到server时开辟ngx_http_conf_ctx_t会调用srv_creat loc_creat, 执行到location时开辟ngx_http_conf_ctx_t会调用一次loc_creat
      所以这种情况会调用1次main_creat 1+2才srv_creat 1+2+2次loc_creat。 需要ngx_http_block   ngx_http_core_server  ngx_http_core_location配合看代码可以看出来
    */ 
    for (m = 0; ngx_modules[m]; m++) { //注意这里为所有的NGX_HTTP_MODULE开辟了main_conf srv_conf loc_conf空间，也就是在http{}的时候为所有main srv loc开辟了空间
        if (ngx_modules[m]->type != NGX_HTTP_MODULE) { //http{}相关配置结构创建首先需要执行ngx_http_core_module，而后才能执行对应的http子模块
            continue;
        }

        module = ngx_modules[m]->ctx;
        mi = ngx_modules[m]->ctx_index; //mi实际上是依次递增的，见签名的ctx_index赋值处

        if (module->create_main_conf) {
            ctx->main_conf[mi] = module->create_main_conf(cf);
            if (ctx->main_conf[mi] == NULL) {
                return NGX_CONF_ERROR;
            }
        }

        if (module->create_srv_conf) {
            ctx->srv_conf[mi] = module->create_srv_conf(cf);
            if (ctx->srv_conf[mi] == NULL) {
                return NGX_CONF_ERROR;
            }
        }

        if (module->create_loc_conf) {
            ctx->loc_conf[mi] = module->create_loc_conf(cf);
            if (ctx->loc_conf[mi] == NULL) {
                return NGX_CONF_ERROR;
            }
        }
    }

    pcf = *cf; //零时保存在解析到http{}时候,在这之前的cf
    cf->ctx = ctx;//零时指向这块新分配的ctx,为存储ngx_http_core_commands开辟的空间

    //执行各个模块的preconfiguration
    for (m = 0; ngx_modules[m]; m++) {
        if (ngx_modules[m]->type != NGX_HTTP_MODULE) {
            continue;
        }

        module = ngx_modules[m]->ctx;

        if (module->preconfiguration) {
            if (module->preconfiguration(cf) != NGX_OK) {
                return NGX_CONF_ERROR;
            }
        }
    }

    /* parse inside the http{} block */

    cf->module_type = NGX_HTTP_MODULE;
    cf->cmd_type = NGX_HTTP_MAIN_CONF;
    rv = ngx_conf_parse(cf, NULL);  
    if (rv != NGX_CONF_OK) {
        goto failed;
    }

    /*
     * init http{} main_conf's, merge the server{}s' srv_conf's
     * and its location{}s' loc_conf's
     */

    cmcf = ctx->main_conf[ngx_http_core_module.ctx_index]; //见ngx_http_core_create_main_conf
    cscfp = cmcf->servers.elts;//一级main_conf中的server中保存的所有二级server结构信息

    
    for (m = 0; ngx_modules[m]; m++) { //按照模块类型进行合并  http{} server{} location{}都属于同一个ngx_http_core_module模块，他们的init_main_conf都是一样的
        if (ngx_modules[m]->type != NGX_HTTP_MODULE) {
            continue;
        }

        module = ngx_modules[m]->ctx;
        mi = ngx_modules[m]->ctx_index;

        /* init http{} main_conf's */

        if (module->init_main_conf) {
            rv = module->init_main_conf(cf, ctx->main_conf[mi]); //见ngx_http_core_init_main_conf
            if (rv != NGX_CONF_OK) {
                goto failed;
            }
        }

        //cf->ctx为http{}的上下文ctx,cmcf为server{}中的所有上下文ctx
        rv = ngx_http_merge_servers(cf, cmcf, module, mi);//合并server{}及其以下的local{}
        if (rv != NGX_CONF_OK) {
            goto failed;
        }
    }

    /* create location trees */
    /*
    经过配置的读取之后，所有server都被保存在http core模块的main配置中的servers数组中，而每个server里面的location都被按配置中
    出现的顺序保存在http core模块的loc配置的locations队列中，上面的代码中先对每个server的location进行排序和分类处理，这一步
    发生在 ngx_http_init_location()函数中：
    */
    for (s = 0; s < cmcf->servers.nelts; s++) {
        /*
          clcf是server块下的ngx_http_core_loc_conf_t结构体，locations成员以双向链表关联着隶属于这个server块的所有location块对应的ngx_http_core_loc_conf_t结构体
          */
        //cscfp[]->ctx就是解析到二级server{}时所在的上下文ctx
        clcf = cscfp[s]->ctx->loc_conf[ngx_http_core_module.ctx_index];//每个server中的loc空间，其实他也是该server下location{}中的loc空间的头部，参考ngx_http_add_location

        /*
         将ngx_http_core_loc_conf_t组成的双向链表按照location匹配字符串进行排序。注意：这个操作是递归进行的，如果某个location块下还具有其他location，那么它的locations链表也会被排序
          */
        if (ngx_http_init_locations(cf, cscfp[s], clcf) != NGX_OK) { 
        //srver{}下所有loc空间(包括server自己的以及其下的location),这里的clcf是解析到server{}行的时候创建的loc_conf
            return NGX_CONF_ERROR;
        }

        /*
          根据已经按照location字符串排序过的双向链表，快速地构建静态的二叉查找树。与ngx_http_init_locations方法类似，速个操作也是递归进行的
          */
        /*
        下面的ngx_http_init_static_location_trees函数就会将那些普通的location(就是ngx_http_init_locations中name noname regex以外的location(exact/inclusive))，
        即staticlocation，进行树化(一种三叉树)处理，之所以要做这样的处理，是为了在处理http请求时能高效的搜索的匹配的location配置。
        */
       /*
    根据已经按照location字符串排序过的双向链表，快速地构建静态的三叉查找树。与ngx_http_init_locations方法类似，速个操作也是递归进行的
        */ //clcf中现在只有普通staticlocation
        if (ngx_http_init_static_location_trees(cf, clcf) != NGX_OK) {
            return NGX_CONF_ERROR;
        }
    }

    if (ngx_http_init_phases(cf, cmcf) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    if (ngx_http_init_headers_in_hash(cf, cmcf) != NGX_OK) {
        return NGX_CONF_ERROR;
    }


    for (m = 0; ngx_modules[m]; m++) {
        if (ngx_modules[m]->type != NGX_HTTP_MODULE) {
            continue;
        }

        module = ngx_modules[m]->ctx;

        if (module->postconfiguration) {
            if (module->postconfiguration(cf) != NGX_OK) {
                return NGX_CONF_ERROR;
            }
        }
    }

    if (ngx_http_variables_init_vars(cf) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    /*
     * http{}'s cf->ctx was needed while the configuration merging
     * and in postconfiguration process
     */

    *cf = pcf;//恢复到上层的ngx_conf_s地址


    if (ngx_http_init_phase_handlers(cf, cmcf) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    /* optimize the lists of ports, addresses and server names */
    if (ngx_http_optimize_servers(cf, cmcf, cmcf->ports) != NGX_OK) {
        return NGX_CONF_ERROR;
    }

    return NGX_CONF_OK;

failed:

    *cf = pcf;

    return rv;
}

/*
有7个HTTP阶段(NGX_HTTP_POST_READ_PHASE、NGX_HTTP_SERVER_REWRITE_PHASE、NGX_HTTP_REWRITE_PHASE、NGX_HTTP_PREACCESS_PHASE、
NGX_HTTP_ACCESS_PHASE、NGX_HTTP_CONTENT_PHASE、NGX_HTTP_LOG_PHASE)是允许任何一个HTTP模块实现自己的ngx_http_handler_pt处
理方法，并将其加入到这7个阶段中去的。在调用HTTP模块的postconfiguration方法向这7个阶段中添加处理方法前，需要先将phases数
组中这7个阶段里的handlers动态数组初始化（ngx_array_t类型需要执行ngx_array_init方法初始化），在这一步骤中，遁过调
用ngx_http_init_phases方法来初始化这7个动态数组。

    通过调用所有HTTP模块的postconfiguration方法。HTTP模块可以在这一步骤中将自己的ngx_http_handler_pt处理方法添加到以上7个HTTP阶段中。
这样在具体的每个阶段就可以执行到我们的handler回调
*/
static ngx_int_t
ngx_http_init_phases(ngx_conf_t *cf, ngx_http_core_main_conf_t *cmcf)
{
    if (ngx_array_init(&cmcf->phases[NGX_HTTP_POST_READ_PHASE].handlers,
                       cf->pool, 1, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    if (ngx_array_init(&cmcf->phases[NGX_HTTP_SERVER_REWRITE_PHASE].handlers,
                       cf->pool, 1, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    if (ngx_array_init(&cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers,
                       cf->pool, 1, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    if (ngx_array_init(&cmcf->phases[NGX_HTTP_PREACCESS_PHASE].handlers,
                       cf->pool, 1, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    if (ngx_array_init(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers,
                       cf->pool, 2, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    if (ngx_array_init(&cmcf->phases[NGX_HTTP_CONTENT_PHASE].handlers,
                       cf->pool, 4, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    if (ngx_array_init(&cmcf->phases[NGX_HTTP_LOG_PHASE].handlers,
                       cf->pool, 1, sizeof(ngx_http_handler_pt))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    return NGX_OK;
}

//把ngx_http_headers_in中的所有成员做hash运算，然后存放到cmcf->headers_in_hash中
static ngx_int_t
ngx_http_init_headers_in_hash(ngx_conf_t *cf, ngx_http_core_main_conf_t *cmcf)
{
    ngx_array_t         headers_in;
    ngx_hash_key_t     *hk;
    ngx_hash_init_t     hash;
    ngx_http_header_t  *header;

    if (ngx_array_init(&headers_in, cf->temp_pool, 32, sizeof(ngx_hash_key_t))
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    for (header = ngx_http_headers_in; header->name.len; header++) {
        hk = ngx_array_push(&headers_in);
        if (hk == NULL) {
            return NGX_ERROR;
        }

        hk->key = header->name;
        hk->key_hash = ngx_hash_key_lc(header->name.data, header->name.len);
        hk->value = header;
    }

    hash.hash = &cmcf->headers_in_hash;
    hash.key = ngx_hash_key_lc;
    hash.max_size = 512;
    hash.bucket_size = ngx_align(64, ngx_cacheline_size);
    hash.name = "headers_in_hash";
    hash.pool = cf->pool;
    hash.temp_pool = NULL;

    if (ngx_hash_init(&hash, headers_in.elts, headers_in.nelts) != NGX_OK) {
        return NGX_ERROR;
    }

    return NGX_OK;
}

/*
在各个HTTP模块能够介入的7个阶段中，实际上共享了4个checker方法：ngx_http_core_generic_phase、ngx_http_core_rewrite_phase、
ngx_http_core_access_phase、ngx_http_core_content_phase。这4个checker方法的主要任务在于，根据phase_handler执行某个HTTP模块实现的
回调方法，并根据方法的返回值决定：当前阶段已经完全结束了吗？下次要执行的回调方法是哪一个？究竟是立刻执行下一个回调方法还是先把控制权交还给epoll?
*/
//cmcf->phases[]数组各个阶段的ngx_http_handler_pt节点信息全部赋值给cmcf->phase_engine.handlers数组队列
static ngx_int_t
ngx_http_init_phase_handlers(ngx_conf_t *cf, ngx_http_core_main_conf_t *cmcf)
{
    ngx_int_t                   j;
    ngx_uint_t                  i, n;
    ngx_uint_t                  find_config_index, use_rewrite, use_access;
    ngx_http_handler_pt        *h;
    ngx_http_phase_handler_t   *ph;
    ngx_http_phase_handler_pt   checker;

    cmcf->phase_engine.server_rewrite_index = (ngx_uint_t) -1;
    cmcf->phase_engine.location_rewrite_index = (ngx_uint_t) -1;
    find_config_index = 0;
    use_rewrite = cmcf->phases[NGX_HTTP_REWRITE_PHASE].handlers.nelts ? 1 : 0;
    use_access = cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers.nelts ? 1 : 0;

    n = use_rewrite + use_access + cmcf->try_files + 1 /* find config phase */;

    for (i = 0; i < NGX_HTTP_LOG_PHASE; i++) {
        n += cmcf->phases[i].handlers.nelts;
    }

    ph = ngx_pcalloc(cf->pool,
                     n * sizeof(ngx_http_phase_handler_t) + sizeof(void *));
    if (ph == NULL) {
        return NGX_ERROR;
    }

    cmcf->phase_engine.handlers = ph;
    n = 0;

    for (i = 0; i < NGX_HTTP_LOG_PHASE; i++) {
        h = cmcf->phases[i].handlers.elts;

        switch (i) { //所有阶段的checker在ngx_http_core_run_phases中调用
        /*
        NGX_HTTP_SERVERREWRITEPHASE阶段，和第3阶段NGXHTTPREWRITE_PHASE都属于地址重写，也都是针对rewrite模块而设定的阶段，前
        者用于server上下文里的地址重写，而后者用于location上下文里的地址重写。为什么要设置两个地址重写阶段，原因在于rewrite模块
        的相关指令（比如rewrite、if、set等）既可用于server上下文．又可用于location上下文。在客户端请求被Nginx接收后，首先做server
        查找与定位，在定位到server（如果没查找到就是默认server）后执行NGXHTTP_SERVER_REWRITEPHASE阶段上的回调函数，然后再进入到下
        一个阶段：NGX_HTTP_FIND_CONFIG_PHASE阶段。
          */
        case NGX_HTTP_SERVER_REWRITE_PHASE:
            if (cmcf->phase_engine.server_rewrite_index == (ngx_uint_t) -1) {
                cmcf->phase_engine.server_rewrite_index = n;
            }
            checker = ngx_http_core_rewrite_phase;

            break;

/*
NGX_HTTP_FIND_CONFIG_PHASE、NGX_HTTP_POSTREWRITE_PHASE、NGX_HTTP_POST_ACCESS_PHASE、NGX_HTTP_TRY_FILES_PHASE这4个阶段则
不允许HTTP模块加入自己的ngx_http_handler_pt方法处理用户请求,但是他们的会占用cmcf->phase_engine.handlers[]数组中的一个成员

 NGX_HTTP_FIND_CONFIG_PHASE阶段上不能挂载任何回调函数，因为它们永远也不 会被执行，该阶段完成的是Nginx的特定任务，即进行
 Location定位。只有把当前请求的对应location找到了，才能以该location上下文中取出更多精确的用户配置值，做后续的进一步请求处理。
 */
        case NGX_HTTP_FIND_CONFIG_PHASE: //该阶段则不允许HTTP模块加入自己的ngx_http_handler_pt方法处理用户请求方法，直接把http框架方法加入到cmcf->phase_engine.handlers数组中
            find_config_index = n;

            ph->checker = ngx_http_core_find_config_phase; //
            ph->phase = i;
            n++; //所有处理方法数增加一次
            ph++; //指向cmcf->phase_engine.handlers数组中下一个未用的ngx_http_phase_handler_t节点位置

            continue;

        case NGX_HTTP_REWRITE_PHASE:
            if (cmcf->phase_engine.location_rewrite_index == (ngx_uint_t) -1) {
                cmcf->phase_engine.location_rewrite_index = n;
            }
            checker = ngx_http_core_rewrite_phase;

            break;  


        case NGX_HTTP_POST_REWRITE_PHASE://该阶段则不允许HTTP模块加入自己的ngx_http_handler_pt方法处理用户请求方法，直接把http框架方法加入到cmcf->phase_engine.handlers数组中
            if (use_rewrite) {
                ph->checker = ngx_http_core_post_rewrite_phase;
                ph->next = find_config_index;//注意:NGX_HTTP_POST_REWRITE_PHASE的下一阶段是NGX_HTTP_FIND_CONFIG_PHASE
                ph->phase = i;
                n++;
                ph++;
            }
            continue;

        case NGX_HTTP_ACCESS_PHASE:
            checker = ngx_http_core_access_phase;
            n++;
            break;

        case NGX_HTTP_POST_ACCESS_PHASE://该阶段则不允许HTTP模块加入自己的ngx_http_handler_pt方法处理用户请求方法，直接把http框架方法加入到cmcf->phase_engine.handlers数组中
            if (use_access) {//只有配置了try_files aaa bbb后才会在 cmcf->phase_engine.handlers添加节点pt，见ngx_http_init_phase_handlers，如果没有配置，则直接跳过try_files阶段
                ph->checker = ngx_http_core_post_access_phase;
                ph->next = n; 
                ph->phase = i;
                ph++;
            }

            continue;

        case NGX_HTTP_TRY_FILES_PHASE://该阶段则不允许HTTP模块加入自己的ngx_http_handler_pt方法处理用户请求方法，直接把http框架方法加入到cmcf->phase_engine.handlers数组中
            if (cmcf->try_files) {
                ph->checker = ngx_http_core_try_files_phase;
                ph->phase = i;
                n++;
                ph++;
            }

            continue;

        case NGX_HTTP_CONTENT_PHASE:
            checker = ngx_http_core_content_phase;
            break;

        default: //NGX_HTTP_POST_READ_PHASE   NGX_HTTP_PREACCESS_PHASE  NGX_HTTP_LOG_PHASE默认都是该函数
            checker = ngx_http_core_generic_phase;
        }
        
//计算i阶段的ngx_http_handler_pt处理方法在所有阶段处理方法中的位置，也就是计算i阶段的处理方法应该存放在cmcf->phase_engine.handlers数组中的具体位置
        n += cmcf->phases[i].handlers.nelts; 

        //注意:每一个阶段中最后加入到handlers[]中的会首先添加到cmcf->phase_engine.handlers
        for (j = cmcf->phases[i].handlers.nelts - 1; j >=0; j--) {
            ph->checker = checker; //每个阶段对应的checker，例如:NGX_HTTP_SERVER_REWRITE_PHASE阶段为ngx_http_core_rewrite_phase
            //i阶段的所有ngx_http_handler_pt处理方法通过ph连接起来，也就是全部添加到cmcf->phase_engine.handlers数组中，各个成员通过ph->next连接在一起
            ph->handler = h[j];
            ph->phase = i;
            ph->next = n;//下一阶段的第一个ngx_http_handler_pt处理方法在数组中的位置
            ph++; //指向cmcf->phase_engine.handlers数组中下一个未用的ngx_http_phase_handler_t节点位置
        }
    }

    return NGX_OK;
}

//这里面包括合并location
/*
图4-4包括4重循环，第1层（最外层）遍历所有的HTTP模块(在该函数外面)，第2层遍历所有的
server{．．．)配置块，第3层是遍历某个server{}块中嵌套的所有location{．．．）块，第4层遍历
某个location{）块中继续嵌套的所有location块（实际上，它会一直递归下去以解析可能被
层层嵌套的’location块）。读者可以对照上述4重循环来理解合并配置项的流程图。
*/ //cf->ctx为http{}的上下文ctx,cmcf为server{}中的所有上下文ctx
//结合ngx_http_core_server与ngx_http_core_location一起阅读该段代码
static char *
ngx_http_merge_servers(ngx_conf_t *cf, ngx_http_core_main_conf_t *cmcf,
    ngx_http_module_t *module, ngx_uint_t ctx_index)
{
    char                        *rv;
    ngx_uint_t                   s;
    ngx_http_conf_ctx_t         *ctx, saved;
    ngx_http_core_loc_conf_t    *clcf;
    ngx_http_core_srv_conf_t   **cscfp;

/*
    //这里的cmcf是main和srv都可以出现的配置，但是是出现在http{}内，server{}外的，所以需要与server{}内的相同配置合并
    //cmcf在外层赋值为ctx->main_conf，所以为http{}内，server{}外的server配置项， saved.srv_conf为ctx->srv_conf，所以为server{}内的server配置项
    例如
    http {
        aaa;
        server {
            aaa
        }
    }
    第一个aaa为cscfp存储的aaa,第二个aaa为saved.srv_conf存储的aaa
*/    
    cscfp = cmcf->servers.elts; //server{}中的所有上下文ctx
    ctx = (ngx_http_conf_ctx_t *) cf->ctx;  //ctx为http{}的上下文ctx
    saved = *ctx; //cf->ctx为http{}的上下文ctx
    rv = NGX_CONF_OK;

    for (s = 0; s < cmcf->servers.nelts; s++) { 

        /* merge the server{}s' srv_conf's */
        /*
            例如
            http {
                aaa;
                server {
                    aaa
                }
            }
            第二个aaa为cscfp存储的aaa,第一个aaa为saved.srv_conf存储的aaa
          */
        ctx->srv_conf = cscfp[s]->ctx->srv_conf;//这是server{}中的srv_conf

        if (module->merge_srv_conf) {//以server为例，saved.srv_conf[ctx_index]为http{}中的srv_conf，cscfp[s]->ctx->srv_conf[ctx_index]为server{}中的srv_conf
        //这里的saved.srv_conf是main和srv都可以出现的配置，但是是出现在http{}内，server{}外的，所以需要与server{}内的相同配置合并
            rv = module->merge_srv_conf(cf, saved.srv_conf[ctx_index],
                                        cscfp[s]->ctx->srv_conf[ctx_index]); //把http{}和server{}内的server配置项合并
            if (rv != NGX_CONF_OK) {
                goto failed;
            }
        }

        if (module->merge_loc_conf) {

            /* merge the server{}'s loc_conf */
            /*
                例如
                http {
                    bbb;
                    server {
                        bbb;
                    }

                    location {
                        bbb;
                    }
                }
                第一个bbb为cscfp[s]->ctx->loc_conf存储的bbb,第二个aaa为saved.loc_conf存储的bbb,第三个bbb为
              */

            ctx->loc_conf = cscfp[s]->ctx->loc_conf;

            rv = module->merge_loc_conf(cf, saved.loc_conf[ctx_index],
                                        cscfp[s]->ctx->loc_conf[ctx_index]); //先合并一级和二级loc,也就是http和server中的location配置
            if (rv != NGX_CONF_OK) {
                goto failed;
            }

            /* merge the locations{}' loc_conf's */

            clcf = cscfp[s]->ctx->loc_conf[ngx_http_core_module.ctx_index]; //该server{}里面的所有location配置项头部，见ngx_http_add_location

            //cscfp[s]->ctx->loc_conf存储的是server{}上下文ctx中的loc_conf, clcf->location指向的是所有location{}上下文中的loc_conf
            rv = ngx_http_merge_locations(cf, clcf->locations,
                                          cscfp[s]->ctx->loc_conf,
                                          module, ctx_index); //location递归合并
            if (rv != NGX_CONF_OK) {
                goto failed;
            }
        }
    }

failed:

    *ctx = saved;

    return rv;
}


static char *
ngx_http_merge_locations(ngx_conf_t *cf, ngx_queue_t *locations,
    void **loc_conf, ngx_http_module_t *module, ngx_uint_t ctx_index)
{
    char                       *rv;
    ngx_queue_t                *q;
    ngx_http_conf_ctx_t        *ctx, saved;
    ngx_http_core_loc_conf_t   *clcf;
    ngx_http_location_queue_t  *lq;

    if (locations == NULL) {
        return NGX_CONF_OK;
    }

    ctx = (ngx_http_conf_ctx_t *) cf->ctx;
    saved = *ctx;

    for (q = ngx_queue_head(locations);
         q != ngx_queue_sentinel(locations);
         q = ngx_queue_next(q))
    {
        lq = (ngx_http_location_queue_t *) q;

        clcf = lq->exact ? lq->exact : lq->inclusive;
        ctx->loc_conf = clcf->loc_conf;

        rv = module->merge_loc_conf(cf, loc_conf[ctx_index],
                                    clcf->loc_conf[ctx_index]);
        if (rv != NGX_CONF_OK) {
            return rv;
        }

        rv = ngx_http_merge_locations(cf, clcf->locations, clcf->loc_conf,
                                      module, ctx_index);
        if (rv != NGX_CONF_OK) {
            return rv;
        }
    }

    *ctx = saved;

    return NGX_CONF_OK;
}

/*
= 开头表示精确匹配
^~ 开头表示uri以某个常规字符串开头，理解为匹配 url路径即可。nginx不对url做编码，因此请求为/static/20%/aa，可以被规则^~ /static/ /aa匹配到（注意是空格）。
~ 开头表示区分大小写的正则匹配
~*  开头表示不区分大小写的正则匹配
!~和!~*分别为区分大小写不匹配及不区分大小写不匹配 的正则
/ 通用匹配，任何请求都会匹配到。


location匹配命令

~      #波浪线表示执行一个正则匹配，区分大小写
~*    #表示执行一个正则匹配，不区分大小写
^~    #^~表示普通字符匹配，如果该选项匹配，只匹配该选项，不匹配别的选项，一般用来匹配目录
=      #进行普通字符精确匹配
@     #"@" 定义一个命名的 location，使用在内部定向时，例如 error_page, try_files



location 匹配的优先级(与location在配置文件中的顺序无关)
= 精确匹配会第一个被处理。如果发现精确匹配，nginx停止搜索其他匹配。
普通字符匹配，正则表达式规则和长的块规则将被优先和查询匹配，也就是说如果该项匹配还需去看有没有正则表达式匹配和更长的匹配。
^~ 则只匹配该规则，nginx停止搜索其他匹配，否则nginx会继续处理其他location指令。
最后匹配理带有"~"和"~*"的指令，如果找到相应的匹配，则nginx停止搜索其他匹配；当没有正则表达式或者没有正则表达式被匹配的情况下，那么匹配程度最高的逐字匹配指令会被使用。

location 优先级官方文档

1.Directives with the = prefix that match the query exactly. If found, searching stops.
2.All remaining directives with conventional strings, longest match first. If this match used the ^~ prefix, searching stops.
3.Regular expressions, in order of definition in the configuration file.
4.If #3 yielded a match, that result is used. Else the match from #2 is used.
1.=前缀的指令严格匹配这个查询。如果找到，停止搜索。
2.所有剩下的常规字符串，最长的匹配。如果这个匹配使用^?前缀，搜索停止。
3.正则表达式，在配置文件中定义的顺序。
4.如果第3条规则产生匹配的话，结果被使用。否则，如同从第2条规则被使用。


例如

location  = / {
# 只匹配"/".
[ configuration A ] 
}
location  / {
# 匹配任何请求，因为所有请求都是以"/"开始
# 但是更长字符匹配或者正则表达式匹配会优先匹配
[ configuration B ] 
}
location ^~ /images/ {
# 匹配任何以 /images/ 开始的请求，并停止匹配 其它location
[ configuration C ] 
}
location ~* \.(gif|jpg|jpeg)$ {
# 匹配以 gif, jpg, or jpeg结尾的请求. 
# 但是所有 /images/ 目录的请求将由 [Configuration C]处理.   
[ configuration D ] 
}请求URI例子:

?/ -> 符合configuration A
?/documents/document.html -> 符合configuration B
?/images/1.gif -> 符合configuration C
?/documents/1.jpg ->符合 configuration D
@location 例子
error_page 404 = @fetch;

location @fetch(
proxy_pass http://fetch;
)
*/
//ngx_http_add_location
//cscf为server{}配置里面的srv_conf,pclcf->locations为该server{}类里面的所有(包括解析server行的时候开辟的local_crate和解析location行的时候开辟的loc_creat空间)loc_conf头部
//参考:http://blog.csdn.net/fengmo_q/article/details/6683377和http://tech.uc.cn/?p=300
static ngx_int_t
ngx_http_init_locations(ngx_conf_t *cf, ngx_http_core_srv_conf_t *cscf,
    ngx_http_core_loc_conf_t *pclcf) //形成3叉树见ngx_http_init_static_location_trees
{
    ngx_uint_t                   n;
    ngx_queue_t                 *q, *locations, 
    *named, 
    tail;//最后name noname regex都会连接到tail链表中
    ngx_http_core_loc_conf_t    *clcf;
    ngx_http_location_queue_t   *lq;
    ngx_http_core_loc_conf_t   **clcfp;
#if (NGX_PCRE)
    ngx_uint_t                   r;
    ngx_queue_t                 *regex;
#endif

    //这是合并过的loc配置项
    locations = pclcf->locations;//存储在server{}中创建的ctx->loc_conf中的是location{}创建的ctx->loc_conf,所以pclcf为server{}中的loc_conf
    //locations是所有的location{}项中创建的loc_conf,所以这两个之和就是server{}下所有的location配置项

    if (locations == NULL) {
        return NGX_OK;
    }
    //如果location = /aaa {}  location ^~ /aaa {} 这两个的uri那么是一样的，他们分别存储在一个ngx_http_core_loc_conf_t中，怎么区分呢? 就通过exact_match noregex来区分，并且在队列中完全匹配在前面
    /* 
      按照类型排序location，排序完后的队列：  (exact_match 或 inclusive(前缀匹配)) (排序好的，如果某个exact_match名字和inclusive location相同，exact_match排在前面) 
      |  regex（未排序）| named(排序好的)  |  noname（未排序）
      */
    /*
    例如:
        http {
            server {
                location { #1
                    location {#2

                    }

                    location {#3

                    }
                }

                location {#4
                    location {#5

                    }

                    location {#6

                    }
                }
            }

        }
     */ //locations是直属于server{}的location{}块(上面的#1 #2)， 或者直属于#1的#2 #3,或者直属于#4中的#5 #6。所以需要三次调用ngx_queue_sort
    //排序完后进行location切分。location排序完，整个list的结构是：前缀匹配|绝对匹配--->正则匹配--->命名--> 未命名
    ngx_queue_sort(locations, ngx_http_cmp_locations);
    
    named = NULL;
    n = 0;
#if (NGX_PCRE)
    regex = NULL;
    r = 0;
#endif

    
    for (q = ngx_queue_head(locations);
         q != ngx_queue_sentinel(locations);
         q = ngx_queue_next(q)) //找到queue中正则表达式和命名location中的位置
    {
        lq = (ngx_http_location_queue_t *) q;

        clcf = lq->exact ? lq->exact : lq->inclusive;
        
        /* 由于可能存在nested location，也就是location里面嵌套的location，这里需要递归的处理一下当前location下面的nested location */  
        if (ngx_http_init_locations(cf, NULL, clcf) != NGX_OK) { //这里是一个递归，如果存在location下面还有locations的话还会进行递归调用
            return NGX_ERROR;
        }

#if (NGX_PCRE)

        if (clcf->regex) { 
            r++; //location 后为正则表达式的个数

            if (regex == NULL) {
                regex = q;//记录链表中的regex正则表达式头
            }

            continue;
        }

#endif
     /*
        location / {
          try_files index.html index.htm @fallback;
        }
        
        location @fallback {
          root /var/www/error;
          index index.html;
        }
    */
        if (clcf->named) { 
            n++; //location后为@name的location个数

            if (named == NULL) {
                named = q;//记录链表中的命名location头named  
            }

            continue;
        }

        if (clcf->noname) { //只要遇到name，则直接返回，在后面拆分。注意未命名的在队列尾部，所以不会影响到前面的named和regx
            break;
        }
    }

    /*
    先分离noname(没有像后面两个那样集中管理)，在分离named(保存到cscf->named_locations)，最后分离regex(保存到pclcf->regex_locations= clcfp)，
    分离这些另类之后，我们处理那些普通的location，普通的location大家应该知道是指的哪些，nginx称为static location。
    */
    if (q != ngx_queue_sentinel(locations)) { //如果队列里面有noname，则以该noname头拆分为locations和tail两个队列链表
        ngx_queue_split(locations, q, &tail);//未命名的拆分出来  noname named regex都连接到tail链表中
    } 
    //实际上这里拆分出的noname放入tail后，如果locations中包含named location,则noname和name放一起(cscf->named_locations)，如果没有named但是有regex，
    //则和regex放一起(pclcf->regex_locations)，如果name和regex和都没有，则和普通locations放一起(pclcf->locations)
    if (named) { 
        clcfp = ngx_palloc(cf->pool,
                           (n + 1) * sizeof(ngx_http_core_loc_conf_t *));
        if (clcfp == NULL) {
            return NGX_ERROR;
        }
        
        /* 如果有named location，将它们保存在所属server的named_locations数组中 */  
        //named_locations的每个成员指向named，，要注意最后一个named的成员的next链表上有连接noname location项目
        cscf->named_locations = clcfp; /* 所有的location @name {}这种ngx_http_core_loc_conf_t全部指向named_locations */

        for (q = named;
             q != ngx_queue_sentinel(locations);
             q = ngx_queue_next(q))
        {
            lq = (ngx_http_location_queue_t *) q;

            *(clcfp++) = lq->exact;
        }

        *clcfp = NULL;

        //拆分到tail中的节点pclcf->regex_locations 数组会指向他们，从而保证了pclcf->regex_locations和 cscf->named_locations中的所有location通过ngx_http_location_queue_t是连接在一起的
        //拆分到tail中的节点cscf->named_locations数组会指向他们
        ngx_queue_split(locations, named, &tail); //以name类型的头部位置拆分locations，并且把拆分的后半部分放入tail头部中
    }

#if (NGX_PCRE)

    if (regex) {

        clcfp = ngx_palloc(cf->pool,
                           (r + 1) * sizeof(ngx_http_core_loc_conf_t *));
        if (clcfp == NULL) {
            return NGX_ERROR;
        }
        
        /* 如果有正则匹配location，将它们保存在所属server的http core模块的loc配置的regex_locations 数组中， 
             这里和named location保存位置不同的原因是由于named location只能存在server里面，而regex location可以作为nested location */   
        pclcf->regex_locations = clcfp; /* 所有的location 正则表达式 {}这种ngx_http_core_loc_conf_t全部指向regex_locations */

        for (q = regex;
             q != ngx_queue_sentinel(locations);
             q = ngx_queue_next(q))
        {
            lq = (ngx_http_location_queue_t *) q;

            *(clcfp++) = lq->exact;
        }

        *clcfp = NULL;
//拆分到tail中的节点pclcf->regex_locations 数组会指向他们，从而保证了pclcf->regex_locations和 cscf->named_locations中的所有location通过ngx_http_location_queue_t是连接在一起的
        ngx_queue_split(locations, regex, &tail); //按照regex拆分
    }

#endif

    return NGX_OK;
}

/*
下面的ngx_http_init_static_location_trees函数就会将那些普通的location(就是ngx_http_init_locations中name noname regex以外的location)，即staticlocation，进行树化(一种三叉树)处理，
之所以要做这样的处理，是为了在处理http请求时能高效的搜索的匹配的location配置。
*/
/*
注意，这里的二叉查找树并不是红黑树，不过，为什么不使用红黑树呢？因为location是由nginx.conf中读取到的，它是静态不变的，
不存在运行过程中在树中添加或者删除location的场景，而且红黑树的查询效率也没有重新构造的静态的完全平衡二叉树高。
*/
//location tree的建立在ngx_http_init_static_location_trees中进行：
//location树产生排序见ngx_http_init_locations
static ngx_int_t
ngx_http_init_static_location_trees(ngx_conf_t *cf,
    ngx_http_core_loc_conf_t *pclcf) //很好的图解，参考http://blog.csdn.net/fengmo_q/article/details/6683377和http://tech.uc.cn/?p=300
{
    ngx_queue_t                *q, *locations;
    ngx_http_core_loc_conf_t   *clcf;
    ngx_http_location_queue_t  *lq;

    locations = pclcf->locations;

    if (locations == NULL) {
        return NGX_OK;
    }

    if (ngx_queue_empty(locations)) {
        return NGX_OK;
    }

    for (q = ngx_queue_head(locations);
         q != ngx_queue_sentinel(locations);
         q = ngx_queue_next(q))
    {
        lq = (ngx_http_location_queue_t *) q;

        clcf = lq->exact ? lq->exact : lq->inclusive;
        
        /* 这里也是由于nested location，需要递归一下 */  
        if (ngx_http_init_static_location_trees(cf, clcf) != NGX_OK) {
            return NGX_ERROR;
        }
    }
    
    /* 
    join队列中名字相同的inclusive和exact类型location，也就是如果某个exact_match的location名字和普通字符串匹配的location名字相同的话， 
    就将它们合到一个节点中，分别保存在节点的exact和inclusive下，这一步的目的实际是去重，为后面的建立排序树做准备 
    */  
    if (ngx_http_join_exact_locations(cf, locations) != NGX_OK) {
        return NGX_ERROR;
    }

    /* 递归每个location节点，得到当前节点的名字为其前缀的location的列表，保存在当前节点的list字段下 */  
    ngx_http_create_locations_list(locations, ngx_queue_head(locations));

    
    /* 递归建立location三叉排序树 */  
    pclcf->static_locations = ngx_http_create_locations_tree(cf, locations, 0);
    if (pclcf->static_locations == NULL) {
        return NGX_ERROR;
    }

    return NGX_OK;
}
/*
= 开头表示精确匹配
^~ 开头表示uri以某个常规字符串开头，理解为匹配 url路径即可。nginx不对url做编码，因此请求为/static/20%/aa，可以被规则^~ /static/ /aa匹配到（注意是空格）。
~ 开头表示区分大小写的正则匹配
~*  开头表示不区分大小写的正则匹配
!~和!~*分别为区分大小写不匹配及不区分大小写不匹配 的正则
/ 通用匹配，任何请求都会匹配到。


location匹配命令

~      #波浪线表示执行一个正则匹配，区分大小写
~*    #表示执行一个正则匹配，不区分大小写
^~    #^~表示普通字符匹配，如果该选项匹配，只匹配该选项，不匹配别的选项，一般用来匹配目录
=      #进行普通字符精确匹配
@     #"@" 定义一个命名的 location，使用在内部定向时，例如 error_page, try_files



location 匹配的优先级(与location在配置文件中的顺序无关)
= 精确匹配会第一个被处理。如果发现精确匹配，nginx停止搜索其他匹配。
普通字符匹配，正则表达式规则和长的块规则将被优先和查询匹配，也就是说如果该项匹配还需去看有没有正则表达式匹配和更长的匹配。
^~ 则只匹配该规则，nginx停止搜索其他匹配，否则nginx会继续处理其他location指令。
最后匹配理带有"~"和"~*"的指令，如果找到相应的匹配，则nginx停止搜索其他匹配；当没有正则表达式或者没有正则表达式被匹配的情况下，那么匹配程度最高的逐字匹配指令会被使用。

location 优先级官方文档

1.Directives with the = prefix that match the query exactly. If found, searching stops.
2.All remaining directives with conventional strings, longest match first. If this match used the ^~ prefix, searching stops.
3.Regular expressions, in order of definition in the configuration file.
4.If #3 yielded a match, that result is used. Else the match from #2 is used.
1.=前缀的指令严格匹配这个查询。如果找到，停止搜索。
2.所有剩下的常规字符串，最长的匹配。如果这个匹配使用^?前缀，搜索停止。
3.正则表达式，在配置文件中定义的顺序。
4.如果第3条规则产生匹配的话，结果被使用。否则，如同从第2条规则被使用。


例如

location  = / {
# 只匹配"/".
[ configuration A ] 
}
location  / {
# 匹配任何请求，因为所有请求都是以"/"开始
# 但是更长字符匹配或者正则表达式匹配会优先匹配
[ configuration B ] 
}
location ^~ /images/ {
# 匹配任何以 /images/ 开始的请求，并停止匹配 其它location
[ configuration C ] 
}
location ~* \.(gif|jpg|jpeg)$ {
# 匹配以 gif, jpg, or jpeg结尾的请求. 
# 但是所有 /images/ 目录的请求将由 [Configuration C]处理.   
[ configuration D ] 
}请求URI例子:

?/ -> 符合configuration A
?/documents/document.html -> 符合configuration B
?/images/1.gif -> 符合configuration C
?/documents/1.jpg ->符合 configuration D
@location 例子
error_page 404 = @fetch;

location @fetch(
proxy_pass http://fetch;
)
*/

//locations为父级对应的loc_conf[]的location  clcf为本级的loc_conf[]
//把所有的location{}中的配置通过lq->queue连接在一起，最终头部为父级server{}中的loc_conf配置里
ngx_int_t
ngx_http_add_location(ngx_conf_t *cf, ngx_queue_t **locations,
    ngx_http_core_loc_conf_t *clcf) //和ngx_http_init_locations配合使用
{
    ngx_http_location_queue_t  *lq;

    if (*locations == NULL) {
        *locations = ngx_palloc(cf->temp_pool,
                                sizeof(ngx_http_location_queue_t));
        if (*locations == NULL) {
            return NGX_ERROR;
        }

        ngx_queue_init(*locations);//父级就是该父级下所有loc的头部，头部是ngx_queue_t，next开始的成员为ngx_http_location_queue_t
    }

    lq = ngx_palloc(cf->temp_pool, sizeof(ngx_http_location_queue_t));
    if (lq == NULL) {
        return NGX_ERROR;
    }

    if (clcf->exact_match
#if (NGX_PCRE)
        || clcf->regex
#endif
        || clcf->named || clcf->noname)
    {
        lq->exact = clcf;
        lq->inclusive = NULL;

    } else { //前缀匹配
        lq->exact = NULL;
        lq->inclusive = clcf; //location ^~  
    }

    lq->name = &clcf->name;
    lq->file_name = cf->conf_file->file.name.data;
    lq->line = cf->conf_file->line;

    ngx_queue_init(&lq->list);

    ngx_queue_insert_tail(*locations, &lq->queue); //添加到父级的loc_conf[]指针中

    return NGX_OK;
}


/*
= 开头表示精确匹配
^~ 开头表示uri以某个常规字符串开头，理解为匹配 url路径即可。nginx不对url做编码，因此请求为/static/20%/aa，可以被规则^~ /static/ /aa匹配到（注意是空格）。
~ 开头表示区分大小写的正则匹配
~*  开头表示不区分大小写的正则匹配
!~和!~*分别为区分大小写不匹配及不区分大小写不匹配 的正则
/ 通用匹配，任何请求都会匹配到。


location匹配命令

~      #波浪线表示执行一个正则匹配，区分大小写
~*    #表示执行一个正则匹配，不区分大小写
^~    #^~表示普通字符匹配，如果该选项匹配，只匹配该选项，不匹配别的选项，一般用来匹配目录
=      #进行普通字符精确匹配
@     #"@" 定义一个命名的 location，使用在内部定向时，例如 error_page, try_files



location 匹配的优先级(与location在配置文件中的顺序无关)
= 精确匹配会第一个被处理。如果发现精确匹配，nginx停止搜索其他匹配。
普通字符匹配，正则表达式规则和长的块规则将被优先和查询匹配，也就是说如果该项匹配还需去看有没有正则表达式匹配和更长的匹配。
^~ 则只匹配该规则，nginx停止搜索其他匹配，否则nginx会继续处理其他location指令。
最后匹配理带有"~"和"~*"的指令，如果找到相应的匹配，则nginx停止搜索其他匹配；当没有正则表达式或者没有正则表达式被匹配的情况下，那么匹配程度最高的逐字匹配指令会被使用。

location 优先级官方文档

1.Directives with the = prefix that match the query exactly. If found, searching stops.
2.All remaining directives with conventional strings, longest match first. If this match used the ^~ prefix, searching stops.
3.Regular expressions, in order of definition in the configuration file.
4.If #3 yielded a match, that result is used. Else the match from #2 is used.
1.=前缀的指令严格匹配这个查询。如果找到，停止搜索。
2.所有剩下的常规字符串，最长的匹配。如果这个匹配使用^?前缀，搜索停止。
3.正则表达式，在配置文件中定义的顺序。
4.如果第3条规则产生匹配的话，结果被使用。否则，如同从第2条规则被使用。


例如

location  = / {
# 只匹配"/".
[ configuration A ] 
}
location  / {
# 匹配任何请求，因为所有请求都是以"/"开始
# 但是更长字符匹配或者正则表达式匹配会优先匹配
[ configuration B ] 
}
location ^~ /images/ {
# 匹配任何以 /images/ 开始的请求，并停止匹配 其它location
[ configuration C ] 
}
location ~* \.(gif|jpg|jpeg)$ {
# 匹配以 gif, jpg, or jpeg结尾的请求. 
# 但是所有 /images/ 目录的请求将由 [Configuration C]处理.   
[ configuration D ] 
}请求URI例子:

?/ -> 符合configuration A
?/documents/document.html -> 符合configuration B
?/images/1.gif -> 符合configuration C
?/documents/1.jpg ->符合 configuration D
@location 例子
error_page 404 = @fetch;

location @fetch(
proxy_pass http://fetch;
)
*/
/*
比较函数ngx_http_cmp_locations的算法原则是：
1 首先是如果比较的额两个节点中插入的是未命名的，那么把该节点加入到后面，如果比较的两个节点都是未命名的，那么保持原定次序。
2 如果插入的两个节点中，插入的是命名的location，那么把该节点加入到后面，如果比较的两个节点都是命名的，那么比较location名称，按照字母序进行排序。
3 如果两个比较节点中，插入的是正则location，那么就把插入即诶的那加入到后面，如果比较的两个节点都是正则，那么就按照原定次序，即保持用户在配置文件里书序的先后顺序。
//所以插入的降序是未命名、命名、正则、前缀匹配|绝对匹配。
*/  
static ngx_int_t
ngx_http_cmp_locations(const ngx_queue_t *one, const ngx_queue_t *two)
{
    ngx_int_t                   rc;
    ngx_http_core_loc_conf_t   *first, *second;
    ngx_http_location_queue_t  *lq1, *lq2;

    lq1 = (ngx_http_location_queue_t *) one;
    lq2 = (ngx_http_location_queue_t *) two;

    first = lq1->exact ? lq1->exact : lq1->inclusive;
    second = lq2->exact ? lq2->exact : lq2->inclusive;

    if (first->noname && !second->noname) {
        /* shift no named locations to the end */
        return 1;
    }

    if (!first->noname && second->noname) {
        /* shift no named locations to the end */
        return -1;
    }

    if (first->noname || second->noname) {
        /* do not sort no named locations */
        return 0;
    }

    if (first->named && !second->named) {
        /* shift named locations to the end */
        return 1;
    }

    if (!first->named && second->named) {
        /* shift named locations to the end */
        return -1;
    }

    if (first->named && second->named) {
        return ngx_strcmp(first->name.data, second->name.data);
    }

#if (NGX_PCRE)

    if (first->regex && !second->regex) {
        /* shift the regex matches to the end */
        return 1;
    }

    if (!first->regex && second->regex) {
        /* shift the regex matches to the end */
        return -1;
    }

    if (first->regex || second->regex) {
        /* do not sort the regex matches */
        return 0;
    }

#endif

    rc = ngx_filename_cmp(first->name.data, second->name.data,
                          ngx_min(first->name.len, second->name.len) + 1); //前缀匹配

    if (rc == 0 && !first->exact_match && second->exact_match) { //完全匹配
        /* an exact match must be before the same inclusive one */
        return 1;
    }

    return rc;
}

//很好的图解，参考http://blog.csdn.net/fengmo_q/article/details/6683377
static ngx_int_t
ngx_http_join_exact_locations(ngx_conf_t *cf, ngx_queue_t *locations)
{
    ngx_queue_t                *q, *x;
    ngx_http_location_queue_t  *lq, *lx;

    q = ngx_queue_head(locations);

    while (q != ngx_queue_last(locations)) {

        x = ngx_queue_next(q);

        lq = (ngx_http_location_queue_t *) q;
        lx = (ngx_http_location_queue_t *) x;

        if (lq->name->len == lx->name->len
            && ngx_filename_cmp(lq->name->data, lx->name->data, lx->name->len)
               == 0)
        {
            if ((lq->exact && lx->exact) || (lq->inclusive && lx->inclusive)) {
                ngx_log_error(NGX_LOG_EMERG, cf->log, 0,
                              "duplicate location \"%V\" in %s:%ui",
                              lx->name, lx->file_name, lx->line);

                return NGX_ERROR;
            }

            lq->inclusive = lx->inclusive;

            ngx_queue_remove(x);

            continue;
        }

        q = ngx_queue_next(q);
    }

    return NGX_OK;
}

/*
接下来调用ngx_http_create_locations_list，这个函数的作用是这样的：

在开始的这个locationqueue中，有一些是有相同路径前缀的，自然他们在排序的时候也是会挨在一起的，ngx_http_create_locations_list会将跟某个location有相同前缀的location，从原来的queue上取下，挂在该location的list成员下，这样在主queue上的location都是具有相互不同name的location了。举个例子：

主链：/a  -> /ab  ->  /abc ->  /b  -> /bc  ->  /bcd ->  /c ->  /cd ->  /cde

  //横向通过ngx_http_location_queue_t->queue连接  纵向通过list连接
  处理之后：/a  -> /b  ->  /c      

            |       |        |

          /ab       /bc      /cd

            |       |        |

         /abc      /bcd       /cde
*/ //递归locations队列中的每个节点，得到以当前节点的名字为前缀的location，并保存在当前节点的list字段 下
static void //图形化参考http://blog.chinaunix.net/uid-27767798-id-3759557.html，这个博客很好理解
ngx_http_create_locations_list(ngx_queue_t *locations, ngx_queue_t *q) //图形化理解参考http://blog.csdn.net/fengmo_q/article/details/6683377
{
    u_char                     *name;
    size_t                      len;
    ngx_queue_t                *x, tail;
    ngx_http_location_queue_t  *lq, *lx;

    //如果location为空就没有必要继续走下面的流程了，尤其是递归到嵌套location
    if (q == ngx_queue_last(locations)) {
        return;
    }

    lq = (ngx_http_location_queue_t *) q;

    if (lq->inclusive == NULL) {
        //如果这个节点是精准匹配那么这个节点，就不会作为某些节点的前缀，不用拥有tree节点
        ngx_http_create_locations_list(locations, ngx_queue_next(q));
        return;
    }

    len = lq->name->len;
    name = lq->name->data;

    for (x = ngx_queue_next(q);
         x != ngx_queue_sentinel(locations);
         x = ngx_queue_next(x))
    {
        /* 
            由于所有location已经按照顺序排列好，递归q节点的后继节点，如果后继节点的长度小于后缀节点的长度，那么可以断定，这个后
            继节点肯定和后缀节点不一样，并且不可能有共同的后缀；如果后继节点和q节点的交集做比较，如果不同，就表示不是同一个前缀，所以
            可以看出，从q节点的location list应该是从q.next到x.prev节点
          */
        lx = (ngx_http_location_queue_t *) x;

        if (len > lx->name->len
            || ngx_filename_cmp(name, lx->name->data, len) != 0)
        {
            break;
        }
    }

    q = ngx_queue_next(q);

    if (q == x) {//如果q和x节点直接没有节点，那么就没有必要递归后面了产生q节点的location list，直接递归q的后继节点x，产生x节点location list 
        ngx_http_create_locations_list(locations, x);
        return;
    }

    ngx_queue_split(locations, q, &tail);//location从q节点开始分割，那么现在location就是q节点之前的一段list
    ngx_queue_add(&lq->list, &tail);//q节点的list初始为从q节点开始到最后的一段list

    
    /*
        原则上因为需要递归两段list，一个为p的location list（从p.next到x.prev），另一段为x.next到location的最后一个元素，这里如果x
        已经是location的最后一个了，那么就没有必要递归x.next到location的这一段了，因为这一段都是空的。
    */
    if (x == ngx_queue_sentinel(locations)) {
        ngx_http_create_locations_list(&lq->list, ngx_queue_head(&lq->list));
        return;
    }
    
    //到了这里可以知道需要递归两段location list了
    ngx_queue_split(&lq->list, x, &tail);//再次分割，lq->list剩下p.next到x.prev的一段了
    ngx_queue_add(locations, &tail);// 放到location 中去
    ngx_http_create_locations_list(&lq->list, ngx_queue_head(&lq->list));//递归p.next到x.prev

    ngx_http_create_locations_list(locations, x);//递归x.next到location 最后了
}


/*
 * to keep cache locality for left leaf nodes, allocate nodes in following
 * order: node, left subtree, right subtree, inclusive subtree
 */
/*    主要说下大概的过程吧，像这种算法的分析，讲起来会很