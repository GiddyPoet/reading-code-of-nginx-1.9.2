
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>


static ngx_int_t ngx_http_stub_status_handler(ngx_http_request_t *r);
static ngx_int_t ngx_http_stub_status_variable(ngx_http_request_t *r,
    ngx_http_variable_value_t *v, uintptr_t data);
static ngx_int_t ngx_http_stub_status_add_variables(ngx_conf_t *cf);
static char *ngx_http_set_stub_status(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);


static ngx_command_t  ngx_http_status_commands[] = {

    { ngx_string("stub_status"),
      NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS|NGX_CONF_TAKE1,
      ngx_http_set_stub_status,
      0,
      0,
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_stub_status_module_ctx = {
    ngx_http_stub_status_add_variables,    /* preconfiguration */
    NULL,                                  /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    NULL,                                  /* create location configuration */
    NULL                                   /* merge location configuration */
};

/* 
璇ユā