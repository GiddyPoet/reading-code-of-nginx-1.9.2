
/*
 * Copyright (C) Roman Arutyunyan
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_stream.h>


typedef void (*ngx_stream_proxy_handler_pt)(ngx_stream_session_t *s);


typedef struct {
    ngx_msec_t                       connect_timeout;
    ngx_msec_t                       timeout;
    ngx_msec_t                       next_upstream_timeout;
    size_t                           downstream_buf_size;
    size_t                           upstream_buf_size;
    ngx_uint_t                       next_upstream_tries;
    ngx_flag_t                       next_upstream;
    ngx_flag_t                       proxy_protocol;
    ngx_addr_t                      *local;

#if (NGX_STREAM_SSL)
    ngx_flag_t                       ssl_enable;
    ngx_flag_t                       ssl_session_reuse;
    ngx_uint_t                       ssl_protocols;
    ngx_str_t                        ssl_ciphers;
    ngx_str_t                        ssl_name;
    ngx_flag_t                       ssl_server_name;

    ngx_flag_t                       ssl_verify;
    ngx_uint_t                       ssl_verify_depth;
    ngx_str_t                        ssl_trusted_certificate;
    ngx_str_t                        ssl_crl;
    ngx_str_t                        ssl_certificate;
    ngx_str_t                        ssl_certificate_key;
    ngx_array_t                     *ssl_passwords;

    ngx_ssl_t                       *ssl;
#endif

    ngx_stream_upstream_srv_conf_t  *upstream;
} ngx_stream_proxy_srv_conf_t;


static void ngx_stream_proxy_handler(ngx_stream_session_t *s);
static void ngx_stream_proxy_connect(ngx_stream_session_t *s);
static void ngx_stream_proxy_init_upstream(ngx_stream_session_t *s);
static void ngx_stream_proxy_upstream_handler(ngx_event_t *ev);
static void ngx_stream_proxy_downstream_handler(ngx_event_t *ev);
static void ngx_stream_proxy_connect_handler(ngx_event_t *ev);
static ngx_int_t ngx_stream_proxy_test_connect(ngx_connection_t *c);
static ngx_int_t ngx_stream_proxy_process(ngx_stream_session_t *s,
    ngx_uint_t from_upstream, ngx_uint_t do_write);
static void ngx_stream_proxy_next_upstream(ngx_stream_session_t *s);
static void ngx_stream_proxy_finalize(ngx_stream_session_t *s, ngx_int_t rc);
static u_char *ngx_stream_proxy_log_error(ngx_log_t *log, u_char *buf,
    size_t len);

static void *ngx_stream_proxy_create_srv_conf(ngx_conf_t *cf);
static char *ngx_stream_proxy_merge_srv_conf(ngx_conf_t *cf, void *parent,
    void *child);
static char *ngx_stream_proxy_pass(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static char *ngx_stream_proxy_bind(ngx_conf_t *cf, ngx_command_t *cmd,
    void *conf);
static ngx_int_t ngx_stream_proxy_send_proxy_protocol(ngx_stream_session_t *s);

#if (NGX_STREAM_SSL)

static char *ngx_stream_proxy_ssl_password_file(ngx_conf_t *cf,
    ngx_command_t *cmd, void *conf);
static void ngx_stream_proxy_ssl_init_connection(ngx_stream_session_t *s);
static void ngx_stream_proxy_ssl_handshake(ngx_connection_t *pc);
static ngx_int_t ngx_stream_proxy_ssl_name(ngx_stream_session_t *s);
static ngx_int_t ngx_stream_proxy_set_ssl(ngx_conf_t *cf,
    ngx_stream_proxy_srv_conf_t *pscf);


static ngx_conf_bitmask_t  ngx_stream_proxy_ssl_protocols[] = {
    { ngx_string("SSLv2"), NGX_SSL_SSLv2 },
    { ngx_string("SSLv3"), NGX_SSL_SSLv3 },
    { ngx_string("TLSv1"), NGX_SSL_TLSv1 },
    { ngx_string("TLSv1.1"), NGX_SSL_TLSv1_1 },
    { ngx_string("TLSv1.2"), NGX_SSL_TLSv1_2 },
    { ngx_null_string, 0 }
};

#endif


static ngx_command_t  ngx_stream_proxy_commands[] = {
/*
proxy_pass

璇