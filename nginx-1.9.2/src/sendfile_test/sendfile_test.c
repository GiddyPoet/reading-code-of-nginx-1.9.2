#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static char *
ngx_http_sendifle_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_sendifle_mytest_handler(ngx_http_request_t *r);



static ngx_command_t  sendfile_test_commands[] =
{

    {
        ngx_string("mytest_sendfile"),
        NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_HTTP_LMT_CONF | NGX_CONF_NOARGS,
        ngx_http_sendifle_mytest,
        NGX_HTTP_LOC_CONF_OFFSET,
        0,
        NULL
    },

    ngx_null_command
};

static ngx_http_module_t  sendfile_test_ctx =
{
    NULL,                              /* preconfiguration */
    NULL,                  		/* postconfiguration */

    NULL,                              /* create main configuration */
    NULL,                              /* init main configuration */

    NULL,                              /* create server configuration */
    NULL,                              /* merge server configuration */

    NULL,       			/* create location configuration */
    NULL         			/* merge location configuration */
};

ngx_module_t  sendfile_test =
{
    NGX_MODULE_V1,
    &sendfile_test_ctx,           /* module context */
    sendfile_test_commands,              /* module directives */
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


static char *
ngx_http_sendifle_mytest(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t  *clcf;

    //棣