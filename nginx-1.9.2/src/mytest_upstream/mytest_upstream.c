#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


typedef struct
{
    ngx_http_status_t           status;
    ngx_str_t					backendServer;
} ngx_http_mytest_upstream_ctx_t;

typedef struct
{
    ngx_http_upstream_conf_t upstream;
} ngx_http_mytest_upstream_conf_t;


static char *
ngx_http_mytest_upstream(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_mytest_upstream_handler(ngx_http_request_t *r);
static void* ngx_http_mytest_upstream_create_loc_conf(ngx_conf_t *cf);
static char *ngx_http_mytest_upstream_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

static ngx_int_t
mytest_upstream_process_header(ngx_http_request_t *r);
static ngx_int_t
mytest_process_status_line(ngx_http_request_t *r);


static ngx_str_t  ngx_http_proxy_hide_headers[] =
{
    ngx_string("Date"),
    ngx_string("Server"),
    ngx_string("X-Pad"),
    ngx_string("X-Accel-Expires"),
    ngx_string("X-Accel-Redirect"),
    ngx_string("X-Accel-Limit-Rate"),
    ngx_string("X-Accel-Buffering"),
    ngx_string("X-Accel-Charset"),
    ngx_null_string
};

static ngx_command_t  ngx_http_mytest_upstream_commands[] =
{

    {
        ngx_string("mytest_upstream"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        ngx_http_mytest_upstream,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t  ngx_http_mytest_upstream_module_ctx =
{
    NULL,                              /* preconfiguration */
    NULL,                  		/* postconfiguration */

    NULL,                              /* create main configuration */
    NULL,                              /* init main configuration */

    NULL,                              /* create server configuration */
    NULL,                              /* merge server configuration */

    ngx_http_mytest_upstream_create_loc_conf,       			/* create location configuration */
    ngx_http_mytest_upstream_merge_loc_conf         			/* merge location configuration */
};

ngx_module_t  ngx_http_mytest_upstream_module =
{
    NGX_MODULE_V1,
    &ngx_http_mytest_upstream_module_ctx,     /* module context */
    ngx_http_mytest_upstream_commands,        /* module directives */
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


static void* ngx_http_mytest_upstream_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_mytest_upstream_conf_t  *mycf;

    mycf = (ngx_http_mytest_upstream_conf_t  *)ngx_pcalloc(cf->pool, sizeof(ngx_http_mytest_upstream_conf_t));
    if (mycf == NULL)
    {
        return NULL;
    }

    //浠ヤ