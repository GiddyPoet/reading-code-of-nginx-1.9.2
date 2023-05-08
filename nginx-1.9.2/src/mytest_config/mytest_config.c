#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
// --add-module=./src/mytest_config --add-module=./src/my_test_module --add-module=./src/mytest_subrequest --add-module=./src/mytest_upstream --add-module=./src/ngx_http_myfilter_module --with-debug --with-file-aio --add-module=./src/sendfile_test
static void*
ngx_http_mytest_config_create_loc_conf(ngx_conf_t *cf);

static char*
ngx_conf_set_myconfig(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static char *
ngx_http_mytest_config_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child);

ngx_int_t
ngx_http_mytest_config_post_conf(ngx_conf_t *cf);

/* 姣